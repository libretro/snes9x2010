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

	if (!IPPU.DoubleWidthPixels)	/* normal width */
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

	/* Mode 7 hires: when the option is on, the frame is Mode 7, the
	   buffer has been promoted to 2x width (either at frame start in
	   cpuexec.c or mid-frame in S9xUpdateScreen), and the BG is not
	   mosaicked, override DM7BG1/DM7BG2 to point at the M7Hires
	   renderers. Mosaic falls through to the existing Normal2x1
	   mosaic - sub-pixel refinement of mosaic blocks is meaningless.
	   Subscreen also falls through - color math composition stays at
	   native sample rate. */
	if (Settings.Mode7Hires && BGMode == 7 && IPPU.DoubleWidthPixels && !sub)
	{
		if (!M7M1)
			DM7BG1 = Renderers_DrawMode7BG1HRNormal1x1;
		if (!M7M2)
			DM7BG2 = Renderers_DrawMode7BG2HRNormal1x1;
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

/* Basic routine to render the backdrop.

   DRAW_PIXEL is the same as above, but since we're just replicating a single
   pixel there's no need for PITCH or BPSTART
   (or interlace at all, really).

   The backdrop is always depth = 1, so Z1 = Z2 = 1.
   And backdrop is always color 0. */

#define NO_INTERLACE	1
#define Z1				1
#define Z2				1
#define Pix				0

#define DRAW_TILE() \
	uint32	l, x; \
	\
	GFX.RealScreenColors = IPPU.ScreenColors; \
	GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
	\
	for (l = GFX.StartY; l <= GFX.EndY; l++, Offset += GFX.PPL) \
	{ \
		for (x = Left; x < Right; x++) \
			DRAW_PIXEL(x, 1); \
	}

#define NAME1	DrawBackdrop16
#define ARGS	uint32 Offset, uint32 Left, uint32 Right

/* Second-level include: Get the DrawBackdrop16 renderers. */

#include "tile.c"

#undef NAME1
#undef ARGS
#undef DRAW_TILE
#undef Pix
#undef Z1
#undef Z2
#undef NO_INTERLACE

/* Basic routine to render a chunk of a Mode 7 BG.*/
/* Mode 7 has no interlace, so BPSTART and PITCH are unused.*/
/* We get some new parameters, so we can use the same DRAW_TILE to do BG1 or BG2:*/
/*     DCMODE tests if Direct Color should apply.*/
/*     BG is the BG, so we use the right clip window.*/
/*     MASK is 0xff or 0x7f, the 'color' portion of the pixel.*/
/* We define Z1/Z2 to either be constant 5 or to vary depending on the 'priority' portion of the pixel.*/

#define CLIP_10_BIT_SIGNED(a)	(((a) & 0x2000) ? ((a) | ~0x3ff) : ((a) & 0x3ff))

extern struct SLineMatrixData	LineMatrixData[240];

#define NO_INTERLACE	1
#define Z1		(D + 7)
#define Z2		(D + 7)
#define MASK		0xff
#define DCMODE		(Memory.FillRAM[0x2130] & 1)
#define BG		0

#define DRAW_TILE_NORMAL() \
	struct SLineMatrixData *l; \
	uint32 x, Line, Offset; \
	uint8	*VRAM1; \
	int aa, cc, startx; \
	VRAM1 = Memory.VRAM + 1; \
   GFX.RealScreenColors = IPPU.ScreenColors; \
	if (DCMODE) \
	{ \
		if (IPPU.DirectColourMapsNeedRebuild) \
			S9xBuildDirectColourMaps(); \
		GFX.RealScreenColors = DirectColourMaps[0]; \
	} \
	\
	GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
	Offset = GFX.StartY * GFX.PPL; \
	l = &LineMatrixData[GFX.StartY]; \
	\
	for ( Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++) \
	{ \
		int AA, BB, CC, DD, xx, yy; \
		int32 HOffset, VOffset, CentreX, CentreY; \
		uint8 Pix, starty; \
		\
		HOffset = ((int32) l->M7HOFS  << 19) >> 19; \
		VOffset = ((int32) l->M7VOFS  << 19) >> 19; \
		CentreX = ((int32) l->CentreX << 19) >> 19; \
		CentreY = ((int32) l->CentreY << 19) >> 19; \
		\
		starty = Line + 1; \
		if (PPU.Mode7VFlip) \
			starty ^= 0xff; \
		yy = CLIP_10_BIT_SIGNED(VOffset - CentreY); \
		BB = ((l->MatrixB * starty) & ~63) + ((l->MatrixB * yy) & ~63) + (CentreX << 8); \
		DD = ((l->MatrixD * starty) & ~63) + ((l->MatrixD * yy) & ~63) + (CentreY << 8); \
		\
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
				\
				X = ((AA + BB) >> 8) & 0x3ff; \
				Y = ((CC + DD) >> 8) & 0x3ff; \
				\
				TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
				b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				\
				DRAW_PIXEL(x, Pix = (b & MASK)); \
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
				\
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
				\
				DRAW_PIXEL(x, Pix = (b & MASK)); \
			} \
		} \
	}

