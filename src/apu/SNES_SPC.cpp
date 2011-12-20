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

static inline void spc_dsp_write( int data, int time )
{
	// Writes DSP registers.
	int addr = m.smp_regs[0][R_DSPADDR];
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
					 if ( (time - 1) >= m.timers[addr - R_T0OUT].next_time )
						 spc_run_timer_( &m.timers [addr - R_T0OUT], time - 1 )->counter = 0;
					else
						m.timers[addr - R_T0OUT].counter = 0;
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
				{
					RUN_DSP(time, reg_times [m.smp_regs[0][R_DSPADDR]] );
					if (m.smp_regs[0][R_DSPADDR] <= 0x7F )
						spc_dsp_write( data, time );
				}
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
	{
		if ( 0 >= m.timers[i].next_time )
			spc_run_timer_( &m.timers [i], 0 );
	}
	
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
		for ( in = m.buf_begin + SPC_SAMPLE_COUNT(); in < main_end; in++ )
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

/***********************************************************************************
 SPC CPU
***********************************************************************************/

// Inclusion here allows static memory access functions and better optimization

// timers are by far the most common thing read from dp
#define CPU_READ_TIMER( time, offset, addr_, out )\
{\
	int adj_time = time + offset;\
	int dp_addr = addr_;\
	int ti = dp_addr - (R_T0OUT + 0xF0);\
	if ( (unsigned) ti < TIMER_COUNT )\
	{\
		Timer* t = &m.timers [ti];\
		if ( adj_time >= t->next_time )\
		t = spc_run_timer_( t, adj_time );\
		out = t->counter;\
		t->counter = 0;\
	}\
	else\
	{\
		out = ram [dp_addr];\
		int i = dp_addr - 0xF0;\
		if ( (unsigned) i < 0x10 )\
		{ \
			int reg = i;  \
			out = m.smp_regs[1][reg]; \
			reg -= R_DSPADDR; \
			/* DSP addr and data */ \
			if ( (unsigned) reg <= 1 ) /* 4% 0xF2 and 0xF3 */ \
			{ \
				out = m.smp_regs[0][R_DSPADDR]; \
				if ( (unsigned) reg == 1 ) \
				{ \
					RUN_DSP( adj_time, reg_times [m.smp_regs[0][R_DSPADDR] & 0x7F] ); \
					out = dsp_m.regs[m.smp_regs[0][R_DSPADDR] & 0x7F ]; /* 0xF3 */ \
				} \
			} \
		} \
	}\
}


#define READ_TIMER( time, addr, out )	CPU_READ_TIMER( rel_time, time, (addr), out )
#define READ( time, addr )		spc_cpu_read((addr), rel_time + time )
#define WRITE( time, addr, data )	spc_cpu_write((data), (addr), rel_time + time )

static unsigned spc_CPU_mem_bit( uint8_t const* pc, int rel_time )
{
	unsigned addr = GET_LE16( pc );
	unsigned t = READ( 0, addr & 0x1FFF ) >> (addr >> 13);
	return t << 8 & 0x100;
}

#define DP_ADDR( addr )                     (dp + (addr))

#define READ_DP_TIMER(  time, addr, out )   CPU_READ_TIMER( rel_time, time, DP_ADDR( addr ), out )
#define READ_DP(  time, addr )              READ ( time, DP_ADDR( addr ) )
#define WRITE_DP( time, addr, data )        WRITE( time, DP_ADDR( addr ), data )

#define READ_PROG16( addr )                 GET_LE16( ram + (addr) )

#define SET_PC( n )     (pc = ram + (n))
#define GET_PC()        (pc - ram)
#define READ_PC( pc )   (*(pc))

#define SET_SP( v )     (sp = ram + 0x101 + (v))
#define GET_SP()        (sp - 0x101 - ram)

#define PUSH16( v )     (sp -= 2, SET_LE16( sp, v ))
#define PUSH( v )       (void) (*--sp = (uint8_t) (v))
#define POP( out )      (void) ((out) = *sp++)

#define MEM_BIT( rel ) spc_CPU_mem_bit( pc, rel_time + rel )

//// Status flag handling

// Hex value in name to clarify code and bit shifting.
// Flag stored in indicated variable during emulation
#define N80 0x80	// nz
#define V40 0x40	// psw
#define P20 0x20	// dp
#define B10 0x10	//psw
#define H08 0x08	//psw
#define I04 0x04	// psw
#define Z02 0x02	// nz
#define C01 0x01	// c

#define NZ_NEG_MASK 0x880	// either bit set indicates N flag set

#define GET_PSW( out )\
{\
	out = psw & ~(N80 | P20 | Z02 | C01);\
	out |= c  >> 8 & C01;\
	out |= dp >> 3 & P20;\
	out |= ((nz >> 4) | nz) & N80;\
	if ( !(uint8_t) nz ) out |= Z02;\
}

#define SET_PSW( in )\
{\
	psw = in;\
	c   = in << 8;\
	dp  = in << 3 & 0x100;\
	nz  = (in << 4 & 0x800) | (~in & Z02);\
}

BOOST::uint8_t* spc_run_until_( int end_time )
{
	int rel_time = m.spc_time - end_time;
	m.spc_time = end_time;
	m.dsp_time += rel_time;
	m.timers [0].next_time += rel_time;
	m.timers [1].next_time += rel_time;
	m.timers [2].next_time += rel_time;
	uint8_t* const ram = m.ram.ram;
	int a = m.cpu_regs.a;
	int x = m.cpu_regs.x;
	int y = m.cpu_regs.y;
	uint8_t const* pc;
	uint8_t* sp;
	int psw;
	int c;
	int nz;
	int dp;

	SET_PC( m.cpu_regs.pc );
	SET_SP( m.cpu_regs.sp );
	SET_PSW( m.cpu_regs.psw );

	goto loop;


	// Main loop

cbranch_taken_loop:
	pc += *(BOOST::int8_t const*) pc;
inc_pc_loop:
	pc++;
loop:
	{
		unsigned opcode;
		unsigned data;

		opcode = *pc;
		if (allow_time_overflow && rel_time >= 0 )
			goto stop;
		if ( (rel_time += m.cycle_table [opcode]) > 0 && !allow_time_overflow)
			goto out_of_time;

		// TODO: if PC is at end of memory, this will get wrong operand (very obscure)
		data = *++pc;
		switch ( opcode )
		{

			// Common instructions

#define BRANCH( cond )\
			{\
				pc++;\
				pc += (BOOST::int8_t) data;\
				if ( cond )\
				goto loop;\
				pc -= (BOOST::int8_t) data;\
				rel_time -= 2;\
				goto loop;\
			}

			case 0xF0: // BEQ
				BRANCH( !(uint8_t) nz ) // 89% taken

			case 0xD0: // BNE
					BRANCH( (uint8_t) nz )

			case 0x3F:{// CALL
					  int old_addr = GET_PC() + 2;
					  SET_PC( GET_LE16( pc ) );
					  PUSH16( old_addr );
					  goto loop;
				  }

			case 0x6F:// RET
					SET_PC( GET_LE16( sp ) );
					sp += 2;
					goto loop;

			case 0xE4: // MOV a,dp
					++pc;
					// 80% from timer
					READ_DP_TIMER( 0, data, a = nz );
					goto loop;

			case 0xFA:{// MOV dp,dp
					  int temp;
					  READ_DP_TIMER( -2, data, temp );
					  data = temp + NO_READ_BEFORE_WRITE ;
				  }
				  // fall through
			case 0x8F:
				  {// MOV dp,#imm
					  int temp = READ_PC( pc + 1 );
					  pc += 2;

					  int i = dp + temp;
					  ram [i] = (uint8_t) data;
					  i -= 0xF0;
					  if ( (unsigned) i < 0x10 ) // 76%
					  {
						  m.smp_regs[0][i] = (uint8_t) data;

						  // Registers other than $F2 and $F4-$F7
						  //if ( i != 2 && i != 4 && i != 5 && i != 6 && i != 7 )
						  if ( ((~0x2F00 << (bits_in_int - 16)) << i) < 0 ) // 12%
						  {
							  if ( i == R_DSPDATA ) // 99%
							  {
								  RUN_DSP(rel_time, reg_times [m.smp_regs[0][R_DSPADDR]] );
								  if (m.smp_regs[0][R_DSPADDR] <= 0x7F )
									  spc_dsp_write( data, rel_time );
							  }
							  else
								  spc_cpu_write_smp_reg_( data, rel_time, i);
						  }
					  }
					  goto loop;
				  }

			case 0xC4: // MOV dp,a
				  ++pc;
				  {
					  int i = dp + data;
					  ram [i] = (uint8_t) a;
					  i -= 0xF0;
					  if ( (unsigned) i < 0x10 ) // 39%
					  {
						  unsigned sel = i - 2;
						  m.smp_regs[0][i] = (uint8_t) a;

						  if ( sel == 1 ) // 51% $F3
						  {
							  RUN_DSP(rel_time, reg_times [m.smp_regs[0][R_DSPADDR]] );
							  if (m.smp_regs[0][R_DSPADDR] <= 0x7F )
								  spc_dsp_write( a, rel_time );
						  }
						  else if ( sel > 1 ) // 1% not $F2 or $F3
							  spc_cpu_write_smp_reg_( a, rel_time, i );
					  }
				  }
				  goto loop;

#define CASE( n )   case n:

				  // Define common address modes based on opcode for immediate mode. Execution
				  // ends with data set to the address of the operand.
#define ADDR_MODES_( op )\
				  CASE( op - 0x02 ) /* (X) */\
				  data = x + dp;\
				  pc--;\
				  goto end_##op;\
				  CASE( op + 0x0F ) /* (dp)+Y */\
				  data = READ_PROG16( data + dp ) + y;\
				  goto end_##op;\
				  CASE( op - 0x01 ) /* (dp+X) */\
				  data = READ_PROG16( ((uint8_t) (data + x)) + dp );\
				  goto end_##op;\
				  CASE( op + 0x0E ) /* abs+Y */\
				  data += y;\
				  goto abs_##op;\
				  CASE( op + 0x0D ) /* abs+X */\
				  data += x;\
				  CASE( op - 0x03 ) /* abs */\
				  abs_##op:\
				  data += 0x100 * READ_PC( ++pc );\
				  goto end_##op;\
				  CASE( op + 0x0C ) /* dp+X */\
				  data = (uint8_t) (data + x);

#define ADDR_MODES_NO_DP( op )\
				  ADDR_MODES_( op )\
				  data += dp;\
				  end_##op:

#define ADDR_MODES( op )\
				  ADDR_MODES_( op )\
				  CASE( op - 0x04 ) /* dp */\
				  data += dp;\
				  end_##op:

				  // 1. 8-bit Data Transmission Commands. Group I

				  ADDR_MODES_NO_DP( 0xE8 ) // MOV A,addr
					  a = nz = READ( 0, data );
				  goto inc_pc_loop;

			case 0xBF:
				  {
					  // MOV A,(X)+
					  int temp = x + dp;
					  x = (uint8_t) (x + 1);
					  a = nz = READ( -1, temp );
					  goto loop;
				  }

			case 0xE8: // MOV A,imm
				  a  = data;
				  nz = data;
				  goto inc_pc_loop;

			case 0xF9: // MOV X,dp+Y
				  data = (uint8_t) (data + y);
			case 0xF8: // MOV X,dp
				  READ_DP_TIMER( 0, data, x = nz );
				  goto inc_pc_loop;

			case 0xE9: // MOV X,abs
				  data = GET_LE16( pc );
				  ++pc;
				  data = READ( 0, data );
			case 0xCD: // MOV X,imm
				  x  = data;
				  nz = data;
				  goto inc_pc_loop;

			case 0xFB: // MOV Y,dp+X
				  data = (uint8_t) (data + x);
			case 0xEB: // MOV Y,dp
				  // 70% from timer
				  pc++;
				  READ_DP_TIMER( 0, data, y = nz );
				  goto loop;

			case 0xEC:{// MOV Y,abs
					  int temp = GET_LE16( pc );
					  pc += 2;
					  READ_TIMER( 0, temp, y = nz );
					  //y = nz = READ( 0, temp );
					  goto loop;
				  }

			case 0x8D: // MOV Y,imm
				  y  = data;
				  nz = data;
				  goto inc_pc_loop;

				  // 2. 8-BIT DATA TRANSMISSION COMMANDS, GROUP 2

				  ADDR_MODES_NO_DP( 0xC8 ) // MOV addr,A
					  WRITE( 0, data, a );
				  goto inc_pc_loop;

				  {
					  int temp;
					  case 0xCC: // MOV abs,Y
					  temp = y;
					  goto mov_abs_temp;
					  case 0xC9: // MOV abs,X
					  temp = x;
mov_abs_temp:
					  WRITE( 0, GET_LE16( pc ), temp );
					  pc += 2;
					  goto loop;
				  }

			case 0xD9: // MOV dp+Y,X
				  data = (uint8_t) (data + y);
			case 0xD8: // MOV dp,X
				  WRITE( 0, data + dp, x );
				  goto inc_pc_loop;

			case 0xDB: // MOV dp+X,Y
				  data = (uint8_t) (data + x);
			case 0xCB: // MOV dp,Y
				  WRITE( 0, data + dp, y );
				  goto inc_pc_loop;

				  // 3. 8-BIT DATA TRANSMISSIN COMMANDS, GROUP 3.

			case 0x7D: // MOV A,X
				  a  = x;
				  nz = x;
				  goto loop;

			case 0xDD: // MOV A,Y
				  a  = y;
				  nz = y;
				  goto loop;

			case 0x5D: // MOV X,A
				  x  = a;
				  nz = a;
				  goto loop;

			case 0xFD: // MOV Y,A
				  y  = a;
				  nz = a;
				  goto loop;

			case 0x9D: // MOV X,SP
				  x = nz = GET_SP();
				  goto loop;

			case 0xBD: // MOV SP,X
				  SET_SP( x );
				  goto loop;

				  //case 0xC6: // MOV (X),A (handled by MOV addr,A in group 2)

			case 0xAF: // MOV (X)+,A
				  WRITE_DP( 0, x, a + NO_READ_BEFORE_WRITE  );
				  x++;
				  goto loop;

				  // 5. 8-BIT LOGIC OPERATION COMMANDS

#define LOGICAL_OP( op, func )\
				  ADDR_MODES( op ) /* addr */\
				  data = READ( 0, data );\
			case op: /* imm */\
					  nz = a func##= data;\
				  goto inc_pc_loop;\
				  {   unsigned addr;\
					  case op + 0x11: /* X,Y */\
								   data = READ_DP( -2, y );\
					  addr = x + dp;\
					  goto addr_##op;\
					  case op + 0x01: /* dp,dp */\
								     data = READ_DP( -3, data );\
					  case op + 0x10:{/*dp,imm*/\
								 uint8_t const* addr2 = pc + 1;\
								 pc += 2;\
								 addr = READ_PC( addr2 ) + dp;\
							 }\
					  addr_##op:\
					  nz = data func READ( -1, addr );\
					  WRITE( 0, addr, nz );\
					  goto loop;\
				  }

				  LOGICAL_OP( 0x28, & ); // AND

				  LOGICAL_OP( 0x08, | ); // OR

				  LOGICAL_OP( 0x48, ^ ); // EOR

				  // 4. 8-BIT ARITHMETIC OPERATION COMMANDS

				  ADDR_MODES( 0x68 ) // CMP addr
					  data = READ( 0, data );
			case 0x68: // CMP imm
				  nz = a - data;
				  c = ~nz;
				  nz &= 0xFF;
				  goto inc_pc_loop;

			case 0x79: // CMP (X),(Y)
				  data = READ_DP( -2, y );
				  nz = READ_DP( -1, x ) - data;
				  c = ~nz;
				  nz &= 0xFF;
				  goto loop;

			case 0x69: // CMP dp,dp
				  data = READ_DP( -3, data );
			case 0x78: // CMP dp,imm
				  nz = READ_DP( -1, READ_PC( ++pc ) ) - data;
				  c = ~nz;
				  nz &= 0xFF;
				  goto inc_pc_loop;

			case 0x3E: // CMP X,dp
				  data += dp;
				  goto cmp_x_addr;
			case 0x1E: // CMP X,abs
				  data = GET_LE16( pc );
				  pc++;
cmp_x_addr:
				  data = READ( 0, data );
			case 0xC8: // CMP X,imm
				  nz = x - data;
				  c = ~nz;
				  nz &= 0xFF;
				  goto inc_pc_loop;

			case 0x7E: // CMP Y,dp
				  data += dp;
				  goto cmp_y_addr;
			case 0x5E: // CMP Y,abs
				  data = GET_LE16( pc );
				  pc++;
cmp_y_addr:
				  data = READ( 0, data );
			case 0xAD: // CMP Y,imm
				  nz = y - data;
				  c = ~nz;
				  nz &= 0xFF;
				  goto inc_pc_loop;

				  {
					  int addr;
					  case 0xB9: // SBC (x),(y)
					  case 0x99: // ADC (x),(y)
					  pc--; // compensate for inc later
					  data = READ_DP( -2, y );
					  addr = x + dp;
					  goto adc_addr;
					  case 0xA9: // SBC dp,dp
					  case 0x89: // ADC dp,dp
					  data = READ_DP( -3, data );
					  case 0xB8: // SBC dp,imm
					  case 0x98: // ADC dp,imm
					  addr = READ_PC( ++pc ) + dp;
adc_addr:
					  nz = READ( -1, addr );
					  goto adc_data;

					  // catch ADC and SBC together, then decode later based on operand
#undef CASE
#define CASE( n ) case n: case (n) + 0x20:
					  ADDR_MODES( 0x88 ) // ADC/SBC addr
						  data = READ( 0, data );
					  case 0xA8: // SBC imm
					  case 0x88: // ADC imm
					  addr = -1; // A
					  nz = a;
adc_data: {
		  int flags;
		  if ( opcode >= 0xA0 ) // SBC
			  data ^= 0xFF;

		  flags = data ^ nz;
		  nz += data + (c >> 8 & 1);
		  flags ^= nz;

		  psw = (psw & ~(V40 | H08)) |
			  (flags >> 1 & H08) |
			  ((flags + 0x80) >> 2 & V40);
		  c = nz;
		  if ( addr < 0 )
		  {
			  a = (uint8_t) nz;
			  goto inc_pc_loop;
		  }
		  WRITE( 0, addr, /*(uint8_t)*/ nz );
		  goto inc_pc_loop;
	  }

				  }

				  // 6. ADDITION & SUBTRACTION COMMANDS

#define INC_DEC_REG( reg, op )\
				  nz  = reg op;\
				  reg = (uint8_t) nz;\
				  goto loop;

			case 0xBC: INC_DEC_REG( a, + 1 ) // INC A
			case 0x3D: INC_DEC_REG( x, + 1 ) // INC X
			case 0xFC: INC_DEC_REG( y, + 1 ) // INC Y

			case 0x9C: INC_DEC_REG( a, - 1 ) // DEC A
			case 0x1D: INC_DEC_REG( x, - 1 ) // DEC X
			case 0xDC: INC_DEC_REG( y, - 1 ) // DEC Y

			case 0x9B: // DEC dp+X
			case 0xBB: // INC dp+X
				   data = (uint8_t) (data + x);
			case 0x8B: // DEC dp
			case 0xAB: // INC dp
				   data += dp;
				   goto inc_abs;
			case 0x8C: // DEC abs
			case 0xAC: // INC abs
				   data = GET_LE16( pc );
				   pc++;
inc_abs:
				   nz = (opcode >> 4 & 2) - 1;
				   nz += READ( -1, data );
				   WRITE( 0, data, /*(uint8_t)*/ nz );
				   goto inc_pc_loop;

				   // 7. SHIFT, ROTATION COMMANDS

			case 0x5C: // LSR A
				   c = 0;
			case 0x7C:{// ROR A
					  nz = (c >> 1 & 0x80) | (a >> 1);
					  c = a << 8;
					  a = nz;
					  goto loop;
				  }

			case 0x1C: // ASL A
				  c = 0;
			case 0x3C:{// ROL A
					  int temp = c >> 8 & 1;
					  c = a << 1;
					  nz = c | temp;
					  a = (uint8_t) nz;
					  goto loop;
				  }

			case 0x0B: // ASL dp
				  c = 0;
				  data += dp;
				  goto rol_mem;
			case 0x1B: // ASL dp+X
				  c = 0;
			case 0x3B: // ROL dp+X
				  data = (uint8_t) (data + x);
			case 0x2B: // ROL dp
				  data += dp;
				  goto rol_mem;
			case 0x0C: // ASL abs
				  c = 0;
			case 0x2C: // ROL abs
				  data = GET_LE16( pc );
				  pc++;
rol_mem:
				  nz = c >> 8 & 1;
				  nz |= (c = READ( -1, data ) << 1);
				  WRITE( 0, data, /*(uint8_t)*/ nz );
				  goto inc_pc_loop;

			case 0x4B: // LSR dp
				  c = 0;
				  data += dp;
				  goto ror_mem;
			case 0x5B: // LSR dp+X
				  c = 0;
			case 0x7B: // ROR dp+X
				  data = (uint8_t) (data + x);
			case 0x6B: // ROR dp
				  data += dp;
				  goto ror_mem;
			case 0x4C: // LSR abs
				  c = 0;
			case 0x6C: // ROR abs
				  data = GET_LE16( pc );
				  pc++;
ror_mem: {
		 int temp = READ( -1, data );
		 nz = (c >> 1 & 0x80) | (temp >> 1);
		 c = temp << 8;
		 WRITE( 0, data, nz );
		 goto inc_pc_loop;
	 }

			case 0x9F: // XCN
	 nz = a = (a >> 4) | (uint8_t) (a << 4);
	 goto loop;

	 // 8. 16-BIT TRANSMISION COMMANDS

			case 0xBA: // MOVW YA,dp
	 a = READ_DP( -2, data );
	 nz = (a & 0x7F) | (a >> 1);
	 y = READ_DP( 0, (uint8_t) (data + 1) );
	 nz |= y;
	 goto inc_pc_loop;

			case 0xDA: // MOVW dp,YA
	 WRITE_DP( -1, data, a );
	 WRITE_DP( 0, (uint8_t) (data + 1), y + NO_READ_BEFORE_WRITE  );
	 goto inc_pc_loop;

	 // 9. 16-BIT OPERATION COMMANDS

			case 0x3A: // INCW dp
			case 0x1A:{// DECW dp
					  int temp;
					  // low byte
					  data += dp;
					  temp = READ( -3, data );
					  temp += (opcode >> 4 & 2) - 1; // +1 for INCW, -1 for DECW
					  nz = ((temp >> 1) | temp) & 0x7F;
					  WRITE( -2, data, /*(uint8_t)*/ temp );

					  // high byte
					  data = (uint8_t) (data + 1) + dp;
					  temp = (uint8_t) ((temp >> 8) + READ( -1, data ));
					  nz |= temp;
					  WRITE( 0, data, temp );

					  goto inc_pc_loop;
				  }

			case 0x7A: // ADDW YA,dp
			case 0x9A:{// SUBW YA,dp
					  int lo = READ_DP( -2, data );
					  int hi = READ_DP( 0, (uint8_t) (data + 1) );
					  int result;
					  int flags;

					  if ( opcode == 0x9A ) // SUBW
					  {
						  lo = (lo ^ 0xFF) + 1;
						  hi ^= 0xFF;
					  }

					  lo += a;
					  result = y + hi + (lo >> 8);
					  flags = hi ^ y ^ result;

					  psw = (psw & ~(V40 | H08)) |
						  (flags >> 1 & H08) |
						  ((flags + 0x80) >> 2 & V40);
					  c = result;
					  a = (uint8_t) lo;
					  result = (uint8_t) result;
					  y = result;
					  nz = (((lo >> 1) | lo) & 0x7F) | result;

					  goto inc_pc_loop;
				  }

			case 0x5A: { // CMPW YA,dp
					   int temp = a - READ_DP( -1, data );
					   nz = ((temp >> 1) | temp) & 0x7F;
					   temp = y + (temp >> 8);
					   temp -= READ_DP( 0, (uint8_t) (data + 1) );
					   nz |= temp;
					   c  = ~temp;
					   nz &= 0xFF;
					   goto inc_pc_loop;
				   }

				   // 10. MULTIPLICATION & DIVISON COMMANDS

			case 0xCF: { // MUL YA
					   unsigned temp = y * a;
					   a = (uint8_t) temp;
					   nz = ((temp >> 1) | temp) & 0x7F;
					   y = temp >> 8;
					   nz |= y;
					   goto loop;
				   }

			case 0x9E: // DIV YA,X
				   {
					   unsigned ya = y * 0x100 + a;

					   psw &= ~(H08 | V40);

					   if ( y >= x )
						   psw |= V40;

					   if ( (y & 15) >= (x & 15) )
						   psw |= H08;

					   if ( y < x * 2 )
					   {
						   a = ya / x;
						   y = ya - a * x;
					   }
					   else
					   {
						   a = 255 - (ya - x * 0x200) / (256 - x);
						   y = x   + (ya - x * 0x200) % (256 - x);
					   }

					   nz = (uint8_t) a;
					   a = (uint8_t) a;

					   goto loop;
				   }

				   // 11. DECIMAL COMPENSATION COMMANDS

			case 0xDF: // DAA
				   if ( a > 0x99 || c & 0x100 )
				   {
					   a += 0x60;
					   c = 0x100;
				   }

				   if ( (a & 0x0F) > 9 || psw & H08 )
					   a += 0x06;

				   nz = a;
				   a = (uint8_t) a;
				   goto loop;

			case 0xBE: // DAS
				   if ( a > 0x99 || !(c & 0x100) )
				   {
					   a -= 0x60;
					   c = 0;
				   }

				   if ( (a & 0x0F) > 9 || !(psw & H08) )
					   a -= 0x06;

				   nz = a;
				   a = (uint8_t) a;
				   goto loop;

				   // 12. BRANCHING COMMANDS

			case 0x2F: // BRA rel
				   pc += (BOOST::int8_t) data;
				   goto inc_pc_loop;

			case 0x30: // BMI
				   BRANCH( (nz & NZ_NEG_MASK) )

			case 0x10: // BPL
					   BRANCH( !(nz & NZ_NEG_MASK) )

			case 0xB0: // BCS
						   BRANCH( c & 0x100 )

			case 0x90: // BCC
							   BRANCH( !(c & 0x100) )

			case 0x70: // BVS
								   BRANCH( psw & V40 )

			case 0x50: // BVC
									   BRANCH( !(psw & V40) )

#define CBRANCH( cond )\
									   {\
										   pc++;\
										   if ( cond )\
										   goto cbranch_taken_loop;\
										   rel_time -= 2;\
										   goto inc_pc_loop;\
									   }

			case 0x03: // BBS dp.bit,rel
			case 0x23:
			case 0x43:
			case 0x63:
			case 0x83:
			case 0xA3:
			case 0xC3:
			case 0xE3:
										   CBRANCH( READ_DP( -4, data ) >> (opcode >> 5) & 1 )

			case 0x13: // BBC dp.bit,rel
			case 0x33:
			case 0x53:
			case 0x73:
			case 0x93:
			case 0xB3:
			case 0xD3:
			case 0xF3:
											   CBRANCH( !(READ_DP( -4, data ) >> (opcode >> 5) & 1) )

			case 0xDE: // CBNE dp+X,rel
												   data = (uint8_t) (data + x);
												   // fall through
			case 0x2E:{// CBNE dp,rel
					  int temp;
					  // 61% from timer
					  READ_DP_TIMER( -4, data, temp );
					  CBRANCH( temp != a )
				  }

			case 0x6E: { // DBNZ dp,rel
					   unsigned temp = READ_DP( -4, data ) - 1;
					   WRITE_DP( -3, (uint8_t) data, /*(uint8_t)*/ temp + NO_READ_BEFORE_WRITE  );
					   CBRANCH( temp )
				   }

			case 0xFE: // DBNZ Y,rel
				   y = (uint8_t) (y - 1);
				   BRANCH( y )

			case 0x1F: // JMP [abs+X]
					   SET_PC( GET_LE16( pc ) + x );
					   // fall through
			case 0x5F: // JMP abs
					   SET_PC( GET_LE16( pc ) );
					   goto loop;

					   // 13. SUB-ROUTINE CALL RETURN COMMANDS

			case 0x0F:{// BRK
					  int temp;
					  int ret_addr = GET_PC();
					  SET_PC( READ_PROG16( 0xFFDE ) ); // vector address verified
					  PUSH16( ret_addr );
					  GET_PSW( temp );
					  psw = (psw | B10) & ~I04;
					  PUSH( temp );
					  goto loop;
				  }

			case 0x4F:{// PCALL offset
					  int ret_addr = GET_PC() + 1;
					  SET_PC( 0xFF00 | data );
					  PUSH16( ret_addr );
					  goto loop;
				  }

			case 0x01: // TCALL n
			case 0x11:
			case 0x21:
			case 0x31:
			case 0x41:
			case 0x51:
			case 0x61:
			case 0x71:
			case 0x81:
			case 0x91:
			case 0xA1:
			case 0xB1:
			case 0xC1:
			case 0xD1:
			case 0xE1:
			case 0xF1: {
					   int ret_addr = GET_PC();
					   SET_PC( READ_PROG16( 0xFFDE - (opcode >> 3) ) );
					   PUSH16( ret_addr );
					   goto loop;
				   }

				   // 14. STACK OPERATION COMMANDS

				   {
					   int temp;
					   case 0x7F: // RET1
					   temp = *sp;
					   SET_PC( GET_LE16( sp + 1 ) );
					   sp += 3;
					   goto set_psw;
					   case 0x8E: // POP PSW
					   POP( temp );
set_psw:
					   SET_PSW( temp );
					   goto loop;
				   }

			case 0x0D: { // PUSH PSW
					   int temp;
					   GET_PSW( temp );
					   PUSH( temp );
					   goto loop;
				   }

			case 0x2D: // PUSH A
				   PUSH( a );
				   goto loop;

			case 0x4D: // PUSH X
				   PUSH( x );
				   goto loop;

			case 0x6D: // PUSH Y
				   PUSH( y );
				   goto loop;

			case 0xAE: // POP A
				   POP( a );
				   goto loop;

			case 0xCE: // POP X
				   POP( x );
				   goto loop;

			case 0xEE: // POP Y
				   POP( y );
				   goto loop;

				   // 15. BIT OPERATION COMMANDS

			case 0x02: // SET1
			case 0x22:
			case 0x42:
			case 0x62:
			case 0x82:
			case 0xA2:
			case 0xC2:
			case 0xE2:
			case 0x12: // CLR1
			case 0x32:
			case 0x52:
			case 0x72:
			case 0x92:
			case 0xB2:
			case 0xD2:
			case 0xF2: {
					   int bit = 1 << (opcode >> 5);
					   int mask = ~bit;
					   if ( opcode & 0x10 )
						   bit = 0;
					   data += dp;
					   WRITE( 0, data, (READ( -1, data ) & mask) | bit );
					   goto inc_pc_loop;
				   }

			case 0x0E: // TSET1 abs
			case 0x4E: // TCLR1 abs
				   data = GET_LE16( pc );
				   pc += 2;
				   {
					   unsigned temp = READ( -2, data );
					   nz = (uint8_t) (a - temp);
					   temp &= ~a;
					   if ( opcode == 0x0E )
						   temp |= a;
					   WRITE( 0, data, temp );
				   }
				   goto loop;

			case 0x4A: // AND1 C,mem.bit
				   c &= MEM_BIT( 0 );
				   pc += 2;
				   goto loop;

			case 0x6A: // AND1 C,/mem.bit
				   c &= ~MEM_BIT( 0 );
				   pc += 2;
				   goto loop;

			case 0x0A: // OR1 C,mem.bit
				   c |= MEM_BIT( -1 );
				   pc += 2;
				   goto loop;

			case 0x2A: // OR1 C,/mem.bit
				   c |= ~MEM_BIT( -1 );
				   pc += 2;
				   goto loop;

			case 0x8A: // EOR1 C,mem.bit
				   c ^= MEM_BIT( -1 );
				   pc += 2;
				   goto loop;

			case 0xEA: // NOT1 mem.bit
				   data = GET_LE16( pc );
				   pc += 2;
				   {
					   unsigned temp = READ( -1, data & 0x1FFF );
					   temp ^= 1 << (data >> 13);
					   WRITE( 0, data & 0x1FFF, temp );
				   }
				   goto loop;

			case 0xCA: // MOV1 mem.bit,C
				   data = GET_LE16( pc );
				   pc += 2;
				   {
					   unsigned temp = READ( -2, data & 0x1FFF );
					   unsigned bit = data >> 13;
					   temp = (temp & ~(1 << bit)) | ((c >> 8 & 1) << bit);
					   WRITE( 0, data & 0x1FFF, temp + NO_READ_BEFORE_WRITE  );
				   }
				   goto loop;

			case 0xAA: // MOV1 C,mem.bit
				   c = MEM_BIT( 0 );
				   pc += 2;
				   goto loop;

				   // 16. PROGRAM PSW FLAG OPERATION COMMANDS

			case 0x60: // CLRC
				   c = 0;
				   goto loop;

			case 0x80: // SETC
				   c = ~0;
				   goto loop;

			case 0xED: // NOTC
				   c ^= 0x100;
				   goto loop;

			case 0xE0: // CLRV
				   psw &= ~(V40 | H08);
				   goto loop;

			case 0x20: // CLRP
				   dp = 0;
				   goto loop;

			case 0x40: // SETP
				   dp = 0x100;
				   goto loop;

			case 0xA0: // EI
				   psw |= I04;
				   goto loop;

			case 0xC0: // DI
				   psw &= ~I04;
				   goto loop;

				   // 17. OTHER COMMANDS

			case 0x00: // NOP
				   goto loop;

			case 0xFF:{// STOP
					  // handle PC wrap-around
					  unsigned addr = GET_PC() - 1;
					  if ( addr >= 0x10000 )
					  {
						  addr &= 0xFFFF;
						  SET_PC( addr );
						  //dprintf( "SPC: PC wrapped around\n" );
						  goto loop;
					  }
				  }
				  // fall through
			case 0xEF: // SLEEP
				  --pc;
				  rel_time = 0;
				  goto stop;
		} // switch
	}   
out_of_time:
	rel_time -= m.cycle_table [*pc]; // undo partial execution of opcode
stop:

	// Uncache registers
#if 0
	if ( GET_PC() >= 0x10000 )
		dprintf( "SPC: PC wrapped around\n" );
#endif
	m.cpu_regs.pc = (uint16_t) GET_PC();
	m.cpu_regs.sp = ( uint8_t) GET_SP();
	m.cpu_regs.a  = ( uint8_t) a;
	m.cpu_regs.x  = ( uint8_t) x;
	m.cpu_regs.y  = ( uint8_t) y;
	{
		int temp;
		GET_PSW( temp );
		m.cpu_regs.psw = (uint8_t) temp;
	}
	m.spc_time += rel_time;
	m.dsp_time -= rel_time;
	m.timers [0].next_time -= rel_time;
	m.timers [1].next_time -= rel_time;
	m.timers [2].next_time -= rel_time;
	return &m.smp_regs[0][R_CPUIO0];
}
