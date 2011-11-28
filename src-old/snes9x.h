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


#ifndef _SNES9X_H_
#define _SNES9X_H_

#ifndef VERSION
#define VERSION	"1.53"
#endif

#include <string>

#include "port.h"
#include "65c816.h"
#include "messages.h"

#ifdef __CELLOS_LV2__
#include "../emu-ps3-constants.h"
#endif

#define S9X_ACCURACY_LEVEL		3

#ifdef ZLIB
#include <zlib.h>
#define STREAM					gzFile
#define READ_STREAM(p, l, s)	gzread(s, p, l)
#define WRITE_STREAM(p, l, s)	gzwrite(s, p, l)
#define GETS_STREAM(p, l, s)	gzgets(s, p, l)
#define GETC_STREAM(s)			gzgetc(s)
#define OPEN_STREAM(f, m)		gzopen(f, m)
#define REOPEN_STREAM(f, m)		gzdopen(f, m)
#define FIND_STREAM(f)			gztell(f)
#define REVERT_STREAM(f, o, s)	gzseek(f, o, s)
#define CLOSE_STREAM(s)			gzclose(s)
#else
#define STREAM					FILE *
#define READ_STREAM(p, l, s)	fread(p, 1, l, s)
#define WRITE_STREAM(p, l, s)	fwrite(p, 1, l, s)
#define GETS_STREAM(p, l, s)	fgets(p, l, s)
#define GETC_STREAM(s)			fgetc(s)
#define OPEN_STREAM(f, m)		fopen(f, m)
#define REOPEN_STREAM(f, m)		fdopen(f, m)
#define FIND_STREAM(f)			ftell(f)
#define REVERT_STREAM(f, o, s)	fseek(f, o, s)
#define CLOSE_STREAM(s)			fclose(s)
#endif

#define SNES_WIDTH					256
#define SNES_HEIGHT					224
#define SNES_HEIGHT_EXTENDED		239
#define MAX_SNES_WIDTH				(SNES_WIDTH * 2)
#define MAX_SNES_HEIGHT				(SNES_HEIGHT_EXTENDED * 2)
#ifndef __CELLOS_LV2__
   #define IMAGE_WIDTH					(Settings.SupportHiRes ? MAX_SNES_WIDTH : SNES_WIDTH)
   #define IMAGE_HEIGHT				(Settings.SupportHiRes ? MAX_SNES_HEIGHT : SNES_HEIGHT_EXTENDED)
#else
   #define IMAGE_WIDTH					MAX_SNES_WIDTH
   #define IMAGE_HEIGHT				MAX_SNES_HEIGHT
#endif

#define	NTSC_MASTER_CLOCK			21477272.0
#define	PAL_MASTER_CLOCK			21281370.0

#define SNES_MAX_NTSC_VCOUNTER		262
#define SNES_MAX_PAL_VCOUNTER		312
#define SNES_HCOUNTER_MAX			341

#define ONE_CYCLE					6
#define SLOW_ONE_CYCLE				8
#define TWO_CYCLES					12
#define	ONE_DOT_CYCLE				4

#define SNES_CYCLES_PER_SCANLINE	(SNES_HCOUNTER_MAX * ONE_DOT_CYCLE)
#define SNES_SCANLINE_TIME			(SNES_CYCLES_PER_SCANLINE / NTSC_MASTER_CLOCK)

#define SNES_WRAM_REFRESH_HC_v1		530
#define SNES_WRAM_REFRESH_HC_v2		538
#define SNES_WRAM_REFRESH_CYCLES	40

#define SNES_HBLANK_START_HC		1096					// H=274
#define	SNES_HDMA_START_HC			1106					// FIXME: not true
#define	SNES_HBLANK_END_HC			4						// H=1
#define	SNES_HDMA_INIT_HC			20						// FIXME: not true
#define	SNES_RENDER_START_HC		192	// FIXME: Snes9x renders a line at a time.

#define SNES_TR_MASK		(1 <<  4)
#define SNES_TL_MASK		(1 <<  5)
#define SNES_X_MASK			(1 <<  6)
#define SNES_A_MASK			(1 <<  7)
#define SNES_RIGHT_MASK		(1 <<  8)
#define SNES_LEFT_MASK		(1 <<  9)
#define SNES_DOWN_MASK		(1 << 10)
#define SNES_UP_MASK		(1 << 11)
#define SNES_START_MASK		(1 << 12)
#define SNES_SELECT_MASK	(1 << 13)
#define SNES_Y_MASK			(1 << 14)
#define SNES_B_MASK			(1 << 15)

