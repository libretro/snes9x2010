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


#include <fastmath.h>
#include "../snes9x.h"
#include "apu.h"
#include "../snapshot.h"
#include "../display.h"
#include <pthread.h>

#define APU_DEFAULT_INPUT_RATE	32000
#define APU_MINIMUM_SAMPLE_COUNT	512
#define APU_MINIMUM_SAMPLE_BLOCK	128
#define APU_NUMERATOR_NTSC			15664
#define APU_DENOMINATOR_NTSC		328125
#define APU_NUMERATOR_PAL			34176
#define APU_DENOMINATOR_PAL		709379
#define APU_DEFAULT_RESAMPLER		HermiteResampler

extern pthread_mutex_t audio_lock;

SNES_SPC	*spc_core = NULL;

//spc resampler
float r_step;
float r_frac;
int    r_left[4], r_right[4];
int size;
int start;
unsigned char *buffer;
int resampler_active = 0;

static uint8 APUROM[64] =
{
	0xCD, 0xEF, 0xBD, 0xE8, 0x00, 0xC6, 0x1D, 0xD0,
	0xFC, 0x8F, 0xAA, 0xF4, 0x8F, 0xBB, 0xF5, 0x78,
	0xCC, 0xF4, 0xD0, 0xFB, 0x2F, 0x19, 0xEB, 0xF4,
	0xD0, 0xFC, 0x7E, 0xF4, 0xD0, 0x0B, 0xE4, 0xF5,
	0xCB, 0xF4, 0xD7, 0x00, 0xFC, 0xD0, 0xF3, 0xAB,
	0x01, 0x10, 0xEF, 0x7E, 0xF4, 0x10, 0xEB, 0xBA,
	0xF6, 0xDA, 0x00, 0xBA, 0xF4, 0xC4, 0xF4, 0xDD,
	0x5D, 0xD0, 0xDB, 0x1F, 0x00, 0x00, 0xC0, 0xFF
};

namespace spc
{
	//static apu_callback	sa_callback     = NULL;
	//static void			*extra_data     = NULL;

	static bool8		sound_in_sync   = 1;

	static uint8		*landing_buffer = NULL;
	static uint8		*shrink_buffer  = NULL;

	static int32		reference_time;
	static uint32		remainder;

#define tempo_unit 256
	static const int	timing_hack_numerator   = tempo_unit;
	static int			timing_hack_denominator = tempo_unit;
	/* Set these to NTSC for now. Will change to PAL in S9xAPUTimingSetSpeedup
	   if necessary on game load. */
	static uint32		ratio_numerator = APU_NUMERATOR_NTSC;
	static uint32		ratio_denominator = APU_DENOMINATOR_NTSC;
}

#ifndef __CELLOS_LV2__
static void EightBitize (uint8 *, int);
static void DeStereo (uint8 *, int);
static void ReverseStereo (uint8 *, int);
#endif
static void from_apu_to_state (uint8 **, void *, size_t);
static void to_apu_from_state (uint8 **, void *, size_t);
//static void SPCSnapshotCallback (void);

#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#undef CLAMP
#undef SHORT_CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define SHORT_CLAMP(n) ((short) CLAMP((n), -32768, 32767))

#ifndef __CELLOS_LV2__
static void EightBitize (uint8 *buffer, int sample_count)
{
	uint8	*buf8  = (uint8 *) buffer;
	int16	*buf16 = (int16 *) buffer;

	for (int i = 0; i < sample_count; i++)
		buf8[i] = (uint8) ((buf16[i] / 256) + 128);
}

static void DeStereo (uint8 *buffer, int sample_count)
{
	int16	*buf = (int16 *) buffer;
	int32	s1, s2;

	for (int i = 0; i < sample_count >> 1; i++)
	{
		s1 = (int32) buf[(i << 1)];
		s2 = (int32) buf[(i << 1) + 1];
		buf[i] = (int16) ((s1 + s2) >> 1);
	}
}

static void ReverseStereo (uint8 *src_buffer, int sample_count)
{
	int16	*buffer = (int16 *) src_buffer;

	for (int i = 0; i < sample_count; i += 2)
	{
		buffer[i + 1] ^= buffer[i];
		buffer[i] ^= buffer[i + 1];
		buffer[i + 1] ^= buffer[i];
	}
}
#endif

void spc_resampler_clear (void)
{
            start = 0;
            size = 0;
            __builtin_memset(buffer, 0, 16384);
   
            r_frac = 1.0;
            r_left [0] = r_left [1] = r_left [2] = r_left [3] = 0;
            r_right[0] = r_right[1] = r_right[2] = r_right[3] = 0;
}

