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

void S9xAutoSaveSRAM()
{

}

/****************************************************************************
 * OpenSoundDevice
 *
 * Main initialisation for Wii sound system
 ***************************************************************************/
bool8 S9xOpenSoundDevice(void)
{
	InitAudio();
	return TRUE;
}

/* eke-eke */
void S9xInitSync()
{
	FrameTimer = 0;
	prev = gettime();
}

/*** Synchronisation ***/

void S9xSyncSpeed ()
{
}

/*** Video / Display related functions ***/
bool8 S9xInitUpdate()
{
	return (TRUE);
}

void S9xDeinitUpdate(int Width, int Height)
{
	update_video(Width, Height);
}

bool8 S9xContinueUpdate(int Width, int Height)
{
	return (TRUE);
}

void S9xSetPalette()
{
	return;
}

/*** Input functions ***/
void S9xHandlePortCommand(s9xcommand_t cmd, int16 data1, int16 data2)
{
	return;
}

bool S9xPollButton(uint32 id, bool * pressed)
{
	return 0;
}

bool S9xPollAxis(uint32 id, int16 * value)
{
	return 0;
}

bool S9xPollPointer(uint32 id, int16 * x, int16 * y)
{
	return 0;
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

const char * S9xChooseMovieFilename(bool8 read_only)
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

const char * S9xGetFilenameInc(const char *e, enum s9x_getdirtype dirtype)
{
	ExitApp();
	return NULL;
}

const char * S9xBasename(const char *name)
{
	ExitApp();
	return name;
}

const char * S9xStringInput (const char * s)
{
	ExitApp();
	return s;
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
