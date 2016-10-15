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

  (c) Copyright 2010 - 2016 Daniel De Matteis. (UNDER NO CIRCUMSTANCE 
  WILL COMMERCIAL RIGHTS EVER BE APPROPRIATED TO ANY PARTY)

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


#ifndef _SNES9X_H_
#define _SNES9X_H_

#ifndef VERSION
#define VERSION	"2010"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "port.h"
#include "65c816.h"
#include "messages.h"

#define ACCESSORY_AUTODETECTION_CONFIRM   0
#define ACCESSORY_AUTODETECTION_ENABLED   1
#define ACCESSORY_AUTODETECTION_NONE      2

#define S9X_ACCURACY_LEVEL		3

#include "../libretro/libretro-common/include/streams/memory_stream.h"
 /* Create some sort of abstraction for files 
  * using memory only since libretro API is memory oriented.
  *
  * Very hacky, but at least allows us to avoid hacking up 
  * the whole source or go through the file system every time 
  * we want to do something. */

#define STREAM memstream_t *
#define READ_STREAM(p, l, s)     memstream_read(s, p, l)
#define WRITE_STREAM(p, l, s)    memstream_write(s, p, l)
#define GETS_STREAM(p, l, s)     memstream_gets(s, p, l)
#define GETC_STREAM(s)           memstream_getc(s)
#define OPEN_STREAM(f, m)        memstream_open(0)
#define FIND_STREAM(f)           memstream_pos(f)
#define REVERT_STREAM(f, o, s)   memstream_seek(f, o, s)
#define CLOSE_STREAM(s)          memstream_close(s)

#define SNES_WIDTH		256
#define SNES_HEIGHT		224
#define SNES_HEIGHT_EXTENDED	239

/* MAX_SNES_WIDTH = SNES_WIDTH * 2 */
#define MAX_SNES_WIDTH		512

/* MAX_SNES_HEIGHT = SNES_HEIGHT_EXTENDED * 2 */
#define MAX_SNES_HEIGHT		478

#define	NTSC_MASTER_CLOCK	21477272.0
#define	PAL_MASTER_CLOCK	21281370.0

#define SNES_MAX_NTSC_VCOUNTER	262
#define SNES_MAX_PAL_VCOUNTER	312
#define SNES_HCOUNTER_MAX	341

#define ONE_CYCLE		6
#define SLOW_ONE_CYCLE		8
#define TWO_CYCLES		12
#define	ONE_DOT_CYCLE		4
#define ONE_DOT_CYCLE_DIV_2	2

#define SNES_CYCLES_PER_SCANLINE	1364

#define SNES_WRAM_REFRESH_HC_v2		538
#define SNES_WRAM_REFRESH_HC_v2_MIN_ONE_DOT_CYCLE	534
#define SNES_WRAM_REFRESH_CYCLES	40

#define SNES_HBLANK_START_HC		1096		/* H=274 */
#define	SNES_HDMA_START_HC		1106		/* FIXME: not true */
#define	SNES_HBLANK_END_HC		4		/* H=1 */
#define	SNES_HDMA_INIT_HC		20		/* FIXME: not true */

/* SNES_RENDER_START_HC = 48 * ONE_DOT_CYCLE */
#define	SNES_RENDER_START_HC		192		/* FIXME: Snes9x renders a line at a time. */

#define SNES_TR_MASK			16
#define SNES_TL_MASK			32
#define SNES_X_MASK			64
#define SNES_A_MASK			128
#define SNES_RIGHT_MASK			256
#define SNES_LEFT_MASK			512
#define SNES_DOWN_MASK			1024
#define SNES_UP_MASK			2048
#define SNES_START_MASK			4096
#define SNES_SELECT_MASK		8192
#define SNES_Y_MASK			16384
#define SNES_B_MASK			32768

#define DEBUG_MODE_FLAG			1	/* debugger */
#define TRACE_FLAG			2	/* debugger */
#define SINGLE_STEP_FLAG		4	/* debugger */
#define BREAK_FLAG			8	/* debugger */
#define SCAN_KEYS_FLAG			16	/* CPU */
#define NMI_FLAG			128	/* CPU */
#define IRQ_FLAG			2048	/* CPU */
#define HALTED_FLAG			4096	/* APU */

