/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * s9xsupport.cpp
 *
 * Snes9x support functions
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ogc/lwp_watchdog.h>

#include "snes9xgx.h"
#include "video.h"
#include "audio.h"
#include "snes9x-next/snes9x.h"
#include "snes9x-next/memmap.h"
#include "snes9x-next/display.h"
#include "snes9x-next/apu/apu.h"
#include "snes9x-next/controls.h"

#define MAX_MESSAGE_LEN (36 * 3)

static long long prev;
static long long now;

/*** Miscellaneous Functions ***/
void S9xExit()
{
	ExitApp();
}

void S9xMessage(int /*type */, int /*number */, const char *message)
{
	static char buffer[MAX_MESSAGE_LEN + 1];
	snprintf(buffer, MAX_MESSAGE_LEN, "%s", message);
	//S9xSetInfoString(buffer);
}

/****************************************************************************
 * OpenSoundDevice
 *
 * Main initialisation for Wii sound system
 ***************************************************************************/
/*** Video / Display related functions ***/
void S9xDeinitUpdate(int Width, int Height)
{
	update_video(Width, Height);
}

/****************************************************************************
 * Note that these are DUMMY functions, and only allow Snes9x to
 * compile. Where possible, they will return an error signal.
 ***************************************************************************/

const char *S9xChooseFilename(bool8 read_only)
{
	ExitApp();
	return NULL;
}

const char * S9xGetDirectory(enum s9x_getdirtype dirtype)
{
	ExitApp();
	return NULL;
}

const char * S9xGetFilename(const char *ex, enum s9x_getdirtype dirtype)
{
	ExitApp();
	return NULL;
}

void _splitpath(char const *buf, char *drive, char *dir, char *fname, char *ext)
{
	ExitApp();
}

void _makepath(char *filename, const char *drive, const char *dir,
		const char *fname, const char *ext)
{
	ExitApp();
}

int dup(int fildes)
{
	ExitApp();
	return 1;
}

int access(const char *pathname, int mode)
{
	ExitApp();
	return 1;
}
