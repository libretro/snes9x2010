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
#include "ppu.h"
#include "memmap.h"
#include "dma.h"
#include "apu/apu.h"
#include "fxemu.h"
#include "sdd1.h"
#include "srtc.h"
#include "tile.h"
#include "controls.h"
#include "crosshairs.h"
#include "cheats.h"
#include "font.h"
#include "display.h"
#include "clip.h"
#ifdef NETPLAY_SUPPORT
#include "netplay.h"
#endif
#include "shiftmul.h"

//BEGINNING OF SNES9x PS3
#include <stdio.h>
#include <stdlib.h>
#include "../ps3video.hpp"
extern PS3Graphics* Graphics;
#include "isel.h"
//END OF SNES9X PS3

extern uint8	*HDMAMemPointers[8];

extern struct SCheatData		Cheat;
extern struct SLineData			LineData[240];
extern struct SLineMatrixData	LineMatrixData[240];

void S9xComputeClipWindows (void);

static int	font_width = 8, font_height = 9;

#define TILE_PLUS(t, x)	(((t) & 0xfc00) | ((t + x) & 0x3ff))

static void SetupOBJ (void)
{
	int	SmallWidth, SmallHeight, LargeWidth, LargeHeight;

	uint64_t smallwidth_array[16]  = {8 , 8 , 8 , 16, 16, 32, 16, 16, 32, 32, 32, 32, 32, 32, 32, 32};
	uint64_t smallheight_array[16] = {8 , 8 , 8 , 16, 16, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};
	uint64_t largewidth_array[16]  = {16, 32, 64, 32, 64, 64, 32, 32, 64, 64, 64, 64, 64, 64, 64, 64};
	uint64_t largeheight_array[16] = {16, 32, 64, 32, 64, 64, 64, 32, 64, 64, 64, 64, 64, 64, 64, 64};

	uint8_t objsizeselect = PPU.OBJSizeSelect;
	SmallWidth = smallwidth_array[objsizeselect];
	SmallHeight = smallheight_array[objsizeselect];
	LargeWidth = largewidth_array[objsizeselect];
	LargeHeight = largeheight_array[objsizeselect];

	int	inc = IPPU.InterlaceOBJ ? 2 : 1;

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
			for (int j = 0; j < 32; j += 4)
			{
				GFX.OBJLines[i].OBJ[j+0].Sprite = -1;
				GFX.OBJLines[i].OBJ[j+1].Sprite = -1;
				GFX.OBJLines[i].OBJ[j+2].Sprite = -1;
				GFX.OBJLines[i].OBJ[j+3].Sprite = -1;
			}
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

				for (uint8 line = 0, Y = (uint8) (PPU.OBJ[S].VPos & 0xff); line < Height; Y++, line += inc)
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

				for (uint8 line = 0, Y = (uint8) (PPU.OBJ[S].VPos & 0xff); line < Height; Y++, line += inc)
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
			int	TileLine = mul<8>(GFX.OBJLines[Y].OBJ[I].Line & 7);
			if (IPPU.InterlaceOBJ)
				TileLine >>= 1;
			int	TileInc = 1;

			if (PPU.OBJ[S].HFlip)
			{
				TileX = (TileX + (GFX.OBJWidths[S] >> 3) - 1) & 0x0f;
				BaseTile |= H_FLIP;
				TileInc = -1;
			}

			GFX.Z2 = D + mul<4>(PPU.OBJ[S].Priority);

			int	DrawMode = 3;
			int	clip = 0, next_clip = -1000;
			int	X = PPU.OBJ[S].HPos;
			if (X == -256)
				X = 256;

			for (int t = tiles, O = Offset + X * PixWidth; X <= 256 && X < PPU.OBJ[S].HPos + GFX.OBJWidths[S]; TileX = (TileX + TileInc) & 0x0f, X += 8, O += mul<8>(PixWidth))
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

static void DisplayStringFromBottom (const char *string, int linesFromBottom, int pixelsFromLeft, bool allowWrap)
{
	if (linesFromBottom <= 0)
		linesFromBottom = 1;

	uint16	*dst = GFX.Screen + (IPPU.RenderedScreenHeight - font_height * linesFromBottom) * GFX.RealPPL + pixelsFromLeft;

	int	len = strlen(string);
	int	max_chars = IPPU.RenderedScreenWidth / (font_width - 1);
	int	char_count = 0;

	for (int i = 0 ; i < len ; i++, char_count++)
	{
		if (char_count >= max_chars || (uint8) string[i] < 32)
		{
			if (!allowWrap)
				break;

			dst += font_height * GFX.RealPPL - (font_width - 1) * max_chars;
			if (dst >= GFX.Screen + IPPU.RenderedScreenHeight * GFX.RealPPL)
				break;

			char_count -= max_chars;
		}

		if ((uint8) string[i] < 32)
			continue;

		S9xDisplayChar(dst, string[i]);
		dst += font_width - 1;
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
			uint32	Y2 = IPPU.Interlace ? mul<2>(Y) + GFX.InterlaceFrame : Y;
			uint32	VOffset = LineData[Y].BG[bg].VOffset + (IPPU.Interlace ? 1 : 0);
			uint32	HOffset = LineData[Y].BG[bg].HOffset;
			int		VirtAlign = ((Y2 + VOffset) & 7) >> (IPPU.Interlace ? 1 : 0);

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
				Offset += mul<8>(PixWidth);
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
				Offset += mul<8>(PixWidth);
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
			uint32	Y2 = IPPU.Interlace ? (mul<2>(Y)) : Y;
			uint32	VOffset = LineData[Y].BG[bg].VOffset + (IPPU.Interlace ? 1 : 0);
			uint32	HOffset = LineData[Y].BG[bg].HOffset;

			Lines = PPU.Mosaic - MosaicStart;
			if (Y + MosaicStart + Lines > GFX.EndY)
				Lines = GFX.EndY - Y - MosaicStart + 1;

			int	VirtAlign = (((Y2 + VOffset) & 7) >> (IPPU.Interlace ? 1 : 0)) << 3;

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

		for (uint32 Y = GFX.StartY; Y <= GFX.EndY; Y++)
		{
			uint32	Y2 = IPPU.Interlace ? (mul<2>(Y)) + GFX.InterlaceFrame : Y;
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

				if (IPPU.Interlace)
					VOffset++;

				uint32	t1, t2;
				int		VirtAlign = (((Y2 + VOffset) & 7) >> (IPPU.Interlace ? 1 : 0)) << 3;
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
				Offset += mul<8>(PixWidth);
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
			uint32	Y2 = IPPU.Interlace ? (mul<2>(Y)) : Y;
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

				if (IPPU.Interlace)
					VOffset++;

				uint32	t1, t2;
				int		VirtAlign = (((Y2 + VOffset) & 7) >> (IPPU.Interlace ? 1 : 0)) << 3;
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

#if 0
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
#endif

/*
static inline void DrawBackdrop (void)
{
	uint32	Offset = GFX.StartY * GFX.PPL;

	for (int clip = 0; clip < GFX.Clip[5].Count; clip++)
	{
		GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1);

		if (BG.EnableMath && (GFX.Clip[5].DrawMode[clip] & 2))
			GFX.DrawBackdropMath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
		else
			GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
	}
}
*/

static inline void S9xLatchCounters (bool force)
{
	if (force || (Memory.FillRAM[0x4213] & 0x80))
	{
		// Latch h and v counters, like the gun
#if 0 // #ifdef CPU_SHUTDOWN
		CPU.WaitAddress = CPU.PCAtOpcodeStart;
#endif

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
				hc -= 2;
			if (hc >= 1308)
				hc -= 2;
		}

		PPU.HBeamPosLatched = (uint16) (hc >> 2);

		Memory.FillRAM[0x213f] |= 0x40;
	}

	if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && (CPU.Cycles >= PPU.GunHLatch << 2)))
		PPU.GunVLatch = 1000;
}

static inline void S9xTryGunLatch (bool force)
{
	if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= (PPU.GunHLatch << 2)))
	{
		if (force || (Memory.FillRAM[0x4213] & 0x80))
		{
#if 0 // #ifdef CPU_SHUTDOWN
			CPU.WaitAddress = CPU.PCAtOpcodeStart;
#endif

			PPU.HVBeamCounterLatched = 1;
			PPU.VBeamPosLatched = (uint16) PPU.GunVLatch;
			PPU.HBeamPosLatched = (uint16) PPU.GunHLatch;

			Memory.FillRAM[0x213f] |= 0x40;
		}

		PPU.GunVLatch = 1000;
	}
}

/*
void S9xCheckMissingHTimerPosition (int32 hc)
{
	if (PPU.HTimerPosition == hc)
	{
		if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
			S9xSetIRQ(PPU_IRQ_SOURCE);
		else
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
			S9xSetIRQ(PPU_IRQ_SOURCE);
	}
}
*/

/*
void S9xCheckMissingHTimerHalt (int32 hc_from, int32 range)
{
	if ((PPU.HTimerPosition >= hc_from) && (PPU.HTimerPosition < (hc_from + range)))
	{
		if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
			CPU.IRQPending = 1;
		else
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
			CPU.IRQPending = 1;
	}
}
*/

/*
void S9xCheckMissingHTimerRange (int32 hc_from, int32 range)
{
	if ((PPU.HTimerPosition >= hc_from) && (PPU.HTimerPosition < (hc_from + range)))
	{
		if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
			S9xSetIRQ(PPU_IRQ_SOURCE);
		else
		if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
			S9xSetIRQ(PPU_IRQ_SOURCE);
	}
}
*/

/*
void S9xUpdateHVTimerPosition (void)
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
					PPU.HTimerPosition += 2;
				if (PPU.IRQHBeamPos > 326)
					PPU.HTimerPosition += 2;
			}

			PPU.HTimerPosition += 14;
			// /IRQ
			PPU.HTimerPosition += 4;
			// after CPU executing
			PPU.HTimerPosition += 6;
		}
		else
			PPU.HTimerPosition = 10 + 4 + 6;
	}
	else
		PPU.HTimerPosition = 10 + 4 + 6;

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
				CPU.NextEvent  = SNES_RENDER_START_HC;
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
				CPU.NextEvent  = SNES_RENDER_START_HC;
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
*/

/*
void S9xFixColourBrightness (void)
{
	IPPU.XB = mul_brightness[PPU.Brightness];

	for (int i = 0; i < 256; i += 4)
	{
		IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i+0])       & 0x1f];
		IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i+0] >>  5) & 0x1f];
		IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i+0] >> 10) & 0x1f];
		IPPU.ScreenColors[i] = BUILD_PIXEL(IPPU.Red[i+0], IPPU.Green[i+0], IPPU.Blue[i+0]);

		IPPU.Red[i+1]   = IPPU.XB[(PPU.CGDATA[i+1])       & 0x1f];
		IPPU.Green[i+1] = IPPU.XB[(PPU.CGDATA[i+1] >>  5) & 0x1f];
		IPPU.Blue[i+1]  = IPPU.XB[(PPU.CGDATA[i+1] >> 10) & 0x1f];
		IPPU.ScreenColors[i+1] = BUILD_PIXEL(IPPU.Red[i+1], IPPU.Green[i+1], IPPU.Blue[i+1]);

		IPPU.Red[i+2]   = IPPU.XB[(PPU.CGDATA[i+2])       & 0x1f];
		IPPU.Green[i+2] = IPPU.XB[(PPU.CGDATA[i+2] >>  5) & 0x1f];
		IPPU.Blue[i+2]  = IPPU.XB[(PPU.CGDATA[i+2] >> 10) & 0x1f];
		IPPU.ScreenColors[i+2] = BUILD_PIXEL(IPPU.Red[i+2], IPPU.Green[i+2], IPPU.Blue[i+2]);

		IPPU.Red[i+3]   = IPPU.XB[(PPU.CGDATA[i+3])       & 0x1f];
		IPPU.Green[i+3] = IPPU.XB[(PPU.CGDATA[i+3] >>  5) & 0x1f];
		IPPU.Blue[i+3]  = IPPU.XB[(PPU.CGDATA[i+3] >> 10) & 0x1f];
		IPPU.ScreenColors[i+3] = BUILD_PIXEL(IPPU.Red[i+3], IPPU.Green[i+3], IPPU.Blue[i+3]);

	}
}
*/

void S9xSetPPU_SuperFX (uint8 Byte, uint16 Address)
{
	// MAP_PPU: $2000-$3FFF

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
#if 0
			// S9xSetPPU() is called to write to DMA[].AAddress
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
	else
		if (Address <= 0x2183)
		{
			switch (Address)
			{
				case 0x2100: // INIDISP
					if (Byte != Memory.FillRAM[0x2100])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();

						if (PPU.Brightness != (Byte & 0xf))
						{
							//IPPU.ColorsChanged = TRUE;
							IPPU.DirectColourMapsNeedRebuild = TRUE;
							PPU.Brightness = Byte & 0xf;

#if 1
							//begin of S9xFixColourBrightness
							//S9xFixColourBrightness();
							IPPU.XB = mul_brightness[PPU.Brightness];

							for (int i = 0; i < 256; i += 4)
							{
								IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i])       & 0x1f];
								IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i] >>  5) & 0x1f];
								IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i] >> 10) & 0x1f];
								IPPU.ScreenColors[i] = (((int) (IPPU.Red[i]) << 10) | ((int) (IPPU.Green[i]) << 5) | (int) (IPPU.Blue[i]));

								IPPU.Red[i+1]   = IPPU.XB[(PPU.CGDATA[i+1])       & 0x1f];
								IPPU.Green[i+1] = IPPU.XB[(PPU.CGDATA[i+1] >>  5) & 0x1f];
								IPPU.Blue[i+1]  = IPPU.XB[(PPU.CGDATA[i+1] >> 10) & 0x1f];
								IPPU.ScreenColors[i+1] = (((int) (IPPU.Red[i+1]) << 10) | ((int) (IPPU.Green[i+1]) << 5) | (int) (IPPU.Blue[i+1]));

								IPPU.Red[i+2]   = IPPU.XB[(PPU.CGDATA[i+2])       & 0x1f];
								IPPU.Green[i+2] = IPPU.XB[(PPU.CGDATA[i+2] >>  5) & 0x1f];
								IPPU.Blue[i+2]  = IPPU.XB[(PPU.CGDATA[i+2] >> 10) & 0x1f];
								IPPU.ScreenColors[i+2] = (((int) (IPPU.Red[i+2]) << 10) | ((int) (IPPU.Green[i+2]) << 5) | (int) (IPPU.Blue[i+2]));

								IPPU.Red[i+3]   = IPPU.XB[(PPU.CGDATA[i+3])       & 0x1f];
								IPPU.Green[i+3] = IPPU.XB[(PPU.CGDATA[i+3] >>  5) & 0x1f];
								IPPU.Blue[i+3]  = IPPU.XB[(PPU.CGDATA[i+3] >> 10) & 0x1f];
								IPPU.ScreenColors[i+3] = (((int) (IPPU.Red[i+3]) << 10) | ((int) (IPPU.Green[i+3]) << 5) | (int) (IPPU.Blue[i+3]));
							}
							//end of S9xFixColourBrightness
