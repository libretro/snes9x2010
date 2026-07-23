/* hdpack.c: HD texture pack engine. See hdpack.h for the format and
 * the architecture summary. All code is MSVC C89.
 *
 * Image sheets may be PNG, WebP or DDS; they are decoded through
 * libretro-common's image_transfer interface, which dispatches on the
 * image type derived here from the file extension. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define hd_mkdir(p) _mkdir(p)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define hd_mkdir(p) mkdir((p), 0755)
#endif

#include "snes9x.h"
#include "memmap.h"
#include "ppu.h"
#include "tile.h"
#include "hdpack.h"

#include <boolean.h>
#include <formats/image.h>
#include <formats/rpng.h>

#include <encodings/crc32.h>

/* ------------------------------------------------------------------ */
/* limits                                                              */
/* ------------------------------------------------------------------ */

#define HD_MAX_SCALE        4
#define HD_MAX_IMAGES       64
#define HD_MAX_WIDTH        512
#define HD_MAX_HEIGHT       478
#define HD_TILE_BYTES       64   /* decoded 8x8, one index byte per pixel */
#define HD_DESC_MAX         8192 /* unique tile+palette combos per frame  */
#define HD_DESC_HASH_SIZE   16384/* power of two, > HD_DESC_MAX           */
#define HD_PACK_HASH_MIN    256

static uint32_t hd_crc32(const void *data, size_t len)
{
   /* encoding_crc32 is the standard CRC-32 (poly 0xEDB88320, init/final
    * 0xFFFFFFFF) - verified bit-identical to the zlib crc32 this
    * previously called, so pack tile hashes are unchanged. */
   return encoding_crc32(0, (const uint8_t *)data, len);
}

/* ------------------------------------------------------------------ */
/* pack data                                                           */
/* ------------------------------------------------------------------ */

typedef struct hd_image
{
   uint32_t *argb;   /* w*h ARGB8888 */
   unsigned  w;
   unsigned  h;
} hd_image;

typedef struct hd_entry
{
   uint8_t  data[HD_TILE_BYTES];
   uint32_t data_crc;
   uint32_t pal_crc;       /* 0xFFFFFFFF == default (any palette) */
   uint16_t img;
   uint16_t x;
   uint16_t y;
   struct hd_entry *next;  /* hash chain */
} hd_entry;

typedef struct hd_pack
{
   int       loaded;
   uint32_t  scale;
   unsigned  n_images;
   hd_image  images[HD_MAX_IMAGES];
   hd_entry **hash;        /* chained hash on data_crc ^ pal_crc      */
   hd_entry **hash_def;    /* chained hash on data_crc, default tiles */
   unsigned  hash_size;    /* shared by both tables (power of two)    */
   unsigned  n_entries;
} hd_pack;

static hd_pack hd;

/* ------------------------------------------------------------------ */
/* per-frame recording                                                 */
/* ------------------------------------------------------------------ */

typedef struct hd_desc
{
   uint8_t   data[HD_TILE_BYTES]; /* unflipped decoded tile            */
   uint32_t  data_crc;
   uint32_t  pal_crc;
   uint16_t  pal_colors[256];     /* recorder only: palette snapshot   */
   uint16_t  n_colors;
   uint8_t   bpp;
   /* composite-time resolution cache */
   const hd_entry *entry;
   int8_t    resolved;
   /* dedup key */
   const uint8_t  *key_cache;
   const uint16_t *key_pal;
} hd_desc;

static hd_desc  *hd_descs;         /* [HD_DESC_MAX] */
static uint32_t  hd_desc_count;
static int32_t  *hd_desc_hash;     /* [HD_DESC_HASH_SIZE], -1 empty */

/* Per native pixel: 0 = none, else ((desc_id + 1) << 6) | suby<<3 | subx,
 * with subx/suby in unflipped tile space. */
static uint32_t *hd_info;          /* [HD_MAX_WIDTH * HD_MAX_HEIGHT] */

static uint16_t *hd_out;           /* composite output buffer */

static int hd_recording;           /* pack loaded or recorder active */
static int hd_record_to_disk;      /* SNES_HD_RECORD */
static char hd_pack_dir[1024];

