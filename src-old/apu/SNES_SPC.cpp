// Core SPC emulation: CPU, timers, SMP registers, memory

// snes_spc 0.9.0. http://www.slack.net/~ant/

#include "SNES_SPC.h"

#include <string.h>
#include "blargg_endian.h"

/* Copyright (C) 2004-2007 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#include "blargg_source.h"
#include "../isel.h"

#define RAM         (m.ram.ram)
#define REGS        (m.smp_regs [0])
#define REGS_IN     (m.smp_regs [1])

// (n ? n : 256)
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)

// Note: SPC_MORE_ACCURACY exists mainly so I can run my validation tests, which
// do crazy echo buffer accesses.
#ifndef SPC_MORE_ACCURACY
	#define SPC_MORE_ACCURACY 0
#endif

#ifdef BLARGG_ENABLE_OPTIMIZER
	#include BLARGG_ENABLE_OPTIMIZER
#endif

#if INT_MAX < 0x7FFFFFFF
	#error "Requires that int type have at least 32 bits"
#endif


//// Timers

#if SPC_DISABLE_TEMPO
	#define TIMER_DIV( t, n ) ((n) >> t->prescaler)
	#define TIMER_MUL( t, n ) ((n) << t->prescaler)
#else
	#define TIMER_DIV( t, n ) ((n) / t->prescaler)
	#define TIMER_MUL( t, n ) ((n) * t->prescaler)
#endif

SNES_SPC::Timer* SNES_SPC::run_timer_( Timer* t, rel_time_t time )
{
	int elapsed = TIMER_DIV( t, time - t->next_time ) + 1;
	t->next_time += TIMER_MUL( t, elapsed );

	if ( t->enabled )
	{
		int remain = IF_0_THEN_256( t->period - t->divider );
		int divider = t->divider + elapsed;
		int over = elapsed - remain;
		if ( over >= 0 )
		{
			int n = over / t->period;
			t->counter = (t->counter + 1 + n) & 0x0F;
			divider = over - n * t->period;
		}
		t->divider = (uint8_t) divider;
	}
	return t;
}

inline SNES_SPC::Timer* SNES_SPC::run_timer( Timer* t, rel_time_t time )
{
	if ( time >= t->next_time )
	{
		int elapsed = TIMER_DIV( t, time - t->next_time ) + 1;
		t->next_time += TIMER_MUL( t, elapsed );

		if ( t->enabled )
		{
			int remain = IF_0_THEN_256( t->period - t->divider );
			int divider = t->divider + elapsed;
			int over = elapsed - remain;
			if ( over >= 0 )
			{
				int n = over / t->period;
				t->counter = (t->counter + 1 + n) & 0x0F;
				divider = over - n * t->period;
			}
			t->divider = (uint8_t) divider;
		}
	}
	return t;
}

//// DSP

#if SPC_LESS_ACCURATE
	int const max_reg_time = 29;
	
	signed char const SNES_SPC::reg_times_ [256] =
	{
		 -1,  0,-11,-10,-15,-11, -2, -2,  4,  3, 14, 14, 26, 26, 14, 22,
		  2,  3,  0,  1,-12,  0,  1,  1,  7,  6, 14, 14, 27, 14, 14, 23,
		  5,  6,  3,  4, -1,  3,  4,  4, 10,  9, 14, 14, 26, -5, 14, 23,
		  8,  9,  6,  7,  2,  6,  7,  7, 13, 12, 14, 14, 27, -4, 14, 24,
		 11, 12,  9, 10,  5,  9, 10, 10, 16, 15, 14, 14, -2, -4, 14, 24,
		 14, 15, 12, 13,  8, 12, 13, 13, 19, 18, 14, 14, -2,-36, 14, 24,
		 17, 18, 15, 16, 11, 15, 16, 16, 22, 21, 14, 14, 28, -3, 14, 25,
		 20, 21, 18, 19, 14, 18, 19, 19, 25, 24, 14, 14, 14, 29, 14, 25,
		 
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
		 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	};
	
	#define RUN_DSP( time, offset ) \
		int count = (time) - (offset) - m.dsp_time;\
		if ( count >= 0 )\
		{\
         int clock count = (count & ~(31)) + clocks_per_sample; \
			m.dsp_time += clock_count;\
			dsp.run( clock_count );\
		}
#else
	#define RUN_DSP( time, offset ) \
		{\
			int count = (time) - m.dsp_time;\
			if ( !SPC_MORE_ACCURACY || count )\
			{\
				m.dsp_time = (time);\
				dsp.run( count );\
			}\
		}
#endif

#if 0
inline void SNES_SPC::dsp_write( int data, rel_time_t time )
{
	RUN_DSP( time, reg_times [REGS [r_dspaddr]] )
#if SPC_LESS_ACCURATE
		else if ( m.dsp_time == skipping_time )
		{
			int r = REGS [r_dspaddr];
			if ( r == SPC_DSP::r_kon )
				m.skipped_kon |= data & ~dsp.m.regs[SPC_DSP::r_koff];

			if ( r == SPC_DSP::r_koff )
			{
				m.skipped_koff |= data;
				m.skipped_kon &= ~data;
			}
		}
#endif

#ifdef SPC_DSP_WRITE_HOOK
		SPC_DSP_WRITE_HOOK( m.spc_time + time, REGS [r_dspaddr], (uint8_t) data );
#endif

		if ( REGS [r_dspaddr] <= 0x7F )
		{
			int addr = REGS[r_dspaddr];
			dsp.m.regs [addr] = (uint8_t) data;
			switch ( addr & 0x0F )
			{
				case v_envx:
					dsp.m.envx_buf = (uint8_t) data;
					break;

				case v_outx:
					dsp.m.outx_buf = (uint8_t) data;
					break;

				case 0x0C:
					if ( addr == r_kon )
						dsp.m.new_kon = (uint8_t) data;

					if ( addr == r_endx ) // always cleared, regardless of data written
					{
						dsp.m.endx_buf = 0;
						dsp.m.regs [r_endx] = 0;
					}
					break;
			}
		}
}
#endif


//// Memory access extras

#if SPC_MORE_ACCURACY
	#define MEM_ACCESS( time, addr ) \
	{\
		if ( time >= m.dsp_time )\
		{\
			RUN_DSP( time, max_reg_time );\
		}\
	}
#else
	#define MEM_ACCESS( time, addr )
#endif


//// CPU write

#if SPC_MORE_ACCURACY
static unsigned char const glitch_probs [3] [256] =
{
	0xC3,0x92,0x5B,0x1C,0xD1,0x92,0x5B,0x1C,0xDB,0x9C,0x72,0x18,0xCD,0x5C,0x38,0x0B,
	0xE1,0x9C,0x74,0x17,0xCF,0x75,0x45,0x0C,0xCF,0x6E,0x4A,0x0D,0xA3,0x3A,0x1D,0x08,
	0xDB,0xA0,0x82,0x19,0xD9,0x73,0x3C,0x0E,0xCB,0x76,0x52,0x0B,0xA5,0x46,0x1D,0x09,
	0xDA,0x74,0x55,0x0F,0xA2,0x3F,0x21,0x05,0x9A,0x40,0x20,0x07,0x63,0x1E,0x10,0x01,
	0xDF,0xA9,0x85,0x1D,0xD3,0x84,0x4B,0x0E,0xCF,0x6F,0x49,0x0F,0xB3,0x48,0x1E,0x05,
	0xD8,0x77,0x52,0x12,0xB7,0x49,0x23,0x06,0xAA,0x45,0x28,0x07,0x7D,0x28,0x0F,0x07,
	0xCC,0x7B,0x4A,0x0E,0xB2,0x4F,0x24,0x07,0xAD,0x43,0x2C,0x06,0x86,0x29,0x11,0x07,
	0xAE,0x48,0x1F,0x0A,0x76,0x21,0x19,0x05,0x76,0x21,0x14,0x05,0x44,0x11,0x0B,0x01,
	0xE7,0xAD,0x96,0x23,0xDC,0x86,0x59,0x0E,0xDC,0x7C,0x5F,0x15,0xBB,0x53,0x2E,0x09,
	0xD6,0x7C,0x4A,0x16,0xBB,0x4A,0x25,0x08,0xB3,0x4F,0x28,0x0B,0x8E,0x23,0x15,0x08,
	0xCF,0x7F,0x57,0x11,0xB5,0x4A,0x23,0x0A,0xAA,0x42,0x28,0x05,0x7D,0x22,0x12,0x03,
	0xA6,0x49,0x28,0x09,0x82,0x2B,0x0D,0x04,0x7A,0x20,0x0F,0x04,0x3D,0x0F,0x09,0x03,
	0xD1,0x7C,0x4C,0x0F,0xAF,0x4E,0x21,0x09,0xA8,0x46,0x2A,0x07,0x85,0x1F,0x0E,0x07,
	0xA6,0x3F,0x26,0x07,0x7C,0x24,0x14,0x07,0x78,0x22,0x16,0x04,0x46,0x12,0x0A,0x02,
	0xA6,0x41,0x2C,0x0A,0x7E,0x28,0x11,0x05,0x73,0x1B,0x14,0x05,0x3D,0x11,0x0A,0x02,
	0x70,0x22,0x17,0x05,0x48,0x13,0x08,0x03,0x3C,0x07,0x0D,0x07,0x26,0x07,0x06,0x01,
	
	0xE0,0x9F,0xDA,0x7C,0x4F,0x18,0x28,0x0D,0xE9,0x9F,0xDA,0x7C,0x4F,0x18,0x1F,0x07,
	0xE6,0x97,0xD8,0x72,0x64,0x13,0x26,0x09,0xDC,0x67,0xA9,0x38,0x21,0x07,0x15,0x06,
	0xE9,0x91,0xD2,0x6B,0x63,0x14,0x2B,0x0E,0xD6,0x61,0xB7,0x41,0x2B,0x0E,0x10,0x09,
	0xCF,0x59,0xB0,0x2F,0x35,0x08,0x0F,0x07,0xB6,0x30,0x7A,0x21,0x17,0x07,0x09,0x03,
	0xE7,0xA3,0xE5,0x6B,0x65,0x1F,0x34,0x09,0xD8,0x6B,0xBE,0x45,0x27,0x07,0x10,0x07,
	0xDA,0x54,0xB1,0x39,0x2E,0x0E,0x17,0x08,0xA9,0x3C,0x86,0x22,0x16,0x06,0x07,0x03,
	0xD4,0x51,0xBC,0x3D,0x38,0x0A,0x13,0x06,0xB2,0x37,0x79,0x1C,0x17,0x05,0x0E,0x06,
	0xA7,0x31,0x74,0x1C,0x11,0x06,0x0C,0x02,0x6D,0x1A,0x38,0x10,0x0B,0x05,0x06,0x03,
	0xEB,0x9A,0xE1,0x7A,0x6F,0x13,0x34,0x0E,0xE6,0x75,0xC5,0x45,0x3E,0x0B,0x1A,0x05,
	0xD8,0x63,0xC1,0x40,0x3C,0x1B,0x19,0x06,0xB3,0x42,0x83,0x29,0x18,0x0A,0x08,0x04,
	0xD4,0x58,0xBA,0x43,0x3F,0x0A,0x1F,0x09,0xB1,0x33,0x8A,0x1F,0x1F,0x06,0x0D,0x05,
	0xAF,0x3C,0x7A,0x1F,0x16,0x08,0x0A,0x01,0x72,0x1B,0x52,0x0D,0x0B,0x09,0x06,0x01,
	0xCF,0x63,0xB7,0x47,0x40,0x10,0x14,0x06,0xC0,0x41,0x96,0x20,0x1C,0x09,0x10,0x05,
	0xA6,0x35,0x82,0x1A,0x20,0x0C,0x0E,0x04,0x80,0x1F,0x53,0x0F,0x0B,0x02,0x06,0x01,
	0xA6,0x31,0x81,0x1B,0x1D,0x01,0x08,0x08,0x7B,0x20,0x4D,0x19,0x0E,0x05,0x07,0x03,
	0x6B,0x17,0x49,0x07,0x0E,0x03,0x0A,0x05,0x37,0x0B,0x1F,0x06,0x04,0x02,0x07,0x01,
	
	0xF0,0xD6,0xED,0xAD,0xEC,0xB1,0xEB,0x79,0xAC,0x22,0x47,0x1E,0x6E,0x1B,0x32,0x0A,
	0xF0,0xD6,0xEA,0xA4,0xED,0xC4,0xDE,0x82,0x98,0x1F,0x50,0x13,0x52,0x15,0x2A,0x0A,
	0xF1,0xD1,0xEB,0xA2,0xEB,0xB7,0xD8,0x69,0xA2,0x1F,0x5B,0x18,0x55,0x18,0x2C,0x0A,
	0xED,0xB5,0xDE,0x7E,0xE6,0x85,0xD3,0x59,0x59,0x0F,0x2C,0x09,0x24,0x07,0x15,0x09,
	0xF1,0xD6,0xEA,0xA0,0xEC,0xBB,0xDA,0x77,0xA9,0x23,0x58,0x14,0x5D,0x12,0x2F,0x09,
	0xF1,0xC1,0xE3,0x86,0xE4,0x87,0xD2,0x4E,0x68,0x15,0x26,0x0B,0x27,0x09,0x15,0x02,
	0xEE,0xA6,0xE0,0x5C,0xE0,0x77,0xC3,0x41,0x67,0x1B,0x3C,0x07,0x2A,0x06,0x19,0x07,
	0xE4,0x75,0xC6,0x43,0xCC,0x50,0x95,0x23,0x35,0x09,0x14,0x04,0x15,0x05,0x0B,0x04,
	0xEE,0xD6,0xED,0xAD,0xEC,0xB1,0xEB,0x79,0xAC,0x22,0x56,0x14,0x5A,0x12,0x26,0x0A,
	0xEE,0xBB,0xE7,0x7E,0xE9,0x8D,0xCB,0x49,0x67,0x11,0x34,0x07,0x2B,0x0B,0x14,0x07,
	0xED,0xA7,0xE5,0x76,0xE3,0x7E,0xC4,0x4B,0x77,0x14,0x34,0x08,0x27,0x07,0x14,0x04,
	0xE7,0x8B,0xD2,0x4C,0xCA,0x56,0x9E,0x31,0x36,0x0C,0x11,0x07,0x14,0x04,0x0A,0x02,
	0xF0,0x9B,0xEA,0x6F,0xE5,0x81,0xC4,0x43,0x74,0x10,0x30,0x0B,0x2D,0x08,0x1B,0x06,
	0xE6,0x83,0xCA,0x48,0xD9,0x56,0xA7,0x23,0x3B,0x09,0x12,0x09,0x15,0x07,0x0A,0x03,
	0xE5,0x5F,0xCB,0x3C,0xCF,0x48,0x91,0x22,0x31,0x0A,0x17,0x08,0x15,0x04,0x0D,0x02,
	0xD1,0x43,0x91,0x20,0xA9,0x2D,0x54,0x12,0x17,0x07,0x09,0x02,0x0C,0x04,0x05,0x03,
};
#endif

// divided into multiple functions to keep rarely-used functionality separate
// so often-used functionality can be optimized better by compiler

// If write isn't preceded by read, data has this added to it
int const no_read_before_write = 0x2000;

void SNES_SPC::cpu_write_smp_reg_( int data, rel_time_t time, int addr )
{
	switch ( addr )
	{
		case r_t0target:
		case r_t1target:
		case r_t2target: {
					 Timer* t = &m.timers [addr - r_t0target];
					 int period = IF_0_THEN_256( data );
					 if ( t->period != period )
					 {
						 if ( time >= t->next_time )
							 t = run_timer_( t, time );

#if SPC_MORE_ACCURACY
						 // Insane behavior when target is written just after counter is
						 // clocked and counter matches new period and new period isn't 1, 2, 4, or 8
						 if ( t->divider == (period & 0xFF) &&
								 t->next_time == time + TIMER_MUL( t, 1 ) &&
								 ((period - 1) | ~0x0F) & period )
						 {
							 //dprintf( "SPC pathological timer target write\n" );

							 // If the period is 3, 5, or 9, there's a probability this behavior won't occur,
							 // based on the previous period
							 int prob = 0xFF;
							 int old_period = t->period & 0xFF;
							 if ( period == 3 ) prob = glitch_probs [0] [old_period];
							 if ( period == 5 ) prob = glitch_probs [1] [old_period];
							 if ( period == 9 ) prob = glitch_probs [2] [old_period];

							 // The glitch suppresses incrementing of one of the counter bits, based on
							 // the lowest set bit in the new period
							 int b = 1;
							 do{
								 b <<= 1;
							 }while ( !(period & b) );

							 if ( (rand() >> 4 & 0xFF) <= prob )
								 t->divider = (t->divider - b) & 0xFF;
						 }
#endif
						 t->period = period;
					 }
					 break;
				 }

		case r_t0out:
		case r_t1out:
		case r_t2out:
				 if ( data < (no_read_before_write  >> 1) )
					 run_timer( &m.timers [addr - r_t0out], time - 1 )->counter = 0;
				 break;

				 // Registers that act like RAM
		case 0x8:
		case 0x9:
				 REGS_IN [addr] = (uint8_t) data;
				 break;
#if 0
		case r_test:
				 /*
				    if ( (uint8_t) data != 0x0A )
				    dprintf( "SPC wrote to test register\n" );
				  */
				 break;