struct SCPUState
{
	int32	Cycles;
	int32	PrevCycles;
	int32	V_Counter;
	uint32	Flags;
	uint8	*PCBase;
	bool8	IRQActive;
	int32	IRQPending;
	int32	MemSpeed;
	int32	MemSpeedx2;
	int32	FastROMSpeed;
	bool8	InDMA;
	bool8	InHDMA;
	bool8	InDMAorHDMA;
	bool8	InWRAMDMAorHDMA;
	uint8	HDMARanInDMA;
	int32	CurrentDMAorHDMAChannel;
	uint8	WhichEvent;
	int32	NextEvent;
	bool8	WaitingForInterrupt;
};

#define HC_HBLANK_START_EVENT	1
#define HC_IRQ_1_3_EVENT	2
#define HC_HDMA_START_EVENT	3
#define	HC_IRQ_3_5_EVENT	4
#define HC_HCOUNTER_MAX_EVENT	5
#define HC_IRQ_5_7_EVENT	6
#define HC_HDMA_INIT_EVENT	7
#define HC_IRQ_7_9_EVENT	8
#define HC_RENDER_EVENT		9
#define HC_IRQ_9_A_EVENT	10
#define HC_WRAM_REFRESH_EVENT	11
#define HC_IRQ_A_1_EVENT	12

struct STimings
{
	int32	H_Max_Master;
	int32	H_Max;
	int32	V_Max_Master;
	int32	V_Max;
	int32	HBlankStart;
	int32	HBlankEnd;
	int32	HDMAInit;
	int32	HDMAStart;
	int32	NMITriggerPos;
	int32	WRAMRefreshPos;
	int32	RenderPos;
	bool8	InterlaceField;
	int32	DMACPUSync;	/* The cycles to synchronize DMA and CPU. Snes9x cannot emulate correctly. */
	int32	NMIDMADelay;	/* The delay of NMI trigger after DMA transfers. Snes9x cannot emulate correctly. */
	int32	IRQPendCount;	/* This value is just a hack, because Snes9x cannot emulate any events in an opcode. */
	int32	APUSpeedup;
	bool8	APUAllowTimeOverflow;
};

struct SSettings
{
	bool8	SoundSync;
	bool8	SuperFX;
	uint8	DSP;
	bool8	SA1;
	bool8	C4;
	bool8	SDD1;
	bool8	SPC7110;
	bool8	SPC7110RTC;
	bool8	OBC1;
	uint8	SETA;
	bool8	SRTC;
	bool8	BS;
	bool8	BSXItself;
	bool8	BSXBootup;

	bool8	ForceLoROM;
	bool8	ForceHiROM;
	bool8	ForceHeader;
	bool8	ForceNoHeader;
	bool8	ForceInterleaved;
	bool8	ForceInterleaved2;
	bool8	ForceInterleaveGD24;
	bool8	ForceNotInterleaved;
	bool8	ForcePAL;
	bool8	ForceNTSC;
	bool8	ApplyCheats;
	bool8	NoPatch;
	bool8	PAL;
	uint32	FrameTimePAL;
	uint32	FrameTimeNTSC;

	uint32  SpeedhackGameID;

	uint32	SoundPlaybackRate;
	uint32	SoundInputRate;

	bool8	Multi;
	char	CartAName[PATH_MAX + 1];
	char	CartBName[PATH_MAX + 1];

	bool8	DisableGameSpecificHacks;
	bool8	BlockInvalidVRAMAccessMaster;
	bool8	BlockInvalidVRAMAccess;
	int32	HDMATimingHack;

	uint32	Throttled;
	int32_t	Crosshair;

	uint32	AccessoryType;
	int32		AccessoryAutoDetection;
	bool8		CurrentROMisMultitapCompatible;
	bool8		CurrentROMisMouseCompatible;
	bool8		CurrentROMisSuperScopeCompatible;
	bool8		CurrentROMisJustifierCompatible;
   bool8    NormalControls;
	bool8		SupportHiRes;
	bool8		Transparency;
   float    SuperFXSpeedPerLine;
};

struct SSNESGameFixes
{
	uint8	SRAMInitialValue;
	uint8	Uniracers;
};

void S9xMessage (int type, int number, const char *message);

extern struct SSettings			Settings;
extern struct SCPUState			CPU;
extern struct STimings			Timings;
extern struct SSNESGameFixes	SNESGameFixes;

/* Use when writing to $4016. */
extern void S9xSetJoypadLatch (bool8 latch);

/* Use when reading $4016/7 (JOYSER0 and JOYSER1). */
extern uint8 S9xReadJOYSERn (int n);

extern bool8 pad_read;

#ifdef __cplusplus
}
#endif

#endif
