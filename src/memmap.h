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


#ifndef _MEMMAP_H_
#define _MEMMAP_H_

#define ROM_NAME_LEN	23

#define MEMMAP_BLOCK_SIZE	(0x1000)
#define MEMMAP_NUM_BLOCKS	(0x1000000 / MEMMAP_BLOCK_SIZE)
#define MEMMAP_SHIFT		(12)
#define MEMMAP_MASK		(MEMMAP_BLOCK_SIZE - 1)

#define MAP_CPU			0
#define MAP_PPU			1
#define MAP_LOROM_SRAM		2
#define MAP_LOROM_SRAM_B	3
#define MAP_HIROM_SRAM		4
#define MAP_DSP			5
#define MAP_SA1RAM		6
#define MAP_BWRAM		7
#define MAP_BWRAM_BITMAP	8
#define MAP_BWRAM_BITMAP2	9
#define MAP_SPC7110_ROM		10
#define MAP_SPC7110_DRAM	11
#define MAP_RONLY_SRAM		12
#define MAP_C4			13
#define MAP_OBC_RAM		14
#define MAP_SETA_DSP		15
#define MAP_SETA_RISC		16
#define MAP_BSX			17
#define MAP_NONE		18
#define MAP_LAST		19

#ifdef HW_DOL
#define MAX_ROM_SIZE 0x600000
#else
#define MAX_ROM_SIZE 0x800000
#endif

#define NOPE 0
#define YEAH 1
#define BIGFIRST 2
#define SMALLFIRST 3

#define MAP_TYPE_I_O 0
#define MAP_TYPE_ROM 1
#define MAP_TYPE_RAM 2

enum speedhacks
{
	SPEEDHACK_NONE		= 0,
	SPEEDHACK_CT,
	SPEEDHACK_DKC1,
	SPEEDHACK_SUPER_METROID,
	SPEEDHACK_KILLER_INSTINCT,
	SPEEDHACK_FF6,
   SPEEDHACK_STAR_FOX_1,
   SPEEDHACK_SUPER_MARIO_WORLD,
};

typedef struct
{
	int32	HeaderCount;

	uint8	*RAM;
	uint8	*ROM;
	uint8	*SRAM;
	uint8	*VRAM;
	uint8	*FillRAM;
	uint8	*BWRAM;
	uint8	*C4RAM;
	uint8	*OBC1RAM;
	uint8	*BSRAM;
	uint8	*BIOSROM;

	uint8	*Map[MEMMAP_NUM_BLOCKS];
	uint8	*WriteMap[MEMMAP_NUM_BLOCKS];
	uint8	BlockIsRAM[MEMMAP_NUM_BLOCKS];
	uint8	BlockIsROM[MEMMAP_NUM_BLOCKS];
	uint8	ExtendedFormat;

	char	ROMFilename[PATH_MAX + 1];
	char	ROMName[ROM_NAME_LEN];
	char	RawROMName[ROM_NAME_LEN];
	char	ROMId[5];
	int32	CompanyId;
	uint8	ROMRegion;
	uint8	ROMSpeed;
	uint8	ROMType;
	uint8	ROMSize;
	uint32	ROMChecksum;
	uint32	ROMComplementChecksum;
	uint32	ROMCRC32;

	bool8	HiROM;
	bool8	LoROM;
	uint8	SRAMSize;
	uint32	SRAMMask;
	uint32	CalculatedSize;
	uint32	CalculatedChecksum;

} CMemory;

bool8	Init (void);
void	Deinit (void);

bool8 LoadROM (const char *filename);
bool8 LoadMultiCart (const char *cartA, const char *cartB);
bool8 LoadSufamiTurbo (const char *cartA, const char *cartB);
bool8 LoadSameGame (const char *cartA, const char *cartB);

void	InitROM (void);

void	map_WriteProtectROM (void);

struct SMulti
{
	int	cartType;
	int32	cartSizeA;
	int32	cartSizeB;
	int32	sramSizeA;
	int32	sramSizeB;
	uint32	sramMaskA;
	uint32	sramMaskB;
	uint32	cartOffsetA;
	uint32	cartOffsetB;
	uint8	*sramA;
	uint8	*sramB;
	char	fileNameA[PATH_MAX + 1];
	char	fileNameB[PATH_MAX + 1];
};

extern CMemory	Memory;
extern struct SMulti	Multi;

#define WRAP_PAGE 0xFF
#define WRAP_BANK 0xFFFF
#define WRAP_NONE 0xFFFFFF

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#endif