#endif

		case r_control:
				 // port clears
				 if ( data & 0x10 )
				 {
					 REGS_IN [r_cpuio0] = 0;
					 REGS_IN [r_cpuio1] = 0;
				 }
				 if ( data & 0x20 )
				 {
					 REGS_IN [r_cpuio2] = 0;
					 REGS_IN [r_cpuio3] = 0;
				 }

				 // timers
				 {
					 Timer* t = &m.timers [0];
					 int enabled = data >> 0 & 1;
					 if ( t->enabled != enabled )
					 {
						 if ( time >= t->next_time )
							 t = run_timer_( t, time );

						 t->enabled = enabled;
						 if ( enabled )
						 {
							 t->divider = 0;
							 t->counter = 0;
						 }
					 }

					 t = &m.timers [1];
					 enabled = data >> 1 & 1;
					 if ( t->enabled != enabled )
					 {
						 if ( time >= t->next_time )
							 t = run_timer_( t, time );

						 t->enabled = enabled;
						 if ( enabled )
						 {
							 t->divider = 0;
							 t->counter = 0;
						 }
					 }

					 t = &m.timers [2];
					 enabled = data >> 2 & 1;
					 if ( t->enabled != enabled )
					 {
						 if ( time >= t->next_time )
							 t = run_timer_( t, time );

						 t->enabled = enabled;
						 if ( enabled )
						 {
							 t->divider = 0;
							 t->counter = 0;
						 }
					 }
				 }

				 //enable_rom( data & 0x80 );
				 int enable = data & 0x80;
				 if ( m.rom_enabled != enable )
				 {
					 m.rom_enabled = dsp.rom_enabled = enable;
					 if ( enable )
						 __builtin_memcpy( m.hi_ram, &RAM [rom_addr], sizeof m.hi_ram );
					 __builtin_memcpy( &RAM [rom_addr], (enable ? m.rom : m.hi_ram), rom_size );
					 // TODO: ROM can still get overwritten when DSP writes to echo buffer
				 }
				 break;
	}
}

void SNES_SPC::cpu_write_smp_reg( int data, rel_time_t time, int addr )
{
	if ( addr == r_dspdata ) // 99%
	{
		//dsp_write( data, time );
		RUN_DSP( time, reg_times [REGS [r_dspaddr]] )
#if SPC_LESS_ACCURATE
			else if ( m.dsp_time == skipping_time )
			{
				int r = REGS [r_dspaddr];
				if ( r == SPC_DSP::r_kon )
					m.skipped_kon |= data & ~dsp.m.regs[SPC_DSP::r_koff];

				if ( r == SPC_DSP::r_koff )
				{
					m.skipped_koff |= data;
					m.skipped_kon &= ~data;
				}
			}
#endif

#ifdef SPC_DSP_WRITE_HOOK
			SPC_DSP_WRITE_HOOK( m.spc_time + time, REGS [r_dspaddr], (uint8_t) data );
#endif

			if ( REGS [r_dspaddr] <= 0x7F )
			{
				int addr = REGS[r_dspaddr];
				dsp.m.regs [addr] = (uint8_t) data;
				switch ( addr & 0x0F )
				{
					case v_envx:
						dsp.m.envx_buf = (uint8_t) data;
						break;

					case v_outx:
						dsp.m.outx_buf = (uint8_t) data;
						break;

					case 0x0C:
						if ( addr == r_kon )
							dsp.m.new_kon = (uint8_t) data;

						if ( addr == r_endx ) // always cleared, regardless of data written
						{
							dsp.m.endx_buf = 0;
							dsp.m.regs [r_endx] = 0;
						}
						break;
				}
			}
	}
	else
		cpu_write_smp_reg_( data, time, addr );
}

int const bits_in_int = CHAR_BIT * sizeof (int);