/* saved real renderers */
static void (*hd_real_tile_nomath) (uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_tile_math)   (uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_clip_nomath) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_clip_math)   (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_mosaic_nomath) (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_mosaic_math)   (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static void (*hd_real_m7bg1_nomath)  (uint32_t, uint32_t, int);
static void (*hd_real_m7bg1_math)    (uint32_t, uint32_t, int);
static void (*hd_real_m7bg2_nomath)  (uint32_t, uint32_t, int);
static void (*hd_real_m7bg2_math)    (uint32_t, uint32_t, int);
static void (*hd_real_backdrop_nomath) (uint32_t, uint32_t, uint32_t);
static void (*hd_real_backdrop_math)   (uint32_t, uint32_t, uint32_t);
static uint8_t *hd_db_snapshot;   /* [HD_MAX_WIDTH * HD_MAX_HEIGHT] */

/* ------------------------------------------------------------------ */
/* helpers                                                             */
/* ------------------------------------------------------------------ */

static int hd_hex_nibble(char c)
{
   if (c >= '0' && c <= '9') return c - '0';
   if (c >= 'a' && c <= 'f') return c - 'a' + 10;
   if (c >= 'A' && c <= 'F') return c - 'A' + 10;
   return -1;
}

static int hd_parse_hex(const char *s, uint8_t *out, unsigned bytes)
{
   unsigned i;
   for (i = 0; i < bytes; i++)
   {
      int hi = hd_hex_nibble(s[i * 2]);
      int lo = hd_hex_nibble(s[i * 2 + 1]);
      if (hi < 0 || lo < 0)
         return 0;
      out[i] = (uint8_t)((hi << 4) | lo);
   }
   return 1;
}

static unsigned hd_colors_for_bpp(unsigned bpp)
{
   if (bpp == 2) return 4;
   if (bpp == 8) return 256;
   return 16;
}

/* ------------------------------------------------------------------ */
/* image loading (PNG / WebP / DDS through image_transfer)             */
/* ------------------------------------------------------------------ */

static enum image_type_enum hd_image_type_from_name(const char *name)
{
   const char *dot = strrchr(name, '.');
   if (!dot)
      return IMAGE_TYPE_NONE;
   if (!strcmp(dot, ".png")  || !strcmp(dot, ".PNG"))
      return IMAGE_TYPE_PNG;
   if (!strcmp(dot, ".webp") || !strcmp(dot, ".WEBP"))
      return IMAGE_TYPE_WEBP;
   if (!strcmp(dot, ".dds")  || !strcmp(dot, ".DDS"))
      return IMAGE_TYPE_DDS;
   return IMAGE_TYPE_NONE;
}

static int hd_load_image(const char *path, hd_image *img)
{
   FILE     *f;
   long      sz;
   void     *buf;
   void     *handle;
   int       retval;
   uint32_t *pixels = NULL;
   unsigned  w = 0, h = 0;
   enum image_type_enum type = hd_image_type_from_name(path);

   if (type == IMAGE_TYPE_NONE)
   {
      fprintf(stderr, "[hdpack] unsupported image extension: %s\n", path);
      return 0;
   }

   f = fopen(path, "rb");
   if (!f)
      return 0;
   fseek(f, 0, SEEK_END);
   sz = ftell(f);
   fseek(f, 0, SEEK_SET);
   if (sz <= 0)
   {
      fclose(f);
      return 0;
   }
   buf = malloc((size_t)sz);
   if (!buf)
   {
      fclose(f);
      return 0;
   }
   if (fread(buf, 1, (size_t)sz, f) != (size_t)sz)
   {
      free(buf);
      fclose(f);
      return 0;
   }
   fclose(f);

   handle = image_transfer_new(type);
   if (!handle)
   {
      free(buf);
      return 0;
   }

   image_transfer_set_buffer_ptr(handle, type, buf, (size_t)sz);

   if (!image_transfer_start(handle, type))
   {
      image_transfer_free(handle, type);
      free(buf);
      return 0;
   }

   while (image_transfer_iterate(handle, type)) { }

   if (!image_transfer_is_valid(handle, type))
   {
      image_transfer_free(handle, type);
      free(buf);
      return 0;
   }

   do
   {
      retval = image_transfer_process(handle, type,
            &pixels, (size_t)sz, &w, &h, false);
   } while (retval == IMAGE_PROCESS_NEXT);

   image_transfer_free(handle, type);
   free(buf);

   if (retval != IMAGE_PROCESS_END || !pixels || !w || !h)
   {
      if (pixels)
         free(pixels);
      return 0;
   }

   img->argb = pixels;
   img->w    = w;
   img->h    = h;
   return 1;
}

/* ------------------------------------------------------------------ */
/* pack hash                                                           */
/* ------------------------------------------------------------------ */

static void hd_pack_insert(hd_entry *e)
{
   unsigned slot;
   if (e->pal_crc == 0xFFFFFFFFu)
   {
      slot = e->data_crc & (hd.hash_size - 1);
      e->next = hd.hash_def[slot];
      hd.hash_def[slot] = e;
   }
   else
   {
      slot = (e->data_crc ^ e->pal_crc) & (hd.hash_size - 1);
      e->next = hd.hash[slot];
      hd.hash[slot] = e;
   }
}

static const hd_entry *hd_pack_find(const uint8_t *data, uint32_t data_crc,
      uint32_t pal_crc)
{
   const hd_entry *e;
   unsigned slot = (data_crc ^ pal_crc) & (hd.hash_size - 1);
   for (e = hd.hash[slot]; e; e = e->next)
   {
      if (e->data_crc == data_crc && e->pal_crc == pal_crc &&
            !memcmp(e->data, data, HD_TILE_BYTES))
         return e;
   }
   slot = data_crc & (hd.hash_size - 1);
   for (e = hd.hash_def[slot]; e; e = e->next)
   {
      if (e->data_crc == data_crc &&
            !memcmp(e->data, data, HD_TILE_BYTES))
         return e;
   }
   return NULL;
}

/* ------------------------------------------------------------------ */
/* hires.txt loading                                                   */
/* ------------------------------------------------------------------ */

static char *hd_next_token(char **cursor)
{
   char *s = *cursor;
   char *tok;
   if (!s)
      return NULL;
   tok = s;
   while (*s && *s != ',')
      s++;
   if (*s == ',')
   {
      *s = '\0';
      *cursor = s + 1;
   }
   else
      *cursor = NULL;
   return tok;
}

static int hd_load_pack(const char *dir)
{
   char  path[1200];
   char  line[600];
   FILE *f;
   unsigned est;

   sprintf(path, "%s/hires.txt", dir);
   f = fopen(path, "rb");
   if (!f)
      return 0;

   /* size the pack hash from the file size (one tile per ~160 bytes) */
   fseek(f, 0, SEEK_END);
   est = (unsigned)(ftell(f) / 64) + HD_PACK_HASH_MIN;
   fseek(f, 0, SEEK_SET);
   hd.hash_size = HD_PACK_HASH_MIN;
   while (hd.hash_size < est)
      hd.hash_size <<= 1;
   hd.hash     = (hd_entry **)calloc(hd.hash_size, sizeof(hd_entry *));
   hd.hash_def = (hd_entry **)calloc(hd.hash_size, sizeof(hd_entry *));
   if (!hd.hash || !hd.hash_def)
   {
      fclose(f);
      return 0;
   }

   hd.scale    = 1;
   hd.n_images = 0;

   while (fgets(line, sizeof(line), f))
   {
      size_t n = strlen(line);
      while (n && (line[n - 1] == '\n' || line[n - 1] == '\r'))
         line[--n] = '\0';

      if (!strncmp(line, "<ver>", 5))
      {
         if (atoi(line + 5) > 1)
         {
            fclose(f);
            fprintf(stderr, "[hdpack] hires.txt version too new\n");
            return 0;
         }
      }
      else if (!strncmp(line, "<scale>", 7))
      {
         int s = atoi(line + 7);
         if (s < 1 || s > HD_MAX_SCALE)
         {
            fclose(f);
            fprintf(stderr, "[hdpack] unsupported <scale>%d\n", s);
            return 0;
         }
         hd.scale = (uint32_t)s;
      }
      else if (!strncmp(line, "<img>", 5))
      {
         if (hd.n_images >= HD_MAX_IMAGES)
            continue;
         /* bound the concatenation: a hostile hires.txt must not be
          * able to overflow the path buffer */
         if (strlen(dir) + 1 + strlen(line + 5) + 1 > sizeof(path))
         {
            fclose(f);
            fprintf(stderr, "[hdpack] image path too long\n");
            return 0;
         }
         sprintf(path, "%s/%s", dir, line + 5);
         if (!hd_load_image(path, &hd.images[hd.n_images]))
         {
            fclose(f);
            fprintf(stderr, "[hdpack] failed to load %s\n", path);
            return 0;
         }
         hd.n_images++;
      }
      else if (!strncmp(line, "<tile>", 6))
      {
         char *cur = line + 6;
         char *t_img  = hd_next_token(&cur);
         char *t_bpp  = hd_next_token(&cur);
         char *t_data = hd_next_token(&cur);
         char *t_pal  = hd_next_token(&cur);
         char *t_x    = hd_next_token(&cur);
         char *t_y    = hd_next_token(&cur);
         hd_entry *e;
         unsigned img;

         if (!t_img || !t_bpp || !t_data || !t_pal || !t_x || !t_y)
            continue;
         if (strlen(t_data) != HD_TILE_BYTES * 2)
            continue;
         img = (unsigned)atoi(t_img);
         if (img >= hd.n_images)
            continue;

         e = (hd_entry *)calloc(1, sizeof(hd_entry));
         if (!e)
            continue;
         if (!hd_parse_hex(t_data, e->data, HD_TILE_BYTES))
         {
            free(e);
            continue;
         }
         e->data_crc = hd_crc32(e->data, HD_TILE_BYTES);
         if (!strcmp(t_pal, "default"))
            e->pal_crc = 0xFFFFFFFFu;
         else
         {
            uint8_t crcb[4];
            /* hd_parse_hex reads exactly 8 characters; reject short
             * tokens rather than reading past their terminator */
            if (strlen(t_pal) != 8 || !hd_parse_hex(t_pal, crcb, 4))
            {
               free(e);
               continue;
            }
            e->pal_crc = ((uint32_t)crcb[0] << 24) | ((uint32_t)crcb[1] << 16) |
                         ((uint32_t)crcb[2] << 8)  |  (uint32_t)crcb[3];
         }
         e->img = (uint16_t)img;
         e->x   = (uint16_t)atoi(t_x);
         e->y   = (uint16_t)atoi(t_y);
         hd_pack_insert(e);
         hd.n_entries++;
      }
   }
   fclose(f);

   if (!hd.n_entries || !hd.n_images)
      return 0;
   hd.loaded = 1;
   fprintf(stderr, "[hdpack] loaded: %u tiles, %u images, scale %u\n",
         hd.n_entries, hd.n_images, hd.scale);
   return 1;
}

/* ------------------------------------------------------------------ */
/* per-frame tile descriptors                                          */
/* ------------------------------------------------------------------ */

static uint32_t hd_cur_word;   /* (id+1)<<6 for the tile being drawn */
static int      hd_cur_hflip;
static int      hd_cur_vflip;

/* Register (or find) the descriptor for the tile just drawn. Mirrors
 * GET_CACHED_TILE()/SELECT_PALETTE() read-only. Returns 0 when
 * recording cannot proceed (table full / blank tile). */
static int hd_tile_begin(uint32_t Tile)
{
   uint32_t  TileNumber, TileAddr;
   uint8_t  *cache;
   const uint16_t *pal;
   unsigned  ncolors, bpp;
   uint32_t  h;
   uint32_t  slot;
   hd_desc  *d;

   TileAddr = BG.TileAddress + ((Tile & 0x3ff) << BG.TileShift);
   if (Tile & 0x100)
      TileAddr += BG.NameSelect;
   TileAddr  &= 0xffff;
   TileNumber = TileAddr >> BG.TileShift;

   /* identity is the unflipped decode */
   cache = &BG.Buffer[TileNumber << 6];
   if (!BG.Buffered[TileNumber])
      BG.Buffered[TileNumber] = BG.ConvertTile(cache, TileAddr, Tile & 0x3ff);
   if (BG.Buffered[TileNumber] == BLANK_TILE)
      return 0;

   /* Direct-colour tiles have no CGRAM palette to key on; leave them
    * to the native fallback. */
   if (BG.DirectColourMode)
      return 0;
   /* Key on the raw CGRAM colors, not IPPU.ScreenColors: ScreenColors
    * are pre-multiplied by the master brightness, so keying on them
    * would mint a new tile identity for every fade step. Brightness is
    * applied to the HD sample at composite time instead. */
   pal = &PPU.CGDATA[((Tile >> BG.PaletteShift) & BG.PaletteMask) + BG.StartPalette];

   switch (BG.TileShift)
   {
      case 4:  bpp = 2; break;
      case 6:  bpp = 8; break;
      default: bpp = 4; break;
   }
   ncolors = hd_colors_for_bpp(bpp);

   /* Dedup on (cache pointer, palette pointer) plus CONTENT. The
    * pointer pair alone is not a stable identity within a frame:
    * games rewrite CGRAM mid-frame (raster palette effects) and DMA
    * new tile data over the same VRAM between draws, in which case
    * the same pointers carry different content and must map to a new
    * descriptor, or later draws would inherit the earlier tiles'
    * artwork. */
   {
      uint32_t pal_crc = hd_crc32(pal, ncolors * sizeof(uint16_t));

      h    = (uint32_t)((size_t)cache >> 6) * 2654435761u ^
             (uint32_t)((size_t)pal   >> 1) * 40503u ^ pal_crc;
      slot = h & (HD_DESC_HASH_SIZE - 1);
      for (;;)
      {
         int32_t id = hd_desc_hash[slot];
         if (id < 0)
            break;
         d = &hd_descs[id];
         if (d->key_cache == cache && d->key_pal == pal &&
               d->pal_crc == pal_crc &&
               !memcmp(d->data, cache, HD_TILE_BYTES))
         {
            hd_cur_word = ((uint32_t)id + 1) << 6;
            return 1;
         }
         slot = (slot + 1) & (HD_DESC_HASH_SIZE - 1);
      }

      if (hd_desc_count >= HD_DESC_MAX)
         return 0;

      d = &hd_descs[hd_desc_count];
      memcpy(d->data, cache, HD_TILE_BYTES);
      d->data_crc  = hd_crc32(d->data, HD_TILE_BYTES);
      d->pal_crc   = pal_crc;
   }
   d = &hd_descs[hd_desc_count];
   d->n_colors  = (uint16_t)ncolors;
   d->bpp       = (uint8_t)bpp;
   d->entry     = NULL;
   d->resolved  = 0;
   d->key_cache = cache;
   d->key_pal   = pal;
   if (hd_record_to_disk)
      memcpy(d->pal_colors, pal, ncolors * sizeof(uint16_t));

   hd_desc_hash[slot] = (int32_t)hd_desc_count;
   hd_cur_word = (hd_desc_count + 1) << 6;
   hd_desc_count++;
   return 1;
}

/* is GFX.S currently pointing into the main screen? */
static INLINE int hd_main_screen_offset(int32_t *base)
{
   ptrdiff_t off = GFX.S - GFX.Screen;
   if (off < 0 || off >= (ptrdiff_t)(HD_MAX_WIDTH * HD_MAX_HEIGHT))
      return 0;
   *base = (int32_t)off;
   return 1;
}

/* Depth-diff recording common to the two wrapper flavors: any pixel
 * whose depth byte changed across the wrapped call was written by
 * this tile (writes always raise the depth strictly). */
static void hd_record_span(uint32_t Tile, uint32_t Offset,
      uint32_t first, uint32_t last,
      uint32_t StartLine, uint32_t LineCount,
      void (*call_tile)(uint32_t, uint32_t, uint32_t, uint32_t),
      void (*call_clip)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
      uint32_t StartPixel, uint32_t Width)
{
   uint8_t  before[8][8];
   int32_t  sbase;
   uint32_t r, n;
   uint32_t rows = LineCount > 8 ? 8 : LineCount;
   int      is_main = hd_main_screen_offset(&sbase);

   if (is_main)
   {
      for (r = 0; r < rows; r++)
         memcpy(before[r], GFX.DB + Offset + r * GFX.PPL + first,
               last - first + 1);
   }

   if (call_tile)
      call_tile(Tile, Offset, StartLine, LineCount);
   else
      call_clip(Tile, Offset, StartPixel, Width, StartLine, LineCount);

   if (!is_main)
      return;


   /* A tile that cannot be given an identity (direct-colour mode,
    * descriptor table full) still wrote pixels; those must drop any
    * previous attribution or the compositor would draw the underlying
    * tile's art over them. */
   if (!hd_tile_begin(Tile))
      hd_cur_word = 0;

   hd_cur_hflip = (Tile & H_FLIP) ? 1 : 0;
   hd_cur_vflip = (Tile & V_FLIP) ? 1 : 0;

   for (r = 0; r < rows; r++)
   {
      const uint8_t *db = GFX.DB + Offset + r * GFX.PPL + first;
      uint32_t       info_row = (uint32_t)sbase + Offset + r * GFX.PPL;
      uint32_t       suby = (StartLine >> 3) + r;
      if (hd_cur_vflip)
         suby = 7 - suby;
      for (n = 0; n <= last - first; n++)
      {
         if (db[n] != before[r][n])
         {
            uint32_t scrx = first + n;
            uint32_t subx = hd_cur_hflip ? (7 - scrx) : scrx;
            hd_info[info_row + scrx] = hd_cur_word ?
                  (hd_cur_word | (suby << 3) | subx) : 0;
         }
      }
   }
}

/* the four wrappers */
static void hd_wrap_tile_nomath(uint32_t Tile, uint32_t Offset,
      uint32_t StartLine, uint32_t LineCount)
{
   hd_record_span(Tile, Offset, 0, 7, StartLine, LineCount,
         hd_real_tile_nomath, NULL, 0, 0);
}

/* Clear-only depth diff: pixels written by renderers whose output a
 * flat texture cannot reproduce (color math, mosaic, Mode 7) drop any
 * earlier tile attribution so the compositor passes the native pixels
 * through. A stale attribution here would make the compositor replace
 * a blended/rotated pixel with the underlying tile's texture. */
/* Tiles drawn through the color-math renderers are not recorded:
 * their on-screen pixels are add/sub blends with the subscreen, which
 * a flat texture replacement cannot reproduce. Any pixel they write
 * must also drop a previously recorded attribution, so run them under
 * the depth diff and clear the info words for changed pixels. */
static void hd_clear_diff_rows(uint32_t Offset, uint32_t first, uint32_t count,
      uint32_t rows, const uint8_t before[8][256])
{
   int32_t  sbase;
   uint32_t r, n;
   if (!hd_main_screen_offset(&sbase))
      return;
   for (r = 0; r < rows; r++)
   {
      const uint8_t *db = GFX.DB + Offset + r * GFX.PPL + first;
      uint32_t info_row = (uint32_t)sbase + Offset + r * GFX.PPL + first;
      for (n = 0; n < count; n++)
      {
         if (db[n] != before[r][n])
            hd_info[info_row + n] = 0;
      }
   }
}

static void hd_wrap_tile_math(uint32_t Tile, uint32_t Offset,
      uint32_t StartLine, uint32_t LineCount)
{
   uint8_t  before[8][256];
   int32_t  sbase;
   uint32_t r;
   uint32_t rows = LineCount > 8 ? 8 : LineCount;
   int      is_main = hd_main_screen_offset(&sbase);
   if (is_main)
      for (r = 0; r < rows; r++)
         memcpy(before[r], GFX.DB + Offset + r * GFX.PPL, 8);
   hd_real_tile_math(Tile, Offset, StartLine, LineCount);
   if (is_main)
      hd_clear_diff_rows(Offset, 0, 8, rows, (const uint8_t (*)[256])before);
}

static void hd_wrap_clip_nomath(uint32_t Tile, uint32_t Offset,
      uint32_t StartPixel, uint32_t Width, uint32_t StartLine, uint32_t LineCount)
{
   uint32_t endpix = StartPixel + Width;
   if (StartPixel > 7 || !Width)
   {
      hd_real_clip_nomath(Tile, Offset, StartPixel, Width, StartLine, LineCount);
      return;
   }
   if (endpix > 8)
      endpix = 8;
   hd_record_span(Tile, Offset, StartPixel, endpix - 1, StartLine, LineCount,
         NULL, hd_real_clip_nomath, StartPixel, Width);
}

static void hd_wrap_clip_math(uint32_t Tile, uint32_t Offset,
      uint32_t StartPixel, uint32_t Width, uint32_t StartLine, uint32_t LineCount)
{
   uint8_t  before[8][256];
   int32_t  sbase;
   uint32_t r;
   uint32_t rows   = LineCount > 8 ? 8 : LineCount;
   uint32_t endpix = StartPixel + Width;
   int      is_main;
   if (StartPixel > 7)
      endpix = StartPixel;
   else if (endpix > 8)
      endpix = 8;
   is_main = hd_main_screen_offset(&sbase);
   if (is_main && endpix > StartPixel)
      for (r = 0; r < rows; r++)
         memcpy(before[r], GFX.DB + Offset + r * GFX.PPL + StartPixel,
               endpix - StartPixel);
   hd_real_clip_math(Tile, Offset, StartPixel, Width, StartLine, LineCount);
   if (is_main && endpix > StartPixel)
      hd_clear_diff_rows(Offset, StartPixel, endpix - StartPixel, rows,
            (const uint8_t (*)[256])before);
}

/* mosaic pixels: same clear-only treatment (both math flavors) */
static void hd_wrap_mosaic_common(uint32_t Tile, uint32_t Offset,
      uint32_t StartPixel, uint32_t Width, uint32_t StartLine, uint32_t LineCount,
      void (*real_fn)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))
{
   uint8_t  before[8][256];
   int32_t  sbase;
   uint32_t r;
   uint32_t rows = LineCount > 8 ? 8 : LineCount;
   uint32_t count = Width > 256 ? 256 : Width;
   int      is_main = hd_main_screen_offset(&sbase);
   if (is_main && count)
      for (r = 0; r < rows; r++)
         memcpy(before[r], GFX.DB + Offset + r * GFX.PPL + StartPixel, count);
   real_fn(Tile, Offset, StartPixel, Width, StartLine, LineCount);
   if (is_main && count)
      hd_clear_diff_rows(Offset, StartPixel, count, rows,
            (const uint8_t (*)[256])before);
}