#define DEBUG_MODE_FLAG		(1 <<  0)	// debugger
#define TRACE_FLAG			(1 <<  1)	// debugger
#define SINGLE_STEP_FLAG	(1 <<  2)	// debugger
#define BREAK_FLAG			(1 <<  3)	// debugger
#define NMI_FLAG			(1 <<  7)	// CPU
#define IRQ_FLAG			(1 << 11)	// CPU
#define SCAN_KEYS_FLAG		(1 <<  4)	// CPU
#define HALTED_FLAG			(1 << 12)	// APU
#define FRAME_ADVANCE_FLAG	(1 <<  9)

#define ROM_NAME_LEN	23
#define AUTO_FRAMERATE	200

struct SCPUState
{
	bool8	IRQActive;
	bool8	InDMA;
	bool8	InHDMA;
	bool8	InDMAorHDMA;
	bool8	InWRAMDMAorHDMA;
	bool8	WaitingForInterrupt;
	bool8	SRAMModified;
	uint8	*PCBase;
	uint8	HDMARanInDMA;
	uint8	WhichEvent;
	int32	Cycles;
	int32	PrevCycles;
	int32	V_Counter;
	int32	IRQPending;
	int32	MemSpeed;
	int32	MemSpeedx2;
	int32	FastROMSpeed;
	int32	CurrentDMAorHDMAChannel;
	int32	NextEvent;
	uint32	Flags;
	uint32	WaitAddress;
	uint32	WaitCounter;
	uint32	PBPCAtOpcodeStart;
	uint32	AutoSaveTimer;
};

enum
{
	HC_HBLANK_START_EVENT = 1,
	HC_IRQ_1_3_EVENT      = 2,
	HC_HDMA_START_EVENT   = 3,
	HC_IRQ_3_5_EVENT      = 4,
	HC_HCOUNTER_MAX_EVENT = 5,
	HC_IRQ_5_7_EVENT      = 6,
	HC_HDMA_INIT_EVENT    = 7,
	HC_IRQ_7_9_EVENT      = 8,
	HC_RENDER_EVENT       = 9,
	HC_IRQ_9_A_EVENT      = 10,
	HC_WRAM_REFRESH_EVENT = 11,
	HC_IRQ_A_1_EVENT      = 12
};

struct STimings
{
	bool8	InterlaceField;
	bool8	APUAllowTimeOverflow;
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
	int32	DMACPUSync;		// The cycles to synchronize DMA and CPU. Snes9x cannot emulate correctly.
	int32	NMIDMADelay;	// The delay of NMI trigger after DMA transfers. Snes9x cannot emulate correctly.
	int32	IRQPendCount;	// This value is just a hack, because Snes9x cannot emulate any events in an opcode.
	int32	APUSpeedup;
};

struct SSettings
{
	//bool8	TraceDMA;
	//bool8	TraceHDMA;
	//bool8	TraceVRAM;
	//bool8	TraceUnknownRegisters;
	//bool8	TraceDSP;
	//bool8	TraceHCEvent;

	bool		SuperFX;
	bool		SA1;
	bool		C4;
	bool		SDD1;
	bool		SPC7110;
	bool		SPC7110RTC;
	bool		OBC1;
	bool		SRTC;
	bool		BS;
	bool		BSXItself;
	bool		BSXBootup;
	bool		MouseMaster;
	bool		SuperScopeMaster;
	bool		JustifierMaster;
	bool		MultiPlayer5Master;
	bool		ForceLoROM;
	bool		ForceHiROM;
	bool		ForceHeader;
	bool		ForceNoHeader;
	bool		ForceInterleaved;
	bool		ForceInterleaved2;
	bool		ForceInterleaveGD24;
	bool		ForceNotInterleaved;
	bool		PAL;
	bool		SoundSync;
	bool		SixteenBitSound;
	bool		Stereo;
	//bool		ReverseStereo;
	//bool		DisplayFrameRate;
	//bool		DisplayWatchedAddresses;
	//bool		DisplayMovieFrame;
	//bool		FrameAdvance;
	bool		Mute;
	bool		SupportHiRes;
	bool		Multi;
	bool		DisableGameSpecificHacks;
	bool		ShutdownMaster;
	bool		Shutdown;
	bool		BlockInvalidVRAMAccessMaster;
	bool		BlockInvalidVRAMAccess;
	bool		DisableIRQ;
	bool		DisableHDMA;
	bool		ForcedPause;
	bool		Paused;
	bool		StopEmulation;
	bool		TurboMode;
	bool		SnapshotScreenshots;
	bool		DontSaveOopsSnapshot;
	//BEGIN OF SNES9x PS3
	bool		CurrentROMisPAL;
	bool		CurrentROMisMultitapCompatible;
	bool		CurrentROMisMouseCompatible;
	bool		CurrentROMisSuperScopeCompatible;
	bool		CurrentROMisJustifierCompatible;
	bool		SaveCustomControlScheme;
	//END OF SNES9x PS3