#endif

							if (PPU.Brightness > IPPU.MaxBrightness)
								IPPU.MaxBrightness = PPU.Brightness;
						}

						if ((Memory.FillRAM[0x2100] & 0x80) != (Byte & 0x80))
						{
							//IPPU.ColorsChanged = TRUE;
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
					if (PPU.OAMAddr & 0x100)
					{
						int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
						if (Byte != PPU.OAMData[addr])
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
							PPU.OAMData[addr] = Byte;
							IPPU.OBJChanged = TRUE;

							// X position high bit, and sprite size (x4)
							struct SOBJ *pObj = &PPU.OBJ[((addr & 0x1f) << 2)];
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
					else
						if (!(PPU.OAMFlip & 1))
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
								if (IPPU.PreviousLine != IPPU.CurrentLine)
									S9xUpdateScreen();
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
					break;

				case 0x2105: // BGMODE
					if (Byte != Memory.FillRAM[0x2105])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.BG[0].BGSize = (Byte >> 4) & 1;
						PPU.BG[1].BGSize = (Byte >> 5) & 1;
						PPU.BG[2].BGSize = (Byte >> 6) & 1;
						PPU.BG[3].BGSize = (Byte >> 7) & 1;
						PPU.BGMode = Byte & 7;
						// BJ: BG3Priority only takes effect if BGMode == 1 and the bit is set
						PPU.BG3Priority = ((Byte & 0x0f) == 0x09);
						/* 
						   if (PPU.BGMode == 5 || PPU.BGMode == 6)
						   IPPU.Interlace = Memory.FillRAM[0x2133] & 1;
						   else
						 */
						IPPU.Interlace = 0;
					}

					break;

				case 0x2106: // MOSAIC
					if (Byte != Memory.FillRAM[0x2106])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.BG[Address-8455].SCSize = Byte & 3;
						PPU.BG[Address-8455].SCBase = (Byte & 0x7c) << 8;
					}

					break;
				case 0x210b: // BG12NBA
				case 0x210c: // BG34NBA
					if (Byte != Memory.FillRAM[Address])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.BG[((Address-0x210b)<<1)].NameBase = (Byte & 7) << 12;
						PPU.BG[((Address-0x210b)<<1)+1].NameBase = ((Byte >> 4) & 7) << 12;
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
						PPU.VMA.Mask1 = (mul<8>(IncCount[i])) - 1;
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
					//CHECK_INBLANK();

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
					break;

				case 0x2119: // VMDATAH
#ifndef CORRECT_VRAM_READS
					IPPU.FirstVRAMRead = TRUE;
#endif
					//Begin of REGISTER 2119 (Byte)
					//CHECK_INBLANK();

					uint32	addresstemp;

					if (PPU.VMA.FullGraphicCount)
					{
						uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
						addresstemp = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;
						Memory.VRAM[addresstemp] = Byte;
					}
					else
						Memory.VRAM[addresstemp = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Byte;

					IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

					if (PPU.VMA.High)
						PPU.VMA.Address += PPU.VMA.Increment;
					break;

				case 0x211a: // M7SEL
					if (Byte != Memory.FillRAM[0x211a])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
					if (PPU.CGFLIP)
					{
						if ((Byte & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen_SuperFX();
							PPU.CGDATA[PPU.CGADD] &= 0x00ff;
							PPU.CGDATA[PPU.CGADD] |= (Byte & 0x7f) << 8;
							//IPPU.ColorsChanged = TRUE;
							IPPU.Blue[PPU.CGADD] = IPPU.XB[(Byte >> 2) & 0x1f];
							IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
							IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
						}

						PPU.CGADD++;
					}
					else
					{
						if (Byte != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen_SuperFX();
							PPU.CGDATA[PPU.CGADD] &= 0x7f00;
							PPU.CGDATA[PPU.CGADD] |= Byte;
							//IPPU.ColorsChanged = TRUE;
							IPPU.Red[PPU.CGADD] = IPPU.XB[Byte & 0x1f];
							IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
							IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
						}
					}

					PPU.CGFLIP ^= 1;

					break;

				case 0x2123: // W12SEL
					if (Byte != Memory.FillRAM[0x2123])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.Window1Left = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2127: // WH1
					if (Byte != Memory.FillRAM[0x2127])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.Window1Right = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2128: // WH2
					if (Byte != Memory.FillRAM[0x2128])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.Window2Left = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2129: // WH3
					if (Byte != Memory.FillRAM[0x2129])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.Window2Right = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x212a: // WBGLOG
					if (Byte != Memory.FillRAM[0x212a])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2131: // CGADSUB
					if (Byte != Memory.FillRAM[0x2131])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
					}

					break;

				case 0x2132: // COLDATA
					if (Byte != Memory.FillRAM[0x2132])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen_SuperFX();
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
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen_SuperFX();
							IPPU.PseudoHires = Byte & 8;
						}

#if 0
						if (Byte & 0x04)
						{
							PPU.ScreenHeight = SNES_HEIGHT_EXTENDED;
							/*
							   if (IPPU.DoubleHeightPixels)
							   IPPU.RenderedScreenHeight = PPU.ScreenHeight << 1;
							   else
							 */
							IPPU.RenderedScreenHeight = PPU.ScreenHeight;
						}
						else
#endif
							PPU.ScreenHeight = SNES_HEIGHT;

						if ((Memory.FillRAM[0x2133] ^ Byte) & 3)
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen_SuperFX();
							if ((Memory.FillRAM[0x2133] ^ Byte) & 2)
								IPPU.OBJChanged = TRUE;
#if 0
							if (PPU.BGMode == 5 || PPU.BGMode == 6)
								IPPU.Interlace = Byte & 1;
							IPPU.InterlaceOBJ = Byte & 2;
#endif
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
					{
						//REGISTER_2180(Byte);
						Memory.RAM[PPU.WRAM++] = Byte;
						PPU.WRAM &= 0x1ffff;
					}
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
			if (Address >= 0x3000 && Address <= 0x32ff)
			{
				S9xSetSuperFX(Byte, Address);
				return;
			}
		}

	Memory.FillRAM[Address] = Byte;
}

void S9xSetPPU (uint8 Byte, uint16 Address)
{
	// MAP_PPU: $2000-$3FFF

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
#if 0
			// S9xSetPPU() is called to write to DMA[].AAddress
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
	else
		if (Address <= 0x2183)
		{
			switch (Address)
			{
				case 0x2100: // INIDISP
					if (Byte != Memory.FillRAM[0x2100])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();

						if (PPU.Brightness != (Byte & 0xf))
						{
							//IPPU.ColorsChanged = TRUE;
							IPPU.DirectColourMapsNeedRebuild = TRUE;
							PPU.Brightness = Byte & 0xf;

#if 1
							//begin of S9xFixColourBrightness
							//S9xFixColourBrightness();
							IPPU.XB = mul_brightness[PPU.Brightness];

							for (int i = 0; i < 256; i += 4)
							{
								IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i])       & 0x1f];
								IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i] >>  5) & 0x1f];
								IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i] >> 10) & 0x1f];
								IPPU.ScreenColors[i] = (((int) (IPPU.Red[i]) << 10) | ((int) (IPPU.Green[i]) << 5) | (int) (IPPU.Blue[i]));

								IPPU.Red[i+1]   = IPPU.XB[(PPU.CGDATA[i+1])       & 0x1f];
								IPPU.Green[i+1] = IPPU.XB[(PPU.CGDATA[i+1] >>  5) & 0x1f];
								IPPU.Blue[i+1]  = IPPU.XB[(PPU.CGDATA[i+1] >> 10) & 0x1f];
								IPPU.ScreenColors[i+1] = (((int) (IPPU.Red[i+1]) << 10) | ((int) (IPPU.Green[i+1]) << 5) | (int) (IPPU.Blue[i+1]));

								IPPU.Red[i+2]   = IPPU.XB[(PPU.CGDATA[i+2])       & 0x1f];
								IPPU.Green[i+2] = IPPU.XB[(PPU.CGDATA[i+2] >>  5) & 0x1f];
								IPPU.Blue[i+2]  = IPPU.XB[(PPU.CGDATA[i+2] >> 10) & 0x1f];
								IPPU.ScreenColors[i+2] = (((int) (IPPU.Red[i+2]) << 10) | ((int) (IPPU.Green[i+2]) << 5) | (int) (IPPU.Blue[i+2]));

								IPPU.Red[i+3]   = IPPU.XB[(PPU.CGDATA[i+3])       & 0x1f];
								IPPU.Green[i+3] = IPPU.XB[(PPU.CGDATA[i+3] >>  5) & 0x1f];
								IPPU.Blue[i+3]  = IPPU.XB[(PPU.CGDATA[i+3] >> 10) & 0x1f];
								IPPU.ScreenColors[i+3] = (((int) (IPPU.Red[i+3]) << 10) | ((int) (IPPU.Green[i+3]) << 5) | (int) (IPPU.Blue[i+3]));
							}
							//end of S9xFixColourBrightness
