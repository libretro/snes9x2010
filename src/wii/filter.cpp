/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * Michniewski 2008
 *
 * HQ2x, HQ3x, HQ4x filters
 * (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)
 *
 * filter.cpp
 *
 * Adapted from Snes9x Win32/MacOSX ports
 * Video Filter Code (hq2x)
 ****************************************************************************/

#ifdef HW_RVL

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <malloc.h>

#include "filter.h"
#include "video.h"
#include "snes9xgx.h"
#include "menu.h"

#include "snes9x-next/memmap.h"

#define NUMBITS (16)

static int RGBtoYUV[1<<NUMBITS];
static uint16 RGBtoBright[1<<NUMBITS];

TFilterMethod FilterMethod = RenderPlain;

//
// Functions:
//

const char* GetFilterName (RenderFilter filterID)
{
	switch(filterID)
	{
		default: return "Unknown";
		case FILTER_NONE: return "None";
	}
}

// Return pointer to appropriate function
TFilterMethod
FilterToMethod (RenderFilter filterID)
{
	switch(filterID)
	{
        default:
        case FILTER_NONE:       return RenderPlain;
	}
}

void
SelectFilterMethod ()
{
	//InfoPrompt((char*)"Select Filter Method.");	// debug

	FilterMethod = FilterToMethod((RenderFilter)GCSettings.FilterMethod);
	//FilterMethodHiRes = FilterToMethod((RenderFilter)GCSettings.FilterMethodHiRes);

	// check whether or not we need filter memory (alloc or free it)
}

//
// Filter Codes:
//

// No enlargement, just render to the screen
void
RenderPlain (uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height)
{
	if (dstPtr == NULL)
	{
		ErrorPrompt((char*)"dstPtr is NULL. exiting!");
		exit(1);
	}
	//memcpy (dstPtr, srcPtr, width*height*srcPitch);
	return;
}

//
// HQ2X Filter Code:
//

#define	Mask_2	0x07E0	// 00000 111111 00000
#define	Mask13	0xF81F	// 11111 000000 11111

#define	Ymask	0xFF0000
#define	Umask	0x00FF00
#define	Vmask	0x0000FF
#define	trY		0x300000
#define	trU		0x000700
#define	trV		0x000006

#define Interp01(c1, c2) \
	((((c1) == (c2)) ? (c1) : \
	(((((((c1) & Mask_2) *  3) + ((c2) & Mask_2)) >> 2) & Mask_2) + \
	 ((((((c1) & Mask13) *  3) + ((c2) & Mask13)) >> 2) & Mask13))))

#define Interp02(c1, c2, c3) \
	((((((c1) & Mask_2) *  2 + ((c2) & Mask_2)     + ((c3) & Mask_2)    ) >> 2) & Mask_2) + \
	(((((c1) & Mask13) *  2 + ((c2) & Mask13)     + ((c3) & Mask13)    ) >> 2) & Mask13))

#define Interp06(c1, c2, c3) \
	((((((c1) & Mask_2) *  5 + ((c2) & Mask_2) * 2 + ((c3) & Mask_2)    ) >> 3) & Mask_2) + \
	(((((c1) & Mask13) *  5 + ((c2) & Mask13) * 2 + ((c3) & Mask13)    ) >> 3) & Mask13))

#define Interp07(c1, c2, c3) \
	((((((c1) & Mask_2) *  6 + ((c2) & Mask_2)     + ((c3) & Mask_2)    ) >> 3) & Mask_2) + \
	(((((c1) & Mask13) *  6 + ((c2) & Mask13)     + ((c3) & Mask13)    ) >> 3) & Mask13))

#define Interp09(c1, c2, c3) \
	((((((c1) & Mask_2) *  2 + ((c2) & Mask_2) * 3 + ((c3) & Mask_2) * 3) >> 3) & Mask_2) + \
	(((((c1) & Mask13) *  2 + ((c2) & Mask13) * 3 + ((c3) & Mask13) * 3) >> 3) & Mask13))

#define Interp10(c1, c2, c3) \
	((((((c1) & Mask_2) * 14 + ((c2) & Mask_2)     + ((c3) & Mask_2)    ) >> 4) & Mask_2) + \
	(((((c1) & Mask13) * 14 + ((c2) & Mask13)     + ((c3) & Mask13)    ) >> 4) & Mask13))

#define PIXEL00_0		*(dp) = w5
#define PIXEL00_10		*(dp) = Interp01(w5, w1)
#define PIXEL00_11		*(dp) = Interp01(w5, w4)
#define PIXEL00_12		*(dp) = Interp01(w5, w2)
#define PIXEL00_20		*(dp) = Interp02(w5, w4, w2)
#define PIXEL00_21		*(dp) = Interp02(w5, w1, w2)
#define PIXEL00_22		*(dp) = Interp02(w5, w1, w4)
#define PIXEL00_60		*(dp) = Interp06(w5, w2, w4)
#define PIXEL00_61		*(dp) = Interp06(w5, w4, w2)
#define PIXEL00_70		*(dp) = Interp07(w5, w4, w2)
#define PIXEL00_90		*(dp) = Interp09(w5, w4, w2)
#define PIXEL00_100		*(dp) = Interp10(w5, w4, w2)