void SNES_SPC::cpu_write( int data, int addr, rel_time_t time )
{
	MEM_ACCESS( time, addr )

		// RAM
		RAM [addr] = (uint8_t) data;
	int reg = addr - 0xF0;
	if ( reg >= 0 ) // 64%
	{
		// $F0-$FF
		if ( reg < reg_count ) // 87%
		{
			REGS [reg] = (uint8_t) data;

			// Ports
#ifdef SPC_PORT_WRITE_HOOK
			if ( (unsigned) (reg - r_cpuio0) < port_count )
				SPC_PORT_WRITE_HOOK( m.spc_time + time, (reg - r_cpuio0),
						(uint8_t) data, &REGS [r_cpuio0] );
#endif

			// Registers other than $F2 and $F4-$F7
			//if ( reg != 2 && reg != 4 && reg != 5 && reg != 6 && reg != 7 )
			// TODO: this is a bit on the fragile side
			if ( ((~0x2F00 << (bits_in_int - 16)) << reg) < 0 ) // 36%
			{
				//cpu_write_smp_reg( data, time, reg );
				if ( reg == r_dspdata ) // 99%
				{
					//dsp_write( data, time );
					RUN_DSP( time, reg_times [REGS [r_dspaddr]] )
#if SPC_LESS_ACCURATE
						else if ( m.dsp_time == skipping_time )
						{
							int r = REGS [r_dspaddr];
							if ( r == SPC_DSP::r_kon )
								m.skipped_kon |= data & ~dsp.m.regs[SPC_DSP::r_koff];

							if ( r == SPC_DSP::r_koff )
							{
								m.skipped_koff |= data;
								m.skipped_kon &= ~data;
							}
						}
#endif

#ifdef SPC_DSP_WRITE_HOOK
						SPC_DSP_WRITE_HOOK( m.spc_time + time, REGS [r_dspaddr], (uint8_t) data );
#endif

						if ( REGS [r_dspaddr] <= 0x7F )
						{
							int addr = REGS[r_dspaddr];
							dsp.m.regs [addr] = (uint8_t) data;
							switch ( addr & 0x0F )
							{
								case v_envx:
									dsp.m.envx_buf = (uint8_t) data;
									break;

								case v_outx:
									dsp.m.outx_buf = (uint8_t) data;
									break;

								case 0x0C:
									if ( addr == r_kon )
										dsp.m.new_kon = (uint8_t) data;

									if ( addr == r_endx ) // always cleared, regardless of data written
									{
										dsp.m.endx_buf = 0;
										dsp.m.regs [r_endx] = 0;
									}
									break;
							}
						}
				}
				else
					cpu_write_smp_reg_( data, time, reg );
			}
		}
		// High mem/address wrap-around
		else
		{
			reg -= rom_addr - 0xF0;
			if ( reg >= 0 ) // 1% in IPL ROM area or address wrapped around
			{
				//cpu_write_high( data, reg, time );
				if ( reg < rom_size )
				{
					m.hi_ram [reg] = (uint8_t) data;
					if ( m.rom_enabled )
						RAM [reg + rom_addr] = m.rom [reg]; // restore overwritten ROM
				}
				else
				{
					*(&(RAM [0]) + reg + rom_addr) = cpu_pad_fill; // restore overwritten padding
					cpu_write( data, reg + rom_addr - 0x10000, time );
				}
			}
		}
	}
}


//// CPU read

inline int SNES_SPC::cpu_read_smp_reg( int reg, rel_time_t time )
{
	int result = REGS_IN [reg];
	reg -= r_dspaddr;
	// DSP addr and data
	if ( (unsigned) reg <= 1 ) // 4% 0xF2 and 0xF3
	{
		result = REGS [r_dspaddr];
		if ( (unsigned) reg == 1 )
		{
			// DSP Read (0xF3)
			RUN_DSP( time, reg_times [REGS [r_dspaddr] & 0x7F] );

			result = dsp.m.regs[REGS [r_dspaddr] & 0x7F];

#ifdef SPC_DSP_READ_HOOK
			SPC_DSP_READ_HOOK( spc_time + time, (REGS [r_dspaddr] & 0x7F), result );
#endif
			//End of DSP Read
		}
	}
	return result;
}

int SNES_SPC::cpu_read( int addr, rel_time_t time )
{
	MEM_ACCESS( time, addr )

		// RAM
		int result = RAM [addr];
	int reg = addr - 0xF0;
	if ( reg >= 0 ) // 40%
	{
		reg -= 0x10;
		if ( (unsigned) reg >= 0xFF00 ) // 21%
		{
			reg += 0x10 - r_t0out;

			// Timers
			if ( (unsigned) reg < timer_count ) // 90%
			{
				Timer* t = &m.timers [reg];
				if ( time >= t->next_time )
					t = run_timer_( t, time );

				result = t->counter;
				t->counter = 0;
			}
			// Other registers
			else if ( reg < 0 ) // 10%
			{
				//result = cpu_read_smp_reg( reg + r_t0out, time );
				reg = reg + r_t0out;
				result = REGS_IN [reg];
				reg -= r_dspaddr;
				// DSP addr and data
				if ( (unsigned) reg <= 1 ) // 4% 0xF2 and 0xF3
				{
					result = REGS [r_dspaddr];
					if ( (unsigned) reg == 1 )
					{
						// DSP Read (0xF3)
						RUN_DSP( time, reg_times [REGS [r_dspaddr] & 0x7F] );

						result = dsp.m.regs[REGS [r_dspaddr] & 0x7F];

#ifdef SPC_DSP_READ_HOOK
						SPC_DSP_READ_HOOK( spc_time + time, (REGS [r_dspaddr] & 0x7F), result );
#endif
						// End of DSP Read
					}
				}
			}
			else // 1%
			{
				result = cpu_read( reg + (r_t0out + 0xF0 - 0x10000), time );
			}
		}
	}

	return result;
}


//// Run

// Prefix and suffix for CPU emulator function
#define SPC_CPU_RUN_FUNC \
BOOST::uint8_t* SNES_SPC::run_until_( time_t end_time )\
{\
	rel_time_t rel_time = m.spc_time - end_time;\
	m.spc_time = end_time;\
	m.dsp_time += rel_time;\
	m.timers [0].next_time += rel_time;\
	m.timers [1].next_time += rel_time;\
	m.timers [2].next_time += rel_time;

#define SPC_CPU_RUN_FUNC_END \
	m.spc_time += rel_time;\
	m.dsp_time -= rel_time;\
	m.timers [0].next_time -= rel_time;\
	m.timers [1].next_time -= rel_time;\
	m.timers [2].next_time -= rel_time;\
	return &REGS [r_cpuio0];\
}

//int const cpu_lag_max = 12 - 1; // DIV YA,X takes 12 clocks

void SNES_SPC::end_frame( time_t end_time )
{
	// Catch CPU up to as close to end as possible. If final instruction
	// would exceed end, does NOT execute it and leaves m.spc_time < end.
	if ( end_time > m.spc_time )
		run_until_( end_time );

	m.spc_time     -= end_time;
	m.extra_clocks += end_time;

	// Greatest number of clocks early that emulation can stop early due to
	// not being able to execute current instruction without going over
	// allowed time.

	// Catch timers up to CPU
	run_timer( &m.timers [0], 0 );
	run_timer( &m.timers [1], 0 );
	run_timer( &m.timers [2], 0 );

	// Catch DSP up to CPU
	if ( m.dsp_time < 0 )
	{
		RUN_DSP( 0, max_reg_time );
	}

	// Save any extra samples beyond what should be generated
	if ( m.buf_begin )
	{
		//save_extra();
		// Get end pointers
		sample_t const* main_end = m.buf_end;     // end of data written to buf
		sample_t const* dsp_end  = dsp.m.out; // end of data written to dsp.extra()
		if ( m.buf_begin <= dsp_end && dsp_end <= main_end )
		{
			main_end = dsp_end;
			dsp_end  = dsp.extra(); // nothing in DSP's extra
		}

		// Copy any extra samples at these ends into extra_buf
		sample_t* out = m.extra_buf;
		sample_t const* in;
		for ( in = m.buf_begin + ((m.extra_clocks >> 5) << 1); in < main_end; in++ )
			*out++ = *in;
		for ( in = dsp.extra(); in < dsp_end ; in++ )
			*out++ = *in;

		m.extra_pos = out;
	}
}

// Inclusion here allows static memory access functions and better optimization
#include "SPC_CPU.h"

#if !SPC_NO_COPY_STATE_FUNCS

#define RAM         (m.ram.ram)
#define REGS        (m.smp_regs [0])
#define REGS_IN     (m.smp_regs [1])

void SNES_SPC::save_regs( uint8_t out [reg_count] )
{
	// Use current timer counter values
	out [r_t0out + 0] = m.timers [0].counter;
	out [r_t0out + 1] = m.timers [1].counter;
	out [r_t0out + 2] = m.timers [2].counter;

	// Last written values
	__builtin_memcpy( out, REGS, r_t0out );
}

void SNES_SPC::init_header( void* spc_out )
{
	spc_file_t* const spc = (spc_file_t*) spc_out;

	spc->has_id666 = 26; // has none
	spc->version   = 30;
	__builtin_memcpy( spc, signature, sizeof spc->signature );
	__builtin_memset( spc->text, 0, sizeof spc->text );
}

void SNES_SPC::save_spc( void* spc_out )
{
	spc_file_t* const spc = (spc_file_t*) spc_out;

	// CPU
	spc->pcl = (uint8_t) (m.cpu_regs.pc >> 0);
	spc->pch = (uint8_t) (m.cpu_regs.pc >> 8);
	spc->a   = m.cpu_regs.a;
	spc->x   = m.cpu_regs.x;
	spc->y   = m.cpu_regs.y;
	spc->psw = m.cpu_regs.psw;
	spc->sp  = m.cpu_regs.sp;

	// RAM, ROM
	__builtin_memcpy( spc->ram, RAM, sizeof spc->ram );

	if ( m.rom_enabled )
		__builtin_memcpy( spc->ram + rom_addr, m.hi_ram, sizeof m.hi_ram );

	__builtin_memset( spc->unused, 0, sizeof spc->unused );
	__builtin_memcpy( spc->ipl_rom, m.rom, sizeof spc->ipl_rom );

	// SMP registers
	save_regs( &spc->ram [0xF0] );
	int i;
	spc->ram [0xF0 + r_cpuio0] = REGS_IN [r_cpuio0 + 0];
	spc->ram [0xF0 + r_cpuio0 + 1] = REGS_IN [r_cpuio0 + 1];
	spc->ram [0xF0 + r_cpuio0 + 2] = REGS_IN [r_cpuio0 + 2];
	spc->ram [0xF0 + r_cpuio0 + 3] = REGS_IN [r_cpuio0 + 3];

	// DSP registers
	for ( i = 0; i < register_count; i++ )
		spc->dsp [i] = dsp.m.regs[i];
}

