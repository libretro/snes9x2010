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

#ifndef _PPU_H_
#define _PPU_H_

#include "memmap.h"

#define FIRST_VISIBLE_LINE	1

#define TILE_2BIT		0
#define TILE_4BIT		1
#define TILE_8BIT		2
#define TILE_2BIT_EVEN		3
#define TILE_2BIT_ODD		4
#define TILE_4BIT_EVEN		5
#define TILE_4BIT_ODD		6

#define MAX_2BIT_TILES		4096
#define MAX_4BIT_TILES		2048
#define MAX_8BIT_TILES		1024

#define CLIP_OR			0
#define CLIP_AND		1
#define CLIP_XOR		2
#define CLIP_XNOR		3

#define PPU_IRQ_SOURCE		2
#define GSU_IRQ_SOURCE		4
#define SA1_DMA_IRQ_SOURCE	32
#define SA1_IRQ_SOURCE		128

/* PPU memory-mapped registers */

#define MEM_PPU_INIDISP 0x2100
#define MEM_PPU_OBSEL   0x2101
#define MEM_PPU_OAMADDL 0x2102
#define MEM_PPU_OAMADDH 0x2103
#define MEM_PPU_OAMDATA 0x2104
#define MEM_PPU_BGMODE  0x2105
#define MEM_PPU_MOSAIC  0x2106
#define MEM_PPU_BG1SC   0x2107
#define MEM_PPU_BG2SC   0x2108
#define MEM_PPU_BG3SC   0x2109
#define MEM_PPU_BG4SC   0x210A
#define MEM_PPU_BG12NBA 0x210B
#define MEM_PPU_BG34NBA 0x210C
#define MEM_PPU_BG1HOFS 0x210D

#define MEM_PPU_TM	0x212C
#define MEM_PPU_TS	0x212D
#define MEM_PPU_TMW	0x212E
#define MEM_PPU_TSW	0x212F

#define MEM_PPU_CGWSEL	0x2130
#define MEM_PPU_CGADSUB 0x2131
#define MEM_PPU_SETINI  0x2133

#define MEM_PPU_APUIO0	0x2140

#define MEM_PPU_STAT77	0x213E
#define MEM_PPU_STAT78	0x213F

#define MEM_PPU_WMDATA	0x2180

#define MEM_PPU_WMADDH	0x2183

/* CPU memory-mapped registers */

#define MEM_CPU_JOYSER0 0x4016
#define MEM_CPU_JOYSER1 0x4017
#define MEM_CPU_NMITIMEN 0x4200
#define MEM_CPU_JOY1L	0x4218 // JOY1L
#define MEM_CPU_JOY1H	0x4219 // JOY1H
#define MEM_CPU_JOY2L	0x421A // JOY2L
#define MEM_CPU_JOY2H	0x421B // JOY2H
#define MEM_CPU_JOY3L	0x421C // JOY3L
#define MEM_CPU_JOY3H	0x421D // JOY3H
#define MEM_CPU_JOY4L	0x421E
#define MEM_CPU_JOY4H	0x421F

/* DMA memory-mapped registers */

#define MEM_DMA_DMAP0	0x4300

/* SA-1 memory-mapped registers */

#define MEM_SA1_CIVL	0x2207
#define MEM_SA1_CIVH	0x2208
#define MEM_SA1_SCNT	0x2209
#define MEM_SA1_SNVL	0x220C
#define MEM_SA1_SNVH	0x220D
#define MEM_SA1_SIVL	0x220E
#define MEM_SA1_SIVH	0x220F

struct ClipData
{
	uint8	Count;
	uint8	DrawMode[6];
	uint16	Left[6];
	uint16	Right[6];
};

struct InternalPPU
{
	struct ClipData Clip[2][6];
	bool8	OBJChanged;
	bool8	DirectColourMapsNeedRebuild;
	uint8	*TileCache[7];
	uint8	*TileCached[7];
#ifdef CORRECT_VRAM_READS
	uint16	VRAMReadBuffer;
#else
	bool8	FirstVRAMRead;
#endif
	bool8	Interlace;
	bool8	InterlaceOBJ;
	bool8	PseudoHires;
	bool8	DoubleWidthPixels;
	bool8	DoubleHeightPixels;
	int	CurrentLine;
	int	PreviousLine;
	uint8	*XB;
	uint16	ScreenColors[256];
	int	RenderedScreenWidth;
	int	RenderedScreenHeight;
};