static void hd_wrap_mosaic_nomath(uint32_t Tile, uint32_t Offset,
      uint32_t StartPixel, uint32_t Width, uint32_t StartLine, uint32_t LineCount)
{
   hd_wrap_mosaic_common(Tile, Offset, StartPixel, Width, StartLine, LineCount,
         hd_real_mosaic_nomath);
}

static void hd_wrap_mosaic_math(uint32_t Tile, uint32_t Offset,
      uint32_t StartPixel, uint32_t Width, uint32_t StartLine, uint32_t LineCount)
{
   hd_wrap_mosaic_common(Tile, Offset, StartPixel, Width, StartLine, LineCount,
         hd_real_mosaic_math);
}

/* Mode 7 spans: clear-only depth diff over [Left, Right). A single
 * call covers every line in GFX.StartY..GFX.EndY - the renderers
 * iterate Offset = GFX.StartY * GFX.PPL through GFX.EndY internally
 * (see DrawMode7BG1_Normal1x1 in tile.c), exactly like the backdrop
 * fill, so the snapshot must span the full line range. */
static void hd_wrap_m7_common(uint32_t Left, uint32_t Right, int D,
      void (*real_fn)(uint32_t, uint32_t, int))
{
   int32_t  sbase;
   uint32_t y, n, count, lines, Offset;
   int      is_main = hd_main_screen_offset(&sbase);
   count  = (Right > Left) ? (Right - Left) : 0;
   lines  = (GFX.EndY >= GFX.StartY) ? (GFX.EndY - GFX.StartY + 1) : 0;
   Offset = GFX.StartY * GFX.PPL;
   if (is_main && count && lines && hd_db_snapshot)
   {
      for (y = 0; y < lines; y++)
         memcpy(hd_db_snapshot + y * count,
               GFX.DB + Offset + y * GFX.PPL + Left, count);
   }
   real_fn(Left, Right, D);
   if (is_main && count && lines && hd_db_snapshot)
   {
      for (y = 0; y < lines; y++)
      {
         const uint8_t *db  = GFX.DB + Offset + y * GFX.PPL + Left;
         const uint8_t *was = hd_db_snapshot + y * count;
         uint32_t info_row  = (uint32_t)sbase + Offset + y * GFX.PPL + Left;
         for (n = 0; n < count; n++)
         {
            if (db[n] != was[n])
               hd_info[info_row + n] = 0;
         }
      }
   }
}

