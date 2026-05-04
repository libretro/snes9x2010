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


/* This file includes itself multiple times.
   The other option would be to have 4 files, 
   where A includes B, and B includes C 3 times, and C includes D 5 times.

   Look for the following marker to find where the divisions are. */

/* Top-level compilation. */

#ifndef _NEWTILE_CPP
#define _NEWTILE_CPP

#include <retro_inline.h>

#include "snes9x.h"
#include "ppu.h"
#include "tile.h"

static INLINE uint16 COLOR_SUB (uint16 C1, uint16 C2)
{
	uint16	mC1, mC2, v = ALPHA_BITS_MASK;

	mC1 = C1 & FIRST_COLOR_MASK;
	mC2 = C2 & FIRST_COLOR_MASK;
	if (mC1 > mC2) v += (mC1 - mC2);

	mC1 = C1 & SECOND_COLOR_MASK;
	mC2 = C2 & SECOND_COLOR_MASK;
	if (mC1 > mC2) v += (mC1 - mC2);

	mC1 = C1 & THIRD_COLOR_MASK;
	mC2 = C2 & THIRD_COLOR_MASK;
	if (mC1 > mC2) v += (mC1 - mC2);

	return (v);
}

static uint16	DirectColourMaps[8][256];
static const uint16 BlackColourMap[256] = {0};

static void S9xBuildDirectColourMaps (void)
{
   uint32 p, c;
	IPPU.XB = mul_brightness[PPU.Brightness];

	for (p = 0; p < 8; p++)
		for (c = 0; c < 256; c++)
			DirectColourMaps[p][c] = BUILD_PIXEL(IPPU.XB[((c & 7) << 2) | ((p & 1) << 1)], IPPU.XB[((c & 0x38) >> 1) | (p & 2)], IPPU.XB[((c & 0xc0) >> 3) | (p & 4)]);

	IPPU.DirectColourMapsNeedRebuild = FALSE;
}

/* Tile rendering bit-shuffle tables.

   pixbit[bitshift][nybble]: maps a 4-bit nybble of pixel data to a
   32-bit value where each of the four bits is placed in a specific
   byte of the output, then shifted left by `bitshift`. Used by the
   DOBIT macro and friends in the tile converters to efficiently OR
   four bitplane bits into a 32-bit accumulator.

   The bit positions depend on host endianness: little-endian places
   bit-0 of nybble in byte 3, bit-1 in byte 2, etc.; big-endian
   reverses. The two #ifdef MSB_FIRST branches produce different
   tables, switched at compile time.

   hrbit_odd[i] and hrbit_even[i]: extract odd-numbered and
   even-numbered bits from byte i, packed into a 4-bit value. Used
   by the hires tile converters for the high-resolution bit-plane
   re-pack. Endianness-independent.

   The values were previously computed at S9xInitTileRenderer call
   time. They are deterministic and small (768 bytes total), so
   they live in .rodata as static const. */

#ifdef MSB_FIRST
static const uint32 pixbit[8][16] =
{
	{
		0x00000000, 0x00000001, 0x00000100, 0x00000101,
		0x00010000, 0x00010001, 0x00010100, 0x00010101,
		0x01000000, 0x01000001, 0x01000100, 0x01000101,
		0x01010000, 0x01010001, 0x01010100, 0x01010101,

	},
	{
		0x00000000, 0x00000002, 0x00000200, 0x00000202,
		0x00020000, 0x00020002, 0x00020200, 0x00020202,
		0x02000000, 0x02000002, 0x02000200, 0x02000202,
		0x02020000, 0x02020002, 0x02020200, 0x02020202,

	},
	{
		0x00000000, 0x00000004, 0x00000400, 0x00000404,
		0x00040000, 0x00040004, 0x00040400, 0x00040404,
		0x04000000, 0x04000004, 0x04000400, 0x04000404,
		0x04040000, 0x04040004, 0x04040400, 0x04040404,

	},
	{
		0x00000000, 0x00000008, 0x00000800, 0x00000808,
		0x00080000, 0x00080008, 0x00080800, 0x00080808,
		0x08000000, 0x08000008, 0x08000800, 0x08000808,
		0x08080000, 0x08080008, 0x08080800, 0x08080808,

	},
	{
		0x00000000, 0x00000010, 0x00001000, 0x00001010,
		0x00100000, 0x00100010, 0x00101000, 0x00101010,
		0x10000000, 0x10000010, 0x10001000, 0x10001010,
		0x10100000, 0x10100010, 0x10101000, 0x10101010,

	},
	{
		0x00000000, 0x00000020, 0x00002000, 0x00002020,
		0x00200000, 0x00200020, 0x00202000, 0x00202020,
		0x20000000, 0x20000020, 0x20002000, 0x20002020,
		0x20200000, 0x20200020, 0x20202000, 0x20202020,

	},
	{
		0x00000000, 0x00000040, 0x00004000, 0x00004040,
		0x00400000, 0x00400040, 0x00404000, 0x00404040,
		0x40000000, 0x40000040, 0x40004000, 0x40004040,
		0x40400000, 0x40400040, 0x40404000, 0x40404040,

	},
	{
		0x00000000, 0x00000080, 0x00008000, 0x00008080,
		0x00800000, 0x00800080, 0x00808000, 0x00808080,
		0x80000000, 0x80000080, 0x80008000, 0x80008080,
		0x80800000, 0x80800080, 0x80808000, 0x80808080,

	},
};
#else
static const uint32 pixbit[8][16] =
{
	{
		0x00000000, 0x01000000, 0x00010000, 0x01010000,
		0x00000100, 0x01000100, 0x00010100, 0x01010100,
		0x00000001, 0x01000001, 0x00010001, 0x01010001,
		0x00000101, 0x01000101, 0x00010101, 0x01010101,

	},
	{
		0x00000000, 0x02000000, 0x00020000, 0x02020000,
		0x00000200, 0x02000200, 0x00020200, 0x02020200,
		0x00000002, 0x02000002, 0x00020002, 0x02020002,
		0x00000202, 0x02000202, 0x00020202, 0x02020202,

	},
	{
		0x00000000, 0x04000000, 0x00040000, 0x04040000,
		0x00000400, 0x04000400, 0x00040400, 0x04040400,
		0x00000004, 0x04000004, 0x00040004, 0x04040004,
		0x00000404, 0x04000404, 0x00040404, 0x04040404,

	},
	{
		0x00000000, 0x08000000, 0x00080000, 0x08080000,
		0x00000800, 0x08000800, 0x00080800, 0x08080800,
		0x00000008, 0x08000008, 0x00080008, 0x08080008,
		0x00000808, 0x08000808, 0x00080808, 0x08080808,

	},
	{
		0x00000000, 0x10000000, 0x00100000, 0x10100000,
		0x00001000, 0x10001000, 0x00101000, 0x10101000,
		0x00000010, 0x10000010, 0x00100010, 0x10100010,
		0x00001010, 0x10001010, 0x00101010, 0x10101010,

	},
	{
		0x00000000, 0x20000000, 0x00200000, 0x20200000,
		0x00002000, 0x20002000, 0x00202000, 0x20202000,
		0x00000020, 0x20000020, 0x00200020, 0x20200020,
		0x00002020, 0x20002020, 0x00202020, 0x20202020,

	},
	{
		0x00000000, 0x40000000, 0x00400000, 0x40400000,
		0x00004000, 0x40004000, 0x00404000, 0x40404000,
		0x00000040, 0x40000040, 0x00400040, 0x40400040,
		0x00004040, 0x40004040, 0x00404040, 0x40404040,

	},
	{
		0x00000000, 0x80000000, 0x00800000, 0x80800000,
		0x00008000, 0x80008000, 0x00808000, 0x80808000,
		0x00000080, 0x80000080, 0x00800080, 0x80800080,
		0x00008080, 0x80008080, 0x00808080, 0x80808080,

	},
};
#endif

static const uint8 hrbit_odd[256] =
{
	0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
	0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07, 0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
	0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
	0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07, 0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
	0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b, 0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f, 0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
	0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b, 0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f, 0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
	0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
	0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07, 0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
	0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03,
	0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07, 0x04, 0x05, 0x04, 0x05, 0x06, 0x07, 0x06, 0x07,
	0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b, 0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f, 0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,
	0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b, 0x08, 0x09, 0x08, 0x09, 0x0a, 0x0b, 0x0a, 0x0b,
	0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f, 0x0c, 0x0d, 0x0c, 0x0d, 0x0e, 0x0f, 0x0e, 0x0f,

};

static const uint8 hrbit_even[256] =
{
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
	0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
	0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03,
	0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
	0x04, 0x04, 0x05, 0x05, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07, 0x06, 0x06, 0x07, 0x07,
	0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
	0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
	0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
	0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
	0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
	0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x0a, 0x0a, 0x0b, 0x0b, 0x0a, 0x0a, 0x0b, 0x0b,
	0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,
	0x0c, 0x0c, 0x0d, 0x0d, 0x0c, 0x0c, 0x0d, 0x0d, 0x0e, 0x0e, 0x0f, 0x0f, 0x0e, 0x0e, 0x0f, 0x0f,

};

void S9xInitTileRenderer (void)
{
	/* All tile renderer tables are now compile-time const. This
	   stub preserves the public API. */
}

/* Here are the tile converters, selected by S9xSelectTileConverter().
   Really, except for the definition of DOBIT and the number of times
   it is called, they're all the same. */

#define DOBIT(n, i) \
	if ((pix = *(tp + (n)))) \
	{ \
		p1 |= pixbit[(i)][pix >> 4]; \
		p2 |= pixbit[(i)][pix & 0xf]; \
	}

