// Core SPC emulation: CPU, timers, SMP registers, memory

// snes_spc 0.9.0. http://www.slack.net/~ant/

#include "SNES_SPC.h"

spc_state_t m;
static signed char reg_times [256];
bool allow_time_overflow;

#include <string.h>

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

// (n ? n : 256)
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)

//// Timers

#define TIMER_DIV( t, n ) ((n) >> t->prescaler)
#define TIMER_MUL( t, n ) ((n) << t->prescaler)

static Timer* spc_run_timer_( Timer* t, int time )
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

inline Timer* spc_run_timer( Timer* t, int time )
{
	if ( time >= t->next_time )
		t = spc_run_timer_( t, time );
	return t;
}

//// ROM

void spc_enable_rom( int enable )
{
	if ( m.rom_enabled != enable )
	{
		m.rom_enabled = enable;
		if ( enable )
			memcpy( m.hi_ram, &m.ram.ram[ROM_ADDR], sizeof m.hi_ram );
		memcpy( &m.ram.ram[ROM_ADDR], (enable ? m.rom : m.hi_ram), ROM_SIZE );
		// TODO: ROM can still get overwritten when DSP writes to echo buffer
	}
}


//// DSP

#define MAX_REG_TIME 29
	
#define RUN_DSP( time, offset ) \
	int count = (time) - (offset) - m.dsp_time; \
	if ( count >= 0 ) \
	{ \
		int clock_count = (count & ~(CLOCKS_PER_SAMPLE - 1)) + CLOCKS_PER_SAMPLE; \
		m.dsp_time += clock_count; \
		dsp_run( clock_count ); \
	}

// Writes DSP registers. For accuracy, you must first call run()
// to catch the DSP up to present.

static inline void dsp_write( int addr, int data )
{
	dsp_m.regs [addr] = (uint8_t) data;
	switch ( addr & 0x0F )
	{
		case V_ENVX:
			dsp_m.envx_buf = (uint8_t) data;
			break;

		case V_OUTX:
			dsp_m.outx_buf = (uint8_t) data;
			break;

		case 0x0C:
			if ( addr == R_KON )
				dsp_m.new_kon = (uint8_t) data;

			if ( addr == R_ENDX ) // always cleared, regardless of data written
			{
				dsp_m.endx_buf = 0;
				dsp_m.regs [R_ENDX] = 0;
			}
			break;
	}
}

static inline void spc_dsp_write( int data, int time )
{
	RUN_DSP(time, reg_times [m.smp_regs[0][R_DSPADDR]] )
	
	if (m.smp_regs[0][R_DSPADDR] <= 0x7F )
		dsp_write(m.smp_regs[0][R_DSPADDR], data );
	//dprintf( "SPC wrote to DSP register > $7F\n" );
}


//// Memory access extras

//// CPU write

// divided into multiple functions to keep rarely-used functionality separate
// so often-used functionality can be optimized better by compiler

// If write isn't preceded by read, data has this added to it
//int const no_read_before_write = 0x2000;
#define NO_READ_BEFORE_WRITE			8192
#define NO_READ_BEFORE_WRITE_DIVIDED_BY_TWO	4096 

static void spc_cpu_write_smp_reg_( int data, int time, int addr )
{
	switch ( addr )
	{
		case R_T0TARGET:
		case R_T1TARGET:
		case R_T2TARGET:
			{
				Timer* t = &m.timers [addr - R_T0TARGET];
				int period = IF_0_THEN_256( data );
				if ( t->period != period )
				{
					if ( time >= t->next_time )
						t = spc_run_timer_( t, time );
					t->period = period;
				}
				break;
			}
		case R_T0OUT:
		case R_T1OUT:
		case R_T2OUT:
				 //dprintf( "SPC wrote to counter %d\n", (int) addr - R_T0OUT );

				 if ( data < NO_READ_BEFORE_WRITE_DIVIDED_BY_TWO)
				 {
					 spc_run_timer( &m.timers [addr - R_T0OUT], time - 1 )->counter = 0;
				 }
				 break;

				 // Registers that act like RAM
		case 0x8:
		case 0x9:
				 m.smp_regs[1][addr] = (uint8_t) data;
				 break;

		case R_TEST:
#if 0
				 if ( (uint8_t) data != 0x0A )
					 dprintf( "SPC wrote to test register\n" );
#endif
				 break;

		case R_CONTROL:
				 // port clears
				 if ( data & 0x10 )
				 {
					 m.smp_regs[1][R_CPUIO0] = 0;
					 m.smp_regs[1][R_CPUIO1] = 0;
				 }
				 if ( data & 0x20 )
				 {
					 m.smp_regs[1][R_CPUIO2] = 0;
					 m.smp_regs[1][R_CPUIO3] = 0;
				 }

				 // timers
				 {
					 for ( int i = 0; i < TIMER_COUNT; i++ )
					 {
						 Timer* t = &m.timers [i];
						 int enabled = data >> i & 1;
						 if ( t->enabled != enabled )
						 {
							 if ( time >= t->next_time )
								 t = spc_run_timer_( t, time );
							 t->enabled = enabled;
							 if ( enabled )
							 {
								 t->divider = 0;
								 t->counter = 0;
							 }
						 }
					 }
				 }
				 spc_enable_rom( data & 0x80 );
				 break;
	}
}