/* The backdrop fill runs AFTER the tile passes (see DrawBackdrop() in
 * ppu.c's S9xUpdateScreen) and paints every pixel its depth test
 * clears - including over lower-priority already-attributed tiles.
 * Clear the attribution for every pixel it changes. It covers
 * GFX.StartY..GFX.EndY internally, so the snapshot spans multiple
 * lines and uses a persistent buffer. */
static void hd_wrap_backdrop_common(uint32_t Offset, uint32_t Left, uint32_t Right,
      void (*real_fn)(uint32_t, uint32_t, uint32_t))
{
   int32_t  sbase;
   uint32_t y, n, count, lines;
   int      is_main = hd_main_screen_offset(&sbase);
   count = (Right > Left) ? (Right - Left) : 0;
   lines = (GFX.EndY >= GFX.StartY) ? (GFX.EndY - GFX.StartY + 1) : 0;
   if (is_main && count && lines && hd_db_snapshot)
   {
      for (y = 0; y < lines; y++)
         memcpy(hd_db_snapshot + y * count,
               GFX.DB + Offset + y * GFX.PPL + Left, count);
   }
   real_fn(Offset, Left, Right);
   if (is_main && count && lines && hd_db_snapshot)
   {
      for (y = 0; y < lines; y++)
      {
         const uint8_t *db  = GFX.DB + Offset + y * GFX.PPL + Left;
         const uint8_t *was = hd_db_snapshot + y * count;
         uint32_t info_row  = (uint32_t)sbase + Offset + y * GFX.PPL + Left;
         for (n = 0; n < count; n++)
         {
            if (db[n] != was[n])
               hd_info[info_row + n] = 0;
         }
      }
   }
}

