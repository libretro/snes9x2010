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


#include "snes9x.h"
#include "memmap.h"
#include "cpuops.h"
#include "dma.h"
#include "apu/apu.h"
#include "fxemu.h"
#include "snapshot.h"

#include <sys/timer.h>
#include <sys/sys_time.h>
#include "../cellframework2/audio/stream.h"
extern cell_audio_handle_t audio_handle;
extern const struct cell_audio_driver *audio_driver;

#define TWO_TIMES_SLOW_ONE_CYCLE 16

static inline void S9xCheckMissingHTimerPosition (int32 hc)
{
	if (PPU.HTimerPosition == hc)
	{
		if ((PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) || (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)))
		{
			CPU.IRQActive |= PPU_IRQ_SOURCE;
			CPU.IRQPending = Timings.IRQPendCount;
			CPU.Flags |= IRQ_FLAG;

			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
			int32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
			int32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
			Registers.PCw = result_register_pcw;

		}
	}
}

void SA1S9xMainLoop (void)
{
	uint64_t sa1_condition_0x10 = (Memory.FillRAM[0x2209] & 0x10);
	uint64_t sa1_condition_0x40 = (Memory.FillRAM[0x2209] & 0x40);

	do	{
		if ((CPU.Flags & NMI_FLAG) && Timings.NMITriggerPos <= CPU.Cycles)
		{
			CPU.Flags &= ~NMI_FLAG;
			Timings.NMITriggerPos = 0xffff;
#if 1
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
			int32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
			int32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
			Registers.PCw = result_register_pcw;
#endif
#if 0
			if (CPU.WaitingForInterrupt)
			{
				CPU.WaitingForInterrupt = FALSE;
				Registers.PCw++;
			}
#endif

			//S9xOpcode_NMI();
#if (S9X_ACCURACY_LEVEL >= 3)
#define AddCycles(n)	{ CPU.Cycles += (n); while (CPU.Cycles >= CPU.NextEvent) { \
	switch (CPU.WhichEvent) \
	{ \
		case HC_HBLANK_START_EVENT: \
					    S9xCheckMissingHTimerPosition(Timings.HBlankStart); \
		S9xReschedule(); \
		break; \
		\
		case HC_HDMA_START_EVENT: \
					  S9xCheckMissingHTimerPosition(Timings.HDMAStart); \
		S9xReschedule(); \
		\
		if (PPU.HDMA && CPU.V_Counter <= PPU.ScreenHeight) \
		{ \
			PPU.HDMA = S9xDoHDMA_NoUniracers(PPU.HDMA); \
		} \
		\
		break; \
		\
		case HC_HCOUNTER_MAX_EVENT: \
					    S9xAPUEndScanline(); \
		CPU.Cycles -= Timings.H_Max; \
		S9xAPUSetReferenceTime(CPU.Cycles); \
		\
		if ((Timings.NMITriggerPos != 0xffff) && (Timings.NMITriggerPos >= Timings.H_Max)) \
		Timings.NMITriggerPos -= Timings.H_Max; \
		\
		CPU.V_Counter++; \
		if (CPU.V_Counter >= Timings.V_Max)	\
		{ \
			CPU.V_Counter = 0; \
			Timings.InterlaceField ^= 1; \
			\
			addition = (IPPU.Interlace & ~(Timings.InterlaceField)); \
			Timings.V_Max = Timings.V_Max_Master + addition; \
			\
			Memory.FillRAM[0x213F] ^= 0x80; \
			PPU.RangeTimeOver = 0; \
			\
			Memory.FillRAM[0x4210] = MAX_5A22_VERSION; \
			CPU.Flags &= ~NMI_FLAG; \
			Timings.NMITriggerPos = 0xffff; \
			PPU.HVBeamCounterLatched = 0; \
			CPU.Flags |= SCAN_KEYS_FLAG; \
		} \
		\
		if (CPU.V_Counter == 240 && !IPPU.Interlace && Timings.InterlaceField) \
		Timings.H_Max = Timings.H_Max_Master - ONE_DOT_CYCLE; \
		else \
		Timings.H_Max = Timings.H_Max_Master; \
		\
		if (CPU.V_Counter != 240 || IPPU.Interlace || !Timings.InterlaceField)	\
		{ \
			if (Timings.WRAMRefreshPos == SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE)	\
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2;					\
			else \
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE;	\
		} \
		S9xCheckMissingHTimerPosition(0); \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)	\
		{ \
			S9xEndScreenRefresh(); \
			PPU.HDMA = 0; \
			IPPU.MaxBrightness = PPU.Brightness; \
			PPU.ForcedBlanking = (Memory.FillRAM[0x2100] >> 7) & 1; \
			\
			if (!PPU.ForcedBlanking) \
			{ \
				PPU.OAMAddr = PPU.SavedOAMAddr; \
				\
				uint8	tmp = 0; \
				\
				if (PPU.OAMPriorityRotation) \
				tmp = (PPU.OAMAddr & 0xFE) >> 1; \
				if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp) \
				{ \
					PPU.FirstSprite = tmp; \
					IPPU.OBJChanged = TRUE; \
				} \
				\
				PPU.OAMFlip = 0; \
			} \
			\
			Memory.FillRAM[0x4210] = 0x80 | MAX_5A22_VERSION; \
			if (Memory.FillRAM[0x4200] & 0x80) \
			{ \
				CPU.Flags |= NMI_FLAG; \
				Timings.NMITriggerPos = 6 + 6; \
			} \
			\
		} \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + 3) \
		{ \
			if (Memory.FillRAM[0x4200] & 1) \
			S9xDoAutoJoypad_SA1(); \
		} \
		\
		if (CPU.V_Counter == FIRST_VISIBLE_LINE)	\
		S9xStartScreenRefresh(); \
		\
		CPU.NextEvent = -1; \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_HDMA_INIT_EVENT: \
					 S9xCheckMissingHTimerPosition(Timings.HDMAInit); \
		S9xReschedule(); \
		\
		if (CPU.V_Counter == 0) \
		{ \
			S9xStartHDMA(); \
		} \
		\
		break; \
		\
		case HC_RENDER_EVENT: \
				      if (CPU.V_Counter >= FIRST_VISIBLE_LINE && CPU.V_Counter <= PPU.ScreenHeight) \
		RenderLine((uint8) (CPU.V_Counter - FIRST_VISIBLE_LINE)); \
		\
		S9xCheckMissingHTimerPosition(SNES_RENDER_START_HC); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_WRAM_REFRESH_EVENT: \
					    \
		if ((PPU.HTimerPosition >= Timings.WRAMRefreshPos) && (PPU.HTimerPosition < (Timings.WRAMRefreshPos + SNES_WRAM_REFRESH_CYCLES))) \
		{ \
			if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
			CPU.IRQPending = 1; \
			else \
			if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
			CPU.IRQPending = 1; \
		} \
		CPU.Cycles += SNES_WRAM_REFRESH_CYCLES; \
		\
		S9xCheckMissingHTimerPosition(Timings.WRAMRefreshPos); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_IRQ_1_3_EVENT: \
		case HC_IRQ_3_5_EVENT: \
		case HC_IRQ_5_7_EVENT: \
		case HC_IRQ_7_9_EVENT: \
		case HC_IRQ_9_A_EVENT: \
		case HC_IRQ_A_1_EVENT: \
				       if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
				       else \
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
		S9xReschedule(); \
		break; \
	} \
} \
} 
#else
#undef AddCycles
#define AddCycles(n)	{ CPU.Cycles += (n); }
#endif

uint64_t addition;
// IRQ and NMI do an opcode fetch as their first "IO" cycle.
AddCycles(CPU.MemSpeed + ONE_CYCLE);

