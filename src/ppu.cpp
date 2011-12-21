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
#include "getset.h"
#include "dma.h"
#include "apu.h"
#include "fxemu.h"
#include "fxinst.h"
#include "sdd1.h"
#include "srtc.h"
#include "controls.h"
#include "crosshairs.h"
#include "display.h"
#include "sdd1emu.h"
#include "spc7110emu.h"
#include "ppu.h"
#include "tile.h"

extern uint8	*HDMAMemPointers[8];

extern struct SLineData			LineData[240];

#ifdef REPORT_MODES
static int counter = 0;
#endif

#define TILE_PLUS(t, x)	(((t) & 0xfc00) | ((t + x) & 0x3ff))

bool8 S9xGraphicsInit (void)
{
	S9xInitTileRenderer();

	GFX.DoInterlace = 0;
	GFX.InterlaceFrame = 0;
	GFX.RealPPL = GFX.Pitch >> 1;
	IPPU.OBJChanged = TRUE;
	IPPU.DirectColourMapsNeedRebuild = TRUE;
	S9xFixColourBrightness();

	GFX.X2   = (uint16 *) malloc(sizeof(uint16) * 0x10000);
	GFX.ZERO = (uint16 *) malloc(sizeof(uint16) * 0x10000);

	GFX.ScreenSize = GFX.Pitch / 2 * SNES_HEIGHT_EXTENDED * 2;
	GFX.SubScreen  = (uint16 *) malloc(GFX.ScreenSize * sizeof(uint16));
	GFX.ZBuffer    = (uint8 *)  malloc(GFX.ScreenSize);
	GFX.SubZBuffer = (uint8 *)  malloc(GFX.ScreenSize);

	if (!GFX.X2 || !GFX.ZERO || !GFX.SubScreen || !GFX.ZBuffer || !GFX.SubZBuffer)
	{
		S9xGraphicsDeinit();
		return (FALSE);
	}

	// Lookup table for color addition
	ZeroMemory(GFX.X2, 0x10000 * sizeof(uint16));
	for (uint32 r = 0; r <= MAX_RED; r++)
	{
		uint32	r2 = r << 1;
		if (r2 > MAX_RED)
			r2 = MAX_RED;

		for (uint32 g = 0; g <= MAX_GREEN; g++)
		{
			uint32	g2 = g << 1;
			if (g2 > MAX_GREEN)
				g2 = MAX_GREEN;

			for (uint32 b = 0; b <= MAX_BLUE; b++)
			{
				uint32	b2 = b << 1;
				if (b2 > MAX_BLUE)
					b2 = MAX_BLUE;

				GFX.X2[BUILD_PIXEL2(r, g, b)] = BUILD_PIXEL2(r2, g2, b2);
				GFX.X2[BUILD_PIXEL2(r, g, b) & ~ALPHA_BITS_MASK] = BUILD_PIXEL2(r2, g2, b2);
			}
		}
	}

	// Lookup table for 1/2 color subtraction
	ZeroMemory(GFX.ZERO, 0x10000 * sizeof(uint16));
	for (uint32 r = 0; r <= MAX_RED; r++)
	{
		uint32	r2 = r;
		if (r2 & 0x10)
			r2 &= ~0x10;
		else
			r2 = 0;

		for (uint32 g = 0; g <= MAX_GREEN; g++)
		{
			uint32	g2 = g;
			if (g2 & GREEN_HI_BIT)
				g2 &= ~GREEN_HI_BIT;
			else
				g2 = 0;

			for (uint32 b = 0; b <= MAX_BLUE; b++)
			{
				uint32	b2 = b;
				if (b2 & 0x10)
					b2 &= ~0x10;
				else
					b2 = 0;

				GFX.ZERO[BUILD_PIXEL2(r, g, b)] = BUILD_PIXEL2(r2, g2, b2);
				GFX.ZERO[BUILD_PIXEL2(r, g, b) & ~ALPHA_BITS_MASK] = BUILD_PIXEL2(r2, g2, b2);
			}
		}
	}

	return (TRUE);
}

void S9xGraphicsDeinit (void)
{
	if (GFX.X2)         { free(GFX.X2);         GFX.X2         = NULL; }
	if (GFX.ZERO)       { free(GFX.ZERO);       GFX.ZERO       = NULL; }
	if (GFX.SubScreen)  { free(GFX.SubScreen);  GFX.SubScreen  = NULL; }
	if (GFX.ZBuffer)    { free(GFX.ZBuffer);    GFX.ZBuffer    = NULL; }
	if (GFX.SubZBuffer) { free(GFX.SubZBuffer); GFX.SubZBuffer = NULL; }
}

static int objsize_array[8][4] = {
	{8,	8,	16,	16}, //0
	{8,	8,	32,	32}, //1
	{8,	8,	64,	64}, //2
	{16,	16,	32,	32}, //3
	{16,	16,	64,	64}, //4
	{32,	32,	64,	64}, //5
	{16,	32,	32,	64}, //6
	{16,	32,	32,	32}, //7
};

static void SetupOBJ (void)
{
	// default
	int	SmallWidth	= objsize_array[4][0];
	int	SmallHeight	= objsize_array[4][1];
	int	LargeWidth	= objsize_array[4][2];
	int	LargeHeight	= objsize_array[4][3];

	if(PPU.OBJSizeSelect < 8)
	{
		SmallWidth	= objsize_array[PPU.OBJSizeSelect][0];
		SmallHeight	= objsize_array[PPU.OBJSizeSelect][1];
		LargeWidth	= objsize_array[PPU.OBJSizeSelect][2];
		LargeHeight	= objsize_array[PPU.OBJSizeSelect][3];
	}

	int	inc = IPPU.InterlaceOBJ ? 2 : 1;

	int startline = (IPPU.InterlaceOBJ && GFX.InterlaceFrame) ? 1 : 0;

	// OK, we have three cases here. Either there's no priority, priority is
	// normal FirstSprite, or priority is FirstSprite+Y. The first two are
	// easy, the last is somewhat more ... interesting. So we split them up.

	int		Height;
	uint8	S;

	if (!PPU.OAMPriorityRotation || !(PPU.OAMFlip & PPU.OAMAddr & 1)) // normal case
	{
		uint8	LineOBJ[SNES_HEIGHT_EXTENDED];
		ZeroMemory(LineOBJ, sizeof(LineOBJ));

		for (int i = 0; i < SNES_HEIGHT_EXTENDED; i++)
		{
			GFX.OBJLines[i].RTOFlags = 0;
			GFX.OBJLines[i].Tiles = 34;
			for (int j = 0; j < 32; j++)
				GFX.OBJLines[i].OBJ[j].Sprite = -1;
		}

		uint8	FirstSprite = PPU.FirstSprite;
		S = FirstSprite;

		do
		{
			if (PPU.OBJ[S].Size)
			{
				GFX.OBJWidths[S] = LargeWidth;
				Height = LargeHeight;
			}
			else
			{
				GFX.OBJWidths[S] = SmallWidth;
				Height = SmallHeight;
			}

			int	HPos = PPU.OBJ[S].HPos;
			if (HPos == -256)
				HPos = 0;

			if (HPos > -GFX.OBJWidths[S] && HPos <= 256)
			{
				if (HPos < 0)
					GFX.OBJVisibleTiles[S] = (GFX.OBJWidths[S] + HPos + 7) >> 3;
				else
				if (HPos + GFX.OBJWidths[S] > 255)
					GFX.OBJVisibleTiles[S] = (256 - HPos + 7) >> 3;
				else
					GFX.OBJVisibleTiles[S] = GFX.OBJWidths[S] >> 3;

				for (uint8 line = startline, Y = (uint8) (PPU.OBJ[S].VPos & 0xff); line < Height; Y++, line += inc)
				{
					if (Y >= SNES_HEIGHT_EXTENDED)
						continue;

					if (LineOBJ[Y] >= 32)
					{
						GFX.OBJLines[Y].RTOFlags |= 0x40;
						continue;
					}

					GFX.OBJLines[Y].Tiles -= GFX.OBJVisibleTiles[S];
					if (GFX.OBJLines[Y].Tiles < 0)
						GFX.OBJLines[Y].RTOFlags |= 0x80;

					GFX.OBJLines[Y].OBJ[LineOBJ[Y]].Sprite = S;
					if (PPU.OBJ[S].VFlip)
						// Yes, Width not Height. It so happens that the
						// sprites with H=2*W flip as two WxW sprites.
						GFX.OBJLines[Y].OBJ[LineOBJ[Y]].Line = line ^ (GFX.OBJWidths[S] - 1);
					else
						GFX.OBJLines[Y].OBJ[LineOBJ[Y]].Line = line;

					LineOBJ[Y]++;
				}
			}

			S = (S + 1) & 0x7f;
		} while (S != FirstSprite);

		for (int Y = 1; Y < SNES_HEIGHT_EXTENDED; Y++)
			GFX.OBJLines[Y].RTOFlags |= GFX.OBJLines[Y - 1].RTOFlags;
	}
	else // evil FirstSprite+Y case
	{
		// First, find out which sprites are on which lines
		uint8	OBJOnLine[SNES_HEIGHT_EXTENDED][128];
		ZeroMemory(OBJOnLine, sizeof(OBJOnLine));

		for (S = 0; S < 128; S++)
		{
			if (PPU.OBJ[S].Size)
			{
				GFX.OBJWidths[S] = LargeWidth;
				Height = LargeHeight;
			}
			else
			{
				GFX.OBJWidths[S] = SmallWidth;
				Height = SmallHeight;
			}

			int	HPos = PPU.OBJ[S].HPos;
			if (HPos == -256)
				HPos = 256;

			if (HPos > -GFX.OBJWidths[S] && HPos <= 256)
			{
				if (HPos < 0)
					GFX.OBJVisibleTiles[S] = (GFX.OBJWidths[S] + HPos + 7) >> 3;
				else
				if (HPos + GFX.OBJWidths[S] >= 257)
					GFX.OBJVisibleTiles[S] = (257 - HPos + 7) >> 3;
				else
					GFX.OBJVisibleTiles[S] = GFX.OBJWidths[S] >> 3;

				for (uint8 line = startline, Y = (uint8) (PPU.OBJ[S].VPos & 0xff); line < Height; Y++, line += inc)
				{
					if (Y >= SNES_HEIGHT_EXTENDED)
						continue;

					if (PPU.OBJ[S].VFlip)
						// Yes, Width not Height. It so happens that the
						// sprites with H=2*W flip as two WxW sprites.
						OBJOnLine[Y][S] = (line ^ (GFX.OBJWidths[S] - 1)) | 0x80;
					else
						OBJOnLine[Y][S] = line | 0x80;
				}
			}
		}

		// Now go through and pull out those OBJ that are actually visible.
		int	j;
		for (int Y = 0; Y < SNES_HEIGHT_EXTENDED; Y++)
		{
			GFX.OBJLines[Y].RTOFlags = Y ? GFX.OBJLines[Y - 1].RTOFlags : 0;
			GFX.OBJLines[Y].Tiles = 34;

			uint8	FirstSprite = (PPU.FirstSprite + Y) & 0x7f;
			S = FirstSprite;
			j = 0;

			do
			{
				if (OBJOnLine[Y][S])
				{
					if (j >= 32)
					{
						GFX.OBJLines[Y].RTOFlags |= 0x40;
						break;
					}

					GFX.OBJLines[Y].Tiles -= GFX.OBJVisibleTiles[S];
					if (GFX.OBJLines[Y].Tiles < 0)
						GFX.OBJLines[Y].RTOFlags |= 0x80;
					GFX.OBJLines[Y].OBJ[j].Sprite = S;
					GFX.OBJLines[Y].OBJ[j++].Line = OBJOnLine[Y][S] & ~0x80;
				}

				S = (S + 1) & 0x7f;
			} while (S != FirstSprite);

			if (j < 32)
				GFX.OBJLines[Y].OBJ[j].Sprite = -1;
		}
	}

	IPPU.OBJChanged = FALSE;
}


static void DrawOBJS (int D)
{
	void (*DrawTile) (uint32, uint32, uint32, uint32) = NULL;
	void (*DrawClippedTile) (uint32, uint32, uint32, uint32, uint32, uint32) = NULL;

	int	PixWidth = IPPU.DoubleWidthPixels ? 2 : 1;
	BG.InterlaceLine = GFX.InterlaceFrame ? 8 : 0;
	GFX.Z1 = 2;

	for (uint32 Y = GFX.StartY, Offset = Y * GFX.PPL; Y <= GFX.EndY; Y++, Offset += GFX.PPL)
	{
		int	I = 0;
		int	tiles = GFX.OBJLines[Y].Tiles;

		for (int S = GFX.OBJLines[Y].OBJ[I].Sprite; S >= 0 && I < 32; S = GFX.OBJLines[Y].OBJ[++I].Sprite)
		{
			tiles += GFX.OBJVisibleTiles[S];
			if (tiles <= 0)
				continue;

			int	BaseTile = (((GFX.OBJLines[Y].OBJ[I].Line << 1) + (PPU.OBJ[S].Name & 0xf0)) & 0xf0) | (PPU.OBJ[S].Name & 0x100) | (PPU.OBJ[S].Palette << 10);
			int	TileX = PPU.OBJ[S].Name & 0x0f;
			int	TileLine = (GFX.OBJLines[Y].OBJ[I].Line & 7) * 8;
			int	TileInc = 1;

			if (PPU.OBJ[S].HFlip)
			{
				TileX = (TileX + (GFX.OBJWidths[S] >> 3) - 1) & 0x0f;
				BaseTile |= H_FLIP;
				TileInc = -1;
			}

			GFX.Z2 = D + PPU.OBJ[S].Priority * 4;

			int	DrawMode = 3;
			int	clip = 0, next_clip = -1000;
			int	X = PPU.OBJ[S].HPos;
			if (X == -256)
				X = 256;

			for (int t = tiles, O = Offset + X * PixWidth; X <= 256 && X < PPU.OBJ[S].HPos + GFX.OBJWidths[S]; TileX = (TileX + TileInc) & 0x0f, X += 8, O += 8 * PixWidth)
			{
				if (X < -7 || --t < 0 || X == 256)
					continue;

				for (int x = X; x < X + 8;)
				{
					if (x >= next_clip)
					{
						for (; clip < GFX.Clip[4].Count && GFX.Clip[4].Left[clip] <= x; clip++) ;
						if (clip == 0 || x >= GFX.Clip[4].Right[clip - 1])
						{
							DrawMode = 0;
							next_clip = ((clip < GFX.Clip[4].Count) ? GFX.Clip[4].Left[clip] : 1000);
						}
						else
						{
							DrawMode = GFX.Clip[4].DrawMode[clip - 1];
							next_clip = GFX.Clip[4].Right[clip - 1];
							GFX.ClipColors = !(DrawMode & 1);

							if (BG.EnableMath && (PPU.OBJ[S].Palette & 4) && (DrawMode & 2))
							{
								DrawTile = GFX.DrawTileMath;
								DrawClippedTile = GFX.DrawClippedTileMath;
							}
							else
							{
								DrawTile = GFX.DrawTileNomath;
								DrawClippedTile = GFX.DrawClippedTileNomath;
							}
						}
					}

					if (x == X && x + 8 < next_clip)
					{
						if (DrawMode)
							DrawTile(BaseTile | TileX, O, TileLine, 1);
						x += 8;
					}
					else
					{
						int	w = (next_clip <= X + 8) ? next_clip - x : X + 8 - x;
						if (DrawMode)
							DrawClippedTile(BaseTile | TileX, O, x - X, w, TileLine, 1);
						x += w;
					}
				}
			}
		}
	}
}

static void DrawBackground (int bg, uint8 Zh, uint8 Zl)
{
	BG.TileAddress = PPU.BG[bg].NameBase << 1;

	uint32	Tile;
	uint16	*SC0, *SC1, *SC2, *SC3;

	SC0 = (uint16 *) &Memory.VRAM[PPU.BG[bg].SCBase << 1];
	SC1 = (PPU.BG[bg].SCSize & 1) ? SC0 + 1024 : SC0;
	if (SC1 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC1 -= 0x8000;
	SC2 = (PPU.BG[bg].SCSize & 2) ? SC1 + 1024 : SC0;
	if (SC2 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC2 -= 0x8000;
	SC3 = (PPU.BG[bg].SCSize & 1) ? SC2 + 1024 : SC2;
	if (SC3 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC3 -= 0x8000;

	uint32	Lines;
	int		OffsetMask  = (BG.TileSizeH == 16) ? 0x3ff : 0x1ff;
	int		OffsetShift = (BG.TileSizeV == 16) ? 4 : 3;
	int		PixWidth = IPPU.DoubleWidthPixels ? 2 : 1;
	bool8		HiresInterlace = IPPU.Interlace && IPPU.DoubleWidthPixels;

	void (*DrawTile) (uint32, uint32, uint32, uint32);
	void (*DrawClippedTile) (uint32, uint32, uint32, uint32, uint32, uint32);

	for (int clip = 0; clip < GFX.Clip[bg].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[bg].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[bg].DrawMode[clip] & 2))
		{
			DrawTile = GFX.DrawTileMath;
			DrawClippedTile = GFX.DrawClippedTileMath;
		}
		else
		{
			DrawTile = GFX.DrawTileNomath;
			DrawClippedTile = GFX.DrawClippedTileNomath;
		}

		for (uint32 Y = GFX.StartY; Y <= GFX.EndY; Y += Lines)
		{
			uint32	Y2 = HiresInterlace ? Y * 2 + GFX.InterlaceFrame : Y;
			uint32	VOffset = LineData[Y].BG[bg].VOffset + (HiresInterlace ? 1 : 0);
			uint32	HOffset = LineData[Y].BG[bg].HOffset;
			int		VirtAlign = ((Y2 + VOffset) & 7) >> (HiresInterlace ? 1 : 0);

			for (Lines = 1; Lines < GFX.LinesPerTile - VirtAlign; Lines++)
			{
				if ((VOffset != LineData[Y + Lines].BG[bg].VOffset) || (HOffset != LineData[Y + Lines].BG[bg].HOffset))
					break;
			}

			if (Y + Lines > GFX.EndY)
				Lines = GFX.EndY - Y + 1;

			VirtAlign <<= 3;

			uint32	t1, t2;
			uint32	TilemapRow = (VOffset + Y2) >> OffsetShift;
			BG.InterlaceLine = ((VOffset + Y2) & 1) << 3;

			if ((VOffset + Y2) & 8)
			{
				t1 = 16;
				t2 = 0;
			}
			else
			{
				t1 = 0;
				t2 = 16;
			}

			uint16	*b1, *b2;

			if (TilemapRow & 0x20)
			{
				b1 = SC2;
				b2 = SC3;
			}
			else
			{
				b1 = SC0;
				b2 = SC1;
			}

			b1 += (TilemapRow & 0x1f) << 5;
			b2 += (TilemapRow & 0x1f) << 5;

			uint32	Left   = GFX.Clip[bg].Left[clip];
			uint32	Right  = GFX.Clip[bg].Right[clip];
			uint32	Offset = Left * PixWidth + Y * GFX.PPL;
			uint32	HPos   = (HOffset + Left) & OffsetMask;
			uint32	HTile  = HPos >> 3;
			uint16	*t;

			if (BG.TileSizeH == 8)
			{
				if (HTile > 31)
					t = b2 + (HTile & 0x1f);
				else
					t = b1 + HTile;
			}
			else
			{
				if (HTile > 63)
					t = b2 + ((HTile >> 1) & 0x1f);
				else
					t = b1 + (HTile >> 1);
			}

			uint32	Width = Right - Left;

			if (HPos & 7)
			{
				uint32	l = HPos & 7;
				uint32	w = 8 - l;
				if (w > Width)
					w = Width;

				Offset -= l * PixWidth;
				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
				{
					DrawClippedTile(Tile, Offset, l, w, VirtAlign, Lines);
					t++;
					if (HTile == 31)
						t = b2;
					else
					if (HTile == 63)
						t = b1;
				}
				else
				{
					if (!(Tile & H_FLIP))
						DrawClippedTile(TILE_PLUS(Tile, (HTile & 1)), Offset, l, w, VirtAlign, Lines);
					else
						DrawClippedTile(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, l, w, VirtAlign, Lines);
					t += HTile & 1;
					if (HTile == 63)
						t = b2;
					else
					if (HTile == 127)
						t = b1;
				}

				HTile++;
				Offset += 8 * PixWidth;
				Width -= w;
			}

			while (Width >= 8)
			{
				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
				{
					DrawTile(Tile, Offset, VirtAlign, Lines);
					t++;
					if (HTile == 31)
						t = b2;
					else
					if (HTile == 63)
						t = b1;
				}
				else
				{
					if (!(Tile & H_FLIP))
						DrawTile(TILE_PLUS(Tile, (HTile & 1)), Offset, VirtAlign, Lines);
					else
						DrawTile(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, VirtAlign, Lines);
					t += HTile & 1;
					if (HTile == 63)
						t = b2;
					else
					if (HTile == 127)
						t = b1;
				}

				HTile++;
				Offset += 8 * PixWidth;
				Width -= 8;
			}

			if (Width)
			{
				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
					DrawClippedTile(Tile, Offset, 0, Width, VirtAlign, Lines);
				else
				{
					if (!(Tile & H_FLIP))
						DrawClippedTile(TILE_PLUS(Tile, (HTile & 1)), Offset, 0, Width, VirtAlign, Lines);
					else
						DrawClippedTile(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, 0, Width, VirtAlign, Lines);
				}
			}
		}
	}
}

#define DRAW_BACKDROP_NO_MATH() \
	uint32	Offset = GFX.StartY * GFX.PPL; \
	for (int clip = 0; clip < GFX.Clip[5].Count; clip++) \
	{ \
		GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1); \
		GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]); \
	}

