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

/*****
 * SPC7110 emulator - version 0.03 (2008-08-10)
 * Copyright (c) 2008, byuu and neviksti
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * The software is provided "as is" and the author disclaims all warranties
 * with regard to this software including all implied warranties of
 * merchantibility and fitness, in no event shall the author be liable for
 * any special, direct, indirect, or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether in an
 * action of contract, negligence or other tortious action, arising out of
 * or in connection with the use or performance of this software.
 *****/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "snes9x.h"
#include "memmap.h"
#include "getset.h"
#include "srtc.h"
#include "display.h"

#include "spc7110emu.h"
#include "spc7110dec.h"

/*read() will spool chunks half the size of SPC7110_DECOMP_BUFFER_SIZE*/
uint8* decomp_buffer;

static unsigned decomp_mode;
static unsigned decomp_offset;

static unsigned decomp_buffer_rdoffset;
static unsigned decomp_buffer_wroffset;
static unsigned decomp_buffer_length;

ContextState context[32];

#define memory_cartrom_read(a)		Memory.ROM[(a)]
#define memory_cartrtc_read(a)		RTCData.reg[(a)]
#define memory_cartrtc_write(a, b)	{ RTCData.reg[(a)] = (b); }

/*==================*/
/*decompression unit*/
/*==================*/
uint8 r4801; /*compression table low*/
uint8 r4802; /*compression table high*/
uint8 r4803; /*compression table bank*/
uint8 r4804; /*compression table index*/
uint8 r4805; /*decompression buffer index low*/
uint8 r4806; /*decompression buffer index high*/
uint8 r4807; /*???*/
uint8 r4808; /*???*/
uint8 r4809; /*compression length low*/
uint8 r480a; /*compression length high*/
uint8 r480b; /*decompression control register*/
uint8 r480c; /*decompression status*/

/*==============*/
/*data port unit*/
/*==============*/
uint8 r4811; /*data pointer low*/
uint8 r4812; /*data pointer high*/
uint8 r4813; /*data pointer bank*/
uint8 r4814; /*data adjust low*/
uint8 r4815; /*data adjust high*/
uint8 r4816; /*data increment low*/
uint8 r4817; /*data increment high*/
uint8 r4818; /*data port control register*/

uint8 r481x;

bool8 r4814_latch;
bool8 r4815_latch;

/*=========*/
/*math unit*/
/*=========*/
uint8 r4820; /*16-bit multiplicand B0, 32-bit dividend B0*/
uint8 r4821; /*16-bit multiplicand B1, 32-bit dividend B1*/
uint8 r4822; /*32-bit dividend B2*/
uint8 r4823; /*32-bit dividend B3*/
uint8 r4824; /*16-bit multiplier B0*/
uint8 r4825; /*16-bit multiplier B1*/
uint8 r4826; /*16-bit divisor B0*/
uint8 r4827; /*16-bit divisor B1*/
uint8 r4828; /*32-bit product B0, 32-bit quotient B0*/
uint8 r4829; /*32-bit product B1, 32-bit quotient B1*/
uint8 r482a; /*32-bit product B2, 32-bit quotient B2*/
uint8 r482b; /*32-bit product B3, 32-bit quotient B3*/
uint8 r482c; /*16-bit remainder B0*/
uint8 r482d; /*16-bit remainder B1*/
uint8 r482e; /*math control register*/
uint8 r482f; /*math status*/

/*===================*/
/*memory mapping unit*/
/*===================*/
uint8 r4830; /*SRAM write enable*/
uint8 r4831; /*$[d0-df]:[0000-ffff] mapping*/
uint8 r4832; /*$[e0-ef]:[0000-ffff] mapping*/
uint8 r4833; /*$[f0-ff]:[0000-ffff] mapping*/
uint8 r4834; /*???*/

unsigned dx_offset;
unsigned ex_offset;
unsigned fx_offset;

/*====================*/
/*real-time clock unit*/
/*====================*/
uint8 r4840; /*RTC latch*/
uint8 r4841; /*RTC index/data port*/
uint8 r4842; /*RTC status*/

#define RTCS_INACTIVE 0
#define RTCS_MODESELECT 1
#define RTCS_INDEXSELECT 2
#define RTCS_WRITE 3

#define  RTCM_LINEAR 0x03
#define RTCM_INDEXED 0x0c
static uint32 spc7110_rtc_mode;
static uint32 rtc_state;
unsigned rtc_index;


static unsigned morton16[2][256];
static unsigned morton32[4][256];

static const uint8 evolution_table[53][4] =
{
	/*{ prob, nextlps, nextmps, toggle invert },*/

	{ 0x5a,  1,  1, 1 },
	{ 0x25,  6,  2, 0 },
	{ 0x11,  8,  3, 0 },
	{ 0x08, 10,  4, 0 },
	{ 0x03, 12,  5, 0 },
	{ 0x01, 15,  5, 0 },

	{ 0x5a,  7,  7, 1 },
	{ 0x3f, 19,  8, 0 },
	{ 0x2c, 21,  9, 0 },
	{ 0x20, 22, 10, 0 },
	{ 0x17, 23, 11, 0 },
	{ 0x11, 25, 12, 0 },
	{ 0x0c, 26, 13, 0 },
	{ 0x09, 28, 14, 0 },
	{ 0x07, 29, 15, 0 },
	{ 0x05, 31, 16, 0 },
	{ 0x04, 32, 17, 0 },
	{ 0x03, 34, 18, 0 },
	{ 0x02, 35,  5, 0 },

	{ 0x5a, 20, 20, 1 },
	{ 0x48, 39, 21, 0 },
	{ 0x3a, 40, 22, 0 },
	{ 0x2e, 42, 23, 0 },
	{ 0x26, 44, 24, 0 },
	{ 0x1f, 45, 25, 0 },
	{ 0x19, 46, 26, 0 },
	{ 0x15, 25, 27, 0 },
	{ 0x11, 26, 28, 0 },
	{ 0x0e, 26, 29, 0 },
	{ 0x0b, 27, 30, 0 },
	{ 0x09, 28, 31, 0 },
	{ 0x08, 29, 32, 0 },
	{ 0x07, 30, 33, 0 },
	{ 0x05, 31, 34, 0 },
	{ 0x04, 33, 35, 0 },
	{ 0x04, 33, 36, 0 },
	{ 0x03, 34, 37, 0 },
	{ 0x02, 35, 38, 0 },
	{ 0x02, 36,  5, 0 },

	{ 0x58, 39, 40, 1 },
	{ 0x4d, 47, 41, 0 },
	{ 0x43, 48, 42, 0 },
	{ 0x3b, 49, 43, 0 },
	{ 0x34, 50, 44, 0 },
	{ 0x2e, 51, 45, 0 },
	{ 0x29, 44, 46, 0 },
	{ 0x25, 45, 24, 0 },

	{ 0x56, 47, 48, 1 },
	{ 0x4f, 47, 49, 0 },
	{ 0x47, 48, 50, 0 },
	{ 0x41, 49, 51, 0 },
	{ 0x3c, 50, 52, 0 },
	{ 0x37, 51, 43, 0 },
};