#undef IF_0_THEN_256
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)
void SNES_SPC::copy_state( unsigned char** io, copy_func_t copy )
{
	SPC_State_Copier copier( io, copy );

	// Make state data more readable by putting 64K RAM, 16 SMP registers,
	// then DSP (with its 128 registers) first

	// RAM
	//enable_rom( 0 ); // will get re-enabled if necessary in regs_loaded() below
	if ( m.rom_enabled != 0 )
	{
		m.rom_enabled = dsp.rom_enabled = 0;
		__builtin_memcpy( &RAM [rom_addr], m.hi_ram, rom_size );
		// TODO: ROM can still get overwritten when DSP writes to echo buffer
	}

	copier.copy( RAM, 0x10000 );

	{
		// SMP registers
		uint8_t regs [reg_count];
		uint8_t regs_in [reg_count];

		__builtin_memcpy( regs, REGS, reg_count );
		__builtin_memcpy( regs_in, REGS_IN, reg_count );

		copier.copy( regs, sizeof regs );
		copier.copy( regs_in, sizeof regs_in );

		__builtin_memcpy( REGS, regs, reg_count);
		__builtin_memcpy( REGS_IN, regs_in, reg_count );

		//enable_rom( REGS [r_control] & 0x80 );
		int enable = REGS [r_control] & 0x80;
		if ( m.rom_enabled != enable )
		{
			m.rom_enabled = dsp.rom_enabled = enable;

			if ( enable )
				__builtin_memcpy( m.hi_ram, &RAM [rom_addr], sizeof m.hi_ram );

			__builtin_memcpy( &RAM [rom_addr], (enable ? m.rom : m.hi_ram), rom_size );
			// TODO: ROM can still get overwritten when DSP writes to echo buffer
		}
	}

	// CPU registers
	SPC_COPY( uint16_t, m.cpu_regs.pc );
	SPC_COPY(  uint8_t, m.cpu_regs.a );
	SPC_COPY(  uint8_t, m.cpu_regs.x );
	SPC_COPY(  uint8_t, m.cpu_regs.y );
	SPC_COPY(  uint8_t, m.cpu_regs.psw );
	SPC_COPY(  uint8_t, m.cpu_regs.sp );
	copier.extra();

	SPC_COPY( int16_t, m.spc_time );
	SPC_COPY( int16_t, m.dsp_time );

	// DSP
	dsp.copy_state( io, copy );

	// Timers
	Timer* timer = &m.timers [0];
	timer->period  = IF_0_THEN_256( REGS [r_t0target] );
	timer->enabled = REGS [r_control];
	SPC_COPY( int16_t, timer->next_time );
	SPC_COPY( uint8_t, timer->divider );
	SPC_COPY( uint8_t, timer->counter );
	copier.extra();

	timer = &m.timers [1];
	timer->period  = IF_0_THEN_256( REGS [r_t0target + 1] );
	timer->enabled = REGS [r_control] >> 1 & 1;
	SPC_COPY( int16_t, timer->next_time );
	SPC_COPY( uint8_t, timer->divider );
	SPC_COPY( uint8_t, timer->counter );
	copier.extra();

	timer = &m.timers [2];
	timer->period  = IF_0_THEN_256( REGS [r_t0target + 2] );
	timer->enabled = REGS [r_control] >> 2 & 1;
	SPC_COPY( int16_t, timer->next_time );
	SPC_COPY( uint8_t, timer->divider );
	SPC_COPY( uint8_t, timer->counter );
	copier.extra();

	//set_tempo( m.tempo );
	int t = m.tempo;
	int const timer2_shift = 4; // 64 kHz
	int const other_shift  = 3; //  8 kHz

#if SPC_DISABLE_TEMPO
	m.timers [2].prescaler = timer2_shift;
	m.timers [1].prescaler = timer2_shift + other_shift;
	m.timers [0].prescaler = timer2_shift + other_shift;
#else
	if ( !t )
		t = 1;
	int const timer2_rate  = 1 << timer2_shift;
	int rate = (timer2_rate * tempo_unit + (t >> 1)) / t;
	if ( rate < (timer2_rate >> 2) )
		rate = timer2_rate >> 2; // max 4x tempo
	m.timers [2].prescaler = rate;
	m.timers [1].prescaler = rate << other_shift;
	m.timers [0].prescaler = rate << other_shift;
#endif

	copier.extra();
}
#endif

// (n ? n : 256)
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)

//// Init

blargg_err_t SNES_SPC::init()
{
	__builtin_memset( &m, 0, sizeof m );
	dsp.init( RAM );

	m.tempo = tempo_unit;

	// Most SPC music doesn't need ROM, and almost all the rest only rely
	// on these two bytes
	m.rom [0x3E] = 0xFF;
	m.rom [0x3F] = 0xC0;

	static unsigned char const cycle_table [128] =
	{//   01   23   45   67   89   AB   CD   EF
		0x28,0x47,0x34,0x36,0x26,0x54,0x54,0x68, // 0
		0x48,0x47,0x45,0x56,0x55,0x65,0x22,0x46, // 1
		0x28,0x47,0x34,0x36,0x26,0x54,0x54,0x74, // 2
		0x48,0x47,0x45,0x56,0x55,0x65,0x22,0x38, // 3
		0x28,0x47,0x34,0x36,0x26,0x44,0x54,0x66, // 4
		0x48,0x47,0x45,0x56,0x55,0x45,0x22,0x43, // 5
		0x28,0x47,0x34,0x36,0x26,0x44,0x54,0x75, // 6
		0x48,0x47,0x45,0x56,0x55,0x55,0x22,0x36, // 7
		0x28,0x47,0x34,0x36,0x26,0x54,0x52,0x45, // 8
		0x48,0x47,0x45,0x56,0x55,0x55,0x22,0xC5, // 9
		0x38,0x47,0x34,0x36,0x26,0x44,0x52,0x44, // A
		0x48,0x47,0x45,0x56,0x55,0x55,0x22,0x34, // B
		0x38,0x47,0x45,0x47,0x25,0x64,0x52,0x49, // C
		0x48,0x47,0x56,0x67,0x45,0x55,0x22,0x83, // D
		0x28,0x47,0x34,0x36,0x24,0x53,0x43,0x40, // E
		0x48,0x47,0x45,0x56,0x34,0x54,0x22,0x60, // F
	};

	// unpack cycle table
	for ( int i = 0; i < 128; i++ )
	{
		int n = cycle_table [i];
		m.cycle_table [i * 2 + 0] = n >> 4;
		m.cycle_table [i * 2 + 1] = n & 0x0F;
	}

	allow_time_overflow = false;

	dsp.rom = m.rom;
	dsp.hi_ram = m.hi_ram;

#if SPC_LESS_ACCURATE
	__builtin_memcpy( reg_times, reg_times_, sizeof reg_times );
#endif

	reset();
	return 0;
}

void SNES_SPC::init_rom( uint8_t const in [rom_size] )
{
	__builtin_memcpy( m.rom, in, sizeof m.rom );
}

void SNES_SPC::set_tempo( int t )
{
	m.tempo = t;
	int const timer2_shift = 4; // 64 kHz
	int const other_shift  = 3; //  8 kHz

#if SPC_DISABLE_TEMPO
	m.timers [2].prescaler = timer2_shift;
	m.timers [1].prescaler = timer2_shift + other_shift;
	m.timers [0].prescaler = timer2_shift + other_shift;
#else
	if ( !t )
		t = 1;
	int const timer2_rate  = 1 << timer2_shift;
	int rate = (timer2_rate * tempo_unit + (t >> 1)) / t;
	if ( rate < (timer2_rate >> 2) )
		rate = timer2_rate >> 2; // max 4x tempo
	m.timers [2].prescaler = rate;
	m.timers [1].prescaler = rate << other_shift;
	m.timers [0].prescaler = rate << other_shift;
#endif
}

// Timer registers have been loaded. Applies these to the timers. Does not
// reset timer prescalers or dividers.
void SNES_SPC::timers_loaded()
{
	Timer* timer = &m.timers [0];
	timer->period  = IF_0_THEN_256( REGS [r_t0target] );
	timer->enabled = REGS [r_control] >> 0 & 1;
	timer->counter = REGS_IN [r_t0out + 0] & 0x0F;

	timer = &m.timers [1];
	timer->period  = IF_0_THEN_256( REGS [r_t0target + 1] );
	timer->enabled = REGS [r_control] >> 1 & 1;
	timer->counter = REGS_IN [r_t0out + 1] & 0x0F;

	timer = &m.timers [2];
	timer->period  = IF_0_THEN_256( REGS [r_t0target + 2] );
	timer->enabled = REGS [r_control] >> 2 & 1;
	timer->counter = REGS_IN [r_t0out + 2] & 0x0F;

	//set_tempo( m.tempo );
	int t = m.tempo;
	int const timer2_shift = 4; // 64 kHz
	int const other_shift  = 3; //  8 kHz

#if SPC_DISABLE_TEMPO
	m.timers [2].prescaler = timer2_shift;
	m.timers [1].prescaler = timer2_shift + other_shift;
	m.timers [0].prescaler = timer2_shift + other_shift;
#else
	if ( !t )
		t = 1;
	int const timer2_rate  = 1 << timer2_shift;
	int rate = (timer2_rate * tempo_unit + (t >> 1)) / t;
	if ( rate < (timer2_rate >> 2) )
		rate = timer2_rate >> 2; // max 4x tempo
	m.timers [2].prescaler = rate;
	m.timers [1].prescaler = rate << other_shift;
	m.timers [0].prescaler = rate << other_shift;
#endif
}

// RAM was just loaded from SPC, with $F0-$FF containing SMP registers
// and timer counts. Copies these to proper registers.
void SNES_SPC::ram_loaded()
{
	m.rom_enabled = dsp.rom_enabled = 0;

	//load_regs( &RAM [0xF0] );
	__builtin_memcpy( REGS, &RAM[0xF0], reg_count );
	__builtin_memcpy( REGS_IN, REGS, reg_count );

	// These always read back as 0
	REGS_IN [r_test    ] = 0;
	REGS_IN [r_control ] = 0;
	REGS_IN [r_t0target] = 0;
	REGS_IN [r_t1target] = 0;
	REGS_IN [r_t2target] = 0;

	// Put STOP instruction around memory to catch PC underflow/overflow
	__builtin_memset( m.ram.padding1, cpu_pad_fill, sizeof m.ram.padding1 );
	__builtin_memset( m.ram.padding2, cpu_pad_fill, sizeof m.ram.padding2 );
}

void SNES_SPC::reset_time_regs()
{
	m.cpu_error     = 0;
	m.echo_accessed = 0;
	m.spc_time      = 0;
	m.dsp_time      = 0;
#if SPC_LESS_ACCURATE
	m.dsp_time = 31;
#endif

	Timer* timer = &m.timers [0];
	timer->next_time = 1;
	timer->divider   = 0;

	timer = &m.timers [1];
	timer->next_time = 1;
	timer->divider   = 0;

	timer = &m.timers [2];
	timer->next_time = 1;
	timer->divider   = 0;

	//regs_loaded();
	// Registers were just loaded. Applies these new values.
	//enable_rom( REGS [r_control] & 0x80 );
	int enable = REGS [r_control] & 0x80;
	if ( m.rom_enabled != enable )
	{
		m.rom_enabled = dsp.rom_enabled = enable;

		if ( enable )
			__builtin_memcpy( m.hi_ram, &RAM [rom_addr], sizeof m.hi_ram );

		__builtin_memcpy( &RAM [rom_addr], (enable ? m.rom : m.hi_ram), rom_size );
		// TODO: ROM can still get overwritten when DSP writes to echo buffer
	}

	timers_loaded();

	m.extra_clocks = 0;

	//reset_buf();
	// Start with half extra buffer of silence
	sample_t* out = m.extra_buf;
	do{
		*out++ = 0;
	}while ( out < &m.extra_buf [(extra_size >> 1)] );

	m.extra_pos = out;
	m.buf_begin = 0;

	dsp.set_output( 0, 0 );
}