if (!CheckEmulation())
{
	//PushB(Registers.PB);
	S9xSetByte_SA1(Registers.PB, Registers.S.W--);

	//PushW(Registers.PCw);
	S9xSetWord_SA1_Write10_WrapBank(Registers.PCw, Registers.S.W - 1);
	Registers.S.W -= 2;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushB(Registers.PL);
	S9xSetByte_SA1(Registers.PL, Registers.S.W--);

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	int32_t mask = ((sa1_condition_0x10) | -(sa1_condition_0x10)) >> 31;
	uint16 addr = (((0) & mask) | ((S9xGetWord(0xFFEA)) & ~mask));
	int32_t result_openbus = (((Memory.FillRAM[0x220d]) & mask) | ((addr >> 8) & ~mask));
	int32_t result_s9xsetpcbase_sa1 = (((Memory.FillRAM[0x220c] | (Memory.FillRAM[0x220d] << 8)) & mask) | (((addr) & ~mask)));
	int32_t result_cpucycles = (((CPU.Cycles + (TWO_TIMES_SLOW_ONE_CYCLE)) & mask) | ((CPU.Cycles) & ~mask));
	OpenBus = result_openbus;
	CPU.Cycles = result_cpucycles;
	S9xSetPCBase_SA1(result_s9xsetpcbase_sa1);
	/*
	   if (sa1_condition_0x10)
	   {
	   OpenBus = Memory.FillRAM[0x220d];
	   AddCycles(TWO_TIMES_SLOW_ONE_CYCLE);
	   S9xSetPCBase_SA1(Memory.FillRAM[0x220c] | (Memory.FillRAM[0x220d] << 8));
	   }
	   else
	   {
	   uint16	addr = S9xGetWord(0xFFEA);
	   OpenBus = addr >> 8;
	   S9xSetPCBase_SA1(addr);
	   }
	 */
}
else
{
	//PushWE(Registers.PCw);
	Registers.SL--;
	//S9xSetWord_SA1_Write10(Registers.PCw, Registers.S.W, WRAP_PAGE);
	S9xSetWord_SA1_Write10_WrapPage(Registers.PCw, Registers.S.W);
	Registers.SL--;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushBE(Registers.PL);
	S9xSetByte_SA1(Registers.PL, Registers.S.W);
	Registers.SL--;

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	int32_t mask = ((sa1_condition_0x10) | -(sa1_condition_0x10)) >> 31;
	uint16 addr = (((0) & mask) | ((S9xGetWord(0xFFEA)) & ~mask));
	int32_t result_openbus = (((Memory.FillRAM[0x220d]) & mask) | ((addr >> 8) & ~mask));
	int32_t result_s9xsetpcbase_sa1 = (((Memory.FillRAM[0x220c] | (Memory.FillRAM[0x220d] << 8)) & mask) | (((addr) & ~mask)));
	int32_t result_cpucycles = (((CPU.Cycles + (TWO_TIMES_SLOW_ONE_CYCLE)) & mask) | ((CPU.Cycles) & ~mask));
	OpenBus = result_openbus;
	CPU.Cycles = result_cpucycles;
	S9xSetPCBase_SA1(result_s9xsetpcbase_sa1);

	/*
	   if (sa1_condition_0x10)
	   {
	   OpenBus = Memory.FillRAM[0x220d];
	   AddCycles(TWO_TIMES_SLOW_ONE_CYCLE);
	   S9xSetPCBase_SA1(Memory.FillRAM[0x220c] | (Memory.FillRAM[0x220d] << 8));
	   }
	   else
	   {
	   uint16	addr = S9xGetWord(0xFFFA);
	   OpenBus = addr >> 8;
	   S9xSetPCBase_SA1(addr);
	   }
	 */
}
}


if (CPU.Flags & IRQ_FLAG)
{
	if (CPU.IRQPending)
		// FIXME: In case of IRQ during WRAM refresh
		CPU.IRQPending--;
	else
	{
		int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
		uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
		uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
		CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
		Registers.PCw = result_register_pcw;
		/*
		   if (CPU.WaitingForInterrupt)
		   {
		   CPU.WaitingForInterrupt = FALSE;
		   Registers.PCw++;
		   }
		 */

		if (CPU.IRQActive)
		{
			if (!CheckFlag(IRQ))
				// in IRQ handler $4211 is supposed to be read, so IRQ_FLAG should be cleared.
				//S9xOpcode_IRQ();
			{

				// IRQ and NMI do an opcode fetch as their first "IO" cycle.
				uint64_t addition;
				AddCycles(CPU.MemSpeed + ONE_CYCLE);

				if (!CheckEmulation())
				{
					//PushB(Registers.PB);
					S9xSetByte_SA1(Registers.PB, Registers.S.W--);

					//PushW(Registers.PCw);
					S9xSetWord_SA1_Write10_WrapBank(Registers.PCw, Registers.S.W - 1);
					Registers.S.W -= 2;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushB(Registers.PL);
					S9xSetByte_SA1(Registers.PL, Registers.S.W--);

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					int32_t mask = ((sa1_condition_0x40) | -(sa1_condition_0x40)) >> 31;
					uint16 addr = (((0) & mask) | ((S9xGetWord(0xFFEE)) & ~mask));
					int32_t result_openbus = (((Memory.FillRAM[0x220f]) & mask) | ((addr >> 8) & ~mask));
					int32_t result_s9xsetpcbase_sa1 = (((Memory.FillRAM[0x220e] | (Memory.FillRAM[0x220f] << 8)) & mask) | (((addr) & ~mask)));
					int32_t result_cpucycles = (((CPU.Cycles + (TWO_TIMES_SLOW_ONE_CYCLE)) & mask) | ((CPU.Cycles) & ~mask));
					OpenBus = result_openbus;
					CPU.Cycles = result_cpucycles;
					S9xSetPCBase_SA1(result_s9xsetpcbase_sa1);


					/*
					   if (sa1_condition_0x40)
					   {
					   OpenBus = Memory.FillRAM[0x220f];
					   AddCycles(TWO_TIMES_SLOW_ONE_CYCLE);
					   S9xSetPCBase_SA1(Memory.FillRAM[0x220e] | (Memory.FillRAM[0x220f] << 8));
					   }
					   else
					   {
					   uint16	addr = S9xGetWord(0xFFEE);
					   OpenBus = addr >> 8;
					   S9xSetPCBase_SA1(addr);
					   }
					 */
				}
				else
				{
					//PushWE(Registers.PCw);
					Registers.SL--;
					//S9xSetWord_SA1_Write10(Registers.PCw, Registers.S.W, WRAP_PAGE);
					S9xSetWord_SA1_Write10_WrapPage(Registers.PCw, Registers.S.W);
					Registers.SL--;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushBE(Registers.PL);
					S9xSetByte_SA1(Registers.PL, Registers.S.W);
					Registers.SL--;

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					int32_t mask = ((sa1_condition_0x40) | -(sa1_condition_0x40)) >> 31;
					uint16 addr = (((0) & mask) | ((S9xGetWord(0xFFEE)) & ~mask));
					int32_t result_openbus = (((Memory.FillRAM[0x220f]) & mask) | ((addr >> 8) & ~mask));
					int32_t result_s9xsetpcbase_sa1 = (((Memory.FillRAM[0x220e] | (Memory.FillRAM[0x220f] << 8)) & mask) | (((addr) & ~mask)));
					int32_t result_cpucycles = (((CPU.Cycles + (TWO_TIMES_SLOW_ONE_CYCLE)) & mask) | ((CPU.Cycles) & ~mask));
					OpenBus = result_openbus;
					CPU.Cycles = result_cpucycles;
					S9xSetPCBase_SA1(result_s9xsetpcbase_sa1);

					/*
					   if (sa1_condition_0x40)
					   {
					   OpenBus = Memory.FillRAM[0x220f];
					   AddCycles(TWO_TIMES_SLOW_ONE_CYCLE);
					   S9xSetPCBase_SA1(Memory.FillRAM[0x220e] | (Memory.FillRAM[0x220f] << 8));
					   }
					   else
					   {
					   uint16	addr = S9xGetWord(0xFFFE);
					   OpenBus = addr >> 8;
					   S9xSetPCBase_SA1(addr);
					   }
					 */
				}
			}
		}
		else
			CPU.Flags &= ~IRQ_FLAG;
	}
}