struct SOBJ
{
	int16	HPos;
	uint16	VPos;
	uint8	HFlip;
	uint8	VFlip;
	uint16	Name;
	uint8	Priority;
	uint8	Palette;
	uint8	Size;
};

struct SPPU
{
	struct
	{
		bool8	High;
		uint8	Increment;
		uint16	Address;
		uint16	Mask1;
		uint16	FullGraphicCount;
		uint16	Shift;
	}	VMA;

	uint32	WRAM;

	struct
	{
		uint16	SCBase;
		uint16	HOffset;
		uint16	VOffset;
		uint8	BGSize;
		uint16	NameBase;
		uint16	SCSize;
	}	BG[4];

	uint8	BGMode;
	uint8	BG3Priority;
	bool8	RenderSub;

	bool8	CGFLIP;
	uint8	CGFLIPRead;
	uint8	CGADD;
	uint16	CGDATA[256];

	struct SOBJ OBJ[128];
	bool8	OBJThroughMain;
	bool8	OBJThroughSub;
	bool8	OBJAddition;
	uint16	OBJNameBase;
	uint16	OBJNameSelect;
	uint8	OBJSizeSelect;

	uint16	OAMAddr;
	uint16	SavedOAMAddr;
	uint8	OAMPriorityRotation;
	uint8	OAMFlip;
	uint8	OAMReadFlip;
	uint16	OAMTileAddress;
	uint16	OAMWriteRegister;
	uint8	OAMData[512 + 32];

	uint8	FirstSprite;
	uint8	LastSprite;
	uint8	RangeTimeOver;

	bool8	HTimerEnabled;
	bool8	VTimerEnabled;
	short	HTimerPosition;
	short	VTimerPosition;
	uint16	IRQHBeamPos;
	uint16	IRQVBeamPos;

	uint8	HBeamFlip;
	uint8	VBeamFlip;
	uint16	HBeamPosLatched;
	uint16	VBeamPosLatched;
	uint16	GunHLatch;
	uint16	GunVLatch;
	uint8	HVBeamCounterLatched;

	bool8	Mode7HFlip;
	bool8	Mode7VFlip;
	uint8	Mode7Repeat;
	short	MatrixA;
	short	MatrixB;
	short	MatrixC;
	short	MatrixD;
	short	CentreX;
	short	CentreY;
	short	M7HOFS;
	short	M7VOFS;

	uint8	Mosaic;
	uint8	MosaicStart;
	bool8	BGMosaic[4];
	bool8	DisableMosaicHack;
	bool8	SFXSpeedupHack;

	uint8	Window1Left;
	uint8	Window1Right;
	uint8	Window2Left;
	uint8	Window2Right;
	bool8	RecomputeClipWindows;
	uint8	ClipCounts[6];
	uint8	ClipWindowOverlapLogic[6];
	uint8	ClipWindow1Enable[6];
	uint8	ClipWindow2Enable[6];
	bool8	ClipWindow1Inside[6];
	bool8	ClipWindow2Inside[6];

	bool8	ForcedBlanking;

	uint8	FixedColourRed;
	uint8	FixedColourGreen;
	uint8	FixedColourBlue;
	uint8	Brightness;
	uint16	ScreenHeight;

	bool8	Need16x8Mulitply;
	uint8	BGnxOFSbyte;
	uint8	M7byte;

	uint8	HDMA;
	uint8	HDMAEnded;

	uint8	OpenBus1;
	uint8	OpenBus2;

	bool8	FullClipping;
};

extern struct SPPU		PPU;
extern struct InternalPPU	IPPU;

void S9xResetPPU (void);
void S9xSoftResetPPU (void);
void S9xSetPPU (uint8 Byte, uint16 Address);
uint8 S9xGetPPU (uint16 Address);
void S9xSetCPU (uint8 Byte, uint16 Address);
uint8 S9xGetCPU (uint16 Address);
void S9xFixColourBrightness (void);
void S9xDoAutoJoypad (void);


#define MAX_5C77_VERSION	0x01
#define MAX_5C78_VERSION	0x03
#define MAX_5A22_VERSION	0x02

#define FLUSH_REDRAW() \
	if (IPPU.PreviousLine != IPPU.CurrentLine) \
		S9xUpdateScreen();