#define reset_common(number) \
   REGS_IN [r_t0out + 0] = number; \
   REGS_IN [r_t0out + 1] = number; \
   REGS_IN [r_t0out + 2] = number; \
	/* Run IPL ROM */ \
	__builtin_memset( &m.cpu_regs, 0, sizeof m.cpu_regs ); \
	m.cpu_regs.pc = rom_addr; \
	\
	REGS [r_test   ] = 0x0A; \
	REGS [r_control] = 0xB0; /* ROM enabled, clear ports */ \
	REGS_IN [r_cpuio0] = 0; \
	REGS_IN [r_cpuio0 + 1] = 0; \
	REGS_IN [r_cpuio0 + 2] = 0; \
	REGS_IN [r_cpuio0 + 3] = 0;
      

void SNES_SPC::soft_reset()
{
	reset_common( 0 );
	
	reset_time_regs();
	dsp.soft_reset();
}

void SNES_SPC::reset()
{
	m.cpu_regs.pc  = 0xFFC0;
	m.cpu_regs.a   = 0x00;
	m.cpu_regs.x   = 0x00;
	m.cpu_regs.y   = 0x00;
	m.cpu_regs.psw = 0x02;
	m.cpu_regs.sp  = 0xEF;
	__builtin_memset( RAM, 0x00, 0x10000 );
	ram_loaded();

	reset_common( 0x0F );
	
	reset_time_regs();
	dsp.reset();
}

char const SNES_SPC::signature [signature_size + 1] =
		"SNES-SPC700 Sound File Data v0.30\x1A\x1A";

//// Sample output

void SNES_SPC::set_output( sample_t* out, int size )
{
	//require( (size & 1) == 0 ); // size must be even

	//m.extra_clocks &= clocks_per_sample - 1;
	m.extra_clocks &= 31;
#if 0
	if ( out )
	{
#endif
		sample_t const* out_end = out + size;
		m.buf_begin = out;
		m.buf_end   = out_end;

		// Copy extra to output
		sample_t const* in = m.extra_buf;
		do{
			*out++ = *in++;
		}while ( in < m.extra_pos && out < out_end );

		// Handle output being full already
		if ( out >= out_end )
		{
			// Have DSP write to remaining extra space
			out     = dsp.extra();
			out_end = &dsp.extra() [extra_size];

			// Copy any remaining extra samples as if DSP wrote them
			while ( in < m.extra_pos )
				*out++ = *in++;
		}

		dsp.set_output( out, out_end - out );
#if 0
	}
	else
	{
		//reset_buf();
		// Start with half extra buffer of silence
		sample_t* out = m.extra_buf;
		do{
			*out++ = 0;
		}while ( out < &m.extra_buf [(extra_size >> 1)] );

		m.extra_pos = out;
		m.buf_begin = 0;

		dsp.set_output( 0, 0 );
	}
#endif
}

//// Snes9x Accessor

void SNES_SPC::dsp_set_spc_snapshot_callback( void (*callback) (void) )
{
	dsp.set_spc_snapshot_callback( callback );
}

void SNES_SPC::dsp_dump_spc_snapshot( void )
{
	dsp.dump_spc_snapshot();
}

#if 0
void SNES_SPC::dsp_set_stereo_switch( int value )
{
	dsp.set_stereo_switch( value );
}
#endif

//SPC_DSP

// TODO: add to blargg_endian.h
#define GET_LE16SA( addr )      ((BOOST::int16_t) GET_LE16( addr ))
#define GET_LE16A( addr )       GET_LE16( addr )
#define SET_LE16A( addr, data ) SET_LE16( addr, data )

static BOOST::uint8_t const initial_regs [register_count] =
{
	0x45,0x8B,0x5A,0x9A,0xE4,0x82,0x1B,0x78,0x00,0x00,0xAA,0x96,0x89,0x0E,0xE0,0x80,
	0x2A,0x49,0x3D,0xBA,0x14,0xA0,0xAC,0xC5,0x00,0x00,0x51,0xBB,0x9C,0x4E,0x7B,0xFF,
	0xF4,0xFD,0x57,0x32,0x37,0xD9,0x42,0x22,0x00,0x00,0x5B,0x3C,0x9F,0x1B,0x87,0x9A,
	0x6F,0x27,0xAF,0x7B,0xE5,0x68,0x0A,0xD9,0x00,0x00,0x9A,0xC5,0x9C,0x4E,0x7B,0xFF,
	0xEA,0x21,0x78,0x4F,0xDD,0xED,0x24,0x14,0x00,0x00,0x77,0xB1,0xD1,0x36,0xC1,0x67,
	0x52,0x57,0x46,0x3D,0x59,0xF4,0x87,0xA4,0x00,0x00,0x7E,0x44,0x00,0x4E,0x7B,0xFF,
	0x75,0xF5,0x06,0x97,0x10,0xC3,0x24,0xBB,0x00,0x00,0x7B,0x7A,0xE0,0x60,0x12,0x0F,
	0xF7,0x74,0x1C,0xE5,0x39,0x3D,0x73,0xC1,0x00,0x00,0x7A,0xB3,0xFF,0x4E,0x7B,0xFF
};

// if ( io < -32768 ) io = -32768;
// if ( io >  32767 ) io =  32767;
#define CLAMP16( io )\
{\
	if ( (int16_t) io != io )\
		io = (io >> 31) ^ 0x7FFF;\
}

// Access global DSP register
#define REG(n)      m.regs [r_##n]

// Access voice DSP register
#define VREG(r,n)   r [v_##n]

#define WRITE_SAMPLES( l, r, out ) \
{\
	out [0] = l;\
	out [1] = r;\
	out += 2;\
	if ( out >= m.out_end )\
	{\
		out       = m.extra;\
		m.out_end = &m.extra [extra_size];\
	}\
}\

void SPC_DSP::set_output( sample_t* out, int size )
{
   #if 0
	if ( !out )
	{
		out  = m.extra;
		size = extra_size;
	}
   #endif
	m.out_begin = out;
	m.out       = out;
	m.out_end   = out + size;
}

// Volume registers and efb are signed! Easy to forget int8_t cast.
// Prefixes are to avoid accidental use of locals with same names.

// Gaussian interpolation

static short const gauss [512] =
{
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
   2,   2,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   5,   5,   5,   5,
   6,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,  10,  10,  10,
  11,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  15,  16,  16,  17,  17,
  18,  19,  19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  26,  27,  27,
  28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  36,  36,  37,  38,  39,  40,
  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,
  58,  59,  60,  61,  62,  64,  65,  66,  67,  69,  70,  71,  73,  74,  76,  77,
  78,  80,  81,  83,  84,  86,  87,  89,  90,  92,  94,  95,  97,  99, 100, 102,
 104, 106, 107, 109, 111, 113, 115, 117, 118, 120, 122, 124, 126, 128, 130, 132,
 134, 137, 139, 141, 143, 145, 147, 150, 152, 154, 156, 159, 161, 163, 166, 168,
 171, 173, 175, 178, 180, 183, 186, 188, 191, 193, 196, 199, 201, 204, 207, 210,
 212, 215, 218, 221, 224, 227, 230, 233, 236, 239, 242, 245, 248, 251, 254, 257,
 260, 263, 267, 270, 273, 276, 280, 283, 286, 290, 293, 297, 300, 304, 307, 311,
 314, 318, 321, 325, 328, 332, 336, 339, 343, 347, 351, 354, 358, 362, 366, 370,
 374, 378, 381, 385, 389, 393, 397, 401, 405, 410, 414, 418, 422, 426, 430, 434,
 439, 443, 447, 451, 456, 460, 464, 469, 473, 477, 482, 486, 491, 495, 499, 504,
 508, 513, 517, 522, 527, 531, 536, 540, 545, 550, 554, 559, 563, 568, 573, 577,
 582, 587, 592, 596, 601, 606, 611, 615, 620, 625, 630, 635, 640, 644, 649, 654,
 659, 664, 669, 674, 678, 683, 688, 693, 698, 703, 708, 713, 718, 723, 728, 732,
 737, 742, 747, 752, 757, 762, 767, 772, 777, 782, 787, 792, 797, 802, 806, 811,
 816, 821, 826, 831, 836, 841, 846, 851, 855, 860, 865, 870, 875, 880, 884, 889,
 894, 899, 904, 908, 913, 918, 923, 927, 932, 937, 941, 946, 951, 955, 960, 965,
 969, 974, 978, 983, 988, 992, 997,1001,1005,1010,1014,1019,1023,1027,1032,1036,
1040,1045,1049,1053,1057,1061,1066,1070,1074,1078,1082,1086,1090,1094,1098,1102,
1106,1109,1113,1117,1121,1125,1128,1132,1136,1139,1143,1146,1150,1153,1157,1160,
1164,1167,1170,1174,1177,1180,1183,1186,1190,1193,1196,1199,1202,1205,1207,1210,
1213,1216,1219,1221,1224,1227,1229,1232,1234,1237,1239,1241,1244,1246,1248,1251,
1253,1255,1257,1259,1261,1263,1265,1267,1269,1270,1272,1274,1275,1277,1279,1280,
1282,1283,1284,1286,1287,1288,1290,1291,1292,1293,1294,1295,1296,1297,1297,1298,
1299,1300,1300,1301,1302,1302,1303,1303,1303,1304,1304,1304,1304,1304,1305,1305,
};

inline int SPC_DSP::interpolate( voice_t const* v )
{
	// Make pointers into gaussian based on fractional position between samples
	int offset = v->interp_pos >> 4 & 0xFF;
	short const* fwd = gauss + 255 - offset;
	short const* rev = gauss       + offset; // mirror left half of gaussian
	
	int const* in = &v->buf [(v->interp_pos >> 12) + v->buf_pos];
	int out;
	out  = (fwd [  0] * in [0]) >> 11;
	out += (fwd [256] * in [1]) >> 11;
	out += (rev [256] * in [2]) >> 11;
	out = (int16_t) out;
	out += (rev [  0] * in [3]) >> 11;
	
	CLAMP16( out );
	out &= ~1;
	return out;
}


//// Counters

int const simple_counter_range = 30720; // 30720

static unsigned const counter_rates [32] =
{
   simple_counter_range + 1, // never fires
          2048, 1536,
	1280, 1024,  768,
	 640,  512,  384,
	 320,  256,  192,
	 160,  128,   96,
	  80,   64,   48,
	  40,   32,   24,
	  20,   16,   12,
	  10,    8,    6,
	   5,    4,    3,
	         2,
	         1
};