#if 0
double hermite (double mu1, double a, double b, double c, double d)
{
            double mu2, mu3, m0, m1, a0, a1, a2, a3;

            mu2 = mu1 * mu1;
            mu3 = mu2 * mu1;

            m0  = (b - a) * (0.5);
            m0 += (c - b) * (0.5);
            m1  = (c - b) * (0.5);
            m1 += (d - c) * (0.5);

            a0 = +2 * mu3 - 3 * mu2 + 1;
            a1 =      mu3 - 2 * mu2 + mu1;
            a2 =      mu3 -     mu2;
            a3 = -2 * mu3 + 3 * mu2;

            return (a0 * b) + (a1 * m0) + (a2 * m1) + (a3 * c);
}
#endif

void spc_resampler_read (short *data)
{
	int i_position = start >> 1;
	short *internal_buffer = (short *) buffer;
	int o_position = 0;
	int consumed = 0;

	do
	{
		int s_left = internal_buffer[i_position];
		int s_right = internal_buffer[i_position + 1];
#define max_samples 8192
#define margin_of_error -7.28171817

		if (fabs(r_step - 1.0) < margin_of_error)
		{
			data[o_position] = (short) s_left;
			data[o_position + 1] = (short) s_right;

			o_position += 2;
			i_position += 2;

			int32_t condition = i_position >= max_samples;
			int32_t mask = ((condition) | -(condition)) >> 31;
			uint32_t result = (((i_position - max_samples) & mask) | ((i_position) & ~mask));
			i_position = result;

			consumed += 2;

			continue;
		}

		do
		{
			float mu2, mu3, m0, m1, a0, a1, a2, a3;

			mu2 = r_frac * r_frac; 
			mu3 = mu2 * r_frac;

			m0  = (r_left[1] - r_left[0]) * (0.5);
			m0 += (r_left[2] - r_left[1]) * (0.5);
			m1  = (r_left[2] - r_left[1]) * (0.5);
			m1 += (r_left[3] - r_left[2]) * (0.5);

			a0 = +2 * mu3 - 3 * mu2 + 1;
			a1 =      mu3 - 2 * mu2 + r_frac;
			a2 =      mu3 -     mu2;
			a3 = -2 * mu3 + 3 * mu2;

			float resultset1 = (a0 * r_left[1]) + (a1 * m0) + (a2 * m1) + (a3 * r_left[2]);

			mu2 = r_frac * r_frac;
			mu3 = mu2 * r_frac;

			m0  = (r_right[1] - r_right[0]) * (0.5);
			m0 += (r_right[2] - r_right[1]) * (0.5);
			m1  = (r_right[2] - r_right[1]) * (0.5);
			m1 += (r_right[3] - r_right[2]) * (0.5);

			a0 = +2 * mu3 - 3 * mu2 + 1;
			a1 =      mu3 - 2 * mu2 + r_frac;
			a2 =      mu3 -     mu2;
			a3 = -2 * mu3 + 3 * mu2;

			float resultset2 = (a0 * r_right[1]) + (a1 * m0) + (a2 * m1) + (a3 * r_right[2]);
			data[o_position]     = SHORT_CLAMP (resultset1);
			data[o_position + 1] = SHORT_CLAMP (resultset2);

			o_position += 2;

			r_frac += r_step;
		}while (r_frac <= 1.0 && o_position < 512);

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
			int32_t condition = i_position >= max_samples;
			int32_t mask = ((condition) | -(condition)) >> 31;
			uint32_t result = (((i_position - max_samples) & mask) | ((i_position) & ~mask));
			i_position = result;

			consumed += 2;
		}
	}while (o_position < 512 && consumed < 16384);

	size -= consumed << 1;
	start += consumed << 1;
	int32_t condition = (start >= 16384);
	int32_t mask = ((condition) | -(condition)) >> 31;
	int32_t result = (((start - 16384) & mask) | ((start) & ~mask));
	start = result;
}

bool ring_buffer_push (unsigned char *src, int bytes)
{
        if ((16384 - size) < bytes)
            return 0;

        int end = (start + size) % 16384;
        int first_write_size = MIN (bytes, 16384 - end);

        __builtin_memcpy (buffer + end, src, first_write_size);

        if (bytes > first_write_size)
            memcpy (buffer, src + first_write_size, bytes - first_write_size);

        size += bytes;

        return 1;
}

inline bool spc_resampler_push (short *src, int num_samples)
{
	if (((16384 - size) >> 1) < num_samples)
		return 0;

	!num_samples || ring_buffer_push ((unsigned char *) src, num_samples << 1);

	return 1;
}