struct SGFX
{
	uint16	*Screen;
	uint16	*SubScreen;
	uint8	*ZBuffer;
	uint8	*SubZBuffer;
	uint32	Pitch;
	uint32	ScreenSize;
	uint16	*S;
	uint8	*DB;
	uint16	*X2;
	uint16	*ZERO;
	uint32	RealPPL;		/* true PPL of Screen buffer */
	uint32	PPL;			/* number of pixels on each of Screen buffer */
	uint32	LinesPerTile;		/* number of lines in 1 tile (4 or 8 due to interlace) */
	uint16	*ScreenColors;		/* screen colors for rendering main */
	uint16	*RealScreenColors;	/* screen colors, ignoring color window clipping */
	uint8	Z1;			/* depth for comparison */
	uint8	Z2;			/* depth to save */
	uint32	FixedColour;
	uint8	DoInterlace;
	uint8	InterlaceFrame;
	uint32	StartY;
	uint32	EndY;
	bool8	ClipColors;
	uint8	OBJWidths[128];
	uint8	OBJVisibleTiles[128];

	struct ClipData	*Clip;

	struct
	{
		uint8	RTOFlags;
		int16	Tiles;

		struct
		{
			int8	Sprite;
			uint8	Line;
		}	OBJ[32];
	}	OBJLines[SNES_HEIGHT_EXTENDED];

	void	(*DrawBackdropMath) (uint32, uint32, uint32);
	void	(*DrawBackdropNomath) (uint32, uint32, uint32);
	void	(*DrawTileMath) (uint32, uint32, uint32, uint32);
	void	(*DrawTileNomath) (uint32, uint32, uint32, uint32);
	void	(*DrawClippedTileMath) (uint32, uint32, uint32, uint32, uint32, uint32);
	void	(*DrawClippedTileNomath) (uint32, uint32, uint32, uint32, uint32, uint32);
	void	(*DrawMosaicPixelMath) (uint32, uint32, uint32, uint32, uint32, uint32);
	void	(*DrawMosaicPixelNomath) (uint32, uint32, uint32, uint32, uint32, uint32);
	void	(*DrawMode7BG1Math) (uint32, uint32, int);
	void	(*DrawMode7BG1Nomath) (uint32, uint32, int);
	void	(*DrawMode7BG2Math) (uint32, uint32, int);
	void	(*DrawMode7BG2Nomath) (uint32, uint32, int);
};

struct SBG
{
	uint8	(*ConvertTile) (uint8 *, uint32, uint32);
	uint8	(*ConvertTileFlip) (uint8 *, uint32, uint32);

	uint32	TileSizeH;
	uint32	TileSizeV;
	uint32	OffsetSizeH;
	uint32	OffsetSizeV;
	uint32	TileShift;
	uint32	TileAddress;
	uint32	NameSelect;
	uint32	SCBase;

	uint32	StartPalette;
	uint32	PaletteShift;
	uint32	PaletteMask;
	uint8	EnableMath;
	uint8	InterlaceLine;

	uint8	*Buffer;
	uint8	*BufferFlip;
	uint8	*Buffered;
	uint8	*BufferedFlip;
	bool8	DirectColourMode;
};

struct SLineData
{
	struct
	{
		uint16	VOffset;
		uint16	HOffset;
	}	BG[4];
};

struct SLineMatrixData
{
	short	MatrixA;
	short	MatrixB;
	short	MatrixC;
	short	MatrixD;
	short	CentreX;
	short	CentreY;
	short	M7HOFS;
	short	M7VOFS;
};

extern uint8		mul_brightness[16][32];
extern struct SBG	BG;
extern struct SGFX	GFX;

#define H_FLIP		0x4000
#define V_FLIP		0x8000
#define BLANK_TILE	2

#define COLOR_ADD1_2(C1, C2) \
	((((((C1) & RGB_REMOVE_LOW_BITS_MASK) + \
	((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1) + \
	((C1) & (C2) & RGB_LOW_BITS_MASK)) | ALPHA_BITS_MASK)

#define COLOR_ADD(C1, C2) \
	(GFX.X2[((((C1) & RGB_REMOVE_LOW_BITS_MASK) + \
	((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1) + \
	((C1) & (C2) & RGB_LOW_BITS_MASK)] | \
	(((C1) ^ (C2)) & RGB_LOW_BITS_MASK))

#define COLOR_SUB1_2(C1, C2) \
	GFX.ZERO[(((C1) | RGB_HI_BITS_MASKx2) - \
	((C2) & RGB_REMOVE_LOW_BITS_MASK)) >> 1]

void S9xUpdateScreen (void);

/* external port interface which must be initialised for each port */

bool8 S9xGraphicsInit (void);
void S9xGraphicsDeinit (void);

extern bool8 coldata_update_screen;

#endif