int const bits_in_int = CHAR_BIT * sizeof (int);

static void spc_cpu_write( int data, int addr, int time )
{
	// RAM
	m.ram.ram[addr] = (uint8_t) data;
	int reg = addr - 0xF0;
	if ( reg >= 0 ) // 64%
	{
		// $F0-$FF
		if ( reg < REG_COUNT ) // 87%
		{
			m.smp_regs[0][reg] = (uint8_t) data;
			
			// Registers other than $F2 and $F4-$F7
			//if ( reg != 2 && reg != 4 && reg != 5 && reg != 6 && reg != 7 )
			// TODO: this is a bit on the fragile side
			if ( ((~0x2F00 << (bits_in_int - 16)) << reg) < 0 ) // 36%
			{
				if ( reg == R_DSPDATA ) // 99%
					spc_dsp_write( data, time );
				else
					spc_cpu_write_smp_reg_( data, time, reg);
			}
		}
		// High mem/address wrap-around
		else
		{
			reg -= ROM_ADDR - 0xF0;
			if ( reg >= 0 ) // 1% in IPL ROM area or address wrapped around
			{
				if ( reg < ROM_SIZE )
				{
					m.hi_ram [reg] = (uint8_t) data;
					if ( m.rom_enabled )
						m.ram.ram[reg + ROM_ADDR] = m.rom [reg]; // restore overwritten ROM
				}
				else
				{
					*(&(m.ram.ram[0]) + reg + ROM_ADDR) = CPU_PAD_FILL; // restore overwritten padding
					spc_cpu_write( data, reg + ROM_ADDR - 0x10000, time );
				}
			}
		}
	}
}

//// CPU read

static int spc_cpu_read( int addr, int time )
{
	// RAM
	int result = m.ram.ram[addr];
	int reg = addr - 0xF0;
	if ( reg >= 0 ) // 40%
	{
		reg -= 0x10;
		if ( (unsigned) reg >= 0xFF00 ) // 21%
		{
			reg += 0x10 - R_T0OUT;
			
			// Timers
			if ( (unsigned) reg < TIMER_COUNT ) // 90%
			{
				Timer* t = &m.timers [reg];
				if ( time >= t->next_time )
					t = spc_run_timer_( t, time );
				result = t->counter;
				t->counter = 0;
			}
			// Other registers
			else if ( reg < 0 ) // 10%
			{
				int reg_tmp = reg + R_T0OUT;
				result = m.smp_regs[1][reg_tmp];
				reg_tmp -= R_DSPADDR;
				// DSP addr and data
				if ( (unsigned) reg_tmp <= 1 ) // 4% 0xF2 and 0xF3
				{
					result = m.smp_regs[0][R_DSPADDR];
					if ( (unsigned) reg_tmp == 1 )
					{
						RUN_DSP( time, reg_times [m.smp_regs[0][R_DSPADDR] & 0x7F] );

						result = dsp_m.regs[m.smp_regs[0][R_DSPADDR] & 0x7F]; // 0xF3
					}
				}
			}
			else // 1%
				result = spc_cpu_read( reg + (R_T0OUT + 0xF0 - 0x10000), time );
		}
	}
	
	return result;
}

//// Run