static unsigned const counter_offsets [32] =
{
	  1, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	     0,
	     0
};

#define init_counter() m.counter = 0;

#define run_counters() \
	if ( --m.counter < 0 ) \
		m.counter = simple_counter_range - 1;

inline unsigned SPC_DSP::read_counter( int rate )
{
	return ((unsigned) m.counter + counter_offsets [rate]) % counter_rates [rate];
}


//// Envelope

inline void SPC_DSP::run_envelope( voice_t* const v )
{
	int env = v->env;
	if ( v->env_mode == env_release ) // 60%
	{
		if ( (env -= 0x8) < 0 )
			env = 0;
		v->env = env;
	}
	else
	{
		int rate;
		int env_data = VREG(v->regs,adsr1);
		if ( m.t_adsr0 & 0x80 ) // 99% ADSR
		{
			if ( v->env_mode >= env_decay ) // 99%
			{
				env--;
				env -= env >> 8;
				rate = env_data & 0x1F;
				if ( v->env_mode == env_decay ) // 1%
					rate = (m.t_adsr0 >> 3 & 0x0E) + 0x10;
			}
			else // env_attack
			{
				rate = ((m.t_adsr0 & 0x0F) << 1) + 1;
				//env += rate < 31 ? 0x20 : 0x400;
				ISEL_IF32(env,rate < 31,env + 0x20,env + 0x400);
			}
		}
		else // GAIN
		{
			int mode;
			env_data = VREG(v->regs,gain);
			mode = env_data >> 5;
			if ( mode < 4 ) // direct
			{
				env = env_data << 4;
				rate = 31;
			}
			else
			{
				rate = env_data & 0x1F;
				if ( mode == 4 ) // 4: linear decrease
				{
					env -= 0x20;
				}
				else if ( mode < 6 ) // 5: exponential decrease
				{
					env--;
					env -= env >> 8;
				}
				else // 6,7: linear increase
				{
					env += 0x20;
					if ( mode > 6 && (unsigned) v->hidden_env >= 0x600 )
						env += 0x8 - 0x20; // 7: two-slope linear increase
				}
			}
		}

		// Sustain level
		if ( (env >> 8) == (env_data >> 5) && v->env_mode == env_decay )
			v->env_mode = env_sustain;

		v->hidden_env = env;

		// unsigned cast because linear decrease going negative also triggers this
		if ( (unsigned) env > 0x7FF )
		{
			//env = (env < 0 ? 0 : 0x7FF);
			ISEL_IF32(env,env < 0,0,0x7FF);
			if ( v->env_mode == env_attack )
				v->env_mode = env_decay;
		}

		if ( !read_counter( rate ) )
			v->env = env; // nothing else is controlled by the counter
	}
}


//// BRR Decoding

inline void SPC_DSP::decode_brr( voice_t* v )
{
	// Arrange the four input nybbles in 0xABCD order for easy decoding
	int nybbles = (m.t_brr_byte << 8) + m.ram [(v->brr_addr + v->brr_offset + 1) & 0xFFFF];

	int const header = m.t_brr_header;

	// Write to next four samples in circular buffer
	int* pos = &v->buf [v->buf_pos];
	int* end;
	if ( (v->buf_pos += 4) >= brr_buf_size )
		v->buf_pos = 0;

	// Decode four samples
	for ( end = pos + 4; pos < end; pos++, nybbles <<= 4 )
	{
		// Extract nybble and sign-extend
		int s = (int16_t) nybbles >> 12;

		// Shift sample based on header
		int const shift = header >> 4;
		s = (s << shift) >> 1;
		if ( shift >= 0xD ) // handle invalid range
			s = (s >> 25) << 11; // same as: s = (s < 0 ? -0x800 : 0)

		// Apply IIR filter (8 is the most commonly used)
		int const filter = header & 0x0C;
		int const p1 = pos [brr_buf_size - 1];
		int const p2 = pos [brr_buf_size - 2] >> 1;
		int32_t condition_filter_gt_8 = (filter >= 8);
		if ( condition_filter_gt_8 )
		{
			s += p1;
			s -= p2;
			int32_t condition = (filter == 8);
			int32_t mask = ((condition) | -(condition)) >> 31;
			int32_t result1 = ((p2 >> 4) & mask) | (((p1 * -13) >> 7) & ~mask);
			s += result1;
			int32_t result2 = (((p1 * -3) >> 6) & mask) | (((3 * p2) >> 4) & ~mask);
			s += result2;
			/*
			   if ( filter == 8 ) // s += p1 * 0.953125 - p2 * 0.46875
			   {
			   s += p2 >> 4;
			   s += (p1 * -3) >> 6;
			   }
			   else // s += p1 * 0.8984375 - p2 * 0.40625
			   {
			   s += (p1 * -13) >> 7;
			   s += (3 * p2) >> 4;
			   }
			 */
		}
		int32_t condition_filter_eq_1 = ((filter != 0) && !condition_filter_gt_8);
		int32_t mask_filter_eq_1 = ((condition_filter_eq_1) | -(condition_filter_eq_1) >> 31);
		int32_t result1 = ((p1 >> 1) & mask_filter_eq_1) | ((0) & ~mask_filter_eq_1);
		s += result1;
		int32_t result2 = (((-p1) >> 5) & mask_filter_eq_1) | ((0) & ~mask_filter_eq_1);
		s += result2;
		/*
		   else if ( filter ) // s += p1 * 0.46875
		   {
		   s += p1 >> 1;
		   s += (-p1) >> 5;
		   }
		 */

		// Adjust and write sample
		CLAMP16( s );
		s = (int16_t) (s << 1);
		pos [brr_buf_size] = pos [0] = s; // second copy simplifies wrap-around
	}
}


//// Misc

#define MISC_CLOCK( n ) inline void SPC_DSP::misc_##n()

MISC_CLOCK( 27 )
{
	m.t_pmon = REG(pmon) & 0xFE; // voice 0 doesn't support PMON
}
MISC_CLOCK( 28 )
{
	m.t_non = REG(non);
	m.t_eon = REG(eon);
	m.t_dir = REG(dir);
}
MISC_CLOCK( 29 )
{
	if ( (m.every_other_sample ^= 1) != 0 )
		m.new_kon &= ~m.kon; // clears KON 63 clocks after it was last read
}
MISC_CLOCK( 30 )
{
	int32_t condition = m.every_other_sample;
	int32_t mask = ((condition) | -(condition)) >> 31;
	uint32_t result_mkon = (((m.new_kon) & mask) | ((m.kon) & ~mask));
	uint32_t result_mtkoff = (((REG(koff) | m.mute_mask) & mask) | ((m.t_koff) & ~mask));
	m.kon = result_mkon;
	m.t_koff = result_mtkoff;
#if 0
	if ( m.every_other_sample )
	{
		m.kon    = m.new_kon;
		m.t_koff = REG(koff) | m.mute_mask; 
	}
#endif

	run_counters();

	// Noise
	int32_t condition2 = (!read_counter( REG(flg) & 0x1F )); 
	int32_t mask2 = ((condition2) | -(condition2)) >> 31;
	int feedback = (m.noise << 13) ^ (m.noise << 14);
	uint32_t result_m_noise = ((((feedback & 0x4000) ^ (m.noise >> 1)) & mask2) | ((m.noise) & ~mask2));
	m.noise = result_m_noise;
#if 0
	if ( !read_counter( REG(flg) & 0x1F ) )
	{
		int feedback = (m.noise << 13) ^ (m.noise << 14);
		m.noise = (feedback & 0x4000) ^ (m.noise >> 1);
	}
#endif
}


//// Voices

#define VOICE_CLOCK( n ) void SPC_DSP::voice_##n( voice_t* const v )

inline VOICE_CLOCK( V1 )
{
	m.t_dir_addr = (m.t_dir << 8) + (m.t_srcn << 2);
	m.t_srcn = VREG(v->regs,srcn);
}
inline VOICE_CLOCK( V2 )
{
	// Read sample pointer (ignored if not needed)
	uint8_t const* entry = &m.ram [m.t_dir_addr];
	if ( !v->kon_delay )
		entry += 2;
	m.t_brr_next_addr = GET_LE16A( entry );

	m.t_adsr0 = VREG(v->regs,adsr0);

	// Read pitch, spread over two clocks
	m.t_pitch = VREG(v->regs,pitchl);
}
inline VOICE_CLOCK( V3a )
{
	m.t_pitch += (VREG(v->regs,pitchh) & 0x3F) << 8;
}
inline VOICE_CLOCK( V3b )
{
	// Read BRR header and byte
	m.t_brr_byte   = m.ram [(v->brr_addr + v->brr_offset) & 0xFFFF];
	m.t_brr_header = m.ram [v->brr_addr]; // brr_addr doesn't need masking
}
VOICE_CLOCK( V3c )
{
	// Pitch modulation using previous voice's output
	if ( m.t_pmon & v->vbit )
		m.t_pitch += ((m.t_output >> 5) * m.t_pitch) >> 10;

	if ( v->kon_delay )
	{
		// Get ready to start BRR decoding on next sample
		if ( v->kon_delay == 5 )
		{
			v->brr_addr    = m.t_brr_next_addr;
			v->brr_offset  = 1;
			v->buf_pos     = 0;
			m.t_brr_header = 0; // header is ignored on this sample
			m.kon_check    = true;

			if (take_spc_snapshot)
			{
				take_spc_snapshot = 0;
				if (spc_snapshot_callback)
					spc_snapshot_callback();
			}
		}

		// Envelope is never run during KON
		v->env        = 0;
		v->hidden_env = 0;

		// Disable BRR decoding until last three samples
		v->interp_pos = 0;
		if ( --v->kon_delay & 3 )
			v->interp_pos = 0x4000;

		// Pitch is never added during KON
		m.t_pitch = 0;
	}

	// Gaussian interpolation
	{
		int output = interpolate( v );

		// Noise
		if ( m.t_non & v->vbit )
			output = (int16_t) (m.noise << 1);

		// Apply envelope
		m.t_output = (output * v->env) >> 11 & ~1;
		v->t_envx_out = (uint8_t) (v->env >> 4);
	}

	// Immediate silence due to end of sample or soft reset
	if ( REG(flg) & 0x80 || (m.t_brr_header & 3) == 1 )
	{
		v->env_mode = env_release;
		v->env      = 0;
	}

	if ( m.every_other_sample )
	{
		// KOFF
		if ( m.t_koff & v->vbit )
			v->env_mode = env_release;

		// KON
		if ( m.kon & v->vbit )
		{
			v->kon_delay = 5;
			v->env_mode  = env_attack;
		}
	}

	// Run envelope for next sample
	if ( !v->kon_delay )
		run_envelope( v );
}

