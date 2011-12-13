/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002  Gary Henderson (gary.henderson@ntlworld.com),
                             Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2002 - 2004  Matthew Kendora

  (c) Copyright 2002 - 2005  Peter Bortas (peter@bortas.org)

  (c) Copyright 2004 - 2005  Joel Yliluoma (http://iki.fi/bisqwit/)

  (c) Copyright 2001 - 2006  John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2006  funkyass (funkyass@spam.shaw.ca),
                             Kris Bleakley (codeviolation@hotmail.com)

  (c) Copyright 2002 - 2010  Brad Jorsch (anomie@users.sourceforge.net),
                             Nach (n-a-c-h@users.sourceforge.net),
                             zones (kasumitokoduck@yahoo.com)

  (c) Copyright 2006 - 2007  nitsuja

  (c) Copyright 2009 - 2010  BearOso,
                             OV2


  BS-X C emulator code
  (c) Copyright 2005 - 2006  Dreamer Nom,
                             zones

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003  _Demo_ (_demo_@zsnes.com),
                             Nach,
                             zsKnight (zsknight@zsnes.com)

  C4 C++ code
  (c) Copyright 2003 - 2006  Brad Jorsch,
                             Nach

  DSP-1 emulator code
  (c) Copyright 1998 - 2006  _Demo_,
                             Andreas Naive (andreasnaive@gmail.com),
                             Gary Henderson,
                             Ivar (ivar@snes9x.com),
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora,
                             Nach,
                             neviksti (neviksti@hotmail.com)

  DSP-2 emulator code
  (c) Copyright 2003         John Weidman,
                             Kris Bleakley,
                             Lord Nightmare (lord_nightmare@users.sourceforge.net),
                             Matthew Kendora,
                             neviksti

  DSP-3 emulator code
  (c) Copyright 2003 - 2006  John Weidman,
                             Kris Bleakley,
                             Lancer,
                             z80 gaiden

  DSP-4 emulator code
  (c) Copyright 2004 - 2006  Dreamer Nom,
                             John Weidman,
                             Kris Bleakley,
                             Nach,
                             z80 gaiden

  OBC1 emulator code
  (c) Copyright 2001 - 2004  zsKnight,
                             pagefault (pagefault@zsnes.com),
                             Kris Bleakley
                             Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code used in 1.39-1.51
  (c) Copyright 2002         Matthew Kendora with research by
                             zsKnight,
                             John Weidman,
                             Dark Force

  SPC7110 and RTC C++ emulator code used in 1.52+
  (c) Copyright 2009         byuu,
                             neviksti

  S-DD1 C emulator code
  (c) Copyright 2003         Brad Jorsch with research by
                             Andreas Naive,
                             John Weidman

  S-RTC C emulator code
  (c) Copyright 2001 - 2006  byuu,
                             John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003         Feather,
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003  _Demo_,
                             pagefault,
                             zsKnight

  Super FX C emulator code
  (c) Copyright 1997 - 1999  Ivar,
                             Gary Henderson,
                             John Weidman

  Sound emulator code used in 1.5-1.51
  (c) Copyright 1998 - 2003  Brad Martin
  (c) Copyright 1998 - 2006  Charles Bilyue'

  Sound emulator code used in 1.52+
  (c) Copyright 2004 - 2007  Shay Green (gblargg@gmail.com)

  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004  Marcus Comstedt (marcus@mc.pp.se)

  2xSaI filter
  (c) Copyright 1999 - 2001  Derek Liauw Kie Fa

  HQ2x, HQ3x, HQ4x filters
  (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)

  NTSC filter
  (c) Copyright 2006 - 2007  Shay Green

  GTK+ GUI code
  (c) Copyright 2004 - 2010  BearOso

  Win32 GUI code
  (c) Copyright 2003 - 2006  blip,
                             funkyass,
                             Matthew Kendora,
                             Nach,
                             nitsuja
  (c) Copyright 2009 - 2010  OV2

  Mac OS GUI code
  (c) Copyright 1998 - 2001  John Stiles
  (c) Copyright 2001 - 2010  zones


  Specific ports contains the works of other authors. See headers in
  individual files.


  Snes9x homepage: http://www.snes9x.com/

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without
  fee, providing that this license information and copyright notice appear
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
 ***********************************************************************************/


#include <math.h>
#include "../snes9x.h"
#include "apu.h"
#include "../snapshot.h"
#include "../display.h"

#define APU_DEFAULT_INPUT_RATE		32000
#define APU_MINIMUM_SAMPLE_COUNT	512
#define APU_MINIMUM_SAMPLE_BLOCK	128
#define APU_NUMERATOR_NTSC		15664
#define APU_DENOMINATOR_NTSC		328125
#define APU_NUMERATOR_PAL		34176
#define APU_DENOMINATOR_PAL		709379

static apu_callback	sa_callback     = NULL;

static bool8		sound_in_sync   = TRUE;

static int		buffer_size;
static int		lag_master      = 0;
static int		lag             = 0;

static short		*landing_buffer = NULL;

static bool8		resampler      = false;

static int32		reference_time;
static uint32		spc_remainder;

static const int	timing_hack_numerator   = TEMPO_UNIT;
static int		timing_hack_denominator = TEMPO_UNIT;
/* Set these to NTSC for now. Will change to PAL in S9xAPUTimingSetSpeedup
   if necessary on game load. */
static uint32		ratio_numerator = APU_NUMERATOR_NTSC;
static uint32		ratio_denominator = APU_DENOMINATOR_NTSC;

/***********************************************************************************
RESAMPLER
 ***********************************************************************************/
static int rb_size;
static int rb_buffer_size;
static int rb_start;
static unsigned char *rb_buffer;
static float r_step;
static float r_frac;
static int    r_left[4], r_right[4];

#define SPACE_EMPTY() (rb_buffer_size - rb_size)
#define SPACE_FILLED() (rb_size)
#define MAX_WRITE() (SPACE_EMPTY() >> 1)
#define AVAIL() ((int) floor (((rb_size >> 2) - r_frac) / r_step) * 2)

#define RESAMPLER_MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define SHORT_CLAMP(n) ((short) CLAMP((n), -32768, 32767))

static float hermite (float mu1, float a, float b, float c, float d)
{
	float mu2, mu3, m0, m1, a0, a1, a2, a3;
	mu2 = mu1 * mu1;
	mu3 = mu2 * mu1;
	m0  = (c - a) * 0.5;
	m1  = (d - b) * 0.5;
	a0 = +2 * mu3 - 3 * mu2 + 1;
	a1 =      mu3 - 2 * mu2 + mu1;
	a2 =      mu3 -     mu2;
	a3 = -2 * mu3 + 3 * mu2;
	return (a0 * b) + (a1 * m0) + (a2 * m1) + (a3 * c);
}

static void resampler_clear(void)
{
	rb_start = 0;
	rb_size = 0;
	memset (rb_buffer,  0, rb_buffer_size);

	r_frac = 1.0;
	r_left [0] = r_left [1] = r_left [2] = r_left [3] = 0;
	r_right[0] = r_right[1] = r_right[2] = r_right[3] = 0;
}

static void resampler_time_ratio(double ratio)
{
	r_step = ratio;
	resampler_clear();
}

static void resampler_read(short *data, int num_samples)
{
	int i_position = rb_start >> 1;
	short *internal_buffer = (short *)rb_buffer;
	int o_position = 0;
	int consumed = 0;

	while (o_position < num_samples && consumed < rb_buffer_size)
	{
		int s_left = internal_buffer[i_position];
		int s_right = internal_buffer[i_position + 1];
		int max_samples = rb_buffer_size >> 1;

		while (r_frac <= 1.0 && o_position < num_samples)
		{
			data[o_position]     = SHORT_CLAMP (hermite(r_frac, r_left [0], r_left [1], r_left [2], r_left [3]));
			data[o_position + 1] = SHORT_CLAMP (hermite(r_frac, r_right[0], r_right[1], r_right[2], r_right[3]));

			o_position += 2;

			r_frac += r_step;
		}

		if (r_frac > 1.0)
		{
			r_left [0] = r_left [1];
			r_left [1] = r_left [2];
			r_left [2] = r_left [3];
			r_left [3] = s_left;

			r_right[0] = r_right[1];
			r_right[1] = r_right[2];
			r_right[2] = r_right[3];
			r_right[3] = s_right;                    

			r_frac -= 1.0;

			i_position += 2;
			if (i_position >= max_samples)
				i_position -= max_samples;
			consumed += 2;
		}
	}

	rb_size -= consumed << 1;
	rb_start += consumed << 1;
	if (rb_start >= rb_buffer_size)
		rb_start -= rb_buffer_size;
}

static void resampler_new(int num_samples)
{
	int new_size = num_samples << 1;

	rb_buffer_size = new_size;
	rb_buffer = new unsigned char[rb_buffer_size];
	memset (rb_buffer, 0, rb_buffer_size);

	rb_size = 0;
	rb_start = 0;
	resampler_clear();
}

static bool ring_buffer_push(unsigned char *src, int bytes)
{
	int end = (rb_start + rb_size) % rb_buffer_size;
	int first_write_size = RESAMPLER_MIN(bytes, rb_buffer_size - end);

	memcpy (rb_buffer + end, src, first_write_size);

	if (bytes > first_write_size)
		memcpy (rb_buffer, src + first_write_size, bytes - first_write_size);

	rb_size += bytes;

	return true;
}


static inline bool resampler_push(short *src, int num_samples)
{
	int bytes = num_samples << 1;
	if (MAX_WRITE() < num_samples || SPACE_EMPTY() < bytes)
		return false;

	!num_samples || ring_buffer_push((unsigned char *)src, bytes);

	return true;
}

static inline void resampler_resize (int num_samples)
{
	int size = num_samples << 1;
	delete[] rb_buffer;
	rb_buffer_size = rb_size;
	rb_buffer = new unsigned char[rb_buffer_size];
	memset (rb_buffer, 0, rb_buffer_size);

	rb_size = 0;
	rb_start = 0;
}

/***********************************************************************************
APU
 ***********************************************************************************/

bool8 S9xMixSamples (short *buffer, int sample_count)
{
	short	*dest = buffer;

	if (AVAIL() >= (sample_count + lag))
	{
		resampler_read(dest, sample_count);
		if (lag == lag_master)
			lag = 0;
	}
	else
	{
		memset(buffer, 0, (sample_count << 1) >> 0);
		if (lag == 0)
			lag = lag_master;

		return (FALSE);
	}

	return (TRUE);
}

int S9xGetSampleCount (void)
{
	return AVAIL();
}

void S9xFinalizeSamples (void)
{
	if (!resampler_push((short *) landing_buffer, spc_sample_count()))
	{
		/* We weren't able to process the entire buffer. Potential overrun. */
		sound_in_sync = FALSE;

		if (Settings.SoundSync)
			return;
	}

	sound_in_sync = FALSE;

	if (!Settings.SoundSync || (SPACE_EMPTY() >= SPACE_FILLED()))
		sound_in_sync = TRUE;

	spc_set_output(landing_buffer, buffer_size >> 1);
}

void S9xLandSamples (void)
{
	if (sa_callback != NULL)
		sa_callback();
	else
		S9xFinalizeSamples();
}

void S9xClearSamples (void)
{
	resampler_clear();
	lag = lag_master;
}

bool8 S9xSyncSound (void)
{
	if (!Settings.SoundSync || sound_in_sync)
		return (TRUE);

	S9xLandSamples();

	return (sound_in_sync);
}

void S9xSetSamplesAvailableCallback (apu_callback callback)
{
	sa_callback = callback;
}

static void UpdatePlaybackRate (void)
{
	if (Settings.SoundInputRate == 0)
		Settings.SoundInputRate = APU_DEFAULT_INPUT_RATE;

	double time_ratio = (double) Settings.SoundInputRate * timing_hack_numerator / (Settings.SoundPlaybackRate * timing_hack_denominator);
	resampler_time_ratio(time_ratio);
}

bool8 S9xInitSound (int buffer_ms, int lag_ms)
{
	// buffer_ms : buffer size given in millisecond
	// lag_ms    : allowable time-lag given in millisecond

	int	sample_count     = buffer_ms * 32000 / 1000;
	int	lag_sample_count = lag_ms    * 32000 / 1000;

	lag_master = lag_sample_count;

	lag_master <<= 1;

	lag = lag_master;

	if (sample_count < APU_MINIMUM_SAMPLE_COUNT)
		sample_count = APU_MINIMUM_SAMPLE_COUNT;

	buffer_size = sample_count;
	buffer_size <<= 1;
	buffer_size <<= 1;

	printf("Sound buffer size: %d (%d samples)\n", buffer_size, sample_count);

	if (landing_buffer)
		delete[] landing_buffer;
	landing_buffer = new short[buffer_size * 2];
	if (!landing_buffer)
		return (FALSE);

	/* The resampler and spc unit use samples (16-bit short) as
	   arguments. Use 2x in the resampler for buffer leveling with SoundSync */
	if (!resampler)
	{
		resampler_new(buffer_size >> (Settings.SoundSync ? 0 : 1));
		resampler = true;
	}
	else
		resampler_resize(buffer_size >> (Settings.SoundSync ? 0 : 1));

	spc_set_output(landing_buffer, buffer_size >> 1);

	UpdatePlaybackRate();

	return true;
}

bool8 S9xInitAPU (void)
{
	spc_init();

	landing_buffer = NULL;

	return (TRUE);
}

void S9xDeinitAPU (void)
{
	if (resampler)
	{
		delete[] rb_buffer;
		resampler = false;
	}

	if (landing_buffer)
	{
		delete[] landing_buffer;
		landing_buffer = NULL;
	}
}

static inline int S9xAPUGetClock (int32 cpucycles)
{
	return (ratio_numerator * (cpucycles - reference_time) + spc_remainder) /
			ratio_denominator;
}

static inline int S9xAPUGetClockRemainder (int32 cpucycles)
{
	return (ratio_numerator * (cpucycles - reference_time) + spc_remainder) %
			ratio_denominator;
}

uint8 S9xAPUReadPort (int port)
{
	// Emulated port read at specified time
	return ((uint8) spc_run_until_(S9xAPUGetClock(CPU.Cycles))[port]);
}

void S9xAPUWritePort (int port, uint8 byte)
{
	// Emulated port write at specified time
	spc_run_until_( S9xAPUGetClock(CPU.Cycles) ) [0x10 + port] = byte;
	m.ram.ram [0xF4 + port] = byte;
}

void S9xAPUSetReferenceTime (int32 cpucycles)
{
	reference_time = cpucycles;
}

void S9xAPUExecute (void)
{
	/* Accumulate partial APU cycles */
	spc_end_frame(S9xAPUGetClock(CPU.Cycles));

	spc_remainder = S9xAPUGetClockRemainder(CPU.Cycles);

	S9xAPUSetReferenceTime(CPU.Cycles);
}

void S9xAPUEndScanline (void)
{
	S9xAPUExecute();

	if (spc_sample_count() >= APU_MINIMUM_SAMPLE_BLOCK || !sound_in_sync)
		S9xLandSamples();
}

void S9xAPUTimingSetSpeedup (int ticks)
{
	if (ticks != 0)
		printf("APU speedup hack: %d\n", ticks);

	timing_hack_denominator = TEMPO_UNIT - ticks;
	spc_set_tempo(timing_hack_denominator);

	ratio_numerator = Settings.PAL ? APU_NUMERATOR_PAL : APU_NUMERATOR_NTSC;
	ratio_denominator = Settings.PAL ? APU_DENOMINATOR_PAL : APU_DENOMINATOR_NTSC;
	ratio_denominator = ratio_denominator * timing_hack_denominator / timing_hack_numerator;

	UpdatePlaybackRate();
}

void S9xAPUAllowTimeOverflow (bool allow)
{
	allow_time_overflow = allow;
}

void S9xResetAPU (void)
{
	reference_time = 0;
	spc_remainder = 0;
	spc_reset();
	spc_set_output(landing_buffer, buffer_size >> 1);

	resampler_clear();
}

void S9xSoftResetAPU (void)
{
	reference_time = 0;
	spc_remainder = 0;
	spc_soft_reset();
	spc_set_output(landing_buffer, buffer_size >> 1);

	resampler_clear();
}

static void from_apu_to_state (uint8 **buf, void *var, size_t size)
{
	memcpy(*buf, var, size);
	*buf += size;
}

static void to_apu_from_state (uint8 **buf, void *var, size_t size)
{
	memcpy(var, *buf, size);
	*buf += size;
}

void S9xAPUSaveState (uint8 *block)
{
	uint8	*ptr = block;

	spc_copy_state(&ptr, from_apu_to_state);

	SET_LE32(ptr, reference_time);
	ptr += sizeof(int32);
	SET_LE32(ptr, spc_remainder);
}

void S9xAPULoadState (uint8 *block)
{
	uint8	*ptr = block;

	S9xResetAPU();

	spc_copy_state(&ptr, to_apu_from_state);

	reference_time = GET_LE32(ptr);
	ptr += sizeof(int32);
	spc_remainder = GET_LE32(ptr);
}