static void hd_wrap_backdrop_nomath(uint32_t Offset, uint32_t Left, uint32_t Right)
{ hd_wrap_backdrop_common(Offset, Left, Right, hd_real_backdrop_nomath); }
static void hd_wrap_backdrop_math(uint32_t Offset, uint32_t Left, uint32_t Right)
{ hd_wrap_backdrop_common(Offset, Left, Right, hd_real_backdrop_math); }

static void hd_wrap_m7bg1_nomath(uint32_t Left, uint32_t Right, int D)
{ hd_wrap_m7_common(Left, Right, D, hd_real_m7bg1_nomath); }
static void hd_wrap_m7bg1_math(uint32_t Left, uint32_t Right, int D)
{ hd_wrap_m7_common(Left, Right, D, hd_real_m7bg1_math); }
static void hd_wrap_m7bg2_nomath(uint32_t Left, uint32_t Right, int D)
{ hd_wrap_m7_common(Left, Right, D, hd_real_m7bg2_nomath); }
static void hd_wrap_m7bg2_math(uint32_t Left, uint32_t Right, int D)
{ hd_wrap_m7_common(Left, Right, D, hd_real_m7bg2_math); }

void S9xHdPackWrapRenderers (int normal1x1)
{
   if (!hd_recording || !normal1x1)
      return;
   /* never wrap twice */
   if (GFX.DrawTileNomath != hd_wrap_tile_nomath)
   {
      hd_real_tile_nomath = GFX.DrawTileNomath;
      GFX.DrawTileNomath  = hd_wrap_tile_nomath;
   }
   if (GFX.DrawTileMath != hd_wrap_tile_math)
   {
      hd_real_tile_math = GFX.DrawTileMath;
      GFX.DrawTileMath  = hd_wrap_tile_math;
   }
   if (GFX.DrawClippedTileNomath != hd_wrap_clip_nomath)
   {
      hd_real_clip_nomath       = GFX.DrawClippedTileNomath;
      GFX.DrawClippedTileNomath = hd_wrap_clip_nomath;
   }
   if (GFX.DrawClippedTileMath != hd_wrap_clip_math)
   {
      hd_real_clip_math       = GFX.DrawClippedTileMath;
      GFX.DrawClippedTileMath = hd_wrap_clip_math;
   }
   if (GFX.DrawMosaicPixelNomath != hd_wrap_mosaic_nomath)
   {
      hd_real_mosaic_nomath     = GFX.DrawMosaicPixelNomath;
      GFX.DrawMosaicPixelNomath = hd_wrap_mosaic_nomath;
   }
   if (GFX.DrawMosaicPixelMath != hd_wrap_mosaic_math)
   {
      hd_real_mosaic_math     = GFX.DrawMosaicPixelMath;
      GFX.DrawMosaicPixelMath = hd_wrap_mosaic_math;
   }
   if (GFX.DrawMode7BG1Nomath != hd_wrap_m7bg1_nomath)
   {
      hd_real_m7bg1_nomath  = GFX.DrawMode7BG1Nomath;
      GFX.DrawMode7BG1Nomath = hd_wrap_m7bg1_nomath;
   }
   if (GFX.DrawMode7BG1Math != hd_wrap_m7bg1_math)
   {
      hd_real_m7bg1_math  = GFX.DrawMode7BG1Math;
      GFX.DrawMode7BG1Math = hd_wrap_m7bg1_math;
   }
   if (GFX.DrawMode7BG2Nomath != hd_wrap_m7bg2_nomath)
   {
      hd_real_m7bg2_nomath  = GFX.DrawMode7BG2Nomath;
      GFX.DrawMode7BG2Nomath = hd_wrap_m7bg2_nomath;
   }
   if (GFX.DrawMode7BG2Math != hd_wrap_m7bg2_math)
   {
      hd_real_m7bg2_math  = GFX.DrawMode7BG2Math;
      GFX.DrawMode7BG2Math = hd_wrap_m7bg2_math;
   }
   if (GFX.DrawBackdropNomath != hd_wrap_backdrop_nomath)
   {
      hd_real_backdrop_nomath = GFX.DrawBackdropNomath;
      GFX.DrawBackdropNomath  = hd_wrap_backdrop_nomath;
   }
   if (GFX.DrawBackdropMath != hd_wrap_backdrop_math)
   {
      hd_real_backdrop_math = GFX.DrawBackdropMath;
      GFX.DrawBackdropMath  = hd_wrap_backdrop_math;
   }
}