VOICE_CLOCK( V4 )
{
	// Decode BRR
	m.t_looped = 0;
	if ( v->interp_pos >= 0x4000 )
	{
		decode_brr( v );

		if ( (v->brr_offset += 2) >= brr_block_size )
		{
			// Start decoding next BRR block
			v->brr_addr = (v->brr_addr + brr_block_size) & 0xFFFF;
			if ( m.t_brr_header & 1 )
			{
				v->brr_addr = m.t_brr_next_addr;
				m.t_looped = v->vbit;
			}
			v->brr_offset = 1;
		}
	}

	// Apply pitch
	v->interp_pos = (v->interp_pos & 0x3FFF) + m.t_pitch;

	// Keep from getting too far ahead (when using pitch modulation)
	if ( v->interp_pos > 0x7FFF )
		v->interp_pos = 0x7FFF;

	// Output left
	//voice_output( v, 0 );
	// Apply left/right volume
	int amp = (m.t_output * (int8_t) VREG(v->regs,voll)) >> 7;
	//amp *= ((stereo_switch & (1 << (v->voice_number))) ? 1 : 0);

	// Add to output total
	m.t_main_out [0] += amp;
	CLAMP16( m.t_main_out [0] );

	// Optionally add to echo total
	if ( m.t_eon & v->vbit )
	{
		m.t_echo_out [0] += amp;
		CLAMP16( m.t_echo_out [0] );
	}
}
inline VOICE_CLOCK( V5 )
{
	// Output right
	//voice_output( v, 1 );
	// Apply left/right volume
	int amp = (m.t_output * (int8_t) VREG(v->regs,voll + 1)) >> 7;
	//amp *= ((stereo_switch & (1 << (v->voice_number + 8))) ? 1 : 0);

	// Add to output total
	m.t_main_out [1] += amp;
	CLAMP16( m.t_main_out [1] );

	// Optionally add to echo total
	if ( m.t_eon & v->vbit )
	{
		m.t_echo_out [1] += amp;
		CLAMP16( m.t_echo_out [1] );
	}

	// ENDX, OUTX, and ENVX won't update if you wrote to them 1-2 clocks earlier
	int endx_buf = REG(endx) | m.t_looped;

	// Clear bit in ENDX if KON just began
	if ( v->kon_delay == 5 )
		endx_buf &= ~v->vbit;
	m.endx_buf = (uint8_t) endx_buf;
}
inline VOICE_CLOCK( V6 )
{
	(void) v; // avoid compiler warning about unused v
	m.outx_buf = (uint8_t) (m.t_output >> 8);
}
inline VOICE_CLOCK( V7 )
{
	// Update ENDX
	REG(endx) = m.endx_buf;
	
	m.envx_buf = v->t_envx_out;
}
inline VOICE_CLOCK( V8 )
{
	// Update OUTX
	VREG(v->regs,outx) = m.outx_buf;
}
inline VOICE_CLOCK( V9 )
{
	// Update ENVX
	VREG(v->regs,envx) = m.envx_buf;
}

// Most voices do all these in one clock, so make a handy composite
inline VOICE_CLOCK( V3 )
{
	voice_V3a( v );
	voice_V3b( v );
	voice_V3c( v );
}

// Common combinations of voice steps on different voices. This greatly reduces
// code size and allows everything to be inlined in these functions.
VOICE_CLOCK(V7_V4_V1) { voice_V7(v); voice_V1(v+3); voice_V4(v+1); }
VOICE_CLOCK(V8_V5_V2) { voice_V8(v); voice_V5(v+1); voice_V2(v+2); }
VOICE_CLOCK(V9_V6_V3) { voice_V9(v); voice_V6(v+1); voice_V3(v+2); }


//// Echo

// Current echo buffer pointer for left/right channel
#define ECHO_PTR( ch )      (&m.ram [m.t_echo_ptr + (ch << 1)])

// Sample in echo history buffer, where 0 is the oldest
#define ECHO_FIR( i )       (m.echo_hist_pos [i])

// Calculate FIR point for left/right channel
#define CALC_FIR( i, ch )   ((ECHO_FIR( i + 1 ) [ch] * (int8_t) REG(fir + (i << 4))) >> 6)

#define ECHO_CLOCK( n ) inline void SPC_DSP::echo_##n()

ECHO_CLOCK( 22 )
{
	// History
	if ( ++m.echo_hist_pos >= &m.echo_hist [echo_hist_size] )
		m.echo_hist_pos = m.echo_hist;

	m.t_echo_ptr = ((m.t_esa << 8) + m.echo_offset) & 0xFFFF;

	int s;
	if ( m.t_echo_ptr >= 0xffc0 && rom_enabled )
		s = GET_LE16SA( &hi_ram [m.t_echo_ptr + 0 * 2 - 0xffc0] );
	else
		s = GET_LE16SA( ECHO_PTR( 0 ) );
	// second copy simplifies wrap-around handling
	ECHO_FIR( 0 ) [0] = ECHO_FIR( 8 ) [0] = s >> 1;

	// FIR (using l and r temporaries below helps compiler optimize)
	int l = CALC_FIR( 0, 0 );
	int r = CALC_FIR( 0, 1 );

	m.t_echo_in [0] = l;
	m.t_echo_in [1] = r;
}
ECHO_CLOCK( 23 )
{
	int l = CALC_FIR( 1, 0 ) + CALC_FIR( 2, 0 );
	int r = CALC_FIR( 1, 1 ) + CALC_FIR( 2, 1 );

	m.t_echo_in [0] += l;
	m.t_echo_in [1] += r;

	int s;
	if ( m.t_echo_ptr >= 0xffc0 && rom_enabled )
		s = GET_LE16SA( &hi_ram [m.t_echo_ptr + 2 - 0xffc0] );
	else
		s = GET_LE16SA( ECHO_PTR( 1 ) );
	// second copy simplifies wrap-around handling
	ECHO_FIR( 0 ) [1] = ECHO_FIR( 8 ) [1] = s >> 1;
}
ECHO_CLOCK( 24 )
{
	int l = CALC_FIR( 3, 0 ) + CALC_FIR( 4, 0 ) + CALC_FIR( 5, 0 );
	int r = CALC_FIR( 3, 1 ) + CALC_FIR( 4, 1 ) + CALC_FIR( 5, 1 );
	
	m.t_echo_in [0] += l;
	m.t_echo_in [1] += r;
}
ECHO_CLOCK( 25 )
{
	int l = m.t_echo_in [0] + CALC_FIR( 6, 0 );
	int r = m.t_echo_in [1] + CALC_FIR( 6, 1 );
	
	l = (int16_t) l;
	r = (int16_t) r;
	
	l += (int16_t) CALC_FIR( 7, 0 );
	r += (int16_t) CALC_FIR( 7, 1 );
	
	CLAMP16( l );
	CLAMP16( r );
	
	m.t_echo_in [0] = l & ~1;
	m.t_echo_in [1] = r & ~1;
}

ECHO_CLOCK( 26 )
{
	// Left output volumes
	// (save sample for next clock so we can output both together)
	int out = (int16_t) ((m.t_main_out [0] * (int8_t) REG(mvoll + (0 << 4))) >> 7) +
		(int16_t) ((m.t_echo_in [0] * (int8_t) REG(evoll + (0 << 4))) >> 7);
	CLAMP16( out );
	m.t_main_out [0] = out;
	//m.t_main_out [0] = echo_output( 0 );

	// Echo feedback
	int l = m.t_echo_out [0] + (int16_t) ((m.t_echo_in [0] * (int8_t) REG(efb)) >> 7);
	int r = m.t_echo_out [1] + (int16_t) ((m.t_echo_in [1] * (int8_t) REG(efb)) >> 7);

	CLAMP16( l );
	CLAMP16( r );

	m.t_echo_out [0] = l & ~1;
	m.t_echo_out [1] = r & ~1;
}
ECHO_CLOCK( 27 )
{
	// Output
	int l = m.t_main_out [0];

	int audio_out = (int16_t) ((m.t_main_out [1] * (int8_t) REG(mvoll + (1 << 4))) >> 7) +
		(int16_t) ((m.t_echo_in [1] * (int8_t) REG(evoll + (1 << 4))) >> 7);
	CLAMP16( audio_out );
	int r = audio_out;
	//int r = echo_output( 1 );

	m.t_main_out [0] = 0;
	m.t_main_out [1] = 0;

	// TODO: global muting isn't this simple (turns DAC on and off
	// or something, causing small ~37-sample pulse when first muted)
	if ( REG(flg) & 0x40 )
	{
		l = 0;
		r = 0;
	}

	// Output sample to DAC
#ifdef SPC_DSP_OUT_HOOK
	SPC_DSP_OUT_HOOK( l, r );
#else
	sample_t* out = m.out;
	WRITE_SAMPLES( l, r, out );
	m.out = out;
#endif
}
ECHO_CLOCK( 28 )
{
	m.t_echo_enabled = REG(flg);
}

ECHO_CLOCK( 29 )
{
	m.t_esa = REG(esa);

	if ( !m.echo_offset )
		m.echo_length = ((REG(edl) & 0x0F) << 11);

	m.echo_offset += 4;
	if ( m.echo_offset >= m.echo_length )
		m.echo_offset = 0;

	// Write left echo
	if ( !(m.t_echo_enabled & 0x20) )
	{
		if ( m.t_echo_ptr >= 0xffc0 && rom_enabled)
			SET_LE16A( &hi_ram[m.t_echo_ptr - 0xffc0], m.t_echo_out[0]);
		else
			SET_LE16A( ECHO_PTR( 0 ), m.t_echo_out[0]);
	}

	m.t_echo_out [0] = 0;

	m.t_echo_enabled = REG(flg);
}
ECHO_CLOCK( 30 )
{
	// Write right echo
	if ( !(m.t_echo_enabled & 0x20) )
	{
		if ( m.t_echo_ptr >= 0xffc0 && rom_enabled)
			SET_LE16A( &hi_ram[m.t_echo_ptr + 2 - 0xffc0], m.t_echo_out[1]);
		else
			SET_LE16A( ECHO_PTR( 1 ), m.t_echo_out[1]);
	}

	m.t_echo_out [1] = 0;
}


//// Timing

// Execute clock for a particular voice
#define V( clock, voice )   voice_##clock( &m.voices [voice] );

/* The most common sequence of clocks uses composite operations
for efficiency. For example, the following are equivalent to the
individual steps on the right:

V(V7_V4_V1,2) -> V(V7,2) V(V4,3) V(V1,5)
V(V8_V5_V2,2) -> V(V8,2) V(V5,3) V(V2,4)
V(V9_V6_V3,2) -> V(V9,2) V(V6,3) V(V3,4) */