#define DrawBackdrop() \
	uint32	Offset = GFX.StartY * GFX.PPL; \
	for (int clip = 0; clip < GFX.Clip[5].Count; clip++) \
	{ \
		GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1); \
		\
		if (BG.EnableMath && (GFX.Clip[5].DrawMode[clip] & 2)) \
			GFX.DrawBackdropMath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]); \
		else \
			GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]); \
	}

static void DrawBackgroundMosaic (int bg, uint8 Zh, uint8 Zl)
{
	BG.TileAddress = PPU.BG[bg].NameBase << 1;

	uint32	Tile;
	uint16	*SC0, *SC1, *SC2, *SC3;

	SC0 = (uint16 *) &Memory.VRAM[PPU.BG[bg].SCBase << 1];
	SC1 = (PPU.BG[bg].SCSize & 1) ? SC0 + 1024 : SC0;
	if (SC1 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC1 -= 0x8000;
	SC2 = (PPU.BG[bg].SCSize & 2) ? SC1 + 1024 : SC0;
	if (SC2 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC2 -= 0x8000;
	SC3 = (PPU.BG[bg].SCSize & 1) ? SC2 + 1024 : SC2;
	if (SC3 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC3 -= 0x8000;

	int	Lines;
	int	OffsetMask  = (BG.TileSizeH == 16) ? 0x3ff : 0x1ff;
	int	OffsetShift = (BG.TileSizeV == 16) ? 4 : 3;
	int	PixWidth = IPPU.DoubleWidthPixels ? 2 : 1;
	bool8 HiresInterlace = IPPU.Interlace && IPPU.DoubleWidthPixels;

	void (*DrawPix) (uint32, uint32, uint32, uint32, uint32, uint32);

	int	MosaicStart = ((uint32) GFX.StartY - PPU.MosaicStart) % PPU.Mosaic;

	for (int clip = 0; clip < GFX.Clip[bg].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[bg].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[bg].DrawMode[clip] & 2))
			DrawPix = GFX.DrawMosaicPixelMath;
		else
			DrawPix = GFX.DrawMosaicPixelNomath;

		for (uint32 Y = GFX.StartY - MosaicStart; Y <= GFX.EndY; Y += PPU.Mosaic)
		{
			uint32	Y2 = HiresInterlace ? Y * 2 : Y;
			uint32	VOffset = LineData[Y].BG[bg].VOffset + (HiresInterlace ? 1 : 0);
			uint32	HOffset = LineData[Y].BG[bg].HOffset;

			Lines = PPU.Mosaic - MosaicStart;
			if (Y + MosaicStart + Lines > GFX.EndY)
				Lines = GFX.EndY - Y - MosaicStart + 1;

			int	VirtAlign = (((Y2 + VOffset) & 7) >> (HiresInterlace ? 1 : 0)) << 3;

			uint32	t1, t2;
			uint32	TilemapRow = (VOffset + Y2) >> OffsetShift;
			BG.InterlaceLine = ((VOffset + Y2) & 1) << 3;

			if ((VOffset + Y2) & 8)
			{
				t1 = 16;
				t2 = 0;
			}
			else
			{
				t1 = 0;
				t2 = 16;
			}

			uint16	*b1, *b2;

			if (TilemapRow & 0x20)
			{
				b1 = SC2;
				b2 = SC3;
			}
			else
			{
				b1 = SC0;
				b2 = SC1;
			}

			b1 += (TilemapRow & 0x1f) << 5;
			b2 += (TilemapRow & 0x1f) << 5;

			uint32	Left   = GFX.Clip[bg].Left[clip];
			uint32	Right  = GFX.Clip[bg].Right[clip];
			uint32	Offset = Left * PixWidth + (Y + MosaicStart) * GFX.PPL;
			uint32	HPos   = (HOffset + Left - (Left % PPU.Mosaic)) & OffsetMask;
			uint32	HTile  = HPos >> 3;
			uint16	*t;

			if (BG.TileSizeH == 8)
			{
				if (HTile > 31)
					t = b2 + (HTile & 0x1f);
				else
					t = b1 + HTile;
			}
			else
			{
				if (HTile > 63)
					t = b2 + ((HTile >> 1) & 0x1f);
				else
					t = b1 + (HTile >> 1);
			}

			uint32	Width = Right - Left;

			HPos &= 7;

			while (Left < Right)
			{
				uint32	w = PPU.Mosaic - (Left % PPU.Mosaic);
				if (w > Width)
					w = Width;

				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
					DrawPix(Tile, Offset, VirtAlign, HPos & 7, w, Lines);
				else
				{
					if (!(Tile & H_FLIP))
						DrawPix(TILE_PLUS(Tile, (HTile & 1)), Offset, VirtAlign, HPos & 7, w, Lines);
					else
						DrawPix(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, VirtAlign, HPos & 7, w, Lines);
				}

				HPos += PPU.Mosaic;

				while (HPos >= 8)
				{
					HPos -= 8;

					if (BG.TileSizeH == 8)
					{
						t++;
						if (HTile == 31)
							t = b2;
						else
						if (HTile == 63)
							t = b1;
					}
					else
					{
						t += HTile & 1;
						if (HTile == 63)
							t = b2;
						else
						if (HTile == 127)
							t = b1;
					}

					HTile++;
				}

				Offset += w * PixWidth;
				Width -= w;
				Left += w;
			}

			MosaicStart = 0;
		}
	}
}

static void DrawBackgroundOffset (int bg, uint8 Zh, uint8 Zl, int VOffOff)
{
	BG.TileAddress = PPU.BG[bg].NameBase << 1;

	uint32	Tile;
	uint16	*SC0, *SC1, *SC2, *SC3;
	uint16	*BPS0, *BPS1, *BPS2, *BPS3;

	BPS0 = (uint16 *) &Memory.VRAM[PPU.BG[2].SCBase << 1];
	BPS1 = (PPU.BG[2].SCSize & 1) ? BPS0 + 1024 : BPS0;
	if (BPS1 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS1 -= 0x8000;
	BPS2 = (PPU.BG[2].SCSize & 2) ? BPS1 + 1024 : BPS0;
	if (BPS2 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS2 -= 0x8000;
	BPS3 = (PPU.BG[2].SCSize & 1) ? BPS2 + 1024 : BPS2;
	if (BPS3 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS3 -= 0x8000;

	SC0 = (uint16 *) &Memory.VRAM[PPU.BG[bg].SCBase << 1];
	SC1 = (PPU.BG[bg].SCSize & 1) ? SC0 + 1024 : SC0;
	if (SC1 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC1 -= 0x8000;
	SC2 = (PPU.BG[bg].SCSize & 2) ? SC1 + 1024 : SC0;
	if (SC2 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC2 -= 0x8000;
	SC3 = (PPU.BG[bg].SCSize & 1) ? SC2 + 1024 : SC2;
	if (SC3 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC3 -= 0x8000;

	int	OffsetMask   = (BG.TileSizeH   == 16) ? 0x3ff : 0x1ff;
	int	OffsetShift  = (BG.TileSizeV   == 16) ? 4 : 3;
	int	Offset2Mask  = (BG.OffsetSizeH == 16) ? 0x3ff : 0x1ff;
	int	Offset2Shift = (BG.OffsetSizeV == 16) ? 4 : 3;
	int	OffsetEnableMask = 0x2000 << bg;
	int	PixWidth = IPPU.DoubleWidthPixels ? 2 : 1;
	bool8	HiresInterlace = IPPU.Interlace && IPPU.DoubleWidthPixels;

	void (*DrawClippedTile) (uint32, uint32, uint32, uint32, uint32, uint32);

	for (int clip = 0; clip < GFX.Clip[bg].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[bg].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[bg].DrawMode[clip] & 2))
			DrawClippedTile = GFX.DrawClippedTileMath;
		else
			DrawClippedTile = GFX.DrawClippedTileNomath;

		for (uint32 Y = GFX.StartY; Y <= GFX.EndY; Y++)
		{
			uint32	Y2 = HiresInterlace ? Y * 2 + GFX.InterlaceFrame : Y;
			uint32	VOff = LineData[Y].BG[2].VOffset - 1;
			uint32	HOff = LineData[Y].BG[2].HOffset;
			uint32	HOffsetRow = VOff >> Offset2Shift;
			uint32	VOffsetRow = (VOff + VOffOff) >> Offset2Shift;
			uint16	*s, *s1, *s2;

			if (HOffsetRow & 0x20)
			{
				s1 = BPS2;
				s2 = BPS3;
			}
			else
			{
				s1 = BPS0;
				s2 = BPS1;
			}

			s1 += (HOffsetRow & 0x1f) << 5;
			s2 += (HOffsetRow & 0x1f) << 5;
			s = ((VOffsetRow & 0x20) ? BPS2 : BPS0) + ((VOffsetRow & 0x1f) << 5);
			int32	VOffsetOffset = s - s1;

			uint32	Left  = GFX.Clip[bg].Left[clip];
			uint32	Right = GFX.Clip[bg].Right[clip];
			uint32	Offset = Left * PixWidth + Y * GFX.PPL;
			uint32	LineHOffset = LineData[Y].BG[bg].HOffset;
			bool8	left_edge = (Left < (8 - (LineHOffset & 7)));
			uint32	Width = Right - Left;

			while (Left < Right)
			{
				uint32	VOffset, HOffset;

				if (left_edge)
				{
					// SNES cannot do OPT for leftmost tile column
					VOffset = LineData[Y].BG[bg].VOffset;
					HOffset = LineHOffset;
					left_edge = FALSE;
				}
				else
				{
					int	HOffTile = ((HOff + Left - 1) & Offset2Mask) >> 3;

					if (BG.OffsetSizeH == 8)
					{
						if (HOffTile > 31)
							s = s2 + (HOffTile & 0x1f);
						else
							s = s1 + HOffTile;
					}
					else
					{
						if (HOffTile > 63)
							s = s2 + ((HOffTile >> 1) & 0x1f);
						else
							s = s1 + (HOffTile >> 1);
					}

					uint16	HCellOffset = READ_WORD(s);
					uint16	VCellOffset;

					if (VOffOff)
						VCellOffset = READ_WORD(s + VOffsetOffset);
					else
					{
						if (HCellOffset & 0x8000)
						{
							VCellOffset = HCellOffset;
							HCellOffset = 0;
						}
						else
							VCellOffset = 0;
					}

					if (VCellOffset & OffsetEnableMask)
						VOffset = VCellOffset + 1;
					else
						VOffset = LineData[Y].BG[bg].VOffset;

					if (HCellOffset & OffsetEnableMask)
						HOffset = (HCellOffset & ~7) | (LineHOffset & 7);
					else
						HOffset = LineHOffset;
				}

				if (HiresInterlace)
					VOffset++;

				uint32	t1, t2;
				int		VirtAlign = (((Y2 + VOffset) & 7) >> (HiresInterlace ? 1 : 0)) << 3;
				int		TilemapRow = (VOffset + Y2) >> OffsetShift;
				BG.InterlaceLine = ((VOffset + Y2) & 1) << 3;

				if ((VOffset + Y2) & 8)
				{
					t1 = 16;
					t2 = 0;
				}
				else
				{
					t1 = 0;
					t2 = 16;
				}

				uint16	*b1, *b2;

				if (TilemapRow & 0x20)
				{
					b1 = SC2;
					b2 = SC3;
				}
				else
				{
					b1 = SC0;
					b2 = SC1;
				}

				b1 += (TilemapRow & 0x1f) << 5;
				b2 += (TilemapRow & 0x1f) << 5;

				uint32	HPos = (HOffset + Left) & OffsetMask;
				uint32	HTile = HPos >> 3;
				uint16	*t;

				if (BG.TileSizeH == 8)
				{
					if (HTile > 31)
						t = b2 + (HTile & 0x1f);
					else
						t = b1 + HTile;
				}
				else
				{
					if (HTile > 63)
						t = b2 + ((HTile >> 1) & 0x1f);
					else
						t = b1 + (HTile >> 1);
				}

				uint32	l = HPos & 7;
				uint32	w = 8 - l;
				if (w > Width)
					w = Width;

				Offset -= l * PixWidth;
				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
				{
					DrawClippedTile(Tile, Offset, l, w, VirtAlign, 1);
				}
				else
				{
					if (!(Tile & H_FLIP))
						DrawClippedTile(TILE_PLUS(Tile, (HTile & 1)), Offset, l, w, VirtAlign, 1);
					else
						DrawClippedTile(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, l, w, VirtAlign, 1);
				}

				Left += w;
				Offset += 8 * PixWidth;
				Width -= w;
			}
		}
	}
}

static void DrawBackgroundOffsetMosaic (int bg, uint8 Zh, uint8 Zl, int VOffOff)
{
	BG.TileAddress = PPU.BG[bg].NameBase << 1;

	uint32	Tile;
	uint16	*SC0, *SC1, *SC2, *SC3;
	uint16	*BPS0, *BPS1, *BPS2, *BPS3;

	BPS0 = (uint16 *) &Memory.VRAM[PPU.BG[2].SCBase << 1];
	BPS1 = (PPU.BG[2].SCSize & 1) ? BPS0 + 1024 : BPS0;
	if (BPS1 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS1 -= 0x8000;
	BPS2 = (PPU.BG[2].SCSize & 2) ? BPS1 + 1024 : BPS0;
	if (BPS2 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS2 -= 0x8000;
	BPS3 = (PPU.BG[2].SCSize & 1) ? BPS2 + 1024 : BPS2;
	if (BPS3 >= (uint16 *) (Memory.VRAM + 0x10000))
		BPS3 -= 0x8000;

	SC0 = (uint16 *) &Memory.VRAM[PPU.BG[bg].SCBase << 1];
	SC1 = (PPU.BG[bg].SCSize & 1) ? SC0 + 1024 : SC0;
	if (SC1 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC1 -= 0x8000;
	SC2 = (PPU.BG[bg].SCSize & 2) ? SC1 + 1024 : SC0;
	if (SC2 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC2 -= 0x8000;
	SC3 = (PPU.BG[bg].SCSize & 1) ? SC2 + 1024 : SC2;
	if (SC3 >= (uint16 *) (Memory.VRAM + 0x10000))
		SC3 -= 0x8000;

	int	Lines;
	int	OffsetMask   = (BG.TileSizeH   == 16) ? 0x3ff : 0x1ff;
	int	OffsetShift  = (BG.TileSizeV   == 16) ? 4 : 3;
	int	Offset2Mask  = (BG.OffsetSizeH == 16) ? 0x3ff : 0x1ff;
	int	Offset2Shift = (BG.OffsetSizeV == 16) ? 4 : 3;
	int	OffsetEnableMask = 0x2000 << bg;
	int	PixWidth = IPPU.DoubleWidthPixels ? 2 : 1;
	bool8	HiresInterlace = IPPU.Interlace && IPPU.DoubleWidthPixels;

	void (*DrawPix) (uint32, uint32, uint32, uint32, uint32, uint32);

	int	MosaicStart = ((uint32) GFX.StartY - PPU.MosaicStart) % PPU.Mosaic;

	for (int clip = 0; clip < GFX.Clip[bg].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[bg].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[bg].DrawMode[clip] & 2))
			DrawPix = GFX.DrawMosaicPixelMath;
		else
			DrawPix = GFX.DrawMosaicPixelNomath;

		for (uint32 Y = GFX.StartY - MosaicStart; Y <= GFX.EndY; Y += PPU.Mosaic)
		{
			uint32	Y2 = HiresInterlace ? Y * 2 : Y;
			uint32	VOff = LineData[Y].BG[2].VOffset - 1;
			uint32	HOff = LineData[Y].BG[2].HOffset;

			Lines = PPU.Mosaic - MosaicStart;
			if (Y + MosaicStart + Lines > GFX.EndY)
				Lines = GFX.EndY - Y - MosaicStart + 1;

			uint32	HOffsetRow = VOff >> Offset2Shift;
			uint32	VOffsetRow = (VOff + VOffOff) >> Offset2Shift;
			uint16	*s, *s1, *s2;

			if (HOffsetRow & 0x20)
			{
				s1 = BPS2;
				s2 = BPS3;
			}
			else
			{
				s1 = BPS0;
				s2 = BPS1;
			}

			s1 += (HOffsetRow & 0x1f) << 5;
			s2 += (HOffsetRow & 0x1f) << 5;
			s = ((VOffsetRow & 0x20) ? BPS2 : BPS0) + ((VOffsetRow & 0x1f) << 5);
			int32	VOffsetOffset = s - s1;

			uint32	Left =  GFX.Clip[bg].Left[clip];
			uint32	Right = GFX.Clip[bg].Right[clip];
			uint32	Offset = Left * PixWidth + (Y + MosaicStart) * GFX.PPL;
			uint32	LineHOffset = LineData[Y].BG[bg].HOffset;
			bool8	left_edge = (Left < (8 - (LineHOffset & 7)));
			uint32	Width = Right - Left;

			while (Left < Right)
			{
				uint32	VOffset, HOffset;

				if (left_edge)
				{
					// SNES cannot do OPT for leftmost tile column
					VOffset = LineData[Y].BG[bg].VOffset;
					HOffset = LineHOffset;
					left_edge = FALSE;
				}
				else
				{
					int	HOffTile = ((HOff + Left - 1) & Offset2Mask) >> 3;

					if (BG.OffsetSizeH == 8)
					{
						if (HOffTile > 31)
							s = s2 + (HOffTile & 0x1f);
						else
							s = s1 + HOffTile;
					}
					else
					{
						if (HOffTile > 63)
							s = s2 + ((HOffTile >> 1) & 0x1f);
						else
							s = s1 + (HOffTile >> 1);
					}

					uint16	HCellOffset = READ_WORD(s);
					uint16	VCellOffset;

					if (VOffOff)
						VCellOffset = READ_WORD(s + VOffsetOffset);
					else
					{
						if (HCellOffset & 0x8000)
						{
							VCellOffset = HCellOffset;
							HCellOffset = 0;
						}
						else
							VCellOffset = 0;
					}

					if (VCellOffset & OffsetEnableMask)
						VOffset = VCellOffset + 1;
					else
						VOffset = LineData[Y].BG[bg].VOffset;

					if (HCellOffset & OffsetEnableMask)
						HOffset = (HCellOffset & ~7) | (LineHOffset & 7);
					else
						HOffset = LineHOffset;
				}

				if (HiresInterlace)
					VOffset++;

				uint32	t1, t2;
				int		VirtAlign = (((Y2 + VOffset) & 7) >> (HiresInterlace ? 1 : 0)) << 3;
				int		TilemapRow = (VOffset + Y2) >> OffsetShift;
				BG.InterlaceLine = ((VOffset + Y2) & 1) << 3;

				if ((VOffset + Y2) & 8)
				{
					t1 = 16;
					t2 = 0;
				}
				else
				{
					t1 = 0;
					t2 = 16;
				}

				uint16	*b1, *b2;

				if (TilemapRow & 0x20)
				{
					b1 = SC2;
					b2 = SC3;
				}
				else
				{
					b1 = SC0;
					b2 = SC1;
				}

				b1 += (TilemapRow & 0x1f) << 5;
				b2 += (TilemapRow & 0x1f) << 5;

				uint32	HPos = (HOffset + Left - (Left % PPU.Mosaic)) & OffsetMask;
				uint32	HTile = HPos >> 3;
				uint16	*t;

				if (BG.TileSizeH == 8)
				{
					if (HTile > 31)
						t = b2 + (HTile & 0x1f);
					else
						t = b1 + HTile;
				}
				else
				{
					if (HTile > 63)
						t = b2 + ((HTile >> 1) & 0x1f);
					else
						t = b1 + (HTile >> 1);
				}

				uint32	w = PPU.Mosaic - (Left % PPU.Mosaic);
				if (w > Width)
					w = Width;

				Tile = READ_WORD(t);
				GFX.Z1 = GFX.Z2 = (Tile & 0x2000) ? Zh : Zl;

				if (BG.TileSizeV == 16)
					Tile = TILE_PLUS(Tile, ((Tile & V_FLIP) ? t2 : t1));

				if (BG.TileSizeH == 8)
					DrawPix(Tile, Offset, VirtAlign, HPos & 7, w, Lines);
				else
				{
					if (!(Tile & H_FLIP))
						DrawPix(TILE_PLUS(Tile, (HTile & 1)), Offset, VirtAlign, HPos & 7, w, Lines);
					else
					if (!(Tile & V_FLIP))
						DrawPix(TILE_PLUS(Tile, 1 - (HTile & 1)), Offset, VirtAlign, HPos & 7, w, Lines);
				}

				Left += w;
				Offset += w * PixWidth;
				Width -= w;
			}

			MosaicStart = 0;
		}
	}
}

static inline void DrawBackgroundMode7 (int bg, void (*DrawMath) (uint32, uint32, int), void (*DrawNomath) (uint32, uint32, int), int D)
{
	for (int clip = 0; clip < GFX.Clip[bg].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[bg].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[bg].DrawMode[clip] & 2))
			DrawMath(GFX.Clip[bg].Left[clip], GFX.Clip[bg].Right[clip], D);
		else
			DrawNomath(GFX.Clip[bg].Left[clip], GFX.Clip[bg].Right[clip], D);
	}
}

static inline void RenderScreen_SFXSpeedupHack()
{
	uint8	BGActive;
	int		D;

	GFX.S = GFX.SubScreen;
	GFX.DB = GFX.SubZBuffer;
	GFX.Clip = IPPU.Clip[1];
	BGActive = Memory.FillRAM[0x212d];
	D = (Memory.FillRAM[0x2130] & 2) << 4; // 'do math' depth flag

	if (BGActive & 0x10)
	{
		BG.TileAddress = PPU.OBJNameBase;
		BG.NameSelect = PPU.OBJNameSelect;
		BG.EnableMath = 0;
		BG.StartPalette = 128;
		S9xSelectTileConverter_SFXDepth4();
		S9xSelectTileRenderers_SFXSpeedup();
		DrawOBJS(D + 4);
	}

	BG.NameSelect = 0;
	S9xSelectTileRenderers_SFXSpeedup();

	#define DO_BG_DEPTH2(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = 0; \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth2(); \
			DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_DEPTH4(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = 0; \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			if (offset) \
			{ \
				BG.OffsetSizeH = BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			} \
			else \
			{ \
				DrawBackground(n, D + Zh, D + Zl); \
			} \
		}

	#define DO_BG_DEPTH4_OFFSET0(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = 0; \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_DEPTH4_OFFSET1(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = 0; \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			BG.OffsetSizeH = BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
		}

	switch (PPU.BGMode)
	{
		case 1:
			DO_BG_DEPTH4_OFFSET0(0,  0, 4, FALSE, FALSE, 15, 11, 0);
			DO_BG_DEPTH4_OFFSET0(1,  0, 4, FALSE, FALSE, 14, 10, 0);
			DO_BG_DEPTH2(2,  0, 2, FALSE, FALSE, (PPU.BG3Priority ? 17 : 7), 3, 0);
			break;

		case 2:
			DO_BG_DEPTH4_OFFSET1(0,  0, 4, FALSE, TRUE,  15,  7, 8);
			DO_BG_DEPTH4_OFFSET1(1,  0, 4, FALSE, TRUE,  11,  3, 8);
			break;
	}

	#undef DO_BG_DEPTH2
	#undef DO_BG_DEPTH4
	#undef DO_BG_DEPTH4_OFFSET0
	#undef DO_BG_DEPTH4_OFFSET1

	BG.EnableMath = 0;

	//ghetto inline of DrawBackdrop - no math - 
	//END - ghetto inline of DrawBackdrop - no math

	BGActive = 0;
	D = 0;

	GFX.S = GFX.Screen;
	GFX.DB = GFX.ZBuffer;
	GFX.Clip = IPPU.Clip[0];
	BGActive = Memory.FillRAM[0x212c];
	D = 32;

	if (BGActive & 0x10)
	{
		BG.TileAddress = PPU.OBJNameBase;
		BG.NameSelect = PPU.OBJNameSelect;
		BG.EnableMath = (Memory.FillRAM[0x2131] & 0x10);
		BG.StartPalette = 128;
		S9xSelectTileConverter_SFXDepth4();
		S9xSelectTileRenderers_SFXSpeedup();
		DrawOBJS(D + 4);
	}

	BG.NameSelect = 0;
	S9xSelectTileRenderers_SFXSpeedup();

	#define DO_BG_DEPTH2(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth2(); \
			DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_DEPTH4(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			if (offset) \
			{ \
				BG.OffsetSizeH = BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
				DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
			} \
			else \
			{ \
				DrawBackground(n, D + Zh, D + Zl); \
			} \
		}

	#define DO_BG_DEPTH4_OFFSET0(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_DEPTH4_OFFSET1(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_SFXDepth4(); \
			BG.OffsetSizeH = BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

	switch (PPU.BGMode)
	{
		case 1:
			DO_BG_DEPTH4_OFFSET0(0,  0, 4, FALSE, FALSE, 15, 11, 0);
			DO_BG_DEPTH4_OFFSET0(1,  0, 4, FALSE, FALSE, 14, 10, 0);
			DO_BG_DEPTH2(2,  0, 2, FALSE, FALSE, (PPU.BG3Priority ? 17 : 7), 3, 0);
			break;

		case 2:
			DO_BG_DEPTH4_OFFSET1(0,  0, 4, FALSE, TRUE,  15,  7, 8);
			DO_BG_DEPTH4_OFFSET1(1,  0, 4, FALSE, TRUE,  11,  3, 8);
			break;
	}

	#undef DO_BG_DEPTH2
	#undef DO_BG_DEPTH4
	#undef DO_BG_DEPTH4_OFFSET0
	#undef DO_BG_DEPTH4_OFFSET1

	BG.EnableMath = (Memory.FillRAM[0x2131] & 0x20);
}

static inline void RenderScreen (bool8 sub)
{
	uint8	BGActive;
	int		D;

	if (!sub)
	{
		GFX.S = GFX.Screen;
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;
		GFX.DB = GFX.ZBuffer;
		GFX.Clip = IPPU.Clip[0];
		BGActive = Memory.FillRAM[0x212c];
		D = 32;
	}
	else
	{
		GFX.S = GFX.SubScreen;
		GFX.DB = GFX.SubZBuffer;
		GFX.Clip = IPPU.Clip[1];
		BGActive = Memory.FillRAM[0x212d];
		D = (Memory.FillRAM[0x2130] & 2) << 4; // 'do math' depth flag
	}

	if (BGActive & 0x10)
	{
		BG.TileAddress = PPU.OBJNameBase;
		BG.NameSelect = PPU.OBJNameSelect;
		BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & 0x10);
		BG.StartPalette = 128;
		S9xSelectTileConverter(4, FALSE, sub, FALSE);
		S9xSelectTileRenderers(PPU.BGMode, sub, TRUE);
		DrawOBJS(D + 4);
	}

	BG.NameSelect = 0;
	S9xSelectTileRenderers(PPU.BGMode, sub, FALSE);

	#define DO_BG(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (!hires && PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter(depth, hires, sub, PPU.BGMosaic[n]); \
			\
			if (offset) \
			{ \
				BG.OffsetSizeH = (!hires && PPU.BG[2].BGSize) ? 16 : 8; \
				BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
				\
				if (PPU.BGMosaic[n] && (hires || PPU.Mosaic > 1) && PPU.DisableMosaicHack) \
					DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
				else \
					DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
			} \
			else \
			{ \
				if (PPU.BGMosaic[n] && (hires || PPU.Mosaic > 1) && PPU.DisableMosaicHack) \
					DrawBackgroundMosaic(n, D + Zh, D + Zl); \
				else \
					DrawBackground(n, D + Zh, D + Zl); \
			} \
		}

	#define DO_BG_HIRES0_OFFSET0(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter(depth, hires, sub, PPU.BGMosaic[n]); \
			\
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1) && PPU.DisableMosaicHack) \
				DrawBackgroundMosaic(n, D + Zh, D + Zl); \
			else \
				DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_HIRES0_OFFSET1(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter(depth, 0, sub, PPU.BGMosaic[n]); \
			\
			BG.OffsetSizeH = (PPU.BG[2].BGSize) ? 16 : 8; \
			BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			\
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1) && PPU.DisableMosaicHack) \
			DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
			else \
			DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

	#define DO_BG_HIRES1_OFFSET0(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter(depth, 1, sub, PPU.BGMosaic[n]); \
				if (PPU.BGMosaic[n] && PPU.DisableMosaicHack) \
					DrawBackgroundMosaic(n, D + Zh, D + Zl); \
				else \
					DrawBackground(n, D + Zh, D + Zl); \
		}

	#define DO_BG_HIRES1_OFFSET1(n, pal, depth, hires, offset, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter(depth, hires, sub, PPU.BGMosaic[n]); \
			\
				BG.OffsetSizeH = 8; \
				BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
				\
				if (PPU.BGMosaic[n] && PPU.DisableMosaicHack) \
					DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
				else \
					DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

	switch (PPU.BGMode)
	{
		case 0:
			DO_BG_HIRES0_OFFSET0(0,  0, 2, FALSE, FALSE, 15, 11, 0);
			DO_BG_HIRES0_OFFSET0(1, 32, 2, FALSE, FALSE, 14, 10, 0);
			DO_BG_HIRES0_OFFSET0(2, 64, 2, FALSE, FALSE,  7,  3, 0);
			DO_BG_HIRES0_OFFSET0(3, 96, 2, FALSE, FALSE,  6,  2, 0);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #0\n");
			#endif
			break;

		case 1:
			DO_BG_HIRES0_OFFSET0(0,  0, 4, FALSE, FALSE, 15, 11, 0);
			DO_BG_HIRES0_OFFSET0(1,  0, 4, FALSE, FALSE, 14, 10, 0);
			DO_BG_HIRES0_OFFSET0(2,  0, 2, FALSE, FALSE, (PPU.BG3Priority ? 17 : 7), 3, 0);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #1\n");
			#endif
			break;

		case 2:
			DO_BG_HIRES0_OFFSET1(0,  0, 4, FALSE, TRUE,  15,  7, 8);
			DO_BG_HIRES0_OFFSET1(1,  0, 4, FALSE, TRUE,  11,  3, 8);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #2\n");
			#endif
			break;

		case 3:
			DO_BG_HIRES0_OFFSET0(0,  0, 8, FALSE, FALSE, 15,  7, 0);
			DO_BG_HIRES0_OFFSET0(1,  0, 4, FALSE, FALSE, 11,  3, 0);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #3\n");
			#endif
			break;

		case 4:
			DO_BG_HIRES0_OFFSET1(0,  0, 8, FALSE, TRUE,  15,  7, 0);
			DO_BG_HIRES0_OFFSET1(1,  0, 2, FALSE, TRUE,  11,  3, 0);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #4\n");
			#endif
			break;

		case 5:
			DO_BG_HIRES1_OFFSET0(0,  0, 4, TRUE,  FALSE, 15,  7, 0);
			DO_BG_HIRES1_OFFSET0(1,  0, 2, TRUE,  FALSE, 11,  3, 0);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #5\n");
			#endif
			break;

		case 6:
			DO_BG_HIRES1_OFFSET1(0,  0, 4, TRUE,  TRUE,  15,  7, 8);
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #6\n");
			#endif
			break;

		case 7:
			if (BGActive & 0x01)
			{
				BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & 1);
				DrawBackgroundMode7(0, GFX.DrawMode7BG1Math, GFX.DrawMode7BG1Nomath, D);
			}

			if ((Memory.FillRAM[0x2133] & 0x40) && (BGActive & 0x02))
			{
				BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & 2);
				DrawBackgroundMode7(1, GFX.DrawMode7BG2Math, GFX.DrawMode7BG2Nomath, D);
			}
			#ifdef REPORT_MODES
			fprintf(stderr, "MODE: #7\n");
			#endif
			break;
	}

	#undef DO_BG
	#undef DO_BG_HIRES0_OFFSET0
	#undef DO_BG_HIRES0_OFFSET1
	#undef DO_BG_HIRES1_OFFSET0
	#undef DO_BG_HIRES1_OFFSET1

	BG.EnableMath = !sub && (Memory.FillRAM[0x2131] & 0x20);
}

