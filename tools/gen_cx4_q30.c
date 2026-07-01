/* gen_cx4_q30.c - offline generator for the CX4 wireframe sine/cosine tables.
 *
 * The Capcom CX4 (Hitachi HG51B169) is a fixed-point DSP: it has no floating
 * point unit and performs all trigonometry through integer data-ROM tables.
 * snes9x2010's CX4 HLE historically used libm (sin/cos/sqrt/atan) for the
 * wireframe transform, which is both non-deterministic across platforms and
 * unfaithful to the hardware.  c4emu.c now uses baked Q30 (2^30) integer
 * sine/cosine tables instead.  Q30 exceeds the chip's own 24-bit (Q23)
 * data-ROM precision, so the transform is lossless relative to the old float
 * path while being fully deterministic.
 *
 * This program regenerates the C4SinTableQ30[]/C4CosTableQ30[] arrays that are
 * embedded in c4emu.c.  It is the ONLY place floating point is used, and it is
 * never compiled into the core.
 *
 *   cc -O2 -o gen_cx4_q30 gen_cx4_q30.c -lm && ./gen_cx4_q30 > q30tables.inc
 */
#include <stdio.h>
#include <math.h>
#include <stdint.h>

static void emit(const char *name, int cosine)
{
    int i;
    printf("static const int32_t %s[512] =\n{\n", name);
    for (i = 0; i < 512; i++)
    {
        double v = cosine ? cos(i * 2.0 * M_PI / 512.0)
                          : sin(i * 2.0 * M_PI / 512.0);
        int32_t q = (int32_t) llround(v * (double) (1 << 30));
        printf("%s%11ld%s", (i % 8 == 0) ? "\t" : " ",
               (long) q, (i == 511) ? "\n" : ",");
        if (i % 8 == 7)
            printf("\n");
    }
    printf("};\n\n");
}

int main(void)
{
    emit("C4SinTableQ30", 0);
    emit("C4CosTableQ30", 1);
    return 0;
}