/* High-resolution Mode 7 renderer body: same structure as DRAW_TILE_NORMAL
   but produces two output pixels per native column with the matrix step
   halved, giving 2x horizontal anti-aliased output.

   Per native column x in [Left, Right), generates two output samples at
   buffer indices 2*x and 2*x+1. Between samples the matrix coordinates
   advance by aa_h = aa/2 and cc_h = cc/2 (16.8 fixed-point, a >>1
   preserves precision adequately - the SNES uses the same integer
   arithmetic for sub-pixel matrix walks and rounding errors below the
   half-LSB don't propagate visibly across a 128x128 texture).

   Total advance per native column is still aa, cc (two half steps). The
   second sample lands halfway between where two adjacent native pixels
   would have sampled, which is what produces the smoother appearance.

   Used in conjunction with the Normal1x1 DRAW_PIXEL macro (which writes
   to GFX.S[Offset + N]) so the two pixel writes hit Offset + 2*x and
   Offset + 2*x + 1 in the doubled output buffer. */

#define DRAW_TILE_NORMAL_M7HIRES() \
	struct SLineMatrixData *l; \
	uint32 x, Line, Offset; \
	uint8	*VRAM1; \
	int aa, cc, aa_h, cc_h, startx; \
	VRAM1 = Memory.VRAM + 1; \
   GFX.RealScreenColors = IPPU.ScreenColors; \
	if (DCMODE) \
	{ \
		if (IPPU.DirectColourMapsNeedRebuild) \
			S9xBuildDirectColourMaps(); \
		GFX.RealScreenColors = DirectColourMaps[0]; \
	} \
	\
	GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
	Offset = GFX.StartY * GFX.PPL; \
	l = &LineMatrixData[GFX.StartY]; \
	\
	for ( Line = GFX.StartY; Line <= GFX.EndY; Line++, Offset += GFX.PPL, l++) \
	{ \
		int AA, BB, CC, DD, xx, yy; \
		int32 HOffset, VOffset, CentreX, CentreY; \
		uint8 Pix, starty; \
		\
		HOffset = ((int32) l->M7HOFS  << 19) >> 19; \
		VOffset = ((int32) l->M7VOFS  << 19) >> 19; \
		CentreX = ((int32) l->CentreX << 19) >> 19; \
		CentreY = ((int32) l->CentreY << 19) >> 19; \
		\
		starty = Line + 1; \
		if (PPU.Mode7VFlip) \
			starty ^= 0xff; \
		yy = CLIP_10_BIT_SIGNED(VOffset - CentreY); \
		BB = ((l->MatrixB * starty) & ~63) + ((l->MatrixB * yy) & ~63) + (CentreX << 8); \
		DD = ((l->MatrixD * starty) & ~63) + ((l->MatrixD * yy) & ~63) + (CentreY << 8); \
		\
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
		aa_h = aa / 2; \
		cc_h = cc / 2; \
		xx = CLIP_10_BIT_SIGNED(HOffset - CentreX); \
		AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63); \
		CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63); \
		if (!PPU.Mode7Repeat) \
		{ \
			for ( x = Left; x < Right; x++) \
			{ \
				int X, Y; \
				uint8 *TileData, b; \
				\
				/* Sample 1 -> output index 2*x */ \
				X = ((AA + BB) >> 8) & 0x3ff; \
				Y = ((CC + DD) >> 8) & 0x3ff; \
				TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
				b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				DRAW_PIXEL(2 * x, Pix = (b & MASK)); \
				AA += aa_h; CC += cc_h; \
				\
				/* Sample 2 -> output index 2*x + 1 */ \
				X = ((AA + BB) >> 8) & 0x3ff; \
				Y = ((CC + DD) >> 8) & 0x3ff; \
				TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
				b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				DRAW_PIXEL(2 * x + 1, Pix = (b & MASK)); \
				AA += aa - aa_h; CC += cc - cc_h; \
			} \
		} \
		else \
		{ \
			for ( x = Left; x < Right; x++) \
			{ \
				int X, Y; \
				uint8 *TileData, b; \
				int do_draw; \
				\
				/* Sample 1 -> output index 2*x */ \
				X = ((AA + BB) >> 8); \
				Y = ((CC + DD) >> 8); \
				do_draw = 1; \
				if (((X | Y) & ~0x3ff) == 0) \
				{ \
					TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
					b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				} \
				else if (PPU.Mode7Repeat == 3) \
					b = *(VRAM1    + ((Y & 7) << 4) + ((X & 7) << 1)); \
				else \
					do_draw = 0; \
				if (do_draw) \
					DRAW_PIXEL(2 * x, Pix = (b & MASK)); \
				AA += aa_h; CC += cc_h; \
				\
				/* Sample 2 -> output index 2*x + 1 */ \
				X = ((AA + BB) >> 8); \
				Y = ((CC + DD) >> 8); \
				do_draw = 1; \
				if (((X | Y) & ~0x3ff) == 0) \
				{ \
					TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
					b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				} \
				else if (PPU.Mode7Repeat == 3) \
					b = *(VRAM1    + ((Y & 7) << 4) + ((X & 7) << 1)); \
				else \
					do_draw = 0; \
				if (do_draw) \
					DRAW_PIXEL(2 * x + 1, Pix = (b & MASK)); \
				AA += aa - aa_h; CC += cc - cc_h; \
			} \
		} \
	}