if (CPU.Flags & SCAN_KEYS_FLAG)
	break;


#ifdef CPU_SHUTDOWN
	CPU.PBPCAtOpcodeStart = Registers.PBPC;
#endif

	register uint8				Op;
	register struct	SOpcodes	*Opcodes;

	CPU.PrevCycles = CPU.Cycles;

if (CPU.PCBase)
{
	Op = CPU.PCBase[Registers.PCw];
	CPU.Cycles += CPU.MemSpeed;
	Opcodes = ICPU.S9xOpcodes;
}
else
{
	Op = S9xGetByte(Registers.PBPC);
	OpenBus = Op;
	Opcodes = S9xOpcodesSlow;
}

if ((Registers.PCw & MEMMAP_MASK) + ICPU.S9xOpLengths[Op] >= MEMMAP_BLOCK_SIZE)
{
	uint8	*oldPCBase = CPU.PCBase;

	CPU.PCBase = S9xGetBasePointer(ICPU.ShiftedPB + ((uint16) (Registers.PCw + 4)));
	if (oldPCBase != CPU.PCBase || (Registers.PCw & ~MEMMAP_MASK) == (0xffff & ~MEMMAP_MASK))
		Opcodes = S9xOpcodesSlow;
}

Registers.PCw++;
(*Opcodes[Op].S9xOpcode)();

if (SA1.Executing)
	S9xSA1MainLoop();

#if (S9X_ACCURACY_LEVEL <= 2)
	do{
		S9xDoHEventProcessing();
	}while(CPU.Cycles >= CPU.NextEvent);
#endif
}while(1);


Registers.PL &= ~(Zero | Negative | Carry | Overflow);
Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

if (CPU.Flags & SCAN_KEYS_FLAG)
{
	//S9xSyncSpeed();
	//begin of S9xSyncSpeed
	uint32_t audio_alive = audio_driver->alive(audio_handle);

	if (Settings.Throttled && audio_alive)
		do{
			sys_timer_usleep(500);
			// Ye, everyone and their mothers hate busywaiting, but s9x doesn't give us a choice :(
		}while (!S9xSyncSound());
	else
	{
		// Drain the sound buffer, but do not use it.
		uint32 buf[1024];
		while (S9xGetSampleCount() >= 512)
			S9xMixSamples_Mute(buf);
	}

	/*
	   if(Settings.HighSpeedSeek > 0)
	   {
	   Settings.HighSpeedSeek--;
	   }
	 */

	if(!Settings.TurboMode)
		IPPU.RenderThisFrame = true;
	else
	{
		if (++IPPU.FrameSkip >= Settings.TurboSkipFrames)
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = true;
		}
		else
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = false;
		}

	}

	//end of S9xSyncSpeed
	CPU.Flags &= ~SCAN_KEYS_FLAG;
}
}
void SuperFXS9xMainLoop (void)
{

	do
	{
		if ((CPU.Flags & NMI_FLAG) && Timings.NMITriggerPos <= CPU.Cycles)
		{
			CPU.Flags &= ~NMI_FLAG;
			Timings.NMITriggerPos = 0xffff;
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
			Registers.PCw = result_register_pcw;

			//S9xOpcode_NMI();
#if (S9X_ACCURACY_LEVEL >= 3)
#undef AddCycles
#define AddCycles(n)	{ CPU.Cycles += (n); while (CPU.Cycles >= CPU.NextEvent) { \
	switch (CPU.WhichEvent) \
	{ \
		case HC_HBLANK_START_EVENT: \
					    S9xCheckMissingHTimerPosition(Timings.HBlankStart); \
		S9xReschedule(); \
		break; \
		\
		case HC_HDMA_START_EVENT: \
					  S9xCheckMissingHTimerPosition(Timings.HDMAStart); \
		S9xReschedule(); \
		\
		if (PPU.HDMA && CPU.V_Counter <= PPU.ScreenHeight) \
		{ \
			PPU.HDMA = S9xDoHDMA_NoUniracers_SuperFX(PPU.HDMA); \
		} \
		\
		break; \
		\
		case HC_HCOUNTER_MAX_EVENT: \
					    if (!SuperFX.oneLineDone) \
		S9xSuperFXExec(); \
		SuperFX.oneLineDone = FALSE; \
		S9xAPUEndScanline(); \
		CPU.Cycles -= Timings.H_Max; \
		S9xAPUSetReferenceTime(CPU.Cycles); \
		\
		if ((Timings.NMITriggerPos != 0xffff) && (Timings.NMITriggerPos >= Timings.H_Max)) \
		Timings.NMITriggerPos -= Timings.H_Max; \
		\
		CPU.V_Counter++; \
		if (CPU.V_Counter >= Timings.V_Max)	\
		{ \
			CPU.V_Counter = 0; \
			Timings.InterlaceField ^= 1; \
			\
			addition = (IPPU.Interlace & ~(Timings.InterlaceField)); \
			Timings.V_Max = Timings.V_Max_Master + addition; \
			\
			Memory.FillRAM[0x213F] ^= 0x80; \
			PPU.RangeTimeOver = 0; \
			\
			Memory.FillRAM[0x4210] = MAX_5A22_VERSION; \
			CPU.Flags &= ~NMI_FLAG; \
			Timings.NMITriggerPos = 0xffff; \
			PPU.HVBeamCounterLatched = 0; \
			CPU.Flags |= SCAN_KEYS_FLAG; \
		} \
		\
		if (CPU.V_Counter == 240 && !IPPU.Interlace && Timings.InterlaceField) \
		Timings.H_Max = Timings.H_Max_Master - ONE_DOT_CYCLE; \
		else \
		Timings.H_Max = Timings.H_Max_Master; \
		if (CPU.V_Counter != 240 || IPPU.Interlace || !Timings.InterlaceField)	\
		{ \
			if (Timings.WRAMRefreshPos == SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE)	\
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2;					\
			else \
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE;	\
		} \
		S9xCheckMissingHTimerPosition(0); \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)	\
		{ \
			S9xEndScreenRefresh_SuperFX(); \
			PPU.HDMA = 0; \
			IPPU.MaxBrightness = PPU.Brightness; \
			PPU.ForcedBlanking = (Memory.FillRAM[0x2100] >> 7) & 1; \
			\
			if (!PPU.ForcedBlanking) \
			{ \
				PPU.OAMAddr = PPU.SavedOAMAddr; \
				\
				uint8	tmp = 0; \
				\
				if (PPU.OAMPriorityRotation) \
				tmp = (PPU.OAMAddr & 0xFE) >> 1; \
				if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp) \
				{ \
					PPU.FirstSprite = tmp; \
					IPPU.OBJChanged = TRUE; \
				} \
				\
				PPU.OAMFlip = 0; \
			} \
			\
			Memory.FillRAM[0x4210] = 0x80 | MAX_5A22_VERSION; \
			if (Memory.FillRAM[0x4200] & 0x80) \
			{ \
				CPU.Flags |= NMI_FLAG; \
				Timings.NMITriggerPos = 6 + 6; \
			} \
			\
		} \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + 3) \
		{ \
			if (Memory.FillRAM[0x4200] & 1) \
			S9xDoAutoJoypad_SuperFX(); \
		} \
		\
		if (CPU.V_Counter == FIRST_VISIBLE_LINE)	\
		S9xStartScreenRefresh_SuperFX(); \
		\
		CPU.NextEvent = -1; \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_HDMA_INIT_EVENT: \
					 S9xCheckMissingHTimerPosition(Timings.HDMAInit); \
		S9xReschedule(); \
		\
		if (CPU.V_Counter == 0) \
		{ \
			S9xStartHDMA_SuperFX(); \
		} \
		\
		break; \
		\
		case HC_RENDER_EVENT: \
				      if (CPU.V_Counter >= FIRST_VISIBLE_LINE && CPU.V_Counter <= PPU.ScreenHeight) \
		RenderLine((uint8) (CPU.V_Counter - FIRST_VISIBLE_LINE)); \
		\
		S9xCheckMissingHTimerPosition(SNES_RENDER_START_HC); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_WRAM_REFRESH_EVENT: \
					    \
		if ((PPU.HTimerPosition >= Timings.WRAMRefreshPos) && (PPU.HTimerPosition < (Timings.WRAMRefreshPos + SNES_WRAM_REFRESH_CYCLES))) \
		{ \
			if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
			CPU.IRQPending = 1; \
			else \
			if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
			CPU.IRQPending = 1; \
		} \
		CPU.Cycles += SNES_WRAM_REFRESH_CYCLES; \
		\
		S9xCheckMissingHTimerPosition(Timings.WRAMRefreshPos); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_IRQ_1_3_EVENT: \
		case HC_IRQ_3_5_EVENT: \
		case HC_IRQ_5_7_EVENT: \
		case HC_IRQ_7_9_EVENT: \
		case HC_IRQ_9_A_EVENT: \
		case HC_IRQ_A_1_EVENT: \
				       if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
				       else \
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
		S9xReschedule(); \
		break; \
	} \
} \
			} 