#define PIXEL01_0		*(dp + 1) = w5
#define PIXEL01_10		*(dp + 1) = Interp01(w5, w3)
#define PIXEL01_11		*(dp + 1) = Interp01(w5, w2)
#define PIXEL01_12		*(dp + 1) = Interp01(w5, w6)
#define PIXEL01_20		*(dp + 1) = Interp02(w5, w2, w6)
#define PIXEL01_21		*(dp + 1) = Interp02(w5, w3, w6)
#define PIXEL01_22		*(dp + 1) = Interp02(w5, w3, w2)
#define PIXEL01_60		*(dp + 1) = Interp06(w5, w6, w2)
#define PIXEL01_61		*(dp + 1) = Interp06(w5, w2, w6)
#define PIXEL01_70		*(dp + 1) = Interp07(w5, w2, w6)
#define PIXEL01_90		*(dp + 1) = Interp09(w5, w2, w6)
#define PIXEL01_100		*(dp + 1) = Interp10(w5, w2, w6)

#define PIXEL10_0		*(dp + dst1line) = w5
#define PIXEL10_10		*(dp + dst1line) = Interp01(w5, w7)
#define PIXEL10_11		*(dp + dst1line) = Interp01(w5, w8)
#define PIXEL10_12		*(dp + dst1line) = Interp01(w5, w4)
#define PIXEL10_20		*(dp + dst1line) = Interp02(w5, w8, w4)
#define PIXEL10_21		*(dp + dst1line) = Interp02(w5, w7, w4)
#define PIXEL10_22		*(dp + dst1line) = Interp02(w5, w7, w8)
#define PIXEL10_60		*(dp + dst1line) = Interp06(w5, w4, w8)
#define PIXEL10_61		*(dp + dst1line) = Interp06(w5, w8, w4)
#define PIXEL10_70		*(dp + dst1line) = Interp07(w5, w8, w4)
#define PIXEL10_90		*(dp + dst1line) = Interp09(w5, w8, w4)
#define PIXEL10_100		*(dp + dst1line) = Interp10(w5, w8, w4)

#define PIXEL11_0		*(dp + dst1line + 1) = w5
#define PIXEL11_10		*(dp + dst1line + 1) = Interp01(w5, w9)
#define PIXEL11_11		*(dp + dst1line + 1) = Interp01(w5, w6)
#define PIXEL11_12		*(dp + dst1line + 1) = Interp01(w5, w8)
#define PIXEL11_20		*(dp + dst1line + 1) = Interp02(w5, w6, w8)
#define PIXEL11_21		*(dp + dst1line + 1) = Interp02(w5, w9, w8)
#define PIXEL11_22		*(dp + dst1line + 1) = Interp02(w5, w9, w6)
#define PIXEL11_60		*(dp + dst1line + 1) = Interp06(w5, w8, w6)
#define PIXEL11_61		*(dp + dst1line + 1) = Interp06(w5, w6, w8)
#define PIXEL11_70		*(dp + dst1line + 1) = Interp07(w5, w6, w8)
#define PIXEL11_90		*(dp + dst1line + 1) = Interp09(w5, w6, w8)
#define PIXEL11_100		*(dp + dst1line + 1) = Interp10(w5, w6, w8)

#define Absolute(c) \
(!(c & (1 << 31)) ? c : (~c + 1))

static inline bool Diff(int c1, int c2)
{
   int c1y = (c1 & Ymask) - (c2 & Ymask);
   if (Absolute(c1y) > trY) return true;
   int c1u = (c1 & Umask) - (c2 & Umask);
   if (Absolute(c1u) > trU) return true;
   int c1v = (c1 & Vmask) - (c2 & Vmask);
   if (Absolute(c1v) > trV) return true;

   return false;
}

void InitLUTs(void)
{
	int	c, r, g, b, y, u, v;

	for (c = 0 ; c < (1<<NUMBITS) ; c++)
  	{
//#ifdef R5G6B5
		b = (int)((c & 0x1F)) << 3;
		g = (int)((c & 0x7E0)) >> 3;
		r = (int)((c & 0xF800)) >> 8;

//#else
//		b = (int)((c & 0x1F)) << 3;
//		g = (int)((c & 0x3E0)) >> 2;
//		r = (int)((c & 0x7C00)) >> 7;
//#endif

		RGBtoBright[c] = r+r+r + g+g+g + b+b;

		y = (int)( 0.256788f*r + 0.504129f*g + 0.097906f*b + 0.5f) + 16;
		u = (int)(-0.148223f*r - 0.290993f*g + 0.439216f*b + 0.5f) + 128;
		v = (int)( 0.439216f*r - 0.367788f*g - 0.071427f*b + 0.5f) + 128;

		RGBtoYUV[c] = (y << 16) + (u << 8) + v;

	}
}

#endif