static inline uint8 CalcWindowMask (int i, uint8 W1, uint8 W2)
{
	if (!PPU.ClipWindow1Enable[i])
	{
		if (!PPU.ClipWindow2Enable[i])
			return (0);
		else
		{
			if (!PPU.ClipWindow2Inside[i])
				return (~W2);
			return (W2);
		}
	}
	else
	{
		if (!PPU.ClipWindow2Enable[i])
		{
			if (!PPU.ClipWindow1Inside[i])
				return (~W1);
			return (W1);
		}
		else
		{
			if (!PPU.ClipWindow1Inside[i])
				W1 = ~W1;
			if (!PPU.ClipWindow2Inside[i])
				W2 = ~W2;

			switch (PPU.ClipWindowOverlapLogic[i])
			{
				case 0: // OR
					return (W1 | W2);

				case 1: // AND
					return (W1 & W2);

				case 2: // XOR
					return (W1 ^ W2);

				case 3: // XNOR
					return (~(W1 ^ W2));
			}
		}
	}

	// Never get here
	return (0);
}

#if 0
static inline void StoreWindowRegions (uint8 Mask, struct ClipData *Clip, int n_regions, int16 *windows, uint8 *drawing_modes, bool8 sub, bool8 StoreMode0)
{
	int	ct = 0;

	for (int j = 0; j < n_regions; j++)
	{
		int	DrawMode = drawing_modes[j];
		if (sub)
			DrawMode |= 1;
		if (Mask & (1 << j))
			DrawMode = 0;

		if (!StoreMode0 && !DrawMode)
			continue;

		if (ct > 0 && Clip->Right[ct - 1] == windows[j] && Clip->DrawMode[ct - 1] == DrawMode)
			Clip->Right[ct - 1] = windows[j + 1]; // This region borders with and has the same drawing mode as the previous region: merge them.
		else
		{
			// Add a new region to the BG
			Clip->Left[ct]     = windows[j];
			Clip->Right[ct]    = windows[j + 1];
			Clip->DrawMode[ct] = DrawMode;
			ct++;
		}
	}

	Clip->Count = ct;
}
#endif

#define StoreWindowRegions_StoreMode1_Mask0(Clip, Clip2) \
{ \
	int	ct = 0; \
	int	ct2 = 0; \
	int	k = 0; \
	do{ \
		int	DrawMode = drawing_modes[k]; \
		int	DrawMode2 = drawing_modes[k]; \
		DrawMode2 |= 1; \
		\
		if (ct > 0 && Clip.Right[ct - 1] == windows[k] && Clip.DrawMode[ct - 1] == DrawMode) \
			Clip.Right[ct - 1] = windows[k + 1]; /* This region borders with and has the same drawing mode as the previous region: merge them. */ \
		else \
		{ \
			/* Add a new region to the BG */ \
			Clip.Left[ct]     = windows[k]; \
			Clip.Right[ct]    = windows[k + 1]; \
			Clip.DrawMode[ct] = DrawMode; \
			ct++; \
		} \
		\
		if (ct2 > 0 && Clip2.Right[ct2 - 1] == windows[k] && Clip2.DrawMode[ct2 - 1] == DrawMode) \
			Clip2.Right[ct2 - 1] = windows[k + 1]; /* This region borders with and has the same drawing mode as the previous region: merge them. */ \
		else \
		{ \
			/* Add a new region to the BG */ \
			Clip2.Left[ct2]     = windows[k]; \
			Clip2.Right[ct2]    = windows[k + 1]; \
			Clip2.DrawMode[ct2] = DrawMode2; \
			ct2++; \
		} \
		k++; \
	}while(k < n_regions); \
	Clip.Count = ct; \
	Clip2.Count = ct2; \
}

#define StoreWindowRegions_Sub0_StoreMode0(Mask, Clip) \
{ \
	int	ct = 0; \
	int	k = 0; \
	\
	do{ \
		int	DrawMode = drawing_modes[k]; \
		if (Mask & (1 << k)) \
			DrawMode = 0; \
		if (DrawMode) \
		{ \
			if (ct > 0 && Clip.Right[ct - 1] == windows[k] && Clip.DrawMode[ct - 1] == DrawMode) \
			Clip.Right[ct - 1] = windows[k + 1]; /* This region borders with and has the same drawing mode as the previous region: merge them. */ \
			else \
			{ \
				/* Add a new region to the BG */ \
				Clip.Left[ct]     = windows[k]; \
				Clip.Right[ct]    = windows[k + 1]; \
				Clip.DrawMode[ct] = DrawMode; \
				ct++; \
			} \
		} \
		k++; \
	}while(k < n_regions); \
	\
	Clip.Count = ct; \
}

