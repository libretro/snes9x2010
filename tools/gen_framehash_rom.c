/* gen_framehash_rom.c - writes the test ROM tools/framehash.c runs.
 *
 * framehash needs something to run, and a check that only works on
 * somebody's private ROM collection is a check that never runs in CI.
 * This emits a 128 KB LoROM image, entirely original, that boots into
 * BG mode 1 with BG1 on the main screen and then rewrites the backdrop
 * colour continuously.
 *
 * That is deliberately the shape the renderer cares about: a CGRAM
 * write mid-frame is what drives FLUSH_REDRAW, so the ROM produces a
 * steady stream of the mid-frame register writes a span-recording
 * renderer has to get right, and every frame differs from the last so
 * a digest that stops changing is itself a failure.
 *
 * 128 KB because memmap.c rejects anything reporting less than 2^7 KB.
 *
 *   cc -O2 -o gen_framehash_rom tools/gen_framehash_rom.c
 *   ./gen_framehash_rom framehash_test.sfc
 */

#include <stdio.h>
#include <string.h>

#define ROM_BYTES 0x20000

static unsigned char rom[ROM_BYTES];
static unsigned      n;

static void e(int b)
{
   rom[n++] = (unsigned char)b;
}

/* absolute operand of the instruction just emitted */
static void w(int addr)
{
   e(addr & 0xFF);
   e(addr >> 8);
}

static void sta(int addr)   /* STA abs */
{
   e(0x8D); w(addr);
}

static void stx(int addr)   /* STX abs */
{
   e(0x8E); w(addr);
}

static void lda_sta(int val, int addr)
{
   e(0xA9); e(val); e(0x8D); w(addr);
}

static void bne(unsigned target)
{
   e(0xD0);
   e((int)((target - (n + 1)) & 0xFF));
}

static void bra(unsigned target)
{
   e(0x80);
   e((int)((target - (n + 1)) & 0xFF));
}

static void patch(unsigned at, unsigned target)
{
   rom[at + 1] = (unsigned char)((target - (at + 2)) & 0xFF);
}

