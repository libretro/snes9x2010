// Core SPC emulation: CPU, timers, SMP registers, memory

// snes_spc 0.9.0. http://www.slack.net/~ant/

#include "SNES_SPC.h"

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

#include "blargg_source.h"

#define RAM         (m.ram.ram)
#define REGS        (m.smp_regs [0])
#define REGS_IN     (m.smp_regs [1])

// (n ? n : 256)
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)

#ifdef BLARGG_ENABLE_OPTIMIZER
	#include BLARGG_ENABLE_OPTIMIZER
#endif

//// Timers

#define TIMER_DIV( t, n ) ((n) >> t->prescaler)
#define TIMER_MUL( t, n ) ((n) << t->prescaler)

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
		t = run_timer_( t, time );
	return t;
}


//// ROM

void SNES_SPC::enable_rom( int enable )
{
	if ( m.rom_enabled != enable )
	{
		m.rom_enabled = enable;
		if ( enable )
			memcpy( m.hi_ram, &RAM [rom_addr], sizeof m.hi_ram );
		memcpy( &RAM [rom_addr], (enable ? m.rom : m.hi_ram), rom_size );
		// TODO: ROM can still get overwritten when DSP writes to echo buffer
	}
}


//// DSP

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
	int clock_count = (count & ~(clocks_per_sample - 1)) + clocks_per_sample;\
	m.dsp_time += clock_count;\
	dsp.run( clock_count );\
}

int SNES_SPC::dsp_read( rel_time_t time )
{
	RUN_DSP( time, reg_times [REGS [r_dspaddr] & 0x7F] );
	
	int result = dsp.read( REGS [r_dspaddr] & 0x7F );
	
	return result;
}

inline void SNES_SPC::dsp_write( int data, rel_time_t time )
{
	RUN_DSP( time, reg_times [REGS [r_dspaddr]] )
	
	if ( REGS [r_dspaddr] <= 0x7F )
		dsp.write( REGS [r_dspaddr], data );
	//dprintf( "SPC wrote to DSP register > $7F\n" );
}


//// Memory access extras

//// CPU write

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
			t = run_timer( t, time );
			t->period = period;
		}
		break;
	}
	case r_t0out:
	case r_t1out:
	case r_t2out:
		//dprintf( "SPC wrote to counter %d\n", (int) addr - r_t0out );
		
		if ( data < no_read_before_write  / 2 )
			run_timer( &m.timers [addr - r_t0out], time - 1 )->counter = 0;
		break;
	
	// Registers that act like RAM
	case 0x8:
	case 0x9:
		REGS_IN [addr] = (uint8_t) data;
		break;
	
	case r_test:
		#if 0
		if ( (uint8_t) data != 0x0A )
			dprintf( "SPC wrote to test register\n" );
		#endif
		break;
	
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
			for ( int i = 0; i < timer_count; i++ )
			{
				Timer* t = &m.timers [i];
				int enabled = data >> i & 1;
				if ( t->enabled != enabled )
				{
					t = run_timer( t, time );
					t->enabled = enabled;
					if ( enabled )
					{
						t->divider = 0;
						t->counter = 0;
					}
				}
			}
		}
		enable_rom( data & 0x80 );
		break;
	}
}

void SNES_SPC::cpu_write_smp_reg( int data, rel_time_t time, int addr )
{
	if ( addr == r_dspdata ) // 99%
		dsp_write( data, time );
	else
		cpu_write_smp_reg_( data, time, addr );
}

void SNES_SPC::cpu_write_high( int data, int i, rel_time_t time )
{
	if ( i < rom_size )
	{
		m.hi_ram [i] = (uint8_t) data;
		if ( m.rom_enabled )
			RAM [i + rom_addr] = m.rom [i]; // restore overwritten ROM
	}
	else
	{
		*(&(RAM [0]) + i + rom_addr) = cpu_pad_fill; // restore overwritten padding
		cpu_write( data, i + rom_addr - 0x10000, time );
	}
}

int const bits_in_int = CHAR_BIT * sizeof (int);