const uint8 mode2_context_table[32][2] = {
/*{ next 0, next 1 },*/

  {  1,  2 },

  {  3,  8 },
  { 13, 14 },

  { 15, 16 },
  { 17, 18 },
  { 19, 20 },
  { 21, 22 },
  { 23, 24 },
  { 25, 26 },
  { 25, 26 },
  { 25, 26 },
  { 25, 26 },
  { 25, 26 },
  { 27, 28 },
  { 29, 30 },

  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },
  { 31, 31 },

  { 31, 31 },
};

/*reverse morton lookup: de-interleave two 8-bit values*/
/*15, 13, 11,  9,  7,  5,  3,  1 -> 15- 8*/
/*14, 12, 10,  8,  6,  4,  2,  0 ->  7- 0*/
#define MORTON_2X8(data) (morton16[0][(data >>  0) & 255] + morton16[1][(data >>  8) & 255])

/*reverse morton lookup: de-interleave four 8-bit values*/
/*31, 27, 23, 19, 15, 11,  7,  3 -> 31-24*/
/*30, 26, 22, 18, 14, 10,  6,  2 -> 23-16*/
/*29, 25, 21, 17, 13,  9,  5,  1 -> 15- 8*/
/*28, 24, 20, 16, 12,  8,  4,  0 ->  7- 0*/
#define MORTON_4X8(data) (morton32[0][(data >>  0) & 255] + morton32[1][(data >>  8) & 255] + morton32[2][(data >> 16) & 255] + morton32[3][(data >> 24) & 255])

#define PROBABILITY(n) (evolution_table[context[n].index][0])
#define NEXT_LPS(n) (evolution_table[context[n].index][1])
#define NEXT_MPS(n) (evolution_table[context[n].index][2])
#define TOGGLE_INVERT(n) (evolution_table[context[n].index][3])

static uint8 spc7110_decomp_dataread (void)
{
	unsigned size = Memory.CalculatedSize - 0x100000;
	while(decomp_offset >= size)
		decomp_offset -= size;
	return memory_cartrom_read(0x100000 + decomp_offset++);
}

void spc7110_decomp_write(uint8 data)
{
	decomp_buffer[decomp_buffer_wroffset++] = data;
	decomp_buffer_wroffset &= SPC7110_DECOMP_BUFFER_SIZE - 1;
	decomp_buffer_length++;
}

