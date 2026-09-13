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

static void emit(int b)
{
   rom[n++] = (unsigned char)b;
}

int main(int argc, char **argv)
{
   const char *out = (argc > 1) ? argv[1] : "framehash_test.sfc";
   const char *title = "FRAMEHASH TEST ROM   ";   /* exactly 21 bytes */
   unsigned    loop, here, i;
   unsigned    checksum = 0, complement;
   FILE       *f;

   memset(rom, 0, sizeof(rom));

   emit(0x78);                     /* SEI                             */
   emit(0x18);                     /* CLC                             */
   emit(0xFB);                     /* XCE          -> native mode     */
   emit(0xE2); emit(0x30);         /* SEP #$30     -> 8-bit A, X, Y   */
   emit(0x9C); emit(0x00); emit(0x42);  /* STZ $4200  no NMI/IRQ      */
   emit(0xA9); emit(0x8F);         /* LDA #$8F                        */
   emit(0x8D); emit(0x00); emit(0x21);  /* STA $2100  force blank     */
   emit(0xA9); emit(0x01);         /* LDA #$01                        */
   emit(0x8D); emit(0x05); emit(0x21);  /* STA $2105  BG mode 1       */
   emit(0xA9); emit(0x01);         /* LDA #$01                        */
   emit(0x8D); emit(0x2C); emit(0x21);  /* STA $212C  BG1 on main     */
   emit(0xA9); emit(0x0F);         /* LDA #$0F                        */
   emit(0x8D); emit(0x00); emit(0x21);  /* STA $2100  visible, full   */
   emit(0xA9); emit(0x00);         /* LDA #$00     -> colour counter  */

   loop = n;
   emit(0x9C); emit(0x21); emit(0x21);  /* STZ $2121  CGRAM addr 0    */
   emit(0x8D); emit(0x22); emit(0x21);  /* STA $2122  backdrop low    */
   emit(0x8D); emit(0x22); emit(0x21);  /* STA $2122  backdrop high   */
   emit(0x1A);                          /* INC A                      */
   here = n + 2;
   emit(0x80); emit((int)((loop - here) & 0xFF));   /* BRA loop       */

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
   printf("%s: %u bytes, %u bytes of code, loop at $%04X\n",
         out, (unsigned)sizeof(rom), n, 0x8000 + loop);
   return 0;
}
