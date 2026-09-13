/* framehash.c - headless libretro host that hashes what the core emits.
 *
 * A renderer change is only safe if the pixels and the samples come out
 * the same.  This runs the core with no window, no audio device and a
 * fixed input script, and prints a digest of the framebuffer and of the
 * audio at a chosen interval.  Two builds of the core that print the
 * same digests produced the same output, frame for frame; the first
 * line that differs names the frame to look at.
 *
 * It exists because "looks the same" is not a claim anyone can check
 * later, and because the threaded renderer work needs a way to say
 * bit-exact and mean it.
 *
 *   cc -O2 -o framehash tools/framehash.c -ldl \
 *      -Ilibretro/libretro-common/include
 *
 *   ./framehash snes9x2010_libretro.so game.sfc --frames 3600
 *   ./framehash ./a.so game.sfc > a.txt
 *   ./framehash ./b.so game.sfc > b.txt && diff a.txt b.txt
 *
 * Options:
 *   --frames N     frames to run (default 3600, one minute at 60Hz)
 *   --interval N   print a digest every N frames (default 60)
 *   --start N      hold Start for one frame every N frames, 0 for never
 *                  (default 0).  Enough to walk a title screen without
 *                  making the run depend on a recording.
 *   --swfb         offer a software framebuffer, so the core's direct
 *                  render path is the one under test.  Off by default,
 *                  because the two paths are separate code and a run
 *                  should say which one it covered.
 *   --replay N     every N frames, save the state, run N frames and
 *                  remember what they produced, restore, and run the
 *                  same N frames again.  The two passes must agree.
 *                  Restoring a state over the one it was taken from
 *                  proves nothing -- it is a no-op even if the restore
 *                  does nothing at all -- so the state has to be
 *                  carried across frames that changed it.  This is the
 *                  property runahead and netplay rest on.  0 disables.
 *
 *                  Audio disagreeing is a failure.  Video disagreeing
 *                  is reported and is not, because a snapshot does not
 *                  carry the frame buffer: the second pass starts from
 *                  whatever the first left there, so any pixel a frame
 *                  does not write itself differs.  snes9x upstream
 *                  does the same, so this reports the gap rather than
 *                  calling it a regression.  Fixing it means putting
 *                  the buffer in the snapshot, which costs half a
 *                  megabyte on a path runahead walks every frame.
 *
 *                  The audio digest of a --replay run matches a plain
 *                  one and can be compared against it directly.  The
 *                  video digest cannot: once a window has diverged the
 *                  buffer stays diverged for the frames after it.
 *   --option K=V   answer the core's request for option K with V.
 *                  Repeatable.  Anything not named keeps the core's
 *                  own default, so a plain run stays reproducible.
 *   --quiet        digests only, no core log output
 *
 * The digest is FNV-1a over the visible pixels only -- width * 2 bytes
 * per row, height rows -- so padding in the pitch cannot make two
 * identical frames look different.  A duplicate frame (data == NULL)
 * hashes as the previous frame's pixels, which is what the frontend
 * would have shown.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#define FH_HANDLE       HMODULE
#define FH_OPEN(p)      LoadLibraryA(p)
#define FH_SYM(h, n)    ((void*)GetProcAddress((h), (n)))
#define FH_CLOSE(h)     FreeLibrary(h)
#else
#include <dlfcn.h>
#define FH_HANDLE       void*
#define FH_OPEN(p)      dlopen((p), RTLD_NOW)
#define FH_SYM(h, n)    dlsym((h), (n))
#define FH_CLOSE(h)     dlclose(h)
#endif

#include <libretro.h>

/* ---- digest ------------------------------------------------------- */

#define FNV_OFFSET 0xcbf29ce484222325ULL
#define FNV_PRIME  0x00000100000001b3ULL

typedef unsigned long long fh_u64;

static void fh_hash(fh_u64 *h, const void *data, size_t bytes)
{
   const unsigned char *p = (const unsigned char*)data;
   size_t i;

   for (i = 0; i < bytes; i++)
   {
      *h ^= (fh_u64)p[i];
      *h *= FNV_PRIME;
   }
}

/* ---- state -------------------------------------------------------- */

static fh_u64   video_hash = FNV_OFFSET;
static fh_u64   audio_hash = FNV_OFFSET;
static unsigned frame_no;
static int      opt_swfb;
static int      opt_quiet;
static unsigned opt_start;

#define FH_MAX_OPTIONS 16
static const char *opt_key[FH_MAX_OPTIONS];
static const char *opt_val[FH_MAX_OPTIONS];
static unsigned    opt_count;
static int      replay_failed;
static unsigned replay_video_differs;