#define StoreWindowRegions_Sub1_StoreMode0(Mask, Clip) \
{ \
	int	ct = 0; \
	int	k = 0; \
	\
	do \
	{ \
		int	DrawMode = drawing_modes[k]; \
		DrawMode |= 1; \
		if (Mask & (1 << k)) \
			DrawMode = 0; \
		\
		if(DrawMode) \
		{ \
			if (ct > 0 && Clip.Right[ct - 1] == windows[k] && Clip.DrawMode[ct - 1] == DrawMode) \
				Clip.Right[ct - 1] = windows[k + 1]; /* This region borders with and has the same drawing mode as the previous region: merge them. */ \
			else \
			{ \
				/* Add a new region to the BG */ \
					Clip.Left[ct]     = windows[k]; \
					Clip.Right[ct]    = windows[k + 1]; \
					Clip.DrawMode[ct] = DrawMode; \
					ct++; \
			} \
		} \
		k++; \
	}while(k < n_regions); \
	Clip.Count = ct; \
}

static uint8	region_map[6][6] =
{
	{ 0, 0x01, 0x03, 0x07, 0x0f, 0x1f },
	{ 0,    0, 0x02, 0x06, 0x0e, 0x1e },
	{ 0,    0,    0, 0x04, 0x0c, 0x1c },
	{ 0,    0,    0,    0, 0x08, 0x18 },
	{ 0,    0,    0,    0,    0, 0x10 }
};

static void S9xComputeClipWindows (void)
{
	int16	windows[6] = { 0, 256, 256, 256, 256, 256 };
	uint8	drawing_modes[5] = { 0, 0, 0, 0, 0 };
	int		n_regions = 1;
	int		i, j;

	// Calculate window regions. We have at most 5 regions, because we have 6 control points
	// (screen edges, window 1 left & right, and window 2 left & right).

	if (PPU.Window1Left <= PPU.Window1Right)
	{
		if (PPU.Window1Left > 0)
		{
			windows[1] = PPU.Window1Left;
			n_regions = 2;
		}

		if (PPU.Window1Right < 255)
		{
			windows[n_regions] = PPU.Window1Right + 1;
			n_regions++;
		}
	}

	if (PPU.Window2Left <= PPU.Window2Right)
	{
		for (i = 0; i <= n_regions; i++)
		{
			if (PPU.Window2Left == windows[i])
				break;

			if (PPU.Window2Left <  windows[i])
			{
				for (j = n_regions; j >= i; j--)
					windows[j + 1] = windows[j];

				windows[i] = PPU.Window2Left;
				n_regions++;
				break;
			}
		}

		for (; i <= n_regions; i++)
		{
			if (PPU.Window2Right + 1 == windows[i])
				break;

			if (PPU.Window2Right + 1 <  windows[i])
			{
				for (j = n_regions; j >= i; j--)
					windows[j + 1] = windows[j];

				windows[i] = PPU.Window2Right + 1;
				n_regions++;
				break;
			}
		}
	}

	// Get a bitmap of which regions correspond to each window.

	uint8	W1 = 0, W2 = 0;

	if (PPU.Window1Left <= PPU.Window1Right)
	{
		for (i = 0; windows[i] != PPU.Window1Left; i++) ;
		for (j = i; windows[j] != PPU.Window1Right + 1; j++) ;
		W1 = region_map[i][j];
	}

	if (PPU.Window2Left <= PPU.Window2Right)
	{
		for (i = 0; windows[i] != PPU.Window2Left; i++) ;
		for (j = i; windows[j] != PPU.Window2Right + 1; j++) ;
		W2 = region_map[i][j];
	}

	// Color Window affects the drawing mode for each region.
	// Modes are: 3=Draw as normal, 2=clip color (math only), 1=no math (draw only), 0=nothing.

	uint8	CW_color = 0, CW_math = 0;
	uint8	CW = CalcWindowMask(5, W1, W2);

	switch (Memory.FillRAM[0x2130] & 0xc0)
	{
		case 0x40:	CW_color = ~CW;		break;
		case 0x80:	CW_color = CW;		break;
		case 0xc0:	CW_color = 0xff;	break;
	}

	switch (Memory.FillRAM[0x2130] & 0x30)
	{
		case 0x10:	CW_math  = ~CW;		break;
		case 0x20:	CW_math  = CW;		break;
		case 0x30:	CW_math  = 0xff;	break;
	}

	for (i = 0; i < n_regions; i++)
	{
		if (!(CW_color & (1 << i)))
			drawing_modes[i] |= 1;
		if (!(CW_math  & (1 << i)))
			drawing_modes[i] |= 2;
	}

	// Store backdrop clip window (draw everywhere color window allows)

	if(PPU.FullClipping)
	{
		StoreWindowRegions_StoreMode1_Mask0(IPPU.Clip[0][5], IPPU.Clip[1][5]);
	}

	// Store per-BG and OBJ clip windows

	for (j = 0; j < 5; j++)
	{
		uint8	W = CalcWindowMask(j, W1, W2);
		uint8 mask_a = 0;
		uint8 mask_b = 0;

		if (Memory.FillRAM[0x212e] & (1 << j))
			mask_a = W;
		if (Memory.FillRAM[0x212f] & (1 << j))
			mask_b = W;
		
		StoreWindowRegions_Sub0_StoreMode0(mask_a, IPPU.Clip[0][j]);
		StoreWindowRegions_Sub1_StoreMode0(mask_b, IPPU.Clip[1][j]);
	}
}

void S9xUpdateScreen (void)
{
	if (IPPU.OBJChanged || IPPU.InterlaceOBJ)
		SetupOBJ();

	// XXX: Check ForceBlank? Or anything else?
	PPU.RangeTimeOver |= GFX.OBJLines[GFX.EndY].RTOFlags;

	GFX.StartY = IPPU.PreviousLine;
	if ((GFX.EndY = IPPU.CurrentLine - 1) >= PPU.ScreenHeight)
		GFX.EndY = PPU.ScreenHeight - 1;

	if (!PPU.ForcedBlanking)
	{
		// If force blank, may as well completely skip all this. We only did
		// the OBJ because (AFAWK) the RTO flags are updated even during force-blank.

		if (PPU.RecomputeClipWindows)
		{
			S9xComputeClipWindows();
			PPU.RecomputeClipWindows = FALSE;
		}

		if (!IPPU.DoubleWidthPixels && (PPU.BGMode == 5 || PPU.BGMode == 6 || IPPU.PseudoHires))
		{
			// Have to back out of the regular speed hack
			for (register uint32 y = 0; y < GFX.StartY; y++)
			{
				register uint16	*p = GFX.Screen + y * GFX.PPL + 255;
				register uint16	*q = GFX.Screen + y * GFX.PPL + 510;

				for (register int x = 255; x >= 0; x--, p--, q -= 2)
					*q = *(q + 1) = *p;
			}

			IPPU.DoubleWidthPixels = TRUE;
			IPPU.RenderedScreenWidth = 512;
		}

		if (!IPPU.DoubleHeightPixels && IPPU.Interlace && (PPU.BGMode == 5 || PPU.BGMode == 6))
		{
			IPPU.DoubleHeightPixels = TRUE;
			IPPU.RenderedScreenHeight = PPU.ScreenHeight << 1;
			GFX.PPL = GFX.RealPPL << 1;
			GFX.DoInterlace = 2;

			for (register int32 y = (int32) GFX.StartY - 1; y >= 0; y--)
				memmove(GFX.Screen + y * GFX.PPL, GFX.Screen + y * GFX.RealPPL, IPPU.RenderedScreenWidth * sizeof(uint16));
		}

		if ((Memory.FillRAM[0x2130] & 0x30) != 0x30 && (Memory.FillRAM[0x2131] & 0x3f))
			GFX.FixedColour = BUILD_PIXEL(IPPU.XB[PPU.FixedColourRed], IPPU.XB[PPU.FixedColourGreen], IPPU.XB[PPU.FixedColourBlue]);

		if(!PPU.SFXSpeedupHack)
		{
			if (PPU.BGMode == 5 || PPU.BGMode == 6 || IPPU.PseudoHires ||
					((Memory.FillRAM[0x2130] & 0x30) != 0x30 && (Memory.FillRAM[0x2130] & 2) && (Memory.FillRAM[0x2131] & 0x3f) && (Memory.FillRAM[0x212d] & 0x1f)))
			{
				// If hires (Mode 5/6 or pseudo-hires) or math is to be done
				// involving the subscreen, then we need to render the subscreen...
				RenderScreen(TRUE);
				if(PPU.RenderSub)
				{
					#ifdef REPORT_MODES
					fprintf(stderr, "Subscreen drawing: Frame #%d\n", counter++);
					#endif
					DRAW_BACKDROP_NO_MATH();
				}
			}

			RenderScreen(FALSE);
		}
		else
			RenderScreen_SFXSpeedupHack();
		DrawBackdrop();
	}
	else
	{
		GFX.S = GFX.Screen + GFX.StartY * GFX.PPL;
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;

		for (uint32 l = GFX.StartY; l <= GFX.EndY; l++, GFX.S += GFX.PPL)
			memset(GFX.S, 0, IPPU.RenderedScreenWidth * sizeof(int));
	}

	IPPU.PreviousLine = IPPU.CurrentLine;
}

static uint16 get_crosshair_color (uint8 color)
{
	switch (color & 15)
	{
		case  0: return (BUILD_PIXEL( 0,  0,  0)); // transparent, shouldn't be used
		case  1: return (BUILD_PIXEL( 0,  0,  0)); // Black
		case  2: return (BUILD_PIXEL( 8,  8,  8)); // 25Grey
		case  3: return (BUILD_PIXEL(16, 16, 16)); // 50Grey
		case  4: return (BUILD_PIXEL(23, 23, 23)); // 75Grey
		case  5: return (BUILD_PIXEL(31, 31, 31)); // White
		case  6: return (BUILD_PIXEL(31,  0,  0)); // Red
		case  7: return (BUILD_PIXEL(31, 16,  0)); // Orange
		case  8: return (BUILD_PIXEL(31, 31,  0)); // Yellow
		case  9: return (BUILD_PIXEL( 0, 31,  0)); // Green
		case 10: return (BUILD_PIXEL( 0, 31, 31)); // Cyan
		case 11: return (BUILD_PIXEL( 0, 23, 31)); // Sky
		case 12: return (BUILD_PIXEL( 0,  0, 31)); // Blue
		case 13: return (BUILD_PIXEL(23,  0, 31)); // Violet
		case 14: return (BUILD_PIXEL(31,  0, 31)); // Magenta
		case 15: return (BUILD_PIXEL(31,  0, 16)); // Purple
	}

	return (0);
}

void S9xDrawCrosshair (const char *crosshair, uint8 fgcolor, uint8 bgcolor, int16 x, int16 y)
{
	if (!crosshair)
		return;

	int16 r, rx = 1, c, cx = 1, W = SNES_WIDTH, H = PPU.ScreenHeight;
	uint16 fg, bg;

	x -= 7;
	y -= 7;

	if (IPPU.DoubleWidthPixels) { cx = 2; x *= 2; W *= 2; }
	if (IPPU.DoubleHeightPixels) { rx = 2; y *= 2; H *= 2; }

	fg = get_crosshair_color(fgcolor);
	bg = get_crosshair_color(bgcolor);

	uint16 *s = GFX.Screen + y * (int32)GFX.RealPPL + x;

	for (r = 0; r < 15 * rx; r++, s += GFX.RealPPL - 15 * cx)
	{
		if (y + r < 0)
		{
			s += 15 * cx;
			continue;
		}

		if (y + r >= H)
			break;

		for (c = 0; c < 15 * cx; c++, s++)
		{
			if (x + c < 0 || s < GFX.Screen)
				continue;

			if (x + c >= W)
			{
				s += 15 * cx - c;
				break;
			}

			uint8 p = crosshair[(r / rx) * 15 + (c / cx)];

			if (p == '#' && fgcolor)
				*s = (fgcolor & 0x10) ? COLOR_ADD1_2(fg, *s) : fg;
			else
				if (p == '.' && bgcolor)
					*s = (bgcolor & 0x10) ? COLOR_ADD1_2(*s, bg) : bg;
		}
	}
}

static inline void S9xLatchCounters (void)
{
	// Latch h and v counters, like the gun

	PPU.HVBeamCounterLatched = 1;
	PPU.VBeamPosLatched = (uint16) CPU.V_Counter;

	// From byuu:
	// All dots are 4 cycles long, except dots 322 and 326. dots 322 and 326 are 6 cycles long.
	// This holds true for all scanlines except scanline 240 on non-interlace odd frames.
	// The reason for this is because this scanline is only 1360 cycles long,
	// instead of 1364 like all other scanlines.
	// This makes the effective range of hscan_pos 0-339 at all times.
	int32	hc = CPU.Cycles;

	if (Timings.H_Max == Timings.H_Max_Master) // 1364
	{
		if (hc >= 1292)
			hc -= ONE_DOT_CYCLE_DIV_2;
		if (hc >= 1308)
			hc -= ONE_DOT_CYCLE_DIV_2;
	}

	PPU.HBeamPosLatched = (uint16) (hc / ONE_DOT_CYCLE);

	Memory.FillRAM[0x213f] |= 0x40;
}

#define S9X_TRY_GUN_LATCH() \
	PPU.HVBeamCounterLatched = 1; \
	PPU.VBeamPosLatched = (uint16) PPU.GunVLatch; \
	PPU.HBeamPosLatched = (uint16) PPU.GunHLatch; \
	Memory.FillRAM[0x213f] |= 0x40;

static void S9xUpdateHVTimerPosition (void)
{
	if (PPU.HTimerEnabled)
	{
		if (PPU.IRQHBeamPos != 0)
		{
			// IRQ_read
			PPU.HTimerPosition = PPU.IRQHBeamPos * ONE_DOT_CYCLE;
			if (Timings.H_Max == Timings.H_Max_Master)	// 1364
			{
				if (PPU.IRQHBeamPos > 322)
					PPU.HTimerPosition += (ONE_DOT_CYCLE_DIV_2);
				if (PPU.IRQHBeamPos > 326)
					PPU.HTimerPosition += (ONE_DOT_CYCLE_DIV_2);
			}

			// Add 14 to HTimerPosition
			// /IRQ - add 4 to HTimerPosition
			// after CPU executing - add 6 to HTimerPosition
			// Total = add 24
			PPU.HTimerPosition += 24;
		}
		else
			PPU.HTimerPosition = 20;
	}
	else
		PPU.HTimerPosition = 20;

	PPU.VTimerPosition = PPU.IRQVBeamPos;

	if ((PPU.HTimerPosition >= Timings.H_Max) && (PPU.IRQHBeamPos < 340))
	{
		PPU.HTimerPosition -= Timings.H_Max;
		PPU.VTimerPosition++;
		// FIXME
		if (PPU.VTimerPosition >= Timings.V_Max)
			PPU.VTimerPosition = 0;
	}

	if (PPU.HTimerPosition < CPU.Cycles)
	{
		switch (CPU.WhichEvent)
		{
			case HC_IRQ_1_3_EVENT:
				CPU.WhichEvent = HC_HDMA_START_EVENT;
				CPU.NextEvent  = Timings.HDMAStart;
				break;

			case HC_IRQ_3_5_EVENT:
				CPU.WhichEvent = HC_HCOUNTER_MAX_EVENT;
				CPU.NextEvent  = Timings.H_Max;
				break;

			case HC_IRQ_5_7_EVENT:
				CPU.WhichEvent = HC_HDMA_INIT_EVENT;
				CPU.NextEvent  = Timings.HDMAInit;
				break;

			case HC_IRQ_7_9_EVENT:
				CPU.WhichEvent = HC_RENDER_EVENT;
				CPU.NextEvent  = Timings.RenderPos;
				break;

			case HC_IRQ_9_A_EVENT:
				CPU.WhichEvent = HC_WRAM_REFRESH_EVENT;
				CPU.NextEvent  = Timings.WRAMRefreshPos;
				break;

			case HC_IRQ_A_1_EVENT:
				CPU.WhichEvent = HC_HBLANK_START_EVENT;
				CPU.NextEvent  = Timings.HBlankStart;
				break;
		}
	}
	else
	if ((PPU.HTimerPosition < CPU.NextEvent) || (!(CPU.WhichEvent & 1) && (PPU.HTimerPosition == CPU.NextEvent)))
	{
		CPU.NextEvent = PPU.HTimerPosition;

		switch (CPU.WhichEvent)
		{
			case HC_HDMA_START_EVENT:
				CPU.WhichEvent = HC_IRQ_1_3_EVENT;
				break;

			case HC_HCOUNTER_MAX_EVENT:
				CPU.WhichEvent = HC_IRQ_3_5_EVENT;
				break;

			case HC_HDMA_INIT_EVENT:
				CPU.WhichEvent = HC_IRQ_5_7_EVENT;
				break;

			case HC_RENDER_EVENT:
				CPU.WhichEvent = HC_IRQ_7_9_EVENT;
				break;

			case HC_WRAM_REFRESH_EVENT:
				CPU.WhichEvent = HC_IRQ_9_A_EVENT;
				break;

			case HC_HBLANK_START_EVENT:
				CPU.WhichEvent = HC_IRQ_A_1_EVENT;
				break;
		}
	}
	else
	{
		switch (CPU.WhichEvent)
		{
			case HC_IRQ_1_3_EVENT:
				CPU.WhichEvent = HC_HDMA_START_EVENT;
				CPU.NextEvent  = Timings.HDMAStart;
				break;

			case HC_IRQ_3_5_EVENT:
				CPU.WhichEvent = HC_HCOUNTER_MAX_EVENT;
				CPU.NextEvent  = Timings.H_Max;
				break;

			case HC_IRQ_5_7_EVENT:
				CPU.WhichEvent = HC_HDMA_INIT_EVENT;
				CPU.NextEvent  = Timings.HDMAInit;
				break;

			case HC_IRQ_7_9_EVENT:
				CPU.WhichEvent = HC_RENDER_EVENT;
				CPU.NextEvent  = Timings.RenderPos;
				break;

			case HC_IRQ_9_A_EVENT:
				CPU.WhichEvent = HC_WRAM_REFRESH_EVENT;
				CPU.NextEvent  = Timings.WRAMRefreshPos;
				break;

			case HC_IRQ_A_1_EVENT:
				CPU.WhichEvent = HC_HBLANK_START_EVENT;
				CPU.NextEvent  = Timings.HBlankStart;
				break;
		}
	}

}

void S9xFixColourBrightness (void)
{
	IPPU.XB = mul_brightness[PPU.Brightness];

	for (int i = 0; i < 256; i++)
	{
		IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i])       & 0x1f];
		IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i] >>  5) & 0x1f];
		IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i] >> 10) & 0x1f];
		IPPU.ScreenColors[i] = BUILD_PIXEL(IPPU.Red[i], IPPU.Green[i], IPPU.Blue[i]);
	}
}