void spc_end_frame( int end_time )
{
	// Catch CPU up to as close to end as possible. If final instruction
	// would exceed end, does NOT execute it and leaves m.spc_time < end.
	if ( end_time > m.spc_time )
		spc_run_until_( end_time );
	
	m.spc_time     -= end_time;
	m.extra_clocks += end_time;
	
	// Catch timers up to CPU
	for ( int i = 0; i < TIMER_COUNT; i++ )
		spc_run_timer( &m.timers [i], 0 );
	
	// Catch DSP up to CPU
	if ( m.dsp_time < 0 )
	{
		RUN_DSP( 0, MAX_REG_TIME );
	}
	
	// Save any extra samples beyond what should be generated
	if ( m.buf_begin )
	{
		// Get end pointers
		short const* main_end = m.buf_end;     // end of data written to buf
		short const* dsp_end  = dsp_m.out; // end of data written to dsp.extra()
		if ( m.buf_begin <= dsp_end && dsp_end <= main_end )
		{
			main_end = dsp_end;
			dsp_end  = dsp_m.extra; // nothing in DSP's extra
		}

		// Copy any extra samples at these ends into extra_buf
		short* out = m.extra_buf;
		short const* in;
		for ( in = m.buf_begin + spc_sample_count(); in < main_end; in++ )
			*out++ = *in;
		for ( in = dsp_m.extra; in < dsp_end ; in++ )
			*out++ = *in;

		m.extra_pos = out;
	}
}

//Support SNES_MEMORY_APURAM
uint8_t * spc_apuram()
{
	return m.ram.ram;
}

//// Init

void spc_reset()
{
	m.cpu_regs.pc  = 0xFFC0;
	m.cpu_regs.a   = 0x00;
	m.cpu_regs.x   = 0x00;
	m.cpu_regs.y   = 0x00;
	m.cpu_regs.psw = 0x02;
	m.cpu_regs.sp  = 0xEF;
	memset( m.ram.ram, 0x00, 0x10000 );

	// RAM was just loaded from SPC, with $F0-$FF containing SMP registers
	// and timer counts. Copies these to proper registers.
	m.rom_enabled = 0;

	// Loads registers from unified 16-byte format
	memcpy( m.smp_regs[0], &m.ram.ram[0xF0], REG_COUNT );
	memcpy( m.smp_regs[1], m.smp_regs[0], REG_COUNT );
	
	// These always read back as 0
	m.smp_regs[1][R_TEST    ] = 0;
	m.smp_regs[1][R_CONTROL ] = 0;
	m.smp_regs[1][R_T0TARGET] = 0;
	m.smp_regs[1][R_T1TARGET] = 0;
	m.smp_regs[1][R_T2TARGET] = 0;
	
	// Put STOP instruction around memory to catch PC underflow/overflow
	memset( m.ram.padding1, CPU_PAD_FILL, sizeof m.ram.padding1 );
	memset( m.ram.padding2, CPU_PAD_FILL, sizeof m.ram.padding2 );

	spc_reset_common( 0x0F );
	dsp_reset();
}

void spc_init()
{
	memset( &m, 0, sizeof m );
	dsp_init( m.ram.ram );
	
	m.tempo = TEMPO_UNIT;
	
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

	signed char const reg_times_ [256] =
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
	
	memcpy( reg_times, reg_times_, sizeof reg_times );
	
	spc_reset();

	uint8_t APUROM[64] =
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

	memcpy( m.rom, APUROM, sizeof m.rom );
}

void spc_set_tempo( int t )
{
	m.tempo = t;
	int const timer2_shift = 4; // 64 kHz
	int const other_shift  = 3; //  8 kHz
	
	m.timers [2].prescaler = timer2_shift;
	m.timers [1].prescaler = timer2_shift + other_shift;
	m.timers [0].prescaler = timer2_shift + other_shift;
}

#define reset_buf() \
	/* Start with half extra buffer of silence */ \
	short* out = m.extra_buf; \
	while ( out < &m.extra_buf [EXTRA_SIZE_DIV_2] ) \
		*out++ = 0; \
	\
	m.extra_pos = out; \
	m.buf_begin = 0; \
	\
	dsp_set_output( 0, 0 );