#endif

							if (PPU.Brightness > IPPU.MaxBrightness)
								IPPU.MaxBrightness = PPU.Brightness;
						}

						if ((Memory.FillRAM[0x2100] & 0x80) != (Byte & 0x80))
						{
							//IPPU.ColorsChanged = TRUE;
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
					if (PPU.OAMAddr & 0x100)
					{
						int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
						if (Byte != PPU.OAMData[addr])
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
							PPU.OAMData[addr] = Byte;
							IPPU.OBJChanged = TRUE;

							// X position high bit, and sprite size (x4)
							struct SOBJ *pObj = &PPU.OBJ[((addr & 0x1f) << 2)];
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
					else
						if (!(PPU.OAMFlip & 1))
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
								if (IPPU.PreviousLine != IPPU.CurrentLine)
									S9xUpdateScreen();
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
					break;

				case 0x2105: // BGMODE
					if (Byte != Memory.FillRAM[0x2105])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.BG[0].BGSize = (Byte >> 4) & 1;
						PPU.BG[1].BGSize = (Byte >> 5) & 1;
						PPU.BG[2].BGSize = (Byte >> 6) & 1;
						PPU.BG[3].BGSize = (Byte >> 7) & 1;
						PPU.BGMode = Byte & 7;
						// BJ: BG3Priority only takes effect if BGMode == 1 and the bit is set
						PPU.BG3Priority = ((Byte & 0x0f) == 0x09);
						if (PPU.BGMode == 5 || PPU.BGMode == 6)
							IPPU.Interlace = Memory.FillRAM[0x2133] & 1;
						else
							IPPU.Interlace = 0;
					}

					break;

				case 0x2106: // MOSAIC
					if (Byte != Memory.FillRAM[0x2106])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.BG[Address-8455].SCSize = Byte & 3;
						PPU.BG[Address-8455].SCBase = (Byte & 0x7c) << 8;
					}

					break;
				case 0x210b: // BG12NBA
				case 0x210c: // BG34NBA
					if (Byte != Memory.FillRAM[Address])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.BG[((Address-0x210b)<<1)].NameBase = (Byte & 7) << 12;
						PPU.BG[((Address-0x210b)<<1)+1].NameBase = ((Byte >> 4) & 7) << 12;
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
						PPU.VMA.Mask1 = (mul<8>(IncCount[i])) - 1;
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
					//CHECK_INBLANK();

					uint32	addresstemp;

					if (PPU.VMA.FullGraphicCount)
					{
						uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
						addresstemp = (((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) & 0xffff;
						Memory.VRAM[addresstemp] = Byte;
					}
					else
						Memory.VRAM[addresstemp = (PPU.VMA.Address << 1) & 0xffff] = Byte;

					IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

					if (!PPU.VMA.High)
					{
						PPU.VMA.Address += PPU.VMA.Increment;
					}
					break;

				case 0x2119: // VMDATAH
#ifndef CORRECT_VRAM_READS
					IPPU.FirstVRAMRead = TRUE;
#endif
					//REGISTER_2119(Byte);
					//Begin of REGISTER 2119 (Byte)
					//CHECK_INBLANK();

					//uint32	addresstemp;

					if (PPU.VMA.FullGraphicCount)
					{
						uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
						addresstemp = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;
						Memory.VRAM[addresstemp] = Byte;
					}
					else
						Memory.VRAM[addresstemp = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Byte;

					IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
					IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
					IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

					if (PPU.VMA.High)
						PPU.VMA.Address += PPU.VMA.Increment;
					break;

				case 0x211a: // M7SEL
					if (Byte != Memory.FillRAM[0x211a])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
					if (PPU.CGFLIP)
					{
						if ((Byte & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
							PPU.CGDATA[PPU.CGADD] &= 0x00ff;
							PPU.CGDATA[PPU.CGADD] |= (Byte & 0x7f) << 8;
							//IPPU.ColorsChanged = TRUE;
							IPPU.Blue[PPU.CGADD] = IPPU.XB[(Byte >> 2) & 0x1f];
							IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
							IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
						}

						PPU.CGADD++;
					}
					else
					{
						if (Byte != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
						{
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
							PPU.CGDATA[PPU.CGADD] &= 0x7f00;
							PPU.CGDATA[PPU.CGADD] |= Byte;
							//IPPU.ColorsChanged = TRUE;
							IPPU.Red[PPU.CGADD] = IPPU.XB[Byte & 0x1f];
							IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
							IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
						}
					}

					PPU.CGFLIP ^= 1;
					break;

				case 0x2123: // W12SEL
					if (Byte != Memory.FillRAM[0x2123])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.Window1Left = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2127: // WH1
					if (Byte != Memory.FillRAM[0x2127])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.Window1Right = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2128: // WH2
					if (Byte != Memory.FillRAM[0x2128])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.Window2Left = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x2129: // WH3
					if (Byte != Memory.FillRAM[0x2129])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.Window2Right = Byte;
						PPU.RecomputeClipWindows = TRUE;
					}

					break;

				case 0x212a: // WBGLOG
					if (Byte != Memory.FillRAM[0x212a])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
						PPU.RecomputeClipWindows = TRUE;
					}
					break;
				case 0x2131: // CGADSUB
					if (Byte != Memory.FillRAM[0x2131])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
					}

					break;

				case 0x2132: // COLDATA
					if (Byte != Memory.FillRAM[0x2132])
					{
						if (IPPU.PreviousLine != IPPU.CurrentLine)
							S9xUpdateScreen();
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
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
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
							if (IPPU.PreviousLine != IPPU.CurrentLine)
								S9xUpdateScreen();
							if ((Memory.FillRAM[0x2133] ^ Byte) & 2)
								IPPU.OBJChanged = TRUE;
							if (PPU.BGMode == 5 || PPU.BGMode == 6)
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
					{
						//REGISTER_2180(Byte);
						Memory.RAM[PPU.WRAM++] = Byte;
						PPU.WRAM &= 0x1ffff;
					}
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
			else
				if (Settings.SA1     && Address >= 0x2200)
				{
					if (Address <= 0x23ff)
						S9xSetSA1(Byte, Address);
					else
						Memory.FillRAM[Address] = Byte;
					return;
				}
				else
					if (Settings.BS      && Address >= 0x2188 && Address <= 0x219f)
						S9xSetBSXPPU(Byte, Address);
					else
						if (Settings.SRTC    && Address == 0x2801)
							S9xSetSRTC(Byte, Address);
		}

	Memory.FillRAM[Address] = Byte;
}

uint8 S9xGetPPU_SuperFX (uint16 Address)
{
	// MAP_PPU: $2000-$3FFF

	if (Address < 0x2100)
		return (OpenBus);

	if (CPU.InDMAorHDMA)
	{
		if (CPU.CurrentDMAorHDMAChannel >= 0 && !DMA[CPU.CurrentDMAorHDMAChannel].ReverseTransfer)
		{
			// S9xGetPPU() is called to read from DMA[].AAddress
			if ((Address & 0xff00) == 0x2100)
				// Cannot access to Address Bus B ($2100-$21FF) via (H)DMA
				return (OpenBus);
			else
				// $2200-$3FFF are connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
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
		// read_port will run the APU until given APU time before reading value
		return (S9xAPUReadPort(Address & 3));
	else
		if (Address <= 0x2183)
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
					S9xLatchCounters(0);
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
					S9xTryGunLatch(false);
					if (PPU.HBeamFlip)
						byte = (PPU.OpenBus2 & 0xfe) | ((PPU.HBeamPosLatched >> 8) & 0x01);
					else
						byte = (uint8) PPU.HBeamPosLatched;
					PPU.HBeamFlip ^= 1;
					return (PPU.OpenBus2 = byte);

				case 0x213d: // OPVCT
					S9xTryGunLatch(false);
					if (PPU.VBeamFlip)
						byte = (PPU.OpenBus2 & 0xfe) | ((PPU.VBeamPosLatched >> 8) & 0x01);
					else
						byte = (uint8) PPU.VBeamPosLatched;
					PPU.VBeamFlip ^= 1;
					return (PPU.OpenBus2 = byte);

				case 0x213e: // STAT77
					if (IPPU.PreviousLine != IPPU.CurrentLine)
						S9xUpdateScreen();
					byte = (PPU.OpenBus1 & 0x10) | PPU.RangeTimeOver | MAX_5C77_VERSION;
					return (PPU.OpenBus1 = byte);

				case 0x213f: // STAT78
					S9xTryGunLatch(false);
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
			if (Address >= 0x3000 && Address <= 0x32ff)
			{
				uint8	byte;

				byte = Memory.FillRAM[Address];
#ifdef CPU_SHUTDOWN
				if (Address == 0x3030)
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
				if (Address == 0x3031)
				{
					//S9xClearIRQ(GSU_IRQ_SOURCE);
					CPU.IRQActive &= ~GSU_IRQ_SOURCE;
					if (!CPU.IRQActive)
						CPU.Flags &= ~IRQ_FLAG;

					Memory.FillRAM[0x3031] = byte & 0x7f;
				}

				return (byte);
			}
			else
				return (OpenBus);
#if 0
			switch (Address)
			{
				case 0x21c2:
#if 0
					if (Model->_5C77 == 2)
						return (0x20);
#endif
					return (OpenBus);

				case 0x21c3:
#if 0
					if (Model->_5C77 == 2)
						return (0);
#endif
					return (OpenBus);

				default:
					return (OpenBus);
			}
#endif
		}
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
			if ((Address & 0xff00) == 0x2100)
				// Cannot access to Address Bus B ($2100-$21FF) via (H)DMA
				return (OpenBus);
			else
				// $2200-$3FFF are connected to Address Bus A
				// SA1, SuperFX and SRTC are mapped here
				// I don't bother for now...
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
		// read_port will run the APU until given APU time before reading value
		return (S9xAPUReadPort(Address & 3));
	else
		if (Address <= 0x2183)
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
					S9xLatchCounters(0);
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
					S9xTryGunLatch(false);
					if (PPU.HBeamFlip)
						byte = (PPU.OpenBus2 & 0xfe) | ((PPU.HBeamPosLatched >> 8) & 0x01);
					else
						byte = (uint8) PPU.HBeamPosLatched;
					PPU.HBeamFlip ^= 1;
					return (PPU.OpenBus2 = byte);

				case 0x213d: // OPVCT
					S9xTryGunLatch(false);
					if (PPU.VBeamFlip)
						byte = (PPU.OpenBus2 & 0xfe) | ((PPU.VBeamPosLatched >> 8) & 0x01);
					else
						byte = (uint8) PPU.VBeamPosLatched;
					PPU.VBeamFlip ^= 1;
					return (PPU.OpenBus2 = byte);

				case 0x213e: // STAT77
					if (IPPU.PreviousLine != IPPU.CurrentLine)
						S9xUpdateScreen();
					byte = (PPU.OpenBus1 & 0x10) | PPU.RangeTimeOver | MAX_5C77_VERSION;
					return (PPU.OpenBus1 = byte);

				case 0x213f: // STAT78
					S9xTryGunLatch(false);
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
			{
				uint8	byte;

				byte = Memory.FillRAM[Address];
#ifdef CPU_SHUTDOWN
				if (Address == 0x3030)
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
				if (Address == 0x3031)
				{
					//S9xClearIRQ(GSU_IRQ_SOURCE);
					CPU.IRQActive &= ~GSU_IRQ_SOURCE;
					if (!CPU.IRQActive)
						CPU.Flags &= ~IRQ_FLAG;

					Memory.FillRAM[0x3031] = byte & 0x7f;
				}

				return (byte);
			}
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
#if 0
			switch (Address)
			{
				case 0x21c2:
#if 0
					if (Model->_5C77 == 2)
						return (0x20);
#endif
					return (OpenBus);

				case 0x21c3:
#if 0
					if (Model->_5C77 == 2)
						return (0);
#endif
					return (OpenBus);

				default:
					return (OpenBus);
			}
#endif
		}
}

void S9xSetCPU_NoSA1 (uint8 Byte, uint16 Address)
{
	if (Address < 0x4200)
	{
		switch (Address)
		{
			case 0x4016: // JOYSER0
				S9xSetJoypadLatch(Byte & 1);
				break;

			case 0x4017: // JOYSER1
				return;

			default:
				break;
		}
	}
	else
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

			uint64_t enable_vtimer, enable_htimer;
			switch (Address)
			{
				case 0x4200: // NMITIMEN
					enable_vtimer = (Byte & 0x20);
					PPU.VTimerEnabled = enable_vtimer;

					enable_htimer = (Byte & 0x10);
					PPU.HTimerEnabled = enable_htimer;

#if 1
					//begin of S9xUpdateHVTimerPosition
					//S9xUpdateHVTimerPosition();
					if (PPU.HTimerEnabled)
					{
						if (PPU.IRQHBeamPos != 0)
						{
							// IRQ_read
							//replace * ONE_DOT_CYCLE with << 2
							PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
							if (Timings.H_Max == Timings.H_Max_Master)	// 1364
							{
								if (PPU.IRQHBeamPos > 322)
									PPU.HTimerPosition += 2;
								if (PPU.IRQHBeamPos > 326)
									PPU.HTimerPosition += 2;
							}

							PPU.HTimerPosition += 14;
							// /IRQ
							PPU.HTimerPosition += 4;
							// after CPU executing
							PPU.HTimerPosition += 6;
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
								CPU.NextEvent  = SNES_RENDER_START_HC;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
					//end of S9xUpdateHVTimerPosition
#endif

					// The case that IRQ will trigger in an instruction such as STA $4200.
					// FIXME: not true but good enough for Snes9x, I think.
					//S9xCheckMissingHTimerRange(CPU.PrevCycles, CPU.Cycles - CPU.PrevCycles);
					if ((PPU.HTimerPosition >= CPU.PrevCycles) && (PPU.HTimerPosition < (CPU.PrevCycles + (CPU.Cycles - CPU.PrevCycles))))
					{
						if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
						{
							//S9xSetIRQ(PPU_IRQ_SOURCE);
							CPU.IRQActive |= PPU_IRQ_SOURCE;
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
						else
							if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
							{
								//S9xSetIRQ(PPU_IRQ_SOURCE);
								CPU.IRQActive |= PPU_IRQ_SOURCE;
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
					}

					if (!(Byte & 0x30))
					{
						//S9xClearIRQ(PPU_IRQ_SOURCE);
						CPU.IRQActive &= ~PPU_IRQ_SOURCE;
						if (!CPU.IRQActive)
							CPU.Flags &= ~IRQ_FLAG;
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
						S9xLatchCounters(1);
					else
						S9xTryGunLatch((Byte & 0x80) ? true : false);
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
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								//replace * ONE_DOT_CYCLE with << 2
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4208: // HTIMEH
					pos = PPU.IRQHBeamPos;
					PPU.IRQHBeamPos = (PPU.IRQHBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQHBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4209: // VTIMEL
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff00) | Byte;
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420a: // VTIMEH
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420b: // MDMAEN
					if (CPU.InDMAorHDMA)
						return;
					// XXX: Not quite right...
					if (Byte)
						CPU.Cycles += Timings.DMACPUSync;
					if (Byte & 0x01)
						S9xDoDMA_NoSA1(0);
					if (Byte & 0x02)
						S9xDoDMA_NoSA1(1);
					if (Byte & 0x04)
						S9xDoDMA_NoSA1(2);
					if (Byte & 0x08)
						S9xDoDMA_NoSA1(3);
					if (Byte & 0x10)
						S9xDoDMA_NoSA1(4);
					if (Byte & 0x20)
						S9xDoDMA_NoSA1(5);
					if (Byte & 0x40)
						S9xDoDMA_NoSA1(6);
					if (Byte & 0x80)
						S9xDoDMA_NoSA1(7);
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
						{
							CPU.FastROMSpeed = ONE_CYCLE;
						}
						else
							CPU.FastROMSpeed = SLOW_ONE_CYCLE;
					}

					break;

				case 0x4210: // RDNMI
#if 0
					Memory.FillRAM[0x4210] = Model->_5A22;
#endif
					return;

				case 0x4211: // TIMEUP
#if 0
					//S9xClearIRQ(PPU_IRQ_SOURCE);
					CPU.IRQActive &= ~PPU_IRQ_SOURCE;
					if (!CPU.IRQActive)
						CPU.Flags &= ~IRQ_FLAG;
#endif
					return;

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
					break;
			}
		}

	Memory.FillRAM[Address] = Byte;
}

void S9xSetCPU_SuperFX (uint8 Byte, uint16 Address)
{
	if (Address < 0x4200)
	{
		switch (Address)
		{
			case 0x4016: // JOYSER0
				S9xSetJoypadLatch(Byte & 1);
				break;

			case 0x4017: // JOYSER1
				return;

			default:
				break;
		}
	}
	else
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

			uint64_t enable_vtimer, enable_htimer;
			switch (Address)
			{
				case 0x4200: // NMITIMEN
					enable_vtimer = (Byte & 0x20);
					PPU.VTimerEnabled = enable_vtimer;

					enable_htimer = (Byte & 0x10);
					PPU.HTimerEnabled = enable_htimer;

#if 1
					//begin of S9xUpdateHVTimerPosition
					//S9xUpdateHVTimerPosition();
					if (PPU.HTimerEnabled)
					{
						if (PPU.IRQHBeamPos != 0)
						{
							// IRQ_read
							//replace * ONE_DOT_CYCLE with << 2
							PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
							if (Timings.H_Max == Timings.H_Max_Master)	// 1364
							{
								if (PPU.IRQHBeamPos > 322)
									PPU.HTimerPosition += 2;
								if (PPU.IRQHBeamPos > 326)
									PPU.HTimerPosition += 2;
							}

							PPU.HTimerPosition += 14;
							// /IRQ
							PPU.HTimerPosition += 4;
							// after CPU executing
							PPU.HTimerPosition += 6;
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
								CPU.NextEvent  = SNES_RENDER_START_HC;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
					//end of S9xUpdateHVTimerPosition
#endif

					// The case that IRQ will trigger in an instruction such as STA $4200.
					// FIXME: not true but good enough for Snes9x, I think.
					//S9xCheckMissingHTimerRange(CPU.PrevCycles, CPU.Cycles - CPU.PrevCycles);
					if ((PPU.HTimerPosition >= CPU.PrevCycles) && (PPU.HTimerPosition < (CPU.PrevCycles + (CPU.Cycles - CPU.PrevCycles))))
					{
						if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
						{
							//S9xSetIRQ(PPU_IRQ_SOURCE);
							CPU.IRQActive |= PPU_IRQ_SOURCE;
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
						else
							if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
							{
								//S9xSetIRQ(PPU_IRQ_SOURCE);
								CPU.IRQActive |= PPU_IRQ_SOURCE;
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
					}

					if (!(Byte & 0x30))
					{
						//S9xClearIRQ(PPU_IRQ_SOURCE);
						CPU.IRQActive &= ~PPU_IRQ_SOURCE;
						if (!CPU.IRQActive)
							CPU.Flags &= ~IRQ_FLAG;
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
						S9xLatchCounters(1);
					else
						S9xTryGunLatch((Byte & 0x80) ? true : false);
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
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								//replace * ONE_DOT_CYCLE with << 2
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4208: // HTIMEH
					pos = PPU.IRQHBeamPos;
					PPU.IRQHBeamPos = (PPU.IRQHBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQHBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4209: // VTIMEL
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff00) | Byte;
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420a: // VTIMEH
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC; 
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420b: // MDMAEN
					if (CPU.InDMAorHDMA)
						return;
					// XXX: Not quite right...
					if (Byte)
						CPU.Cycles += Timings.DMACPUSync;
					if (Byte & 0x01)
						S9xDoDMA_SuperFX(0);
					if (Byte & 0x02)
						S9xDoDMA_SuperFX(1);
					if (Byte & 0x04)
						S9xDoDMA_SuperFX(2);
					if (Byte & 0x08)
						S9xDoDMA_SuperFX(3);
					if (Byte & 0x10)
						S9xDoDMA_SuperFX(4);
					if (Byte & 0x20)
						S9xDoDMA_SuperFX(5);
					if (Byte & 0x40)
						S9xDoDMA_SuperFX(6);
					if (Byte & 0x80)
						S9xDoDMA_SuperFX(7);
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
						{
							CPU.FastROMSpeed = ONE_CYCLE;
						}
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
					break;
			}
		}

	Memory.FillRAM[Address] = Byte;
}

void S9xSetCPU (uint8 Byte, uint16 Address)
{
	if (Address < 0x4200)
	{
		switch (Address)
		{
			case 0x4016: // JOYSER0
				S9xSetJoypadLatch(Byte & 1);
				break;

			case 0x4017: // JOYSER1
				return;

			default:
				break;
		}
	}
	else
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

			uint64_t enable_vtimer, enable_htimer;
			switch (Address)
			{
				case 0x4200: // NMITIMEN
					enable_vtimer = (Byte & 0x20);
					enable_htimer = (Byte & 0x10);

					PPU.VTimerEnabled =  enable_vtimer;
					PPU.HTimerEnabled =  enable_htimer;

#if 1
					//begin of S9xUpdateHVTimerPosition
					//S9xUpdateHVTimerPosition();
					if (PPU.HTimerEnabled)
					{
						if (PPU.IRQHBeamPos != 0)
						{
							// IRQ_read
							//replace * ONE_DOT_CYCLE with << 2
							PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
							if (Timings.H_Max == Timings.H_Max_Master)	// 1364
							{
								if (PPU.IRQHBeamPos > 322)
									PPU.HTimerPosition += 2;
								if (PPU.IRQHBeamPos > 326)
									PPU.HTimerPosition += 2;
							}

							PPU.HTimerPosition += 14;
							// /IRQ
							PPU.HTimerPosition += 4;
							// after CPU executing
							PPU.HTimerPosition += 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;
					}
					else
						PPU.HTimerPosition = 10 + 4 + 6;

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
								CPU.NextEvent  = SNES_RENDER_START_HC;
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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
					//end of S9xUpdateHVTimerPosition
#endif

					// The case that IRQ will trigger in an instruction such as STA $4200.
					// FIXME: not true but good enough for Snes9x, I think.
					//S9xCheckMissingHTimerRange(CPU.PrevCycles, CPU.Cycles - CPU.PrevCycles);
					if ((PPU.HTimerPosition >= CPU.PrevCycles) && (PPU.HTimerPosition < (CPU.PrevCycles + (CPU.Cycles - CPU.PrevCycles))))
					{
						if (PPU.HTimerEnabled && (!PPU.VTimerEnabled || (CPU.V_Counter == PPU.VTimerPosition)))
						{
							//S9xSetIRQ(PPU_IRQ_SOURCE);
							CPU.IRQActive |= PPU_IRQ_SOURCE;
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
						else
							if (PPU.VTimerEnabled && (CPU.V_Counter == PPU.VTimerPosition))
							{
								//S9xSetIRQ(PPU_IRQ_SOURCE);
								CPU.IRQActive |= PPU_IRQ_SOURCE;
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
					}

					if (!(Byte & 0x30))
					{
						//S9xClearIRQ(PPU_IRQ_SOURCE);
						CPU.IRQActive &= ~PPU_IRQ_SOURCE;
						if (!CPU.IRQActive)
							CPU.Flags &= ~IRQ_FLAG;
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
						S9xLatchCounters(1);
					else
						S9xTryGunLatch((Byte & 0x80) ? true : false);
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
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								//replace * ONE_DOT_CYCLE with << 2
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4208: // HTIMEH
					pos = PPU.IRQHBeamPos;
					PPU.IRQHBeamPos = (PPU.IRQHBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQHBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x4209: // VTIMEL
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff00) | Byte;
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420a: // VTIMEH
					pos = PPU.IRQVBeamPos;
					PPU.IRQVBeamPos = (PPU.IRQVBeamPos & 0xff) | ((Byte & 1) << 8);
					if (PPU.IRQVBeamPos != pos)
					{
						//S9xUpdateHVTimerPosition();
#if 1
						//begin of S9xUpdateHVTimerPosition
						//S9xUpdateHVTimerPosition();
						if (PPU.HTimerEnabled)
						{
							if (PPU.IRQHBeamPos != 0)
							{
								// IRQ_read
								PPU.HTimerPosition = PPU.IRQHBeamPos << 2;
								if (Timings.H_Max == Timings.H_Max_Master)	// 1364
								{
									if (PPU.IRQHBeamPos > 322)
										PPU.HTimerPosition += 2;
									if (PPU.IRQHBeamPos > 326)
										PPU.HTimerPosition += 2;
								}

								PPU.HTimerPosition += 14;
								// /IRQ
								PPU.HTimerPosition += 4;
								// after CPU executing
								PPU.HTimerPosition += 6;
							}
							else
								PPU.HTimerPosition = 10 + 4 + 6;
						}
						else
							PPU.HTimerPosition = 10 + 4 + 6;

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
									CPU.NextEvent  = SNES_RENDER_START_HC;
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
										CPU.NextEvent  = SNES_RENDER_START_HC;
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
						//end of S9xUpdateHVTimerPosition
#endif
					}
					break;

				case 0x420b: // MDMAEN
					if (CPU.InDMAorHDMA)
						return;
					// XXX: Not quite right...
					if (Byte)
						CPU.Cycles += Timings.DMACPUSync;
					if (Byte & 0x01)
						S9xDoDMA(0);
					if (Byte & 0x02)
						S9xDoDMA(1);
					if (Byte & 0x04)
						S9xDoDMA(2);
					if (Byte & 0x08)
						S9xDoDMA(3);
					if (Byte & 0x10)
						S9xDoDMA(4);
					if (Byte & 0x20)
						S9xDoDMA(5);
					if (Byte & 0x40)
						S9xDoDMA(6);
					if (Byte & 0x80)
						S9xDoDMA(7);
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

uint8 S9xGetCPU_SuperFX (uint16 Address)
{
	if (Address < 0x4200)
	{

		switch (Address)
		{
			case 0x4016: // JOYSER0
			case 0x4017: // JOYSER1
				return (S9xReadJOYSERn_SuperFX(Address));

			default:
				return (OpenBus);
		}
	}
	else
		if ((Address & 0xff80) == 0x4300)
		{
			if (CPU.InDMAorHDMA)
				return (OpenBus);

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
		else
		{
			uint8	byte;

			switch (Address)
			{
				case 0x4210: // RDNMI
#ifdef CPU_SHUTDOWN
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
					byte = Memory.FillRAM[0x4210];
					Memory.FillRAM[0x4210] = MAX_5A22_VERSION;
					return ((byte & 0x80) | (OpenBus & 0x70) | MAX_5A22_VERSION);

				case 0x4211: // TIMEUP
					byte = (CPU.IRQActive & PPU_IRQ_SOURCE) ? 0x80 : 0;
					//S9xClearIRQ(PPU_IRQ_SOURCE);
					CPU.IRQActive &= ~PPU_IRQ_SOURCE;
					if (!CPU.IRQActive)
						CPU.Flags &= ~IRQ_FLAG;

					return (byte | (OpenBus & 0x7f));

				case 0x4212: // HVBJOY
#ifdef CPU_SHUTDOWN
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
					return (REGISTER_4212() | (OpenBus & 0x3e));

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
					return (Memory.FillRAM[Address]);

				default:
					return (OpenBus);
			}
		}
}

uint8 S9xGetCPU (uint16 Address)
{
	if (Address < 0x4200)
	{

		switch (Address)
		{
			case 0x4016: // JOYSER0
			case 0x4017: // JOYSER1
				return (S9xReadJOYSERn(Address));

			default:
				return (OpenBus);
		}
	}
	else
		if ((Address & 0xff80) == 0x4300)
		{
			if (CPU.InDMAorHDMA)
				return (OpenBus);

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
		else
		{
			uint8	byte;

			switch (Address)
			{
				case 0x4210: // RDNMI
#ifdef CPU_SHUTDOWN
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
					byte = Memory.FillRAM[0x4210];
					Memory.FillRAM[0x4210] = MAX_5A22_VERSION;
					return ((byte & 0x80) | (OpenBus & 0x70) | MAX_5A22_VERSION);

				case 0x4211: // TIMEUP
					byte = (CPU.IRQActive & PPU_IRQ_SOURCE) ? 0x80 : 0;
					//S9xClearIRQ(PPU_IRQ_SOURCE);
					CPU.IRQActive &= ~PPU_IRQ_SOURCE;
					if (!CPU.IRQActive)
						CPU.Flags &= ~IRQ_FLAG;

					return (byte | (OpenBus & 0x7f));

				case 0x4212: // HVBJOY
#ifdef CPU_SHUTDOWN
					CPU.WaitAddress = CPU.PBPCAtOpcodeStart;
#endif
					return (REGISTER_4212() | (OpenBus & 0x3e));

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
					return (Memory.FillRAM[Address]);

				default:
					if (Settings.SPC7110 && Address >= 0x4800)
						return (S9xGetSPC7110(Address));
					if (Settings.SDD1 && Address >= 0x4800 && Address <= 0x4807)
						return (Memory.FillRAM[Address]);
					return (OpenBus);
			}
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
	//IPPU.ColorsChanged = TRUE;
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
	IPPU.RenderThisFrame = TRUE;
	IPPU.RenderedScreenWidth = SNES_WIDTH;
	IPPU.RenderedScreenHeight = SNES_HEIGHT;
	//IPPU.FrameCount = 0;
	//IPPU.RenderedFramesCount = 0;
	//IPPU.DisplayedRenderedFrameCount = 0;
	IPPU.SkippedFrames = 0;
	IPPU.FrameSkip = 0;

#if 1
	//begin of S9xFixColourBrightness
	//S9xFixColourBrightness();
	IPPU.XB = mul_brightness[PPU.Brightness];

	for (int i = 0; i < 256; i += 4)
	{
		IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i])       & 0x1f];
		IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i] >>  5) & 0x1f];
		IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i] >> 10) & 0x1f];
		IPPU.ScreenColors[i] = (((int) (IPPU.Red[i]) << 10) | ((int) (IPPU.Green[i]) << 5) | (int) (IPPU.Blue[i]));

		IPPU.Red[i+1]   = IPPU.XB[(PPU.CGDATA[i+1])       & 0x1f];
		IPPU.Green[i+1] = IPPU.XB[(PPU.CGDATA[i+1] >>  5) & 0x1f];
		IPPU.Blue[i+1]  = IPPU.XB[(PPU.CGDATA[i+1] >> 10) & 0x1f];
		IPPU.ScreenColors[i+1] = (((int) (IPPU.Red[i+1]) << 10) | ((int) (IPPU.Green[i+1]) << 5) | (int) (IPPU.Blue[i+1]));

		IPPU.Red[i+2]   = IPPU.XB[(PPU.CGDATA[i+2])       & 0x1f];
		IPPU.Green[i+2] = IPPU.XB[(PPU.CGDATA[i+2] >>  5) & 0x1f];
		IPPU.Blue[i+2]  = IPPU.XB[(PPU.CGDATA[i+2] >> 10) & 0x1f];
		IPPU.ScreenColors[i+2] = (((int) (IPPU.Red[i+2]) << 10) | ((int) (IPPU.Green[i+2]) << 5) | (int) (IPPU.Blue[i+2]));

		IPPU.Red[i+3]   = IPPU.XB[(PPU.CGDATA[i+3])       & 0x1f];
		IPPU.Green[i+3] = IPPU.XB[(PPU.CGDATA[i+3] >>  5) & 0x1f];
		IPPU.Blue[i+3]  = IPPU.XB[(PPU.CGDATA[i+3] >> 10) & 0x1f];
		IPPU.ScreenColors[i+3] = (((int) (IPPU.Red[i+3]) << 10) | ((int) (IPPU.Green[i+3]) << 5) | (int) (IPPU.Blue[i+3]));
	}
	//end of S9xFixColourBrightness
#endif

	for (int c = 0; c < 0x8000; c += 0x100)
		memset(&Memory.FillRAM[c], c >> 8, 0x100);
	ZeroMemory(&Memory.FillRAM[0x2100], 0x100);
	ZeroMemory(&Memory.FillRAM[0x4200], 0x100);
	ZeroMemory(&Memory.FillRAM[0x4000], 0x100);
	// For BS Suttehakkun 2...
	ZeroMemory(&Memory.FillRAM[0x1000], 0x1000);

	Memory.FillRAM[0x4201] = Memory.FillRAM[0x4213] = 0xff;
}

bool8 S9xGraphicsInit (void)
{
	S9xInitTileRenderer();
	ZeroMemory(BlackColourMap, 256 * sizeof(uint16));

#ifdef GFX_MULTI_FORMAT
#if 0
	if (GFX.BuildPixel == NULL)
		S9xSetRenderPixelFormat(RGB565);
#endif
#endif

	GFX.DoInterlace = 0;
	GFX.InterlaceFrame = 0;
	GFX.RealPPL = GFX.Pitch >> 1;
	IPPU.OBJChanged = TRUE;
	IPPU.DirectColourMapsNeedRebuild = TRUE;
	Settings.BG_Forced = 0;

	//S9xFixColourBrightness();
	IPPU.XB = mul_brightness[PPU.Brightness];

	for (int i = 0; i < 256; i += 4)
	{
		IPPU.Red[i]   = IPPU.XB[(PPU.CGDATA[i+0])       & 0x1f];
		IPPU.Green[i] = IPPU.XB[(PPU.CGDATA[i+0] >>  5) & 0x1f];
		IPPU.Blue[i]  = IPPU.XB[(PPU.CGDATA[i+0] >> 10) & 0x1f];
		IPPU.ScreenColors[i] = (((int) (IPPU.Red[i+0]) << 10) | ((int) (IPPU.Green[i+0]) << 5) | (int) (IPPU.Blue[i+0]));

		IPPU.Red[i+1]   = IPPU.XB[(PPU.CGDATA[i+1])       & 0x1f];
		IPPU.Green[i+1] = IPPU.XB[(PPU.CGDATA[i+1] >>  5) & 0x1f];
		IPPU.Blue[i+1]  = IPPU.XB[(PPU.CGDATA[i+1] >> 10) & 0x1f];
		IPPU.ScreenColors[i+1] = (((int) (IPPU.Red[i+1]) << 10) | ((int) (IPPU.Green[i+1]) << 5) | (int) (IPPU.Blue[i+1]));

		IPPU.Red[i+2]   = IPPU.XB[(PPU.CGDATA[i+2])       & 0x1f];
		IPPU.Green[i+2] = IPPU.XB[(PPU.CGDATA[i+2] >>  5) & 0x1f];
		IPPU.Blue[i+2]  = IPPU.XB[(PPU.CGDATA[i+2] >> 10) & 0x1f];
		IPPU.ScreenColors[i+2] = (((int) (IPPU.Red[i+2]) << 10) | ((int) (IPPU.Green[i+2]) << 5) | (int) (IPPU.Blue[i+2]));

		IPPU.Red[i+3]   = IPPU.XB[(PPU.CGDATA[i+3])       & 0x1f];
		IPPU.Green[i+3] = IPPU.XB[(PPU.CGDATA[i+3] >>  5) & 0x1f];
		IPPU.Blue[i+3]  = IPPU.XB[(PPU.CGDATA[i+3] >> 10) & 0x1f];
		IPPU.ScreenColors[i+3] = (((int) (IPPU.Red[i+3]) << 10) | ((int) (IPPU.Green[i+3]) << 5) | (int) (IPPU.Blue[i+3]));
	}

	GFX.X2   = (uint16 *) malloc(sizeof(uint16) << 16);
	GFX.ZERO = (uint16 *) malloc(sizeof(uint16) << 16);

	GFX.ScreenSize = GFX.Pitch / 2 * SNES_HEIGHT_EXTENDED * 2;
	GFX.SubScreen  = (uint16 *) malloc(GFX.ScreenSize * sizeof(uint16));
	GFX.ZBuffer    = (uint8 *)  malloc(GFX.ScreenSize);
	GFX.SubZBuffer = (uint8 *)  malloc(GFX.ScreenSize);

	if (!GFX.X2 || !GFX.ZERO || !GFX.SubScreen || !GFX.ZBuffer || !GFX.SubZBuffer)
	{
		S9xGraphicsDeinit();
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

				GFX.X2[(((int) (r) << 10) | ((int) (g) << 5) | (int) (b))] = (((int) (r2) << 10) | ((int) (g2) << 5) | (int) (b2));
				GFX.X2[(((int) (r) << 10) | ((int) (g) << 5) | (int) (b)) & ~ALPHA_BITS_MASK] = (((int) (r2) << 10) | ((int) (g2) << 5) | (int) (b2));
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

				GFX.ZERO[(((int) (r) << 10) | ((int) (g) << 5) | (int) (b))] = (((int) (r2) << 10) | ((int) (g2) << 5) | (int) (b2));
				GFX.ZERO[(((int) (r) << 10) | ((int) (g) << 5) | (int) (b)) & ~ALPHA_BITS_MASK] = (((int) (r2) << 10) | ((int) (g2) << 5) | (int) (b2));
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

void S9xStartScreenRefresh_SuperFX (void)
{
#ifndef NO_FRAMESKIP
	if (IPPU.RenderThisFrame)
	{
#endif
		IPPU.MaxBrightness = PPU.Brightness;

		IPPU.DoubleWidthPixels = FALSE;
		IPPU.DoubleHeightPixels = FALSE;
		IPPU.RenderedScreenWidth = SNES_WIDTH;
		IPPU.RenderedScreenHeight = PPU.ScreenHeight;
		GFX.RealPPL = GFX.PPL = GFX.Pitch >> 1;

		//IPPU.RenderedFramesCount++;

		PPU.MosaicStart = 0;
		PPU.RecomputeClipWindows = TRUE;
		IPPU.PreviousLine = IPPU.CurrentLine = 0;

		ZeroMemory(GFX.ZBuffer, GFX.ScreenSize);
		ZeroMemory(GFX.SubZBuffer, GFX.ScreenSize);
#ifndef NO_FRAMESKIP
	}
#endif

	if (GFX.InfoStringTimeout > 0 && --GFX.InfoStringTimeout == 0)
		GFX.InfoString = NULL;

	//IPPU.TotalEmulatedFrames++;
}

void S9xStartScreenRefresh (void)
{
#ifndef NO_FRAMESKIP
	if (IPPU.RenderThisFrame)
	{
#endif
		if (GFX.DoInterlace && GFX.InterlaceFrame == 0)
			GFX.InterlaceFrame = 1;
		else
		{

			GFX.InterlaceFrame = 0;
			if (GFX.DoInterlace)
				GFX.DoInterlace--;

			IPPU.MaxBrightness = PPU.Brightness;

			IPPU.PseudoHires = Memory.FillRAM[0x2133] & 8;
			uint64_t bgmode5_6 = PPU.BGMode == 5 || PPU.BGMode == 6;
			uint64_t interlaced = IPPU.Interlace | IPPU.InterlaceOBJ;
			if (bgmode5_6)
			{
				IPPU.Interlace    = Memory.FillRAM[0x2133] & 1;
				IPPU.InterlaceOBJ = Memory.FillRAM[0x2133] & 2;
			}
#define SNES_WIDTH_TIMES_TWO 512
			uint32_t condition2 = bgmode5_6 || IPPU.PseudoHires || interlaced;
			IPPU.DoubleWidthPixels = isel_if(condition2,TRUE,FALSE); 
			IPPU.RenderedScreenWidth = isel_if(condition2,SNES_WIDTH_TIMES_TWO,SNES_WIDTH);
			GFX.RealPPL = GFX.Pitch >> 1; 
			if (condition2)
			{
				GFX.PPL = isel_if(interlaced,GFX.RealPPL << 1,GFX.RealPPL);
				IPPU.DoubleHeightPixels = isel_if(interlaced, TRUE, FALSE);
				IPPU.RenderedScreenHeight = isel_if(interlaced, PPU.ScreenHeight << 1,PPU.ScreenHeight);
				if (interlaced)
					GFX.DoInterlace++;
			}
			else
			{
				IPPU.DoubleHeightPixels = FALSE;
				IPPU.RenderedScreenHeight = PPU.ScreenHeight;
				GFX.PPL = GFX.Pitch >> 1;
			}

			//IPPU.RenderedFramesCount++;
		}

		PPU.MosaicStart = 0;
		PPU.RecomputeClipWindows = TRUE;
		IPPU.PreviousLine = IPPU.CurrentLine = 0;

		ZeroMemory(GFX.ZBuffer, GFX.ScreenSize);
		ZeroMemory(GFX.SubZBuffer, GFX.ScreenSize);
#ifndef NO_FRAMESKIP
	}
#endif


	if (GFX.InfoStringTimeout > 0 && --GFX.InfoStringTimeout == 0)
		GFX.InfoString = NULL;

	//IPPU.TotalEmulatedFrames++;
}

void S9xEndScreenRefresh_SuperFX (void)
{
#ifndef NO_FRAMESKIP
	if (IPPU.RenderThisFrame)
	{
#endif
		//if (IPPU.PreviousLine != IPPU.CurrentLine)
		S9xUpdateScreen_SuperFX();

#if 0
		if (IPPU.ColorsChanged)
		{
#endif
			uint32 saved = PPU.CGDATA[0];
			//IPPU.ColorsChanged = FALSE;
			//S9xSetPalette();
			PPU.CGDATA[0] = saved;
			//}

			S9xControlEOF_SuperFX();


			if (Settings.AutoDisplayMessages)
			{



				if (GFX.InfoString && *GFX.InfoString)
					S9xDisplayString(GFX.InfoString, 5, 1, true);
			}

			//S9xDeinitUpdate(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight);
			Graphics->Draw(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Screen);
			psglSwap();
#ifndef NO_FRAMESKIP
	}
	else
		S9xControlEOF_SuperFX();
#endif

	S9xApplyCheats();


	if (CPU.SRAMModified)
	{
		if (!CPU.AutoSaveTimer)
		{
			if (!(CPU.AutoSaveTimer = 0))
				CPU.SRAMModified = FALSE;
		}
		else
		{
			if (!--CPU.AutoSaveTimer)
			{
				if(!Settings.SRAMWriteProtect)
					Memory.SaveSRAM(S9xGetFilename(".srm", SRAM_DIR));

				CPU.SRAMModified = FALSE;
			}
		}
	}
}

void S9xEndScreenRefresh (void)
{
#ifndef NO_FRAMESKIP
	if (IPPU.RenderThisFrame)
	{
#endif
		//if (IPPU.PreviousLine != IPPU.CurrentLine)
		S9xUpdateScreen();

		if (GFX.DoInterlace && GFX.InterlaceFrame == 0)
		{
			S9xControlEOF();
			//S9xContinueUpdate(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight);
		}
		else
		{
#if 0
			if (IPPU.ColorsChanged)
			{
#endif
				uint32 saved = PPU.CGDATA[0];
				//IPPU.ColorsChanged = FALSE;
				//S9xSetPalette();
				PPU.CGDATA[0] = saved;
				//}

				S9xControlEOF();

				if (Settings.AutoDisplayMessages)
				{


					/*
					   if (Settings.DisplayMovieFrame && S9xMovieActive())
					   S9xDisplayString(GFX.FrameDisplayString, 1, 1, false);
					 */

					if (GFX.InfoString && *GFX.InfoString)
						S9xDisplayString(GFX.InfoString, 5, 1, true);
				}

				//S9xDeinitUpdate(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight);
				int32_t height = IPPU.RenderedScreenHeight;
				int32_t condition = 240 - height - Settings.OddScreenHeightJumpFix;
				int32_t mask = (condition | -condition) >> 31;
				IPPU.RenderedScreenHeight = (height & mask) | (224 & ~mask);
				Graphics->Draw(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Screen);
				psglSwap();
		}
#ifndef NO_FRAMESKIP
	}
	else
		S9xControlEOF();
#endif

	S9xApplyCheats();


	if (CPU.SRAMModified)
	{
		if (!CPU.AutoSaveTimer)
		{
			//if (!(CPU.AutoSaveTimer = Settings.AutoSaveDelay * Memory.ROMFramesPerSecond))
			if (!(CPU.AutoSaveTimer = 0))
				CPU.SRAMModified = FALSE;
		}
		else
		{
			if (!--CPU.AutoSaveTimer)
			{
				//S9xAutoSaveSRAM();
				if(!Settings.SRAMWriteProtect)
					Memory.SaveSRAM(S9xGetFilename(".srm", SRAM_DIR));

				CPU.SRAMModified = FALSE;
			}
		}
	}
}

void RenderLine (uint8 C)
{
#ifndef NO_FRAMESKIP
	if (IPPU.RenderThisFrame)
	{
#endif
		LineData[C].BG[0].VOffset = PPU.BG[0].VOffset + 1;
		LineData[C].BG[0].HOffset = PPU.BG[0].HOffset;
		LineData[C].BG[1].VOffset = PPU.BG[1].VOffset + 1;
		LineData[C].BG[1].HOffset = PPU.BG[1].HOffset;

		//SNES9x PS3 - cold code was placed in the 'do then'
		//block rather than 'else' block
		if (PPU.BGMode != 7)
		{
			LineData[C].BG[2].VOffset = PPU.BG[2].VOffset + 1;
			LineData[C].BG[2].HOffset = PPU.BG[2].HOffset;
			LineData[C].BG[3].VOffset = PPU.BG[3].VOffset + 1;
			LineData[C].BG[3].HOffset = PPU.BG[3].HOffset;
		}
		else
		{
			struct SLineMatrixData *p = &LineMatrixData[C];
			p->MatrixA = PPU.MatrixA;
			p->MatrixB = PPU.MatrixB;
			p->MatrixC = PPU.MatrixC;
			p->MatrixD = PPU.MatrixD;
			p->CentreX = PPU.CentreX;
			p->CentreY = PPU.CentreY;
			p->M7HOFS  = PPU.M7HOFS;
			p->M7VOFS  = PPU.M7VOFS;
		}

		IPPU.CurrentLine = C + 1;
#ifndef NO_FRAMESKIP
	}
	else
	{
		// if we're not rendering this frame, we still need to update this
		// XXX: Check ForceBlank? Or anything else?
		if (IPPU.OBJChanged)
			SetupOBJ();
		PPU.RangeTimeOver |= GFX.OBJLines[C].RTOFlags;
	}
#endif
}

void S9xUpdateScreen_SuperFX (void)
{
	if (IPPU.OBJChanged)
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
			//S9xComputeClipWindows();
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
					windows[2] = 256;
					windows[1] = PPU.Window1Left;
					n_regions = 2;
				}

				if (PPU.Window1Right < 255)
				{
					windows[n_regions + 1] = 256;
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

			uint8	W1, W2;

			if (PPU.Window1Left <= PPU.Window1Right)
			{
				for (i = 0; windows[i] != PPU.Window1Left; i++) ;
				for (j = i; windows[j] != PPU.Window1Right + 1; j++) ;
				W1 = region_map[i][j];
			}
			else
				W1 = 0;

			if (PPU.Window2Left <= PPU.Window2Right)
			{
				for (i = 0; windows[i] != PPU.Window2Left; i++) ;
				for (j = i; windows[j] != PPU.Window2Right + 1; j++) ;
				W2 = region_map[i][j];
			}
			else
				W2 = 0;

			// Color Window affects the drawing mode for each region.
			// Modes are: 3=Draw as normal, 2=clip color (math only), 1=no math (draw only), 0=nothing.

			uint8	CW_color = 0, CW_math = 0;
			uint8	CW = CalcWindowMask(5, W1, W2);

			switch (Memory.FillRAM[0x2130] & 0xc0)
			{
				case 0x00:	CW_color = 0;		break;
				case 0x40:	CW_color = ~CW;		break;
				case 0x80:	CW_color = CW;		break;
				case 0xc0:	CW_color = 0xff;	break;
			}

			switch (Memory.FillRAM[0x2130] & 0x30)
			{
				case 0x00:	CW_math  = 0;		break;
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

			//StoreWindowRegions(0, &IPPU.Clip[0][5], n_regions, windows, drawing_modes, FALSE, TRUE);
			//StoreWindowRegions_SubFalse_StoreMode0True_MaskFalse(&IPPU.Clip[0][5], n_regions, windows, drawing_modes);
			int	ct = 0;
			struct ClipData *Clip = &IPPU.Clip[0][5];

			for (int j = 0; j < n_regions; j++)
			{
				int	DrawMode = drawing_modes[j];

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

			//StoreWindowRegions(0, &IPPU.Clip[1][5], n_regions, windows, drawing_modes, TRUE,  TRUE);
			//StoreWindowRegions_SubTrue_StoreMode0True_MaskFalse(&IPPU.Clip[1][5], n_regions, windows, drawing_modes);
			ct = 0;
			struct ClipData *clip2 = &IPPU.Clip[1][5];

			for (int j = 0; j < n_regions; j++)
			{
				int	drawmode = drawing_modes[j];
				drawmode |= 1;

				if (ct > 0 && clip2->Right[ct - 1] == windows[j] && clip2->DrawMode[ct - 1] == drawmode)
					clip2->Right[ct - 1] = windows[j + 1]; // this region borders with and has the same drawing mode as the previous region: merge them.
				else
				{
					// add a new region to the bg
					clip2->Left[ct]     = windows[j];
					clip2->Right[ct]    = windows[j + 1];
					clip2->DrawMode[ct] = drawmode;
					ct++;
				}
			}

			clip2->Count = ct;

			// Store per-BG and OBJ clip windows

			//0
			uint8	W = CalcWindowMask(0, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 1)
				//StoreWindowRegions(W, &IPPU.Clip[0][0], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][0], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][0], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][0], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 1) 
				//StoreWindowRegions(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][0], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][0], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][0], n_regions, windows, drawing_modes);


			//1
			W = CalcWindowMask(1, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 2)
				//StoreWindowRegions(W, &IPPU.Clip[0][1], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][1], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][1], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][1], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 2)
				//StoreWindowRegions(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][1], n_regions, windows, drawing_modes);

			//2
			W = CalcWindowMask(2, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 4)
				//StoreWindowRegions(W, &IPPU.Clip[0][2], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][2], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][2], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][2], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 4)
				//StoreWindowRegions(W, &IPPU.Clip[1][2], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][2], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][2], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][2], n_regions, windows, drawing_modes);

			//3
			W = CalcWindowMask(3, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 8)
				//StoreWindowRegions(W, &IPPU.Clip[0][3], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][3], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][3], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][3], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 8)
				//StoreWindowRegions(W, &IPPU.Clip[1][3], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][3], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][3], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][3], n_regions, windows, drawing_modes);

			//4
			W = CalcWindowMask(4, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 16)
				//StoreWindowRegions(W, &IPPU.Clip[0][4], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][4], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][4], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][4], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 16)
				//StoreWindowRegions(W, &IPPU.Clip[1][4], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][4], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][4], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][4], n_regions, windows, drawing_modes);

			PPU.RecomputeClipWindows = FALSE;
		}

		if ((Memory.FillRAM[0x2130] & 0x30) != 0x30 && (Memory.FillRAM[0x2131] & 0x3f))
			GFX.FixedColour = (((int) (IPPU.XB[PPU.FixedColourRed]) << 10) | ((int) (IPPU.XB[PPU.FixedColourGreen]) << 5) | (int) (IPPU.XB[PPU.FixedColourBlue]));


		//RenderScreen(FALSE);
		uint8	BGActive;
		int		D;

		//loop invariant code
		GFX.Clip = IPPU.Clip[0];
		BGActive = Memory.FillRAM[0x212c];
		GFX.S = GFX.Screen;
#if 0
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;
#endif
		GFX.DB = GFX.ZBuffer;
		D = 32;

		if (BGActive & 0x10)
		{
			BG.TileAddress = PPU.OBJNameBase;
			BG.NameSelect = PPU.OBJNameSelect;
			BG.EnableMath = (Memory.FillRAM[0x2131] & 0x10);
			BG.StartPalette = 128;
			S9xSelectTileConverter_HiResFalse_SubFalse(4);
			S9xSelectTileRenderers_SuperFX_SubFalse();
			DrawOBJS(D + 4);
		}

		BG.NameSelect = 0;
		S9xSelectTileRenderers_SuperFX_SubFalse();

#define DO_BG_HIRES_FALSE_OFFSET_FALSE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
			DrawBackgroundMosaic(n, D + Zh, D + Zl); \
			else \
			DrawBackground(n, D + Zh, D + Zl); \
		}

#define DO_BG_HIRES_FALSE_OFFSET_TRUE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
			BG.OffsetSizeH = (PPU.BG[2].BGSize) ? 16 : 8; \
			BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
			DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
			else \
			DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

		switch (PPU.BGMode)
		{
			case 0:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 2, 15, 11, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1, 32, 2, 14, 10, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(2, 64, 2, 7,  3, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(3, 96, 2, 6,  2, 0);
				break;

			case 1:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 4, 15, 11, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 14, 10, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(2,  0, 2, isel_if(PPU.BG3Priority,17,7), 3, 0);
				break;

			case 2:
				DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 4, 15,  7, 8);
				DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 4, 11,  3, 8);
				break;

			case 3:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 8, 15,  7, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 11,  3, 0);
				break;

			case 4:
				DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 8, 15,  7, 0);
				DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 2, 11,  3, 0);
				break;

#if 0
			case 7:
				if (BGActive & 0x01)
				{
					BG.EnableMath = (Memory.FillRAM[0x2131] & 1);

					//DrawBackgroundMode7(0, GFX.DrawMode7BG1Math, GFX.DrawMode7BG1Nomath, D);
					for (int clip = 0; clip < GFX.Clip[0].Count; clip++)
					{
						GFX.ClipColors = !(GFX.Clip[0].DrawMode[clip] & 1);
						if (BG.EnableMath && (GFX.Clip[0].DrawMode[clip] & 2))
							GFX.DrawMode7BG1Math(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
						else
							GFX.DrawMode7BG1Nomath(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
					}
				}

				if ((Memory.FillRAM[0x2133] & 0x40) && (BGActive & 0x02))
				{
					BG.EnableMath = (Memory.FillRAM[0x2131] & 2);

					//DrawBackgroundMode7(1, GFX.DrawMode7BG2Math, GFX.DrawMode7BG2Nomath, D);
					for (int clip = 0; clip < GFX.Clip[1].Count; clip++)
					{
						GFX.ClipColors = !(GFX.Clip[1].DrawMode[clip] & 1);

						if (BG.EnableMath && (GFX.Clip[1].DrawMode[clip] & 2))
							GFX.DrawMode7BG2Math(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
						else
							GFX.DrawMode7BG2Nomath(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
					}
				}

				break;
#endif
		}

		//#undef DO_BG
#undef DO_BG_HIRES_TRUE_OFFSET_TRUE
#undef DO_BG_HIRES_TRUE_OFFSET_FALSE
#undef DO_BG_HIRES_FALSE_OFFSET_TRUE
#undef DO_BG_HIRES_FALSE_OFFSET_FALSE

		BG.EnableMath = (Memory.FillRAM[0x2131] & 0x20);

		//DrawBackdrop();
		uint32	Offset = GFX.StartY * GFX.PPL;

		for (int clip = 0; clip < GFX.Clip[5].Count; clip++)
		{
			GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1);

			if (BG.EnableMath && (GFX.Clip[5].DrawMode[clip] & 2))
				GFX.DrawBackdropMath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
			else
				GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
		}
	}
	else
	{
		//const uint16	black = (((int) (0) << 10) | ((int) (0) << 5) | (int) (0));
#define black 0

		GFX.S = GFX.Screen + GFX.StartY * GFX.PPL;
#if 0
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;
#endif

		for (uint32 l = GFX.StartY; l <= GFX.EndY; l++, GFX.S += GFX.PPL)
			for (int x = 0; x < IPPU.RenderedScreenWidth; x += 4)
			{
				GFX.S[x+0] = black;
				GFX.S[x+1] = black;
				GFX.S[x+2] = black;
				GFX.S[x+3] = black;
			}
	}

	IPPU.PreviousLine = IPPU.CurrentLine;
}

void S9xUpdateScreen (void)
{
	if (IPPU.OBJChanged)
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
			//S9xComputeClipWindows();
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
					windows[2] = 256;
					windows[1] = PPU.Window1Left;
					n_regions = 2;
				}

				if (PPU.Window1Right < 255)
				{
					windows[n_regions + 1] = 256;
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

			uint8	W1, W2;

			if (PPU.Window1Left <= PPU.Window1Right)
			{
				for (i = 0; windows[i] != PPU.Window1Left; i++) ;
				for (j = i; windows[j] != PPU.Window1Right + 1; j++) ;
				W1 = region_map[i][j];
			}
			else
				W1 = 0;

			if (PPU.Window2Left <= PPU.Window2Right)
			{
				for (i = 0; windows[i] != PPU.Window2Left; i++) ;
				for (j = i; windows[j] != PPU.Window2Right + 1; j++) ;
				W2 = region_map[i][j];
			}
			else
				W2 = 0;

			// Color Window affects the drawing mode for each region.
			// Modes are: 3=Draw as normal, 2=clip color (math only), 1=no math (draw only), 0=nothing.

			uint8	CW_color = 0, CW_math = 0;
			uint8	CW = CalcWindowMask(5, W1, W2);

			switch (Memory.FillRAM[0x2130] & 0xc0)
			{
				case 0x00:	CW_color = 0;		break;
				case 0x40:	CW_color = ~CW;		break;
				case 0x80:	CW_color = CW;		break;
				case 0xc0:	CW_color = 0xff;	break;
			}

			switch (Memory.FillRAM[0x2130] & 0x30)
			{
				case 0x00:	CW_math  = 0;		break;
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

			//StoreWindowRegions(0, &IPPU.Clip[0][5], n_regions, windows, drawing_modes, FALSE, TRUE);
			//StoreWindowRegions_SubFalse_StoreMode0True_MaskFalse(&IPPU.Clip[0][5], n_regions, windows, drawing_modes);
			int	ct = 0;
			struct ClipData *Clip = &IPPU.Clip[0][5];

			for (int j = 0; j < n_regions; j++)
			{
				int	DrawMode = drawing_modes[j];

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

			//StoreWindowRegions(0, &IPPU.Clip[1][5], n_regions, windows, drawing_modes, TRUE,  TRUE);
			//StoreWindowRegions_SubTrue_StoreMode0True_MaskFalse(&IPPU.Clip[1][5], n_regions, windows, drawing_modes);
			ct = 0;
			struct ClipData *clip2 = &IPPU.Clip[1][5];

			for (int j = 0; j < n_regions; j++)
			{
				int	drawmode = drawing_modes[j];
				drawmode |= 1;

				if (ct > 0 && clip2->Right[ct - 1] == windows[j] && clip2->DrawMode[ct - 1] == drawmode)
					clip2->Right[ct - 1] = windows[j + 1]; // this region borders with and has the same drawing mode as the previous region: merge them.
				else
				{
					// add a new region to the bg
					clip2->Left[ct]     = windows[j];
					clip2->Right[ct]    = windows[j + 1];
					clip2->DrawMode[ct] = drawmode;
					ct++;
				}
			}

			clip2->Count = ct;

			// Store per-BG and OBJ clip windows

			//0
			uint8	W = CalcWindowMask(0, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 1)
				//StoreWindowRegions(W, &IPPU.Clip[0][0], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][0], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][0], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][0], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 1) 
				//StoreWindowRegions(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][0], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][0], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][0], n_regions, windows, drawing_modes);


			//1
			W = CalcWindowMask(1, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 2)
				//StoreWindowRegions(W, &IPPU.Clip[0][1], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][1], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][1], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][1], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 2)
				//StoreWindowRegions(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][1], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][1], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][1], n_regions, windows, drawing_modes);

			//2
			W = CalcWindowMask(2, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 4)
				//StoreWindowRegions(W, &IPPU.Clip[0][2], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][2], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][2], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][2], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 4)
				//StoreWindowRegions(W, &IPPU.Clip[1][2], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][2], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][2], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][2], n_regions, windows, drawing_modes);

			//3
			W = CalcWindowMask(3, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 8)
				//StoreWindowRegions(W, &IPPU.Clip[0][3], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][3], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][3], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][3], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 8)
				//StoreWindowRegions(W, &IPPU.Clip[1][3], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][3], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][3], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][3], n_regions, windows, drawing_modes);

			//4
			W = CalcWindowMask(4, W1, W2);
			if (Memory.FillRAM[0 + 0x212e] & 16)
				//StoreWindowRegions(W, &IPPU.Clip[0][4], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False(W, &IPPU.Clip[0][4], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[0][4], n_regions, windows, drawing_modes, 0);
				StoreWindowRegions_SubFalse_StoreMode0False_MaskFalse(&IPPU.Clip[0][4], n_regions, windows, drawing_modes);

			if (Memory.FillRAM[1 + 0x212e] & 16)
				//StoreWindowRegions(W, &IPPU.Clip[1][4], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False(W, &IPPU.Clip[1][4], n_regions, windows, drawing_modes);
			else
				//StoreWindowRegions(0, &IPPU.Clip[1][4], n_regions, windows, drawing_modes, 1);
				StoreWindowRegions_SubTrue_StoreMode0False_MaskFalse(&IPPU.Clip[1][4], n_regions, windows, drawing_modes);

			PPU.RecomputeClipWindows = FALSE;
		}

		if (!IPPU.DoubleWidthPixels && (PPU.BGMode == 5 || PPU.BGMode == 6 || IPPU.PseudoHires || IPPU.Interlace || IPPU.InterlaceOBJ))
		{
			{
				// Have to back out of the regular speed hack
				for (register uint32 y = 0; y < GFX.StartY; y++)
				{
					register uint16	*p = GFX.Screen + y * GFX.PPL + 255;
					register uint16	*q = GFX.Screen + y * GFX.PPL + 510;

					for (register int x = 255; x >= 0; x--, p--, q -= 2)
						*q = *(q + 1) = *p;
				}
			}

			IPPU.DoubleWidthPixels = TRUE;
			IPPU.RenderedScreenWidth = 512;
		}

		if (!IPPU.DoubleHeightPixels && (IPPU.Interlace || IPPU.InterlaceOBJ))
		{
			IPPU.DoubleHeightPixels = TRUE;
			IPPU.RenderedScreenHeight = PPU.ScreenHeight << 1;
			GFX.PPL = GFX.RealPPL << 1;
			GFX.DoInterlace = 2;

			for (register int32 y = (int32) GFX.StartY - 1; y >= 0; y--)
				memmove(GFX.Screen + y * GFX.PPL, GFX.Screen + y * GFX.RealPPL, IPPU.RenderedScreenWidth * sizeof(uint16));
		}

		if ((Memory.FillRAM[0x2130] & 0x30) != 0x30 && (Memory.FillRAM[0x2131] & 0x3f))
			GFX.FixedColour = (((int) (IPPU.XB[PPU.FixedColourRed]) << 10) | ((int) (IPPU.XB[PPU.FixedColourGreen]) << 5) | (int) (IPPU.XB[PPU.FixedColourBlue]));

		if (PPU.BGMode == 5 || PPU.BGMode == 6 || IPPU.PseudoHires ||
				((Memory.FillRAM[0x2130] & 0x30) != 0x30 && (Memory.FillRAM[0x2130] & 2) && (Memory.FillRAM[0x2131] & 0x3f) && (Memory.FillRAM[0x212d] & 0x1f)))
		{
			// If hires (Mode 5/6 or pseudo-hires) or math is to be done
			// involving the subscreen, then we need to render the subscreen...
			//RenderScreen(TRUE);
			uint8	BGActive;
			int		D;

			//loop invariant code
			GFX.Clip = IPPU.Clip[1];
			BGActive = Memory.FillRAM[0x212c+1];
			GFX.S = GFX.SubScreen;
			GFX.DB = GFX.SubZBuffer;
			D = (Memory.FillRAM[0x2130] & 2) << 4; // 'do math' depth flag

			if (BGActive & 0x10)
			{
				BG.TileAddress = PPU.OBJNameBase;
				BG.NameSelect = PPU.OBJNameSelect;
				BG.EnableMath = 0;
				BG.StartPalette = 128;
				S9xSelectTileConverter_HiResFalse_SubFalse(4);
				S9xSelectTileRenderers(PPU.BGMode, 1, TRUE);
				DrawOBJS(D + 4);
			}

			BG.NameSelect = 0;
			S9xSelectTileRenderers(PPU.BGMode, 1, FALSE);

#define DO_BG_HIRES_FALSE_OFFSET_FALSE(n, pal, depth, Zh, Zl, voffoff) \
			if (BGActive & (1 << n)) \
			{ \
				BG.StartPalette = pal; \
				BG.EnableMath = 0; \
				BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
				BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
				S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
				if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
				DrawBackgroundMosaic(n, D + Zh, D + Zl); \
				else \
				DrawBackground(n, D + Zh, D + Zl); \
			}

#define DO_BG_HIRES_FALSE_OFFSET_TRUE(n, pal, depth, Zh, Zl, voffoff) \
			if (BGActive & (1 << n)) \
			{ \
				BG.StartPalette = pal; \
				BG.EnableMath = 0; \
				BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
				BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
				S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
				BG.OffsetSizeH = (PPU.BG[2].BGSize) ? 16 : 8; \
				BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
				\
				if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
				DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
				else \
				DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
			}

#define DO_BG_HIRES_TRUE_OFFSET_FALSE(n, pal, depth, Zh, Zl, voffoff) \
			if (BGActive & (1 << n)) \
			{ \
				BG.StartPalette = pal; \
				BG.EnableMath = 0; \
				BG.TileSizeH = 8; \
				BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
				S9xSelectTileConverter_HiResTrue_SubTrue(depth); \
				if (PPU.BGMosaic[n]) \
				DrawBackgroundMosaic(n, D + Zh, D + Zl); \
				else \
				DrawBackground(n, D + Zh, D + Zl); \
			}

#define DO_BG_HIRES_TRUE_OFFSET_TRUE(n, pal, depth, Zh, Zl, voffoff) \
			if (BGActive & (1 << n)) \
			{ \
				BG.StartPalette = pal; \
				BG.EnableMath = 0; \
				BG.TileSizeH = 8; \
				BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
				S9xSelectTileConverter_HiResTrue_SubTrue(depth); \
				BG.OffsetSizeH = 8; \
				BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
				\
				if (PPU.BGMosaic[n]) \
				DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
				else \
				DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
			}

			switch (PPU.BGMode)
			{
				case 0:
					DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 2, 15, 11, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(1, 32, 2, 14, 10, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(2, 64, 2, 7,  3, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(3, 96, 2, 6,  2, 0);
					break;

				case 1:
					DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 4, 15, 11, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 14, 10, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(2,  0, 2, (PPU.BG3Priority ? 17 : 7), 3, 0);
					break;

				case 2:
					DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 4, 15,  7, 8);
					DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 4, 11,  3, 8);
					break;

				case 3:
					DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 8, 15,  7, 0);
					DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 11,  3, 0);
					break;

				case 4:
					DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 8, 15,  7, 0);
					DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 2, 11,  3, 0);
					break;

				case 5:
					DO_BG_HIRES_TRUE_OFFSET_FALSE(0,  0, 4, 15,  7, 0);
					DO_BG_HIRES_TRUE_OFFSET_FALSE(1,  0, 2, 11,  3, 0);
					break;

				case 6:
					DO_BG_HIRES_TRUE_OFFSET_TRUE(0,  0, 4, 15,  7, 8);
					break;

				case 7:
					if (BGActive & 0x01)
					{
						BG.EnableMath = 0;
						//DrawBackgroundMode7(0, GFX.DrawMode7BG1Math, GFX.DrawMode7BG1Nomath, D);
						for (int clip = 0; clip < GFX.Clip[0].Count; clip++)
						{
							GFX.ClipColors = !(GFX.Clip[0].DrawMode[clip] & 1);

							if (BG.EnableMath && (GFX.Clip[0].DrawMode[clip] & 2))
								GFX.DrawMode7BG1Math(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
							else
								GFX.DrawMode7BG1Nomath(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
						}
					}

					if ((Memory.FillRAM[0x2133] & 0x40) && (BGActive & 0x02))
					{
						BG.EnableMath = 0;
						//DrawBackgroundMode7(1, GFX.DrawMode7BG2Math, GFX.DrawMode7BG2Nomath, D);
						for (int clip = 0; clip < GFX.Clip[1].Count; clip++)
						{
							GFX.ClipColors = !(GFX.Clip[1].DrawMode[clip] & 1);

							if (BG.EnableMath && (GFX.Clip[1].DrawMode[clip] & 2))
								GFX.DrawMode7BG2Math(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
							else
								GFX.DrawMode7BG2Nomath(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
						}
					}

					break;
			}

#undef DO_BG_HIRES_TRUE_OFFSET_TRUE
#undef DO_BG_HIRES_TRUE_OFFSET_FALSE
#undef DO_BG_HIRES_FALSE_OFFSET_FALSE
#undef DO_BG_HIRES_FALSE_OFFSET_TRUE

			BG.EnableMath = 0;

			//DrawBackdrop();
			uint32	Offset = GFX.StartY * GFX.PPL;

			for (int clip = 0; clip < GFX.Clip[5].Count; clip++)
			{
				GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1);

				if (BG.EnableMath && (GFX.Clip[5].DrawMode[clip] & 2))
					GFX.DrawBackdropMath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
				else
					GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
			}
		}

		//RenderScreen(FALSE);
		uint8	BGActive;
		int		D;

		//loop invariant code
		GFX.Clip = IPPU.Clip[0];
		BGActive = Memory.FillRAM[0x212c];
		GFX.S = GFX.Screen;
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;
		GFX.DB = GFX.ZBuffer;
		D = 32;

		if (BGActive & 0x10)
		{
			BG.TileAddress = PPU.OBJNameBase;
			BG.NameSelect = PPU.OBJNameSelect;
			BG.EnableMath = (Memory.FillRAM[0x2131] & 0x10);
			BG.StartPalette = 128;
			S9xSelectTileConverter_HiResFalse_SubFalse(4);
			S9xSelectTileRenderers(PPU.BGMode, 0, TRUE);
			DrawOBJS(D + 4);
		}

		BG.NameSelect = 0;
		S9xSelectTileRenderers(PPU.BGMode, 0, FALSE);

#define DO_BG_HIRES_FALSE_OFFSET_FALSE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
			DrawBackgroundMosaic(n, D + Zh, D + Zl); \
			else \
			DrawBackground(n, D + Zh, D + Zl); \
		}

#define DO_BG_HIRES_FALSE_OFFSET_TRUE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = (PPU.BG[n].BGSize) ? 16 : 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResFalse_SubFalse(depth); \
			BG.OffsetSizeH = (PPU.BG[2].BGSize) ? 16 : 8; \
			BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			if (PPU.BGMosaic[n] && (PPU.Mosaic > 1)) \
			DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
			else \
			DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

#define DO_BG_HIRES_TRUE_OFFSET_FALSE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResTrue_SubFalse(depth, PPU.BGMosaic[n]); \
			\
			if (PPU.BGMosaic[n]) \
			DrawBackgroundMosaic(n, D + Zh, D + Zl); \
			else \
			DrawBackground(n, D + Zh, D + Zl); \
		}

#define DO_BG_HIRES_TRUE_OFFSET_TRUE(n, pal, depth, Zh, Zl, voffoff) \
		if (BGActive & (1 << n)) \
		{ \
			BG.StartPalette = pal; \
			BG.EnableMath = (Memory.FillRAM[0x2131] & (1 << n)); \
			BG.TileSizeH = 8; \
			BG.TileSizeV = (PPU.BG[n].BGSize) ? 16 : 8; \
			S9xSelectTileConverter_HiResTrue_SubFalse(depth, PPU.BGMosaic[n]); \
			BG.OffsetSizeH = 8; \
			BG.OffsetSizeV = (PPU.BG[2].BGSize) ? 16 : 8; \
			\
			if (PPU.BGMosaic[n]) \
			DrawBackgroundOffsetMosaic(n, D + Zh, D + Zl, voffoff); \
			else \
			DrawBackgroundOffset(n, D + Zh, D + Zl, voffoff); \
		}

		switch (PPU.BGMode)
		{
			case 0:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 2, 15, 11, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1, 32, 2, 14, 10, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(2, 64, 2, 7,  3, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(3, 96, 2, 6,  2, 0);
				break;

			case 1:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 4, 15, 11, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 14, 10, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(2,  0, 2, (PPU.BG3Priority ? 17 : 7), 3, 0);
				break;

			case 2:
				DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 4, 15,  7, 8);
				DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 4, 11,  3, 8);
				break;

			case 3:
				DO_BG_HIRES_FALSE_OFFSET_FALSE(0,  0, 8, 15,  7, 0);
				DO_BG_HIRES_FALSE_OFFSET_FALSE(1,  0, 4, 11,  3, 0);
				break;

			case 4:
				DO_BG_HIRES_FALSE_OFFSET_TRUE(0,  0, 8, 15,  7, 0);
				DO_BG_HIRES_FALSE_OFFSET_TRUE(1,  0, 2, 11,  3, 0);
				break;

			case 5:
				DO_BG_HIRES_TRUE_OFFSET_FALSE(0,  0, 4, 15,  7, 0);
				DO_BG_HIRES_TRUE_OFFSET_FALSE(1,  0, 2, 11,  3, 0);
				break;

			case 6:
				DO_BG_HIRES_TRUE_OFFSET_TRUE(0,  0, 4, 15,  7, 8);
				break;

			case 7:
				if (BGActive & 0x01)
				{
					BG.EnableMath = (Memory.FillRAM[0x2131] & 1);
					//DrawBackgroundMode7(0, GFX.DrawMode7BG1Math, GFX.DrawMode7BG1Nomath, D);
					for (int clip = 0; clip < GFX.Clip[0].Count; clip++)
					{
						GFX.ClipColors = !(GFX.Clip[0].DrawMode[clip] & 1);

						if (BG.EnableMath && (GFX.Clip[0].DrawMode[clip] & 2))
							GFX.DrawMode7BG1Math(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
						else
							GFX.DrawMode7BG1Nomath(GFX.Clip[0].Left[clip], GFX.Clip[0].Right[clip], D);
					}
				}

				if ((Memory.FillRAM[0x2133] & 0x40) && (BGActive & 0x02))
				{
					BG.EnableMath = (Memory.FillRAM[0x2131] & 2);
					//DrawBackgroundMode7(1, GFX.DrawMode7BG2Math, GFX.DrawMode7BG2Nomath, D);
					for (int clip = 0; clip < GFX.Clip[1].Count; clip++)
					{
						GFX.ClipColors = !(GFX.Clip[1].DrawMode[clip] & 1);

						if (BG.EnableMath && (GFX.Clip[1].DrawMode[clip] & 2))
							GFX.DrawMode7BG2Math(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
						else
							GFX.DrawMode7BG2Nomath(GFX.Clip[1].Left[clip], GFX.Clip[1].Right[clip], D);
					}
				}

				break;
		}

		//#undef DO_BG
#undef DO_BG_HIRES_TRUE_OFFSET_TRUE
#undef DO_BG_HIRES_TRUE_OFFSET_FALSE
#undef DO_BG_HIRES_FALSE_OFFSET_TRUE
#undef DO_BG_HIRES_FALSE_OFFSET_FALSE

		BG.EnableMath = (Memory.FillRAM[0x2131] & 0x20);

		//DrawBackdrop();
		uint32	Offset = GFX.StartY * GFX.PPL;

		for (int clip = 0; clip < GFX.Clip[5].Count; clip++)
		{
			GFX.ClipColors = !(GFX.Clip[5].DrawMode[clip] & 1);

			if (BG.EnableMath && (GFX.Clip[5].DrawMode[clip] & 2))
				GFX.DrawBackdropMath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
			else
				GFX.DrawBackdropNomath(Offset, GFX.Clip[5].Left[clip], GFX.Clip[5].Right[clip]);
		}
	}
	else
	{
		//const uint16	black = (((int) (0) << 10) | ((int) (0) << 5) | (int) (0));
#undef black
#define black 0

		GFX.S = GFX.Screen + GFX.StartY * GFX.PPL;
		if (GFX.DoInterlace && GFX.InterlaceFrame)
			GFX.S += GFX.RealPPL;

		for (uint32 l = GFX.StartY; l <= GFX.EndY; l++, GFX.S += GFX.PPL)
			for (int x = 0; x < IPPU.RenderedScreenWidth; x += 4)
			{
				GFX.S[x+0] = black;
				GFX.S[x+1] = black;
				GFX.S[x+2] = black;
				GFX.S[x+3] = black;
			}
	}

	IPPU.PreviousLine = IPPU.CurrentLine;
}




/*
void S9xReRefresh (void)
{
	// Be careful when calling this function from the thread other than the emulation one...
	// Here it's assumed no drawing occurs from the emulation thread when Settings.Paused is TRUE.
	if (Settings.Paused)
		S9xDeinitUpdate(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight);
}
*/

void S9xSetInfoString (const char *string)
{
	if (Settings.InitialInfoStringTimeout > 0)
	{
		GFX.InfoString = string;
		GFX.InfoStringTimeout = Settings.InitialInfoStringTimeout;
		//S9xReRefresh();
	}
}

void S9xDisplayChar (uint16 *s, uint8 c)
{
#undef black
#define black 0
	//const uint16	black = (((int) (0) << 10) | ((int) (0) << 5) | (int) (0));

	int	line   = ((c - 32) >> 4) * font_height;
	int	offset = ((c - 32) & 15) * font_width;

	for (int h = 0; h < font_height; h++, line++, s += GFX.RealPPL - font_width)
	{
		for (int w = 0; w < font_width; w++, s++)
		{
			char	p = font[line][offset + w];

			if (p == '#')
				*s = Settings.DisplayColor;
			else
				if (p == '.')
					*s = black;
		}
	}
}

static uint16 get_crosshair_color (uint8 color)
{
	switch (color & 15)
	{
		case  0: return (((int) (0) << 10)  | ((int) (0) << 5) | (int) (0)); // transparent, shouldn't be used
		case  1: return (((int) (0) << 10)  | ((int) (0) << 5) | (int) (0)); // Black
		case  2: return (((int) (8) << 10)  | ((int) (8) << 5) | (int) (8)); // 25Grey
		case  3: return (((int) (16) << 10)  | ((int) (16) << 5) | (int) (16)); // 50Grey
		case  4: return (((int) (23) << 10)  | ((int) (23) << 5) | (int) (23)); // 75Grey
		case  5: return (((int) (31) << 10)  | ((int) (31) << 5) | (int) (31)); // White
		case  6: return (((int) (31) << 10)  | ((int) (0) << 5) | (int) (0)); // Red
		case  7: return (((int) (31) << 10)  | ((int) (16) << 5) | (int) (0)); // Orange
		case  8: return (((int) (31) << 10)  | ((int) (31) << 5) | (int) (0)); // Yellow
		case  9: return (((int) (0) << 10)  | ((int) (31) << 5) | (int) (0)); // Green
		case 10: return (((int) (0) << 10)  | ((int) (31) << 5) | (int) (31)); // Cyan
		case 11: return (((int) (0) << 10)  | ((int) (23) << 5) | (int) (31)); // Sky
		case 12: return (((int) (0) << 10)  | ((int) (0) << 5)  | (int) (31)); // Blue
		case 13: return (((int) (23) << 10) | ((int) (0) << 5)  | (int) (31)); // Violet
		case 14: return (((int) (31) << 10) | ((int) (0) << 5)  | (int) (31)); // Magenta
		case 15: return (((int) (31) << 10) | ((int) (0) << 5)  | (int) (16)); // Purple
	}

	return (0);
}

void S9xDrawCrosshair (const char *crosshair, uint8 fgcolor, uint8 bgcolor, int16 x, int16 y)
{
	if (!crosshair)
		return;

	int16	r, rx = 1, c, cx = 1, W = SNES_WIDTH, H = PPU.ScreenHeight;
	uint16	fg, bg;

	x -= 7;
	y -= 7;

	if (IPPU.DoubleWidthPixels)  { cx = 2; x *= 2; W *= 2; }
	if (IPPU.DoubleHeightPixels) { rx = 2; y *= 2; H *= 2; }

	fg = get_crosshair_color(fgcolor);
	bg = get_crosshair_color(bgcolor);

	// XXX: FIXME: why does it crash without this on Linux port? There are no out-of-bound writes without it...
#if (defined(__unix) || defined(__linux) || defined(__sun) || defined(__DJGPP))
	if (x >= 0 && y >= 0)
#endif
	{
		uint16	*s = GFX.Screen + y * GFX.RealPPL + x;

		for (r = 0; r < (mul<15>(rx)); r++, s += GFX.RealPPL - (mul<15>(cx)))
		{
			if (y + r < 0)
			{
				s += mul<15>(cx);
				continue;
			}

			if (y + r >= H)
				break;

			for (c = 0; c < (mul<15>(cx)); c++, s++)
			{
				if (x + c < 0 || s < GFX.Screen)
					continue;

				if (x + c >= W)
				{
					s += (mul<15>(cx)) - c;
					break;
				}

				uint8	p = crosshair[mul<15>(r / rx) + (c / cx)];

				if (p == '#' && fgcolor)
					*s = (fgcolor & 0x10) ? COLOR_ADD1_2(fg, *s) : fg;
				else
					if (p == '.' && bgcolor)
						*s = (bgcolor & 0x10) ? COLOR_ADD1_2(*s, bg) : bg;
			}
		}
	}
}

#ifdef GFX_MULTI_FORMAT

//static uint32 BuildPixelRGB565  (uint32, uint32, uint32);
//static uint32 BuildPixelRGB555  (uint32, uint32, uint32);
//static uint32 BuildPixelBGR565  (uint32, uint32, uint32);
//static uint32 BuildPixelBGR555  (uint32, uint32, uint32);
//static uint32 BuildPixelGBR565  (uint32, uint32, uint32);
//static uint32 BuildPixelGBR555  (uint32, uint32, uint32);
//static uint32 BuildPixelRGB5551 (uint32, uint32, uint32);

//static uint32 BuildPixel2RGB565  (uint32, uint32, uint32);
//static uint32 BuildPixel2RGB555  (uint32, uint32, uint32);
//static uint32 BuildPixel2BGR565  (uint32, uint32, uint32);
//static uint32 BuildPixel2BGR555  (uint32, uint32, uint32);
//static uint32 BuildPixel2GBR565  (uint32, uint32, uint32);
//static uint32 BuildPixel2GBR555  (uint32, uint32, uint32);
//static uint32 BuildPixel2RGB5551 (uint32, uint32, uint32);

//static void DecomposePixelRGB565  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelRGB555  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelBGR565  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelBGR555  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelGBR565  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelGBR555  (uint32, uint32 &, uint32 &, uint32 &);
//static void DecomposePixelRGB5551 (uint32, uint32 &, uint32 &, uint32 &);

#if 0
#define _BUILD_PIXEL(F) \
static uint32 BuildPixel##F (uint32 R, uint32 G, uint32 B) \
{ \
	return (BUILD_PIXEL_##F(R, G, B)); \
} \
\
static uint32 BuildPixel2##F (uint32 R, uint32 G, uint32 B) \
{ \
	return (BUILD_PIXEL2_##F(R, G, B)); \
} \
\
static void DecomposePixel##F (uint32 pixel, uint32 &R, uint32 &G, uint32 &B) \
{ \
	DECOMPOSE_PIXEL_##F(pixel, R, G, B); \
}
#endif

//_BUILD_PIXEL(RGB565)

//the only one we use, so let's hardcode this
//_BUILD_PIXEL(RGB555)

#if 0
static uint32 BuildPixelRGB555 (uint32 R, uint32 G, uint32 B)
{
	//return (BUILD_PIXEL_RGB555(R, G, B));
   return (((int) (R) << 10) | ((int) (G) << 5) | (int) (B));
}
#endif

#if 0
static uint32 BuildPixel2RGB555(uint32 R, uint32 G, uint32 B)
{
	//return (BUILD_PIXEL2_RGB555(R, G, B));
   return (((int) (R) << 10) | ((int) (G) << 5) | (int) (B));
}
#endif

#if 0
static void DecomposePixelRGB555 (uint32 pixel, uint32 &R, uint32 &G, uint32 &B)
{ 
	//DECOMPOSE_PIXEL_RGB555(pixel, R, G, B);
   R = (pixel) >> 10;
   G = ((pixel) >> 5) & 0x1f;
   B = (pixel) & 0x1f;
}
#endif



//_BUILD_PIXEL(BGR565)
//_BUILD_PIXEL(BGR555)
//_BUILD_PIXEL(GBR565)
//_BUILD_PIXEL(GBR555)
//_BUILD_PIXEL(RGB5551)

//taken out of BUILD_SETUP(F)
//GFX.DecomposePixel         = DecomposePixel##F;
//GFX.BuildPixel             = BuildPixel##F;
//GFX.BuildPixel2            = BuildPixel2##F;

#define _BUILD_SETUP(F) \
RED_LOW_BIT_MASK           = RED_LOW_BIT_MASK_##F; \
GREEN_LOW_BIT_MASK         = GREEN_LOW_BIT_MASK_##F; \
BLUE_LOW_BIT_MASK          = BLUE_LOW_BIT_MASK_##F; \
RED_HI_BIT_MASK            = RED_HI_BIT_MASK_##F; \
GREEN_HI_BIT_MASK          = GREEN_HI_BIT_MASK_##F; \
BLUE_HI_BIT_MASK           = BLUE_HI_BIT_MASK_##F; \
MAX_RED                    = MAX_RED_##F; \
MAX_GREEN                  = MAX_GREEN_##F; \
MAX_BLUE                   = MAX_BLUE_##F; \
SPARE_RGB_BIT_MASK         = SPARE_RGB_BIT_MASK_##F; \
GREEN_HI_BIT               = ((MAX_GREEN_##F + 1) >> 1); \
RGB_LOW_BITS_MASK          = (RED_LOW_BIT_MASK_##F | GREEN_LOW_BIT_MASK_##F | BLUE_LOW_BIT_MASK_##F); \
RGB_HI_BITS_MASK           = (RED_HI_BIT_MASK_##F  | GREEN_HI_BIT_MASK_##F  | BLUE_HI_BIT_MASK_##F); \
RGB_HI_BITS_MASKx2         = (RED_HI_BIT_MASK_##F  | GREEN_HI_BIT_MASK_##F  | BLUE_HI_BIT_MASK_##F) << 1; \
RGB_REMOVE_LOW_BITS_MASK   = ~RGB_LOW_BITS_MASK; \
FIRST_COLOR_MASK           = FIRST_COLOR_MASK_##F; \
SECOND_COLOR_MASK          = SECOND_COLOR_MASK_##F; \
THIRD_COLOR_MASK           = THIRD_COLOR_MASK_##F; \
ALPHA_BITS_MASK            = ALPHA_BITS_MASK_##F; \
FIRST_THIRD_COLOR_MASK     = FIRST_COLOR_MASK | THIRD_COLOR_MASK; \
TWO_LOW_BITS_MASK          = RGB_LOW_BITS_MASK | (RGB_LOW_BITS_MASK << 1); \
HIGH_BITS_SHIFTED_TWO_MASK = ((FIRST_COLOR_MASK | SECOND_COLOR_MASK | THIRD_COLOR_MASK) & ~TWO_LOW_BITS_MASK) >> 2;

bool8 S9xSetRenderPixelFormat (int format)
{
	//GFX.PixelFormat = format;

#ifndef __CELLOS_LV2__
	switch (format)
	{
		case RGB565:
			_BUILD_SETUP(RGB565)
				return (TRUE);

		case RGB555:
			_BUILD_SETUP(RGB555)
				return (TRUE);

		case BGR565:
			_BUILD_SETUP(BGR565)
				return (TRUE);

		case BGR555:
			_BUILD_SETUP(BGR555)
				return (TRUE);

		case GBR565:
			_BUILD_SETUP(GBR565)
				return (TRUE);

		case GBR555:
			_BUILD_SETUP(GBR555)
				return (TRUE);

		case RGB5551:
			_BUILD_SETUP(RGB5551)
				return (TRUE);

		default:
			break;
	}
#else
	_BUILD_SETUP(RGB555)
		return (TRUE);
#endif

	return (FALSE);
}

#endif