static uint8 ConvertTile2 (uint8 *pCache, uint32 TileAddr, uint32 unused)
{
	uint8 line;

	uint8 *tp = &Memory.VRAM[TileAddr];

	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;

	for (line = 8; line != 0; line--, tp += 2)
	{
		uint8 pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

static uint8 ConvertTile4 (uint8 *pCache, uint32 TileAddr, uint32 unused)
{
	uint8 line;

	uint8 *tp      = (uint8*)&Memory.VRAM[TileAddr];
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;

	for (line = 8; line != 0; line--, tp += 2)
	{
		uint8 pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		DOBIT(16, 2);
		DOBIT(17, 3);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

static uint8 ConvertTile8 (uint8 *pCache, uint32 TileAddr, uint32 unused)
{
	uint8 line;

	uint8 *tp      = (uint8*)&Memory.VRAM[TileAddr];
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;

	for (line = 8; line != 0; line--, tp += 2)
	{
		uint8	pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		DOBIT(16, 2);
		DOBIT(17, 3);
		DOBIT(32, 4);
		DOBIT(33, 5);
		DOBIT(48, 6);
		DOBIT(49, 7);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

#undef DOBIT

#define DOBIT(n, i) \
	if ((pix = hrbit_odd[*(tp1 + (n))])) \
		p1 |= pixbit[(i)][pix]; \
	if ((pix = hrbit_odd[*(tp2 + (n))])) \
		p2 |= pixbit[(i)][pix];

static uint8 ConvertTile2h_odd (uint8 *pCache, uint32 TileAddr, uint32 Tile)
{
	uint8 line;

	uint8 *tp1     = (uint8*)&Memory.VRAM[TileAddr];
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;
   uint8 *tp2 = tp1 + (1 << 4);

	if (Tile == 0x3ff)
		tp2 = tp1 - (0x3ff << 4);

	for (line = 8; line != 0; line--, tp1 += 2, tp2 += 2)
	{
		uint8 pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

static uint8 ConvertTile4h_odd (uint8 *pCache, uint32 TileAddr, uint32 Tile)
{
	uint8 line;

	uint8 *tp1      = (uint8*)&Memory.VRAM[TileAddr];
	uint8 *tp2	    = (uint8*)tp1;
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;

	if (Tile == 0x3ff)
		tp2 -= (0x3ff << 5);
	else
		tp2 += (1 << 5);

	for (line = 8; line != 0; line--, tp1 += 2, tp2 += 2)
	{
		uint8	pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		DOBIT(16, 2);
		DOBIT(17, 3);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

#undef DOBIT

#define DOBIT(n, i) \
	if ((pix = hrbit_even[*(tp1 + (n))])) \
		p1 |= pixbit[(i)][pix]; \
	if ((pix = hrbit_even[*(tp2 + (n))])) \
		p2 |= pixbit[(i)][pix];

static uint8 ConvertTile2h_even (uint8 *pCache, uint32 TileAddr, uint32 Tile)
{
	uint8 line;

	uint8 *tp1 = (uint8*)&Memory.VRAM[TileAddr];
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;
   uint8 *tp2 = tp1 + (1 << 4);

	if (Tile == 0x3ff)
		tp2 = tp1 - (0x3ff << 4);

	for (line = 8; line != 0; line--, tp1 += 2, tp2 += 2)
	{
		uint8	pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

static uint8 ConvertTile4h_even (uint8 *pCache, uint32 TileAddr, uint32 Tile)
{
	uint8 line;

	uint8  *tp1     = (uint8*)&Memory.VRAM[TileAddr];
	uint32 *p       = (uint32 *) pCache;
	uint32 non_zero = 0;
   uint8  *tp2     = tp1 + (1 << 5);

	if (Tile == 0x3ff)
		tp2 = tp1 - (0x3ff << 5);

	for (line = 8; line != 0; line--, tp1 += 2, tp2 += 2)
	{
		uint8	pix;

		uint32 p1 = 0;
		uint32 p2 = 0;

		DOBIT( 0, 0);
		DOBIT( 1, 1);
		DOBIT(16, 2);
		DOBIT(17, 3);
		*p++ = p1;
		*p++ = p2;
		non_zero |= p1 | p2;
	}

	return (non_zero ? TRUE : BLANK_TILE);
}

#undef DOBIT

/* First-level include: Get all the renderers. */

#include "tile.c"

/* Functions to select which converter and renderer to use. */

void S9xSelectTileRenderers_SFXSpeedup (void)
{
	int i;
	GFX.LinesPerTile = 8;

	GFX.DrawTileNomath        = Renderers_DrawTile16Normal1x1[0];
	GFX.DrawClippedTileNomath = Renderers_DrawClippedTile16Normal1x1[0];
	GFX.DrawBackdropNomath    = Renderers_DrawBackdrop16Normal1x1[0];

	i = (Memory.FillRAM[0x2131] & 0x80) ? 4 : 1;
	if (Memory.FillRAM[0x2131] & 0x40)
	{
		i++;
		if (Memory.FillRAM[0x2130] & 2)
			i++;
	}

	GFX.DrawTileMath        = Renderers_DrawTile16Normal1x1[i];
	GFX.DrawClippedTileMath = Renderers_DrawClippedTile16Normal1x1[i];
	GFX.DrawBackdropMath    = Renderers_DrawBackdrop16Normal1x1[i];
}

void S9xSelectTileRenderers (int BGMode, bool8 sub, bool8 obj)
{
	void	(**DT)		(uint32, uint32, uint32, uint32);
	void	(**DCT)		(uint32, uint32, uint32, uint32, uint32, uint32);
	void	(**DMP)		(uint32, uint32, uint32, uint32, uint32, uint32);
	void	(**DB)		(uint32, uint32, uint32);
	void	(**DM7BG1)	(uint32, uint32, int);
	void	(**DM7BG2)	(uint32, uint32, int);
	int i;
	bool8	M7M1, M7M2, interlace, hires;

	M7M1 = PPU.BGMosaic[0] && PPU.Mosaic > 1;
	M7M2 = PPU.BGMosaic[1] && PPU.Mosaic > 1;

	interlace = obj ? FALSE : IPPU.Interlace;
	hires = !sub && (BGMode == 5 || BGMode == 6 || IPPU.PseudoHires);

	if (IPPU.QuadWidthPixels)		/* quad width (Mode 7 hires 4x) */
	{
		DT     = Renderers_DrawTile16Normal4x1;
		DCT    = Renderers_DrawClippedTile16Normal4x1;
		DMP    = Renderers_DrawMosaicPixel16Normal4x1;
		DB     = Renderers_DrawBackdrop16Normal4x1;
		/* M7-mosaic 4x not implemented; mosaic is meaningless under
		   sub-pixel refinement. Fall through to 2x mosaic, which
		   replicates each native pixel twice -- the buffer is 4x
		   wide, so half the columns will be unfilled. Mode 7 mosaic
		   is rare; this is a graceful-degrade rather than a hard
		   failure. */
		DM7BG1 = M7M1 ? Renderers_DrawMode7MosaicBG1Normal2x1 : Renderers_DrawMode7BG1Normal2x1;
		DM7BG2 = M7M2 ? Renderers_DrawMode7MosaicBG2Normal2x1 : Renderers_DrawMode7BG2Normal2x1;
		GFX.LinesPerTile = 8;
	}
	else if (!IPPU.DoubleWidthPixels)	/* normal width */
	{
		DT     = Renderers_DrawTile16Normal1x1;
		DCT    = Renderers_DrawClippedTile16Normal1x1;
		DMP    = Renderers_DrawMosaicPixel16Normal1x1;
		DB     = Renderers_DrawBackdrop16Normal1x1;
		DM7BG1 = M7M1 ? Renderers_DrawMode7MosaicBG1Normal1x1 : Renderers_DrawMode7BG1Normal1x1;
		DM7BG2 = M7M2 ? Renderers_DrawMode7MosaicBG2Normal1x1 : Renderers_DrawMode7BG2Normal1x1;
		GFX.LinesPerTile = 8;
	}
	else if (hires)			/* hires double width */
	{
		DB     = Renderers_DrawBackdrop16Hires;
		DM7BG1 = M7M1 ? Renderers_DrawMode7MosaicBG1Hires : Renderers_DrawMode7BG1Hires;
		DM7BG2 = M7M2 ? Renderers_DrawMode7MosaicBG2Hires : Renderers_DrawMode7BG2Hires;

		if (interlace)
		{
			DT     = Renderers_DrawTile16HiresInterlace;
			DCT    = Renderers_DrawClippedTile16HiresInterlace;
			DMP    = Renderers_DrawMosaicPixel16HiresInterlace;
			GFX.LinesPerTile = 4;
		}
		else
		{
			DT     = Renderers_DrawTile16Hires;
			DCT    = Renderers_DrawClippedTile16Hires;
			DMP    = Renderers_DrawMosaicPixel16Hires;
			GFX.LinesPerTile = 8;
		}
	}
	else				/* normal2x1 - DoubleWidthPixels but not hires */
	{
		DB     = Renderers_DrawBackdrop16Normal2x1;
		DM7BG1 = M7M1 ? Renderers_DrawMode7MosaicBG1Normal2x1 : Renderers_DrawMode7BG1Normal2x1;
		DM7BG2 = M7M2 ? Renderers_DrawMode7MosaicBG2Normal2x1 : Renderers_DrawMode7BG2Normal2x1;

		if (interlace)
		{
			DT     = Renderers_DrawTile16Interlace;
			DCT    = Renderers_DrawClippedTile16Interlace;
			DMP    = Renderers_DrawMosaicPixel16Interlace;
			GFX.LinesPerTile = 4;
		}
		else
		{
			DT     = Renderers_DrawTile16Normal2x1;
			DCT    = Renderers_DrawClippedTile16Normal2x1;
			DMP    = Renderers_DrawMosaicPixel16Normal2x1;
			GFX.LinesPerTile = 8;
		}
	}

	/* Mode 7 hires / bilinear dispatch.

	   Two orthogonal options drive the choice:
	     Settings.Mode7Hires:         0 (off), 2 (2x), 4 (4x)
	     Settings.Mode7HiresBilinear: 0 (off), 1 (stable), 2 (smooth)

	   The stable/smooth distinction is an internal runtime flag
	   inside M7HR_BLEND_AND_WRITE, so a single BL renderer body
	   handles both filter modes. Stable does X-only blending with
	   floor-Y nearest-neighbor (matches HD-no-BL's Y sampling, no
	   one-scanline seam on hostile content). Smooth does the full
	   4-corner bilinear blend (more aggressive smoothing, but can
	   produce a one-scanline seam where adjacent texel rows
	   disagree -- e.g. Tiny Toons rainbow rings).

	   When BL is on at Hires=off, we need a 1x BL renderer that
	   fills the native 256-wide buffer with bilinear samples (one
	   sub-sample per output pixel). When BL is on at Hires=2, the
	   existing 2x BL renderer (two sub-samples per native pixel)
	   applies. When BL is on at Hires=4, the new 4x BL renderer
	   (four sub-samples). HR-only variants drop the BL math
	   entirely, just nearest-neighbor sampling at the chosen rate.

	   Mosaic falls through unchanged at all hires levels -- mosaic
	   is intentionally unrefined. Subscreen also falls through:
	   color math composition stays at native sample rate. */
	if (BGMode == 7 && !sub)
	{
		if (Settings.Mode7Hires == 4 && IPPU.QuadWidthPixels)
		{
			if (Settings.Mode7HiresBilinear)
			{
				if (!M7M1)
					DM7BG1 = Renderers_DrawMode7BG1BL4XNormal1x1;
				if (!M7M2)
					DM7BG2 = Renderers_DrawMode7BG2BL4XNormal1x1;
			}
			else
			{
				if (!M7M1)
					DM7BG1 = Renderers_DrawMode7BG1HR4XNormal1x1;
				if (!M7M2)
					DM7BG2 = Renderers_DrawMode7BG2HR4XNormal1x1;
			}
		}
		else if (Settings.Mode7Hires == 2 && IPPU.DoubleWidthPixels)
		{
			if (Settings.Mode7HiresBilinear)
			{
				if (!M7M1)
					DM7BG1 = Renderers_DrawMode7BG1BLNormal1x1;
				if (!M7M2)
					DM7BG2 = Renderers_DrawMode7BG2BLNormal1x1;
			}
			else
			{
				if (!M7M1)
					DM7BG1 = Renderers_DrawMode7BG1HRNormal1x1;
				if (!M7M2)
					DM7BG2 = Renderers_DrawMode7BG2HRNormal1x1;
			}
		}
		else if (Settings.Mode7HiresBilinear && !IPPU.DoubleWidthPixels)
		{
			/* BL on at Hires=off: bilinear at native 1x width. */
			if (!M7M1)
				DM7BG1 = Renderers_DrawMode7BG1BL1XNormal1x1;
			if (!M7M2)
				DM7BG2 = Renderers_DrawMode7BG2BL1XNormal1x1;
		}
	}

	GFX.DrawTileNomath        = DT[0];
	GFX.DrawClippedTileNomath = DCT[0];
	GFX.DrawMosaicPixelNomath = DMP[0];
	GFX.DrawBackdropNomath    = DB[0];
	GFX.DrawMode7BG1Nomath    = DM7BG1[0];
	GFX.DrawMode7BG2Nomath    = DM7BG2[0];

	if (!Settings.Transparency)
		i = 0;
	else
	{
		i = (Memory.FillRAM[0x2131] & 0x80) ? 4 : 1;
		if (Memory.FillRAM[0x2131] & 0x40)
		{
			i++;
			if (Memory.FillRAM[0x2130] & 2)
				i++;
		}
	}

	GFX.DrawTileMath        = DT[i];
	GFX.DrawClippedTileMath = DCT[i];
	GFX.DrawMosaicPixelMath = DMP[i];
	GFX.DrawBackdropMath    = DB[i];
	GFX.DrawMode7BG1Math    = DM7BG1[i];
	GFX.DrawMode7BG2Math    = DM7BG2[i];
}

void S9xSelectTileConverter_Depth4 (void)
{
	BG.ConvertTile = BG.ConvertTileFlip = ConvertTile4;
	BG.Buffer      = BG.BufferFlip      = IPPU.TileCache[TILE_4BIT];
	BG.Buffered    = BG.BufferedFlip    = IPPU.TileCached[TILE_4BIT];
	BG.TileShift        = 5;
	BG.PaletteShift     = 10 - 4;
	BG.PaletteMask      = 7 << 4;
	BG.DirectColourMode = FALSE;
}

void S9xSelectTileConverter_Depth2 (void)
{
	BG.ConvertTile = BG.ConvertTileFlip = ConvertTile2;
	BG.Buffer      = BG.BufferFlip      = IPPU.TileCache[TILE_2BIT];
	BG.Buffered    = BG.BufferedFlip    = IPPU.TileCached[TILE_2BIT];
	BG.TileShift        = 4;
	BG.PaletteShift     = 10 - 2;
	BG.PaletteMask      = 7 << 2;
	BG.DirectColourMode = FALSE;
}

void S9xSelectTileConverter_Depth8 (void)
{
	BG.ConvertTile      = BG.ConvertTileFlip = ConvertTile8;
	BG.Buffer           = BG.BufferFlip      = IPPU.TileCache[TILE_8BIT];
	BG.Buffered         = BG.BufferedFlip    = IPPU.TileCached[TILE_8BIT];
	BG.TileShift        = 6;
	BG.PaletteShift     = 0;
	BG.PaletteMask      = 0;
	BG.DirectColourMode = Memory.FillRAM[0x2130] & 1;
}

void S9xSelectTileConverter (int depth, bool8 hires, bool8 sub, bool8 mosaic)
{
	switch (depth)
	{
		case 8:
			BG.ConvertTile      = BG.ConvertTileFlip = ConvertTile8;
			BG.Buffer           = BG.BufferFlip      = IPPU.TileCache[TILE_8BIT];
			BG.Buffered         = BG.BufferedFlip    = IPPU.TileCached[TILE_8BIT];
			BG.TileShift        = 6;
			BG.PaletteShift     = 0;
			BG.PaletteMask      = 0;
			BG.DirectColourMode = Memory.FillRAM[0x2130] & 1;

			break;

		case 4:
			if (hires)
			{
				if (sub || mosaic)
				{
					BG.ConvertTile     = ConvertTile4h_even;
					BG.Buffer          = IPPU.TileCache[TILE_4BIT_EVEN];
					BG.Buffered        = IPPU.TileCached[TILE_4BIT_EVEN];
					BG.ConvertTileFlip = ConvertTile4h_odd;
					BG.BufferFlip      = IPPU.TileCache[TILE_4BIT_ODD];
					BG.BufferedFlip    = IPPU.TileCached[TILE_4BIT_ODD];
				}
				else
				{
					BG.ConvertTile     = ConvertTile4h_odd;
					BG.Buffer          = IPPU.TileCache[TILE_4BIT_ODD];
					BG.Buffered        = IPPU.TileCached[TILE_4BIT_ODD];
					BG.ConvertTileFlip = ConvertTile4h_even;
					BG.BufferFlip      = IPPU.TileCache[TILE_4BIT_EVEN];
					BG.BufferedFlip    = IPPU.TileCached[TILE_4BIT_EVEN];
				}
			}
			else
			{
				BG.ConvertTile = BG.ConvertTileFlip = ConvertTile4;
				BG.Buffer      = BG.BufferFlip      = IPPU.TileCache[TILE_4BIT];
				BG.Buffered    = BG.BufferedFlip    = IPPU.TileCached[TILE_4BIT];
			}

			BG.TileShift        = 5;
			BG.PaletteShift     = 10 - 4;
			BG.PaletteMask      = 7 << 4;
			BG.DirectColourMode = FALSE;

			break;

		case 2:
			if (hires)
			{
				if (sub || mosaic)
				{
					BG.ConvertTile     = ConvertTile2h_even;
					BG.Buffer          = IPPU.TileCache[TILE_2BIT_EVEN];
					BG.Buffered        = IPPU.TileCached[TILE_2BIT_EVEN];
					BG.ConvertTileFlip = ConvertTile2h_odd;
					BG.BufferFlip      = IPPU.TileCache[TILE_2BIT_ODD];
					BG.BufferedFlip    = IPPU.TileCached[TILE_2BIT_ODD];
				}
				else
				{
					BG.ConvertTile     = ConvertTile2h_odd;
					BG.Buffer          = IPPU.TileCache[TILE_2BIT_ODD];
					BG.Buffered        = IPPU.TileCached[TILE_2BIT_ODD];
					BG.ConvertTileFlip = ConvertTile2h_even;
					BG.BufferFlip      = IPPU.TileCache[TILE_2BIT_EVEN];
					BG.BufferedFlip    = IPPU.TileCached[TILE_2BIT_EVEN];
				}
			}
			else
			{
				BG.ConvertTile = BG.ConvertTileFlip = ConvertTile2;
				BG.Buffer      = BG.BufferFlip      = IPPU.TileCache[TILE_2BIT];
				BG.Buffered    = BG.BufferedFlip    = IPPU.TileCached[TILE_2BIT];
			}

			BG.TileShift        = 4;
			BG.PaletteShift     = 10 - 2;
			BG.PaletteMask      = 7 << 2;
			BG.DirectColourMode = FALSE;

			break;
	}
}

/*****************************************************************************/
#else
#ifndef NAME1 /* First-level: Get all the renderers. */
/*****************************************************************************/

#define GET_CACHED_TILE() \
	uint32	TileNumber, TileAddr; \
	TileAddr = BG.TileAddress + ((Tile & 0x3ff) << BG.TileShift); \
	if (Tile & 0x100) \
		TileAddr += BG.NameSelect; \
	TileAddr &= 0xffff; \
	TileNumber = TileAddr >> BG.TileShift; \
	if (Tile & H_FLIP) \
	{ \
		pCache = &BG.BufferFlip[TileNumber << 6]; \
		if (!BG.BufferedFlip[TileNumber]) \
			BG.BufferedFlip[TileNumber] = BG.ConvertTileFlip(pCache, TileAddr, Tile & 0x3ff); \
	} \
	else \
	{ \
		pCache = &BG.Buffer[TileNumber << 6]; \
		if (!BG.Buffered[TileNumber]) \
			BG.Buffered[TileNumber] = BG.ConvertTile(pCache, TileAddr, Tile & 0x3ff); \
	}

#define IS_BLANK_TILE() \
	(BG.Buffered[TileNumber] == BLANK_TILE)

#define SELECT_PALETTE() \
   GFX.RealScreenColors = &IPPU.ScreenColors[((Tile >> BG.PaletteShift) & BG.PaletteMask) + BG.StartPalette]; \
	if (BG.DirectColourMode) \
	{ \
		if (IPPU.DirectColourMapsNeedRebuild) \
			S9xBuildDirectColourMaps(); \
		GFX.RealScreenColors = DirectColourMaps[(Tile >> 10) & 7]; \
	} \
	GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors

#define NOMATH(Op, Main, Sub, SD) \
	(Main)

#define REGMATH(Op, Main, Sub, SD) \
	(COLOR_##Op((Main), ((SD) & 0x20) ? (Sub) : GFX.FixedColour))

#define MATHF1_2(Op, Main, Sub, SD) \
	(GFX.ClipColors ? (COLOR_##Op((Main), GFX.FixedColour)) : (COLOR_##Op##1_2((Main), GFX.FixedColour)))

#define MATHS1_2(Op, Main, Sub, SD) \
	(GFX.ClipColors ? REGMATH(Op, Main, Sub, SD) : (((SD) & 0x20) ? COLOR_##Op##1_2((Main), (Sub)) : COLOR_##Op((Main), GFX.FixedColour)))

/* Basic routine to render an unclipped tile.
   Input parameters:

   BPSTART = either StartLine or (StartLine * 2 + BG.InterlaceLine),
   so interlace modes can render every other line from the tile.

   PITCH = 1 or 2, again so interlace can count lines properly.
   DRAW_PIXEL(N, M) is a routine to actually draw the pixel. N is the pixel in the row to draw,
   and M is a test which if false means the pixel should be skipped.

   Z1 is the "draw if Z1 > cur_depth".

   Z2 is the "cur_depth = new_depth". OBJ need the two separate.

   Pix is the pixel to draw. */

#define Z1	GFX.Z1
#define Z2	GFX.Z2

#define DRAW_TILE() \
	uint8	*pCache, *bp, Pix, n; \
	int32	l; \
	\
	GET_CACHED_TILE(); \
	if (IS_BLANK_TILE()) \
		return; \
	SELECT_PALETTE(); \
	\
	if (!(Tile & (V_FLIP | H_FLIP))) \
	{ \
		bp = pCache + BPSTART; \
		for (l = LineCount; l > 0; l--, bp += 8 * PITCH, Offset += GFX.PPL) \
		{ \
         for (n = 0; n < 8; n++) \
         { \
            DRAW_PIXEL(n, Pix = bp[n]); \
         } \
		} \
	} \
	else \
	if (!(Tile & V_FLIP)) \
	{ \
		bp = pCache + BPSTART; \
		for (l = LineCount; l > 0; l--, bp += 8 * PITCH, Offset += GFX.PPL) \
      { \
         for (n = 0; n < 8; n++) \
         { \
            DRAW_PIXEL(n, Pix = bp[7 - n]); \
         } \
      } \
	} \
	else \
	if (!(Tile & H_FLIP)) \
	{ \
		bp = pCache + 56 - BPSTART; \
		for (l = LineCount; l > 0; l--, bp -= 8 * PITCH, Offset += GFX.PPL) \
      { \
         for (n = 0; n < 8; n++) \
         { \
            DRAW_PIXEL(n, Pix = bp[n]); \
         } \
      } \
	} \
	else \
	{ \
		bp = pCache + 56 - BPSTART; \
		for (l = LineCount; l > 0; l--, bp -= 8 * PITCH, Offset += GFX.PPL) \
      { \
         for (n = 0; n < 8; n++) \
         { \
            DRAW_PIXEL(n, Pix = bp[7 - n]); \
         } \
      } \
	}

#define NAME1	DrawTile16
#define ARGS	uint32 Tile, uint32 Offset, uint32 StartLine, uint32 LineCount

/* Second-level include: Get the DrawTile16 renderers. */

#include "tile.c"

#undef NAME1
#undef ARGS
#undef DRAW_TILE
#undef Z1
#undef Z2

/* Basic routine to render a clipped tile. Inputs same as above. */

#define Z1	GFX.Z1
#define Z2	GFX.Z2

#define DRAW_TILE() \
	uint8	*pCache, *bp, Pix, w; \
	int32	l; \
	\
	GET_CACHED_TILE(); \
	if (IS_BLANK_TILE()) \
		return; \
	SELECT_PALETTE(); \
	\
	if (!(Tile & (V_FLIP | H_FLIP))) \
	{ \
		bp = pCache + BPSTART; \
		for (l = LineCount; l > 0; l--, bp += 8 * PITCH, Offset += GFX.PPL) \
		{ \
		 w = Width; \
         switch (StartPixel) \
         { \
            case 0: DRAW_PIXEL(0, Pix = bp[0]); if (!--w) break; /* Fall through */ \
            case 1: DRAW_PIXEL(1, Pix = bp[1]); if (!--w) break; /* Fall through */ \
            case 2: DRAW_PIXEL(2, Pix = bp[2]); if (!--w) break; /* Fall through */ \
            case 3: DRAW_PIXEL(3, Pix = bp[3]); if (!--w) break; /* Fall through */ \
            case 4: DRAW_PIXEL(4, Pix = bp[4]); if (!--w) break; /* Fall through */ \
            case 5: DRAW_PIXEL(5, Pix = bp[5]); if (!--w) break; /* Fall through */ \
            case 6: DRAW_PIXEL(6, Pix = bp[6]); if (!--w) break; /* Fall through */ \
            case 7: DRAW_PIXEL(7, Pix = bp[7]); break; \
         } \
		} \
	} \
	else \
	if (!(Tile & V_FLIP)) \
	{ \
		bp = pCache + BPSTART; \
		for (l = LineCount; l > 0; l--, bp += 8 * PITCH, Offset += GFX.PPL) \
		{ \
		   w = Width; \
         switch (StartPixel) \
         { \
            case 0: DRAW_PIXEL(0, Pix = bp[7]); if (!--w) break; /* Fall through */ \
            case 1: DRAW_PIXEL(1, Pix = bp[6]); if (!--w) break; /* Fall through */ \
            case 2: DRAW_PIXEL(2, Pix = bp[5]); if (!--w) break; /* Fall through */ \
            case 3: DRAW_PIXEL(3, Pix = bp[4]); if (!--w) break; /* Fall through */ \
            case 4: DRAW_PIXEL(4, Pix = bp[3]); if (!--w) break; /* Fall through */ \
            case 5: DRAW_PIXEL(5, Pix = bp[2]); if (!--w) break; /* Fall through */ \
            case 6: DRAW_PIXEL(6, Pix = bp[1]); if (!--w) break; /* Fall through */ \
            case 7: DRAW_PIXEL(7, Pix = bp[0]); break; \
         } \
		} \
	} \
	else \
	if (!(Tile & H_FLIP)) \
	{ \
		bp = pCache + 56 - BPSTART; \
		for (l = LineCount; l > 0; l--, bp -= 8 * PITCH, Offset += GFX.PPL) \
		{ \
		 w = Width; \
         switch (StartPixel) \
         { \
            case 0: DRAW_PIXEL(0, Pix = bp[0]); if (!--w) break; /* Fall through */ \
            case 1: DRAW_PIXEL(1, Pix = bp[1]); if (!--w) break; /* Fall through */ \
            case 2: DRAW_PIXEL(2, Pix = bp[2]); if (!--w) break; /* Fall through */ \
            case 3: DRAW_PIXEL(3, Pix = bp[3]); if (!--w) break; /* Fall through */ \
            case 4: DRAW_PIXEL(4, Pix = bp[4]); if (!--w) break; /* Fall through */ \
            case 5: DRAW_PIXEL(5, Pix = bp[5]); if (!--w) break; /* Fall through */ \
            case 6: DRAW_PIXEL(6, Pix = bp[6]); if (!--w) break; /* Fall through */ \
            case 7: DRAW_PIXEL(7, Pix = bp[7]); break; \
         } \
		} \
	} \
	else \
	{ \
		bp = pCache + 56 - BPSTART; \
		for (l = LineCount; l > 0; l--, bp -= 8 * PITCH, Offset += GFX.PPL) \
		{ \
		 w = Width; \
       switch (StartPixel) \
       { \
          case 0: DRAW_PIXEL(0, Pix = bp[7]); if (!--w) break; /* Fall through */ \
          case 1: DRAW_PIXEL(1, Pix = bp[6]); if (!--w) break; /* Fall through */ \
          case 2: DRAW_PIXEL(2, Pix = bp[5]); if (!--w) break; /* Fall through */ \
          case 3: DRAW_PIXEL(3, Pix = bp[4]); if (!--w) break; /* Fall through */ \
          case 4: DRAW_PIXEL(4, Pix = bp[3]); if (!--w) break; /* Fall through */ \
          case 5: DRAW_PIXEL(5, Pix = bp[2]); if (!--w) break; /* Fall through */ \
          case 6: DRAW_PIXEL(6, Pix = bp[1]); if (!--w) break; /* Fall through */ \
          case 7: DRAW_PIXEL(7, Pix = bp[0]); break; \
       } \
		} \
	}

#define NAME1	DrawClippedTile16
#define ARGS	uint32 Tile, uint32 Offset, uint32 StartPixel, uint32 Width, uint32 StartLine, uint32 LineCount

/* Second-level include: Get the DrawClippedTile16 renderers. */

#include "tile.c"

#undef NAME1
#undef ARGS
#undef DRAW_TILE
#undef Z1
#undef Z2

/* Basic routine to render a single mosaic pixel.
   DRAW_PIXEL, BPSTART, Z1, Z2 and Pix are the same as above, but PITCH is not used. */

#define Z1	GFX.Z1
#define Z2	GFX.Z2

#define DRAW_TILE() \
	uint8	*pCache, Pix; \
	int32	l, w; \
	\
	GET_CACHED_TILE(); \
	if (IS_BLANK_TILE()) \
		return; \
	SELECT_PALETTE(); \
	\
	if (Tile & H_FLIP) \
		StartPixel = 7 - StartPixel; \
	\
	if (Tile & V_FLIP) \
		Pix = pCache[56 - BPSTART + StartPixel]; \
	else \
		Pix = pCache[BPSTART + StartPixel]; \
	\
	if (Pix) \
	{ \
		for (l = LineCount; l > 0; l--, Offset += GFX.PPL) \
		{ \
			for (w = Width - 1; w >= 0; w--) \
				DRAW_PIXEL(w, 1); \
		} \
	}

#define NAME1	DrawMosaicPixel16
#define ARGS	uint32 Tile, uint32 Offset, uint32 StartLine, uint32 StartPixel, uint32 Width, uint32 LineCount

/* Second-level include: Get the DrawMosaicPixel16 renderers. */

#include "tile.c"

#undef NAME1
#undef ARGS
#undef DRAW_TILE
#undef Z1
#undef Z2

/* ====================================================================
 * DrawBackdrop16 renderers
 * ====================================================================
 *
 * One NAME1 family: DrawBackdrop16. Fills a horizontal range of the
 * scanline buffer with the backdrop colour (palette index 0). No
 * tile lookup, no priority test against per-pixel data -- the
 * backdrop is always at depth 1, drawn after all BG/OBJ layers, so
 * the Z-test reduces to "1 > GFX.DB[N]" (i.e. only fill where
 * nothing has been written yet).
 *
 * The original templated form baked Z1 = Z2 = 1 and Pix = 0 as
 * preprocessor #defines around #include "tile.c". The de-templated
 * form bakes the same constants into the plotter macros directly.
 *
 * ARGS shape differs from Mode 7 / tile families: takes (Offset,
 * Left, Right). Offset is the per-call scanline base offset, not
 * a local computed inside the body.
 *
 * Four NAME2 variants emitted: Normal1x1, Normal2x1, Normal4x1,
 * Hires. All four are referenced by the dispatcher; the original
 * templated form used NO_INTERLACE to skip Interlace and
 * HiresInterlace, and we do the same here by just not emitting them.
 *
 * 7 math variants per NAME2, 4 NAME2 = 28 functions + 4 dispatch
 * arrays.
 */

/* ---- Section-internal pixel plotters -------------------------------
 *
 * Mirror the original DRAW_PIXEL_* macro shapes exactly, with Z1,
 * Z2, Pix, and M (the per-call test) baked in as literals: Z = 1,
 * Pix = 0, M = 1. The `&& (1)` clause is preserved verbatim so the
 * expression tree the compiler sees is identical to the templated
 * expansion.
 *
 * Shape: (N, MATH_SELECTOR, MATH_OP). N is the scanline x-index;
 * the output index depends on the plotter's pixel rate (1x, 2x,
 * 4x, or 2x-hires). N is unparenthesized inside the body to match
 * the original DRAW_PIXEL_* substitution semantics. */

/* Normal1x1: writes one pixel at Offset + N. */
#define BACKDROP_PIXEL_N1x1(N, MATH_SELECTOR, MATH_OP) \
    if (1 > GFX.DB[Offset + N] && (1)) \
    { \
        GFX.S[Offset + N] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[0], \
            GFX.SubScreen[Offset + N], \
            GFX.SubZBuffer[Offset + N]); \
        GFX.DB[Offset + N] = 1; \
    }

/* Normal2x1: writes two adjacent pixels at Offset + 2*N and +2*N+1. */
#define BACKDROP_PIXEL_N2x1(N, MATH_SELECTOR, MATH_OP) \
    if (1 > GFX.DB[Offset + 2 * N] && (1)) \
    { \
        GFX.S[Offset + 2 * N] = GFX.S[Offset + 2 * N + 1] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[0], \
            GFX.SubScreen[Offset + 2 * N], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = 1; \
    }

/* Normal4x1: writes four adjacent pixels at Offset + 4*N..+4*N+3. */
#define BACKDROP_PIXEL_N4x1(N, MATH_SELECTOR, MATH_OP) \
    if (1 > GFX.DB[Offset + 4 * N] && (1)) \
    { \
        uint16 cc__ = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[0], \
            GFX.SubScreen[Offset + 4 * N], \
            GFX.SubZBuffer[Offset + 4 * N]); \
        GFX.S[Offset + 4 * N] = GFX.S[Offset + 4 * N + 1] = GFX.S[Offset + 4 * N + 2] = GFX.S[Offset + 4 * N + 3] = cc__; \
        GFX.DB[Offset + 4 * N] = GFX.DB[Offset + 4 * N + 1] = GFX.DB[Offset + 4 * N + 2] = GFX.DB[Offset + 4 * N + 3] = 1; \
    }

/* Hires (H2x1): main-screen pixel goes through MATH normally;
 * sub-screen-side pixel uses the swapped operand order. */
#define BACKDROP_PIXEL_H2x1(N, MATH_SELECTOR, MATH_OP) \
    if (1 > GFX.DB[Offset + 2 * N] && (1)) \
    { \
        GFX.S[Offset + 2 * N] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[0], \
            GFX.SubScreen[Offset + 2 * N], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.S[Offset + 2 * N + 1] = MATH_SELECTOR(MATH_OP, \
            (GFX.ClipColors ? 0 : GFX.SubScreen[Offset + 2 * N + 2]), \
            GFX.RealScreenColors[0], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = 1; \
    }

/* ---- Section-internal tile body ------------------------------------
 *
 * Per-line walk, one scanline at a time. Body identical to the
 * previously-templated DRAW_TILE() macro for backdrop (now deleted),
 * parameterized for explicit MATH_SELECTOR / MATH_OP / PIXEL_PLOT. */
#define TILE_BODY_BACKDROP(MATH_SELECTOR, MATH_OP, PIXEL_PLOT) \
{ \
    uint32 l, x; \
    GFX.RealScreenColors = IPPU.ScreenColors; \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    for (l = GFX.StartY; l <= GFX.EndY; l++, Offset += GFX.PPL) \
    { \
        for (x = Left; x < Right; x++) \
            PIXEL_PLOT(x, MATH_SELECTOR, MATH_OP) \
    } \
}

/* ---- Outer fan-out ------------------------------------------------- */
#define DEFINE_BACKDROP_FN(suffix, MATH_SELECTOR, MATH_OP, NAME2_TAG, PIXEL_PLOT) \
static void DrawBackdrop16##suffix##NAME2_TAG (uint32 Offset, uint32 Left, uint32 Right) \
TILE_BODY_BACKDROP(MATH_SELECTOR, MATH_OP, PIXEL_PLOT)

/* Per-NAME2 7-fold math fan-out plus dispatch array. */
#define DEFINE_BACKDROP_NAME2(NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(_,        NOMATH,   ADD, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(Add_,     REGMATH,  ADD, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(AddF1_2_, MATHF1_2, ADD, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(AddS1_2_, MATHS1_2, ADD, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(Sub_,     REGMATH,  SUB, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(SubF1_2_, MATHF1_2, SUB, NAME2_TAG, PIXEL_PLOT) \
DEFINE_BACKDROP_FN(SubS1_2_, MATHS1_2, SUB, NAME2_TAG, PIXEL_PLOT) \
static void (*Renderers_DrawBackdrop16##NAME2_TAG[7]) (uint32, uint32, uint32) = \
{ \
    DrawBackdrop16_##NAME2_TAG, \
    DrawBackdrop16Add_##NAME2_TAG, \
    DrawBackdrop16AddF1_2_##NAME2_TAG, \
    DrawBackdrop16AddS1_2_##NAME2_TAG, \
    DrawBackdrop16Sub_##NAME2_TAG, \
    DrawBackdrop16SubF1_2_##NAME2_TAG, \
    DrawBackdrop16SubS1_2_##NAME2_TAG, \
};

/* Emit the four NAME2 variants the dispatcher uses. */
DEFINE_BACKDROP_NAME2(Normal1x1, BACKDROP_PIXEL_N1x1)
DEFINE_BACKDROP_NAME2(Normal2x1, BACKDROP_PIXEL_N2x1)
DEFINE_BACKDROP_NAME2(Normal4x1, BACKDROP_PIXEL_N4x1)
DEFINE_BACKDROP_NAME2(Hires,     BACKDROP_PIXEL_H2x1)

#undef DEFINE_BACKDROP_NAME2
#undef DEFINE_BACKDROP_FN
#undef TILE_BODY_BACKDROP
#undef BACKDROP_PIXEL_H2x1
#undef BACKDROP_PIXEL_N4x1
#undef BACKDROP_PIXEL_N2x1
#undef BACKDROP_PIXEL_N1x1

/* End of DrawBackdrop16 de-templated section.
 * ==================================================================== */

/* All Mode 7 renderer families (native, mosaic, HR2X, HR4X, BL2X,
 * BL4X, BL1X) are de-templated and emit their functions explicitly
 * via section-local fan-out macros (DEFINE_M7_BG, DEFINE_M7_HR_FAMILY,
 * etc.). The file-scope Z1/Z2/MASK/DCMODE/BG/NO_INTERLACE #defines
 * that the templated form previously needed are gone. Each
 * de-templated section passes the equivalent values as explicit
 * macro parameters. */

#define CLIP_10_BIT_SIGNED(a)	(((a) & 0x2000) ? ((a) | ~0x3ff) : ((a) & 0x3ff))

extern struct SLineMatrixData	LineMatrixData[240];

/* High-resolution Mode 7 with bilinear filtering: same output rate
   as the nearest-neighbour HR family (de-templated below), but
   each output pixel is blended from the four texels surrounding
   the fractional sample position instead of taking the nearest
   texel.

   Per output sample:
     Xi = X >> 8, Yi = Y >> 8         (integer top-left texel coords)
     Xf = X & 0xff, Yf = Y & 0xff     (fractional weights, 0..255)
     TL, TR, BL, BR = the four texels at (Xi,Yi), (Xi+1,Yi),
                                          (Xi,Yi+1), (Xi+1,Yi+1)
     Each texel resolves to an RGB565 colour via
     GFX.ScreenColors[palette_index]. Palette index 0 contributes
     no weight to the blend (treated as transparent); see
     M7HR_BLEND_AND_WRITE for the per-corner opacity handling.

   The bilinear blend is the standard tensor-product of horizontal
   then vertical linear interpolation, with weights computed as
     wx0 = 256 - Xf, wx1 = Xf
     wy0 = 256 - Yf, wy1 = Yf
   and per-channel rounding via >>16 after summing the four weighted
   contributions.

   The blended colour goes through MATH() like nearest-neighbour Mode
   7 does, so colour math (CGADSUB / CGWSEL) works on bilinear-
   filtered Mode 7. The 7 generated functions per (NAME1, NAME2)
   pair are therefore genuinely distinct -- one per math op. (This
   was not always true: an earlier version of the BL path bypassed
   MATH and the 7 variants collapsed to one. Commit 6d214cf -- "Mode
   7 hires bilinear: respect color math" -- fixed that.) */

/* Look up one bilinear corner texel.

   Parameters:
     X_, Y_   integer texel coordinates (already masked to the
              valid VRAM range by the caller).
     Pix_     out: palette index, with the caller's MASK applied.
     vram1    pointer to the start of VRAM tile data
              (Memory.VRAM + 1; computed once per renderer call).
     mask     palette mask: 0xff for BG1, 0x7f for BG2 EXTBG. */
#define M7HR_LOOKUP_PIX(X_, Y_, Pix_, vram1, mask) \
	{ \
		int X__ = (X_); \
		int Y__ = (Y_); \
		uint8 *TileData__ = (vram1) + (Memory.VRAM[((Y__ & ~7) << 5) + ((X__ >> 2) & ~1)] << 7); \
		uint8 b__ = *(TileData__ + ((Y__ & 7) << 4) + ((X__ & 7) << 1)); \
		(Pix_) = b__ & (mask); \
	}

/* Same as M7HR_LOOKUP_PIX but additionally returns the raw byte
   (before MASK is applied). The raw byte's bit 0x80 is the BG2
   per-pixel priority bit in EXTBG mode. */
#define M7HR_LOOKUP_PIX_RAW(X_, Y_, Pix_, RawByte_, vram1, mask) \
	{ \
		int X__ = (X_); \
		int Y__ = (Y_); \
		uint8 *TileData__ = (vram1) + (Memory.VRAM[((Y__ & ~7) << 5) + ((X__ >> 2) & ~1)] << 7); \
		uint8 b__ = *(TileData__ + ((Y__ & 7) << 4) + ((X__ & 7) << 1)); \
		(Pix_) = b__ & (mask); \
		(RawByte_) = b__; \
	}

/* Look up the four bilinear corners around fractional position
   (Xi, Yi) where Xi, Yi are integer texel coordinates already
   masked to [0..0x3ff]. Detects the common case where all four
   corners fall within the same 8x8 tile (Xi & 7 != 7 and
   Yi & 7 != 7) and does ONE map lookup instead of FOUR. About
   76% of bilinear samples hit this fast path; the slow path
   handles the tile-boundary edges by falling back to four
   separate lookups via M7HR_LOOKUP_PIX.

   Also returns b_tl_raw - the raw (unmasked) byte for the TL
   corner. Used by BG2BL to recover the per-pixel priority bit
   (b_tl_raw & 0x80) in EXTBG mode; ignored by BG1BL where Z is
   constant.

   Parameters:
     Xi_, Yi_                       integer texel coords (in-range)
     p_tl, p_tr, p_bl, p_br         out: four corner palette indices
     b_tl_raw                       out: raw TL byte before MASK
     vram1, mask                    see M7HR_LOOKUP_PIX */
#define M7HR_LOOKUP_4(Xi_, Yi_, p_tl, p_tr, p_bl, p_br, b_tl_raw, vram1, mask) \
	{ \
		int Xi__ = (Xi_); \
		int Yi__ = (Yi_); \
		int xib__ = Xi__ & 7; \
		int yib__ = Yi__ & 7; \
		if (xib__ != 7 && yib__ != 7) \
		{ \
			uint8 *TileData__ = (vram1) + (Memory.VRAM[((Yi__ & ~7) << 5) + ((Xi__ >> 2) & ~1)] << 7); \
			uint8 *row0__ = TileData__ + (yib__ << 4); \
			uint8 *row1__ = TileData__ + ((yib__ + 1) << 4); \
			uint8 b_tl__ = row0__[xib__       << 1]; \
			(b_tl_raw) = b_tl__; \
			(p_tl) = b_tl__ & (mask); \
			(p_tr) = row0__[(xib__ + 1) << 1] & (mask); \
			(p_bl) = row1__[xib__       << 1] & (mask); \
			(p_br) = row1__[(xib__ + 1) << 1] & (mask); \
		} \
		else \
		{ \
			int Xi1__ = (Xi__ + 1) & 0x3ff; \
			int Yi1__ = (Yi__ + 1) & 0x3ff; \
			M7HR_LOOKUP_PIX_RAW(Xi__, Yi__, (p_tl), (b_tl_raw), (vram1), (mask)); \
			M7HR_LOOKUP_PIX(Xi1__, Yi__, (p_tr), (vram1), (mask)); \
			M7HR_LOOKUP_PIX(Xi__, Yi1__, (p_bl), (vram1), (mask)); \
			M7HR_LOOKUP_PIX(Xi1__, Yi1__, (p_br), (vram1), (mask)); \
		} \
	}

/* Stable (X-only) blend. Vertical blending across texel rows can
   produce muddy artifacts when adjacent rows contain dissimilar
   content (e.g. Tiny Toons rainbow rings, palette-rotation effects,
   anything with horizontal banding or per-line CGRAM/VRAM HDMA),
   because the bilinear blend averages dissimilar palette entries
   into colors visibly different from both source rows. Stable
   ignores the bottom row entirely: the caller passes the top row's
   two corners (TL and TR), and only X is interpolated.

   This is the safer default. It matches HD-no-BL's Y sampling
   exactly (floor Y) and inherits its seam-free behaviour, while
   still smoothing X-axis aliasing which is the main visible win
   for Mode 7 perspective. */
#define M7HR_BLEND_RGB(out, TL, TR, Xf) \
	{ \
		uint32 wx1 = (Xf), wx0 = 256 - wx1; \
		uint32 r = (((TL) >> 11) & 0x1f) * wx0 + (((TR) >> 11) & 0x1f) * wx1; \
		uint32 g = (((TL) >>  6) & 0x1f) * wx0 + (((TR) >>  6) & 0x1f) * wx1; \
		uint32 b = (((TL)      ) & 0x1f) * wx0 + (((TR)      ) & 0x1f) * wx1; \
		(out) = (uint16)(((r >> 8) << 11) | ((g >> 8) << 6) | (b >> 8)); \
	}

/* Smooth (4-corner) blend. Full bilinear interpolation across all
   four texels surrounding the fractional sample position. More
   aggressive smoothing than stable, particularly along the Y axis,
   but can produce a one-scanline seam where adjacent texel rows
   differ abruptly (the blend lands far from both rows -- a "muddy"
   average that doesn't match either neighbour). */
#define M7HR_BLEND_RGB_4C(out, TL, TR, BL, BR, Xf, Yf) \
	{ \
		uint32 wx1 = (Xf), wx0 = 256 - wx1; \
		uint32 wy1 = (Yf), wy0 = 256 - wy1; \
		uint32 w_tl = wx0 * wy0, w_tr = wx1 * wy0; \
		uint32 w_bl = wx0 * wy1, w_br = wx1 * wy1; \
		uint32 r = (((TL) >> 11) & 0x1f) * w_tl + (((TR) >> 11) & 0x1f) * w_tr \
		         + (((BL) >> 11) & 0x1f) * w_bl + (((BR) >> 11) & 0x1f) * w_br; \
		uint32 g = (((TL) >>  6) & 0x1f) * w_tl + (((TR) >>  6) & 0x1f) * w_tr \
		         + (((BL) >>  6) & 0x1f) * w_bl + (((BR) >>  6) & 0x1f) * w_br; \
		uint32 b = (((TL)      ) & 0x1f) * w_tl + (((TR)      ) & 0x1f) * w_tr \
		         + (((BL)      ) & 0x1f) * w_bl + (((BR)      ) & 0x1f) * w_br; \
		(out) = (uint16)(((r >> 16) << 11) | ((g >> 16) << 6) | (b >> 16)); \
	}

/* Look up four corners from the fill tile (tile 0) - used when the
   sample is out-of-range and PPU.Mode7Repeat == 3. The corner index
   into the fill tile is (X & 7, Y & 7) which always falls within
   the same 8x8 fill tile, so this is a single TileData lookup with
   four pixel reads.

   Also returns b_tl_raw - the raw (unmasked) byte for the TL corner.
   See M7HR_LOOKUP_4 for usage and parameter meanings. */
#define M7HR_LOOKUP_4_FILL(Xi_, Yi_, p_tl, p_tr, p_bl, p_br, b_tl_raw, vram1, mask) \
	{ \
		int Xi__ = (Xi_); \
		int Yi__ = (Yi_); \
		int xib0__ =  Xi__      & 7; \
		int yib0__ =  Yi__      & 7; \
		int xib1__ = (Xi__ + 1) & 7; \
		int yib1__ = (Yi__ + 1) & 7; \
		uint8 *row0__ = (vram1) + (yib0__ << 4); \
		uint8 *row1__ = (vram1) + (yib1__ << 4); \
		uint8 b_tl__ = row0__[xib0__ << 1]; \
		(b_tl_raw) = b_tl__; \
		(p_tl) = b_tl__ & (mask); \
		(p_tr) = row0__[xib1__ << 1] & (mask); \
		(p_bl) = row1__[xib0__ << 1] & (mask); \
		(p_br) = row1__[xib1__ << 1] & (mask); \
	}

/* Given four corner palette indices, the raw TL byte (for BG2's
   per-pixel priority bit), and fractional weights, do the
   configured Mode 7 blend (stable X-only on floor-Y, or smooth
   4-corner bilinear) and write the result to GFX.S[offset+out_offset]
   / GFX.DB[offset+out_offset] subject to the Z test. Shared between
   the Mode7Repeat == 0 (wrap) path and the in-range case of the
   Mode7Repeat != 0 paths.

   Settings.Mode7HiresBilinear selects the filter:
     == 1 (stable)  Y is nearest-neighbor (floor Yi, mirroring HD
                    Mode 7's sampling). X is interpolated between
                    the row's two corners. Cannot produce one-line
                    Y seams.
     == 2 (smooth)  Full 4-corner bilinear. Smoother on gradient
                    content, but can produce a one-scanline seam
                    on hostile content where adjacent texel rows
                    contain dissimilar palette entries.

   The raw TL byte is exposed as local 'b' so that z1_expr / z2_expr
   parameters can reference 'b & 0x80' the same way the native HR
   path does. For BG1, z1_expr is constant and 'b' is unused; the
   compiler eliminates the dead read.

   Index 0 is the SNES universal transparency index. Native HR's
   DRAW_PIXEL skips writes when (M = Pix & MASK) is zero, leaving
   GFX.DB[N] = 0 so DrawBackdrop() can fill that pixel later. This
   blend mirrors that semantics with a 4-bit per-corner opacity
   bitmap (op_mask):

     op_mask == 0x0 -> all four corners transparent. Skip the
                       pixel; backdrop will fill at end of frame.
     op_mask == 0xF -> all four corners opaque. Take the fast
                       path: same-index early-out for solid regions,
                       otherwise the configured blend.
     op_mask other  -> mixed transparency. Alpha-aware variant of
                       the configured blend: zero-index corners
                       contribute zero weight; the remaining weight
                       is renormalized so texture edges fade
                       smoothly into the backdrop instead of
                       producing a sharp seam.

   Parameters:
     out_offset                       offset into the scanline buffer
                                      to write (relative to 'offset')
     p_tl, p_tr, p_bl, p_br           4 corner palette indices
     b_tl_raw                         raw TL byte (pre-mask)
     Xf, Yf                           fractional weights, 0..255
     math_selector, math_op           colour-math selector and op,
                                      such that
                                      math_selector(math_op, A, B, C)
                                      yields the colour-math result.
                                      Use NOMATH/ADD when no math
                                      applies; otherwise one of
                                      REGMATH, MATHF1_2, MATHS1_2
                                      paired with ADD or SUB. Same
                                      tokens as the level-3
                                      template's MATH selector.
     z1_expr, z2_expr                 Z-test value and Z-write
                                      value. Often equal but not
                                      always (OBJ uses different
                                      Z1 and Z2). May reference
                                      the local 'b'.
     offset                           scanline base offset into the
                                      GFX.S / GFX.DB / GFX.SubScreen
                                      / GFX.SubZBuffer arrays. */
#define M7HR_BLEND_AND_WRITE(out_offset, p_tl, p_tr, p_bl, p_br, b_tl_raw, Xf, Yf, \
                             math_selector, math_op, z1_expr, z2_expr, offset) \
	{ \
		uint8 b = (b_tl_raw); \
		uint8 op_tl = ((p_tl) != 0); \
		uint8 op_tr = ((p_tr) != 0); \
		uint8 op_bl = ((p_bl) != 0); \
		uint8 op_br = ((p_br) != 0); \
		uint8 op_mask = op_tl | (op_tr << 1) | (op_bl << 2) | (op_br << 3); \
		uint8 smooth = (Settings.Mode7HiresBilinear == 2); \
		uint16 blended; \
		(void)b; \
		if (op_mask == 0) \
		{ \
			/* All transparent: skip. Backdrop fills via DrawBackdrop. */ \
		} \
		else if (op_mask == 0xF) \
		{ \
			/* All opaque: fast path. */ \
			if (smooth) \
			{ \
				if ((p_tl) == (p_tr) && (p_tl) == (p_bl) && (p_tl) == (p_br)) \
				{ \
					blended = GFX.ScreenColors[(p_tl)]; \
				} \
				else \
				{ \
					uint16 c_tl = GFX.ScreenColors[(p_tl)]; \
					uint16 c_tr = GFX.ScreenColors[(p_tr)]; \
					uint16 c_bl = GFX.ScreenColors[(p_bl)]; \
					uint16 c_br = GFX.ScreenColors[(p_br)]; \
					M7HR_BLEND_RGB_4C(blended, c_tl, c_tr, c_bl, c_br, (Xf), (Yf)); \
				} \
			} \
			else \
			{ \
				/* Stable: X-only on floor-Y row. */ \
				if ((p_tl) == (p_tr)) \
				{ \
					blended = GFX.ScreenColors[(p_tl)]; \
				} \
				else \
				{ \
					uint16 c_tl = GFX.ScreenColors[(p_tl)]; \
					uint16 c_tr = GFX.ScreenColors[(p_tr)]; \
					M7HR_BLEND_RGB(blended, c_tl, c_tr, (Xf)); \
				} \
			} \
			if ((z1_expr) > GFX.DB[(offset) + (out_offset)]) \
			{ \
				GFX.S[(offset) + (out_offset)] = math_selector(math_op, blended, GFX.SubScreen[(offset) + (out_offset)], GFX.SubZBuffer[(offset) + (out_offset)]); \
				GFX.DB[(offset) + (out_offset)] = (z2_expr); \
			} \
		} \
		else \
		{ \
			/* Mixed transparency: alpha-aware blend. */ \
			if (smooth) \
			{ \
				/* 4-corner alpha-aware blend. Zero-index corners \
				   contribute zero weight; renormalize via division. */ \
				uint32 wx1_ = (Xf), wx0_ = 256 - wx1_; \
				uint32 wy1_ = (Yf), wy0_ = 256 - wy1_; \
				uint32 w_tl_ = op_tl ? wx0_ * wy0_ : 0; \
				uint32 w_tr_ = op_tr ? wx1_ * wy0_ : 0; \
				uint32 w_bl_ = op_bl ? wx0_ * wy1_ : 0; \
				uint32 w_br_ = op_br ? wx1_ * wy1_ : 0; \
				uint32 wsum_ = w_tl_ + w_tr_ + w_bl_ + w_br_; \
				if (wsum_ != 0) \
				{ \
					uint16 c_tl = op_tl ? GFX.ScreenColors[(p_tl)] : 0; \
					uint16 c_tr = op_tr ? GFX.ScreenColors[(p_tr)] : 0; \
					uint16 c_bl = op_bl ? GFX.ScreenColors[(p_bl)] : 0; \
					uint16 c_br = op_br ? GFX.ScreenColors[(p_br)] : 0; \
					uint32 r_ = ((c_tl >> 11) & 0x1f) * w_tl_ \
					          + ((c_tr >> 11) & 0x1f) * w_tr_ \
					          + ((c_bl >> 11) & 0x1f) * w_bl_ \
					          + ((c_br >> 11) & 0x1f) * w_br_; \
					uint32 g_ = ((c_tl >>  6) & 0x1f) * w_tl_ \
					          + ((c_tr >>  6) & 0x1f) * w_tr_ \
					          + ((c_bl >>  6) & 0x1f) * w_bl_ \
					          + ((c_br >>  6) & 0x1f) * w_br_; \
					uint32 b_ = ( c_tl        & 0x1f) * w_tl_ \
					          + ( c_tr        & 0x1f) * w_tr_ \
					          + ( c_bl        & 0x1f) * w_bl_ \
					          + ( c_br        & 0x1f) * w_br_; \
					blended = (uint16)(((r_ / wsum_) << 11) | ((g_ / wsum_) << 6) | (b_ / wsum_)); \
					if ((z1_expr) > GFX.DB[(offset) + (out_offset)]) \
					{ \
						GFX.S[(offset) + (out_offset)] = math_selector(math_op, blended, GFX.SubScreen[(offset) + (out_offset)], GFX.SubZBuffer[(offset) + (out_offset)]); \
						GFX.DB[(offset) + (out_offset)] = (z2_expr); \
					} \
				} \
			} \
			else \
			{ \
				/* Stable: alpha-aware horizontal blend on top row only. */ \
				uint32 wx1_ = (Xf), wx0_ = 256 - wx1_; \
				uint32 w_l_ = op_tl ? wx0_ : 0; \
				uint32 w_r_ = op_tr ? wx1_ : 0; \
				uint32 wsum_ = w_l_ + w_r_; \
				if (wsum_ != 0) \
				{ \
					uint16 c_l = op_tl ? GFX.ScreenColors[(p_tl)] : 0; \
					uint16 c_r = op_tr ? GFX.ScreenColors[(p_tr)] : 0; \
					uint32 r_ = ((c_l >> 11) & 0x1f) * w_l_ + ((c_r >> 11) & 0x1f) * w_r_; \
					uint32 g_ = ((c_l >>  6) & 0x1f) * w_l_ + ((c_r >>  6) & 0x1f) * w_r_; \
					uint32 b_ = ( c_l        & 0x1f) * w_l_ + ( c_r        & 0x1f) * w_r_; \
					blended = (uint16)(((r_ / wsum_) << 11) | ((g_ / wsum_) << 6) | (b_ / wsum_)); \
					if ((z1_expr) > GFX.DB[(offset) + (out_offset)]) \
					{ \
						GFX.S[(offset) + (out_offset)] = math_selector(math_op, blended, GFX.SubScreen[(offset) + (out_offset)], GFX.SubZBuffer[(offset) + (out_offset)]); \
						GFX.DB[(offset) + (out_offset)] = (z2_expr); \
					} \
				} \
			} \
		} \
	}

/* Wrapper: take a fractional sample position (X_full, Y_full in
   16.8 fixed-point), look up the four bilinear corners, and blend
   + write through M7HR_BLEND_AND_WRITE. Used by the wrap path
   (Mode7Repeat == 0); the repeat-mode paths inline LOOKUP_4 and
   LOOKUP_4_FILL separately because they need to choose between
   them per-sample.

   Parameters: see M7HR_LOOKUP_4 (vram1, mask) and
   M7HR_BLEND_AND_WRITE (math_selector, math_op, z1_expr, z2_expr,
   offset). out_offset is relative to 'offset'. */
#define M7HR_SAMPLE_BILINEAR(out_offset, X_full, Y_full, \
                             vram1, mask, \
                             math_selector, math_op, z1_expr, z2_expr, offset) \
	{ \
		int Xi = ((X_full) >> 8) & 0x3ff; \
		int Yi = ((Y_full) >> 8) & 0x3ff; \
		uint32 Xf = (uint32)((X_full) & 0xff); \
		uint32 Yf = (uint32)((Y_full) & 0xff); \
		uint8 p_tl, p_tr, p_bl, p_br; \
		uint8 b_tl_raw_; \
		M7HR_LOOKUP_4(Xi, Yi, p_tl, p_tr, p_bl, p_br, b_tl_raw_, (vram1), (mask)); \
		M7HR_BLEND_AND_WRITE(out_offset, p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xf, Yf, \
		                     math_selector, math_op, z1_expr, z2_expr, (offset)); \
	}

/* ====================================================================
 * Native and Mosaic Mode 7 (BG1, BG2) renderers
 * ====================================================================
 *
 * Four NAME1 families:
 *   DrawMode7BG1, DrawMode7BG2                 -- native Mode 7
 *   DrawMode7MosaicBG1, DrawMode7MosaicBG2     -- mosaic Mode 7
 *
 * Native: one sample per native pixel; no horizontal upsampling.
 * Mosaic: same sample model but each computed pixel is replicated
 * across an HMosaic-by-VMosaic block. Block sizes come from
 * PPU.Mosaic; per-BG enable from PPU.BGMosaic[BG_INDEX] where
 * BG_INDEX is 0 for BG1 and 1 for BG2.
 *
 * Each NAME1 family emits three NAME2 variants -- Normal1x1,
 * Normal2x1, Hires -- the three the dispatcher selects in
 * S9xSelectTileRenderers above. The level-2 dispatch in the
 * templated form emitted three more (Normal4x1, Interlace,
 * HiresInterlace) that LTO always stripped; the explicit form
 * does not emit them at all.
 *
 * BG1 vs BG2 split (same shape for native and mosaic):
 *   BG1: Z = D + 7 const, MASK = 0xff, DCMODE = $2130 bit 0.
 *   BG2: Z = D + ((b & 0x80) ? 11 : 3), MASK = 0x7f, DCMODE = 0.
 *
 * Each (NAME1, NAME2) pair gets the standard 7-fold math fan-out,
 * for a total of 4 x 3 x 7 = 84 functions and 4 x 3 = 12 dispatch
 * arrays.
 */

/* ---- Section-internal pixel plotters -------------------------------
 *
 * These mirror the Normal1x1 / Normal2x1 / Hires2x1 plotter
 * shapes from the (now-bypassed) level-2 dispatch, but take
 * MATH_SELECTOR / MATH_OP / Z_EXPR as macro parameters instead of
 * reading Z1 / Z2 / MATH from enclosing scope. The plotter is
 * passed as a parameter to TILE_BODY_NORMAL_M7 / TILE_BODY_MOSAIC_M7
 * below, which calls it once per pixel.
 *
 * Shape: (N, M, MATH_SELECTOR, MATH_OP, Z_EXPR) where N is the
 * native-column index (0..255 typically) and M is the
 * assignment-and-test expression (Pix = (b & MASK_VAL)) -- we
 * preserve the original assignment-in-test form so the compiler
 * can short-circuit the AND when Z fails. The output index is
 * derived from N according to the plotter's pixel rate. */

/* Normal1x1: writes one pixel at Offset + N. N is unparenthesized
 * inside the body to match the original DRAW_PIXEL substitution
 * semantics (the mosaic body passes a compound expression for N). */
#define M7N_PIXEL_N1x1(N, M, MATH_SELECTOR, MATH_OP, Z_EXPR) \
    if ((Z_EXPR) > GFX.DB[Offset + N] && (M)) \
    { \
        GFX.S[Offset + N] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[Pix], \
            GFX.SubScreen[Offset + N], \
            GFX.SubZBuffer[Offset + N]); \
        GFX.DB[Offset + N] = (Z_EXPR); \
    }

/* Normal2x1: writes two adjacent pixels at Offset + 2*N and
 * Offset + 2*N + 1, both with the same colour and Z value.
 * N is unparenthesized; see M7N_PIXEL_N1x1. */
#define M7N_PIXEL_N2x1(N, M, MATH_SELECTOR, MATH_OP, Z_EXPR) \
    if ((Z_EXPR) > GFX.DB[Offset + 2 * N] && (M)) \
    { \
        GFX.S[Offset + 2 * N] = GFX.S[Offset + 2 * N + 1] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[Pix], \
            GFX.SubScreen[Offset + 2 * N], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = (Z_EXPR); \
    }

/* Hires (H2x1): main-screen pixel goes through MATH normally;
 * sub-screen-side pixel uses a swapped operand order so the
 * subscreen pixel at +2N+2 acts as the "subscreen" for the
 * main pixel at +2N+1 (see PPU/CGADSUB hires-math notes in the
 * non-Mode-7 plotter). N is unparenthesized; see M7N_PIXEL_N1x1. */
#define M7N_PIXEL_H2x1(N, M, MATH_SELECTOR, MATH_OP, Z_EXPR) \
    if ((Z_EXPR) > GFX.DB[Offset + 2 * N] && (M)) \
    { \
        GFX.S[Offset + 2 * N] = MATH_SELECTOR(MATH_OP, \
            GFX.ScreenColors[Pix], \
            GFX.SubScreen[Offset + 2 * N], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.S[Offset + 2 * N + 1] = MATH_SELECTOR(MATH_OP, \
            (GFX.ClipColors ? 0 : GFX.SubScreen[Offset + 2 * N + 2]), \
            GFX.RealScreenColors[Pix], \
            GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = (Z_EXPR); \
    }

/* ---- Section-internal tile body ------------------------------------
 *
 * TILE_BODY_NORMAL_M7 is the per-line Mode 7 native body. Takes the
 * pixel plotter (M7N_PIXEL_*) as a parameter via PIXEL_PLOT and
 * forwards MATH_SELECTOR / MATH_OP / Z_EXPR / MASK_VAL / DC_EXPR.
 * The body itself is identical to the previously-templated
 * DRAW_TILE_NORMAL macro (now deleted): the structure and the
 * arithmetic are unchanged. */
#define TILE_BODY_NORMAL_M7(MATH_SELECTOR, MATH_OP, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
{ \
    struct SLineMatrixData *l; \
    uint32 x, Line, Offset; \
    uint8 *VRAM1 = Memory.VRAM + 1; \
    int aa, cc, startx; \
    GFX.RealScreenColors = IPPU.ScreenColors; \
    if (DC_EXPR) \
    { \
        if (IPPU.DirectColourMapsNeedRebuild) \
            S9xBuildDirectColourMaps(); \
        GFX.RealScreenColors = DirectColourMaps[0]; \
    } \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL; \
    l = &LineMatrixData[GFX.StartY]; \
    for ( Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++) \
    { \
        int AA, BB, CC, DD, xx, yy; \
        int32 HOffset, VOffset, CentreX, CentreY; \
        uint8 Pix, starty; \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19; \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19; \
        CentreX = ((int32) l->CentreX << 19) >> 19; \
        CentreY = ((int32) l->CentreY << 19) >> 19; \
        starty = Line + 1; \
        if (PPU.Mode7VFlip) \
            starty ^= 0xff; \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY); \
        BB = ((l->MatrixB * starty) & ~63) + ((l->MatrixB * yy) & ~63) + (CentreX << 8); \
        DD = ((l->MatrixD * starty) & ~63) + ((l->MatrixD * yy) & ~63) + (CentreY << 8); \
        if (PPU.Mode7HFlip) \
        { \
            startx = Right - 1; \
            aa = -l->MatrixA; \
            cc = -l->MatrixC; \
        } \
        else \
        { \
            startx = Left; \
            aa = l->MatrixA; \
            cc = l->MatrixC; \
        } \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX); \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63); \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63); \
        if (!PPU.Mode7Repeat) \
        { \
            for ( x = Left; x < Right; x++, AA += aa, CC += cc) \
            { \
                int X, Y; \
                uint8 *TileData, b; \
                X = ((AA + BB) >> 8) & 0x3ff; \
                Y = ((CC + DD) >> 8) & 0x3ff; \
                TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
                b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
                PIXEL_PLOT(x, Pix = (b & MASK_VAL), MATH_SELECTOR, MATH_OP, (Z_EXPR)) \
            } \
        } \
        else \
        { \
            for ( x = Left; x < Right; x++, AA += aa, CC += cc) \
            { \
                int X, Y; \
                uint8 *TileData, b; \
                X = ((AA + BB) >> 8); \
                Y = ((CC + DD) >> 8); \
                if (((X | Y) & ~0x3ff) == 0) \
                { \
                    TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
                    b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
                } \
                else \
                if (PPU.Mode7Repeat == 3) \
                    b = *(VRAM1    + ((Y & 7) << 4) + ((X & 7) << 1)); \
                else \
                    continue; \
                PIXEL_PLOT(x, Pix = (b & MASK_VAL), MATH_SELECTOR, MATH_OP, (Z_EXPR)) \
            } \
        } \
    } \
}

/* ---- Outer fan-out: emit one function per (math, NAME2) ------------
 *
 * Materializes one DrawMode7BG{N}{suffix}{NAME2_TAG} function
 * with the chosen math op and pixel plotter. */
#define DEFINE_M7_FN(suffix, MATH_SELECTOR, MATH_OP, BG_NAME, NAME2_TAG, \
                     Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
static void BG_NAME##suffix##NAME2_TAG (uint32 Left, uint32 Right, int D) \
TILE_BODY_NORMAL_M7(MATH_SELECTOR, MATH_OP, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT)

/* Per-NAME2 7-fold math fan-out, plus the dispatcher array. The
 * 4-stage interlace pattern (Normal2x1 / Hires / Interlace /
 * HiresInterlace) of the templated form is preserved here: in
 * the templated code Interlace/HiresInterlace differ from
 * Normal2x1/Hires only via BPSTART/PITCH which the Mode 7 body
 * doesn't reference, so the bodies are identical and LTO folds
 * them. We emit the same set of functions; the same LTO folding
 * applies. */
#define DEFINE_M7_NAME2(BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(_,        NOMATH,   ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(Add_,     REGMATH,  ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(AddF1_2_, MATHF1_2, ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(AddS1_2_, MATHS1_2, ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(Sub_,     REGMATH,  SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(SubF1_2_, MATHF1_2, SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
DEFINE_M7_FN(SubS1_2_, MATHS1_2, SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, PIXEL_PLOT) \
static void (*Renderers_##BG_NAME##NAME2_TAG[7]) (uint32, uint32, int) = \
{ \
    BG_NAME##_##NAME2_TAG, \
    BG_NAME##Add_##NAME2_TAG, \
    BG_NAME##AddF1_2_##NAME2_TAG, \
    BG_NAME##AddS1_2_##NAME2_TAG, \
    BG_NAME##Sub_##NAME2_TAG, \
    BG_NAME##SubF1_2_##NAME2_TAG, \
    BG_NAME##SubS1_2_##NAME2_TAG, \
};

/* Per-NAME1 NAME2 fan-out: emit only the NAME2 variants the
 * dispatcher actually selects (S9xSelectTileRenderers in this
 * file). The templated form previously emitted Normal4x1,
 * Interlace, and HiresInterlace too, but they were never
 * referenced by the dispatcher and the linker (LTO) stripped
 * them. They are no longer emitted here. */
#define DEFINE_M7_BG(BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR) \
DEFINE_M7_NAME2(BG_NAME, Normal1x1,      Z_EXPR, MASK_VAL, DC_EXPR, M7N_PIXEL_N1x1) \
DEFINE_M7_NAME2(BG_NAME, Normal2x1,      Z_EXPR, MASK_VAL, DC_EXPR, M7N_PIXEL_N2x1) \
DEFINE_M7_NAME2(BG_NAME, Hires,          Z_EXPR, MASK_VAL, DC_EXPR, M7N_PIXEL_H2x1)

/* BG1 fan-out (native). */
DEFINE_M7_BG(DrawMode7BG1, (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

/* BG2 fan-out (native). The Z_EXPR references the local 'b' (the raw
 * sampled byte) for the EXTBG per-pixel priority bit. 'b' is
 * declared inside TILE_BODY_NORMAL_M7's per-pixel scope and is
 * in scope at the PIXEL_PLOT call site. */
DEFINE_M7_BG(DrawMode7BG2, (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

/* ---- Section-internal mosaic tile body ----------------------------
 *
 * TILE_BODY_MOSAIC_M7 mirrors TILE_BODY_NORMAL_M7's structure but
 * with mosaic block replication: each computed pixel is replicated
 * across an HMosaic-by-VMosaic block. The horizontal and vertical
 * mosaic sizes come from PPU.Mosaic; per-BG enable from
 * PPU.BGMosaic[BG_INDEX]. Body identical to the previously-templated
 * DRAW_TILE_MOSAIC macro (now deleted), parameterized for explicit
 * MATH_SELECTOR/MATH_OP/Z_EXPR/MASK_VAL/DC_EXPR/BG_INDEX/PIXEL_PLOT.
 *
 * BG_INDEX is 0 for BG1 mosaic, 1 for BG2 mosaic -- selects which
 * PPU.BGMosaic[] entry to read for the per-BG enable bit. The
 * native body doesn't need this because it doesn't read BGMosaic. */
#define TILE_BODY_MOSAIC_M7(MATH_SELECTOR, MATH_OP, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
{ \
    struct SLineMatrixData *l; \
    uint32 Line, Offset; \
    int32 h, w, x, MLeft, MRight; \
    int aa, cc, startx, StartY, HMosaic, VMosaic, MosaicStart; \
    uint8 *VRAM1 = Memory.VRAM + 1; \
    GFX.RealScreenColors = IPPU.ScreenColors; \
    if (DC_EXPR) \
    { \
        if (IPPU.DirectColourMapsNeedRebuild) \
            S9xBuildDirectColourMaps(); \
        GFX.RealScreenColors = DirectColourMaps[0]; \
    } \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    StartY = GFX.StartY; \
    HMosaic = 1; \
    VMosaic = 1; \
    MosaicStart = 0; \
    MLeft = Left; \
    MRight = Right; \
    if (PPU.BGMosaic[0]) \
    { \
        VMosaic = PPU.Mosaic; \
        MosaicStart = ((uint32) GFX.StartY - PPU.MosaicStart) % VMosaic; \
        StartY -= MosaicStart; \
    } \
    if (PPU.BGMosaic[BG_INDEX]) \
    { \
        HMosaic = PPU.Mosaic; \
        MLeft  -= MLeft  % HMosaic; \
        MRight += HMosaic - 1; \
        MRight -= MRight % HMosaic; \
    } \
    Offset = StartY * GFX.PPL; \
    l = &LineMatrixData[StartY]; \
    for ( Line = StartY; Line <= GFX.EndY; Line += VMosaic, Offset += VMosaic * GFX.PPL, l += VMosaic) \
    { \
        int xx, yy, AA, BB, CC, DD; \
        int32 HOffset, VOffset, CentreX, CentreY; \
        uint8 Pix, ctr, starty; \
        if (Line + VMosaic > GFX.EndY) \
            VMosaic = GFX.EndY - Line + 1; \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19; \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19; \
        CentreX = ((int32) l->CentreX << 19) >> 19; \
        CentreY = ((int32) l->CentreY << 19) >> 19; \
        starty = Line + 1; \
        if (PPU.Mode7VFlip) \
            starty ^= 0xff; \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY); \
        BB = ((l->MatrixB * starty) & ~63) + ((l->MatrixB * yy) & ~63) + (CentreX << 8); \
        DD = ((l->MatrixD * starty) & ~63) + ((l->MatrixD * yy) & ~63) + (CentreY << 8); \
        if (PPU.Mode7HFlip) \
        { \
            startx = MRight - 1; \
            aa = -l->MatrixA; \
            cc = -l->MatrixC; \
        } \
        else \
        { \
            startx = MLeft; \
            aa = l->MatrixA; \
            cc = l->MatrixC; \
        } \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX); \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63); \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63); \
        ctr = 1; \
        if (!PPU.Mode7Repeat) \
        { \
            for ( x = MLeft; x < MRight; x++, AA += aa, CC += cc) \
            { \
                int X, Y; \
                uint8 *TileData, b; \
                if (--ctr) \
                    continue; \
                ctr = HMosaic; \
                X = ((AA + BB) >> 8) & 0x3ff; \
                Y = ((CC + DD) >> 8) & 0x3ff; \
                TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
                b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
                if ((Pix = (b & MASK_VAL))) \
                { \
                    for ( h = MosaicStart; h < VMosaic; h++) \
                    { \
                        for ( w = x + HMosaic - 1; w >= x; w--) \
                            PIXEL_PLOT(w + h * GFX.PPL, (w >= (int32) Left && w < (int32) Right), MATH_SELECTOR, MATH_OP, (Z_EXPR)) \
                    } \
                } \
            } \
        } \
        else \
        { \
            for ( x = MLeft; x < MRight; x++, AA += aa, CC += cc) \
            { \
                int X, Y; \
                uint8 b, *TileData; \
                if (--ctr) \
                    continue; \
                ctr = HMosaic; \
                X = ((AA + BB) >> 8); \
                Y = ((CC + DD) >> 8); \
                if (((X | Y) & ~0x3ff) == 0) \
                { \
                    TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
                    b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
                } \
                else \
                if (PPU.Mode7Repeat == 3) \
                    b = *(VRAM1    + ((Y & 7) << 4) + ((X & 7) << 1)); \
                else \
                    continue; \
                if ((Pix = (b & MASK_VAL))) \
                { \
                    for ( h = MosaicStart; h < VMosaic; h++) \
                    { \
                        for ( w = x + HMosaic - 1; w >= x; w--) \
                            PIXEL_PLOT(w + h * GFX.PPL, (w >= (int32) Left && w < (int32) Right), MATH_SELECTOR, MATH_OP, (Z_EXPR)) \
                    } \
                } \
            } \
        } \
        MosaicStart = 0; \
    } \
}

/* ---- Mosaic outer fan-out: emit one function per (math, NAME2) ----- */
#define DEFINE_M7_MOSAIC_FN(suffix, MATH_SELECTOR, MATH_OP, BG_NAME, NAME2_TAG, \
                            Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
static void BG_NAME##suffix##NAME2_TAG (uint32 Left, uint32 Right, int D) \
TILE_BODY_MOSAIC_M7(MATH_SELECTOR, MATH_OP, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT)

/* Mosaic per-NAME2 7-fold math fan-out, plus the dispatcher array. */
#define DEFINE_M7_MOSAIC_NAME2(BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(_,        NOMATH,   ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(Add_,     REGMATH,  ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(AddF1_2_, MATHF1_2, ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(AddS1_2_, MATHS1_2, ADD, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(Sub_,     REGMATH,  SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(SubF1_2_, MATHF1_2, SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
DEFINE_M7_MOSAIC_FN(SubS1_2_, MATHS1_2, SUB, BG_NAME, NAME2_TAG, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, PIXEL_PLOT) \
static void (*Renderers_##BG_NAME##NAME2_TAG[7]) (uint32, uint32, int) = \
{ \
    BG_NAME##_##NAME2_TAG, \
    BG_NAME##Add_##NAME2_TAG, \
    BG_NAME##AddF1_2_##NAME2_TAG, \
    BG_NAME##AddS1_2_##NAME2_TAG, \
    BG_NAME##Sub_##NAME2_TAG, \
    BG_NAME##SubF1_2_##NAME2_TAG, \
    BG_NAME##SubS1_2_##NAME2_TAG, \
};

/* Per-NAME1 NAME2 fan-out for mosaic: Normal1x1, Normal2x1, Hires
 * (same set as native -- the dispatcher uses these three). */
#define DEFINE_M7_MOSAIC_BG(BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX) \
DEFINE_M7_MOSAIC_NAME2(BG_NAME, Normal1x1, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, M7N_PIXEL_N1x1) \
DEFINE_M7_MOSAIC_NAME2(BG_NAME, Normal2x1, Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, M7N_PIXEL_N2x1) \
DEFINE_M7_MOSAIC_NAME2(BG_NAME, Hires,     Z_EXPR, MASK_VAL, DC_EXPR, BG_INDEX, M7N_PIXEL_H2x1)

/* MosaicBG1 fan-out. BG_INDEX = 0. */
DEFINE_M7_MOSAIC_BG(DrawMode7MosaicBG1, (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1), 0)

/* MosaicBG2 fan-out. BG_INDEX = 1. Z_EXPR references the local 'b'
 * (raw byte) for the EXTBG per-pixel priority bit. */
DEFINE_M7_MOSAIC_BG(DrawMode7MosaicBG2, (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0, 1)

#undef DEFINE_M7_MOSAIC_BG
#undef DEFINE_M7_MOSAIC_NAME2
#undef DEFINE_M7_MOSAIC_FN
#undef TILE_BODY_MOSAIC_M7

#undef DEFINE_M7_BG
#undef DEFINE_M7_NAME2
#undef DEFINE_M7_FN
#undef TILE_BODY_NORMAL_M7
#undef M7N_PIXEL_H2x1
#undef M7N_PIXEL_N2x1
#undef M7N_PIXEL_N1x1

/* End of native + mosaic Mode 7 de-templated section.
 * ==================================================================== */

/* ====================================================================
 * Mode 7 2x hires (HR / HR2X) renderers
 * ====================================================================
 *
 * Two NAME1 families: DrawMode7BG1HR (BG1) and DrawMode7BG2HR
 * (BG2 EXTBG). The 2x nearest-neighbour Mode 7 path: two samples
 * per native pixel using half-step (aa_h = aa/2) sub-pixel
 * division, each written through the standard Z-test + MATH
 * pixel write (no bilinear blending; nearest-neighbour from the
 * floor texel at each sub-sample position).
 *
 * BG1 vs BG2 split: same as the other M7 hires families.
 *   BG1: Z = D + 7 const, MASK = 0xff, DCMODE = $2130 bit 0.
 *   BG2: Z = D + ((b & 0x80) ? 11 : 3), MASK = 0x7f, DCMODE = 0.
 *
 * Mosaic is intentionally not implemented for hires -- mosaic
 * falls back to the native renderer when the option is on.
 * Mosaic by definition produces blocks; sub-pixel refinement is
 * meaningless there.
 *
 * Each family has 7 functions, one per math op. The 7-fold fan-out
 * is materialised by DEFINE_M7_HR_FAMILY below.
 */

/* Local fan-out for the 14 HR functions (2 BGs x 7 math ops).
 *
 * Parameters (same shape as DEFINE_M7_BL_FAMILY, but the body
 * uses inline nearest-neighbour DRAW_PIXEL writes instead of
 * M7HR_SAMPLE_BILINEAR / M7HR_BLEND_AND_WRITE):
 *   suffix         token suffix appended to the function name.
 *   MATH_SELECTOR  one of NOMATH, REGMATH, MATHF1_2, MATHS1_2.
 *   MATH_OP        ADD or SUB.
 *   BG_NAME        DrawMode7BG1HR or DrawMode7BG2HR.
 *   Z_EXPR         Z1/Z2 expression. BG2 references the local
 *                  'b' (the raw sampled byte before MASK is
 *                  applied) for the per-pixel priority bit.
 *   MASK_VAL       palette mask: 0xff for BG1, 0x7f for BG2.
 *   DC_EXPR        Direct Colour Mode enable expression.
 *
 * The per-pixel write inlines the Normal1x1 plotter body, matching
 * the original DRAW_PIXEL(N, M) shape (assignment-and-test in a
 * single expression so Pix is only assigned when the Z-test
 * passes):
 *   if Z_EXPR > GFX.DB[Offset + idx] && (Pix = (b & MASK_VAL)):
 *       GFX.S[Offset + idx] = MATH_SELECTOR(MATH_OP,
 *           GFX.ScreenColors[Pix],
 *           GFX.SubScreen[Offset + idx],
 *           GFX.SubZBuffer[Offset + idx])
 *       GFX.DB[Offset + idx] = Z_EXPR
 * BG2's Z_EXPR references 'b' so the EXTBG priority bit
 * propagates correctly. */
#define DEFINE_M7_HR_FAMILY(suffix, MATH_SELECTOR, MATH_OP,                    \
                            BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR)                \
static void BG_NAME##suffix##Normal1x1 (uint32 Left, uint32 Right, int D)      \
{                                                                              \
    struct SLineMatrixData *l;                                                 \
    uint32 x, Line, Offset;                                                    \
    uint8 *VRAM1 = Memory.VRAM + 1;                                            \
    int aa, cc, aa_h, cc_h, startx;                                            \
                                                                               \
    GFX.RealScreenColors = IPPU.ScreenColors;                                  \
    if (DC_EXPR)                                                               \
    {                                                                          \
        if (IPPU.DirectColourMapsNeedRebuild)                                  \
            S9xBuildDirectColourMaps();                                        \
        GFX.RealScreenColors = DirectColourMaps[0];                            \
    }                                                                          \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL;                                             \
    l = &LineMatrixData[GFX.StartY];                                           \
                                                                               \
    for (Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++)  \
    {                                                                          \
        int AA, BB, CC, DD, xx, yy;                                            \
        int32 HOffset, VOffset, CentreX, CentreY;                              \
        uint8 Pix, starty;                                                     \
                                                                               \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19;                            \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19;                            \
        CentreX = ((int32) l->CentreX << 19) >> 19;                            \
        CentreY = ((int32) l->CentreY << 19) >> 19;                            \
                                                                               \
        starty = Line + 1;                                                     \
        if (PPU.Mode7VFlip)                                                    \
            starty ^= 0xff;                                                    \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY);                            \
        BB = ((l->MatrixB * starty) & ~63)                                     \
           + ((l->MatrixB * yy)     & ~63) + (CentreX << 8);                   \
        DD = ((l->MatrixD * starty) & ~63)                                     \
           + ((l->MatrixD * yy)     & ~63) + (CentreY << 8);                   \
                                                                               \
        if (PPU.Mode7HFlip)                                                    \
        {                                                                      \
            startx = Right - 1;                                                \
            aa = -l->MatrixA;                                                  \
            cc = -l->MatrixC;                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            startx = Left;                                                     \
            aa = l->MatrixA;                                                   \
            cc = l->MatrixC;                                                   \
        }                                                                      \
        aa_h = aa / 2;                                                         \
        cc_h = cc / 2;                                                         \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX);                            \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63);                  \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63);                  \
                                                                               \
        if (!PPU.Mode7Repeat)                                                  \
        {                                                                      \
            /* Wrap path: X & Y mask to 10 bits, no clipping. */               \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int X, Y;                                                      \
                uint8 *TileData, b;                                            \
                                                                               \
                /* Sample 1 -> output index 2*x */                             \
                X = ((AA + BB) >> 8) & 0x3ff;                                  \
                Y = ((CC + DD) >> 8) & 0x3ff;                                  \
                TileData = VRAM1 +                                             \
                    (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7);     \
                b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));             \
                if ((Z_EXPR) > GFX.DB[Offset + 2 * x]                          \
                    && (Pix = (b & MASK_VAL)))                                 \
                {                                                              \
                    GFX.S[Offset + 2 * x] = MATH_SELECTOR(                     \
                        MATH_OP,                                               \
                        GFX.ScreenColors[Pix],                                 \
                        GFX.SubScreen[Offset + 2 * x],                         \
                        GFX.SubZBuffer[Offset + 2 * x]);                       \
                    GFX.DB[Offset + 2 * x] = (Z_EXPR);                         \
                }                                                              \
                AA += aa_h; CC += cc_h;                                        \
                                                                               \
                /* Sample 2 -> output index 2*x + 1 */                         \
                X = ((AA + BB) >> 8) & 0x3ff;                                  \
                Y = ((CC + DD) >> 8) & 0x3ff;                                  \
                TileData = VRAM1 +                                             \
                    (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7);     \
                b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));             \
                if ((Z_EXPR) > GFX.DB[Offset + 2 * x + 1]                      \
                    && (Pix = (b & MASK_VAL)))                                 \
                {                                                              \
                    GFX.S[Offset + 2 * x + 1] = MATH_SELECTOR(                 \
                        MATH_OP,                                               \
                        GFX.ScreenColors[Pix],                                 \
                        GFX.SubScreen[Offset + 2 * x + 1],                     \
                        GFX.SubZBuffer[Offset + 2 * x + 1]);                   \
                    GFX.DB[Offset + 2 * x + 1] = (Z_EXPR);                     \
                }                                                              \
                AA += aa - aa_h; CC += cc - cc_h;                              \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            /* Repeat-mode path: clip out-of-range samples (modes 1/2)         \
             * or fill with tile 0 (mode 3). */                                \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int X, Y;                                                      \
                uint8 *TileData, b;                                            \
                int do_draw;                                                   \
                                                                               \
                /* Sample 1 -> output index 2*x */                             \
                X = ((AA + BB) >> 8);                                          \
                Y = ((CC + DD) >> 8);                                          \
                do_draw = 1;                                                   \
                if (((X | Y) & ~0x3ff) == 0)                                   \
                {                                                              \
                    TileData = VRAM1 +                                         \
                        (Memory.VRAM[((Y & ~7) << 5)                           \
                                   + ((X >> 2) & ~1)] << 7);                   \
                    b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));         \
                }                                                              \
                else if (PPU.Mode7Repeat == 3)                                 \
                    b = *(VRAM1 + ((Y & 7) << 4) + ((X & 7) << 1));            \
                else                                                           \
                    do_draw = 0;                                               \
                if (do_draw)                                                   \
                {                                                              \
                    if ((Z_EXPR) > GFX.DB[Offset + 2 * x]                      \
                        && (Pix = (b & MASK_VAL)))                             \
                    {                                                          \
                        GFX.S[Offset + 2 * x] = MATH_SELECTOR(                 \
                            MATH_OP,                                           \
                            GFX.ScreenColors[Pix],                             \
                            GFX.SubScreen[Offset + 2 * x],                     \
                            GFX.SubZBuffer[Offset + 2 * x]);                   \
                        GFX.DB[Offset + 2 * x] = (Z_EXPR);                     \
                    }                                                          \
                }                                                              \
                AA += aa_h; CC += cc_h;                                        \
                                                                               \
                /* Sample 2 -> output index 2*x + 1 */                         \
                X = ((AA + BB) >> 8);                                          \
                Y = ((CC + DD) >> 8);                                          \
                do_draw = 1;                                                   \
                if (((X | Y) & ~0x3ff) == 0)                                   \
                {                                                              \
                    TileData = VRAM1 +                                         \
                        (Memory.VRAM[((Y & ~7) << 5)                           \
                                   + ((X >> 2) & ~1)] << 7);                   \
                    b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));         \
                }                                                              \
                else if (PPU.Mode7Repeat == 3)                                 \
                    b = *(VRAM1 + ((Y & 7) << 4) + ((X & 7) << 1));            \
                else                                                           \
                    do_draw = 0;                                               \
                if (do_draw)                                                   \
                {                                                              \
                    if ((Z_EXPR) > GFX.DB[Offset + 2 * x + 1]                  \
                        && (Pix = (b & MASK_VAL)))                             \
                    {                                                          \
                        GFX.S[Offset + 2 * x + 1] = MATH_SELECTOR(             \
                            MATH_OP,                                           \
                            GFX.ScreenColors[Pix],                             \
                            GFX.SubScreen[Offset + 2 * x + 1],                 \
                            GFX.SubZBuffer[Offset + 2 * x + 1]);               \
                        GFX.DB[Offset + 2 * x + 1] = (Z_EXPR);                 \
                    }                                                          \
                }                                                              \
                AA += aa - aa_h; CC += cc - cc_h;                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}

/* BG1 fan-out: 7 math variants. Z = D + 7 const. MASK = 0xff.
 * DCMODE follows $2130 bit 0 (Direct Colour Mode). */
DEFINE_M7_HR_FAMILY(_,        NOMATH,   ADD, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG1HR,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

static void (*Renderers_DrawMode7BG1HRNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG1HR_Normal1x1,
    DrawMode7BG1HRAdd_Normal1x1,
    DrawMode7BG1HRAddF1_2_Normal1x1,
    DrawMode7BG1HRAddS1_2_Normal1x1,
    DrawMode7BG1HRSub_Normal1x1,
    DrawMode7BG1HRSubF1_2_Normal1x1,
    DrawMode7BG1HRSubS1_2_Normal1x1,
};

/* BG2 fan-out: see HR4X / BL4X for the EXTBG priority bit
 * explanation. */
DEFINE_M7_HR_FAMILY(_,        NOMATH,   ADD, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG2HR,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

static void (*Renderers_DrawMode7BG2HRNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG2HR_Normal1x1,
    DrawMode7BG2HRAdd_Normal1x1,
    DrawMode7BG2HRAddF1_2_Normal1x1,
    DrawMode7BG2HRAddS1_2_Normal1x1,
    DrawMode7BG2HRSub_Normal1x1,
    DrawMode7BG2HRSubF1_2_Normal1x1,
    DrawMode7BG2HRSubS1_2_Normal1x1,
};

#undef DEFINE_M7_HR_FAMILY

/* End of HR (2x nearest-neighbour) de-templated section.
 * ==================================================================== */

/* ====================================================================
 * Mode 7 4x hires (HR4X) renderers
 * ====================================================================
 *
 * Two NAME1 families: DrawMode7BG1HR4X (BG1) and DrawMode7BG2HR4X
 * (BG2 EXTBG). Each renderer samples the SNES Mode 7 affine matrix
 * four times per native pixel, advancing AA, CC by aa_q = aa/4 and
 * cc_q = cc/4 between sub-samples. Output goes to four contiguous
 * buffer pixels at GFX.S[Offset + 4*x .. + 4*x + 3]. The buffer
 * has been promoted to 4x SNES width before this renderer runs;
 * see cpuexec.c (frame-start promotion) and ppu.c::S9xUpdateScreen
 * (mid-frame promotion).
 *
 * BG1 vs BG2: differ in MASK (0xff vs 0x7f), Z1/Z2 (constant +7 vs
 * priority-bit-dependent), and DCMODE (Direct Colour from $2130
 * bit 0 vs always off). Same renderer body otherwise.
 *
 * Each family has 7 functions, one per color-math op:
 *   _              NoMath        (no color math, copy through)
 *   Add_           Add subscreen / fixed colour
 *   AddF1_2_       Add fixed colour, half intensity
 *   AddS1_2_       Add subscreen if available else fixed, half
 *   Sub_           Subtract subscreen / fixed colour
 *   SubF1_2_       Subtract fixed colour, half intensity
 *   SubS1_2_       Subtract subscreen if available else fixed, half
 *
 * The 7-fold fan-out is materialised by the local
 * DEFINE_M7_HR4X_FAMILY macro below. Each instantiation produces
 * one specialised function with the math op resolved at compile
 * time, so the inner pixel-write loop never branches on math state.
 *
 * Compatibility note: this section is hand-written. The earlier
 * templated HR2X / BL / BL4X / BL1X paths still go through the
 * 3-level #include "tile.c" scaffold (see docs/tile-internals.md
 * for the templating model). HR4X is the first family converted
 * as part of the tile-untangle work; the remaining families will
 * follow incrementally.
 */

/* Local fan-out for the 14 HR4X functions (2 BGs x 7 math ops).
 *
 * Parameters:
 *   suffix         token suffix appended to the function name
 *                  (e.g. "AddF1_2_" -> ...HR4XAddF1_2_Normal1x1)
 *   MATH_SELECTOR  one of NOMATH, REGMATH, MATHF1_2, MATHS1_2.
 *                  These are the file-level macros from the preamble
 *                  that select the colour-math arithmetic shape.
 *   MATH_OP        token concatenated by COLOR_##Op when the
 *                  selector resolves: ADD or SUB. NOMATH ignores it.
 *   BG_NAME        DrawMode7BG1HR4X or DrawMode7BG2HR4X.
 *   Z_EXPR         expression for Z1/Z2 (BG1: D+7 constant.
 *                  BG2: D + ((b & 0x80) ? 11 : 3) per-pixel).
 *   MASK_VAL       palette mask: 0xff for BG1, 0x7f for BG2.
 *   DC_EXPR        Direct Colour Mode enable expression.
 *
 * Body structure:
 *   - Per-line setup: unpack the matrix from LineMatrixData, apply
 *     HFlip, compute aa/cc and aa_q/cc_q quarter-step, set up AA/BB
 *     /CC/DD initial values.
 *   - For each native pixel x in [Left, Right): take 4 sub-samples,
 *     each looking up the texel from VRAM via the Mode 7 affine
 *     transform, then writing to buffer index 4*x + sub.
 *   - Quarter-step advance with the rounding leftover deposited on
 *     the final sub-sample so the per-native-pixel advance sums to
 *     exactly aa/cc (no drift).
 *   - Two paths inside the pixel loop: PPU.Mode7Repeat == 0 (wrap,
 *     no clipping) and PPU.Mode7Repeat != 0 (clip out-of-range, or
 *     fill with tile 0 in mode 3).
 */
#define DEFINE_M7_HR4X_FAMILY(suffix, MATH_SELECTOR, MATH_OP,                  \
                              BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR)              \
static void BG_NAME##suffix##Normal1x1 (uint32 Left, uint32 Right, int D)      \
{                                                                              \
    struct SLineMatrixData *l;                                                 \
    uint32 x, Line, Offset;                                                    \
    uint8 *VRAM1 = Memory.VRAM + 1;                                            \
    int aa, cc, aa_q, cc_q, startx;                                            \
                                                                               \
    GFX.RealScreenColors = IPPU.ScreenColors;                                  \
    if (DC_EXPR)                                                               \
    {                                                                          \
        if (IPPU.DirectColourMapsNeedRebuild)                                  \
            S9xBuildDirectColourMaps();                                        \
        GFX.RealScreenColors = DirectColourMaps[0];                            \
    }                                                                          \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL;                                             \
    l = &LineMatrixData[GFX.StartY];                                           \
                                                                               \
    for (Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++)  \
    {                                                                          \
        int AA, BB, CC, DD, xx, yy;                                            \
        int32 HOffset, VOffset, CentreX, CentreY;                              \
        uint8 Pix, starty;                                                     \
                                                                               \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19;                            \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19;                            \
        CentreX = ((int32) l->CentreX << 19) >> 19;                            \
        CentreY = ((int32) l->CentreY << 19) >> 19;                            \
                                                                               \
        starty = Line + 1;                                                     \
        if (PPU.Mode7VFlip)                                                    \
            starty ^= 0xff;                                                    \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY);                            \
        BB = ((l->MatrixB * starty) & ~63)                                     \
           + ((l->MatrixB * yy)     & ~63) + (CentreX << 8);                   \
        DD = ((l->MatrixD * starty) & ~63)                                     \
           + ((l->MatrixD * yy)     & ~63) + (CentreY << 8);                   \
                                                                               \
        if (PPU.Mode7HFlip)                                                    \
        {                                                                      \
            startx = Right - 1;                                                \
            aa = -l->MatrixA;                                                  \
            cc = -l->MatrixC;                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            startx = Left;                                                     \
            aa = l->MatrixA;                                                   \
            cc = l->MatrixC;                                                   \
        }                                                                      \
        aa_q = aa / 4;                                                         \
        cc_q = cc / 4;                                                         \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX);                            \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63);                  \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63);                  \
                                                                               \
        if (!PPU.Mode7Repeat)                                                  \
        {                                                                      \
            /* Wrap path: X & Y mask to 10 bits, no clipping. */               \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int X, Y, sub;                                                 \
                uint8 *TileData, b;                                            \
                for (sub = 0; sub < 4; sub++)                                  \
                {                                                              \
                    X = ((AA + BB) >> 8) & 0x3ff;                              \
                    Y = ((CC + DD) >> 8) & 0x3ff;                              \
                    TileData = VRAM1 +                                         \
                        (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
                    b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));         \
                    Pix = (b & MASK_VAL);                                      \
                    /* Z-test + write. Z_EXPR may reference 'b' (BG2). */      \
                    {                                                          \
                        uint32 idx = 4 * x + sub;                              \
                        if ((Z_EXPR) > GFX.DB[Offset + idx] && Pix)            \
                        {                                                      \
                            GFX.S[Offset + idx] = MATH_SELECTOR(               \
                                MATH_OP,                                       \
                                GFX.ScreenColors[Pix],                         \
                                GFX.SubScreen[Offset + idx],                   \
                                GFX.SubZBuffer[Offset + idx]);                 \
                            GFX.DB[Offset + idx] = (Z_EXPR);                   \
                        }                                                      \
                    }                                                          \
                    /* Quarter-step. Final sub-sample absorbs the              \
                     * rounding leftover so the total advance equals           \
                     * aa/cc per native pixel. */                              \
                    if (sub < 3)                                               \
                    {                                                          \
                        AA += aa_q;                                            \
                        CC += cc_q;                                            \
                    }                                                          \
                    else                                                       \
                    {                                                          \
                        AA += aa - 3 * aa_q;                                   \
                        CC += cc - 3 * cc_q;                                   \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            /* Repeat-mode path: clip out-of-range samples (modes 1/2)         \
             * or fill with tile 0 (mode 3). */                                \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int X, Y, sub, do_draw;                                        \
                uint8 *TileData, b;                                            \
                for (sub = 0; sub < 4; sub++)                                  \
                {                                                              \
                    X = ((AA + BB) >> 8);                                      \
                    Y = ((CC + DD) >> 8);                                      \
                    do_draw = 1;                                               \
                    if (((X | Y) & ~0x3ff) == 0)                               \
                    {                                                          \
                        TileData = VRAM1 +                                     \
                            (Memory.VRAM[((Y & ~7) << 5)                       \
                                       + ((X >> 2) & ~1)] << 7);               \
                        b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1));     \
                    }                                                          \
                    else if (PPU.Mode7Repeat == 3)                             \
                        b = *(VRAM1 + ((Y & 7) << 4) + ((X & 7) << 1));        \
                    else                                                       \
                        do_draw = 0;                                           \
                    if (do_draw)                                               \
                    {                                                          \
                        uint32 idx = 4 * x + sub;                              \
                        Pix = (b & MASK_VAL);                                  \
                        if ((Z_EXPR) > GFX.DB[Offset + idx] && Pix)            \
                        {                                                      \
                            GFX.S[Offset + idx] = MATH_SELECTOR(               \
                                MATH_OP,                                       \
                                GFX.ScreenColors[Pix],                         \
                                GFX.SubScreen[Offset + idx],                   \
                                GFX.SubZBuffer[Offset + idx]);                 \
                            GFX.DB[Offset + idx] = (Z_EXPR);                   \
                        }                                                      \
                    }                                                          \
                    if (sub < 3)                                               \
                    {                                                          \
                        AA += aa_q;                                            \
                        CC += cc_q;                                            \
                    }                                                          \
                    else                                                       \
                    {                                                          \
                        AA += aa - 3 * aa_q;                                   \
                        CC += cc - 3 * cc_q;                                   \
                    }                                                          \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}

/* BG1 fan-out: 7 math variants. Z = D + 7 constant. MASK = 0xff.
 * DCMODE follows $2130 bit 0 (Direct Colour Mode). */
DEFINE_M7_HR4X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_HR4X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG1HR4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

static void (*Renderers_DrawMode7BG1HR4XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG1HR4X_Normal1x1,
    DrawMode7BG1HR4XAdd_Normal1x1,
    DrawMode7BG1HR4XAddF1_2_Normal1x1,
    DrawMode7BG1HR4XAddS1_2_Normal1x1,
    DrawMode7BG1HR4XSub_Normal1x1,
    DrawMode7BG1HR4XSubF1_2_Normal1x1,
    DrawMode7BG1HR4XSubS1_2_Normal1x1,
};

/* BG2 fan-out: 7 math variants. Z = D + ((b & 0x80) ? 11 : 3) -- the
 * EXTBG per-pixel priority bit lives in bit 0x80 of the raw sampled
 * byte, which selects high (11) vs low (3) priority tier. The macro
 * body exposes 'b' as a local so Z_EXPR can reach it. MASK = 0x7f
 * (the priority bit is excluded from the palette index). DCMODE is
 * always off for BG2 EXTBG. */
DEFINE_M7_HR4X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_HR4X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG2HR4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

static void (*Renderers_DrawMode7BG2HR4XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG2HR4X_Normal1x1,
    DrawMode7BG2HR4XAdd_Normal1x1,
    DrawMode7BG2HR4XAddF1_2_Normal1x1,
    DrawMode7BG2HR4XAddS1_2_Normal1x1,
    DrawMode7BG2HR4XSub_Normal1x1,
    DrawMode7BG2HR4XSubF1_2_Normal1x1,
    DrawMode7BG2HR4XSubS1_2_Normal1x1,
};

#undef DEFINE_M7_HR4X_FAMILY

/* End of HR4X de-templated section.
 * ==================================================================== */

/* ====================================================================
 * Mode 7 2x hires bilinear (BL / BL2X) renderers
 * ====================================================================
 *
 * Two NAME1 families: DrawMode7BG1BL (BG1) and DrawMode7BG2BL
 * (BG2 EXTBG). The 2x bilinear path: same 2x output rate as the
 * nearest-neighbour HR2X (still templated for now, will be
 * de-templated in Stage 2.5), but each output sample goes through
 * bilinear blending via M7HR_SAMPLE_BILINEAR / M7HR_BLEND_AND_WRITE
 * instead of nearest-neighbour DRAW_PIXEL.
 *
 * Two samples per native pixel x: writes to GFX.S[Offset + 2*x]
 * and GFX.S[Offset + 2*x + 1]. The matrix advances by aa_h = aa/2
 * between sub-samples and by (aa - aa_h) on the final sub-sample
 * so the per-native-pixel total advance equals exactly aa (no
 * drift).
 *
 * Bilinear filter mode (stable / smooth) is selected by
 * Settings.Mode7HiresBilinear inside M7HR_BLEND_AND_WRITE; the
 * branch is frame-constant.
 *
 * BG1 vs BG2 split: same as the other M7 hires families.
 *   BG1: Z = D + 7 const, MASK = 0xff, DCMODE = $2130 bit 0.
 *   BG2: Z = D + ((b & 0x80) ? 11 : 3), MASK = 0x7f, DCMODE = 0.
 *
 *   (BG2 honors the EXTBG per-pixel priority bit. An earlier
 *   draft of the BL path pinned BG2 to the low tier under the
 *   theory that the bit was no longer well-defined under
 *   bilinear blending, but that was both unnecessary and
 *   inconsistent with the other M7 paths -- the priority bit
 *   comes from the TL corner's raw byte, which is well-defined.)
 *
 * Each family has 7 functions, one per math op. The 7-fold fan-out
 * is materialised by DEFINE_M7_BL_FAMILY below.
 */

/* Local fan-out for the 14 BL functions (2 BGs x 7 math ops).
 *
 * Parameters (same shape as DEFINE_M7_BL4X_FAMILY):
 *   suffix         token suffix appended to the function name.
 *   MATH_SELECTOR  one of NOMATH, REGMATH, MATHF1_2, MATHS1_2.
 *   MATH_OP        ADD or SUB.
 *   BG_NAME        DrawMode7BG1BL or DrawMode7BG2BL.
 *   Z_EXPR         Z1/Z2 expression.
 *   MASK_VAL       palette mask: 0xff for BG1, 0x7f for BG2.
 *   DC_EXPR        Direct Colour Mode enable expression.
 *
 * Body structure: per-line matrix setup (identical to BL4X / BL1X
 * / HR4X), then a per-pixel loop that takes 2 samples per native
 * pixel using half-step (aa_h = aa/2) advance. The wrap path
 * uses M7HR_SAMPLE_BILINEAR; the repeat path inlines the
 * LOOKUP_4 / LOOKUP_4_FILL split for per-sample range checking. */
#define DEFINE_M7_BL_FAMILY(suffix, MATH_SELECTOR, MATH_OP,                    \
                            BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR)                \
static void BG_NAME##suffix##Normal1x1 (uint32 Left, uint32 Right, int D)      \
{                                                                              \
    struct SLineMatrixData *l;                                                 \
    uint32 x, Line, Offset;                                                    \
    uint8 *VRAM1 = Memory.VRAM + 1;                                            \
    int aa, cc, aa_h, cc_h, startx;                                            \
                                                                               \
    GFX.RealScreenColors = IPPU.ScreenColors;                                  \
    if (DC_EXPR)                                                               \
    {                                                                          \
        if (IPPU.DirectColourMapsNeedRebuild)                                  \
            S9xBuildDirectColourMaps();                                        \
        GFX.RealScreenColors = DirectColourMaps[0];                            \
    }                                                                          \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL;                                             \
    l = &LineMatrixData[GFX.StartY];                                           \
                                                                               \
    for (Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++)  \
    {                                                                          \
        int AA, BB, CC, DD, xx, yy;                                            \
        int32 HOffset, VOffset, CentreX, CentreY;                              \
        uint8 starty;                                                          \
                                                                               \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19;                            \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19;                            \
        CentreX = ((int32) l->CentreX << 19) >> 19;                            \
        CentreY = ((int32) l->CentreY << 19) >> 19;                            \
                                                                               \
        starty = Line + 1;                                                     \
        if (PPU.Mode7VFlip)                                                    \
            starty ^= 0xff;                                                    \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY);                            \
        BB = ((l->MatrixB * starty) & ~63)                                     \
           + ((l->MatrixB * yy)     & ~63) + (CentreX << 8);                   \
        DD = ((l->MatrixD * starty) & ~63)                                     \
           + ((l->MatrixD * yy)     & ~63) + (CentreY << 8);                   \
                                                                               \
        if (PPU.Mode7HFlip)                                                    \
        {                                                                      \
            startx = Right - 1;                                                \
            aa = -l->MatrixA;                                                  \
            cc = -l->MatrixC;                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            startx = Left;                                                     \
            aa = l->MatrixA;                                                   \
            cc = l->MatrixC;                                                   \
        }                                                                      \
        aa_h = aa / 2;                                                         \
        cc_h = cc / 2;                                                         \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX);                            \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63);                  \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63);                  \
                                                                               \
        if (!PPU.Mode7Repeat)                                                  \
        {                                                                      \
            /* Wrap path. */                                                   \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                M7HR_SAMPLE_BILINEAR(2 * x,     AA + BB, CC + DD,              \
                    VRAM1, MASK_VAL,                                           \
                    MATH_SELECTOR, MATH_OP,                                    \
                    (Z_EXPR), (Z_EXPR), Offset);                               \
                AA += aa_h; CC += cc_h;                                        \
                M7HR_SAMPLE_BILINEAR(2 * x + 1, AA + BB, CC + DD,              \
                    VRAM1, MASK_VAL,                                           \
                    MATH_SELECTOR, MATH_OP,                                    \
                    (Z_EXPR), (Z_EXPR), Offset);                               \
                AA += aa - aa_h; CC += cc - cc_h;                              \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            /* Repeat-mode path: clip out-of-range samples (modes 1/2)         \
             * or fill with tile 0 (mode 3). */                                \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int X1 = (AA + BB) >> 8;                                       \
                int Y1 = (CC + DD) >> 8;                                       \
                int X2, Y2;                                                    \
                uint32 Xf1 = (uint32)((AA + BB) & 0xff);                       \
                uint32 Yf1 = (uint32)((CC + DD) & 0xff);                       \
                uint32 Xf2, Yf2;                                               \
                uint8 p_tl, p_tr, p_bl, p_br;                                  \
                uint8 b_tl_raw_;                                               \
                /* Sample 1 -> output 2*x */                                   \
                if (((X1 | Y1) & ~0x3ff) == 0)                                 \
                {                                                              \
                    M7HR_LOOKUP_4(X1, Y1, p_tl, p_tr, p_bl, p_br,              \
                                  b_tl_raw_, VRAM1, MASK_VAL);                 \
                    M7HR_BLEND_AND_WRITE(2 * x,                                \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xf1, Yf1,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
                else if (PPU.Mode7Repeat == 3)                                 \
                {                                                              \
                    M7HR_LOOKUP_4_FILL(X1, Y1, p_tl, p_tr, p_bl, p_br,         \
                                       b_tl_raw_, VRAM1, MASK_VAL);            \
                    M7HR_BLEND_AND_WRITE(2 * x,                                \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xf1, Yf1,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
                /* else: clip - leave pixel untouched (transparent) */         \
                AA += aa_h; CC += cc_h;                                        \
                /* Sample 2 -> output 2*x + 1 */                               \
                X2  = (AA + BB) >> 8;                                          \
                Y2  = (CC + DD) >> 8;                                          \
                Xf2 = (uint32)((AA + BB) & 0xff);                              \
                Yf2 = (uint32)((CC + DD) & 0xff);                              \
                if (((X2 | Y2) & ~0x3ff) == 0)                                 \
                {                                                              \
                    M7HR_LOOKUP_4(X2, Y2, p_tl, p_tr, p_bl, p_br,              \
                                  b_tl_raw_, VRAM1, MASK_VAL);                 \
                    M7HR_BLEND_AND_WRITE(2 * x + 1,                            \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xf2, Yf2,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
                else if (PPU.Mode7Repeat == 3)                                 \
                {                                                              \
                    M7HR_LOOKUP_4_FILL(X2, Y2, p_tl, p_tr, p_bl, p_br,         \
                                       b_tl_raw_, VRAM1, MASK_VAL);            \
                    M7HR_BLEND_AND_WRITE(2 * x + 1,                            \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xf2, Yf2,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
                AA += aa - aa_h; CC += cc - cc_h;                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}

/* BG1 fan-out: 7 math variants. Z = D + 7 const. MASK = 0xff.
 * DCMODE follows $2130 bit 0 (Direct Colour Mode). */
DEFINE_M7_BL_FAMILY(_,        NOMATH,   ADD, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG1BL,
                    (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

static void (*Renderers_DrawMode7BG1BLNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG1BL_Normal1x1,
    DrawMode7BG1BLAdd_Normal1x1,
    DrawMode7BG1BLAddF1_2_Normal1x1,
    DrawMode7BG1BLAddS1_2_Normal1x1,
    DrawMode7BG1BLSub_Normal1x1,
    DrawMode7BG1BLSubF1_2_Normal1x1,
    DrawMode7BG1BLSubS1_2_Normal1x1,
};

/* BG2 fan-out: see HR4X / BL4X for the EXTBG priority bit
 * explanation. */
DEFINE_M7_BL_FAMILY(_,        NOMATH,   ADD, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG2BL,
                    (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

static void (*Renderers_DrawMode7BG2BLNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG2BL_Normal1x1,
    DrawMode7BG2BLAdd_Normal1x1,
    DrawMode7BG2BLAddF1_2_Normal1x1,
    DrawMode7BG2BLAddS1_2_Normal1x1,
    DrawMode7BG2BLSub_Normal1x1,
    DrawMode7BG2BLSubF1_2_Normal1x1,
    DrawMode7BG2BLSubS1_2_Normal1x1,
};

#undef DEFINE_M7_BL_FAMILY

/* End of BL (2x bilinear) de-templated section.
 * ==================================================================== */

/* ====================================================================
 * Mode 7 4x hires bilinear (BL4X) renderers
 * ====================================================================
 *
 * Two NAME1 families: DrawMode7BG1BL4X (BG1) and DrawMode7BG2BL4X
 * (BG2 EXTBG). Same 4x sub-sampling structure as HR4X (see the
 * HR4X section earlier in this file), but each sub-sample goes
 * through bilinear filtering via M7HR_SAMPLE_BILINEAR /
 * M7HR_BLEND_AND_WRITE instead of nearest-neighbour DRAW_PIXEL.
 *
 * The bilinear filter has two modes selected by
 * Settings.Mode7HiresBilinear (1 = stable, 2 = smooth). The mode
 * is read inside M7HR_BLEND_AND_WRITE per-sample but the branch is
 * frame-constant; the predictor handles it cleanly.
 *
 * BG1 vs BG2 split: same as HR4X.
 *   BG1: Z = D + 7 const, MASK = 0xff, DCMODE = $2130 bit 0.
 *   BG2: Z = D + ((b & 0x80) ? 11 : 3), MASK = 0x7f, DCMODE = 0.
 *
 * Each family has 7 functions, one per math op (Nomath, Add,
 * AddF1_2, AddS1_2, Sub, SubF1_2, SubS1_2). The 7-fold fan-out is
 * materialised by DEFINE_M7_BL4X_FAMILY below.
 *
 * The four bilinear corner samples for each sub-pixel position
 * come from M7HR_LOOKUP_4 (in-range) or M7HR_LOOKUP_4_FILL (fill
 * mode, PPU.Mode7Repeat == 3). Samples that fall out of range
 * with PPU.Mode7Repeat in {1, 2} are clipped (no write).
 */

/* Local fan-out for the 14 BL4X functions (2 BGs x 7 math ops).
 *
 * Parameters:
 *   suffix         token suffix appended to the function name.
 *   MATH_SELECTOR  one of NOMATH, REGMATH, MATHF1_2, MATHS1_2.
 *   MATH_OP        ADD or SUB (NOMATH ignores it).
 *   BG_NAME        DrawMode7BG1BL4X or DrawMode7BG2BL4X.
 *   Z_EXPR         Z1/Z2 expression (BG1: D+7. BG2 references 'b'
 *                  declared inside M7HR_BLEND_AND_WRITE).
 *   MASK_VAL       palette mask: 0xff for BG1, 0x7f for BG2.
 *   DC_EXPR        Direct Colour Mode enable expression.
 *
 * Body structure mirrors the HR4X fan-out (per-line matrix setup,
 * x in [Left, Right) sub-loop with 4 sub-samples, quarter-step
 * arithmetic, wrap vs repeat-mode split). The only difference
 * from HR4X: each sub-sample writes through bilinear blending
 * instead of nearest-neighbour. */
#define DEFINE_M7_BL4X_FAMILY(suffix, MATH_SELECTOR, MATH_OP,                  \
                              BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR)              \
static void BG_NAME##suffix##Normal1x1 (uint32 Left, uint32 Right, int D)      \
{                                                                              \
    struct SLineMatrixData *l;                                                 \
    uint32 x, Line, Offset;                                                    \
    uint8 *VRAM1 = Memory.VRAM + 1;                                            \
    int aa, cc, aa_q, cc_q, startx;                                            \
                                                                               \
    GFX.RealScreenColors = IPPU.ScreenColors;                                  \
    if (DC_EXPR)                                                               \
    {                                                                          \
        if (IPPU.DirectColourMapsNeedRebuild)                                  \
            S9xBuildDirectColourMaps();                                        \
        GFX.RealScreenColors = DirectColourMaps[0];                            \
    }                                                                          \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL;                                             \
    l = &LineMatrixData[GFX.StartY];                                           \
                                                                               \
    for (Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++)  \
    {                                                                          \
        int AA, BB, CC, DD, xx, yy;                                            \
        int32 HOffset, VOffset, CentreX, CentreY;                              \
        uint8 starty;                                                          \
                                                                               \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19;                            \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19;                            \
        CentreX = ((int32) l->CentreX << 19) >> 19;                            \
        CentreY = ((int32) l->CentreY << 19) >> 19;                            \
                                                                               \
        starty = Line + 1;                                                     \
        if (PPU.Mode7VFlip)                                                    \
            starty ^= 0xff;                                                    \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY);                            \
        BB = ((l->MatrixB * starty) & ~63)                                     \
           + ((l->MatrixB * yy)     & ~63) + (CentreX << 8);                   \
        DD = ((l->MatrixD * starty) & ~63)                                     \
           + ((l->MatrixD * yy)     & ~63) + (CentreY << 8);                   \
                                                                               \
        if (PPU.Mode7HFlip)                                                    \
        {                                                                      \
            startx = Right - 1;                                                \
            aa = -l->MatrixA;                                                  \
            cc = -l->MatrixC;                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            startx = Left;                                                     \
            aa = l->MatrixA;                                                   \
            cc = l->MatrixC;                                                   \
        }                                                                      \
        aa_q = aa / 4;                                                         \
        cc_q = cc / 4;                                                         \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX);                            \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63);                  \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63);                  \
                                                                               \
        if (!PPU.Mode7Repeat)                                                  \
        {                                                                      \
            /* Wrap path. */                                                   \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int sub;                                                       \
                for (sub = 0; sub < 4; sub++)                                  \
                {                                                              \
                    M7HR_SAMPLE_BILINEAR(4 * x + sub, AA + BB, CC + DD,        \
                        VRAM1, MASK_VAL,                                       \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                    if (sub < 3) { AA += aa_q;          CC += cc_q;          } \
                    else         { AA += aa - 3 * aa_q; CC += cc - 3 * cc_q; } \
                }                                                              \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            /* Repeat-mode path: clip out-of-range samples (modes 1/2)         \
             * or fill with tile 0 (mode 3). */                                \
            for (x = Left; x < Right; x++)                                     \
            {                                                                  \
                int sub;                                                       \
                for (sub = 0; sub < 4; sub++)                                  \
                {                                                              \
                    int Xs = (AA + BB) >> 8;                                   \
                    int Ys = (CC + DD) >> 8;                                   \
                    uint32 Xfs = (uint32)((AA + BB) & 0xff);                   \
                    uint32 Yfs = (uint32)((CC + DD) & 0xff);                   \
                    uint8 p_tl, p_tr, p_bl, p_br;                              \
                    uint8 b_tl_raw_;                                           \
                    if (((Xs | Ys) & ~0x3ff) == 0)                             \
                    {                                                          \
                        M7HR_LOOKUP_4(Xs, Ys, p_tl, p_tr, p_bl, p_br,          \
                                      b_tl_raw_, VRAM1, MASK_VAL);             \
                        M7HR_BLEND_AND_WRITE(4 * x + sub,                      \
                            p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xfs, Yfs,       \
                            MATH_SELECTOR, MATH_OP,                            \
                            (Z_EXPR), (Z_EXPR), Offset);                       \
                    }                                                          \
                    else if (PPU.Mode7Repeat == 3)                             \
                    {                                                          \
                        M7HR_LOOKUP_4_FILL(Xs, Ys, p_tl, p_tr, p_bl, p_br,     \
                                           b_tl_raw_, VRAM1, MASK_VAL);        \
                        M7HR_BLEND_AND_WRITE(4 * x + sub,                      \
                            p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xfs, Yfs,       \
                            MATH_SELECTOR, MATH_OP,                            \
                            (Z_EXPR), (Z_EXPR), Offset);                       \
                    }                                                          \
                    if (sub < 3) { AA += aa_q;          CC += cc_q;          } \
                    else         { AA += aa - 3 * aa_q; CC += cc - 3 * cc_q; } \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}

/* BG1 fan-out: 7 math variants. Z = D + 7 const. MASK = 0xff.
 * DCMODE follows $2130 bit 0 (Direct Colour Mode). */
DEFINE_M7_BL4X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL4X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG1BL4X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

static void (*Renderers_DrawMode7BG1BL4XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG1BL4X_Normal1x1,
    DrawMode7BG1BL4XAdd_Normal1x1,
    DrawMode7BG1BL4XAddF1_2_Normal1x1,
    DrawMode7BG1BL4XAddS1_2_Normal1x1,
    DrawMode7BG1BL4XSub_Normal1x1,
    DrawMode7BG1BL4XSubF1_2_Normal1x1,
    DrawMode7BG1BL4XSubS1_2_Normal1x1,
};

/* BG2 fan-out: see HR4X for the EXTBG priority bit explanation.
 * Same Z_EXPR shape (references the local 'b' declared inside
 * M7HR_BLEND_AND_WRITE). */
DEFINE_M7_BL4X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL4X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG2BL4X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

static void (*Renderers_DrawMode7BG2BL4XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG2BL4X_Normal1x1,
    DrawMode7BG2BL4XAdd_Normal1x1,
    DrawMode7BG2BL4XAddF1_2_Normal1x1,
    DrawMode7BG2BL4XAddS1_2_Normal1x1,
    DrawMode7BG2BL4XSub_Normal1x1,
    DrawMode7BG2BL4XSubF1_2_Normal1x1,
    DrawMode7BG2BL4XSubS1_2_Normal1x1,
};

#undef DEFINE_M7_BL4X_FAMILY

/* End of BL4X de-templated section.
 * ==================================================================== */

/* Mode 7 1x bilinear (BL1X) renderers: bilinear filter at native
   width. One sample per native pixel. Used when bilinear is on at
   Hires=off. */

/* ====================================================================
 * Mode 7 native (1x) bilinear (BL1X) renderers
 * ====================================================================
 *
 * Two NAME1 families: DrawMode7BG1BL1X (BG1) and DrawMode7BG2BL1X
 * (BG2 EXTBG). Bilinear-filtered Mode 7 at native (1x) horizontal
 * resolution. One sample per native pixel, but each sample goes
 * through bilinear blending via M7HR_BLEND_AND_WRITE. Used when
 * Settings.Mode7HiresBilinear is on at Hires=off -- gives a
 * smoother native-width Mode 7 without committing to a wider buffer.
 *
 * Differs from BL2X / BL4X by writing one sample per native pixel
 * (no sub-pixel division of aa/cc): the per-iteration advance is
 * AA += aa, CC += cc, and output goes to GFX.S[Offset + x] (no
 * 2*x or 4*x sub-pixel index).
 *
 * BG1 vs BG2 split: same as the other M7 hires families.
 *   BG1: Z = D + 7 const, MASK = 0xff, DCMODE = $2130 bit 0.
 *   BG2: Z = D + ((b & 0x80) ? 11 : 3), MASK = 0x7f, DCMODE = 0.
 *
 * Each family has 7 functions, one per math op. The 7-fold fan-out
 * is materialised by DEFINE_M7_BL1X_FAMILY below.
 */

/* Local fan-out for the 14 BL1X functions (2 BGs x 7 math ops).
 *
 * Parameters (same shape as DEFINE_M7_BL4X_FAMILY):
 *   suffix         token suffix appended to the function name.
 *   MATH_SELECTOR  one of NOMATH, REGMATH, MATHF1_2, MATHS1_2.
 *   MATH_OP        ADD or SUB.
 *   BG_NAME        DrawMode7BG1BL1X or DrawMode7BG2BL1X.
 *   Z_EXPR         Z1/Z2 expression. Same shape as BL4X.
 *   MASK_VAL       palette mask: 0xff for BG1, 0x7f for BG2.
 *   DC_EXPR        Direct Colour Mode enable expression.
 *
 * Body structure: per-line matrix setup (identical to BL4X /
 * HR4X), then a single sample per native pixel x (no sub-loop).
 * The wrap path uses M7HR_SAMPLE_BILINEAR; the repeat path
 * inlines the LOOKUP_4 / LOOKUP_4_FILL split because it needs
 * per-sample range checking. */
#define DEFINE_M7_BL1X_FAMILY(suffix, MATH_SELECTOR, MATH_OP,                  \
                              BG_NAME, Z_EXPR, MASK_VAL, DC_EXPR)              \
static void BG_NAME##suffix##Normal1x1 (uint32 Left, uint32 Right, int D)      \
{                                                                              \
    struct SLineMatrixData *l;                                                 \
    uint32 x, Line, Offset;                                                    \
    uint8 *VRAM1 = Memory.VRAM + 1;                                            \
    int aa, cc, startx;                                                        \
                                                                               \
    GFX.RealScreenColors = IPPU.ScreenColors;                                  \
    if (DC_EXPR)                                                               \
    {                                                                          \
        if (IPPU.DirectColourMapsNeedRebuild)                                  \
            S9xBuildDirectColourMaps();                                        \
        GFX.RealScreenColors = DirectColourMaps[0];                            \
    }                                                                          \
    GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
    Offset = GFX.StartY * GFX.PPL;                                             \
    l = &LineMatrixData[GFX.StartY];                                           \
                                                                               \
    for (Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++)  \
    {                                                                          \
        int AA, BB, CC, DD, xx, yy;                                            \
        int32 HOffset, VOffset, CentreX, CentreY;                              \
        uint8 starty;                                                          \
                                                                               \
        HOffset = ((int32) l->M7HOFS  << 19) >> 19;                            \
        VOffset = ((int32) l->M7VOFS  << 19) >> 19;                            \
        CentreX = ((int32) l->CentreX << 19) >> 19;                            \
        CentreY = ((int32) l->CentreY << 19) >> 19;                            \
                                                                               \
        starty = Line + 1;                                                     \
        if (PPU.Mode7VFlip)                                                    \
            starty ^= 0xff;                                                    \
        yy = CLIP_10_BIT_SIGNED(VOffset - CentreY);                            \
        BB = ((l->MatrixB * starty) & ~63)                                     \
           + ((l->MatrixB * yy)     & ~63) + (CentreX << 8);                   \
        DD = ((l->MatrixD * starty) & ~63)                                     \
           + ((l->MatrixD * yy)     & ~63) + (CentreY << 8);                   \
                                                                               \
        if (PPU.Mode7HFlip)                                                    \
        {                                                                      \
            startx = Right - 1;                                                \
            aa = -l->MatrixA;                                                  \
            cc = -l->MatrixC;                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            startx = Left;                                                     \
            aa = l->MatrixA;                                                   \
            cc = l->MatrixC;                                                   \
        }                                                                      \
        xx = CLIP_10_BIT_SIGNED(HOffset - CentreX);                            \
        AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63);                  \
        CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63);                  \
                                                                               \
        if (!PPU.Mode7Repeat)                                                  \
        {                                                                      \
            /* Wrap path. */                                                   \
            for (x = Left; x < Right; x++, AA += aa, CC += cc)                 \
            {                                                                  \
                M7HR_SAMPLE_BILINEAR(x, AA + BB, CC + DD,                      \
                    VRAM1, MASK_VAL,                                           \
                    MATH_SELECTOR, MATH_OP,                                    \
                    (Z_EXPR), (Z_EXPR), Offset);                               \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            /* Repeat-mode path: clip out-of-range samples (modes 1/2)         \
             * or fill with tile 0 (mode 3). */                                \
            for (x = Left; x < Right; x++, AA += aa, CC += cc)                 \
            {                                                                  \
                int Xs = (AA + BB) >> 8;                                       \
                int Ys = (CC + DD) >> 8;                                       \
                uint32 Xfs = (uint32)((AA + BB) & 0xff);                       \
                uint32 Yfs = (uint32)((CC + DD) & 0xff);                       \
                uint8 p_tl, p_tr, p_bl, p_br;                                  \
                uint8 b_tl_raw_;                                               \
                if (((Xs | Ys) & ~0x3ff) == 0)                                 \
                {                                                              \
                    M7HR_LOOKUP_4(Xs, Ys, p_tl, p_tr, p_bl, p_br,              \
                                  b_tl_raw_, VRAM1, MASK_VAL);                 \
                    M7HR_BLEND_AND_WRITE(x,                                    \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xfs, Yfs,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
                else if (PPU.Mode7Repeat == 3)                                 \
                {                                                              \
                    M7HR_LOOKUP_4_FILL(Xs, Ys, p_tl, p_tr, p_bl, p_br,         \
                                       b_tl_raw_, VRAM1, MASK_VAL);            \
                    M7HR_BLEND_AND_WRITE(x,                                    \
                        p_tl, p_tr, p_bl, p_br, b_tl_raw_, Xfs, Yfs,           \
                        MATH_SELECTOR, MATH_OP,                                \
                        (Z_EXPR), (Z_EXPR), Offset);                           \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }                                                                          \
}

/* BG1 fan-out: 7 math variants. Z = D + 7 const. MASK = 0xff.
 * DCMODE follows $2130 bit 0 (Direct Colour Mode). */
DEFINE_M7_BL1X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))
DEFINE_M7_BL1X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG1BL1X,
                      (D + 7), 0xff, (Memory.FillRAM[0x2130] & 1))

static void (*Renderers_DrawMode7BG1BL1XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG1BL1X_Normal1x1,
    DrawMode7BG1BL1XAdd_Normal1x1,
    DrawMode7BG1BL1XAddF1_2_Normal1x1,
    DrawMode7BG1BL1XAddS1_2_Normal1x1,
    DrawMode7BG1BL1XSub_Normal1x1,
    DrawMode7BG1BL1XSubF1_2_Normal1x1,
    DrawMode7BG1BL1XSubS1_2_Normal1x1,
};

/* BG2 fan-out: see HR4X / BL4X for the EXTBG priority bit
 * explanation. */
DEFINE_M7_BL1X_FAMILY(_,        NOMATH,   ADD, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(Add_,     REGMATH,  ADD, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(AddF1_2_, MATHF1_2, ADD, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(AddS1_2_, MATHS1_2, ADD, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(Sub_,     REGMATH,  SUB, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(SubF1_2_, MATHF1_2, SUB, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)
DEFINE_M7_BL1X_FAMILY(SubS1_2_, MATHS1_2, SUB, DrawMode7BG2BL1X,
                      (D + ((b & 0x80) ? 11 : 3)), 0x7f, 0)

static void (*Renderers_DrawMode7BG2BL1XNormal1x1[7]) (uint32, uint32, int) =
{
    DrawMode7BG2BL1X_Normal1x1,
    DrawMode7BG2BL1XAdd_Normal1x1,
    DrawMode7BG2BL1XAddF1_2_Normal1x1,
    DrawMode7BG2BL1XAddS1_2_Normal1x1,
    DrawMode7BG2BL1XSub_Normal1x1,
    DrawMode7BG2BL1XSubF1_2_Normal1x1,
    DrawMode7BG2BL1XSubS1_2_Normal1x1,
};

#undef DEFINE_M7_BL1X_FAMILY

/* End of BL1X de-templated section.
 * ==================================================================== */

/*****************************************************************************/
#else
#ifndef NAME2 /* Second-level: Get all the NAME1 renderers. */
/*****************************************************************************/

#define BPSTART	StartLine
#define PITCH	1

/* The 1x1 pixel plotter, for speedhacking modes. */

#define DRAW_PIXEL(N, M) \
	if (Z1 > GFX.DB[Offset + N] && (M)) \
	{ \
		GFX.S[Offset + N] = MATH(GFX.ScreenColors[Pix], GFX.SubScreen[Offset + N], GFX.SubZBuffer[Offset + N]); \
		GFX.DB[Offset + N] = Z2; \
	}

#define NAME2	Normal1x1

/* Third-level include: Get the Normal1x1 renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

/* The remaining NAME2 variants (Normal2x1, Hires, Interlace,
   HiresInterlace) are not used by the M7Hires Mode 7 renderers - the
   M7Hires DRAW_TILE body emits to 2x output indices itself, so it pairs
   with the Normal1x1 DRAW_PIXEL above. The M7HIRES_ONLY guard lets
   those NAME1 instantiations skip these blocks and emit only the
   Normal1x1 variant. All other NAME1 instantiations leave M7HIRES_ONLY
   undefined, so the existing set of variants is generated unchanged. */
#ifndef M7HIRES_ONLY

/* The 2x1 pixel plotter, for normal rendering when we've used hires/interlace already this frame. */

#define DRAW_PIXEL_N2x1(N, M) \
	if (Z1 > GFX.DB[Offset + 2 * N] && (M)) \
	{ \
		GFX.S[Offset + 2 * N] = GFX.S[Offset + 2 * N + 1] = MATH(GFX.ScreenColors[Pix], GFX.SubScreen[Offset + 2 * N], GFX.SubZBuffer[Offset + 2 * N]); \
		GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = Z2; \
	}

#define DRAW_PIXEL(N, M)	DRAW_PIXEL_N2x1(N, M)
#define NAME2				Normal2x1

/* Third-level include: Get the Normal2x1 renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

/* The 4x1 pixel plotter, for Mode 7 hires 4x rendering. Each native
   pixel writes to four buffer pixels at Offset + 4*N, +4*N+1,
   +4*N+2, +4*N+3. Used for non-Mode7 BG layers and sprites/HUD when
   the frame is in 4x Mode 7 hires; the Mode 7 BGs themselves use
   the M7HR_4X / M7BL_4X tile bodies which write to the same indices
   directly. */

#define DRAW_PIXEL_N4x1(N, M) \
	if (Z1 > GFX.DB[Offset + 4 * N] && (M)) \
	{ \
		uint16 cc__ = MATH(GFX.ScreenColors[Pix], GFX.SubScreen[Offset + 4 * N], GFX.SubZBuffer[Offset + 4 * N]); \
		GFX.S[Offset + 4 * N] = GFX.S[Offset + 4 * N + 1] = GFX.S[Offset + 4 * N + 2] = GFX.S[Offset + 4 * N + 3] = cc__; \
		GFX.DB[Offset + 4 * N] = GFX.DB[Offset + 4 * N + 1] = GFX.DB[Offset + 4 * N + 2] = GFX.DB[Offset + 4 * N + 3] = Z2; \
	}

#define DRAW_PIXEL(N, M)	DRAW_PIXEL_N4x1(N, M)
#define NAME2				Normal4x1

/* Third-level include: Get the Normal4x1 renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

/* Hires pixel plotter, this combines the main and subscreen pixels as appropriate to render hires or pseudo-hires images.*/
/* Use it only on the main screen, subscreen should use Normal2x1 instead.*/
/* Hires math:*/
/*     Main pixel is mathed as normal: Main(x, y) * Sub(x, y).*/
/*     Sub pixel is mathed somewhat weird: Basically, for Sub(x + 1, y) we apply the same operation we applied to Main(x, y)*/
/*     (e.g. no math, add fixed, add1/2 subscreen) using Main(x, y) as the "corresponding subscreen pixel".*/
/*     Also, color window clipping clips Sub(x + 1, y) if Main(x, y) is clipped, not Main(x + 1, y).*/
/*     We don't know how Sub(0, y) is handled.*/

#define DRAW_PIXEL_H2x1(N, M) \
if (Z1 > GFX.DB[Offset + 2 * N] && (M)) \
    { \
        GFX.S[Offset + 2 * N] = MATH(GFX.ScreenColors[Pix], GFX.SubScreen[Offset + 2 * N], GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.S[Offset + 2 * N + 1] = MATH((GFX.ClipColors ? 0 : GFX.SubScreen[Offset + 2 * N + 2]), GFX.RealScreenColors[Pix], GFX.SubZBuffer[Offset + 2 * N]); \
        GFX.DB[Offset + 2 * N] = GFX.DB[Offset + 2 * N + 1] = Z2; \
    }

#define DRAW_PIXEL(N, M)	DRAW_PIXEL_H2x1(N, M)
#define NAME2				Hires

/* Third-level include: Get the Hires renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

/* Interlace: Only draw every other line, so we'll redefine BPSTART and
   PITCH to do so.

   Otherwise, it's the same as Normal2x1/Hires2x1. */

#undef BPSTART
#undef PITCH

#define BPSTART	(StartLine * 2 + BG.InterlaceLine)
#define PITCH	2

#ifndef NO_INTERLACE

#define DRAW_PIXEL(N, M)	DRAW_PIXEL_N2x1(N, M)
#define NAME2				Interlace

/* Third-level include: Get the double width Interlace renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

#define DRAW_PIXEL(N, M)	DRAW_PIXEL_H2x1(N, M)
#define NAME2				HiresInterlace

/* Third-level include: Get the HiresInterlace renderers. */

#include "tile.c"

#undef NAME2
#undef DRAW_PIXEL

#endif

#endif /* !M7HIRES_ONLY */

#undef BPSTART
#undef PITCH

/*****************************************************************************/
#else /* Third-level: Renderers for each math mode for NAME1 + NAME2. */
/*****************************************************************************/

#define CONCAT3(A, B, C)	A##B##C
#define MAKENAME(A, B, C)	CONCAT3(A, B, C)

/* Each of the 7 math variants below defines three macros that
   together describe the colour-math op for this specialisation:

     MATH(A, B, C)        compose-and-return form. Used by the older
                          DRAW_PIXEL_* plotters that write through a
                          single MATH(...) call site.
     MATH_SELECTOR        token: NOMATH, REGMATH, MATHF1_2, or
                          MATHS1_2. Used by helpers that take the
                          selector and op as separate parameters,
                          e.g. M7HR_BLEND_AND_WRITE invokes
                          math_selector(math_op, A, B, C).
     MATH_OP              token: ADD or SUB (for selectors that
                          care; NOMATH ignores it).

   MATH is the compose-and-return form: MATH(A,B,C) yields the
   colour. MATH_SELECTOR / MATH_OP are the lower-level pair that
   the parameterised helpers consume. Both forms describe the same
   op; pick whichever fits the caller. */

static void MAKENAME(NAME1, _, NAME2) (ARGS)
{
#define MATH(A, B, C)	NOMATH(x, A, B, C)
#define MATH_SELECTOR	NOMATH
#define MATH_OP		x
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, Add_, NAME2) (ARGS)
{
#define MATH(A, B, C)	REGMATH(ADD, A, B, C)
#define MATH_SELECTOR	REGMATH
#define MATH_OP		ADD
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, AddF1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHF1_2(ADD, A, B, C)
#define MATH_SELECTOR	MATHF1_2
#define MATH_OP		ADD
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, AddS1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHS1_2(ADD, A, B, C)
#define MATH_SELECTOR	MATHS1_2
#define MATH_OP		ADD
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, Sub_, NAME2) (ARGS)
{
#define MATH(A, B, C)	REGMATH(SUB, A, B, C)
#define MATH_SELECTOR	REGMATH
#define MATH_OP		SUB
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, SubF1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHF1_2(SUB, A, B, C)
#define MATH_SELECTOR	MATHF1_2
#define MATH_OP		SUB
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void MAKENAME(NAME1, SubS1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHS1_2(SUB, A, B, C)
#define MATH_SELECTOR	MATHS1_2
#define MATH_OP		SUB
	DRAW_TILE();
#undef MATH
#undef MATH_SELECTOR
#undef MATH_OP
}

static void (*MAKENAME(Renderers_, NAME1, NAME2)[7]) (ARGS) =
{
	MAKENAME(NAME1, _, NAME2),
	MAKENAME(NAME1, Add_, NAME2),
	MAKENAME(NAME1, AddF1_2_, NAME2),
	MAKENAME(NAME1, AddS1_2_, NAME2),
	MAKENAME(NAME1, Sub_, NAME2),
	MAKENAME(NAME1, SubF1_2_, NAME2),
	MAKENAME(NAME1, SubS1_2_, NAME2)
};

#undef MAKENAME
#undef CONCAT3

#endif
#endif
#endif