#else
#undef AddCycles
#define AddCycles(n)	{ CPU.Cycles += (n); }
#endif


			// IRQ and NMI do an opcode fetch as their first "IO" cycle.
			uint64_t addition;
			AddCycles(CPU.MemSpeed + ONE_CYCLE);

if (!CheckEmulation())
{
	//PushB(Registers.PB);
	S9xSetByte_SuperFX(Registers.PB, Registers.S.W--);

	//PushW(Registers.PCw);
	S9xSetWord_SuperFX_Write10(Registers.PCw, Registers.S.W - 1, WRAP_BANK);
	Registers.S.W -= 2;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushB(Registers.PL);
	S9xSetByte_SuperFX(Registers.PL, Registers.S.W--);

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	uint16	addr = S9xGetWord_WrapNone_SuperFX(0xFFEA);
	OpenBus = addr >> 8;
	S9xSetPCBase_SuperFX(addr);
}
else
{
	//PushWE(Registers.PCw);
	Registers.SL--; 
	S9xSetWord_SuperFX_Write10(Registers.PCw, Registers.S.W, WRAP_PAGE); 
	Registers.SL--;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushBE(Registers.PL);
	S9xSetByte_SuperFX(Registers.PL, Registers.S.W);
	Registers.SL--;

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	uint16	addr = S9xGetWord_WrapNone_SuperFX(0xFFFA);
	OpenBus = addr >> 8;
	S9xSetPCBase_SuperFX(addr);
}
}


if (CPU.Flags & IRQ_FLAG)
{
	if (CPU.IRQPending)
		// FIXME: In case of IRQ during WRAM refresh
		CPU.IRQPending--;
	else
	{
		int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
		uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
		uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
		CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
		Registers.PCw = result_register_pcw;

		if (CPU.IRQActive)
		{
			if (!CheckFlag(IRQ))
				// in IRQ handler $4211 is supposed to be read, so IRQ_FLAG should be cleared.
				//S9xOpcode_IRQ();
			{

				// IRQ and NMI do an opcode fetch as their first "IO" cycle.
				uint64_t addition;
				AddCycles(CPU.MemSpeed + ONE_CYCLE);

				if (!CheckEmulation())
				{
					//PushB(Registers.PB);
					S9xSetByte_SuperFX(Registers.PB, Registers.S.W--);

					//PushW(Registers.PCw);
					S9xSetWord_SuperFX_Write10(Registers.PCw, Registers.S.W - 1, WRAP_BANK);
					Registers.S.W -= 2;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushB(Registers.PL);
					S9xSetByte_SuperFX(Registers.PL, Registers.S.W--);

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					uint16	addr = S9xGetWord_WrapNone_SuperFX(0xFFEE);
					OpenBus = addr >> 8;
					S9xSetPCBase_SuperFX(addr);
				}
				else
				{
					//PushWE(Registers.PCw);
					Registers.SL--;
					S9xSetWord_SuperFX_Write10(Registers.PCw, Registers.S.W, WRAP_PAGE);
					Registers.SL--;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushBE(Registers.PL);
					S9xSetByte_SuperFX(Registers.PL, Registers.S.W);
					Registers.SL--;

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					uint16	addr = S9xGetWord_WrapNone_SuperFX(0xFFFE);
					OpenBus = addr >> 8;
					S9xSetPCBase_SuperFX(addr);
				}
			}
		}
		else
			CPU.Flags &= ~IRQ_FLAG;
	}
}

if (CPU.Flags & SCAN_KEYS_FLAG)
	break;


#ifdef CPU_SHUTDOWN
	CPU.PBPCAtOpcodeStart = Registers.PBPC;
#endif

	register uint8				Op;
	register struct	SOpcodes	*Opcodes;

	CPU.PrevCycles = CPU.Cycles;

if (CPU.PCBase)
{
	Op = CPU.PCBase[Registers.PCw];
	CPU.Cycles += CPU.MemSpeed;
	Opcodes = ICPU.S9xOpcodes;
}
else
{
	Op = S9xGetByte_SuperFX(Registers.PBPC);
	OpenBus = Op;
	Opcodes = S9xOpcodesSlow;
}

if ((Registers.PCw & MEMMAP_MASK) + ICPU.S9xOpLengths[Op] >= MEMMAP_BLOCK_SIZE)
{
	uint8	*oldPCBase = CPU.PCBase;

	CPU.PCBase = S9xGetBasePointer(ICPU.ShiftedPB + ((uint16) (Registers.PCw + 4)));
	if (oldPCBase != CPU.PCBase || (Registers.PCw & ~MEMMAP_MASK) == (0xffff & ~MEMMAP_MASK))
		Opcodes = S9xOpcodesSlow;
}

Registers.PCw++;
(*Opcodes[Op].S9xOpcode)();

#if (S9X_ACCURACY_LEVEL <= 2)
do{
	S9xDoHEventProcessing_SuperFX();
}while(CPU.Cycles >= CPU.NextEvent);
#endif
}while(1);

Registers.PL &= ~(Zero | Negative | Carry | Overflow);
Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

if (CPU.Flags & SCAN_KEYS_FLAG)
{
	//S9xSyncSpeed();
	//begin of S9xSyncSpeed
	uint32_t audio_alive = audio_driver->alive(audio_handle);

	if (Settings.Throttled && audio_alive)
		do{
			sys_timer_usleep(500);
			// Ye, everyone and their mothers hate busywaiting, but s9x doesn't give us a choice :(
		}while (!S9xSyncSound());
	else
	{
		// Drain the sound buffer, but do not use it.
		uint32 buf[1024];
		while (S9xGetSampleCount() >= 512)
			S9xMixSamples_Mute(buf);
	}

	/*
	   if(Settings.HighSpeedSeek > 0)
	   {
	   Settings.HighSpeedSeek--;
	   }
	 */

	if(!Settings.TurboMode)
		IPPU.RenderThisFrame = true;
	else
	{
		if (++IPPU.FrameSkip >= Settings.TurboSkipFrames)
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = true;
		}
		else
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = false;
		}

	}