void SNES_SPC::cpu_write( int data, int addr, rel_time_t time )
{
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
				cpu_write_smp_reg( data, time, reg );
		}
		// High mem/address wrap-around
		else
		{
			reg -= rom_addr - 0xF0;
			if ( reg >= 0 ) // 1% in IPL ROM area or address wrapped around
				cpu_write_high( data, reg, time );
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
			result = dsp_read( time ); // 0xF3
	}
	return result;
}

int SNES_SPC::cpu_read( int addr, rel_time_t time )
{
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
				result = cpu_read_smp_reg( reg + r_t0out, time );
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

void SNES_SPC::end_frame( time_t end_time )
{
	// Catch CPU up to as close to end as possible. If final instruction
	// would exceed end, does NOT execute it and leaves m.spc_time < end.
	if ( end_time > m.spc_time )
		run_until_( end_time );
	
	m.spc_time     -= end_time;
	m.extra_clocks += end_time;
	
	// Catch timers up to CPU
	for ( int i = 0; i < timer_count; i++ )
		run_timer( &m.timers [i], 0 );
	
	// Catch DSP up to CPU
	if ( m.dsp_time < 0 )
	{
		RUN_DSP( 0, max_reg_time );
	}
	
	// Save any extra samples beyond what should be generated
	if ( m.buf_begin )
		save_extra();
}

//Support SNES_MEMORY_APURAM
uint8_t * SNES_SPC::apuram()
{
	return m.ram.ram;
}

//// Init

void SNES_SPC::init()
{
	memset( &m, 0, sizeof m );
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
	
	memcpy( reg_times, reg_times_, sizeof reg_times );
	
	reset();
}

void SNES_SPC::init_rom( uint8_t const in [rom_size] )
{
	memcpy( m.rom, in, sizeof m.rom );
}

void SNES_SPC::set_tempo( int t )
{
	m.tempo = t;
	int const timer2_shift = 4; // 64 kHz
	int const other_shift  = 3; //  8 kHz
	
	m.timers [2].prescaler = timer2_shift;
	m.timers [1].prescaler = timer2_shift + other_shift;
	m.timers [0].prescaler = timer2_shift + other_shift;
}

// Timer registers have been loaded. Applies these to the timers. Does not
// reset timer prescalers or dividers.
void SNES_SPC::timers_loaded()
{
	int i;
	for ( i = 0; i < timer_count; i++ )
	{
		Timer* t = &m.timers [i];
		t->period  = IF_0_THEN_256( REGS [r_t0target + i] );
		t->enabled = REGS [r_control] >> i & 1;
		t->counter = REGS_IN [r_t0out + i] & 0x0F;
	}
	
	set_tempo( m.tempo );
}

// Loads registers from unified 16-byte format
void SNES_SPC::load_regs( uint8_t const in [reg_count] )
{
	memcpy( REGS, in, reg_count );
	memcpy( REGS_IN, REGS, reg_count );
	
	// These always read back as 0
	REGS_IN [r_test    ] = 0;
	REGS_IN [r_control ] = 0;
	REGS_IN [r_t0target] = 0;
	REGS_IN [r_t1target] = 0;
	REGS_IN [r_t2target] = 0;
}

// RAM was just loaded from SPC, with $F0-$FF containing SMP registers
// and timer counts. Copies these to proper registers.
void SNES_SPC::ram_loaded()
{
	m.rom_enabled = 0;
	load_regs( &RAM [0xF0] );
	
	// Put STOP instruction around memory to catch PC underflow/overflow
	memset( m.ram.padding1, cpu_pad_fill, sizeof m.ram.padding1 );
	memset( m.ram.padding2, cpu_pad_fill, sizeof m.ram.padding2 );
}

// Registers were just loaded. Applies these new values.
void SNES_SPC::regs_loaded()
{
	enable_rom( REGS [r_control] & 0x80 );
	timers_loaded();
}

void SNES_SPC::reset_time_regs()
{
	m.spc_time      = 0;
	m.dsp_time      = 0;
	m.dsp_time = clocks_per_sample + 1;
	
	for ( int i = 0; i < timer_count; i++ )
	{
		Timer* t = &m.timers [i];
		t->next_time = 1;
		t->divider   = 0;
	}
	
	regs_loaded();
	
	m.extra_clocks = 0;
	reset_buf();
}

void SNES_SPC::reset_common( int timer_counter_init )
{
	int i;
	for ( i = 0; i < timer_count; i++ )
		REGS_IN [r_t0out + i] = timer_counter_init;
	
	// Run IPL ROM
	memset( &m.cpu_regs, 0, sizeof m.cpu_regs );
	m.cpu_regs.pc = rom_addr;
	
	REGS [r_test   ] = 0x0A;
	REGS [r_control] = 0xB0; // ROM enabled, clear ports
	for ( i = 0; i < port_count; i++ )
		REGS_IN [r_cpuio0 + i] = 0;
	
	reset_time_regs();
}

void SNES_SPC::soft_reset()
{
	reset_common( 0 );
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
	memset( RAM, 0x00, 0x10000 );
	ram_loaded();
	reset_common( 0x0F );
	dsp.reset();
}

//// Sample output

void SNES_SPC::reset_buf()
{
	// Start with half extra buffer of silence
	short* out = m.extra_buf;
	while ( out < &m.extra_buf [extra_size / 2] )
		*out++ = 0;
	
	m.extra_pos = out;
	m.buf_begin = 0;
	
	dsp.set_output( 0, 0 );
}

void SNES_SPC::set_output( short* out, int size )
{
	m.extra_clocks &= clocks_per_sample - 1;
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
			out     = dsp.extra();
			out_end = &dsp.extra() [extra_size];
			
			// Copy any remaining extra samples as if DSP wrote them
			while ( in < m.extra_pos )
				*out++ = *in++;
		}
		
		dsp.set_output( out, out_end - out );
	}
	else
	{
		reset_buf();
	}
}