void spc_reset_common( int timer_counter_init )
{
	int i;
	for ( i = 0; i < TIMER_COUNT; i++ )
		m.smp_regs[1][R_T0OUT + i] = timer_counter_init;
	
	// Run IPL ROM
	memset( &m.cpu_regs, 0, sizeof m.cpu_regs );
	m.cpu_regs.pc = ROM_ADDR;
	
	m.smp_regs[0][R_TEST   ] = 0x0A;
	m.smp_regs[0][R_CONTROL] = 0xB0; // ROM enabled, clear ports
	for ( i = 0; i < PORT_COUNT; i++ )
		m.smp_regs[1][R_CPUIO0 + i] = 0;
	
	// reset time registers
	m.spc_time      = 0;
	m.dsp_time      = 0;
	m.dsp_time = CLOCKS_PER_SAMPLE + 1;
	
	for ( int i = 0; i < TIMER_COUNT; i++ )
	{
		Timer* t = &m.timers [i];
		t->next_time = 1;
		t->divider   = 0;
	}
	
	// Registers were just loaded. Applies these new values.
	spc_enable_rom( m.smp_regs[0][R_CONTROL] & 0x80 );

	// Timer registers have been loaded. Applies these to the timers. Does not
	// reset timer prescalers or dividers.
	for (int i = 0; i < TIMER_COUNT; i++ )
	{
		Timer* t = &m.timers [i];
		t->period  = IF_0_THEN_256( m.smp_regs[0][R_T0TARGET + i] );
		t->enabled = m.smp_regs[0][R_CONTROL] >> i & 1;
		t->counter = m.smp_regs[1][R_T0OUT + i] & 0x0F;
	}
	
	spc_set_tempo( m.tempo );
	
	m.extra_clocks = 0;
	reset_buf();
}

void spc_soft_reset()
{
	spc_reset_common( 0 );
	dsp_soft_reset();
}


//// Sample output


void spc_set_output( short* out, int size )
{
	m.extra_clocks &= CLOCKS_PER_SAMPLE - 1;
	if ( out )
	{
		short const* out_end = out + size;
		m.buf_begin = out;
		m.buf_end   = out_end;
		
		// Copy extra to output
		short const* in = m.extra_buf;
		while ( in < m.extra_pos && out < out_end )
			*out++ = *in++;
		
		// Handle output being full already
		if ( out >= out_end )
		{
			// Have DSP write to remaining extra space
			out     = dsp_m.extra; 
			out_end = &dsp_m.extra[EXTRA_SIZE];
			
			// Copy any remaining extra samples as if DSP wrote them
			while ( in < m.extra_pos )
				*out++ = *in++;
		}
		
		dsp_set_output( out, out_end - out );
	}
	else
	{
		reset_buf();
	}
}

#if !SPC_NO_COPY_STATE_FUNCS
void spc_copy_state( unsigned char** io, dsp_copy_func_t copy )
{
	spc_state_copy_t copier;
	copier.func = copy;
	copier.buf = io;
	
	// Make state data more readable by putting 64K RAM, 16 SMP registers,
	// then DSP (with its 128 registers) first

	// RAM
	spc_enable_rom( 0 ); // will get re-enabled if necessary in regs_loaded() below
	spc_copier_copy(&copier, m.ram.ram, 0x10000 );
	
	{
		// SMP registers
		uint8_t regs [REG_COUNT];
		uint8_t regs_in [REG_COUNT];

		memcpy( regs, m.smp_regs[0], REG_COUNT );
		memcpy( regs_in, m.smp_regs[1], REG_COUNT );

		spc_copier_copy(&copier, regs, sizeof(regs));
		spc_copier_copy(&copier, regs_in, sizeof(regs_in));

		memcpy( m.smp_regs[0], regs, REG_COUNT);
		memcpy( m.smp_regs[1], regs_in, REG_COUNT );

		spc_enable_rom( m.smp_regs[0][R_CONTROL] & 0x80 );
	}
	
	// CPU registers
	SPC_COPY( uint16_t, m.cpu_regs.pc );
	SPC_COPY(  uint8_t, m.cpu_regs.a );
	SPC_COPY(  uint8_t, m.cpu_regs.x );
	SPC_COPY(  uint8_t, m.cpu_regs.y );
	SPC_COPY(  uint8_t, m.cpu_regs.psw );
	SPC_COPY(  uint8_t, m.cpu_regs.sp );
	spc_copier_extra(&copier);
	
	SPC_COPY( int16_t, m.spc_time );
	SPC_COPY( int16_t, m.dsp_time );

	// DSP
	dsp_copy_state( io, copy );
	
	// Timers
	for ( int i = 0; i < TIMER_COUNT; i++ )
	{
		Timer* t = &m.timers [i];
		t->period  = IF_0_THEN_256( m.smp_regs[0][R_T0TARGET + i] );
		t->enabled = m.smp_regs[0][R_CONTROL] >> i & 1;
		SPC_COPY( int16_t, t->next_time );
		SPC_COPY( uint8_t, t->divider );
		SPC_COPY( uint8_t, t->counter );
		spc_copier_extra(&copier);
	}

	spc_set_tempo( m.tempo );

	spc_copier_extra(&copier);
}
#endif

// Inclusion here allows static memory access functions and better optimization
#include "SPC_CPU.h"