#if 0
	//Frameskipping code
	static uint64_t next_frame_time = 0;

	uint64_t current_frame_time = sys_time_get_system_time();

	if (next_frame_time == 0)
		next_frame_time = current_frame_time + 1;
	// Frame went by faster than required, no need to skip.
	if (current_frame_time <= next_frame_time)
	{
		IPPU.RenderThisFrame = true;
		IPPU.SkippedFrames = 0;
	}
	else
	{
		unsigned limit = (Settings.SkipFrames == AUTO_FRAMERATE) ? 3 : Settings.SkipFrames;
		// We can't go on skipping forever.
		if (++IPPU.SkippedFrames >= limit)
		{
			IPPU.RenderThisFrame = true;
			IPPU.SkippedFrames = 0;
			next_frame_time = current_frame_time;
		}
		else
			IPPU.RenderThisFrame = false;
	}
	next_frame_time += Settings.FrameTime; 
	//End frameskipping code
#endif
	//end of S9xSyncSpeed
	CPU.Flags &= ~SCAN_KEYS_FLAG;
}
}


void S9xMainLoop (void)
{

	do
	{
		if ((CPU.Flags & NMI_FLAG) && Timings.NMITriggerPos <= CPU.Cycles)
		{
			CPU.Flags &= ~NMI_FLAG;
			Timings.NMITriggerPos = 0xffff;
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
			Registers.PCw = result_register_pcw;

			//S9xOpcode_NMI();
#if (S9X_ACCURACY_LEVEL >= 3)
#undef AddCycles
#define AddCycles(n)	{ CPU.Cycles += (n); while (CPU.Cycles >= CPU.NextEvent) { \
	switch (CPU.WhichEvent) \
	{ \
		case HC_HBLANK_START_EVENT: \
					    S9xCheckMissingHTimerPosition(Timings.HBlankStart); \
		S9xReschedule(); \
		break; \
		\
		case HC_HDMA_START_EVENT: \
					  S9xCheckMissingHTimerPosition(Timings.HDMAStart); \
		S9xReschedule(); \
		\
		if (PPU.HDMA && CPU.V_Counter <= PPU.ScreenHeight) \
		{ \
			PPU.HDMA = S9xDoHDMA(PPU.HDMA); \
		} \
		\
		break; \
		\
		case HC_HCOUNTER_MAX_EVENT: \
					    S9xAPUEndScanline(); \
		CPU.Cycles -= Timings.H_Max; \
		S9xAPUSetReferenceTime(CPU.Cycles); \
		\
		if ((Timings.NMITriggerPos != 0xffff) && (Timings.NMITriggerPos >= Timings.H_Max)) \
		Timings.NMITriggerPos -= Timings.H_Max; \
		\
		CPU.V_Counter++; \
		if (CPU.V_Counter >= Timings.V_Max)	\
		{ \
			CPU.V_Counter = 0; \
			Timings.InterlaceField ^= 1; \
			\
			addition = (IPPU.Interlace & ~(Timings.InterlaceField)); \
			Timings.V_Max = Timings.V_Max_Master + addition;	\
			\
			Memory.FillRAM[0x213F] ^= 0x80; \
			PPU.RangeTimeOver = 0; \
			\
			Memory.FillRAM[0x4210] = MAX_5A22_VERSION; \
			CPU.Flags &= ~NMI_FLAG; \
			Timings.NMITriggerPos = 0xffff; \
			PPU.HVBeamCounterLatched = 0; \
			CPU.Flags |= SCAN_KEYS_FLAG; \
		} \
		\
		if (CPU.V_Counter == 240 && !IPPU.Interlace && Timings.InterlaceField) \
		Timings.H_Max = Timings.H_Max_Master - ONE_DOT_CYCLE; \
		else \
		Timings.H_Max = Timings.H_Max_Master; \
		if (CPU.V_Counter != 240 || IPPU.Interlace || !Timings.InterlaceField)	\
		{ \
			if (Timings.WRAMRefreshPos == SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE)	\
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2;					\
			else \
			Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE;	\
		} \
		S9xCheckMissingHTimerPosition(0); \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)	\
		{ \
			S9xEndScreenRefresh(); \
			PPU.HDMA = 0; \
			IPPU.MaxBrightness = PPU.Brightness; \
			PPU.ForcedBlanking = (Memory.FillRAM[0x2100] >> 7) & 1; \
			\
			if (!PPU.ForcedBlanking) \
			{ \
				PPU.OAMAddr = PPU.SavedOAMAddr; \
				\
				uint8	tmp = 0; \
				\
				if (PPU.OAMPriorityRotation) \
				tmp = (PPU.OAMAddr & 0xFE) >> 1; \
				if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp) \
				{ \
					PPU.FirstSprite = tmp; \
					IPPU.OBJChanged = TRUE; \
				} \
				\
				PPU.OAMFlip = 0; \
			} \
			\
			Memory.FillRAM[0x4210] = 0x80 | MAX_5A22_VERSION; \
			if (Memory.FillRAM[0x4200] & 0x80) \
			{ \
				CPU.Flags |= NMI_FLAG; \
				Timings.NMITriggerPos = 6 + 6; \
			} \
			\
		} \
		\
		if (CPU.V_Counter == PPU.ScreenHeight + 3) \
		{ \
			if (Memory.FillRAM[0x4200] & 1) \
			S9xDoAutoJoypad(); \
		} \
		\
		if (CPU.V_Counter == FIRST_VISIBLE_LINE)	\
		S9xStartScreenRefresh(); \
		\
		CPU.NextEvent = -1; \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_HDMA_INIT_EVENT: \
					 S9xCheckMissingHTimerPosition(Timings.HDMAInit); \
		S9xReschedule(); \
		\
		if (CPU.V_Counter == 0) \
		{ \
			S9xStartHDMA(); \
		} \
		\
		break; \
		\
		case HC_RENDER_EVENT: \
				      if (CPU.V_Counter >= FIRST_VISIBLE_LINE && CPU.V_Counter <= PPU.ScreenHeight) \
		RenderLine((uint8) (CPU.V_Counter - FIRST_VISIBLE_LINE)); \
		\
		S9xCheckMissingHTimerPosition(SNES_RENDER_START_HC); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_WRAM_REFRESH_EVENT: \
					    \
		if ((PPU.HTimerPosition >= Timings.WRAMRefreshPos) && (PPU.HTimerPosition < (Timings.WRAMRefreshPos + SNES_WRAM_REFRESH_CYCLES))) \
		{ \
			if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
			CPU.IRQPending = 1; \
			else \
			if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
			CPU.IRQPending = 1; \
		} \
		CPU.Cycles += SNES_WRAM_REFRESH_CYCLES; \
		\
		S9xCheckMissingHTimerPosition(Timings.WRAMRefreshPos); \
		S9xReschedule(); \
		\
		break; \
		\
		case HC_IRQ_1_3_EVENT: \
		case HC_IRQ_3_5_EVENT: \
		case HC_IRQ_5_7_EVENT: \
		case HC_IRQ_7_9_EVENT: \
		case HC_IRQ_9_A_EVENT: \
		case HC_IRQ_A_1_EVENT: \
				       if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition))) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
				       else \
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition)) \
		{ \
			CPU.IRQActive |= PPU_IRQ_SOURCE; \
			CPU.IRQPending = Timings.IRQPendCount; \
			CPU.Flags |= IRQ_FLAG; \
			int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31; \
			uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask); \
			uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask); \
			CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt; \
			Registers.PCw = result_register_pcw; \
		} \
		S9xReschedule(); \
		break; \
	} \
} \
			} 