void SNES_SPC::save_extra()
{
	// Get end pointers
	short const* main_end = m.buf_end;     // end of data written to buf
	short const* dsp_end  = dsp.out_pos(); // end of data written to dsp.extra()
	if ( m.buf_begin <= dsp_end && dsp_end <= main_end )
	{
		main_end = dsp_end;
		dsp_end  = dsp.extra(); // nothing in DSP's extra
	}
	
	// Copy any extra samples at these ends into extra_buf
	short* out = m.extra_buf;
	short const* in;
	for ( in = m.buf_begin + sample_count(); in < main_end; in++ )
		*out++ = *in;
	for ( in = dsp.extra(); in < dsp_end ; in++ )
		*out++ = *in;
	
	m.extra_pos = out;
}

#if !SPC_NO_COPY_STATE_FUNCS
void SNES_SPC::copy_state( unsigned char** io, copy_func_t copy )
{
	SPC_State_Copier copier( io, copy );
	
	// Make state data more readable by putting 64K RAM, 16 SMP registers,
	// then DSP (with its 128 registers) first

	// RAM
	enable_rom( 0 ); // will get re-enabled if necessary in regs_loaded() below
	copier.copy( RAM, 0x10000 );
	
	{
		// SMP registers
		uint8_t regs [reg_count];
		uint8_t regs_in [reg_count];

		memcpy( regs, REGS, reg_count );
		memcpy( regs_in, REGS_IN, reg_count );

		copier.copy( regs, sizeof regs );
		copier.copy( regs_in, sizeof regs_in );

		memcpy( REGS, regs, reg_count);
		memcpy( REGS_IN, regs_in, reg_count );

		enable_rom( REGS [r_control] & 0x80 );
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
	for ( int i = 0; i < timer_count; i++ )
	{
		Timer* t = &m.timers [i];
		t->period  = IF_0_THEN_256( REGS [r_t0target + i] );
		t->enabled = REGS [r_control] >> i & 1;
		SPC_COPY( int16_t, t->next_time );
		SPC_COPY( uint8_t, t->divider );
		SPC_COPY( uint8_t, t->counter );
		copier.extra();
	}

	set_tempo( m.tempo );

	copier.extra();
}
#endif

// Inclusion here allows static memory access functions and better optimization
#include "SPC_CPU.h"