static inline void REGISTER_2122 (uint8 Byte)
{
	if (PPU.CGFLIP)
	{
		if ((Byte & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
		{
			FLUSH_REDRAW();
			PPU.CGDATA[PPU.CGADD] &= 0x00ff;
			PPU.CGDATA[PPU.CGADD] |= (Byte & 0x7f) << 8;
			IPPU.Blue[PPU.CGADD] = IPPU.XB[(Byte >> 2) & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
			IPPU.ScreenColors[PPU.CGADD] = (uint16) BUILD_PIXEL(IPPU.Red[PPU.CGADD], IPPU.Green[PPU.CGADD], IPPU.Blue[PPU.CGADD]);
		}

		PPU.CGADD++;
	}
	else
	{
		if (Byte != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
		{
			FLUSH_REDRAW();
			PPU.CGDATA[PPU.CGADD] &= 0x7f00;
			PPU.CGDATA[PPU.CGADD] |= Byte;
			IPPU.Red[PPU.CGADD] = IPPU.XB[Byte & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
			IPPU.ScreenColors[PPU.CGADD] = (uint16) BUILD_PIXEL(IPPU.Red[PPU.CGADD], IPPU.Green[PPU.CGADD], IPPU.Blue[PPU.CGADD]);
		}
	}

	PPU.CGFLIP ^= 1;
}

// This code is correct, however due to Snes9x's inaccurate timings, some games might be broken by this change. :(
#define CHECK_INBLANK !(Settings.BlockInvalidVRAMAccess && !PPU.ForcedBlanking && CPU.V_Counter < PPU.ScreenHeight + FIRST_VISIBLE_LINE)

static inline void REGISTER_2118 (uint8 Byte)
{
	uint32	address;

	if (PPU.VMA.FullGraphicCount)
	{
		uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
		address = (((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) & 0xffff;
		Memory.VRAM[address] = Byte;
	}
	else
		Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (!PPU.VMA.High)
	{
		PPU.VMA.Address += PPU.VMA.Increment;
	}
}

static inline void REGISTER_2119 (uint8 Byte)
{
	uint32	address;

	if (PPU.VMA.FullGraphicCount)
	{
		uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
		address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;
		Memory.VRAM[address] = Byte;
	}
	else
		Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (PPU.VMA.High)
	{
		PPU.VMA.Address += PPU.VMA.Increment;
	}
}

static inline void REGISTER_2104 (uint8 Byte)
{
	uint16 SignExtend[2] = {0x0000,0xff00};
	if (PPU.OAMAddr & 0x100)
	{
		int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
		if (Byte != PPU.OAMData[addr])
		{
			FLUSH_REDRAW();
			PPU.OAMData[addr] = Byte;
			IPPU.OBJChanged = TRUE;

			// X position high bit, and sprite size (x4)
			struct SOBJ *pObj = &PPU.OBJ[(addr & 0x1f) * 4];
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Byte >> 0) & 1];
			pObj++->Size = Byte & 2;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Byte >> 2) & 1];
			pObj++->Size = Byte & 8;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Byte >> 4) & 1];
			pObj++->Size = Byte & 32;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Byte >> 6) & 1];
			pObj->Size = Byte & 128;
		}

		PPU.OAMFlip ^= 1;
		if (!(PPU.OAMFlip & 1))
		{
			++PPU.OAMAddr;
			PPU.OAMAddr &= 0x1ff;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			}
		}
		else
		{
			if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
				IPPU.OBJChanged = TRUE;
		}
	}
	else if (!(PPU.OAMFlip & 1))
	{
		PPU.OAMWriteRegister &= 0xff00;
		PPU.OAMWriteRegister |= Byte;
		PPU.OAMFlip |= 1;
		if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
			IPPU.OBJChanged = TRUE;
	}
	else
	{
		PPU.OAMWriteRegister &= 0x00ff;
		uint8 lowbyte = (uint8) (PPU.OAMWriteRegister);
		uint8 highbyte = Byte;
		PPU.OAMWriteRegister |= Byte << 8;

		int addr = (PPU.OAMAddr << 1);
		if (lowbyte != PPU.OAMData[addr] || highbyte != PPU.OAMData[addr + 1])
		{
			FLUSH_REDRAW();
			PPU.OAMData[addr] = lowbyte;
			PPU.OAMData[addr + 1] = highbyte;
			IPPU.OBJChanged = TRUE;
			if (addr & 2)
			{
				// Tile
				PPU.OBJ[addr = PPU.OAMAddr >> 1].Name = PPU.OAMWriteRegister & 0x1ff;
				// priority, h and v flip.
				PPU.OBJ[addr].Palette  = (highbyte >> 1) & 7;
				PPU.OBJ[addr].Priority = (highbyte >> 4) & 3;
				PPU.OBJ[addr].HFlip    = (highbyte >> 6) & 1;
				PPU.OBJ[addr].VFlip    = (highbyte >> 7) & 1;
			}
			else
			{
				// X position (low)
				PPU.OBJ[addr = PPU.OAMAddr >> 1].HPos &= 0xff00;
				PPU.OBJ[addr].HPos |= lowbyte;
				// Sprite Y position
				PPU.OBJ[addr].VPos = highbyte;
			}
		}

		PPU.OAMFlip &= ~1;
		++PPU.OAMAddr;
		if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
		{
			PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
			IPPU.OBJChanged = TRUE;
		}
	}
}

static void S9xSetSuperFX (uint8 byte, uint16 address)
{
	switch (address)
	{
		case 0x3030:
			if ((Memory.FillRAM[0x3030] ^ byte) & FLG_G)
			{
				Memory.FillRAM[0x3030] = byte;
				if (byte & FLG_G)
				{
					if (!SuperFX.oneLineDone)
					{
						if(CHECK_EXEC_SUPERFX())
							S9xSuperFXExec();
						SuperFX.oneLineDone = TRUE;
					}
				}
				else
				{
					//FX Flush cache
					GSU.vCacheFlags = 0;
					GSU.vCacheBaseReg = 0;
					GSU.bCacheActive = FALSE;
				}
			}
			else
				Memory.FillRAM[0x3030] = byte;

			break;

		case 0x3031:
		case 0x3033:
			Memory.FillRAM[address] = byte;
			break;
		case 0x3034:
		case 0x3036:
			Memory.FillRAM[address] = byte & 0x7f;
			break;
		case 0x3037:
			Memory.FillRAM[0x3037] = byte;
			break;
		case 0x3038:
			Memory.FillRAM[0x3038] = byte;
			// SCBR write seen. We need to update our cached screen pointers
			GSU.vSCBRDirty = TRUE;
			break;

		case 0x3039:
		case 0x303a:
			Memory.FillRAM[address] = byte;
			break;
		case 0x303b:
			break;

		case 0x303c:
			Memory.FillRAM[0x303c] = byte;
			/* Update BankReg and Bank pointer */
			GSU.vRamBankReg = (uint32) byte & (FX_RAM_BANKS - 1);
			GSU.pvRamBank = GSU.apvRamBank[byte & 0x3];
			break;

		case 0x303f:
			Memory.FillRAM[0x303f] = byte;
			break;

		case 0x301f:
			Memory.FillRAM[0x301f] = byte;
			Memory.FillRAM[0x3000 + GSU_SFR] |= FLG_G;
			if (!SuperFX.oneLineDone)
			{
				if(CHECK_EXEC_SUPERFX())
					S9xSuperFXExec();
				SuperFX.oneLineDone = TRUE;
			}

			break;

		default:
			Memory.FillRAM[address] = byte;
			if (address >= 0x3100)
			{
				// Write access to the cache
				if ((address & 0x00f) == 0x00f)
					GSU.vCacheFlags |= 1 << ((address & 0x1f0) >> 4);
			}

			break;
	}
}

#define REGISTER_2180(Byte) \
	Memory.RAM[PPU.WRAM++] = Byte; \
	PPU.WRAM &= 0x1ffff;

