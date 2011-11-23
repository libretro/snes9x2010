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

// Return codes
#define PNGU_OK							0

#include <gccore.h>

#ifdef __cplusplus
	extern "C" {
#endif

// Types
typedef unsigned char PNGU_u8;
typedef unsigned short PNGU_u16;
typedef unsigned int PNGU_u32;
typedef unsigned long long PNGU_u64;

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
* Image context handling							                        *
****************************************************************************/

// Selects a PNG file, previously loaded into a buffer, and creates an image context for subsequent processing.
IMGCTX PNGU_SelectImageFromBuffer (const void *buffer);

// Selects a PNG file, from any devoptab device, and creates an image context for subsequent processing.
IMGCTX PNGU_SelectImageFromDevice (const char *filename);

// Frees resources associated with an image context. Always call this function when you no longer need the IMGCTX.
void PNGU_ReleaseImageContext (IMGCTX ctx);

/****************************************************************************
* Miscellaneous								                             	*
****************************************************************************/

// Retrieves info from selected PNG file, including image dimensions, color format, background and transparency colors.
int PNGU_GetImageProperties (IMGCTX ctx, PNGUPROP *fileproperties);

/****************************************************************************
* Image conversion								                            *
****************************************************************************/

u8 * DecodePNG(const u8 *src, int *width, int *height, u8 *dst, int maxwidth, int maxheight);
int PNGU_EncodeFromRGB (IMGCTX ctx, u32 width, u32 height, void *buffer, u32 stride);
int PNGU_EncodeFromGXTexture (IMGCTX ctx, u32 width, u32 height, void *buffer, u32 stride);
int PNGU_EncodeFromEFB (IMGCTX ctx, u32 width, u32 height);
// Expands selected image into a 4x4 tiled RGBA8 buffer. You need to specify context, image dimensions,
// destination address and default alpha value, which is used if the source image doesn't have an alpha channel.
int PNGU_DecodeTo4x4RGBA8Cover (IMGCTX ctx, PNGU_u32 width, PNGU_u32 height, void *buffer, PNGU_u8 default_alpha);


#ifdef __cplusplus
	}
#endif

#endif