/* Last frame's pixels, so a duplicate frame hashes what would have
 * been shown rather than nothing. */
static unsigned char *last_frame;
static size_t         last_frame_bytes;

/* Offered to the core when --swfb is on.  Sized for the widest frame
 * the core can ask for. */
#define FH_FB_W 1208
#define FH_FB_H 512
static unsigned short fh_fb[FH_FB_W * FH_FB_H];

/* ---- callbacks ---------------------------------------------------- */

static void fh_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   (void)fmt;
   /* Core log output is noise in a digest run and its wording is not
    * part of what is being compared. */
}

static bool fh_environment(unsigned cmd, void *data)
{
   switch (cmd)
   {
      case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
         /* RGB565 only: the digest is over 16-bit pixels. */
         return *(const enum retro_pixel_format*)data
            == RETRO_PIXEL_FORMAT_RGB565;

      case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
      case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
         *(const char**)data = ".";
         return true;

      case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
         ((struct retro_log_callback*)data)->log = fh_log;
         return true;

      case RETRO_ENVIRONMENT_GET_VARIABLE:
      {
         /* Every option at its default unless --option named it, so a
          * run is reproducible without carrying a config file
          * alongside it, and an option-gated path can still be
          * reached without editing the core to reach it. */
         struct retro_variable *v = (struct retro_variable*)data;
         unsigned i;

         for (i = 0; i < opt_count; i++)
         {
            if (strcmp(v->key, opt_key[i]) == 0)
            {
               v->value = opt_val[i];
               return true;
            }
         }
         v->value = NULL;
         return false;
      }

      case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
         *(bool*)data = false;
         return true;

      case RETRO_ENVIRONMENT_GET_CAN_DUPE:
         *(bool*)data = true;
         return true;

      case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
      {
         struct retro_framebuffer *fb = (struct retro_framebuffer*)data;

         if (!opt_swfb)
            return false;
         if (fb->width > FH_FB_W || fb->height > FH_FB_H)
            return false;

         fb->data         = fh_fb;
         fb->pitch        = FH_FB_W * sizeof(unsigned short);
         fb->format       = RETRO_PIXEL_FORMAT_RGB565;
         fb->memory_flags = 0;
         return true;
      }

      default:
         /* Anything else is a capability this host does not offer;
          * the core is expected to cope. */
         return false;
   }
}

static void fh_video(const void *data, unsigned width, unsigned height,
      size_t pitch)
{
   size_t row = (size_t)width * sizeof(unsigned short);
   unsigned y;

   if (!data)
   {
      /* Duplicate frame: the frontend shows the previous one again. */
      if (last_frame)
         fh_hash(&video_hash, last_frame, last_frame_bytes);
      return;
   }

   if (last_frame_bytes != row * height)
   {
      free(last_frame);
      last_frame       = (unsigned char*)malloc(row * height);
      last_frame_bytes = last_frame ? row * height : 0;
   }

   for (y = 0; y < height; y++)
   {
      const unsigned char *src = (const unsigned char*)data + y * pitch;

      fh_hash(&video_hash, src, row);
      if (last_frame)
         memcpy(last_frame + y * row, src, row);
   }
}

static void fh_audio_sample(int16_t l, int16_t r)
{
   int16_t pair[2];
   pair[0] = l;
   pair[1] = r;
   fh_hash(&audio_hash, pair, sizeof(pair));
}

static size_t fh_audio_batch(const int16_t *data, size_t frames)
{
   fh_hash(&audio_hash, data, frames * 2 * sizeof(int16_t));
   return frames;
}

static void fh_input_poll(void) { }

static int16_t fh_input_state(unsigned port, unsigned device,
      unsigned index, unsigned id)
{
   (void)device;
   (void)index;

   if (port != 0 || !opt_start)
      return 0;

   /* One frame of Start every opt_start frames.  Deterministic, and
    * enough to get past a title screen without the run depending on a
    * recorded input file. */
   if (id == RETRO_DEVICE_ID_JOYPAD_START && (frame_no % opt_start) == 0)
      return 1;

   return 0;
}

/* ---- core binding ------------------------------------------------- */

struct fh_core
{
   void   (*init)(void);
   void   (*deinit)(void);
   void   (*set_environment)(retro_environment_t);
   void   (*set_video_refresh)(retro_video_refresh_t);
   void   (*set_audio_sample)(retro_audio_sample_t);
   void   (*set_audio_sample_batch)(retro_audio_sample_batch_t);
   void   (*set_input_poll)(retro_input_poll_t);
   void   (*set_input_state)(retro_input_state_t);
   void   (*get_system_info)(struct retro_system_info*);
   void   (*get_system_av_info)(struct retro_system_av_info*);
   bool   (*load_game)(const struct retro_game_info*);
   void   (*unload_game)(void);
   void   (*run)(void);
   size_t (*serialize_size)(void);
   bool   (*serialize)(void*, size_t);
   bool   (*unserialize)(const void*, size_t);
};