#else
#undef AddCycles
#define AddCycles(n)	{ CPU.Cycles += (n); }
#endif


			// IRQ and NMI do an opcode fetch as their first "IO" cycle.
			uint64_t addition;
			AddCycles(CPU.MemSpeed + ONE_CYCLE);

if (!CheckEmulation())
{
	//PushB(Registers.PB);
	S9xSetByte_NoSA1(Registers.PB, Registers.S.W--);

	//PushW(Registers.PCw);
	S9xSetWord_Write10_WrapBank_NoSA1(Registers.PCw, Registers.S.W - 1);
	Registers.S.W -= 2;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushB(Registers.PL);
	S9xSetByte_NoSA1(Registers.PL, Registers.S.W--);

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	uint16	addr = S9xGetWord_WrapNone_NoSA1(0xFFEA);
	OpenBus = addr >> 8;
	S9xSetPCBase_NoSA1(addr);
}
else
{
	//PushWE(Registers.PCw);
	Registers.SL--;
	S9xSetWord_Write10_WrapPage_NoSA1(Registers.PCw, Registers.S.W);
	Registers.SL--;

	Registers.PL &= ~(Zero | Negative | Carry | Overflow);
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

	//PushBE(Registers.PL);
	S9xSetByte_NoSA1(Registers.PL, Registers.S.W);
	Registers.SL--;

	OpenBus = Registers.PL;
	ClearDecimal();
	SetIRQ();

	uint16	addr = S9xGetWord_WrapNone_NoSA1(0xFFFA);
	OpenBus = addr >> 8;
	S9xSetPCBase_NoSA1(addr);
}
}


if (CPU.Flags & IRQ_FLAG)
{
	if (CPU.IRQPending)
		// FIXME: In case of IRQ during WRAM refresh
		CPU.IRQPending--;
	else
	{
		int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
		uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
		uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
		CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
		Registers.PCw = result_register_pcw;

		if (CPU.IRQActive)
		{
			if (!CheckFlag(IRQ))
				// in IRQ handler $4211 is supposed to be read, so IRQ_FLAG should be cleared.
				//S9xOpcode_IRQ();
			{

				// IRQ and NMI do an opcode fetch as their first "IO" cycle.
				uint64_t addition;
				AddCycles(CPU.MemSpeed + ONE_CYCLE);

				if (!CheckEmulation())
				{
					//PushB(Registers.PB);
					S9xSetByte_NoSA1(Registers.PB, Registers.S.W--);

					//PushW(Registers.PCw);
					S9xSetWord_Write10_WrapBank_NoSA1(Registers.PCw, Registers.S.W - 1);
					Registers.S.W -= 2;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushB(Registers.PL);
					S9xSetByte_NoSA1(Registers.PL, Registers.S.W--);

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					uint16	addr = S9xGetWord_WrapNone_NoSA1(0xFFEE);
					OpenBus = addr >> 8;
					S9xSetPCBase_NoSA1(addr);
				}
				else
				{
					//PushWE(Registers.PCw);
					Registers.SL--;
					S9xSetWord_Write10_WrapPage_NoSA1(Registers.PCw, Registers.S.W);
					Registers.SL--;

					Registers.PL &= ~(Zero | Negative | Carry | Overflow);
					Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

					//PushBE(Registers.PL);
					S9xSetByte_NoSA1(Registers.PL, Registers.S.W);
					Registers.SL--;

					OpenBus = Registers.PL;
					ClearDecimal();
					SetIRQ();

					uint16	addr = S9xGetWord_WrapNone_NoSA1(0xFFFE);
					OpenBus = addr >> 8;
					S9xSetPCBase_NoSA1(addr);
				}
			}
		}
		else
			CPU.Flags &= ~IRQ_FLAG;
	}
}

if (CPU.Flags & SCAN_KEYS_FLAG)
	break;


#ifdef CPU_SHUTDOWN
	CPU.PBPCAtOpcodeStart = Registers.PBPC;
#endif

	register uint8				Op;
	register struct	SOpcodes	*Opcodes;

	CPU.PrevCycles = CPU.Cycles;

if (CPU.PCBase)
{
	Op = CPU.PCBase[Registers.PCw];
	CPU.Cycles += CPU.MemSpeed;
	Opcodes = ICPU.S9xOpcodes;
}
else
{
	Op = S9xGetByte(Registers.PBPC);
	OpenBus = Op;
	Opcodes = S9xOpcodesSlow;
}

if ((Registers.PCw & MEMMAP_MASK) + ICPU.S9xOpLengths[Op] >= MEMMAP_BLOCK_SIZE)
{
	uint8	*oldPCBase = CPU.PCBase;

	CPU.PCBase = S9xGetBasePointer(ICPU.ShiftedPB + ((uint16) (Registers.PCw + 4)));
	if (oldPCBase != CPU.PCBase || (Registers.PCw & ~MEMMAP_MASK) == (0xffff & ~MEMMAP_MASK))
		Opcodes = S9xOpcodesSlow;
}

Registers.PCw++;
(*Opcodes[Op].S9xOpcode)();

#if (S9X_ACCURACY_LEVEL <= 2)
do{
	S9xDoHEventProcessing();
}while(CPU.Cycles >= CPU.NextEvent);
#endif
}while(1);

Registers.PL &= ~(Zero | Negative | Carry | Overflow);
Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);

if (CPU.Flags & SCAN_KEYS_FLAG)
{
	//S9xSyncSpeed();
	//begin of S9xSyncSpeed
	uint32_t audio_alive = audio_driver->alive(audio_handle);

	if (Settings.Throttled && audio_alive)
		do{
			sys_timer_usleep(500);
			// Ye, everyone and their mothers hate busywaiting, but s9x doesn't give us a choice :(
		}while (!S9xSyncSound());
	else
	{
		// Drain the sound buffer, but do not use it.
		uint32 buf[1024];
		while (S9xGetSampleCount() >= 512)
			S9xMixSamples_Mute(buf);
	}

	/*
	   if(Settings.HighSpeedSeek > 0)
	   {
	   Settings.HighSpeedSeek--;
	   }
	 */

	if(!Settings.TurboMode)
		IPPU.RenderThisFrame = true;
	else
	{
		if (++IPPU.FrameSkip >= Settings.TurboSkipFrames)
		{
			IPPU.FrameSkip = 0;
			IPPU.SkippedFrames = 0;
			IPPU.RenderThisFrame = true;
		}
		else
		{
			IPPU.SkippedFrames = IPPU.SkippedFrames + 1;
			IPPU.RenderThisFrame = false;
		}

	}

	//end of S9xSyncSpeed
	CPU.Flags &= ~SCAN_KEYS_FLAG;
}
}

void S9xSetIRQ (uint32 source)
{
	CPU.IRQActive |= source;
	CPU.IRQPending = Timings.IRQPendCount;
	CPU.Flags |= IRQ_FLAG;

	if (CPU.WaitingForInterrupt)
	{
		// Force IRQ to trigger immediately after WAI -
		// Final Fantasy Mystic Quest crashes without this.
		CPU.WaitingForInterrupt = FALSE;
		Registers.PCw++;
	}

}

void S9xClearIRQ (uint32 source)
{
	CPU.IRQActive &= ~source;
	if (!CPU.IRQActive)
		CPU.Flags &= ~IRQ_FLAG;

}