void S9xSetPPU (uint8 Byte, uint16 Address)
{
	// MAP_PPU: $2000-$3FFF

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
			// S9xSetPPU() is called to write to DMA[].AAddress
			#if 0
			if ((Address & 0xff00) == 0x2100)
			{
				// Cannot access to Address Bus B ($2100-$21ff) via (H)DMA
				return;
			}
			else
			{
				// 0x2000-0x3FFF is connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
				return;
			}
			#endif
			return;
		}
		else
		{
			// S9xSetPPU() is called to read from $21xx
			// Take care of DMA wrapping
			if (Address > 0x21ff)
				Address = 0x2100 + (Address & 0xff);
		}
	}


	if ((Address & 0xffc0) == 0x2140) // APUIO0, APUIO1, APUIO2, APUIO3
		// write_port will run the APU until given clock before writing value
		S9xAPUWritePort(Address & 3, Byte);
	else if (Address <= 0x2183)
	{
		switch (Address)
		{
			case 0x2100: // INIDISP
				if (Byte != Memory.FillRAM[0x2100])
				{
					FLUSH_REDRAW();

					if (PPU.Brightness != (Byte & 0xf))
					{
						IPPU.DirectColourMapsNeedRebuild = TRUE;
						PPU.Brightness = Byte & 0xf;
						S9xFixColourBrightness();
						if (PPU.Brightness > IPPU.MaxBrightness)
							IPPU.MaxBrightness = PPU.Brightness;
					}

					if ((Memory.FillRAM[0x2100] & 0x80) != (Byte & 0x80))
					{
						PPU.ForcedBlanking = (Byte >> 7) & 1;
					}
				}

				if ((Memory.FillRAM[0x2100] & 0x80) && CPU.V_Counter == PPU.ScreenHeight + FIRST_VISIBLE_LINE)
				{
					PPU.OAMAddr = PPU.SavedOAMAddr;

					uint8 tmp = 0;
					if (PPU.OAMPriorityRotation)
						tmp = (PPU.OAMAddr & 0xfe) >> 1;
					if ((PPU.OAMFlip & 1) || PPU.FirstSprite != tmp)
					{
						PPU.FirstSprite = tmp;
						IPPU.OBJChanged = TRUE;
					}

					PPU.OAMFlip = 0;
				}

				break;

			case 0x2101: // OBSEL
				if (Byte != Memory.FillRAM[0x2101])
				{
					FLUSH_REDRAW();
					PPU.OBJNameBase = (Byte & 3) << 14;
					PPU.OBJNameSelect = ((Byte >> 3) & 3) << 13;
					PPU.OBJSizeSelect = (Byte >> 5) & 7;
					IPPU.OBJChanged = TRUE;
				}

				break;

			case 0x2102: // OAMADDL
				PPU.OAMAddr = ((Memory.FillRAM[0x2103] & 1) << 8) | Byte;
				PPU.OAMFlip = 2;
				PPU.OAMReadFlip = 0;
				PPU.SavedOAMAddr = PPU.OAMAddr;
				if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
				{
					PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
					IPPU.OBJChanged = TRUE;
				}

				break;

			case 0x2103: // OAMADDH
				PPU.OAMAddr = ((Byte & 1) << 8) | Memory.FillRAM[0x2102];
				PPU.OAMPriorityRotation = (Byte & 0x80) ? 1 : 0;
				if (PPU.OAMPriorityRotation)
				{
					if (PPU.FirstSprite != (PPU.OAMAddr >> 1))
					{
						PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
						IPPU.OBJChanged = TRUE;
					}
				}
				else
				{
					if (PPU.FirstSprite != 0)
					{
						PPU.FirstSprite = 0;
						IPPU.OBJChanged = TRUE;
					}
				}

				PPU.OAMFlip = 0;
				PPU.OAMReadFlip = 0;
				PPU.SavedOAMAddr = PPU.OAMAddr;

				break;

			case 0x2104: // OAMDATA
				REGISTER_2104(Byte);
				break;

			case 0x2105: // BGMODE
				if (Byte != Memory.FillRAM[0x2105])
				{
					FLUSH_REDRAW();
					PPU.BG[0].BGSize = (Byte >> 4) & 1;
					PPU.BG[1].BGSize = (Byte >> 5) & 1;
					PPU.BG[2].BGSize = (Byte >> 6) & 1;
					PPU.BG[3].BGSize = (Byte >> 7) & 1;
					PPU.BGMode = Byte & 7;
					// BJ: BG3Priority only takes effect if BGMode == 1 and the bit is set
					PPU.BG3Priority = ((Byte & 0x0f) == 0x09);
					IPPU.Interlace = Memory.FillRAM[0x2133] & 1;
				}

				break;

			case 0x2106: // MOSAIC
				if (Byte != Memory.FillRAM[0x2106])
				{
					FLUSH_REDRAW();
					PPU.MosaicStart = CPU.V_Counter;
					if (PPU.MosaicStart > PPU.ScreenHeight)
						PPU.MosaicStart = 0;
					PPU.Mosaic = (Byte >> 4) + 1;
					PPU.BGMosaic[0] = (Byte & 1);
					PPU.BGMosaic[1] = (Byte & 2);
					PPU.BGMosaic[2] = (Byte & 4);
					PPU.BGMosaic[3] = (Byte & 8);
				}

				break;

			case 0x2107: // BG1SC
			case 0x2108: // BG2SC
			case 0x2109: // BG3SC
			case 0x210a: // BG4SC
				if (Byte != Memory.FillRAM[Address])
				{
					FLUSH_REDRAW();
					PPU.BG[Address-0x2107].SCSize = Byte & 3;
					PPU.BG[Address-0x2107].SCBase = (Byte & 0x7c) << 8;
				}

				break;
			case 0x210b: // BG12NBA
				if (Byte != Memory.FillRAM[0x210b])
				{
					FLUSH_REDRAW();
					PPU.BG[0].NameBase = (Byte & 7) << 12;
					PPU.BG[1].NameBase = ((Byte >> 4) & 7) << 12;
				}

				break;

			case 0x210c: // BG34NBA
				if (Byte != Memory.FillRAM[0x210c])
				{
					FLUSH_REDRAW();
					PPU.BG[2].NameBase = (Byte & 7) << 12;
					PPU.BG[3].NameBase = ((Byte >> 4) & 7) << 12;
				}

				break;

			case 0x210d: // BG1HOFS, M7HOFS
				PPU.BG[0].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[0].HOffset >> 8) & 7);
				PPU.M7HOFS = (Byte << 8) | PPU.M7byte;
				PPU.BGnxOFSbyte = Byte;
				PPU.M7byte = Byte;
				break;

			case 0x210e: // BG1VOFS, M7VOFS
				PPU.BG[0].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
				PPU.M7VOFS = (Byte << 8) | PPU.M7byte;
				PPU.BGnxOFSbyte = Byte;
				PPU.M7byte = Byte;
				break;

			case 0x210f: // BG2HOFS
				PPU.BG[1].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[1].HOffset >> 8) & 7);
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2110: // BG2VOFS
				PPU.BG[1].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2111: // BG3HOFS
				PPU.BG[2].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[2].HOffset >> 8) & 7);
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2112: // BG3VOFS
				PPU.BG[2].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2113: // BG4HOFS
				PPU.BG[3].HOffset = (Byte << 8) | (PPU.BGnxOFSbyte & ~7) | ((PPU.BG[3].HOffset >> 8) & 7);
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2114: // BG4VOFS
				PPU.BG[3].VOffset = (Byte << 8) | PPU.BGnxOFSbyte;
				PPU.BGnxOFSbyte = Byte;
				break;

			case 0x2115: // VMAIN
				PPU.VMA.High = (Byte & 0x80) == 0 ? FALSE : TRUE;
				switch (Byte & 3)
				{
					case 0: PPU.VMA.Increment = 1;   break;
					case 1: PPU.VMA.Increment = 32;  break;
					case 2: PPU.VMA.Increment = 128; break;
					case 3: PPU.VMA.Increment = 128; break;
				}

				if (Byte & 0x0c)
				{
					static uint16 Shift[4]    = { 0, 5, 6, 7 };
					static uint16 IncCount[4] = { 0, 32, 64, 128 };

					uint8 i = (Byte & 0x0c) >> 2;
					PPU.VMA.FullGraphicCount = IncCount[i];
					PPU.VMA.Mask1 = IncCount[i] * 8 - 1;
					PPU.VMA.Shift = Shift[i];
				}
				else
					PPU.VMA.FullGraphicCount = 0;
				break;

			case 0x2116: // VMADDL
				PPU.VMA.Address &= 0xff00;
				PPU.VMA.Address |= Byte;
			#ifdef CORRECT_VRAM_READS
				if (PPU.VMA.FullGraphicCount)
				{
					uint32 addr = PPU.VMA.Address;
					uint32 rem = addr & PPU.VMA.Mask1;
					uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
					IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
				}
				else
					IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));
			#else
				IPPU.FirstVRAMRead = TRUE;
			#endif
				break;

			case 0x2117: // VMADDH
				PPU.VMA.Address &= 0x00ff;
				PPU.VMA.Address |= Byte << 8;
			#ifdef CORRECT_VRAM_READS
				if (PPU.VMA.FullGraphicCount)
				{
					uint32 addr = PPU.VMA.Address;
					uint32 rem = addr & PPU.VMA.Mask1;
					uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
					IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
				}
				else
					IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));
			#else
				IPPU.FirstVRAMRead = TRUE;
			#endif
				break;

			case 0x2118: // VMDATAL
			#ifndef CORRECT_VRAM_READS
				IPPU.FirstVRAMRead = TRUE;
			#endif
				if(CHECK_INBLANK)
					REGISTER_2118(Byte);
				break;

			case 0x2119: // VMDATAH
			#ifndef CORRECT_VRAM_READS
				IPPU.FirstVRAMRead = TRUE;
			#endif
				if(CHECK_INBLANK)
					REGISTER_2119(Byte);
				break;

			case 0x211a: // M7SEL
				if (Byte != Memory.FillRAM[0x211a])
				{
					FLUSH_REDRAW();
					PPU.Mode7Repeat = Byte >> 6;
					if (PPU.Mode7Repeat == 1)
						PPU.Mode7Repeat = 0;
					PPU.Mode7VFlip = (Byte & 2) >> 1;
					PPU.Mode7HFlip = Byte & 1;
				}

				break;

			case 0x211b: // M7A
				PPU.MatrixA = PPU.M7byte | (Byte << 8);
				PPU.Need16x8Mulitply = TRUE;
				PPU.M7byte = Byte;
				break;

			case 0x211c: // M7B
				PPU.MatrixB = PPU.M7byte | (Byte << 8);
				PPU.Need16x8Mulitply = TRUE;
				PPU.M7byte = Byte;
				break;

			case 0x211d: // M7C
				PPU.MatrixC = PPU.M7byte | (Byte << 8);
				PPU.M7byte = Byte;
				break;

			case 0x211e: // M7D
				PPU.MatrixD = PPU.M7byte | (Byte << 8);
				PPU.M7byte = Byte;
				break;

			case 0x211f: // M7X
				PPU.CentreX = PPU.M7byte | (Byte << 8);
				PPU.M7byte = Byte;
				break;

			case 0x2120: // M7Y
				PPU.CentreY = PPU.M7byte | (Byte << 8);
				PPU.M7byte = Byte;
				break;

			case 0x2121: // CGADD
				PPU.CGFLIP = 0;
				PPU.CGFLIPRead = 0;
				PPU.CGADD = Byte;
				break;

			case 0x2122: // CGDATA
				REGISTER_2122(Byte);
				break;

			case 0x2123: // W12SEL
				if (Byte != Memory.FillRAM[0x2123])
				{
					FLUSH_REDRAW();
					PPU.ClipWindow1Enable[0] = !!(Byte & 0x02);
					PPU.ClipWindow1Enable[1] = !!(Byte & 0x20);
					PPU.ClipWindow2Enable[0] = !!(Byte & 0x08);
					PPU.ClipWindow2Enable[1] = !!(Byte & 0x80);
					PPU.ClipWindow1Inside[0] = !(Byte & 0x01);
					PPU.ClipWindow1Inside[1] = !(Byte & 0x10);
					PPU.ClipWindow2Inside[0] = !(Byte & 0x04);
					PPU.ClipWindow2Inside[1] = !(Byte & 0x40);
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2124: // W34SEL
				if (Byte != Memory.FillRAM[0x2124])
				{
					FLUSH_REDRAW();
					PPU.ClipWindow1Enable[2] = !!(Byte & 0x02);
					PPU.ClipWindow1Enable[3] = !!(Byte & 0x20);
					PPU.ClipWindow2Enable[2] = !!(Byte & 0x08);
					PPU.ClipWindow2Enable[3] = !!(Byte & 0x80);
					PPU.ClipWindow1Inside[2] = !(Byte & 0x01);
					PPU.ClipWindow1Inside[3] = !(Byte & 0x10);
					PPU.ClipWindow2Inside[2] = !(Byte & 0x04);
					PPU.ClipWindow2Inside[3] = !(Byte & 0x40);
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2125: // WOBJSEL
				if (Byte != Memory.FillRAM[0x2125])
				{
					FLUSH_REDRAW();
					PPU.ClipWindow1Enable[4] = !!(Byte & 0x02);
					PPU.ClipWindow1Enable[5] = !!(Byte & 0x20);
					PPU.ClipWindow2Enable[4] = !!(Byte & 0x08);
					PPU.ClipWindow2Enable[5] = !!(Byte & 0x80);
					PPU.ClipWindow1Inside[4] = !(Byte & 0x01);
					PPU.ClipWindow1Inside[5] = !(Byte & 0x10);
					PPU.ClipWindow2Inside[4] = !(Byte & 0x04);
					PPU.ClipWindow2Inside[5] = !(Byte & 0x40);
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2126: // WH0
				if (Byte != Memory.FillRAM[0x2126])
				{
					FLUSH_REDRAW();
					PPU.Window1Left = Byte;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2127: // WH1
				if (Byte != Memory.FillRAM[0x2127])
				{
					FLUSH_REDRAW();
					PPU.Window1Right = Byte;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2128: // WH2
				if (Byte != Memory.FillRAM[0x2128])
				{
					FLUSH_REDRAW();
					PPU.Window2Left = Byte;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x2129: // WH3
				if (Byte != Memory.FillRAM[0x2129])
				{
					FLUSH_REDRAW();
					PPU.Window2Right = Byte;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x212a: // WBGLOG
				if (Byte != Memory.FillRAM[0x212a])
				{
					FLUSH_REDRAW();
					PPU.ClipWindowOverlapLogic[0] = (Byte & 0x03);
					PPU.ClipWindowOverlapLogic[1] = (Byte & 0x0c) >> 2;
					PPU.ClipWindowOverlapLogic[2] = (Byte & 0x30) >> 4;
					PPU.ClipWindowOverlapLogic[3] = (Byte & 0xc0) >> 6;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x212b: // WOBJLOG
				if (Byte != Memory.FillRAM[0x212b])
				{
					FLUSH_REDRAW();
					PPU.ClipWindowOverlapLogic[4] = (Byte & 0x03);
					PPU.ClipWindowOverlapLogic[5] = (Byte & 0x0c) >> 2;
					PPU.RecomputeClipWindows = TRUE;
				}

				break;

			case 0x212c: // TM
			case 0x212d: // TS
			case 0x212e: // TMW
			case 0x212f: // TSW
			case 0x2130: // CGWSEL
				if (Byte != Memory.FillRAM[Address])
				{
					FLUSH_REDRAW();
					PPU.RecomputeClipWindows = TRUE;
				}

				break;
			case 0x2131: // CGADSUB
				if (Byte != Memory.FillRAM[0x2131])
				{
					FLUSH_REDRAW();
				}

				break;

			case 0x2132: // COLDATA
				if (Byte != Memory.FillRAM[0x2132])
				{
					FLUSH_REDRAW();
					if (Byte & 0x80)
						PPU.FixedColourBlue  = Byte & 0x1f;
					if (Byte & 0x40)
						PPU.FixedColourGreen = Byte & 0x1f;
					if (Byte & 0x20)
						PPU.FixedColourRed   = Byte & 0x1f;
				}

				break;

			case 0x2133: // SETINI
				if (Byte != Memory.FillRAM[0x2133])
				{
					if ((Memory.FillRAM[0x2133] ^ Byte) & 8)
					{
						FLUSH_REDRAW();
						IPPU.PseudoHires = Byte & 8;
					}

					if (Byte & 0x04)
					{
						PPU.ScreenHeight = SNES_HEIGHT_EXTENDED;
						if (IPPU.DoubleHeightPixels)
							IPPU.RenderedScreenHeight = PPU.ScreenHeight << 1;
						else
							IPPU.RenderedScreenHeight = PPU.ScreenHeight;
					}
					else
						PPU.ScreenHeight = SNES_HEIGHT;

					if ((Memory.FillRAM[0x2133] ^ Byte) & 3)
					{
						FLUSH_REDRAW();
						if ((Memory.FillRAM[0x2133] ^ Byte) & 2)
							IPPU.OBJChanged = TRUE;
						IPPU.Interlace = Byte & 1;
						IPPU.InterlaceOBJ = Byte & 2;
					}
				}

				break;

			case 0x2134: // MPYL
			case 0x2135: // MPYM
			case 0x2136: // MPYH
			case 0x2137: // SLHV
			case 0x2138: // OAMDATAREAD
			case 0x2139: // VMDATALREAD
			case 0x213a: // VMDATAHREAD
			case 0x213b: // CGDATAREAD
			case 0x213c: // OPHCT
			case 0x213d: // OPVCT
			case 0x213e: // STAT77
			case 0x213f: // STAT78
				return;

			case 0x2180: // WMDATA
				if (!CPU.InWRAMDMAorHDMA)
					REGISTER_2180(Byte);
				break;

			case 0x2181: // WMADDL
				if (!CPU.InWRAMDMAorHDMA)
				{
					PPU.WRAM &= 0x1ff00;
					PPU.WRAM |= Byte;
				}

				break;

			case 0x2182: // WMADDM
				if (!CPU.InWRAMDMAorHDMA)
				{
					PPU.WRAM &= 0x100ff;
					PPU.WRAM |= Byte << 8;
				}

				break;

			case 0x2183: // WMADDH
				if (!CPU.InWRAMDMAorHDMA)
				{
					PPU.WRAM &= 0x0ffff;
					PPU.WRAM |= Byte << 16;
					PPU.WRAM &= 0x1ffff;
				}

				break;
		}
	}
	else
	{
		if (Settings.SuperFX && Address >= 0x3000 && Address <= 0x32ff)
		{
			S9xSetSuperFX(Byte, Address);
			return;
		}
		else if (Settings.SA1     && Address >= 0x2200)
		{
			if (Address <= 0x23ff)
				S9xSetSA1(Byte, Address);
			else
				Memory.FillRAM[Address] = Byte;
			return;
		}
		else if (Settings.BS      && Address >= 0x2188 && Address <= 0x219f)
			S9xSetBSXPPU(Byte, Address);
		else if (Settings.SRTC    && Address == 0x2801)
			S9xSetSRTC(Byte, Address);
	}

	Memory.FillRAM[Address] = Byte;
}

static uint8 S9xGetSuperFX (uint16 address)
{
	uint8	byte = Memory.FillRAM[address];
	if (address == 0x3031)
	{
		S9X_CLEAR_IRQ(GSU_IRQ_SOURCE);
		Memory.FillRAM[0x3031] = byte & 0x7f;
	}

	return (byte);
}

uint8 S9xGetPPU (uint16 Address)
{
	// MAP_PPU: $2000-$3FFF

	if (Address < 0x2100)
		return (OpenBus);

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && !DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
			// S9xGetPPU() is called to read from DMA[].AAddress
			#if 0
			if ((Address & 0xff00) == 0x2100)
				// Cannot access to Address Bus B ($2100-$21FF) via (H)DMA
				return (OpenBus);
			else
				// $2200-$3FFF are connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
			#endif
				return (OpenBus);
		}
		else
		{
			// S9xGetPPU() is called to write to $21xx
			// Take care of DMA wrapping
			if (Address > 0x21ff)
				Address = 0x2100 + (Address & 0xff);
		}
	}

	if ((Address & 0xffc0) == 0x2140) // APUIO0, APUIO1, APUIO2, APUIO3
		return (S9xAPUReadPort(Address & 3)); // will run the APU until given APU time before reading value
	else if (Address <= 0x2183)
	{
		uint8	byte;

		switch (Address)
		{
			case 0x2104: // OAMDATA
			case 0x2105: // BGMODE
			case 0x2106: // MOSAIC
			case 0x2108: // BG2SC
			case 0x2109: // BG3SC
			case 0x210a: // BG4SC
			case 0x2114: // BG4VOFS
			case 0x2115: // VMAIN
			case 0x2116: // VMADDL
			case 0x2118: // VMDATAL
			case 0x2119: // VMDATAH
			case 0x211a: // M7SEL
			case 0x2124: // W34SEL
			case 0x2125: // WOBJSEL
			case 0x2126: // WH0
			case 0x2128: // WH2
			case 0x2129: // WH3
			case 0x212a: // WBGLOG
				return (PPU.OpenBus1);

			case 0x2134: // MPYL
			case 0x2135: // MPYM
			case 0x2136: // MPYH
				if (PPU.Need16x8Mulitply)
				{
					int32 r = (int32) PPU.MatrixA * (int32) (PPU.MatrixB >> 8);
					Memory.FillRAM[0x2134] = (uint8) r;
					Memory.FillRAM[0x2135] = (uint8) (r >> 8);
					Memory.FillRAM[0x2136] = (uint8) (r >> 16);
					PPU.Need16x8Mulitply = FALSE;
				}
				return (PPU.OpenBus1 = Memory.FillRAM[Address]);

			case 0x2137: // SLHV
				if ((Memory.FillRAM[0x4213] & 0x80))
					S9xLatchCounters();
				if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
					PPU.GunVLatch = 1000;
				return (OpenBus);

			case 0x2138: // OAMDATAREAD
				if (PPU.OAMAddr & 0x100)
				{
					if (!(PPU.OAMFlip & 1))
						byte = PPU.OAMData[(PPU.OAMAddr & 0x10f) << 1];
					else
					{
						byte = PPU.OAMData[((PPU.OAMAddr & 0x10f) << 1) + 1];
						PPU.OAMAddr = (PPU.OAMAddr + 1) & 0x1ff;
						if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
						{
							PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
							IPPU.OBJChanged = TRUE;
						}
					}
				}
				else
				{
					if (!(PPU.OAMFlip & 1))
						byte = PPU.OAMData[PPU.OAMAddr << 1];
					else
					{
						byte = PPU.OAMData[(PPU.OAMAddr << 1) + 1];
						++PPU.OAMAddr;
						if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
						{
							PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
							IPPU.OBJChanged = TRUE;
						}
					}
				}

				PPU.OAMFlip ^= 1;
				return (PPU.OpenBus1 = byte);

			case 0x2139: // VMDATALREAD
#ifdef CORRECT_VRAM_READS
				byte = IPPU.VRAMReadBuffer & 0xff;
				if (!PPU.VMA.High)
				{
					if (PPU.VMA.FullGraphicCount)
					{
						uint32 addr = PPU.VMA.Address;
						uint32 rem = addr & PPU.VMA.Mask1;
						uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
						IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
					}
					else
						IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

					PPU.VMA.Address += PPU.VMA.Increment;
				}
#else
				if (IPPU.FirstVRAMRead)
					byte = Memory.VRAM[(PPU.VMA.Address << 1) & 0xffff];
				else
					if (PPU.VMA.FullGraphicCount)
					{
						uint32 addr = PPU.VMA.Address - 1;
						uint32 rem = addr & PPU.VMA.Mask1;
						uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
						byte = Memory.VRAM[((address << 1) - 2) & 0xffff];
					}
					else
						byte = Memory.VRAM[((PPU.VMA.Address << 1) - 2) & 0xffff];

				if (!PPU.VMA.High)
				{
					PPU.VMA.Address += PPU.VMA.Increment;
					IPPU.FirstVRAMRead = FALSE;
				}
#endif
				return (PPU.OpenBus1 = byte);

			case 0x213a: // VMDATAHREAD
#ifdef CORRECT_VRAM_READS
				byte = (IPPU.VRAMReadBuffer >> 8) & 0xff;
				if (PPU.VMA.High)
				{
					if (PPU.VMA.FullGraphicCount)
					{
						uint32 addr = PPU.VMA.Address;
						uint32 rem = addr & PPU.VMA.Mask1;
						uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
						IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
					}
					else
						IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

					PPU.VMA.Address += PPU.VMA.Increment;
				}
#else
				if (IPPU.FirstVRAMRead)
					byte = Memory.VRAM[((PPU.VMA.Address << 1) + 1) & 0xffff];
				else
					if (PPU.VMA.FullGraphicCount)
					{
						uint32 addr = PPU.VMA.Address - 1;
						uint32 rem = addr & PPU.VMA.Mask1;
						uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
						byte = Memory.VRAM[((address << 1) - 1) & 0xffff];
					}
					else
						byte = Memory.VRAM[((PPU.VMA.Address << 1) - 1) & 0xffff];

				if (PPU.VMA.High)
				{
					PPU.VMA.Address += PPU.VMA.Increment;
					IPPU.FirstVRAMRead = FALSE;
				}
#endif
				return (PPU.OpenBus1 = byte);

			case 0x213b: // CGDATAREAD
				if (PPU.CGFLIPRead)
					byte = (PPU.OpenBus2 & 0x80) | ((PPU.CGDATA[PPU.CGADD++] >> 8) & 0x7f);
				else
					byte = PPU.CGDATA[PPU.CGADD] & 0xff;
				PPU.CGFLIPRead ^= 1;
				return (PPU.OpenBus2 = byte);

			case 0x213c: // OPHCT
				if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
				{
					if ((Memory.FillRAM[0x4213] & 0x80))
					{
						S9X_TRY_GUN_LATCH();
					}
					PPU.GunVLatch = 1000;
				}
				if (PPU.HBeamFlip)
					byte = (PPU.OpenBus2 & 0xfe) | ((PPU.HBeamPosLatched >> 8) & 0x01);
				else
					byte = (uint8) PPU.HBeamPosLatched;
				PPU.HBeamFlip ^= 1;
				return (PPU.OpenBus2 = byte);

			case 0x213d: // OPVCT
				if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
				{
					if ((Memory.FillRAM[0x4213] & 0x80))
					{
						S9X_TRY_GUN_LATCH();
					}
					PPU.GunVLatch = 1000;
				}
				if (PPU.VBeamFlip)
					byte = (PPU.OpenBus2 & 0xfe) | ((PPU.VBeamPosLatched >> 8) & 0x01);
				else
					byte = (uint8) PPU.VBeamPosLatched;
				PPU.VBeamFlip ^= 1;
				return (PPU.OpenBus2 = byte);

			case 0x213e: // STAT77
				FLUSH_REDRAW();
				byte = (PPU.OpenBus1 & 0x10) | PPU.RangeTimeOver | MAX_5C77_VERSION;
				return (PPU.OpenBus1 = byte);

			case 0x213f: // STAT78
				if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
				{
					if ((Memory.FillRAM[0x4213] & 0x80))
					{
						S9X_TRY_GUN_LATCH();
					}
					PPU.GunVLatch = 1000;
				}
				PPU.VBeamFlip = PPU.HBeamFlip = 0;
				byte = (PPU.OpenBus2 & 0x20) | (Memory.FillRAM[0x213f] & 0xc0) | (Settings.PAL ? 0x10 : 0) | MAX_5C78_VERSION;
				Memory.FillRAM[0x213f] &= ~0x40;
				return (PPU.OpenBus2 = byte);

			case 0x2180: // WMDATA
				if (!CPU.InWRAMDMAorHDMA)
				{
					byte = Memory.RAM[PPU.WRAM++];
					PPU.WRAM &= 0x1ffff;
				}
				else
					byte = OpenBus;
				return (byte);

			default:
				return (OpenBus);
		}
	}
	else
	{
		if (Settings.SuperFX && Address >= 0x3000 && Address <= 0x32ff)
			return (S9xGetSuperFX(Address));
		else
			if (Settings.SA1     && Address >= 0x2200)
				return (S9xGetSA1(Address));
			else
				if (Settings.BS      && Address >= 0x2188 && Address <= 0x219f)
					return (S9xGetBSXPPU(Address));
				else	
					if (Settings.SRTC    && Address == 0x2800)
						return (S9xGetSRTC(Address));
					else
						return (OpenBus);
	}
}

static uint8	sdd1_decode_buffer[0x10000];

static inline bool8 addCyclesInDMA (uint8 dma_channel)
{
	// Add 8 cycles per byte, sync APU, and do HC related events.
	// If HDMA was done in S9xDoHEventProcessing(), check if it used the same channel as DMA.
	CPU.Cycles += SLOW_ONE_CYCLE;

	while (CPU.Cycles >= CPU.NextEvent)
		S9xDoHEventProcessing();

	if (CPU.HDMARanInDMA & (1 << dma_channel))
	{
		CPU.HDMARanInDMA = 0;
		// If HDMA triggers in the middle of DMA transfer and it uses the same channel,
		// it kills the DMA transfer immediately. $43x2 and $43x5 stop updating.
		return (FALSE);
	}

	CPU.HDMARanInDMA = 0;
	return (TRUE);
}

static inline void REGISTER_2119_linear (uint8 Byte)
{
	uint32	address = ((PPU.VMA.Address << 1) + 1) & 0xffff;

	Memory.VRAM[address] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (PPU.VMA.High)
		PPU.VMA.Address += PPU.VMA.Increment;
}

static inline void REGISTER_2118_linear (uint8 Byte)
{
	uint32	address = (PPU.VMA.Address << 1) & 0xffff;

	Memory.VRAM[address] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (!PPU.VMA.High)
		PPU.VMA.Address += PPU.VMA.Increment;
}

static inline void REGISTER_2118_tile (uint8 Byte)
{
	uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	uint32 address = (((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) & 0xffff;

	Memory.VRAM[address] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (!PPU.VMA.High)
		PPU.VMA.Address += PPU.VMA.Increment;
}

static inline void REGISTER_2119_tile (uint8 Byte)
{
	uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	Memory.VRAM[address] = Byte;

	IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	if (PPU.VMA.High)
		PPU.VMA.Address += PPU.VMA.Increment;
}

static uint8 dma_channels_to_be_used[8] = {0};
static bool special_chips_active = false;

static void S9xDoDMA (void)
{
	for(uint8 Channel = 0; Channel < 8; Channel++)
	{
		if(dma_channels_to_be_used[Channel] != 1)
			continue;

		CPU.InDMA = TRUE;
		CPU.InDMAorHDMA = TRUE;
		CPU.CurrentDMAorHDMAChannel = Channel;

		SDMA	*d = &DMA[Channel];

		// Check invalid DMA first
		if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80 && !d->ReverseTransfer)
		{
			// Attempting a DMA from WRAM to $2180 will not work, WRAM will not be written.
			// Attempting a DMA from $2180 to WRAM will similarly not work,
			// the value written is (initially) the OpenBus value.
			// In either case, the address in $2181-3 is not incremented.

			// Does an invalid DMA actually take time?
			// I'd say yes, since 'invalid' is probably just the WRAM chip
			// not being able to read and write itself at the same time
			// And no, PPU.WRAM should not be updated.

			int32	c = d->TransferBytes;
			// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
			if (c == 0)
				c = 0x10000;

			// 8 cycles per channel
			CPU.Cycles += SLOW_ONE_CYCLE;

			// 8 cycles per byte
			do
			{
				d->TransferBytes--;
				d->AAddress++;
				c--;
				if (!addCyclesInDMA(Channel))
				{
					CPU.InDMA = FALSE;
					CPU.InDMAorHDMA = FALSE;
					CPU.CurrentDMAorHDMAChannel = -1;
					continue;
				}
			}while(c);


			CPU.InDMA = FALSE;
			CPU.InDMAorHDMA = FALSE;
			CPU.CurrentDMAorHDMAChannel = -1;
			continue;
		}

		// Prepare for accessing $2118-2119
		switch (d->BAddress)
		{
			case 0x18:
			case 0x19:
				FLUSH_REDRAW();
				break;
		}

		int32	inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);
		int32	count = d->TransferBytes;
		// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
		if (count == 0)
			count = 0x10000;

		// Prepare for custom chip DMA
		uint8	*in_sdd1_dma = NULL;
		uint8	*spc7110_dma = NULL;
		bool8	in_sa1_dma = FALSE;

		// S-DD1
		if(special_chips_active)
		{
			if (Settings.SDD1)
			{
				if (d->AAddressFixed && Memory.FillRAM[0x4801] > 0)
				{
					// XXX: Should probably verify that we're DMAing from ROM?
					// And somewhere we should make sure we're not running across a mapping boundary too.
					// Hacky support for pre-decompressed S-DD1 data
					inc = !d->AAddressDecrement ? 1 : -1;

					uint8	*in_ptr = S9xGetBasePointer(((d->ABank << 16) | d->AAddress));
					if (in_ptr)
					{
						in_ptr += d->AAddress;
						SDD1_decompress(sdd1_decode_buffer, in_ptr, d->TransferBytes);
					}

					in_sdd1_dma = sdd1_decode_buffer;
				}

				Memory.FillRAM[0x4801] = 0;
			}

			// SPC7110


			if (Settings.SPC7110)
			{
				if (d->AAddress == 0x4800 || d->ABank == 0x50)
				{
					spc7110_dma = (uint8*)malloc(d->TransferBytes);
					for (int i = 0; i < d->TransferBytes; i++)
						spc7110_dma[i] = spc7110_decomp_read();

					int32	icount = r4809 | (r480a << 8);
					icount -= d->TransferBytes;
					r4809 =  icount & 0x00ff;
					r480a = (icount & 0xff00) >> 8;

					inc = 1;
					d->AAddress -= count;
				}
			}

			// SA-1

			if (Settings.SA1)
			{
				if (SA1.in_char_dma && d->BAddress == 0x18 && (d->ABank & 0xf0) == 0x40)
				{
					// Perform packed bitmap to PPU character format conversion on the data
					// before transmitting it to V-RAM via-DMA.
					int32	num_chars = 1 << ((Memory.FillRAM[0x2231] >> 2) & 7);
					int32	depth = (Memory.FillRAM[0x2231] & 3) == 0 ? 8 : (Memory.FillRAM[0x2231] & 3) == 1 ? 4 : 2;
					int32	bytes_per_char = 8 * depth;
					int32	bytes_per_line = depth * num_chars;
					int32	char_line_bytes = bytes_per_char * num_chars;
					uint32	addr = (d->AAddress / char_line_bytes) * char_line_bytes;

					uint8	*base = S9xGetBasePointer((d->ABank << 16) + addr);
					if (!base)
					{
						//"SA-1: DMA from non-block address $%02X:%04X", d->ABank, addr
						base = Memory.ROM;
					}

					base += addr;

					uint8	*buffer = &Memory.ROM[MAX_ROM_SIZE - 0x10000];
					uint8	*p = buffer;
					uint32	inc_sa1 = char_line_bytes - (d->AAddress % char_line_bytes);
					uint32	char_count = inc_sa1 / bytes_per_char;

					in_sa1_dma = TRUE;

					switch (depth)
					{
						case 2:
							for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
							{
								uint8	*line = base + (num_chars - char_count) * 2;
								for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 2)
								{
									uint8	*q = line;
									for (int32 l = 0; l < 8; l++, q += bytes_per_line)
									{
										for (int32 b = 0; b < 2; b++)
										{
											uint8	r = *(q + b);
											*(p + 0) = (*(p + 0) << 1) | ((r >> 0) & 1);
											*(p + 1) = (*(p + 1) << 1) | ((r >> 1) & 1);
											*(p + 0) = (*(p + 0) << 1) | ((r >> 2) & 1);
											*(p + 1) = (*(p + 1) << 1) | ((r >> 3) & 1);
											*(p + 0) = (*(p + 0) << 1) | ((r >> 4) & 1);
											*(p + 1) = (*(p + 1) << 1) | ((r >> 5) & 1);
											*(p + 0) = (*(p + 0) << 1) | ((r >> 6) & 1);
											*(p + 1) = (*(p + 1) << 1) | ((r >> 7) & 1);
										}

										p += 2;
									}
								}
							}

							break;

						case 4:
							for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
							{
								uint8	*line = base + (num_chars - char_count) * 4;
								for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 4)
								{
									uint8	*q = line;
									for (int32 l = 0; l < 8; l++, q += bytes_per_line)
									{
										for (int32 b = 0; b < 4; b++)
										{
											uint8	r = *(q + b);
											*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
											*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
											*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
											*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
											*(p +  0) = (*(p +  0) << 1) | ((r >> 4) & 1);
											*(p +  1) = (*(p +  1) << 1) | ((r >> 5) & 1);
											*(p + 16) = (*(p + 16) << 1) | ((r >> 6) & 1);
											*(p + 17) = (*(p + 17) << 1) | ((r >> 7) & 1);
										}

										p += 2;
									}

									p += 32 - 16;
								}
							}

							break;

						case 8:
							for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
							{
								uint8	*line = base + (num_chars - char_count) * 8;
								for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 8)
								{
									uint8	*q = line;
									for (int32 l = 0; l < 8; l++, q += bytes_per_line)
									{
										for (int32 b = 0; b < 8; b++)
										{
											uint8	r = *(q + b);
											*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
											*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
											*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
											*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
											*(p + 32) = (*(p + 32) << 1) | ((r >> 4) & 1);
											*(p + 33) = (*(p + 33) << 1) | ((r >> 5) & 1);
											*(p + 48) = (*(p + 48) << 1) | ((r >> 6) & 1);
											*(p + 49) = (*(p + 49) << 1) | ((r >> 7) & 1);
										}

										p += 2;
									}

									p += 64 - 16;
								}
							}

							break;
					}
				}
			}
		}

		// Do Transfer

		uint8	Work;

		// 8 cycles per channel
		CPU.Cycles += SLOW_ONE_CYCLE;

		if (!d->ReverseTransfer)
		{
			// CPU -> PPU
			int32	b = 0;
			uint16	p = d->AAddress;
			uint8	*base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);

			int32	rem = count;
			// Transfer per block if d->AAdressFixed is FALSE
			count = d->AAddressFixed ? rem : (d->AAddressDecrement ? ((p & MEMMAP_MASK) + 1) : (MEMMAP_BLOCK_SIZE - (p & MEMMAP_MASK)));

			// Settings for custom chip DMA
			if(special_chips_active)
			{
				if (in_sa1_dma)
				{
					base = &Memory.ROM[MAX_ROM_SIZE - 0x10000];
					p = 0;
					count = rem;
				}
				else
					if (in_sdd1_dma)
					{
						base = in_sdd1_dma;
						p = 0;
						count = rem;
					}
					else if (spc7110_dma)
					{
						base = spc7110_dma;
						p = 0;
						count = rem;
					}
			}

				bool8 inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
						(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));


			// 8 cycles per byte
#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			p += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				continue; \
			}

			do
			{
				if (count > rem)
					count = rem;
				rem -= count;

				CPU.InWRAMDMAorHDMA = inWRAM_DMA;

				if (!base)
				{
					// DMA SLOW PATH
					if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
					{
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
						}while(--count > 0);
					}
					else
						if (d->TransferMode == 1 || d->TransferMode == 5)
						{
							// This is a variation on Duff's Device. It is legal C/C++.
							switch (b)
							{
								default:
									while (count > 1)
									{
										Work = S9xGetByte((d->ABank << 16) + p);
										S9xSetPPU(Work, 0x2100 + d->BAddress);
										UPDATE_COUNTERS;
										count--;

										case 1:
										Work = S9xGetByte((d->ABank << 16) + p);
										S9xSetPPU(Work, 0x2101 + d->BAddress);
										UPDATE_COUNTERS;
										count--;
									}
							}

							if (count == 1)
							{
								Work = S9xGetByte((d->ABank << 16) + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
						else
							if (d->TransferMode == 3 || d->TransferMode == 7)
							{
								switch (b)
								{
									default:
										do
										{
											Work = S9xGetByte((d->ABank << 16) + p);
											S9xSetPPU(Work, 0x2100 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 1;
												break;
											}

											case 1:
											Work = S9xGetByte((d->ABank << 16) + p);
											S9xSetPPU(Work, 0x2100 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 2;
												break;
											}

											case 2:
											Work = S9xGetByte((d->ABank << 16) + p);
											S9xSetPPU(Work, 0x2101 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 3;
												break;
											}

											case 3:
											Work = S9xGetByte((d->ABank << 16) + p);
											S9xSetPPU(Work, 0x2101 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 0;
												break;
											}
										}while(1);
								}
							}
							else
								if (d->TransferMode == 4)
								{
									switch (b)
									{
										default:
											do
											{
												Work = S9xGetByte((d->ABank << 16) + p);
												S9xSetPPU(Work, 0x2100 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 1;
													break;
												}

												case 1:
												Work = S9xGetByte((d->ABank << 16) + p);
												S9xSetPPU(Work, 0x2101 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 2;
													break;
												}

												case 2:
												Work = S9xGetByte((d->ABank << 16) + p);
												S9xSetPPU(Work, 0x2102 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 3;
													break;
												}

												case 3:
												Work = S9xGetByte((d->ABank << 16) + p);
												S9xSetPPU(Work, 0x2103 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 0;
													break;
												}
											}while(1);
									}
								}
				}
				else
				{
					// DMA FAST PATH
					if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
					{
						switch (d->BAddress)
						{
							case 0x04: // OAMDATA
								do
								{
									Work = *(base + p);
									REGISTER_2104(Work);
									UPDATE_COUNTERS;
								}while(--count > 0);

								break;

							case 0x18: // VMDATAL
#ifndef CORRECT_VRAM_READS
								IPPU.FirstVRAMRead = TRUE;
#endif
								if (!PPU.VMA.FullGraphicCount)
								{
									do
									{
										Work = *(base + p);
										if(CHECK_INBLANK)
											REGISTER_2118_linear(Work);
										UPDATE_COUNTERS;
									} while (--count > 0);
								}
								else
								{
									do
									{
										Work = *(base + p);
										if(CHECK_INBLANK)
											REGISTER_2118_tile(Work);
										UPDATE_COUNTERS;
									} while (--count > 0);
								}

								break;

							case 0x19: // VMDATAH
#ifndef CORRECT_VRAM_READS
								IPPU.FirstVRAMRead = TRUE;
#endif
								if (!PPU.VMA.FullGraphicCount)
								{
									do
									{
										Work = *(base + p);
										if(CHECK_INBLANK)
											REGISTER_2119_linear(Work);
										UPDATE_COUNTERS;
									} while (--count > 0);
								}
								else
								{
									do
									{
										Work = *(base + p);
										if(CHECK_INBLANK)
											REGISTER_2119_tile(Work);
										UPDATE_COUNTERS;
									} while (--count > 0);
								}

								break;

							case 0x22: // CGDATA
								do
								{
									Work = *(base + p);
									REGISTER_2122(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);

								break;

							case 0x80: // WMDATA
								if (!CPU.InWRAMDMAorHDMA)
								{
									do
									{
										Work = *(base + p);
										REGISTER_2180(Work);
										UPDATE_COUNTERS;
									} while (--count > 0);
								}
								else
								{
									do
									{
										UPDATE_COUNTERS;
									} while (--count > 0);
								}

								break;

							default:
								do
								{
									Work = *(base + p);
									S9xSetPPU(Work, 0x2100 + d->BAddress);
									UPDATE_COUNTERS;
								} while (--count > 0);

								break;
						}
					}
					else
						if (d->TransferMode == 1 || d->TransferMode == 5)
						{
							if (d->BAddress == 0x18)
							{
								// VMDATAL
#ifndef CORRECT_VRAM_READS
								IPPU.FirstVRAMRead = TRUE;
#endif
								if (!PPU.VMA.FullGraphicCount)
								{
									switch (b)
									{
										default:
											while (count > 1)
											{
												Work = *(base + p);
												REGISTER_2118_linear(Work);
												UPDATE_COUNTERS;
												count--;

												case 1:
												Work = *(base + p);
												if(CHECK_INBLANK)
													REGISTER_2119_linear(Work);
												UPDATE_COUNTERS;
												count--;
											}
									}

									if (count == 1)
									{
										Work = *(base + p);
										REGISTER_2118_linear(Work);
										UPDATE_COUNTERS;
										b = 1;
									}
									else
										b = 0;
								}
								else
								{
									switch (b)
									{
										default:
											while (count > 1)
											{
												Work = *(base + p);
												REGISTER_2118_tile(Work);
												UPDATE_COUNTERS;
												count--;

												case 1:
												Work = *(base + p);
												if(CHECK_INBLANK)
													REGISTER_2119_tile(Work);
												UPDATE_COUNTERS;
												count--;
											}
									}

									if (count == 1)
									{
										Work = *(base + p);
										REGISTER_2118_tile(Work);
										UPDATE_COUNTERS;
										b = 1;
									}
									else
										b = 0;
								}
							}
							else
							{
								// DMA mode 1 general case
								switch (b)
								{
									default:
										while (count > 1)
										{
											Work = *(base + p);
											S9xSetPPU(Work, 0x2100 + d->BAddress);
											UPDATE_COUNTERS;
											count--;

											case 1:
											Work = *(base + p);
											S9xSetPPU(Work, 0x2101 + d->BAddress);
											UPDATE_COUNTERS;
											count--;
										}
								}

								if (count == 1)
								{
									Work = *(base + p);
									S9xSetPPU(Work, 0x2100 + d->BAddress);
									UPDATE_COUNTERS;
									b = 1;
								}
								else
									b = 0;
							}
						}
						else
							if (d->TransferMode == 3 || d->TransferMode == 7)
							{
								switch (b)
								{
									default:
										do
										{
											Work = *(base + p);
											S9xSetPPU(Work, 0x2100 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 1;
												break;
											}

											case 1:
											Work = *(base + p);
											S9xSetPPU(Work, 0x2100 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 2;
												break;
											}

											case 2:
											Work = *(base + p);
											S9xSetPPU(Work, 0x2101 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 3;
												break;
											}

											case 3:
											Work = *(base + p);
											S9xSetPPU(Work, 0x2101 + d->BAddress);
											UPDATE_COUNTERS;
											if (--count <= 0)
											{
												b = 0;
												break;
											}
										} while (1);
								}
							}
							else
								if (d->TransferMode == 4)
								{
									switch (b)
									{
										default:
											do
											{
												Work = *(base + p);
												S9xSetPPU(Work, 0x2100 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 1;
													break;
												}

												case 1:
												Work = *(base + p);
												S9xSetPPU(Work, 0x2101 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 2;
													break;
												}

												case 2:
												Work = *(base + p);
												S9xSetPPU(Work, 0x2102 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 3;
													break;
												}

												case 3:
												Work = *(base + p);
												S9xSetPPU(Work, 0x2103 + d->BAddress);
												UPDATE_COUNTERS;
												if (--count <= 0)
												{
													b = 0;
													break;
												}
											} while (1);
									}
								}
				}

				if (rem <= 0)
					break;

				base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
				count = MEMMAP_BLOCK_SIZE;
				inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
						(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
			}while(1);

#undef UPDATE_COUNTERS
		}
		else
		{
			// PPU -> CPU

			// 8 cycles per byte
#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				continue; \
			}

			if (d->BAddress > 0x80 - 4 && d->BAddress <= 0x83 && !(d->ABank & 0x40))
			{
				// REVERSE-DMA REALLY-SLOW PATH
				do
				{
					switch (d->TransferMode)
					{
						case 0:
						case 2:
						case 6:
							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 1:
						case 5:
							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 3:
						case 7:
							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 4:
							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2102 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
							Work = S9xGetPPU(0x2103 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						default:
							while (count)
							{
								UPDATE_COUNTERS;
								count--;
							}

							break;
					}
				} while (count);
			}
			else
			{
				// REVERSE-DMA FASTER PATH
				CPU.InWRAMDMAorHDMA = (d->ABank == 0x7e || d->ABank == 0x7f);
				do
				{
					switch (d->TransferMode)
					{
						case 0:
						case 2:
						case 6:
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 1:
						case 5:
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 3:
						case 7:
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						case 4:
							Work = S9xGetPPU(0x2100 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2101 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2102 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							if (!--count)
								break;

							Work = S9xGetPPU(0x2103 + d->BAddress);
							S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
							UPDATE_COUNTERS;
							count--;

							break;

						default:
							while (count)
							{
								UPDATE_COUNTERS;
								count--;
							}

							break;
					}
				} while (count);
			}
		}

		if ((CPU.Flags & NMI_FLAG) && (Timings.NMITriggerPos != 0xffff))
		{
			Timings.NMITriggerPos = CPU.Cycles + Timings.NMIDMADelay;
			if (Timings.NMITriggerPos >= Timings.H_Max)
				Timings.NMITriggerPos -= Timings.H_Max;
		}

		// Release the memory used in SPC7110 DMA
		if (Settings.SPC7110)
		{
			if (spc7110_dma)
				free(spc7110_dma);
		}

		CPU.InDMA = FALSE;
		CPU.InDMAorHDMA = FALSE;
		CPU.InWRAMDMAorHDMA = FALSE;
		CPU.CurrentDMAorHDMAChannel = -1;
	}
}

void S9xSetCPU (uint8 Byte, uint16 Address)
{
	if ((Address & 0xff80) == 0x4300)
	{
		if (CPU.InDMAorHDMA)
			return;

		int	d = (Address >> 4) & 0x7;

		switch (Address & 0xf)
		{
			case 0x0: // 0x43x0: DMAPx
				DMA[d].ReverseTransfer        = (Byte & 0x80) ? TRUE : FALSE;
				DMA[d].HDMAIndirectAddressing = (Byte & 0x40) ? TRUE : FALSE;
				DMA[d].UnusedBit43x0          = (Byte & 0x20) ? TRUE : FALSE;
				DMA[d].AAddressDecrement      = (Byte & 0x10) ? TRUE : FALSE;
				DMA[d].AAddressFixed          = (Byte & 0x08) ? TRUE : FALSE;
				DMA[d].TransferMode           = (Byte & 7);
				return;

			case 0x1: // 0x43x1: BBADx
				DMA[d].BAddress = Byte;
				return;

			case 0x2: // 0x43x2: A1TxL
				DMA[d].AAddress &= 0xff00;
				DMA[d].AAddress |= Byte;
				return;

			case 0x3: // 0x43x3: A1TxH
				DMA[d].AAddress &= 0xff;
				DMA[d].AAddress |= Byte << 8;
				return;

			case 0x4: // 0x43x4: A1Bx
				DMA[d].ABank = Byte;
				HDMAMemPointers[d] = NULL;
				return;

			case 0x5: // 0x43x5: DASxL
				DMA[d].DMACount_Or_HDMAIndirectAddress &= 0xff00;
				DMA[d].DMACount_Or_HDMAIndirectAddress |= Byte;
				HDMAMemPointers[d] = NULL;
				return;

			case 0x6: // 0x43x6: DASxH
				DMA[d].DMACount_Or_HDMAIndirectAddress &= 0xff;
				DMA[d].DMACount_Or_HDMAIndirectAddress |= Byte << 8;
				HDMAMemPointers[d] = NULL;
				return;

			case 0x7: // 0x43x7: DASBx
				DMA[d].IndirectBank = Byte;
				HDMAMemPointers[d] = NULL;
				return;

			case 0x8: // 0x43x8: A2AxL
				DMA[d].Address &= 0xff00;
				DMA[d].Address |= Byte;
				HDMAMemPointers[d] = NULL;
				return;

			case 0x9: // 0x43x9: A2AxH
				DMA[d].Address &= 0xff;
				DMA[d].Address |= Byte << 8;
				HDMAMemPointers[d] = NULL;
				return;

			case 0xa: // 0x43xa: NLTRx
				if (Byte & 0x7f)
				{
					DMA[d].LineCount = Byte & 0x7f;
					DMA[d].Repeat = !(Byte & 0x80);
				}
				else
				{
					DMA[d].LineCount = 128;
					DMA[d].Repeat = !!(Byte & 0x80);
				}

				return;

			case 0xb: // 0x43xb: ????x
			case 0xf: // 0x43xf: mirror of 0x43xb
				DMA[d].UnknownByte = Byte;
				return;

			default:
				break;
		}
	}
	else
	{
		uint16	pos;

		switch (Address)
		{
			case 0x4200: // NMITIMEN
				if (Byte & 0x20)
					PPU.VTimerEnabled = TRUE;
				else
					PPU.VTimerEnabled = FALSE;

				if (Byte & 0x10)
					PPU.HTimerEnabled = TRUE;
				else
					PPU.HTimerEnabled = FALSE;

				S9xUpdateHVTimerPosition();

				// The case that IRQ will trigger in an instruction such as STA $4200.
				// FIXME: not true but good enough for Snes9x, I think.
				if ((PPU.HTimerPosition >= CPU.PrevCycles) && (PPU.HTimerPosition < (CPU.PrevCycles + (CPU.Cycles - CPU.PrevCycles))))
				{
					//Check Missing H Timer Range
					bool v_counter_eq_vtimer_pos = (CPU.V_Counter == PPU.VTimerPosition);
					if ((PPU.HTimerEnabled && (!PPU.VTimerEnabled || v_counter_eq_vtimer_pos)) || (PPU.VTimerEnabled && v_counter_eq_vtimer_pos))
					{
						S9X_SET_IRQ(PPU_IRQ_SOURCE);
					}
				}

				if (!(Byte & 0x30))
				{
					S9X_CLEAR_IRQ(PPU_IRQ_SOURCE);
				}

				// NMI can trigger immediately during VBlank as long as NMI_read ($4210) wasn't cleard.
				if ((Byte & 0x80) && !(Memory.FillRAM[0x4200] & 0x80) &&
					(CPU.V_Counter >= PPU.ScreenHeight + FIRST_VISIBLE_LINE) && (Memory.FillRAM[0x4210] & 0x80))
				{
					// FIXME: triggered at HC+=6, checked just before the final CPU cycle,
					// then, when to call S9xOpcode_NMI()?
					CPU.Flags |= NMI_FLAG;
					Timings.NMITriggerPos = CPU.Cycles + 6 + 6;
				}

				break;

			case 0x4201: // WRIO
				if ((Byte & 0x80) == 0 && (Memory.FillRAM[0x4213] & 0x80) == 0x80)
				{
					S9xLatchCounters();
					if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
						PPU.GunVLatch = 1000;
				}
				else if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
				{
					bool force = (Byte & 0x80) ? true : false;
					if (force || (Memory.FillRAM[0x4213] & 0x80))
					{
						S9X_TRY_GUN_LATCH();
					}
					PPU.GunVLatch = 1000;
				}
				Memory.FillRAM[0x4201] = Memory.FillRAM[0x4213] = Byte;
				break;

			case 0x4202: // WRMPYA
				break;

			case 0x4203: // WRMPYB
			{
				uint32 res = Memory.FillRAM[0x4202] * Byte;
				// FIXME: The update occurs 8 machine cycles after $4203 is set.
				Memory.FillRAM[0x4216] = (uint8) res;
				Memory.FillRAM[0x4217] = (uint8) (res >> 8);
				break;
			}

			case 0x4204: // WRDIVL
			case 0x4205: // WRDIVH
				break;

			case 0x4206: // WRDIVB
			{
				uint16 a = Memory.FillRAM[0x4204] + (Memory.FillRAM[0x4205] << 8);
				uint16 div = Byte ? a / Byte : 0xffff;
				uint16 rem = Byte ? a % Byte : a;
				// FIXME: The update occurs 16 machine cycles after $4206 is set.
				Memory.FillRAM[0x4214] = (uint8) div;
				Memory.FillRAM[0x4215] = div >> 8;
				Memory.FillRAM[0x4216] = (uint8) rem;
				Memory.FillRAM[0x4217] = rem >> 8;
				break;
			}

			case 0x4207: // HTIMEL
				pos = PPU.IRQHBeamPos;
				PPU.IRQHBeamPos = (PPU.IRQHBeamPos & 0xff00) | Byte;
				if (PPU.IRQHBeamPos != pos)
					S9xUpdateHVTimerPosition();
				break;

			case 0x4208: // HTIMEH
				pos = PPU.IRQHBeamPos;
				PPU.IRQHBeamPos = (PPU.IRQHBeamPos & 0xff) | ((Byte & 1) << 8);
				if (PPU.IRQHBeamPos != pos)
					S9xUpdateHVTimerPosition();
				break;

			case 0x4209: // VTIMEL
				pos = PPU.IRQVBeamPos;
				PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff00) | Byte;
				if (PPU.IRQVBeamPos != pos)
					S9xUpdateHVTimerPosition();
				break;

			case 0x420a: // VTIMEH
				pos = PPU.IRQVBeamPos;
				PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff) | ((Byte & 1) << 8);
				if (PPU.IRQVBeamPos != pos)
					S9xUpdateHVTimerPosition();
				break;

			case 0x420b: // MDMAEN
				if (CPU.InDMAorHDMA)
					return;
				// XXX: Not quite right...
				if (Byte)
					CPU.Cycles += Timings.DMACPUSync;

				memset(dma_channels_to_be_used, 0, 8 * sizeof(uint8));

				if (Settings.SPC7110 || Settings.SDD1 || Settings.SA1)
					special_chips_active = true;
				else
					special_chips_active = false;

				if (Byte & 0x01)
					dma_channels_to_be_used[0] = 1;
				if (Byte & 0x02)
					dma_channels_to_be_used[1] = 1;
				if (Byte & 0x04)
					dma_channels_to_be_used[2] = 1;
				if (Byte & 0x08)
					dma_channels_to_be_used[3] = 1;
				if (Byte & 0x10)
					dma_channels_to_be_used[4] = 1;
				if (Byte & 0x20)
					dma_channels_to_be_used[5] = 1;
				if (Byte & 0x40)
					dma_channels_to_be_used[6] = 1;
				if (Byte & 0x80)
					dma_channels_to_be_used[7] = 1;
				S9xDoDMA();
				break;

			case 0x420c: // HDMAEN
				if (CPU.InDMAorHDMA)
					return;
				Memory.FillRAM[0x420c] = Byte;
				// Yoshi's Island, Genjyu Ryodan, Mortal Kombat, Tales of Phantasia
				PPU.HDMA = Byte & ~PPU.HDMAEnded;
				break;

			case 0x420d: // MEMSEL
				if ((Byte & 1) != (Memory.FillRAM[0x420d] & 1))
				{
					if (Byte & 1)
						CPU.FastROMSpeed = ONE_CYCLE;
					else
						CPU.FastROMSpeed = SLOW_ONE_CYCLE;
				}

				break;

			case 0x4210: // RDNMI
			case 0x4211: // TIMEUP
			case 0x4212: // HVBJOY
			case 0x4213: // RDIO
			case 0x4214: // RDDIVL
			case 0x4215: // RDDIVH
			case 0x4216: // RDMPYL
			case 0x4217: // RDMPYH
			case 0x4218: // JOY1L
			case 0x4219: // JOY1H
			case 0x421a: // JOY2L
			case 0x421b: // JOY2H
			case 0x421c: // JOY3L
			case 0x421d: // JOY3H
			case 0x421e: // JOY4L
			case 0x421f: // JOY4H
				return;

			default:
				if (Settings.SPC7110 && Address >= 0x4800)
					S9xSetSPC7110(Byte, Address);
				else
				if (Settings.SDD1 && Address >= 0x4804 && Address <= 0x4807)
					S9xSetSDD1MemoryMap(Address - 0x4804, Byte & 7);
				break;
		}
	}

	Memory.FillRAM[Address] = Byte;
}

