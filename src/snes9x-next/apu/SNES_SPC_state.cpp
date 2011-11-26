// SPC emulation state save/load: copy_state(), save_spc()
// Separate file to avoid linking in unless needed

// snes_spc 0.9.0. http://www.slack.net/‾ant/

#include "SNES_SPC.h"

#if !SPC_NO_COPY_STATE_FUNCS

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

#include <stdio.h>
#include "blargg_source.h"

#define RAM         (m.ram.ram)
#define REGS        (m.smp_regs [0])
#define REGS_IN     (m.smp_regs [1])

#undef IF_0_THEN_256
#define IF_0_THEN_256( n ) ((uint8_t) ((n) - 1) + 1)
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