void S9xDoHEventProcessing_SuperFX (void)
{


#ifdef CPU_SHUTDOWN
	CPU.WaitCounter++;
#endif

	switch (CPU.WhichEvent)
	{
		case HC_HBLANK_START_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HBlankStart);
			S9xReschedule();
			break;

		case HC_HDMA_START_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HDMAStart);
			S9xReschedule();

			if (PPU.HDMA && CPU.V_Counter <= PPU.ScreenHeight)
			{
				PPU.HDMA = S9xDoHDMA_NoUniracers_SuperFX(PPU.HDMA);
			}

			break;

		case HC_HCOUNTER_MAX_EVENT:
			if (!SuperFX.oneLineDone)
				S9xSuperFXExec();
			SuperFX.oneLineDone = FALSE;

			S9xAPUEndScanline();
			CPU.Cycles -= Timings.H_Max;
			S9xAPUSetReferenceTime(CPU.Cycles);

			if ((Timings.NMITriggerPos != 0xffff) && (Timings.NMITriggerPos >= Timings.H_Max))
				Timings.NMITriggerPos -= Timings.H_Max;

			CPU.V_Counter++;
			if (CPU.V_Counter >= Timings.V_Max)	// V ranges from 0 to Timings.V_Max - 1
			{
				CPU.V_Counter = 0;
				Timings.InterlaceField ^= 1;

				// From byuu:
				// [NTSC]
				// interlace mode has 525 scanlines: 263 on the even frame, and 262 on the odd.
				// non-interlace mode has 524 scanlines: 262 scanlines on both even and odd frames.
				// [PAL] <PAL info is unverified on hardware>
				// interlace mode has 625 scanlines: 313 on the even frame, and 312 on the odd.
				// non-interlace mode has 624 scanlines: 312 scanlines on both even and odd frames.
				uint64_t addition = (IPPU.Interlace & ~(Timings.InterlaceField));
				Timings.V_Max = Timings.V_Max_Master + addition;	// 263 (NTSC), 313?(PAL)

				Memory.FillRAM[0x213F] ^= 0x80;
				PPU.RangeTimeOver = 0;

				// FIXME: reading $4210 will wait 2 cycles, then perform reading, then wait 4 more cycles.
				Memory.FillRAM[0x4210] = MAX_5A22_VERSION;
				CPU.Flags &= ~NMI_FLAG;
				Timings.NMITriggerPos = 0xffff;
				PPU.HVBeamCounterLatched = 0;
				CPU.Flags |= SCAN_KEYS_FLAG;
			}

			// From byuu:
			// In non-interlace mode, there are 341 dots per scanline, and 262 scanlines per frame.
			// On odd frames, scanline 240 is one dot short.
			// In interlace mode, there are always 341 dots per scanline. Even frames have 263 scanlines,
			// and odd frames have 262 scanlines.
			// Interlace mode scanline 240 on odd frames is not missing a dot.
			if (CPU.V_Counter == 240 && !IPPU.Interlace && Timings.InterlaceField)	// V=240
				Timings.H_Max = Timings.H_Max_Master - ONE_DOT_CYCLE;	// HC=1360
			else
				Timings.H_Max = Timings.H_Max_Master;					// HC=1364

#if 0
			if (Model->_5A22 == 2)
			{
#endif
				if (CPU.V_Counter != 240 || IPPU.Interlace || !Timings.InterlaceField)	// V=240
				{
					if (Timings.WRAMRefreshPos == SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE)	// HC=534
						Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2;					// HC=538
					else
						Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE;	// HC=534
				}
#if 0
			}
			else
				Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v1;
#endif

			S9xCheckMissingHTimerPosition(0);

			if (CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)	// VBlank starts from V=225(240).
			{
				S9xEndScreenRefresh_SuperFX();
				PPU.HDMA = 0;
				// Bits 7 and 6 of $4212 are computed when read in S9xGetPPU.
				IPPU.MaxBrightness = PPU.Brightness;
				PPU.ForcedBlanking = (Memory.FillRAM[0x2100] >> 7) & 1;

				if (!PPU.ForcedBlanking)
				{
					PPU.OAMAddr = PPU.SavedOAMAddr;

					uint8	tmp = 0;

					if (PPU.OAMPriorityRotation)
						tmp = (PPU.OAMAddr & 0xFE) >> 1;
					if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp)
					{
						PPU.FirstSprite = tmp;
						IPPU.OBJChanged = TRUE;
					}

					PPU.OAMFlip = 0;
				}

				// FIXME: writing to $4210 will wait 6 cycles.
				Memory.FillRAM[0x4210] = 0x80 | MAX_5A22_VERSION;
				if (Memory.FillRAM[0x4200] & 0x80)
				{
					// FIXME: triggered at HC=6, checked just before the final CPU cycle,
					// then, when to call S9xOpcode_NMI()?
					CPU.Flags |= NMI_FLAG;
					Timings.NMITriggerPos = 6 + 6;
				}

			}

			if (CPU.V_Counter == PPU.ScreenHeight + 3)	// FIXME: not true
			{
				if (Memory.FillRAM[0x4200] & 1)
					S9xDoAutoJoypad_SuperFX();
			}

			if (CPU.V_Counter == FIRST_VISIBLE_LINE)	// V=1
				S9xStartScreenRefresh_SuperFX();

			CPU.NextEvent = -1;
			S9xReschedule();

			break;

		case HC_HDMA_INIT_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HDMAInit);
			S9xReschedule();

			if (CPU.V_Counter == 0)
			{
				S9xStartHDMA();
			}

			break;

		case HC_RENDER_EVENT:
			if (CPU.V_Counter >= FIRST_VISIBLE_LINE && CPU.V_Counter <= PPU.ScreenHeight)
				RenderLine((uint8) (CPU.V_Counter - FIRST_VISIBLE_LINE));

			S9xCheckMissingHTimerPosition(SNES_RENDER_START_HC);
			S9xReschedule();

			break;

		case HC_WRAM_REFRESH_EVENT:

			//S9xCheckMissingHTimerHalt(Timings.WRAMRefreshPos, SNES_WRAM_REFRESH_CYCLES);
			if ((PPU.HTimerPosition >= Timings.WRAMRefreshPos) && (PPU.HTimerPosition < (Timings.WRAMRefreshPos + SNES_WRAM_REFRESH_CYCLES)))
			{
				if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
					CPU.IRQPending = 1;
				else
					if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
						CPU.IRQPending = 1;
			}

			CPU.Cycles += SNES_WRAM_REFRESH_CYCLES;

			S9xCheckMissingHTimerPosition(Timings.WRAMRefreshPos);
			S9xReschedule();

			break;

		case HC_IRQ_1_3_EVENT:
		case HC_IRQ_3_5_EVENT:
		case HC_IRQ_5_7_EVENT:
		case HC_IRQ_7_9_EVENT:
		case HC_IRQ_9_A_EVENT:
		case HC_IRQ_A_1_EVENT:
			if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
			{
				//S9xSetIRQ(PPU_IRQ_SOURCE);
				CPU.IRQActive |= PPU_IRQ_SOURCE;
				CPU.IRQPending = Timings.IRQPendCount;
				CPU.Flags |= IRQ_FLAG;

				int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
				uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
				uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
				CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
				Registers.PCw = result_register_pcw;
			}
			else
				if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
				{
					//S9xSetIRQ(PPU_IRQ_SOURCE);
					CPU.IRQActive |= PPU_IRQ_SOURCE;
					CPU.IRQPending = Timings.IRQPendCount;
					CPU.Flags |= IRQ_FLAG;

					int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
					uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
					uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
					CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
					Registers.PCw = result_register_pcw;
				}

			S9xReschedule();
			break;
	}

}