static inline uint8 REGISTER_4212 (void)
{
	uint8	byte = 0;

	if ((CPU.V_Counter >= PPU.ScreenHeight + FIRST_VISIBLE_LINE) && (CPU.V_Counter < PPU.ScreenHeight + FIRST_VISIBLE_LINE + 3))
		byte = 1;
	if ((CPU.Cycles < Timings.HBlankEnd) || (CPU.Cycles >= Timings.HBlankStart))
		byte |= 0x40;
	if (CPU.V_Counter >= PPU.ScreenHeight + FIRST_VISIBLE_LINE)
		byte |= 0x80;

	return (byte);
}

uint8 S9xGetCPU (uint16 Address)
{
	int	d = (Address >> 4) & 0x7;

	switch (Address & 0xf)
	{
		case 0x0: // 0x43x0: DMAPx
			return ((DMA[d].ReverseTransfer        ? 0x80 : 0) |
					(DMA[d].HDMAIndirectAddressing ? 0x40 : 0) |
					(DMA[d].UnusedBit43x0          ? 0x20 : 0) |
					(DMA[d].AAddressDecrement      ? 0x10 : 0) |
					(DMA[d].AAddressFixed          ? 0x08 : 0) |
					(DMA[d].TransferMode & 7));

		case 0x1: // 0x43x1: BBADx
			return (DMA[d].BAddress);

		case 0x2: // 0x43x2: A1TxL
			return (DMA[d].AAddress & 0xff);

		case 0x3: // 0x43x3: A1TxH
			return (DMA[d].AAddress >> 8);

		case 0x4: // 0x43x4: A1Bx
			return (DMA[d].ABank);

		case 0x5: // 0x43x5: DASxL
			return (DMA[d].DMACount_Or_HDMAIndirectAddress & 0xff);

		case 0x6: // 0x43x6: DASxH
			return (DMA[d].DMACount_Or_HDMAIndirectAddress >> 8);

		case 0x7: // 0x43x7: DASBx
			return (DMA[d].IndirectBank);

		case 0x8: // 0x43x8: A2AxL
			return (DMA[d].Address & 0xff);

		case 0x9: // 0x43x9: A2AxH
			return (DMA[d].Address >> 8);

		case 0xa: // 0x43xa: NLTRx
			return (DMA[d].LineCount ^ (DMA[d].Repeat ? 0x00 : 0x80));

		case 0xb: // 0x43xb: ????x
		case 0xf: // 0x43xf: mirror of 0x43xb
			return (DMA[d].UnknownByte);

		default:
			return (OpenBus);
	}
}