int main(int argc, char **argv)
{
   const char *out = (argc > 1) ? argv[1] : "framehash_test.sfc";
   const char *title = "FRAMEHASH TEST ROM   ";   /* exactly 21 bytes */
   unsigned    L, M, set, m1, m2, m4;
   unsigned    b_m1, b_m2, b_m4, b_s1, b_s2, b_s3;
   unsigned    b_st, st_skip;
   unsigned    i;
   unsigned    checksum = 0, complement;
   FILE       *f;

   memset(rom, 0, sizeof(rom));

   /* --- init --------------------------------------------------- */
   e(0x78);                          /* SEI                        */
   e(0x18); e(0xFB);                 /* CLC ; XCE  -> native       */
   e(0xC2); e(0x30);                 /* REP #$30   -> 16-bit A,X,Y */
   e(0xA2); e(0xFF); e(0x1F); e(0x1B);    /* LDX #$1FFF ; TXS      */
   e(0xE2); e(0x20);                 /* SEP #$20   -> 8-bit A      */
   e(0x9C); e(0x00); e(0x42);        /* STZ $4200  no NMI/IRQ      */
   e(0xA9); e(0x8F); sta(0x2100);      /* force blank                */

   /* VRAM: $2000 words of tile and tilemap data. */
   e(0xA9); e(0x80); sta(0x2115);      /* VMAIN: step after $2119    */
   e(0xA2); e(0x00); e(0x00); stx(0x2116);  /* VRAM address 0        */
   e(0xA0); e(0x00); e(0x20);        /* LDY #$2000                 */
   e(0xA9); e(0x01);                 /* LDA #$01                   */
   L = n;
   e(0x8D); e(0x18); e(0x21);        /* STA $2118                  */
   e(0x49); e(0xA5);                 /* EOR #$A5                   */
   e(0x8D); e(0x19); e(0x21);        /* STA $2119                  */
   e(0x49); e(0xA5);                 /* EOR #$A5                   */
   e(0x1A); e(0x88);                 /* INC A ; DEY                */
   bne(L);

   /* CGRAM: all 256 entries. */
   e(0x9C); e(0x21); e(0x21);        /* STZ $2121                  */
   e(0xA2); e(0x00); e(0x01);        /* LDX #$0100                 */
   e(0xA9); e(0x13);                 /* LDA #$13                   */
   L = n;
   e(0x8D); e(0x22); e(0x21); e(0x49); e(0x5A);
   e(0x8D); e(0x22); e(0x21); e(0x49); e(0x5A);
   e(0x1A); e(0xCA);                 /* INC A ; DEX                */
   bne(L);

   /* OAM: 128 sprites of X, Y, tile, attributes.  Priority 3 is
    * forced into the attribute byte, otherwise every sprite sits
    * under the opaque backgrounds and the OBJ renderer contributes
    * nothing to the image at all. */
   e(0x9C); e(0x02); e(0x21); e(0x9C); e(0x03); e(0x21);
   e(0xA2); e(0x80); e(0x00);        /* LDX #$0080                 */
   e(0xA9); e(0x07);                 /* LDA #$07                   */
   L = n;
   e(0x8D); e(0x04); e(0x21); e(0x1A);    /* X                     */
   e(0x8D); e(0x04); e(0x21); e(0x1A);    /* Y                     */
   e(0x8D); e(0x04); e(0x21); e(0x1A);    /* tile                  */
   e(0x48); e(0x09); e(0x30);        /* PHA ; ORA #$30             */
   e(0x8D); e(0x04); e(0x21);        /* attributes                 */
   e(0x68); e(0x1A); e(0xCA);        /* PLA ; INC A ; DEX          */
   bne(L);
   e(0xA2); e(0x20); e(0x00);        /* LDX #$0020  high table     */
   L = n;
   e(0x8D); e(0x04); e(0x21); e(0x1A); e(0xCA);
   bne(L);

   lda_sta(0x09, 0x2105);   /* BG mode 1, BG3 priority             */
   lda_sta(0x00, 0x2107);   /* BG1 tilemap $0000                   */
   lda_sta(0x04, 0x2108);   /* BG2 tilemap $0800                   */
   lda_sta(0x08, 0x2109);   /* BG3 tilemap $1000                   */
   lda_sta(0x00, 0x210B);   /* BG1/BG2 character base              */
   lda_sta(0x60, 0x2101);   /* OBJ size, name base inside the fill */
   lda_sta(0x13, 0x212C);   /* BG1, BG2, OBJ on the main screen    */
   lda_sta(0x02, 0x212D);   /* BG2 on the subscreen                */
   lda_sta(0x02, 0x2123);   /* BG1 masked by window 1 (bit 1 enables;
                             * bit 0 would only invert it)          */
   lda_sta(0x40, 0x2126);   /* window 1 left                       */
   lda_sta(0xC0, 0x2127);   /* window 1 right                      */
   lda_sta(0x01, 0x212E);   /* window masks the main screen BG1    */

   /* HDMA channel 0 rewrites window 1's left and right edge every few
    * scanlines, from the table at $00:FE00 below.  This is the shape a
    * game drives a window in: the clip registers change mid-frame, so
    * every write ends a span and the span that follows has to be drawn
    * with the window the game had set by then, not the one the frame
    * started with.  Without it the window registers are written once
    * before the first line and every span in the frame sees the same
    * value, which is the one case that cannot tell the two apart. */
   lda_sta(0x01, 0x4300);   /* B<-A, two registers, once per line  */
   lda_sta(0x26, 0x4301);   /* starting at $2126                   */
   lda_sta(0x00, 0x4302);   /* table at $00:FE00                   */
   lda_sta(0xFE, 0x4303);
   lda_sta(0x00, 0x4304);
   /* Channel 1 rewrites CGWSEL and channel 2 the two window-enable
    * registers, also per scanline. Those three decide which clip
    * regions the colour-math windows produce, so a renderer that reads
    * them at the moment it draws rather than the moment the span was
    * recorded produces different regions here and only here. */
   lda_sta(0x00, 0x4310);   /* B<-A, one register, once per line   */
   lda_sta(0x30, 0x4311);   /* $2130 CGWSEL                        */
   lda_sta(0x00, 0x4312);   /* table at $00:FD00                   */
   lda_sta(0xFD, 0x4313);
   lda_sta(0x00, 0x4314);

   lda_sta(0x01, 0x4320);   /* B<-A, two registers, once per line  */
   lda_sta(0x2E, 0x4321);   /* $212E main, $212F sub window enable */
   lda_sta(0x00, 0x4322);   /* table at $00:FC00                   */
   lda_sta(0xFC, 0x4323);
   lda_sta(0x00, 0x4324);

   lda_sta(0x07, 0x420C);   /* HDMA channels 0, 1 and 2 on         */
   lda_sta(0x03, 0x2130);   /* colour math against the subscreen, and
                             * direct colour, which is the only thing
                             * that reaches the palette-map builder    */
   lda_sta(0x23, 0x2131);   /* add, BG1 and BG2                    */

   /* Mode 7 matrix: identity, each register low byte then high. */
   e(0xA9); e(0x00); sta(0x211B); e(0xA9); e(0x01); sta(0x211B);
   e(0xA9); e(0x00); sta(0x211C); e(0xA9); e(0x00); sta(0x211C);
   e(0xA9); e(0x00); sta(0x211D); e(0xA9); e(0x00); sta(0x211D);
   e(0xA9); e(0x00); sta(0x211E); e(0xA9); e(0x01); sta(0x211E);

   lda_sta(0x31, 0x2106);   /* mosaic on BG1 only, so BG2 still
                             * exercises the plain background
                             * renderer rather than the mosaic one */
   lda_sta(0x08, 0x2133);   /* pseudo-hires: the only thing here that
                             * reaches mid-frame resolution promotion */
   lda_sta(0x0F, 0x2100);   /* visible, full brightness            */

   e(0x9C); e(0x00); e(0x00);        /* STZ $00  frame counter     */

   /* --- per-frame loop ------------------------------------------ */
   M = n;

   /* Cycle BG mode 1, 2, 4 and 7 on bits 6-5 of the counter.  Modes
    * 2 and 4 reach the offset-change renderer and mode 7 a third
    * one, none of which the plain path covers. */
   e(0xA5); e(0x00);                 /* LDA $00                    */
   e(0x29); e(0x60);                 /* AND #$60                   */
   b_m1 = n; e(0xF0); e(0x00);       /* BEQ m1                     */
   e(0xC9); e(0x20); b_m2 = n; e(0xF0); e(0x00);
   e(0xC9); e(0x40); b_m4 = n; e(0xF0); e(0x00);
   e(0xA9); e(0x07); b_s1 = n; e(0x80); e(0x00);   /* mode 7       */
   m4 = n; e(0xA9); e(0x04); b_s2 = n; e(0x80); e(0x00);
   m2 = n; e(0xA9); e(0x02); b_s3 = n; e(0x80); e(0x00);
   m1 = n; e(0xA9); e(0x09);
   set = n;
   e(0x8D); e(0x05); e(0x21);        /* STA $2105                  */
   patch(b_m1, m1); patch(b_m2, m2); patch(b_m4, m4);
   patch(b_s1, set); patch(b_s2, set); patch(b_s3, set);

   /* Toggle forced blank from the counter.  Nothing else here reaches
    * the path that blanks a span instead of drawing it. */
   e(0xA5); e(0x00); e(0x29); e(0x80);    /* LDA $00 ; AND #$80      */
   e(0x09); e(0x0F);                      /* ORA #$0F  keep brightness */
   e(0x8D); e(0x00); e(0x21);             /* STA $2100               */

   e(0xA5); e(0x00);                 /* LDA $00                    */
   e(0x8D); e(0x0D); e(0x21); e(0x8D); e(0x0D); e(0x21);  /* BG1HOFS */
   e(0x8D); e(0x0F); e(0x21); e(0x8D); e(0x0F); e(0x21);  /* BG2HOFS */

   /* Backdrop low byte comes from STAT77, so the range-over flag the
    * renderer produces is read back by the CPU and lands in a pixel.
    * Nothing else here reads a PPU status register.
    *
    * Only on one iteration in 64.  Reading a status register is a
    * barrier for a renderer that runs behind the CPU, and a loop that
    * reads one every pass is not a game, it is a benchmark of the
    * barrier. */
   e(0xA5); e(0x00);                 /* LDA $00                    */
   e(0x29); e(0x3F);                 /* AND #$3F                   */
   b_st = n; e(0xD0); e(0x00);       /* BNE skip                   */
   e(0x9C); e(0x21); e(0x21);        /* STZ $2121                  */
   e(0xAD); e(0x3E); e(0x21);        /* LDA $213E                  */
   e(0x8D); e(0x22); e(0x21);        /* backdrop low               */
   e(0xA5); e(0x00);
   e(0x8D); e(0x22); e(0x21);        /* backdrop high              */
   st_skip = n;
   patch(b_st, st_skip);
   e(0xE6); e(0x00);                 /* INC $00                    */
   bra(M);

   /* The HDMA window table at $00:FE00.  Fifteen entries of sixteen
    * lines, each giving window 1 a left and a right edge.  The edges
    * cross over half way down, so the inverted window -- left past
    * right, which selects nothing -- is drawn as well. */
   {
      unsigned char *t = rom + 0x7E00;
      unsigned       e_i;

      for (e_i = 0; e_i < 15; e_i++)
      {
         *t++ = 16;                              /* lines in this entry */
         *t++ = (unsigned char)(e_i * 16);       /* $2126 window 1 left  */
         *t++ = (unsigned char)(255 - e_i * 16); /* $2127 window 1 right */
      }
      *t = 0;                                    /* end of table         */

      /* CGWSEL at $00:FD00. The low two bits keep the subscreen as the
       * colour-math source and direct colour on, as the init does; the
       * top four are what the clip regions are built from. */
      t = rom + 0x7D00;
      for (e_i = 0; e_i < 15; e_i++)
      {
         *t++ = 16;
         *t++ = (unsigned char)((e_i << 4) | 0x03);
      }
      *t = 0;

      /* Main and sub screen window enable at $00:FC00, five bits each. */
      t = rom + 0x7C00;
      for (e_i = 0; e_i < 15; e_i++)
      {
         *t++ = 16;
         *t++ = (unsigned char)(e_i & 0x1f);        /* $212E main */
         *t++ = (unsigned char)((e_i * 3) & 0x1f);  /* $212F sub  */
      }
      *t = 0;
   }

   memcpy(rom + 0x7FC0, title, 21);
   rom[0x7FD5] = 0x20;   /* LoROM, slow ROM                           */
   rom[0x7FD6] = 0x00;   /* ROM only, no coprocessor                  */
   rom[0x7FD7] = 0x07;   /* 2^7 KB = 128 KB                           */
   rom[0x7FD8] = 0x00;   /* no SRAM                                   */
   rom[0x7FD9] = 0x01;   /* NTSC                                      */
   rom[0x7FDA] = 0x33;
   rom[0x7FDB] = 0x00;   /* version                                   */

   rom[0x7FFC] = 0x00;   /* emulation reset vector -> $8000           */
   rom[0x7FFD] = 0x80;

   /* Checksum over the image with the checksum fields zeroed, then the
    * complement, the way the cartridge header defines it. */
   for (i = 0; i < ROM_BYTES; i++)
      checksum += rom[i];
   checksum  &= 0xFFFF;
   complement = checksum ^ 0xFFFF;

   rom[0x7FDC] = (unsigned char)(complement & 0xFF);
   rom[0x7FDD] = (unsigned char)(complement >> 8);
   rom[0x7FDE] = (unsigned char)(checksum & 0xFF);
   rom[0x7FDF] = (unsigned char)(checksum >> 8);

   if (!(f = fopen(out, "wb")))
   {
      fprintf(stderr, "gen_framehash_rom: cannot write %s\n", out);
      return 1;
   }

   if (fwrite(rom, 1, sizeof(rom), f) != sizeof(rom))
   {
      fprintf(stderr, "gen_framehash_rom: short write to %s\n", out);
      fclose(f);
      return 1;
   }

   fclose(f);
   printf("%s: %u bytes, %u bytes of code, frame loop at $%04X\n",
         out, (unsigned)sizeof(rom), n, 0x8000 + M);
   return 0;
}