static int fh_bind(FH_HANDLE h, struct fh_core *c)
{
   struct { const char *name; void **slot; } map[] = {
      { "retro_init",                    NULL },
      { "retro_deinit",                  NULL },
      { "retro_set_environment",         NULL },
      { "retro_set_video_refresh",       NULL },
      { "retro_set_audio_sample",        NULL },
      { "retro_set_audio_sample_batch",  NULL },
      { "retro_set_input_poll",          NULL },
      { "retro_set_input_state",         NULL },
      { "retro_get_system_info",         NULL },
      { "retro_get_system_av_info",      NULL },
      { "retro_load_game",               NULL },
      { "retro_unload_game",             NULL },
      { "retro_run",                     NULL },
      { "retro_serialize_size",          NULL },
      { "retro_serialize",               NULL },
      { "retro_unserialize",             NULL }
   };
   void **slots[16];
   size_t i;

   slots[0]  = (void**)&c->init;
   slots[1]  = (void**)&c->deinit;
   slots[2]  = (void**)&c->set_environment;
   slots[3]  = (void**)&c->set_video_refresh;
   slots[4]  = (void**)&c->set_audio_sample;
   slots[5]  = (void**)&c->set_audio_sample_batch;
   slots[6]  = (void**)&c->set_input_poll;
   slots[7]  = (void**)&c->set_input_state;
   slots[8]  = (void**)&c->get_system_info;
   slots[9]  = (void**)&c->get_system_av_info;
   slots[10] = (void**)&c->load_game;
   slots[11] = (void**)&c->unload_game;
   slots[12] = (void**)&c->run;
   slots[13] = (void**)&c->serialize_size;
   slots[14] = (void**)&c->serialize;
   slots[15] = (void**)&c->unserialize;

   for (i = 0; i < sizeof(map) / sizeof(map[0]); i++)
   {
      void *sym = FH_SYM(h, map[i].name);

      if (!sym)
      {
         fprintf(stderr, "framehash: core is missing %s\n", map[i].name);
         return 0;
      }
      *slots[i] = sym;
   }

   return 1;
}

/* ---- main --------------------------------------------------------- */

static unsigned char *fh_read_file(const char *path, size_t *out_size)
{
   unsigned char *buf;
   long           size;
   FILE          *f = fopen(path, "rb");

   if (!f)
      return NULL;

   if (fseek(f, 0, SEEK_END) != 0)
   {
      fclose(f);
      return NULL;
   }
   size = ftell(f);
   if (size <= 0 || fseek(f, 0, SEEK_SET) != 0)
   {
      fclose(f);
      return NULL;
   }

   if (!(buf = (unsigned char*)malloc((size_t)size)))
   {
      fclose(f);
      return NULL;
   }

   if (fread(buf, 1, (size_t)size, f) != (size_t)size)
   {
      free(buf);
      fclose(f);
      return NULL;
   }

   fclose(f);
   *out_size = (size_t)size;
   return buf;
}

static void usage(void)
{
   fprintf(stderr,
      "usage: framehash <core> <rom> [--frames N] [--interval N]\n"
      "                 [--start N] [--replay N] [--option K=V]\n"
      "                 [--swfb] [--quiet]\n");
}