uint8 S9xGetCPU_Alt(uint16 Address)
{
	uint8	byte;

	switch (Address)
	{
		case 0x4210: // RDNMI
			byte = Memory.FillRAM[0x4210];
			Memory.FillRAM[0x4210] = MAX_5A22_VERSION;
			return ((byte & 0x80) | (OpenBus & 0x70) | MAX_5A22_VERSION);

		case 0x4211: // TIMEUP
			byte = (CPU.IRQActive & PPU_IRQ_SOURCE) ? 0x80 : 0;
			S9X_CLEAR_IRQ(PPU_IRQ_SOURCE);
			return (byte | (OpenBus & 0x7f));
		case 0x4212: // HVBJOY
			return (REGISTER_4212() | (OpenBus & 0x3e));
		case 0x4213: // RDIO
			return (Memory.FillRAM[0x4213]);
		case 0x4214: // RDDIVL
		case 0x4215: // RDDIVH
		case 0x4216: // RDMPYL
		case 0x4217: // RDMPYH
			return (Memory.FillRAM[Address]);
		case 0x4218: // JOY1L
		case 0x4219: // JOY1H
		case 0x421a: // JOY2L
		case 0x421b: // JOY2H
		case 0x421c: // JOY3L
		case 0x421d: // JOY3H
		case 0x421e: // JOY4L
		case 0x421f: // JOY4H
			if (Memory.FillRAM[0x4200] & 1)
				pad_read = TRUE;
			return (Memory.FillRAM[Address]);

		default:
			if (Settings.SPC7110 && Address >= 0x4800)
				return (S9xGetSPC7110(Address));
			if (Settings.SDD1 && Address >= 0x4800 && Address <= 0x4807)
				return (Memory.FillRAM[Address]);
			return (OpenBus);
	}
}

void S9xResetPPU (void)
{
	S9xSoftResetPPU();
	S9xControlsReset();
	PPU.M7HOFS = 0;
	PPU.M7VOFS = 0;
	PPU.M7byte = 0;
}

void S9xSoftResetPPU (void)
{
	S9xControlsSoftReset();

	PPU.VMA.High = 0;
	PPU.VMA.Increment = 1;
	PPU.VMA.Address = 0;
	PPU.VMA.FullGraphicCount = 0;
	PPU.VMA.Shift = 0;

	PPU.WRAM = 0;

	for (int c = 0; c < 4; c++)
	{
		PPU.BG[c].SCBase = 0;
		PPU.BG[c].HOffset = 0;
		PPU.BG[c].VOffset = 0;
		PPU.BG[c].BGSize = 0;
		PPU.BG[c].NameBase = 0;
		PPU.BG[c].SCSize = 0;
	}

	PPU.BGMode = 0;
	PPU.BG3Priority = 0;

	PPU.CGFLIP = 0;
	PPU.CGFLIPRead = 0;
	PPU.CGADD = 0;

	for (int c = 0; c < 256; c++)
	{
		IPPU.Red[c]   = (c & 7) << 2;
		IPPU.Green[c] = ((c >> 3) & 7) << 2;
		IPPU.Blue[c]  = ((c >> 6) & 2) << 3;
		PPU.CGDATA[c] = IPPU.Red[c] | (IPPU.Green[c] << 5) | (IPPU.Blue[c] << 10);
	}

	for (int c = 0; c < 128; c++)
	{
		PPU.OBJ[c].HPos = 0;
		PPU.OBJ[c].VPos = 0;
		PPU.OBJ[c].HFlip = 0;
		PPU.OBJ[c].VFlip = 0;
		PPU.OBJ[c].Name = 0;
		PPU.OBJ[c].Priority = 0;
		PPU.OBJ[c].Palette = 0;
		PPU.OBJ[c].Size = 0;
	}

	PPU.OBJThroughMain = FALSE;
	PPU.OBJThroughSub = FALSE;
	PPU.OBJAddition = FALSE;
	PPU.OBJNameBase = 0;
	PPU.OBJNameSelect = 0;
	PPU.OBJSizeSelect = 0;

	PPU.OAMAddr = 0;
	PPU.SavedOAMAddr = 0;
	PPU.OAMPriorityRotation = 0;
	PPU.OAMFlip = 0;
	PPU.OAMReadFlip = 0;
	PPU.OAMTileAddress = 0;
	PPU.OAMWriteRegister = 0;
	ZeroMemory(PPU.OAMData, 512 + 32);

	PPU.FirstSprite = 0;
	PPU.LastSprite = 127;
	PPU.RangeTimeOver = 0;

	PPU.HTimerEnabled = FALSE;
	PPU.VTimerEnabled = FALSE;
	PPU.HTimerPosition = Timings.H_Max + 1;
	PPU.VTimerPosition = Timings.V_Max + 1;
	PPU.IRQHBeamPos = 0x1ff;
	PPU.IRQVBeamPos = 0x1ff;

	PPU.HBeamFlip = 0;
	PPU.VBeamFlip = 0;
	PPU.HBeamPosLatched = 0;
	PPU.VBeamPosLatched = 0;
	PPU.GunHLatch = 0;
	PPU.GunVLatch = 1000;
	PPU.HVBeamCounterLatched = 0;

	PPU.Mode7HFlip = FALSE;
	PPU.Mode7VFlip = FALSE;
	PPU.Mode7Repeat = 0;
	PPU.MatrixA = 0;
	PPU.MatrixB = 0;
	PPU.MatrixC = 0;
	PPU.MatrixD = 0;
	PPU.CentreX = 0;
	PPU.CentreY = 0;

	PPU.Mosaic = 0;
	PPU.BGMosaic[0] = FALSE;
	PPU.BGMosaic[1] = FALSE;
	PPU.BGMosaic[2] = FALSE;
	PPU.BGMosaic[3] = FALSE;
	
	PPU.Window1Left = 1;
	PPU.Window1Right = 0;
	PPU.Window2Left = 1;
	PPU.Window2Right = 0;
	PPU.RecomputeClipWindows = TRUE;

	for (int c = 0; c < 6; c++)
	{
		PPU.ClipCounts[c] = 0;
		PPU.ClipWindowOverlapLogic[c] = CLIP_OR;
		PPU.ClipWindow1Enable[c] = FALSE;
		PPU.ClipWindow2Enable[c] = FALSE;
		PPU.ClipWindow1Inside[c] = TRUE;
		PPU.ClipWindow2Inside[c] = TRUE;
	}

	PPU.ForcedBlanking = TRUE;

	PPU.FixedColourRed = 0;
	PPU.FixedColourGreen = 0;
	PPU.FixedColourBlue = 0;
	PPU.Brightness = 0;
	PPU.ScreenHeight = SNES_HEIGHT;

	PPU.Need16x8Mulitply = FALSE;
	PPU.BGnxOFSbyte = 0;

	PPU.HDMA = 0;
	PPU.HDMAEnded = 0;

	PPU.OpenBus1 = 0;
	PPU.OpenBus2 = 0;

	for (int c = 0; c < 2; c++)
		memset(&IPPU.Clip[c], 0, sizeof(struct ClipData));
	IPPU.OBJChanged = TRUE;
	IPPU.DirectColourMapsNeedRebuild = TRUE;
	ZeroMemory(IPPU.TileCached[TILE_2BIT], MAX_2BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_4BIT], MAX_4BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_8BIT], MAX_8BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_2BIT_EVEN], MAX_2BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_2BIT_ODD],  MAX_2BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_4BIT_EVEN], MAX_4BIT_TILES);
	ZeroMemory(IPPU.TileCached[TILE_4BIT_ODD],  MAX_4BIT_TILES);
#ifdef CORRECT_VRAM_READS
	IPPU.VRAMReadBuffer = 0; // XXX: FIXME: anything better?
#else
	IPPU.FirstVRAMRead = FALSE;
#endif
	IPPU.Interlace = FALSE;
	IPPU.InterlaceOBJ = FALSE;
	IPPU.DoubleWidthPixels = FALSE;
	IPPU.DoubleHeightPixels = FALSE;
	IPPU.CurrentLine = 0;
	IPPU.PreviousLine = 0;
	IPPU.XB = NULL;
	for (int c = 0; c < 256; c++)
		IPPU.ScreenColors[c] = c;
	IPPU.MaxBrightness = 0;
	IPPU.RenderedScreenWidth = SNES_WIDTH;
	IPPU.RenderedScreenHeight = SNES_HEIGHT;

	S9xFixColourBrightness();

	for (int c = 0; c < 0x8000; c += 0x100)
		memset(&Memory.FillRAM[c], c >> 8, 0x100);
	ZeroMemory(&Memory.FillRAM[0x2100], 0x100);
	ZeroMemory(&Memory.FillRAM[0x4200], 0x100);
	ZeroMemory(&Memory.FillRAM[0x4000], 0x100);
	// For BS Suttehakkun 2...
	ZeroMemory(&Memory.FillRAM[0x1000], 0x1000);

	Memory.FillRAM[0x4201] = Memory.FillRAM[0x4213] = 0xff;
}
