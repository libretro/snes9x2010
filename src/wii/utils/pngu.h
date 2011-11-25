/********************************************************************************************
*
* PNGU
* 
* Original author: frontier (http://frontier-dev.net)
* Modified by Tantric, 2009-2010
*
********************************************************************************************/

#ifndef __PNGU__
#define __PNGU__

#include <gccore.h>

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct
{
	u8 r;
	u8 g;
	u8 b;
} PNGUCOLOR;

typedef struct
{
	u32 imgWidth; // In pixels
	u32 imgHeight; // In pixels
	u32 imgBitDepth; // In bitx
	u32 imgColorType; // PNGU_COLOR_TYPE_*
	u32 validBckgrnd; // Non zero if there is a background color
	PNGUCOLOR bckgrnd; // Background color
	u32 numTrans; // Number of transparent colors
	PNGUCOLOR *trans; // Transparent colors
} PNGUPROP;

// Image context, always initialize with SelectImageFrom* and free with ReleaseImageContext
struct _IMGCTX;
typedef struct _IMGCTX *IMGCTX; 

/****************************************************************************
* Image conversion								                            *
****************************************************************************/

u8 * DecodePNG(const u8 *src, int *width, int *height, u8 *dst, int maxwidth, int maxheight);

#ifdef __cplusplus
	}
#endif

#endif