// Voice      0      1      2      3      4      5      6      7
#define GEN_DSP_TIMING \
PHASE( 0)  V(V5,0)V(V2,1)\
PHASE( 1)  V(V6,0)V(V3,1)\
PHASE( 2)  V(V7_V4_V1,0)\
PHASE( 3)  V(V8_V5_V2,0)\
PHASE( 4)  V(V9_V6_V3,0)\
PHASE( 5)         V(V7_V4_V1,1)\
PHASE( 6)         V(V8_V5_V2,1)\
PHASE( 7)         V(V9_V6_V3,1)\
PHASE( 8)                V(V7_V4_V1,2)\
PHASE( 9)                V(V8_V5_V2,2)\
PHASE(10)                V(V9_V6_V3,2)\
PHASE(11)                       V(V7_V4_V1,3)\
PHASE(12)                       V(V8_V5_V2,3)\
PHASE(13)                       V(V9_V6_V3,3)\
PHASE(14)                              V(V7_V4_V1,4)\
PHASE(15)                              V(V8_V5_V2,4)\
PHASE(16)                              V(V9_V6_V3,4)\
PHASE(17)  V(V1,0)                            V(V7,5)V(V4,6)\
PHASE(18)                                     V(V8_V5_V2,5)\
PHASE(19)                                     V(V9_V6_V3,5)\
PHASE(20)         V(V1,1)                            V(V7,6)V(V4,7)\
PHASE(21)                                            V(V8,6)V(V5,7)  V(V2,0)  /* t_brr_next_addr order dependency */\
PHASE(22)  V(V3a,0)                                  V(V9,6)V(V6,7)  echo_22();\
PHASE(23)                                                   V(V7,7)  echo_23();\
PHASE(24)                                                   V(V8,7)  echo_24();\
PHASE(25)  V(V3b,0)                                         V(V9,7)  echo_25();\
PHASE(26)                                                            echo_26();\
PHASE(27) misc_27();                                                 echo_27();\
PHASE(28) misc_28();                                                 echo_28();\
PHASE(29) misc_29();                                                 echo_29();\
PHASE(30) misc_30();V(V3c,0)                                         echo_30();\
PHASE(31)  V(V4,0)       V(V1,2)\

#if !SPC_DSP_CUSTOM_RUN

void SPC_DSP::run( int clocks_remain )
{
	int const phase = m.phase;
	m.phase = (phase + clocks_remain) & 31;
	switch ( phase )
	{
loop:

#define PHASE( n ) if ( n && !--clocks_remain ) break; case n:
		GEN_DSP_TIMING
#undef PHASE

			if ( --clocks_remain )
				goto loop;
	}
}

#endif


//// Setup

void SPC_DSP::init( void* ram_64k )
{
	m.ram = (uint8_t*) ram_64k;
	//mute_voices( 0 );
	m.mute_mask = 0;
	//disable_surround( false );
	set_output( 0, 0 );
	reset();

	//stereo_switch = 0xffff;
	take_spc_snapshot = 0;
	spc_snapshot_callback = 0;

}

void SPC_DSP::soft_reset()
{
	REG(flg) = 0xE0;

	m.noise              = 0x4000;
	m.echo_hist_pos      = m.echo_hist;
	m.every_other_sample = 1;
	m.echo_offset        = 0;
	m.phase              = 0;
	
	init_counter();

#if 0
   #pragma loop unroll 4
	for (int i = 0; i < voice_count; i++)
		m.voices[i].voice_number = i;
#endif
}

void SPC_DSP::load( uint8_t const regs [register_count] )
{
	memcpy( m.regs, regs, sizeof m.regs );
	memset( &m.regs [register_count], 0, offsetof (state_t,ram) - register_count );

	// Internal state
	for ( int i = voice_count; --i >= 0; )
	{
		voice_t* v = &m.voices [i];
		v->brr_offset = 1;
		v->vbit       = 1 << i;
		v->regs       = &m.regs [i * 0x10];
	}
	m.new_kon = REG(kon);
	m.t_dir   = REG(dir);
	m.t_esa   = REG(esa);

	m.noise              = 0x4000;
	m.echo_hist_pos      = m.echo_hist;
	m.every_other_sample = 1;
	m.echo_offset        = 0;
	m.phase              = 0;

	init_counter();

#if 0
#pragma loop unroll 4
	for (int i = 0; i < voice_count; i++)
		m.voices[i].voice_number = i;
#endif
}

void SPC_DSP::reset() { load( initial_regs ); }


//// State save/load

#if !SPC_NO_COPY_STATE_FUNCS

void SPC_State_Copier::copy( void* state, size_t size )
{
	func( buf, state, size );
}

int SPC_State_Copier::copy_int( int state, int size )
{
	BOOST::uint8_t s [2];
	SET_LE16( s, state );
	func( buf, &s, size );
	return GET_LE16( s );
}

void SPC_State_Copier::skip( int count )
{
	if ( count > 0 )
	{
		char temp [64];
		memset( temp, 0, sizeof temp );
		do
		{
			int n = sizeof temp;
			if ( n > count )
				n = count;
			count -= n;
			func( buf, temp, n );
		}
		while ( count );
	}
}

void SPC_State_Copier::extra()
{
	int n = 0;
	SPC_State_Copier& copier = *this;
	SPC_COPY( uint8_t, n );
	skip( n );
}

void SPC_DSP::copy_state( unsigned char** io, copy_func_t copy )
{
	SPC_State_Copier copier( io, copy );

	// DSP registers
	copier.copy( m.regs, register_count );

	// Internal state

	// Voices
	int i;
#pragma loop unroll 4
	for ( i = 0; i < voice_count; i++ )
	{
		voice_t* v = &m.voices [i];

		// BRR buffer
		int i;
#pragma loop unroll 4
		for ( i = 0; i < brr_buf_size; i++ )
		{
			int s = v->buf [i];
			SPC_COPY(  int16_t, s );
			v->buf [i] = v->buf [i + brr_buf_size] = s;
		}

		SPC_COPY( uint16_t, v->interp_pos );
		SPC_COPY( uint16_t, v->brr_addr );
		SPC_COPY( uint16_t, v->env );
		SPC_COPY(  int16_t, v->hidden_env );
		SPC_COPY(  uint8_t, v->buf_pos );
		SPC_COPY(  uint8_t, v->brr_offset );
		SPC_COPY(  uint8_t, v->kon_delay );
		{
			int m = v->env_mode;
			SPC_COPY(  uint8_t, m );
			v->env_mode = (enum env_mode_t) m;
		}
		SPC_COPY(  uint8_t, v->t_envx_out );

		copier.extra();
	}

	// Echo history
	//0
	int s = m.echo_hist_pos [0] [0];
	int s2 = m.echo_hist_pos [0] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [0] [0] = s; // write back at offset 0
	m.echo_hist [0] [1] = s2; // write back at offset 0

	//1
	s = m.echo_hist_pos [1] [0];
	s2 = m.echo_hist_pos [1] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [1] [0] = s; // write back at offset 0
	m.echo_hist [1] [1] = s2; // write back at offset 0

	//2
	s = m.echo_hist_pos [2] [0];
	s2 = m.echo_hist_pos [2] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [2] [0] = s; // write back at offset 0
	m.echo_hist [2] [1] = s2; // write back at offset 0

	//3
	s = m.echo_hist_pos [3] [0];
	s2 = m.echo_hist_pos [3] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [3] [0] = s; // write back at offset 0
	m.echo_hist [3] [1] = s2; // write back at offset 0

	//4
	s = m.echo_hist_pos [4] [0];
	s2 = m.echo_hist_pos [4] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [4] [0] = s; // write back at offset 0
	m.echo_hist [4] [1] = s2; // write back at offset 0

	//5
	s = m.echo_hist_pos [5] [0];
	s2 = m.echo_hist_pos [5] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [5] [0] = s; // write back at offset 0
	m.echo_hist [5] [1] = s2; // write back at offset 0

	//6
	s = m.echo_hist_pos [6] [0];
	s2 = m.echo_hist_pos [6] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [6] [0] = s; // write back at offset 0
	m.echo_hist [6] [1] = s2; // write back at offset 0

	//7
	s = m.echo_hist_pos [7] [0];
	s2 = m.echo_hist_pos [7] [1];

	SPC_COPY( int16_t, s );
	SPC_COPY( int16_t, s2 );
	m.echo_hist [7] [0] = s; // write back at offset 0
	m.echo_hist [7] [1] = s2; // write back at offset 0
	m.echo_hist_pos = m.echo_hist;
	memcpy( &m.echo_hist [echo_hist_size], m.echo_hist, sizeof m.echo_hist [0] << 3 );

	// Misc
	SPC_COPY(  uint8_t, m.every_other_sample );
	SPC_COPY(  uint8_t, m.kon );

	SPC_COPY( uint16_t, m.noise );
	SPC_COPY( uint16_t, m.counter );
	SPC_COPY( uint16_t, m.echo_offset );
	SPC_COPY( uint16_t, m.echo_length );
	SPC_COPY(  uint8_t, m.phase );

	SPC_COPY(  uint8_t, m.new_kon );
	SPC_COPY(  uint8_t, m.endx_buf );
	SPC_COPY(  uint8_t, m.envx_buf );
	SPC_COPY(  uint8_t, m.outx_buf );

	SPC_COPY(  uint8_t, m.t_pmon );
	SPC_COPY(  uint8_t, m.t_non );
	SPC_COPY(  uint8_t, m.t_eon );
	SPC_COPY(  uint8_t, m.t_dir );
	SPC_COPY(  uint8_t, m.t_koff );

	SPC_COPY( uint16_t, m.t_brr_next_addr );
	SPC_COPY(  uint8_t, m.t_adsr0 );
	SPC_COPY(  uint8_t, m.t_brr_header );
	SPC_COPY(  uint8_t, m.t_brr_byte );
	SPC_COPY(  uint8_t, m.t_srcn );
	SPC_COPY(  uint8_t, m.t_esa );
	SPC_COPY(  uint8_t, m.t_echo_enabled );

	SPC_COPY(  int16_t, m.t_main_out [0] );
	SPC_COPY(  int16_t, m.t_main_out [1] );
	SPC_COPY(  int16_t, m.t_echo_out [0] );
	SPC_COPY(  int16_t, m.t_echo_out [1] );
	SPC_COPY(  int16_t, m.t_echo_in  [0] );
	SPC_COPY(  int16_t, m.t_echo_in  [1] );

	SPC_COPY( uint16_t, m.t_dir_addr );
	SPC_COPY( uint16_t, m.t_pitch );
	SPC_COPY(  int16_t, m.t_output );
	SPC_COPY( uint16_t, m.t_echo_ptr );
	SPC_COPY(  uint8_t, m.t_looped );

	copier.extra();
}
#endif


//// Snes9x Accessor

void SPC_DSP::set_spc_snapshot_callback( void (*callback) (void) )
{
	spc_snapshot_callback = callback;
}

void SPC_DSP::dump_spc_snapshot( void )
{
	take_spc_snapshot = 1;
}

#if 0
void SPC_DSP::set_stereo_switch( int value )
{
	stereo_switch = value;
}
#endif

SPC_DSP::uint8_t SPC_DSP::reg_value( int ch, int addr )
{
	return m.voices[ch].regs[addr];
}

int SPC_DSP::envx_value( int ch )
{
	return m.voices[ch].env;
}