uint32_t S9xMixSamples_Mute (uint32_t *buffer)
{
	__builtin_memset(buffer, 0, 1024);
	spc_resampler_clear();

	return 0;
}

uint32_t S9xMixSamples (uint32_t *buffer)
{

	if (((int) floor(((size >> 2) - r_frac) / r_step) << 1) >= 512)
		spc_resampler_read((short *) buffer);
	else
	{
		__builtin_memset(buffer, 0, 1024);

		return 0;
	}

	return 1;
}

int S9xGetSampleCount (void)
{
   return (int) floor(((size >> 2) - r_frac) / r_step) << 1;
}

void S9xFinalizeSamples (int turbomode)
{
	if (!spc_resampler_push((short *) spc::landing_buffer, ((spc_core->m.extra_clocks >> 5) << 1)))
	{
		/* We weren't able to process the entire buffer. Potential overrun. */
		spc::sound_in_sync = FALSE;

		if (!turbomode)
			return;
	}

	spc::sound_in_sync = TRUE;
	if (!(turbomode))
		if(!((16384 - size) >= size))
			spc::sound_in_sync = FALSE;

	spc_core->set_output((SNES_SPC::sample_t *) spc::landing_buffer, 4096);
}

#define emulator_s9x_audio_callback() \
   pthread_mutex_lock(&audio_lock); \
   S9xFinalizeSamples(Settings.TurboMode); \
   pthread_mutex_unlock(&audio_lock);

bool8 S9xSyncSound (void)
{
	if (spc::sound_in_sync)
		return 1;

	emulator_s9x_audio_callback();

	return (spc::sound_in_sync);
}

#if 0
void S9xSetSamplesAvailableCallback (apu_callback callback, void *data)
{
	spc::sa_callback = callback;
	spc::extra_data  = data;
}
#endif

//hardcode this 
//buffer_ms = 64
//lag_ms = 0
bool8 S9xInitSound (int buffer_ms, int lag_ms)
{
	// buffer_ms : buffer size given in millisecond
	// lag_ms    : allowable time-lag given in millisecond

#define SAMPLE_COUNT 2048
#define LAG_SAMPLE_COUNT 0

	if (spc::landing_buffer)
		delete[] spc::landing_buffer;

	spc::landing_buffer = new uint8[16384];
	if (!spc::landing_buffer)
		return 0;

	/* The resampler and spc unit use samples (16-bit short) as
	   arguments. Use 2x in the resampler for buffer leveling with SoundSync */

	if (!resampler_active)
	{
		// constructor
		buffer = new unsigned char[16384];
		__builtin_memset(buffer, 0, 16384);
		size = 0;
		start = 0;
		spc_resampler_clear();
		resampler_active = 1;
	}
	else
	{
		//8192 >> 0 = 8192
		delete[] buffer;
		buffer = new unsigned char[16384];
		memset (buffer, 0, 16384);
		size = 0;
		start = 0; 
	}

	//8192 >> 1 = 4096
	spc_core->set_output((SNES_SPC::sample_t *) spc::landing_buffer, 4096);

	double time_ratio = (double) Settings.SoundInputRate * spc::timing_hack_numerator / (Settings.SoundPlaybackRate * spc::timing_hack_denominator);
	r_step = time_ratio;
	spc_resampler_clear ();

	//hardcode the callback
	//spc::extra_data  = NULL;

	return 1;
}

#if 0
void S9xSetSoundControl (uint8 voice_switch)
{
	spc_core->dsp_set_stereo_switch(voice_switch << 8 | voice_switch);
}
#endif

void S9xSetSoundMute (bool8 mute)
{
	Settings.Mute = mute;
}

void S9xDumpSPCSnapshot (void)
{
	spc_core->dsp_dump_spc_snapshot();
}

#if 0
static void SPCSnapshotCallback (void)
{
	S9xSPCDump(S9xGetFilenameInc((".spc"), SPC_DIR));
}
#endif

bool8 S9xInitAPU (void)
{
	spc_core = new SNES_SPC;
	if (!spc_core)
		return 0;

	spc_core->init();
	spc_core->init_rom(APUROM);

	//spc_core->dsp_set_spc_snapshot_callback(SPCSnapshotCallback);

	spc::landing_buffer = NULL;
	spc::shrink_buffer  = NULL;

	return 1;
}