static void spc7110_decomp_mode2(bool8 init)
{
	unsigned i, pixel, data;
	static unsigned pixelorder[16], realorder[16];
	static uint8 bitplanebuffer[16], buffer_index;
	static uint8 in, val, span;
	static int out0, out1, inverts, lps, in_count;

	if(init == TRUE)
	{
		for( i = 0; i < 16; i++)
			pixelorder[i] = i;

		buffer_index = 0;
		out0 = out1 = inverts = lps = 0;
		span = 0xff;
		val = spc7110_decomp_dataread();
		in = spc7110_decomp_dataread();
		in_count = 8;
		return;
	}

	while(decomp_buffer_length < (SPC7110_DECOMP_BUFFER_SIZE >> 1))
	{
		unsigned bit, a, b, c, con, refcon, m, n, prob, flag_lps, 
		invertbit, shift;
		for( pixel = 0; pixel < 8; pixel++)
		{
			/*get first symbol context*/
			a = ((out0 >> (0 * 4)) & 15);
			b = ((out0 >> (7 * 4)) & 15);
			c = ((out1 >> (0 * 4)) & 15);
			con = 0;
			refcon = (a == b) ? (b != c) : (b == c) ? 2 : 4 - (a == c);

			/*update pixel order*/
			for(m = 0; m < 16; m++)
				if(pixelorder[m] == a)
					break;
			for(n = m; n >  0; n--)
				pixelorder[n] = pixelorder[n - 1];
			pixelorder[0] = a;

			/*calculate the real pixel order*/
			for(m = 0; m < 16; m++)
				realorder[m] = pixelorder[m];

			/*rotate reference pixel c value to top*/
			for(m = 0; m < 16; m++)
				if(realorder[m] == c)
					break;
			for(n = m; n >  0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = c;

			/*rotate reference pixel b value to top*/
			for(m = 0; m < 16; m++) if(realorder[m] == b) break;
			for(n = m; n >  0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = b;

			/*rotate reference pixel a value to top*/
			for(m = 0; m < 16; m++) if(realorder[m] == a) break;
			for(n = m; n >  0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = a;

			/*get 4 symbols*/
			for( bit = 0; bit < 4; bit++)
			{
				/*get prob*/
				prob = PROBABILITY(con);

				/*get symbol*/
				if(val <= span - prob) { /*mps*/
					span = span - prob;
					flag_lps = 0;
				} else { /*lps*/
					val = val - (span - (prob - 1));
					span = prob - 1;
					flag_lps = 1;
				}

				/*renormalize*/
				shift = 0;
				while(span < 0x7f) {
					shift++;

					span = (span << 1) + 1;
					val = (val << 1) + (in >> 7);

					in <<= 1;
					if(--in_count == 0) {
						in = spc7110_decomp_dataread();
						in_count = 8;
					}
				}

				/*update processing info*/
				lps = (lps << 1) + flag_lps;
				invertbit = context[con].invert;
				inverts = (inverts << 1) + invertbit;

				/*update context state*/
				if(flag_lps & TOGGLE_INVERT(con)) context[con].invert ^= 1;
				if(flag_lps) context[con].index = NEXT_LPS(con);
				else if(shift) context[con].index = NEXT_MPS(con);

				/*get next context*/
				con = mode2_context_table[con][flag_lps ^ invertbit] + (con == 1 ? refcon : 0);
			}

			/*get pixel*/
			b = realorder[(lps ^ inverts) & 0x0f];
			out1 = (out1 << 4) + ((out0 >> 28) & 0x0f);
			out0 = (out0 << 4) + b;
		}

		/*convert pixel data into bitplanes*/
		data = MORTON_4X8(out0);
		spc7110_decomp_write(data >> 24);
		spc7110_decomp_write(data >> 16);
		bitplanebuffer[buffer_index++] = data >> 8;
		bitplanebuffer[buffer_index++] = data;

		if(buffer_index == 16)
		{
			for( i = 0; i < 16; i++)
				spc7110_decomp_write(bitplanebuffer[i]);
			buffer_index = 0;
		}
	}
}

void spc7110_decomp_mode1(bool8 init)
{
	unsigned i, bit, pixel, data;
	static unsigned pixelorder[4], realorder[4];
	static uint8 in, val, span;
	static int out, inverts, lps, in_count;

	if(init == TRUE)
	{
		for( i = 0; i < 4; i++) pixelorder[i] = i;
		out = inverts = lps = 0;
		span = 0xff;
		val = spc7110_decomp_dataread();
		in = spc7110_decomp_dataread();
		in_count = 8;
		return;
	}

	while(decomp_buffer_length < (SPC7110_DECOMP_BUFFER_SIZE >> 1))
	{
		for( pixel = 0; pixel < 8; pixel++)
		{
			unsigned a, b, c, con, m, n, prob, flag_lps, shift;
			/*get first symbol context*/
			a = ((out >> (1 * 2)) & 3);
			b = ((out >> (7 * 2)) & 3);
			c = ((out >> (8 * 2)) & 3);
			con = (a == b) ? (b != c) : (b == c) ? 2 : 4 - (a == c);

			/*update pixel order*/
			for(m = 0; m < 4; m++) if(pixelorder[m] == a) break;
			for(n = m; n > 0; n--) pixelorder[n] = pixelorder[n - 1];
			pixelorder[0] = a;

			/*calculate the real pixel order*/
			for(m = 0; m < 4; m++) realorder[m] = pixelorder[m];

			/*rotate reference pixel c value to top*/
			for(m = 0; m < 4; m++) if(realorder[m] == c) break;
			for(n = m; n > 0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = c;

			/*rotate reference pixel b value to top*/
			for(m = 0; m < 4; m++) if(realorder[m] == b) break;
			for(n = m; n > 0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = b;

			/*rotate reference pixel a value to top*/
			for(m = 0; m < 4; m++) if(realorder[m] == a) break;
			for(n = m; n > 0; n--) realorder[n] = realorder[n - 1];
			realorder[0] = a;

			/*get 2 symbols*/
			for( bit = 0; bit < 2; bit++)
			{
				/*get prob*/
				prob = PROBABILITY(con);

				/*get symbol*/
				if(val <= span - prob) { /*mps*/
					span = span - prob;
					flag_lps = 0;
				} else { /*lps*/
					val = val - (span - (prob - 1));
					span = prob - 1;
					flag_lps = 1;
				}

				/*renormalize*/
				shift = 0;
				while(span < 0x7f) {
					shift++;

					span = (span << 1) + 1;
					val = (val << 1) + (in >> 7);

					in <<= 1;
					if(--in_count == 0)
					{
						in = spc7110_decomp_dataread();
						in_count = 8;
					}
				}

				/*update processing info*/
				lps = (lps << 1) + flag_lps;
				inverts = (inverts << 1) + context[con].invert;

				/*update context state*/
				if(flag_lps & TOGGLE_INVERT(con))
					context[con].invert ^= 1;
				if(flag_lps)
					context[con].index = NEXT_LPS(con);
				else if(shift)
					context[con].index = NEXT_MPS(con);

				/*get next context*/
				con = 5 + (con << 1) + ((lps ^ inverts) & 1);
			}

			/*get pixel*/
			b = realorder[(lps ^ inverts) & 3];
			out = (out << 2) + b;
		}

		/*turn pixel data into bitplanes*/
		data = MORTON_2X8(out);
		spc7110_decomp_write(data >> 8);
		spc7110_decomp_write(data);
	}
}

static void spc7110_decomp_mode0(bool8 init)
{
	unsigned bit;
	static uint8 val, in, span;
	static int out, inverts, lps, in_count;

	if(init == TRUE)
	{
		out = inverts = lps = 0;
		span = 0xff;
		val = spc7110_decomp_dataread();
		in = spc7110_decomp_dataread();
		in_count = 8;
		return;
	}

	while(decomp_buffer_length < (SPC7110_DECOMP_BUFFER_SIZE >> 1))
	{
		for( bit = 0; bit < 8; bit++)
		{
			unsigned prob, mps, flag_lps, shift;
			/*get context*/
			uint8 mask = (1 << (bit & 3)) - 1;
			uint8 con = mask + ((inverts & mask) ^ (lps & mask));
			if(bit > 3) con += 15;

			/*get prob and mps*/
			prob = PROBABILITY(con);
			mps = (((out >> 15) & 1) ^ context[con].invert);

			/*get bit*/
			if(val <= span - prob) { /*mps*/
				span = span - prob;
				out = (out << 1) + mps;
				flag_lps = 0;
			} else { /*lps*/
				val = val - (span - (prob - 1));
				span = prob - 1;
				out = (out << 1) + 1 - mps;
				flag_lps = 1;
			}

			/*renormalize*/
			shift = 0;
			while(span < 0x7f) {
				shift++;

				span = (span << 1) + 1;
				val = (val << 1) + (in >> 7);

				in <<= 1;
				if(--in_count == 0) {
					in = spc7110_decomp_dataread();
					in_count = 8;
				}
			}

			/*update processing info*/
			lps = (lps << 1) + flag_lps;
			inverts = (inverts << 1) + context[con].invert;

			/*update context state*/
			if(flag_lps & TOGGLE_INVERT(con))
				context[con].invert ^= 1;
			if(flag_lps)
				context[con].index = NEXT_LPS(con);
			else if(shift)
				context[con].index = NEXT_MPS(con);
		}

		/*save byte*/
		spc7110_decomp_write(out);
	}
}

uint8 spc7110_decomp_read (void)
{
	uint8 data;

	if(decomp_buffer_length == 0)
	{
		/*decompress at least (SPC7110_DECOMP_BUFFER_SIZE / 2) bytes to the buffer*/
		switch(decomp_mode)
		{
			case 0:
				spc7110_decomp_mode0(FALSE);
				break;
			case 1:
				spc7110_decomp_mode1(FALSE);
				break;
			case 2:
				spc7110_decomp_mode2(FALSE);
				break;
			default:
				return 0x00;
		}
	}

	data = decomp_buffer[decomp_buffer_rdoffset++];
	decomp_buffer_rdoffset &= SPC7110_DECOMP_BUFFER_SIZE - 1;
	decomp_buffer_length--;
	return data;
}

static void spc7110_decomp_init(unsigned mode, unsigned offset, unsigned index)
{
	unsigned i;
	decomp_mode = mode;
	decomp_offset = offset;

	decomp_buffer_rdoffset = 0;
	decomp_buffer_wroffset = 0;
	decomp_buffer_length   = 0;

	/*reset context states*/
	for( i = 0; i < 32; i++)
	{
		context[i].index  = 0;
		context[i].invert = 0;
	}

	switch(decomp_mode)
	{
		case 0:
			spc7110_decomp_mode0(TRUE);
			break;
		case 1:
			spc7110_decomp_mode1(TRUE);
			break;
		case 2:
			spc7110_decomp_mode2(TRUE);
			break;
	}

	/*decompress up to requested output data index*/
	while(index--)
		spc7110_decomp_read();
}

void spc7110_decomp_reset (void)
{
	/*mode 3 is invalid; this is treated as a special case to always return 0x00*/
	/*set to mode 3 so that reading decomp port before starting first decomp will return 0x00*/
	decomp_mode = 3;

	decomp_buffer_rdoffset = 0;
	decomp_buffer_wroffset = 0;
	decomp_buffer_length   = 0;
}

void spc7110_decomp_start (void)
{
	unsigned i;
	decomp_buffer = (uint8_t*)malloc(SPC7110_DECOMP_BUFFER_SIZE);
	spc7110_decomp_reset();

	/*initialize reverse morton lookup tables*/
	for( i = 0; i < 256; i++)
	{
#define map(x, y) (((i >> x) & 1) << y)
		/*2x8-bit*/
		morton16[1][i] = map(7, 15) + map(6,  7) + map(5, 14) + map(4,  6)
			+ map(3, 13) + map(2,  5) + map(1, 12) + map(0,  4);
		morton16[0][i] = map(7, 11) + map(6,  3) + map(5, 10) + map(4,  2)
			+ map(3,  9) + map(2,  1) + map(1,  8) + map(0,  0);
		/*4x8-bit*/
		morton32[3][i] = map(7, 31) + map(6, 23) + map(5, 15) + map(4,  7)
			+ map(3, 30) + map(2, 22) + map(1, 14) + map(0,  6);
		morton32[2][i] = map(7, 29) + map(6, 21) + map(5, 13) + map(4,  5)
			+ map(3, 28) + map(2, 20) + map(1, 12) + map(0,  4);
		morton32[1][i] = map(7, 27) + map(6, 19) + map(5, 11) + map(4,  3)
			+ map(3, 26) + map(2, 18) + map(1, 10) + map(0,  2);
		morton32[0][i] = map(7, 25) + map(6, 17) + map(5,  9) + map(4,  1)
			+ map(3, 24) + map(2, 16) + map(1,  8) + map(0,  0);
#undef map
	}
}

static const unsigned months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

unsigned s7_datarom_addr(unsigned addr)
{
	unsigned size = Memory.CalculatedSize - 0x100000;
	while(addr >= size)
		addr -= size;
	return addr + 0x100000;
}

unsigned s7_data_adjust (void)
{
	return r4814 + (r4815 << 8);
}

static unsigned s7_data_pointer (void)
{
	return r4811 + (r4812 << 8) + (r4813 << 16);
}

static void s7_set_data_pointer(unsigned addr)
{
	r4811 = addr;
	r4812 = addr >> 8;
	r4813 = addr >> 16;
}

static void s7_update_time(int offset)
{
	bool8 update;
	time_t rtc_time, current_time, diff;

	rtc_time = (memory_cartrtc_read(16) <<  0)
		| (memory_cartrtc_read(17) <<  8)
		| (memory_cartrtc_read(18) << 16)
		| (memory_cartrtc_read(19) << 24);

	current_time = time(0) - offset;

	/*sizeof(time_t) is platform-dependent; though memory::cartrtc needs to be platform-agnostic.*/
	/*yet platforms with 32-bit signed time_t will overflow every ~68 years. handle this by*/
	/*accounting for overflow at the cost of 1-bit precision (to catch underflow). this will allow*/
	/*memory::cartrtc timestamp to remain valid for up to ~34 years from the last update, even if*/
	/*time_t overflows. calculation should be valid regardless of number representation, time_t size,*/
	/*or whether time_t is signed or unsigned.*/
	diff = (current_time >= rtc_time)
		? (current_time - rtc_time)
		: ((time_t)-1 - rtc_time + current_time + 1);  /*compensate for overflow*/
	if(diff > (time_t)-1 / 2)
		diff = 0;            /*compensate for underflow*/

	update = TRUE;

	if(memory_cartrtc_read(13) & 1)
		update = FALSE;  /*do not update if CR0 timer disable flag is set*/
	if(memory_cartrtc_read(15) & 3)
		update = FALSE;  /*do not update if CR2 timer disable flags are set*/

	if(diff > 0 && update == TRUE)
	{
		unsigned second, minute, hour, day, days, month, year, weekday;
		bool8 leapyear;

		second  = memory_cartrtc_read( 0) + memory_cartrtc_read( 1) * 10;
		minute  = memory_cartrtc_read( 2) + memory_cartrtc_read( 3) * 10;
		hour    = memory_cartrtc_read( 4) + memory_cartrtc_read( 5) * 10;
		day     = memory_cartrtc_read( 6) + memory_cartrtc_read( 7) * 10;
		month   = memory_cartrtc_read( 8) + memory_cartrtc_read( 9) * 10;
		year    = memory_cartrtc_read(10) + memory_cartrtc_read(11) * 10;
		weekday = memory_cartrtc_read(12);

		day--;
		month--;
		year += (year >= 90) ? 1900 : 2000;  /*range = 1990-2089*/

		second += diff;
		while(second >= 60)
		{
			second -= 60;

			minute++;
			if(minute < 60)
				continue;
			minute = 0;

			hour++;
			if(hour < 24)
				continue;
			hour = 0;

			day++;
			weekday = (weekday + 1) % 7;
			days = months[month % 12];
			if(days == 28)
			{
				leapyear = FALSE;
				if((year % 4) == 0)
				{
					leapyear = TRUE;
					if((year % 100) == 0 && (year % 400) != 0)
						leapyear = FALSE;
				}

				if(leapyear)
					days++;
			}
			if(day < days)
				continue;
			day = 0;

			month++;
			if(month < 12)
				continue;
			month = 0;

			year++;
		}

		day++;
		month++;
		year %= 100;

		memory_cartrtc_write( 0, second % 10);
		memory_cartrtc_write( 1, second / 10);
		memory_cartrtc_write( 2, minute % 10);
		memory_cartrtc_write( 3, minute / 10);
		memory_cartrtc_write( 4, hour % 10);
		memory_cartrtc_write( 5, hour / 10);
		memory_cartrtc_write( 6, day % 10);
		memory_cartrtc_write( 7, day / 10);
		memory_cartrtc_write( 8, month % 10);
		memory_cartrtc_write( 9, month / 10);
		memory_cartrtc_write(10, year % 10);
		memory_cartrtc_write(11, (year / 10) % 10);
		memory_cartrtc_write(12, weekday % 7);
	}

	memory_cartrtc_write(16, current_time >>  0);
	memory_cartrtc_write(17, current_time >>  8);
	memory_cartrtc_write(18, current_time >> 16);
	memory_cartrtc_write(19, current_time >> 24);
}

static void s7_mmio_write(unsigned addr, uint8 data)
{
	addr &= 0xffff;

	switch(addr)
	{
		/*==================*/
		/*decompression unit*/
		/*==================*/

		case 0x4801: r4801 = data; break;
		case 0x4802: r4802 = data; break;
		case 0x4803: r4803 = data; break;
		case 0x4804: r4804 = data; break;
		case 0x4805: r4805 = data; break;
		case 0x4806:
			     {
				     unsigned table, index, addr, mode, offset;
				     r4806 = data;

				     table   = (r4801 + (r4802 << 8) + (r4803 << 16));
				     index   = (r4804 << 2);
				     /*unsigned length  = (r4809 + (r480a << 8));*/
				     addr    = s7_datarom_addr(table + index);
				     mode    = (memory_cartrom_read(addr + 0));
				     offset  = (memory_cartrom_read(addr + 1) << 16)
					     + (memory_cartrom_read(addr + 2) <<  8)
					     + (memory_cartrom_read(addr + 3) <<  0);

				     spc7110_decomp_init(mode, offset, (r4805 + (r4806 << 8)) << mode);
				     r480c = 0x80;
			     } break;

		case 0x4807: r4807 = data; break;
		case 0x4808: r4808 = data; break;
		case 0x4809: r4809 = data; break;
		case 0x480a: r480a = data; break;
		case 0x480b: r480b = data; break;

			     /*==============*/
			     /*data port unit*/
			     /*==============*/

		case 0x4811:
			r4811 = data;
			r481x |= 0x01;
			break;
		case 0x4812:
			r4812 = data;
			r481x |= 0x02;
			break;
		case 0x4813:
			r4813 = data;
			r481x |= 0x04;
			break;
		case 0x4814:
			{
				unsigned increment;

				r4814 = data;
				r4814_latch = TRUE;
				if(!r4815_latch)
					break;
				if(!(r4818 & 2))
					break;
				if(r4818 & 0x10)
					break;

				if((r4818 & 0x60) == 0x20)
				{
					increment = s7_data_adjust() & 0xff;
					if(r4818 & 8)
						increment = (int8)increment;  /*8-bit sign extend*/
					s7_set_data_pointer(s7_data_pointer() + increment);
				}
				else if((r4818 & 0x60) == 0x40)
				{
					increment = s7_data_adjust();
					if(r4818 & 8)
						increment = (int16)increment;  /*16-bit sign extend*/
					s7_set_data_pointer(s7_data_pointer() + increment);
				}
			}
			break;
		case 0x4815:
			{
				unsigned increment;

				r4815 = data;
				r4815_latch = TRUE;
				if(!r4814_latch)
					break;
				if(!(r4818 & 2))
					break;
				if(r4818 & 0x10)
					break;

				if((r4818 & 0x60) == 0x20)
				{
					increment = s7_data_adjust() & 0xff;
					if(r4818 & 8) increment = (int8)increment;  /*8-bit sign extend*/
					s7_set_data_pointer(s7_data_pointer() + increment);
				}
				else if((r4818 & 0x60) == 0x40)
				{
					increment = s7_data_adjust();
					if(r4818 & 8)
						increment = (int16)increment;  /*16-bit sign extend*/
					s7_set_data_pointer(s7_data_pointer() + increment);
				}
			}
			break;
		case 0x4816:
			r4816 = data;
			break;
		case 0x4817:
			r4817 = data;
			break;
		case 0x4818:
			{
				if(r481x != 0x07)
					break;

				r4818 = data;
				r4814_latch = r4815_latch = FALSE;
			}
			break;

			/*=========*/
			/*math unit*/
			/*=========*/
		case 0x4820:
			r4820 = data;
			break;
		case 0x4821:
			r4821 = data;
			break;
		case 0x4822:
			r4822 = data;
			break;
		case 0x4823:
			r4823 = data;
			break;
		case 0x4824:
			r4824 = data;
			break;
		case 0x4825:
			{
				signed result;
				r4825 = data;

				if(r482e & 1)
				{
					/*signed 16-bit x 16-bit multiplication*/
					int16 r0 = (int16)(r4824 + (r4825 << 8));
					int16 r1 = (int16)(r4820 + (r4821 << 8));

					result = r0 * r1;
					r4828 = result;
					r4829 = result >> 8;
					r482a = result >> 16;
					r482b = result >> 24;
				}
				else
				{
					/*unsigned 16-bit x 16-bit multiplication*/
					uint16 r0 = (uint16)(r4824 + (r4825 << 8));
					uint16 r1 = (uint16)(r4820 + (r4821 << 8));

					result = r0 * r1;
					r4828 = result;
					r4829 = result >> 8;
					r482a = result >> 16;
					r482b = result >> 24;
				}

				r482f = 0x80;
			} break;
		case 0x4826:
			r4826 = data;
			break;
		case 0x4827:
			{
				r4827 = data;

				if(r482e & 1)
				{
					int32 dividend, quotient;
					int16 divisor, remainder;
					/*signed 32-bit x 16-bit division*/
					dividend = (int32)(r4820 + (r4821 << 8) + (r4822 << 16) + (r4823 << 24));
					divisor  = (int16)(r4826 + (r4827 << 8));

					if(divisor)
					{
						quotient  = (int32)(dividend / divisor);
						remainder = (int32)(dividend % divisor);
					}
					else
					{
						/*illegal division by zero*/
						quotient  = 0;
						remainder = dividend & 0xffff;
					}

					r4828 = quotient;
					r4829 = quotient >> 8;
					r482a = quotient >> 16;
					r482b = quotient >> 24;

					r482c = remainder;
					r482d = remainder >> 8;
				}
				else
				{
					uint32 dividend, quotient;
					uint16 divisor, remainder;
					/*unsigned 32-bit x 16-bit division*/
					dividend = (uint32)(r4820 + (r4821 << 8) + (r4822 << 16) + (r4823 << 24));
					divisor  = (uint16)(r4826 + (r4827 << 8));

					if(divisor)
					{
						quotient  = (uint32)(dividend / divisor);
						remainder = (uint16)(dividend % divisor);
					}
					else
					{
						/*illegal division by zero*/
						quotient  = 0;
						remainder = dividend & 0xffff;
					}

					r4828 = quotient;
					r4829 = quotient >> 8;
					r482a = quotient >> 16;
					r482b = quotient >> 24;

					r482c = remainder;
					r482d = remainder >> 8;
				}

				r482f = 0x80;
			}
			break;
		case 0x482e:
			{
				/*reset math unit*/
				r4820 = r4821 = r4822 = r4823 = 0;
				r4824 = r4825 = r4826 = r4827 = 0;
				r4828 = r4829 = r482a = r482b = 0;
				r482c = r482d = 0;

				r482e = data;
			}
			break;
			/*===================*/
			/*memory mapping unit*/
			/*===================*/
		case 0x4830:
			r4830 = data;
			break;
		case 0x4831:
			{
				r4831 = data;
				dx_offset = s7_datarom_addr((data & 7) * 0x100000);
			}
			break;
		case 0x4832:
			{
				r4832 = data;
				ex_offset = s7_datarom_addr((data & 7) * 0x100000);
			}
			break;
		case 0x4833:
			{
				r4833 = data;
				fx_offset = s7_datarom_addr((data & 7) * 0x100000);
			}
			break;
		case 0x4834:
			r4834 = data; break;
			/*====================*/
			/*real-time clock unit*/
			/*====================*/
		case 0x4840:
			{
				r4840 = data;
				if(!(r4840 & 1))
				{
					/*disable RTC*/
					rtc_state = RTCS_INACTIVE;
					s7_update_time(0);
				}
				else
				{
					/*enable RTC*/
					r4842 = 0x80;
					rtc_state = RTCS_MODESELECT;
				}
			}
			break;
		case 0x4841:
			     {
				     r4841 = data;

				     switch(rtc_state)
				     {
					     case RTCS_MODESELECT:
						     {
							     if(data == RTCM_LINEAR || data == RTCM_INDEXED)
							     {
								     r4842 = 0x80;
								     rtc_state = RTCS_INDEXSELECT;
								     spc7110_rtc_mode  = data;
								     rtc_index = 0;
							     }
						     }
						     break;
					     case RTCS_INDEXSELECT:
						     {
							     r4842 = 0x80;
							     rtc_index = data & 15;
							     if(spc7110_rtc_mode == RTCM_LINEAR)
								     rtc_state = RTCS_WRITE;
						     }
						     break;
					     case RTCS_WRITE:
						     {
							     r4842 = 0x80;

							     /*control register 0*/
							     if(rtc_index == 13)
							     {
								     /*increment second counter*/
								     if(data & 2)
								     	s7_update_time(+1);

								     /*round minute counter*/
								     if(data & 8)
								     {
									     unsigned second;
									     s7_update_time(0);

									     second = memory_cartrtc_read( 0) + memory_cartrtc_read( 1) * 10;
									     /*clear seconds*/
									     memory_cartrtc_write(0, 0);
									     memory_cartrtc_write(1, 0);

									     if(second >= 30)
										     s7_update_time(+60);
								     }
							     }

							     /*control register 2*/
							     if(rtc_index == 15)
							     {
								     /*disable timer and clear second counter*/
								     if((data & 1) && !(memory_cartrtc_read(15) & 1))
								     {
									     s7_update_time(0);

									     /*clear seconds*/
									     memory_cartrtc_write(0, 0);
									     memory_cartrtc_write(1, 0);
								     }

								     /*disable timer*/
								     if((data & 2) && !(memory_cartrtc_read(15) & 2))
								     {
									     s7_update_time(0);
								     }
							     }

							     memory_cartrtc_write(rtc_index, data & 15);
							     rtc_index = (rtc_index + 1) & 15;
						     }
						     break;
					     case RTCS_INACTIVE:
						     break;
				     } /*switch(rtc_state)*/
			     }
			     break;
	}
}

static void s7_power (void)
{
	r4801 = 0x00;
	r4802 = 0x00;
	r4803 = 0x00;
	r4804 = 0x00;
	r4805 = 0x00;
	r4806 = 0x00;
	r4807 = 0x00;
	r4808 = 0x00;
	r4809 = 0x00;
	r480a = 0x00;
	r480b = 0x00;
	r480c = 0x00;

	spc7110_decomp_reset();

	r4811 = 0x00;
	r4812 = 0x00;
	r4813 = 0x00;
	r4814 = 0x00;
	r4815 = 0x00;
	r4816 = 0x00;
	r4817 = 0x00;
	r4818 = 0x00;

	r481x = 0x00;
	r4814_latch = FALSE;
	r4815_latch = FALSE;

	r4820 = 0x00;
	r4821 = 0x00;
	r4822 = 0x00;
	r4823 = 0x00;
	r4824 = 0x00;
	r4825 = 0x00;
	r4826 = 0x00;
	r4827 = 0x00;
	r4828 = 0x00;
	r4829 = 0x00;
	r482a = 0x00;
	r482b = 0x00;
	r482c = 0x00;
	r482d = 0x00;
	r482e = 0x00;
	r482f = 0x00;

	r4830 = 0x00;
	s7_mmio_write(0x4831, 0);
	s7_mmio_write(0x4832, 1);
	s7_mmio_write(0x4833, 2);
	r4834 = 0x00;

	r4840 = 0x00;
	r4841 = 0x00;
	r4842 = 0x00;

	if(Settings.SPC7110RTC)
	{
		rtc_state = RTCS_INACTIVE;
		spc7110_rtc_mode  = RTCM_LINEAR;
		rtc_index = 0;
	}
}




static unsigned s7_data_increment (void)
{
	return r4816 + (r4817 << 8);
}


static void s7_set_data_adjust(unsigned addr)
{
	r4814 = addr;
	r4815 = addr >> 8;
}


static uint8 s7_mmio_read(unsigned addr)
{
	addr &= 0xffff;

	switch(addr)
	{
		/*==================*/
		/*decompression unit*/
		/*==================*/

		case 0x4800:
			{
				uint16 counter = (r4809 + (r480a << 8));
				counter--;
				r4809 = counter;
				r480a = counter >> 8;
				return spc7110_decomp_read();
			}
		case 0x4801:
			return r4801;
		case 0x4802:
			return r4802;
		case 0x4803:
			return r4803;
		case 0x4804:
			return r4804;
		case 0x4805:
			return r4805;
		case 0x4806:
			return r4806;
		case 0x4807:
			return r4807;
		case 0x4808:
			return r4808;
		case 0x4809:
			return r4809;
		case 0x480a:
			return r480a;
		case 0x480b:
			return r480b;
		case 0x480c:
			{
				uint8 status = r480c;
				r480c &= 0x7f;
				return status;
			}

			/*==============*/
			/*data port unit*/
			/*==============*/
		case 0x4810:
			{
				unsigned addr, adjust, adjustaddr, increment;
				uint8 data;

				if(r481x != 0x07)
					return 0x00;

				addr = s7_data_pointer();
				adjust = s7_data_adjust();
				if(r4818 & 8)
					adjust = (int16)adjust;  /*16-bit sign extend*/

				adjustaddr = addr;
				if(r4818 & 2)
				{
					adjustaddr += adjust;
					s7_set_data_adjust(adjust + 1);
				}

				data = memory_cartrom_read(s7_datarom_addr(adjustaddr));
				if(!(r4818 & 2))
				{
					increment = (r4818 & 1) ? s7_data_increment() : 1;
					if(r4818 & 4)
						increment = (int16)increment;  /*16-bit sign extend*/

					if((r4818 & 16) == 0)
						s7_set_data_pointer(addr + increment);
					else
						s7_set_data_adjust(adjust + increment);
				}

				return data;
			}
		case 0x4811:
			return r4811;
		case 0x4812:
			return r4812;
		case 0x4813:
			return r4813;
		case 0x4814:
			return r4814;
		case 0x4815:
			return r4815;
		case 0x4816:
			return r4816;
		case 0x4817:
			return r4817;
		case 0x4818:
			return r4818;
		case 0x481a:
			{
				unsigned addr, adjust;
				uint8 data;

				if(r481x != 0x07)
					return 0x00;

				addr = s7_data_pointer();
				adjust = s7_data_adjust();

				if(r4818 & 8)
					adjust = (int16)adjust;  /*16-bit sign extend*/

				data = memory_cartrom_read(s7_datarom_addr(addr + adjust));
				if((r4818 & 0x60) == 0x60)
				{
					if((r4818 & 16) == 0)
						s7_set_data_pointer(addr + adjust);
					else
						s7_set_data_adjust(adjust + adjust);
				}

				return data;
			}

			     /*=========*/
			     /*math unit*/
			     /*=========*/

		case 0x4820: return r4820;
		case 0x4821: return r4821;
		case 0x4822: return r4822;
		case 0x4823: return r4823;
		case 0x4824: return r4824;
		case 0x4825: return r4825;
		case 0x4826: return r4826;
		case 0x4827: return r4827;
		case 0x4828: return r4828;
		case 0x4829: return r4829;
		case 0x482a: return r482a;
		case 0x482b: return r482b;
		case 0x482c: return r482c;
		case 0x482d: return r482d;
		case 0x482e: return r482e;
		case 0x482f:
			     {
				     uint8 status;
				     status = r482f;
				     r482f &= 0x7f;
				     return status;
			     }

			     /*===================*/
			     /*memory mapping unit*/
			     /*===================*/

		case 0x4830: return r4830;
		case 0x4831: return r4831;
		case 0x4832: return r4832;
		case 0x4833: return r4833;
		case 0x4834: return r4834;

			     /*====================*/
			     /*real-time clock unit*/
			     /*====================*/

		case 0x4840: return r4840;
		case 0x4841:
			     {
				     uint8 data;

				     if(rtc_state == RTCS_INACTIVE || rtc_state == RTCS_MODESELECT)
					     return 0x00;

				     r4842 = 0x80;
				     data = memory_cartrtc_read(rtc_index);
				     rtc_index = (rtc_index + 1) & 15;
				     return data;
			     }
		case 0x4842:
			     {
				     uint8 status;
				     status = r4842;
				     r4842 &= 0x7f;
				     return status;
			     }
	}

	return OpenBus;
}

void S9xInitSPC7110 (void)
{
	spc7110_decomp_start();
	s7_power();
	memset(RTCData.reg, 0, 20);
}

void S9xResetSPC7110 (void)
{
	s7_power();
}

void S9xFreeSPC7110 (void)
{
	if(decomp_buffer)
		free(decomp_buffer);
}

static void SetSPC7110SRAMMap (uint8 newstate)
{
	if (newstate & 0x80)
	{
		Memory.Map[0x006] = (uint8 *) MAP_HIROM_SRAM;
		Memory.Map[0x007] = (uint8 *) MAP_HIROM_SRAM;
		Memory.Map[0x306] = (uint8 *) MAP_HIROM_SRAM;
		Memory.Map[0x307] = (uint8 *) MAP_HIROM_SRAM;
	}
	else
	{
		Memory.Map[0x006] = (uint8 *) MAP_RONLY_SRAM;
		Memory.Map[0x007] = (uint8 *) MAP_RONLY_SRAM;
		Memory.Map[0x306] = (uint8 *) MAP_RONLY_SRAM;
		Memory.Map[0x307] = (uint8 *) MAP_RONLY_SRAM;
	}
}

uint8 * S9xGetBasePointerSPC7110 (uint32 address)
{
	uint32	i;
	
	switch (address & 0xf00000)
	{
		case 0xd00000:
			i = dx_offset;
			break;
			
		case 0xe00000:
			i = ex_offset;
			break;
			
		case 0xf00000:
			i = fx_offset;
			break;
			
		default:
			i = 0;
			break;
	}
	
	i += address & 0x0f0000;

	return (&Memory.ROM[i]);
}

uint8 S9xGetSPC7110Byte (uint32 address)
{
	uint32 i;
	
	i = 0;

	switch (address & 0xf00000)
	{
		case 0xd00000:
			i = dx_offset;
			break;

		case 0xe00000:
			i = ex_offset;
			break;

		case 0xf00000:
			i = fx_offset;
			break;
	}

	i += address & 0x0fffff;

	return (Memory.ROM[i]);
}

uint8 S9xGetSPC7110 (uint16 address)
{
	if (!Settings.SPC7110RTC && address > 0x483f)
		return (OpenBus);
	
	return (s7_mmio_read(address));
}

void S9xSetSPC7110 (uint8 byte, uint16 address)
{
	if (!Settings.SPC7110RTC && address > 0x483f)
		return;

	if (address == 0x4830)
		SetSPC7110SRAMMap(byte);

	s7_mmio_write(address, byte);
}

void S9xSPC7110PreSaveState (void)
{
	int i;
	s7snap.r4801 = r4801;
	s7snap.r4802 = r4802;
	s7snap.r4803 = r4803;
	s7snap.r4804 = r4804;
	s7snap.r4805 = r4805;
	s7snap.r4806 = r4806;
	s7snap.r4807 = r4807;
	s7snap.r4808 = r4808;
	s7snap.r4809 = r4809;
	s7snap.r480a = r480a;
	s7snap.r480b = r480b;
	s7snap.r480c = r480c;

	s7snap.r4811 = r4811;
	s7snap.r4812 = r4812;
	s7snap.r4813 = r4813;
	s7snap.r4814 = r4814;
	s7snap.r4815 = r4815;
	s7snap.r4816 = r4816;
	s7snap.r4817 = r4817;
	s7snap.r4818 = r4818;

	s7snap.r481x = r481x;

	s7snap.r4814_latch = r4814_latch ? TRUE : FALSE;
	s7snap.r4815_latch = r4815_latch ? TRUE : FALSE;

	s7snap.r4820 = r4820;
	s7snap.r4821 = r4821;
	s7snap.r4822 = r4822;
	s7snap.r4823 = r4823;
	s7snap.r4824 = r4824;
	s7snap.r4825 = r4825;
	s7snap.r4826 = r4826;
	s7snap.r4827 = r4827;
	s7snap.r4828 = r4828;
	s7snap.r4829 = r4829;
	s7snap.r482a = r482a;
	s7snap.r482b = r482b;
	s7snap.r482c = r482c;
	s7snap.r482d = r482d;
	s7snap.r482e = r482e;
	s7snap.r482f = r482f;

	s7snap.r4830 = r4830;
	s7snap.r4831 = r4831;
	s7snap.r4832 = r4832;
	s7snap.r4833 = r4833;
	s7snap.r4834 = r4834;

	s7snap.dx_offset = (uint32) dx_offset;
	s7snap.ex_offset = (uint32) ex_offset;
	s7snap.fx_offset = (uint32) fx_offset;

	s7snap.r4840 = r4840;
	s7snap.r4841 = r4841;
	s7snap.r4842 = r4842;

	s7snap.rtc_state = (int32)  rtc_state;
	s7snap.rtc_mode  = (int32)  spc7110_rtc_mode;
	s7snap.rtc_index = (uint32) rtc_index;

	s7snap.decomp_mode   = (uint32) decomp_mode;
	s7snap.decomp_offset = (uint32) decomp_offset;

	for ( i = 0; i < SPC7110_DECOMP_BUFFER_SIZE; i++)
		s7snap.decomp_buffer[i] = decomp_buffer[i];

	s7snap.decomp_buffer_rdoffset = (uint32) decomp_buffer_rdoffset;
	s7snap.decomp_buffer_wroffset = (uint32) decomp_buffer_wroffset;
	s7snap.decomp_buffer_length   = (uint32) decomp_buffer_length;

	for ( i = 0; i < 32; i++)
	{
		s7snap.context[i].index  = context[i].index;
		s7snap.context[i].invert = context[i].invert;
	}
}

void S9xSPC7110PostLoadState (int version)//version unused
{
	r4801 = s7snap.r4801;
	r4802 = s7snap.r4802;
	r4803 = s7snap.r4803;
	r4804 = s7snap.r4804;
	r4805 = s7snap.r4805;
	r4806 = s7snap.r4806;
	r4807 = s7snap.r4807;
	r4808 = s7snap.r4808;
	r4809 = s7snap.r4809;
	r480a = s7snap.r480a;
	r480b = s7snap.r480b;
	r480c = s7snap.r480c;

	r4811 = s7snap.r4811;
	r4812 = s7snap.r4812;
	r4813 = s7snap.r4813;
	r4814 = s7snap.r4814;
	r4815 = s7snap.r4815;
	r4816 = s7snap.r4816;
	r4817 = s7snap.r4817;
	r4818 = s7snap.r4818;

	r481x = s7snap.r481x;

	r4814_latch = s7snap.r4814_latch ? TRUE : FALSE;
	r4815_latch = s7snap.r4815_latch ? TRUE : FALSE;

	r4820 = s7snap.r4820;
	r4821 = s7snap.r4821;
	r4822 = s7snap.r4822;
	r4823 = s7snap.r4823;
	r4824 = s7snap.r4824;
	r4825 = s7snap.r4825;
	r4826 = s7snap.r4826;
	r4827 = s7snap.r4827;
	r4828 = s7snap.r4828;
	r4829 = s7snap.r4829;
	r482a = s7snap.r482a;
	r482b = s7snap.r482b;
	r482c = s7snap.r482c;
	r482d = s7snap.r482d;
	r482e = s7snap.r482e;
	r482f = s7snap.r482f;

	r4830 = s7snap.r4830;
	r4831 = s7snap.r4831;
	r4832 = s7snap.r4832;
	r4833 = s7snap.r4833;
	r4834 = s7snap.r4834;

	dx_offset = (unsigned) s7snap.dx_offset;
	ex_offset = (unsigned) s7snap.ex_offset;
	fx_offset = (unsigned) s7snap.fx_offset;

	r4840 = s7snap.r4840;
	r4841 = s7snap.r4841;
	r4842 = s7snap.r4842;

	rtc_state = s7snap.rtc_state;
	spc7110_rtc_mode  = s7snap.rtc_mode;
	rtc_index = (unsigned)           s7snap.rtc_index;

	decomp_mode   = (unsigned) s7snap.decomp_mode;
	decomp_offset = (unsigned) s7snap.decomp_offset;

   memcpy(decomp_buffer,s7snap.decomp_buffer,SPC7110_DECOMP_BUFFER_SIZE);

	decomp_buffer_rdoffset = (unsigned) s7snap.decomp_buffer_rdoffset;
	decomp_buffer_wroffset = (unsigned) s7snap.decomp_buffer_wroffset;
	decomp_buffer_length   = (unsigned) s7snap.decomp_buffer_length;

   memcpy(context,s7snap.context,sizeof(ContextState) * 32);
   
	s7_update_time(0);
}