void S9xDoHEventProcessing (void)
{
#ifdef CPU_SHUTDOWN
	CPU.WaitCounter++;
#endif

	switch (CPU.WhichEvent)
	{
		case HC_HBLANK_START_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HBlankStart);
			S9xReschedule();
			break;

		case HC_HDMA_START_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HDMAStart);
			S9xReschedule();

			if (PPU.HDMA && CPU.V_Counter <= PPU.ScreenHeight)
				PPU.HDMA = S9xDoHDMA(PPU.HDMA);

			break;

		case HC_HCOUNTER_MAX_EVENT:
			if(Settings.SuperFX)
			{
				if (!SuperFX.oneLineDone)
					S9xSuperFXExec();
				SuperFX.oneLineDone = FALSE;
			}

			S9xAPUEndScanline();
			CPU.Cycles -= Timings.H_Max;
			S9xAPUSetReferenceTime(CPU.Cycles);

			if ((Timings.NMITriggerPos != 0xffff) && (Timings.NMITriggerPos >= Timings.H_Max))
				Timings.NMITriggerPos -= Timings.H_Max;

			CPU.V_Counter++;
			if (CPU.V_Counter >= Timings.V_Max)	// V ranges from 0 to Timings.V_Max - 1
			{
				CPU.V_Counter = 0;
				Timings.InterlaceField ^= 1;

				// From byuu:
				// [NTSC]
				// interlace mode has 525 scanlines: 263 on the even frame, and 262 on the odd.
				// non-interlace mode has 524 scanlines: 262 scanlines on both even and odd frames.
				// [PAL] <PAL info is unverified on hardware>
				// interlace mode has 625 scanlines: 313 on the even frame, and 312 on the odd.
				// non-interlace mode has 624 scanlines: 312 scanlines on both even and odd frames.
				uint64_t addition = (IPPU.Interlace & ~(Timings.InterlaceField));
				Timings.V_Max = Timings.V_Max_Master + addition;	// 263 (NTSC), 313?(PAL)
				// 262 (NTSC), 312?(PAL)

				Memory.FillRAM[0x213F] ^= 0x80;
				PPU.RangeTimeOver = 0;

				// FIXME: reading $4210 will wait 2 cycles, then perform reading, then wait 4 more cycles.
				Memory.FillRAM[0x4210] = MAX_5A22_VERSION;
				CPU.Flags &= ~NMI_FLAG;
				Timings.NMITriggerPos = 0xffff;
				PPU.HVBeamCounterLatched = 0;
				CPU.Flags |= SCAN_KEYS_FLAG;
			}

			// From byuu:
			// In non-interlace mode, there are 341 dots per scanline, and 262 scanlines per frame.
			// On odd frames, scanline 240 is one dot short.
			// In interlace mode, there are always 341 dots per scanline. Even frames have 263 scanlines,
			// and odd frames have 262 scanlines.
			// Interlace mode scanline 240 on odd frames is not missing a dot.
			if (CPU.V_Counter == 240 && !IPPU.Interlace && Timings.InterlaceField)	// V=240
				Timings.H_Max = Timings.H_Max_Master - ONE_DOT_CYCLE;	// HC=1360
			else
				Timings.H_Max = Timings.H_Max_Master;					// HC=1364

#if 0
			if (Model->_5A22 == 2)
			{
#endif
				if (CPU.V_Counter != 240 || IPPU.Interlace || !Timings.InterlaceField)	// V=240
				{
					if (Timings.WRAMRefreshPos == SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE)	// HC=534
						Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2;					// HC=538
					else
						Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v2 - ONE_DOT_CYCLE;	// HC=534
				}
#if 0
			}
			else
				Timings.WRAMRefreshPos = SNES_WRAM_REFRESH_HC_v1;
#endif

			S9xCheckMissingHTimerPosition(0);

			if (CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)	// VBlank starts from V=225(240).
			{
				S9xEndScreenRefresh();
				PPU.HDMA = 0;
				// Bits 7 and 6 of $4212 are computed when read in S9xGetPPU.
				IPPU.MaxBrightness = PPU.Brightness;
				PPU.ForcedBlanking = (Memory.FillRAM[0x2100] >> 7) & 1;

				if (!PPU.ForcedBlanking)
				{
					PPU.OAMAddr = PPU.SavedOAMAddr;

					uint8	tmp = 0;

					if (PPU.OAMPriorityRotation)
						tmp = (PPU.OAMAddr & 0xFE) >> 1;
					if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp)
					{
						PPU.FirstSprite = tmp;
						IPPU.OBJChanged = TRUE;
					}

					PPU.OAMFlip = 0;
				}

				// FIXME: writing to $4210 will wait 6 cycles.
				Memory.FillRAM[0x4210] = 0x80 | MAX_5A22_VERSION;
				if (Memory.FillRAM[0x4200] & 0x80)
				{
					// FIXME: triggered at HC=6, checked just before the final CPU cycle,
					// then, when to call S9xOpcode_NMI()?
					CPU.Flags |= NMI_FLAG;
					Timings.NMITriggerPos = 6 + 6;
				}

			}

			if (CPU.V_Counter == PPU.ScreenHeight + 3)	// FIXME: not true
			{
				if (Memory.FillRAM[0x4200] & 1)
					S9xDoAutoJoypad();
			}

			if (CPU.V_Counter == FIRST_VISIBLE_LINE)	// V=1
				S9xStartScreenRefresh();

			CPU.NextEvent = -1;
			S9xReschedule();

			break;

		case HC_HDMA_INIT_EVENT:
			S9xCheckMissingHTimerPosition(Timings.HDMAInit);
			S9xReschedule();

			if (CPU.V_Counter == 0)
			{
				S9xStartHDMA();
			}

			break;

		case HC_RENDER_EVENT:
			if (CPU.V_Counter >= FIRST_VISIBLE_LINE && CPU.V_Counter <= PPU.ScreenHeight)
				RenderLine((uint8) (CPU.V_Counter - FIRST_VISIBLE_LINE));

			S9xCheckMissingHTimerPosition(SNES_RENDER_START_HC);
			S9xReschedule();

			break;

		case HC_WRAM_REFRESH_EVENT:

			//S9xCheckMissingHTimerHalt(Timings.WRAMRefreshPos, SNES_WRAM_REFRESH_CYCLES);
			if ((PPU.HTimerPosition >= Timings.WRAMRefreshPos) && (PPU.HTimerPosition < (Timings.WRAMRefreshPos + SNES_WRAM_REFRESH_CYCLES)))
			{
				if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
					CPU.IRQPending = 1;
				else
					if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
						CPU.IRQPending = 1;
			}

			CPU.Cycles += SNES_WRAM_REFRESH_CYCLES;

			S9xCheckMissingHTimerPosition(Timings.WRAMRefreshPos);
			S9xReschedule();

			break;

		case HC_IRQ_1_3_EVENT:
		case HC_IRQ_3_5_EVENT:
		case HC_IRQ_5_7_EVENT:
		case HC_IRQ_7_9_EVENT:
		case HC_IRQ_9_A_EVENT:
		case HC_IRQ_A_1_EVENT:
			if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
			{
				//S9xSetIRQ(PPU_IRQ_SOURCE);
				CPU.IRQActive |= PPU_IRQ_SOURCE;
				CPU.IRQPending = Timings.IRQPendCount;
				CPU.Flags |= IRQ_FLAG;

				int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
				uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
				uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
				CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
				Registers.PCw = result_register_pcw;
			}
			else
				if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
				{
					//S9xSetIRQ(PPU_IRQ_SOURCE);
					CPU.IRQActive |= PPU_IRQ_SOURCE;
					CPU.IRQPending = Timings.IRQPendCount;
					CPU.Flags |= IRQ_FLAG;

					int32_t mask = ((CPU.WaitingForInterrupt) | -(CPU.WaitingForInterrupt)) >> 31;
					uint32_t result_cpu_wait_for_interrupt = ((0) & mask) | ((CPU.WaitingForInterrupt) & ~mask);
					uint32_t result_register_pcw = ((Registers.PCw + 1) & mask) | ((Registers.PCw) & ~mask);
					CPU.WaitingForInterrupt = result_cpu_wait_for_interrupt;
					Registers.PCw = result_register_pcw;
				}

			S9xReschedule();
			break;
	}

}