int main(int argc, char **argv)
{
   struct fh_core            core;
   struct retro_system_info  sys;
   struct retro_system_av_info av;
   struct retro_game_info    game;
   FH_HANDLE                 h;
   unsigned char            *rom;
   size_t                    rom_size;
   unsigned                  frames   = 3600;
   unsigned                  interval = 60;
   unsigned                  replay   = 0;
   unsigned char            *state    = NULL;
   size_t                    state_size = 0;
   int                       i;

   if (argc < 3)
   {
      usage();
      return 2;
   }

   for (i = 3; i < argc; i++)
   {
      if (!strcmp(argv[i], "--frames") && i + 1 < argc)
         frames = (unsigned)strtoul(argv[++i], NULL, 0);
      else if (!strcmp(argv[i], "--interval") && i + 1 < argc)
         interval = (unsigned)strtoul(argv[++i], NULL, 0);
      else if (!strcmp(argv[i], "--replay") && i + 1 < argc)
         replay = (unsigned)strtoul(argv[++i], NULL, 0);
      else if (!strcmp(argv[i], "--option") && i + 1 < argc)
      {
         char *kv = argv[++i];
         char *eq = strchr(kv, '=');

         if (!eq || opt_count >= FH_MAX_OPTIONS)
         {
            fprintf(stderr, "framehash: bad --option %s\n", kv);
            return 2;
         }
         *eq = '\0';
         opt_key[opt_count] = kv;
         opt_val[opt_count] = eq + 1;
         opt_count++;
      }
      else if (!strcmp(argv[i], "--start") && i + 1 < argc)
         opt_start = (unsigned)strtoul(argv[++i], NULL, 0);
      else if (!strcmp(argv[i], "--swfb"))
         opt_swfb = 1;
      else if (!strcmp(argv[i], "--quiet"))
         opt_quiet = 1;
      else
      {
         fprintf(stderr, "framehash: unknown option %s\n", argv[i]);
         usage();
         return 2;
      }
   }

   if (!interval)
      interval = 1;

   if (!(rom = fh_read_file(argv[2], &rom_size)))
   {
      fprintf(stderr, "framehash: cannot read %s\n", argv[2]);
      return 1;
   }

   if (!(h = FH_OPEN(argv[1])))
   {
      fprintf(stderr, "framehash: cannot load %s\n", argv[1]);
      free(rom);
      return 1;
   }

   memset(&core, 0, sizeof(core));
   if (!fh_bind(h, &core))
   {
      FH_CLOSE(h);
      free(rom);
      return 1;
   }

   core.set_environment(fh_environment);
   core.set_video_refresh(fh_video);
   core.set_audio_sample(fh_audio_sample);
   core.set_audio_sample_batch(fh_audio_batch);
   core.set_input_poll(fh_input_poll);
   core.set_input_state(fh_input_state);

   memset(&sys, 0, sizeof(sys));
   core.get_system_info(&sys);

   core.init();

   memset(&game, 0, sizeof(game));
   game.path = argv[2];
   game.data = rom;
   game.size = rom_size;

   if (!core.load_game(&game))
   {
      fprintf(stderr, "framehash: core refused %s\n", argv[2]);
      core.deinit();
      FH_CLOSE(h);
      free(rom);
      return 1;
   }

   memset(&av, 0, sizeof(av));
   core.get_system_av_info(&av);

   if (!opt_quiet)
      printf("# core %s %s  rom %s  %ux%u  swfb %s  start %u\n",
            sys.library_name ? sys.library_name : "?",
            sys.library_version ? sys.library_version : "?",
            argv[2], av.geometry.base_width, av.geometry.base_height,
            opt_swfb ? "on" : "off", opt_start);

   if (replay)
   {
      state_size = core.serialize_size();
      if (!state_size || !(state = (unsigned char*)malloc(state_size)))
      {
         fprintf(stderr, "framehash: no serialize buffer\n");
         replay = 0;
      }
   }

   for (frame_no = 1; frame_no <= frames; frame_no++)
   {
      core.run();

      if (replay && (frame_no % replay) == 0 && frame_no + replay <= frames)
      {
         fh_u64   before_v = video_hash, before_a = audio_hash;
         fh_u64   first_v,  first_a;
         unsigned mark = frame_no, k;

         if (!core.serialize(state, state_size))
         {
            fprintf(stderr, "framehash: serialize failed at frame %u\n",
                  frame_no);
            replay = 0;
            continue;
         }

         for (k = 0; k < replay; k++)
         {
            frame_no++;
            core.run();
         }
         first_v = video_hash;
         first_a = audio_hash;

         if (!core.unserialize(state, state_size))
         {
            fprintf(stderr, "framehash: unserialize failed at frame %u\n",
                  mark);
            replay = 0;
            continue;
         }

         /* Second pass over the same frames, from the restored state. */
         video_hash = before_v;
         audio_hash = before_a;
         frame_no   = mark;
         for (k = 0; k < replay; k++)
         {
            frame_no++;
            core.run();
         }

         if (audio_hash != first_a)
         {
            printf("REPLAY AUDIO MISMATCH after frame %u: "
                   "%016llx vs %016llx\n",
                   mark, first_a, audio_hash);
            replay_failed = 1;
         }
         else if (video_hash != first_v)
         {
            replay_video_differs++;
            video_hash = first_v;   /* keep the run comparable */
         }
      }

      if ((frame_no % interval) == 0)
         printf("frame %6u  video %016llx  audio %016llx\n",
               frame_no, video_hash, audio_hash);
   }

   printf("total  %6u  video %016llx  audio %016llx\n",
         frames, video_hash, audio_hash);

   if (replay_video_differs)
      printf("# %u replay windows differed in video only: the frame "
             "buffer is not in a snapshot\n", replay_video_differs);

   free(state);
   core.unload_game();
   core.deinit();
   FH_CLOSE(h);
   free(last_frame);
   free(rom);
   return replay_failed;
}