/* ------------------------------------------------------------------ */
/* compositor                                                          */
/* ------------------------------------------------------------------ */

static INLINE uint16_t hd_argb_to_565(uint32_t c)
{
   return (uint16_t)(((c >> 8) & 0xf800) | ((c >> 5) & 0x07e0) | ((c >> 3) & 0x001f));
}

/* alpha-blend HD argb over a native 565 pixel, applying the current
 * master brightness to the HD sample (the native pixel already has it) */
static uint16_t hd_blend_565(uint32_t argb, uint16_t under)
{
   uint32_t a  = argb >> 24;
   uint32_t br = PPU.Brightness;
   uint32_t r, g, b, ur, ug, ub;
   if (a == 0)
      return under;
   if (br < 15)
   {
      r = (((argb >> 16) & 0xff) * br) / 15;
      g = (((argb >> 8)  & 0xff) * br) / 15;
      b = (( argb        & 0xff) * br) / 15;
      argb = (argb & 0xff000000u) | (r << 16) | (g << 8) | b;
   }
   if (a >= 255)
      return hd_argb_to_565(argb);
   /* Expand the native 565 under-pixel to full 8-bit range with bit
    * replication. A bare left-shift tops out at 248/252/248 instead of
    * 255, which darkens the base image everywhere an HD texture is
    * alpha-blended over it. */
   ur = (under >> 11) & 0x1f; ur = (ur << 3) | (ur >> 2);
   ug = (under >> 5)  & 0x3f; ug = (ug << 2) | (ug >> 4);
   ub =  under        & 0x1f; ub = (ub << 3) | (ub >> 2);
   r = ((argb >> 16) & 0xff) * a + ur * (255 - a);
   g = ((argb >> 8)  & 0xff) * a + ug * (255 - a);
   b = ( argb        & 0xff) * a + ub * (255 - a);
   r /= 255; g /= 255; b /= 255;
   return (uint16_t)(((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));
}

uint16_t *S9xHdPackComposite (int width, int height,
      int *out_width, int *out_height, int *out_pitch)
{
   uint32_t scale = hd.scale;
   int      x, y;
   unsigned dx, dy;
   int      src_ppl = (int)(GFX.Pitch >> 1);
   int      out_w, out_h;

   if (!hd.loaded || width > 256 || height > HD_MAX_HEIGHT)
      return NULL;


   out_w = width  * (int)scale;
   out_h = height * (int)scale;

   for (y = 0; y < height; y++)
   {
      const uint16_t *src  = GFX.Screen + y * src_ppl;
      const uint32_t *info = hd_info + y * src_ppl;
      for (x = 0; x < width; x++)
      {
         uint32_t iw = info[x];
         const hd_entry *e = NULL;
         uint16_t native = src[x];

         if (iw)
         {
            hd_desc *d = &hd_descs[(iw >> 6) - 1];
            if (!d->resolved)
            {
               d->entry = hd_pack_find(d->data, d->data_crc, d->pal_crc);
               d->resolved = 1;
            }
            e = d->entry;
         }

         if (e)
         {
            const hd_image *im  = &hd.images[e->img];
            uint32_t subx = iw & 7;
            uint32_t suby = (iw >> 3) & 7;
            unsigned bx   = e->x + subx * scale;
            unsigned by   = e->y + suby * scale;
            for (dy = 0; dy < scale; dy++)
            {
               uint16_t *dst = hd_out + ((unsigned)y * scale + dy) * (unsigned)out_w
                             + (unsigned)x * scale;
               if (by + dy < im->h && bx + scale <= im->w)
               {
                  const uint32_t *hp = im->argb + (by + dy) * im->w + bx;
                  for (dx = 0; dx < scale; dx++)
                     dst[dx] = hd_blend_565(hp[dx], native);
               }
               else
               {
                  for (dx = 0; dx < scale; dx++)
                     dst[dx] = native;
               }
            }
         }
         else
         {
            for (dy = 0; dy < scale; dy++)
            {
               uint16_t *dst = hd_out + ((unsigned)y * scale + dy) * (unsigned)out_w
                             + (unsigned)x * scale;
               for (dx = 0; dx < scale; dx++)
                  dst[dx] = native;
            }
         }
      }
   }

   *out_width  = out_w;
   *out_height = out_h;
   *out_pitch  = out_w * 2;
   return hd_out;
}

/* ------------------------------------------------------------------ */
/* recorder                                                            */
/* ------------------------------------------------------------------ */

/* Every unique (tile, palette) seen across the whole session gets one
 * cell in a growing sheet. Flushed to PNG + hires.txt on deinit. */

#define HD_REC_MAX      16384
#define HD_REC_PER_ROW  16

typedef struct hd_rec_tile
{
   uint8_t  data[HD_TILE_BYTES];
   uint32_t data_crc;
   uint32_t pal_crc;
   uint16_t pal_colors[256];
   uint16_t n_colors;
   uint8_t  bpp;
} hd_rec_tile;

static hd_rec_tile *hd_rec;
static unsigned     hd_rec_count;
static uint32_t    *hd_rec_hash;   /* [HD_DESC_HASH_SIZE*2] slot -> index+1 */

static void hd_record_frame_tiles(void)
{
   uint32_t i;
   for (i = 0; i < hd_desc_count; i++)
   {
      hd_desc *d = &hd_descs[i];
      uint32_t h    = d->data_crc ^ (d->pal_crc * 2654435761u);
      uint32_t mask = HD_DESC_HASH_SIZE * 2 - 1;
      uint32_t slot = h & mask;
      int found = 0;
      for (;;)
      {
         uint32_t v = hd_rec_hash[slot];
         if (!v)
            break;
         if (hd_rec[v - 1].data_crc == d->data_crc &&
               hd_rec[v - 1].pal_crc == d->pal_crc &&
               !memcmp(hd_rec[v - 1].data, d->data, HD_TILE_BYTES))
         {
            found = 1;
            break;
         }
         slot = (slot + 1) & mask;
      }
      if (!found && hd_rec_count < HD_REC_MAX)
      {
         hd_rec_tile *t = &hd_rec[hd_rec_count];
         memcpy(t->data, d->data, HD_TILE_BYTES);
         memcpy(t->pal_colors, d->pal_colors, d->n_colors * sizeof(uint16_t));
         t->data_crc = d->data_crc;
         t->pal_crc  = d->pal_crc;
         t->n_colors = d->n_colors;
         t->bpp      = d->bpp;
         hd_rec_hash[slot] = ++hd_rec_count;
      }
   }
}

static void hd_recorder_flush(void)
{
   unsigned rows, w, h, i, px, py;
   uint32_t *sheet;
   char path[1200];
   FILE *f;

   if (!hd_rec_count)
      return;

   rows = (hd_rec_count + HD_REC_PER_ROW - 1) / HD_REC_PER_ROW;
   w    = HD_REC_PER_ROW * 8;
   h    = rows * 8;
   sheet = (uint32_t *)calloc((size_t)w * h, sizeof(uint32_t));
   if (!sheet)
      return;

   for (i = 0; i < hd_rec_count; i++)
   {
      const hd_rec_tile *t = &hd_rec[i];
      unsigned bx = (i % HD_REC_PER_ROW) * 8;
      unsigned by = (i / HD_REC_PER_ROW) * 8;
      for (py = 0; py < 8; py++)
      {
         for (px = 0; px < 8; px++)
         {
            uint8_t  idx  = t->data[py * 8 + px];
            uint32_t argb = 0;
            if (idx)
            {
               /* CGRAM 15-bit 0BBBBBGGGGGRRRRR */
               uint16_t c = t->pal_colors[idx & (t->n_colors - 1)];
               argb = 0xff000000u |
                      (((uint32_t)(c)       & 0x1f) << 19) |
                      (((uint32_t)(c >> 5)  & 0x1f) << 11) |
                      (((uint32_t)(c >> 10) & 0x1f) << 3);
            }
            sheet[(by + py) * w + bx + px] = argb;
         }
      }
   }

   sprintf(path, "%s/tiles.png", hd_pack_dir);
   if (!rpng_save_image_argb(path, sheet, w, h, w * 4))
      fprintf(stderr, "[hdpack] recorder: failed to write %s\n", path);
   free(sheet);

   sprintf(path, "%s/hires.txt", hd_pack_dir);
   f = fopen(path, "wb");
   if (!f)
      return;
   fprintf(f, "<ver>1\n<scale>1\n<img>tiles.png\n");
   for (i = 0; i < hd_rec_count; i++)
   {
      const hd_rec_tile *t = &hd_rec[i];
      unsigned k;
      fprintf(f, "<tile>0,%u,", (unsigned)t->bpp);
      for (k = 0; k < HD_TILE_BYTES; k++)
         fprintf(f, "%02x", t->data[k]);
      fprintf(f, ",%08x,%u,%u\n", t->pal_crc,
            (i % HD_REC_PER_ROW) * 8, (i / HD_REC_PER_ROW) * 8);
   }
   fclose(f);
   fprintf(stderr, "[hdpack] recorder: wrote %u tiles to %s\n",
         hd_rec_count, hd_pack_dir);
}

/* ------------------------------------------------------------------ */
/* lifecycle                                                           */
/* ------------------------------------------------------------------ */

int S9xHdPackActive (void)
{
   return hd.loaded;
}

int S9xHdPackRecording (void)
{
   return hd_recording;
}

uint32_t S9xHdPackScale (void)
{
   return hd.loaded ? hd.scale : 1;
}

void S9xHdPackFrameBegin (void)
{
   if (!hd_recording)
      return;
   if (hd_record_to_disk)
      hd_record_frame_tiles();
   hd_desc_count = 0;
   memset(hd_desc_hash, 0xff, HD_DESC_HASH_SIZE * sizeof(int32_t));
   memset(hd_info, 0, HD_MAX_WIDTH * HD_MAX_HEIGHT * sizeof(uint32_t));
}

int S9xHdPackInit (const char *rom_path)
{
   const char *base;
   const char *dot;
   size_t      dirlen, stemlen;

   S9xHdPackDeinit();

   if (!rom_path || !rom_path[0])
      return 0;

   /* "<dir>/<stem>.hdpack" */
   base = strrchr(rom_path, '/');
#ifdef _WIN32
   {
      const char *b2 = strrchr(rom_path, '\\');
      if (b2 && (!base || b2 > base))
         base = b2;
   }
#endif
   base   = base ? base + 1 : rom_path;
   dirlen = (size_t)(base - rom_path);
   dot    = strrchr(base, '.');
   stemlen = dot ? (size_t)(dot - base) : strlen(base);
   if (dirlen + stemlen + 16 >= sizeof(hd_pack_dir))
      return 0;
   memcpy(hd_pack_dir, rom_path, dirlen + stemlen);
   strcpy(hd_pack_dir + dirlen + stemlen, ".hdpack");

   hd_record_to_disk = 0;
   {
      const char *rec = getenv("SNES_HD_RECORD");
      if (rec && rec[0] == '1')
         hd_record_to_disk = 1;
   }

   if (!hd_record_to_disk)
   {
      if (!hd_load_pack(hd_pack_dir))
      {
         S9xHdPackDeinit();
         return 0;
      }
   }
   else
   {
      hd_rec      = (hd_rec_tile *)calloc(HD_REC_MAX, sizeof(hd_rec_tile));
      hd_rec_hash = (uint32_t *)calloc(HD_DESC_HASH_SIZE * 2, sizeof(uint32_t));
      if (!hd_rec || !hd_rec_hash)
      {
         S9xHdPackDeinit();
         return 0;
      }
      hd_mkdir(hd_pack_dir);
      fprintf(stderr, "[hdpack] recording to %s\n", hd_pack_dir);
   }

   hd_descs     = (hd_desc *)malloc(HD_DESC_MAX * sizeof(hd_desc));
   hd_desc_hash = (int32_t *)malloc(HD_DESC_HASH_SIZE * sizeof(int32_t));
   hd_info      = (uint32_t *)malloc(HD_MAX_WIDTH * HD_MAX_HEIGHT * sizeof(uint32_t));
   hd_db_snapshot = (uint8_t *)malloc(HD_MAX_WIDTH * HD_MAX_HEIGHT);
   if (hd.loaded)
      hd_out = (uint16_t *)malloc((size_t)256 * HD_MAX_SCALE *
            HD_MAX_HEIGHT * HD_MAX_SCALE * sizeof(uint16_t));
   if (!hd_descs || !hd_desc_hash || !hd_info || !hd_db_snapshot ||
         (hd.loaded && !hd_out))
   {
      S9xHdPackDeinit();
      return 0;
   }

   hd_desc_count = 0;
   memset(hd_desc_hash, 0xff, HD_DESC_HASH_SIZE * sizeof(int32_t));
   memset(hd_info, 0, HD_MAX_WIDTH * HD_MAX_HEIGHT * sizeof(uint32_t));
   hd_recording = 1;
   return 1;
}

void S9xHdPackDeinit (void)
{
   unsigned i, s;

   if (hd_record_to_disk && hd_rec)
   {
      hd_record_frame_tiles();
      hd_recorder_flush();
   }

   for (i = 0; i < hd.n_images; i++)
   {
      if (hd.images[i].argb)
         free(hd.images[i].argb);
   }
   if (hd.hash)
   {
      for (s = 0; s < hd.hash_size; s++)
      {
         hd_entry *e = hd.hash[s];
         while (e)
         {
            hd_entry *n = e->next;
            free(e);
            e = n;
         }
      }
      free(hd.hash);
   }
   if (hd.hash_def)
   {
      for (s = 0; s < hd.hash_size; s++)
      {
         hd_entry *e = hd.hash_def[s];
         while (e)
         {
            hd_entry *n = e->next;
            free(e);
            e = n;
         }
      }
      free(hd.hash_def);
   }
   memset(&hd, 0, sizeof(hd));

   if (hd_descs)     { free(hd_descs);     hd_descs = NULL; }
   if (hd_desc_hash) { free(hd_desc_hash); hd_desc_hash = NULL; }
   if (hd_info)      { free(hd_info);      hd_info = NULL; }
   if (hd_db_snapshot) { free(hd_db_snapshot); hd_db_snapshot = NULL; }
   if (hd_out)       { free(hd_out);       hd_out = NULL; }
   if (hd_rec)       { free(hd_rec);       hd_rec = NULL; }
   if (hd_rec_hash)  { free(hd_rec_hash);  hd_rec_hash = NULL; }
   hd_rec_count      = 0;
   hd_recording      = 0;
   hd_record_to_disk = 0;
}