#define DRAW_TILE_MOSAIC() \
	struct SLineMatrixData *l; \
	uint32 Line, Offset; \
	int32 h, w, x, MLeft, MRight; \
	int	aa, cc, startx, StartY, HMosaic, VMosaic, MosaicStart; \
	uint8 *VRAM1; \
	VRAM1 = Memory.VRAM + 1; \
   GFX.RealScreenColors = IPPU.ScreenColors; \
	if (DCMODE) \
	{ \
		if (IPPU.DirectColourMapsNeedRebuild) \
			S9xBuildDirectColourMaps(); \
		GFX.RealScreenColors = DirectColourMaps[0]; \
	} \
	\
	GFX.ScreenColors = GFX.ClipColors ? BlackColourMap : GFX.RealScreenColors; \
	\
	\
	StartY = GFX.StartY; \
	HMosaic = 1; \
	VMosaic = 1; \
	MosaicStart = 0; \
	MLeft = Left; \
	MRight = Right; \
	\
	if (PPU.BGMosaic[0]) \
	{ \
		VMosaic = PPU.Mosaic; \
		MosaicStart = ((uint32) GFX.StartY - PPU.MosaicStart) % VMosaic; \
		StartY -= MosaicStart; \
	} \
	\
	if (PPU.BGMosaic[BG]) \
	{ \
		HMosaic = PPU.Mosaic; \
		MLeft  -= MLeft  % HMosaic; \
		MRight += HMosaic - 1; \
		MRight -= MRight % HMosaic; \
	} \
	\
	Offset = StartY * GFX.PPL; \
	l = &LineMatrixData[StartY]; \
	\
	for ( Line = StartY; Line <= GFX.EndY; Line += VMosaic, Offset += VMosaic * GFX.PPL, l += VMosaic) \
	{ \
		int xx, yy, AA, BB, CC, DD; \
		int32 HOffset, VOffset, CentreX, CentreY; \
		uint8 Pix, ctr, starty; \
		if (Line + VMosaic > GFX.EndY) \
			VMosaic = GFX.EndY - Line + 1; \
		\
		HOffset = ((int32) l->M7HOFS  << 19) >> 19; \
		VOffset = ((int32) l->M7VOFS  << 19) >> 19; \
		CentreX = ((int32) l->CentreX << 19) >> 19; \
		CentreY = ((int32) l->CentreY << 19) >> 19; \
		\
		starty = Line + 1; \
		if (PPU.Mode7VFlip) \
			starty ^= 0xff; \
		\
		yy = CLIP_10_BIT_SIGNED(VOffset - CentreY); \
		BB = ((l->MatrixB * starty) & ~63) + ((l->MatrixB * yy) & ~63) + (CentreX << 8); \
		DD = ((l->MatrixD * starty) & ~63) + ((l->MatrixD * yy) & ~63) + (CentreY << 8); \
		\
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
		\
		xx = CLIP_10_BIT_SIGNED(HOffset - CentreX); \
		AA = l->MatrixA * startx + ((l->MatrixA * xx) & ~63); \
		CC = l->MatrixC * startx + ((l->MatrixC * xx) & ~63); \
		ctr = 1; \
		\
		if (!PPU.Mode7Repeat) \
		{ \
			for ( x = MLeft; x < MRight; x++, AA += aa, CC += cc) \
			{ \
				int X, Y; \
				uint8 *TileData, b; \
				if (--ctr) \
					continue; \
				ctr = HMosaic; \
				\
				X = ((AA + BB) >> 8) & 0x3ff; \
				Y = ((CC + DD) >> 8) & 0x3ff; \
				TileData = VRAM1 + (Memory.VRAM[((Y & ~7) << 5) + ((X >> 2) & ~1)] << 7); \
				b = *(TileData + ((Y & 7) << 4) + ((X & 7) << 1)); \
				\
				if ((Pix = (b & MASK))) \
				{ \
					for ( h = MosaicStart; h < VMosaic; h++) \
					{ \
						for ( w = x + HMosaic - 1; w >= x; w--) \
							DRAW_PIXEL(w + h * GFX.PPL, (w >= (int32) Left && w < (int32) Right)); \
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
				\
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
				\
				if ((Pix = (b & MASK))) \
				{ \
					for ( h = MosaicStart; h < VMosaic; h++) \
					{ \
						for ( w = x + HMosaic - 1; w >= x; w--) \
							DRAW_PIXEL(w + h * GFX.PPL, (w >= (int32) Left && w < (int32) Right)); \
					} \
				} \
			} \
		} \
		\
		MosaicStart = 0; \
	}

#define DRAW_TILE()	DRAW_TILE_NORMAL()
#define NAME1		DrawMode7BG1
#define ARGS		uint32 Left, uint32 Right, int D

/* Second-level include: Get the DrawMode7BG1 renderers. */

#include "tile.c"

#undef NAME1
#undef DRAW_TILE

#define DRAW_TILE()	DRAW_TILE_MOSAIC()
#define NAME1		DrawMode7MosaicBG1

/* Second-level include: Get the DrawMode7MosaicBG1 renderers. */

#include "tile.c"

#undef DRAW_TILE
#undef NAME1
#undef Z1
#undef Z2
#undef MASK
#undef DCMODE
#undef BG

#define NAME1		DrawMode7BG2
#define DRAW_TILE()	DRAW_TILE_NORMAL()
#define Z1			(D + ((b & 0x80) ? 11 : 3))
#define Z2			(D + ((b & 0x80) ? 11 : 3))
#define MASK		0x7f
#define DCMODE		0
#define BG			1

/* Second-level include: Get the DrawMode7BG2 renderers. */

#include "tile.c"

#undef NAME1
#undef DRAW_TILE

#define DRAW_TILE()	DRAW_TILE_MOSAIC()
#define NAME1		DrawMode7MosaicBG2

/* Second-level include: Get the DrawMode7MosaicBG2 renderers. */

#include "tile.c"

#undef MASK
#undef DCMODE
#undef BG
#undef NAME1
#undef ARGS
#undef DRAW_TILE
#undef Z1
#undef Z2

/* Mode 7 hires (M7HIRES) renderers: 2x horizontal sample/output for
   Mode 7 backgrounds. Use the M7Hires DRAW_TILE body and gate the
   level-2 body to emit only the Normal1x1 NAME2 variant; the M7Hires
   tile body itself handles output at 2x indices. Two NAME1 variants:
   DrawMode7BG1HR and DrawMode7BG2HR, mirroring the regular Mode 7 BG1
   and BG2 priority/MASK conventions. Mosaic is intentionally not
   implemented for hires - mosaic falls back to the native renderer
   when the option is on (mosaic by definition produces blocks;
   sub-pixel refinement is meaningless there).

   These functions are scaffolded but not yet referenced by any
   dispatcher. The wiring lands in a follow-up patch. */

#define ARGS		uint32 Left, uint32 Right, int D
#define M7HIRES_ONLY

#define Z1			(D + 7)
#define Z2			(D + 7)
#define MASK		0xff
#define DCMODE		(Memory.FillRAM[0x2130] & 1)
#define BG			0
#define DRAW_TILE()	DRAW_TILE_NORMAL_M7HIRES()
#define NAME1		DrawMode7BG1HR

/* Second-level include: Get the DrawMode7BG1HR renderers (Normal1x1 only). */

#include "tile.c"

#undef NAME1
#undef DRAW_TILE
#undef Z1
#undef Z2
#undef MASK
#undef DCMODE
#undef BG

#define Z1			(D + ((b & 0x80) ? 11 : 3))
#define Z2			(D + ((b & 0x80) ? 11 : 3))
#define MASK		0x7f
#define DCMODE		0
#define BG			1
#define DRAW_TILE()	DRAW_TILE_NORMAL_M7HIRES()
#define NAME1		DrawMode7BG2HR

/* Second-level include: Get the DrawMode7BG2HR renderers (Normal1x1 only). */

#include "tile.c"

#undef NAME1
#undef DRAW_TILE
#undef Z1
#undef Z2
#undef MASK
#undef DCMODE
#undef BG
#undef ARGS
#undef M7HIRES_ONLY

#undef DRAW_TILE_NORMAL
#undef DRAW_TILE_NORMAL_M7HIRES
#undef DRAW_TILE_MOSAIC
#undef NO_INTERLACE

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

static void MAKENAME(NAME1, _, NAME2) (ARGS)
{
#define MATH(A, B, C)	NOMATH(x, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, Add_, NAME2) (ARGS)
{
#define MATH(A, B, C)	REGMATH(ADD, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, AddF1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHF1_2(ADD, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, AddS1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHS1_2(ADD, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, Sub_, NAME2) (ARGS)
{
#define MATH(A, B, C)	REGMATH(SUB, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, SubF1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHF1_2(SUB, A, B, C)
	DRAW_TILE();
#undef MATH
}

static void MAKENAME(NAME1, SubS1_2_, NAME2) (ARGS)
{
#define MATH(A, B, C)	MATHS1_2(SUB, A, B, C)
	DRAW_TILE();
#undef MATH
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