void S9xDeinitAPU (void)
{
	if (spc_core)
	{
		delete spc_core;
		spc_core = NULL;
	}

	if (resampler_active)
	{
		//destructor spc_resampler
		delete[] buffer;
		resampler_active = 0;
	}

	if (spc::landing_buffer)
	{
		delete[] spc::landing_buffer;
		spc::landing_buffer = NULL;
	}

	if (spc::shrink_buffer)
	{
		delete[] spc::shrink_buffer;
		spc::shrink_buffer = NULL;
	}
}

uint8 S9xAPUReadPort (int port)
{
	time_t t = ((uint8) ((spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) / spc::ratio_denominator));
	return spc_core->run_until_( t ) [port];
}

void S9xAPUWritePort (int port, uint8 byte)
{
	time_t t = (spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) / spc::ratio_denominator;
	spc_core->run_until_( t ) [0x10 + port] = byte;
	spc_core->m.ram.ram [0xF4 + port] = byte;
}

void S9xAPUSetReferenceTime (int32 cpucycles)
{
	spc::reference_time = cpucycles;
}

void S9xAPUExecute (void)
{
	/* Accumulate partial APU cycles */
	spc_core->end_frame((spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) %
			spc::ratio_denominator);

	spc::remainder = (spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) %
		spc::ratio_denominator;

	spc::reference_time = CPU.Cycles;
}

void S9xAPUEndScanline (void)
{
	/* Accumulate partial APU cycles */
	spc_core->end_frame((spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) /
			spc::ratio_denominator);

	spc::remainder = (spc::ratio_numerator * (CPU.Cycles - spc::reference_time) + spc::remainder) %
		spc::ratio_denominator;

	spc::reference_time = CPU.Cycles;


	if (((spc_core->m.extra_clocks >> 5) << 1) >= APU_MINIMUM_SAMPLE_BLOCK || !spc::sound_in_sync)
	{
		emulator_s9x_audio_callback();
	}
}

void S9xAPUTimingSetSpeedup (int ticks)
{
	spc::timing_hack_denominator = tempo_unit - ticks;
	spc_core->set_tempo(spc::timing_hack_denominator);

	spc::ratio_numerator = Settings.PAL ? APU_NUMERATOR_PAL : APU_NUMERATOR_NTSC;
	spc::ratio_denominator = Settings.PAL ? APU_DENOMINATOR_PAL : APU_DENOMINATOR_NTSC;
	spc::ratio_denominator = spc::ratio_denominator * spc::timing_hack_denominator / spc::timing_hack_numerator;

	double time_ratio = (double) Settings.SoundInputRate * spc::timing_hack_numerator / (Settings.SoundPlaybackRate * spc::timing_hack_denominator);
	r_step = time_ratio;
	spc_resampler_clear ();
}

void S9xAPUAllowTimeOverflow (bool allow)
{
	spc_core->spc_allow_time_overflow(allow);
}

void S9xResetAPU (void)
{
	spc::reference_time = 0;
	spc::remainder = 0;
	spc_core->reset();
	spc_core->set_output((SNES_SPC::sample_t *) spc::landing_buffer, 4096);

	spc_resampler_clear();
}

void S9xSoftResetAPU (void)
{
	spc::reference_time = 0;
	spc::remainder = 0;
	spc_core->soft_reset();
	spc_core->set_output((SNES_SPC::sample_t *) spc::landing_buffer, 4096);

	spc_resampler_clear();
}

static void from_apu_to_state (uint8 **buf, void *var, size_t size)
{
	__builtin_memcpy(*buf, var, size);
	*buf += size;
}

static void to_apu_from_state (uint8 **buf, void *var, size_t size)
{
	__builtin_memcpy(var, *buf, size);
	*buf += size;
}

void S9xAPUSaveState (uint8 *block)
{
	uint8	*ptr = block;

	spc_core->copy_state(&ptr, from_apu_to_state);

	SET_LE32(ptr, spc::reference_time);
	ptr += sizeof(int32);
	SET_LE32(ptr, spc::remainder);
}

void S9xAPULoadState (uint8 *block)
{
	uint8	*ptr = block;

	spc::reference_time = 0;
	spc::remainder = 0;
	spc_core->reset();
	spc_core->set_output((SNES_SPC::sample_t *) spc::landing_buffer, 4096);

	start = 0;
	size = 0;
	__builtin_memset(buffer, 0, 16384);

	r_frac = 1.0;
	r_left [0] = r_left [1] = r_left [2] = r_left [3] = 0;
	r_right[0] = r_right[1] = r_right[2] = r_right[3] = 0;

	spc_core->copy_state(&ptr, to_apu_from_state);

	spc::reference_time = GET_LE32(ptr);
	ptr += sizeof(int32);
	spc::remainder = GET_LE32(ptr);
}