	char	CartAName[PATH_MAX + 1];
	char	CartBName[PATH_MAX + 1];

	uint32_t	StretchScreenshots;

	uint8_t		DSP;
	uint8_t		SETA;
	uint8_t		BG_Forced;

	uint16_t	DisplayColor;

	int32_t		HDMATimingHack;
	int32_t		AutoSaveDelay;
	int32_t		AccessoryAutoDetection;
	int32_t		PS3OverscanAmount;
	int32_t		ControlScheme;

	uint32_t	ForcePAL;
	uint32_t	ForceNTSC;
	uint32_t	FrameTimePAL;
	uint32_t	FrameTimeNTSC;
	uint32_t	FrameTime;
	uint32_t	SoundPlaybackRate;
	uint32_t	SoundInputRate;
	//uint32_t	Transparency;
	//uint32_t	DisableGraphicWindows;
	//uint32_t	DisplayPressedKeys;
	uint32_t	AutoDisplayMessages;
	uint32_t	InitialInfoStringTimeout;
	uint32_t	ApplyCheats;
	uint32_t	NoPatch;
	uint32_t	SkipFrames;
	uint32_t	TurboSkipFrames;
	uint32_t	AutoMaxSkipFrames;
	uint32_t	HighSpeedSeek;
	//BEGIN OF SNES9x PS3
	uint32_t	PS3KeepAspect;
	uint32_t	PS3Smooth;
	uint32_t	PS3Smooth2;
	uint32_t	SRAMWriteProtect;
	uint32_t	PS3OverscanEnabled;
	uint32_t	SoundMode;
	uint32_t	PS3CurrentResolution;
	uint32_t	PS3PALTemporalMode60Hz;
	uint32_t	PS3FontSize;
	uint32_t	ScreenshotsEnabled;
	uint32_t	AccessoryType;
	uint32_t	OddScreenHeightJumpFix;
	uint32_t	Throttled;
	uint32_t	TripleBuffering;
	uint32_t	CurrentSaveStateSlot;
	uint32_t	CurrentCheatPosition;
	uint32_t	ScaleEnabled;
	uint32_t	ScaleFactor;
	uint32_t	ResetBeforeRecordingMovie;
	uint32_t	ApplyShaderPresetOnStartup;
	uint32_t	ViewportX;
	uint32_t	ViewportY;
	uint32_t	ViewportWidth;
	uint32_t	ViewportHeight;
	//END OF SNES9x PS3

	//bool8	NetPlay;
	//bool8	NetPlayServer;
	//char	ServerName[128];
	//int		Port;

	//bool8		MovieTruncate;
	//bool8		MovieNotifyIgnored;
	//bool8		WrongMovieStateProtection;
	//bool8		DumpStreams;
	//int		DumpStreamsMaxFrames;

	//bool8	TakeScreenshot;

	//bool8	UpAndDown;

	//bool8	OpenGLEnable;


	char	PS3PathSaveStates[MAX_PATH_LENGTH];
	char	PS3PathSRAM[MAX_PATH_LENGTH];
	char	PS3PathScreenshots[MAX_PATH_LENGTH];
	char	PS3PathCheats[MAX_PATH_LENGTH];
	char	PS3CurrentShader[MAX_PATH_LENGTH];
	char	PS3CurrentShader2[MAX_PATH_LENGTH];
	char	PS3CurrentBorder[MAX_PATH_LENGTH];
	char	PS3PathROMDirectory[MAX_PATH_LENGTH];
	char	RSoundServerIPAddress[MAX_PATH_LENGTH];
	char	GameAwareShaderPath[MAX_PATH_LENGTH];
	char	ShaderPresetPath[MAX_PATH_LENGTH];
	char	ShaderPresetTitle[MAX_PATH_LENGTH];
};

struct SSNESGameFixes
{
	uint8	SRAMInitialValue;
	uint8	Uniracers;
};

enum
{
	PAUSE_NETPLAY_CONNECT		= (1 << 0),
	PAUSE_TOGGLE_FULL_SCREEN	= (1 << 1),
	PAUSE_EXIT					= (1 << 2),
	PAUSE_MENU					= (1 << 3),
	PAUSE_INACTIVE_WINDOW		= (1 << 4),
	PAUSE_WINDOW_ICONISED		= (1 << 5),
	PAUSE_RESTORE_GUI			= (1 << 6),
	PAUSE_FREEZE_FILE			= (1 << 7)
};

void S9xSetPause(uint32);
void S9xClearPause(uint32);
void S9xExit(void);
void S9xMessage(int, int, const char *);

extern struct SSettings			Settings;
extern struct SCPUState			CPU;
extern struct STimings			Timings;
extern struct SSNESGameFixes	SNESGameFixes;
extern char						String[513];

#endif
