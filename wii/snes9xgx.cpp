/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007-July 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * snes9xgx.cpp
 *
 * This file controls overall program flow. Most things start and end here!
 ***************************************************************************/
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asndlib.h>

#include <ogc/machine/asm.h>
#include <ogc/lwp_heap.h>
#include <ogc/system.h>
#include <ogc/machine/processor.h>

#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <debug.h>
#include <sys/iosupport.h>

#include <di/di.h>

#include "snes9xgx.h"
#include "video.h"
#include "menu.h"
#include "sram.h"
#include "freeze.h"
#include "preferences.h"
#include "button_mapping.h"
#include "fileop.h"
#include "filebrowser.h"
#include "input.h"
#include "utils/FreeTypeGX.h"

#include "../src/cheats.h"
#include "../src/snes9x.h"
#include "../src/memmap.h"
#include "../src/apu/apu.h"
#include "../src/controls.h"
#include "../src/snapshot.h"
#include "../src/display.h"
#include "../src/language.h"
#include "../src/srtc.h"

#define MAX_MESSAGE_LEN (36 * 3)

int ConfigRequested = 0;
int ShutdownRequested = 0;
int ResetRequested = 0;
int ExitRequested = 0;
char appPath[1024] = { 0 };
char loadedFile[1024] = { 0 };

extern "C" {
extern void __exception_setreload(int t);
}

static heap_cntrl mem2_heap;

/*** Double buffered audio ***/
// Audio is fixed to 32Khz/16bit/Stereo
#define AUDIOBUFFER 2048
static short soundbuffer[2][AUDIOBUFFER] __attribute__ ((__aligned__ (32)));
static int whichab = 0;	/*** Audio buffer flip switch ***/

static lwpq_t audioqueue;
static mutex_t audiomutex = LWP_MUTEX_NULL;

/****************************************************************************
 * AUDIO
 ***************************************************************************/
/****************************************************************************
 * MixSamples
 * This continually calls S9xMixSamples On each DMA Completion
 ***************************************************************************/
static void MixSamples()
{

	whichab ^= 1;
	AUDIO_InitDMA ((u32) soundbuffer[whichab], AUDIOBUFFER);
	LWP_MutexLock(audiomutex);
	S9xMixSamples (soundbuffer[whichab], AUDIOBUFFER >> 1);
	LWP_MutexUnlock(audiomutex);
	DCFlushRange (soundbuffer[whichab], AUDIOBUFFER);
	LWP_ThreadSignal(audioqueue);
}

static void FinalizeSamplesCallback(void)
{ 
	LWP_MutexLock(audiomutex);
	S9xFinalizeSamples();
	LWP_MutexUnlock(audiomutex);
}

static void InitAudio ()
{
	AUDIO_Init(NULL);
	AUDIO_SetDSPSampleRate(AI_SAMPLERATE_32KHZ);
	AUDIO_RegisterDMACallback(MixSamples);
	LWP_InitQueue(&audioqueue);
	LWP_MutexInit(&audiomutex, false);
}

/****************************************************************************
 * SwitchAudioMode
 *
 * Switches between menu (no sound) and emulator (sound)
 ***************************************************************************/
static void SwitchAudioMode(int mode)
{
	if(mode == 0) // emulator
	{
		memset(soundbuffer[0],0,AUDIOBUFFER);
		memset(soundbuffer[1],0,AUDIOBUFFER);
		DCFlushRange(soundbuffer[0],AUDIOBUFFER);
		DCFlushRange(soundbuffer[1],AUDIOBUFFER);
		AUDIO_InitDMA((u32)soundbuffer[whichab],AUDIOBUFFER);
		AUDIO_StartDMA();

		S9xSetSamplesAvailableCallback(FinalizeSamplesCallback);
	}
	else // menu
	{
		S9xSetSamplesAvailableCallback(NULL);
		AUDIO_StopDMA();
	}
}

/****************************************************************************
 * AudioStart
 *
 * Called to kick off the Audio Queue
 ***************************************************************************/
static void AudioStart ()
{
	memset (soundbuffer[whichab], 0, AUDIOBUFFER);
	MixSamples();
}

/****************************************************************************
 * SNES9x implementation
 ***************************************************************************/
void S9xMessage(int /*type */, int /*number */, const char *message)
{
	static char buffer[MAX_MESSAGE_LEN + 1];
	snprintf(buffer, MAX_MESSAGE_LEN, "%s", message);
}

extern SCheatData Cheat;

/****************************************************************************
 * LoadCheatFile
 *
 * Loads cheat file from save buffer
 * Custom version of S9xLoadCheatFile()
 ***************************************************************************/

static bool LoadCheatFile (int length)
{
	uint8 data [28];
	int offset = 0;

	while (offset < length)
	{
		if(Cheat.num_cheats >= MAX_CHEATS || (length - offset) < 28)
			break;

		memcpy (data, savebuffer+offset, 28);
		offset += 28;

		Cheat.c [Cheat.num_cheats].enabled = 0; // cheats always off
		Cheat.c [Cheat.num_cheats].byte = data [1];
		Cheat.c [Cheat.num_cheats].address = data [2] | (data [3] << 8) | (data [4] << 16);
		Cheat.c [Cheat.num_cheats].saved_byte = data [5];
		Cheat.c [Cheat.num_cheats].saved = (data [0] & 8) != 0;
		memcpy (Cheat.c [Cheat.num_cheats].name, &data[8], 20);
		Cheat.c [Cheat.num_cheats].name[20] = 0;
		Cheat.num_cheats++;
	}
	return true;
}

/****************************************************************************
 * S9xCustomLoadCheatFile
 *
 * Erases any prexisting cheats, loads cheats from a cheat file
 * Called when a ROM is first loaded
 ***************************************************************************/
void S9xCustomLoadCheatFile(void)
{
	memset(Cheat.c, 0, sizeof(Cheat.c));
	Cheat.num_cheats = 0;

	char filepath[1024];
	int offset = 0;

	if(!MakeFilePath(filepath, FILE_CHEAT))
		return;

	AllocSaveBuffer();

	offset = LoadFile(filepath, SILENT);

	// load cheat file if present
	if(offset > 0)
		LoadCheatFile (offset);

	FreeSaveBuffer ();
}

/****************************************************************************
 * Load SRAM
 ***************************************************************************/
bool LoadSRAM (char * filepath, bool silent)
{
	int len = 0;
	int device;
	bool result = false;

	if(!FindDevice(filepath, &device))
		return 0;

	Memory.ClearSRAM();

	int size = Memory.SRAMSize ? (1 << (Memory.SRAMSize + 3)) * 128 : 0;

	if (size > 0x20000)
		size = 0x20000;

	if (size)
	{
		len = LoadFile((char *)Memory.SRAM, filepath, size, silent);

		if (len > 0)
		{
			if (len - size == 512)
				memmove(Memory.SRAM, Memory.SRAM + 512, size);

			if (Settings.SRTC || Settings.SPC7110RTC)
			{
				int pathlen = strlen(filepath);
				filepath[pathlen-3] = 'r';
				filepath[pathlen-2] = 't';
				filepath[pathlen-1] = 'c';
				LoadFile((char *)RTCData.reg, filepath, 20, silent);
			}
			result = true;
		}
		else if(!silent)
		{
			// if we reached here, nothing was done!
			ErrorPrompt("SRAM file not found");
		}
		S9xSoftReset();
	}
	return result;
}

bool LoadSRAMAuto (bool silent)
{
	char filepath[MAXPATHLEN];

	// look for Auto save file
	if(!MakeFilePath(filepath, FILE_SRAM, Memory.ROMFilename, 0))
		return false;

	if (LoadSRAM(filepath, silent))
		return true;

	// look for file with no number or Auto appended
	if(!MakeFilePath(filepath, FILE_SRAM, Memory.ROMFilename, -1))
		return false;

	if(LoadSRAM(filepath, silent))
		return true;

	return false;
}

bool8 CustomBSX_LoadBIOS(void)
{
	return false;
}

/****************************************************************************
 * Save SRAM
 ***************************************************************************/
bool SaveSRAM (char * filepath, bool silent)
{
	bool retval = false;
	int offset = 0;
	int device;

	if(!FindDevice(filepath, &device))
		return 0;

	if (Settings.SuperFX && Memory.ROMType < 0x15) // doesn't have SRAM
		return true;

	if (Settings.SA1 && Memory.ROMType == 0x34)    // doesn't have SRAM
		return true;

	// determine SRAM size
	int size = Memory.SRAMSize ? (1 << (Memory.SRAMSize + 3)) * 128 : 0;

	if (size > 0x20000)
		size = 0x20000;

	if (size > 0)
	{
		offset = SaveFile((char *)Memory.SRAM, filepath, size, silent);

		if (Settings.SRTC || Settings.SPC7110RTC)
		{
			int pathlen = strlen(filepath);
			filepath[pathlen-3] = 'r';
			filepath[pathlen-2] = 't';
			filepath[pathlen-1] = 'c';
			SaveFile((char *)RTCData.reg, filepath, 20, silent);
		}

		if (offset > 0)
		{
			if (!silent)
				InfoPrompt("Save successful");
			retval = true;
		}
	}
	else
	{
		if(!silent)
			ErrorPrompt("No SRAM data to save!");
	}
	return retval;
}

bool SaveSRAMAuto (bool silent)
{
	char filepath[1024];

	// look for file with no number or Auto appended
	if(!MakeFilePath(filepath, FILE_SRAM, Memory.ROMFilename, -1))
		return false;

	FILE * fp = fopen (filepath, "rb");

	if(fp) // file found
	{
		fclose (fp);
	}	
	else
	{
		if(!MakeFilePath(filepath, FILE_SRAM, Memory.ROMFilename, 0))
			return false;
	}

	return SaveSRAM(filepath, silent);
}

/****************************************************************************
 * DUMMY SNES9x functions
 ***************************************************************************/
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

const char * S9xBasename(const char *name)
{
	ExitApp();
	return name;
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

bool8 S9xOpenSnapshotFile(const char *filepath, bool8 readonly, STREAM *file)
{
	return FALSE;
}

void S9xCloseSnapshotFile(STREAM s)
{

}

/****************************************************************************
 * SaveSnapshot
 ***************************************************************************/
int SaveSnapshot (char * filepath, bool silent)
{
	int device;

	if(!FindDevice(filepath, &device))
		return 0;

	// save screenshot
	if(gameScreenPngSize > 0)
	{
		char screenpath[1024];
		strcpy(screenpath, filepath);
		screenpath[strlen(screenpath)-4] = 0;
		sprintf(screenpath, "%s.png", screenpath);
		SaveFile((char *)gameScreenPng, screenpath, gameScreenPngSize, silent);
	}

	STREAM fp = OPEN_STREAM(filepath, "wb");
	
	if(!fp)
	{
		if(!silent)
			ErrorPrompt("Save failed!");
		return 0;
	}

	S9xFreezeToStream(fp);
	CLOSE_STREAM(fp);

	if(!silent)
		InfoPrompt("Save successful");
	return 1;
}

int SaveSnapshotAuto (bool silent)
{
	char filepath[1024];

	if(!MakeFilePath(filepath, FILE_SNAPSHOT, Memory.ROMFilename, 0))
		return false;

	return SaveSnapshot(filepath, silent);
}

/****************************************************************************
 * LoadSnapshot
 ***************************************************************************/
int LoadSnapshot (char * filepath, bool silent)
{
	int device;
				
	if(!FindDevice(filepath, &device))
		return 0;

	STREAM fp = OPEN_STREAM(filepath, "rb");

	if(!fp)
	{
		if(!silent)
			ErrorPrompt("Unable to open snapshot!");
		return 0;
	}

	int	result = S9xUnfreezeFromStream(fp);
	CLOSE_STREAM(fp);

	if (result == SUCCESS)
		return 1;

	switch (result)
	{
		case WRONG_FORMAT:
			ErrorPrompt(SAVE_ERR_WRONG_FORMAT);
			break;

		case WRONG_VERSION:
			ErrorPrompt(SAVE_ERR_WRONG_VERSION);
			break;

		case SNAPSHOT_INCONSISTENT:
			ErrorPrompt(MOVIE_ERR_SNAPSHOT_INCONSISTENT);
			break;
	}
	return 0;
}

int LoadSnapshotAuto (bool silent)
{
	char filepath[1024];

	if(!MakeFilePath(filepath, FILE_SNAPSHOT, Memory.ROMFilename, 0))
		return false;

	return LoadSnapshot(filepath, silent);
}

/****************************************************************************
 * Shutdown / Reboot / Exit
 ***************************************************************************/

void ExitCleanup()
{
	AUDIO_StopDMA();
	StopGX();

	HaltDeviceThread();
	UnmountAllFAT();

	DI_Close();
}

void ExitApp()
{
	ShutoffRumble();

	SavePrefs(SILENT);

	if (SNESROMSize > 0 && !ConfigRequested && GCSettings.AutoSave == 1)
		SaveSRAMAuto(SILENT);

	ExitCleanup();

	if(ShutdownRequested)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);

	if(GCSettings.ExitAction == 0) // Auto
	{
		char * sig = (char *)0x80001804;
		if(
			sig[0] == 'S' &&
			sig[1] == 'T' &&
			sig[2] == 'U' &&
			sig[3] == 'B' &&
			sig[4] == 'H' &&
			sig[5] == 'A' &&
			sig[6] == 'X' &&
			sig[7] == 'X')
			GCSettings.ExitAction = 3; // Exit to HBC
		else
			GCSettings.ExitAction = 1; // HBC not found
	}

	if(GCSettings.ExitAction == 1) // Exit to Menu
	{
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	else if(GCSettings.ExitAction == 2) // Shutdown Wii
	{
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);
	}
	else // Exit to Loader
	{
		exit(0);
	}
}

void ShutdownCB()
{
	ShutdownRequested = 1;
}
void ResetCB()
{
	ResetRequested = 1;
}

/****************************************************************************
 * IOS Check
 ***************************************************************************/
bool SupportedIOS(u32 ios)
{
	if(ios == 58 || ios == 61)
		return true;

	return false;
}

bool SaneIOS(u32 ios)
{
	bool res = false;
	u32 num_titles=0;
	u32 tmd_size;

	if(ios > 200)
		return false;

	if (ES_GetNumTitles(&num_titles) < 0)
		return false;

	if(num_titles < 1) 
		return false;

	u64 *titles = (u64 *)memalign(32, num_titles * sizeof(u64) + 32);
	
	if(!titles)
		return false;

	if (ES_GetTitles(titles, num_titles) < 0)
	{
		free(titles);
		return false;
	}
	
	u32 *tmdbuffer = (u32 *)memalign(32, MAX_SIGNED_TMD_SIZE);

	if(!tmdbuffer)
	{
		free(titles);
		return false;
	}

	for(u32 n=0; n < num_titles; n++)
	{
		if((titles[n] & 0xFFFFFFFF) != ios) 
			continue;

		if (ES_GetStoredTMDSize(titles[n], &tmd_size) < 0)
			break;

		if (tmd_size > 4096)
			break;

		if (ES_GetStoredTMD(titles[n], (signed_blob *)tmdbuffer, tmd_size) < 0)
			break;

		if (tmdbuffer[1] || tmdbuffer[2])
		{
			res = true;
			break;
		}
	}
	free(tmdbuffer);
    free(titles);
	return res;
}

/****************************************************************************
 * MEM2 memory allocator
 ***************************************************************************/

static u32 InitMem2Manager(void) 
{
	int size = (12*1024*1024);
	u32 level;
	_CPU_ISR_Disable(level);
	size &= ~0x1f; // round down, because otherwise we may exceed the area
	void *mem2_heap_ptr = (void *)((u32)SYS_GetArena2Hi()-size);
	SYS_SetArena2Hi(mem2_heap_ptr);
	_CPU_ISR_Restore(level);
	size = __lwp_heap_init(&mem2_heap, mem2_heap_ptr, size, 32);
	return size;
}

static void * mem2_malloc(u32 size)
{
	return __lwp_heap_allocate(&mem2_heap, size);
}

int main(int argc, char *argv[])
{
	L2Enhance();
	
	u32 ios = IOS_GetVersion();

	if(!SupportedIOS(ios))
	{
		s32 preferred = IOS_GetPreferredVersion();

		if(SupportedIOS(preferred))
			IOS_ReloadIOS(preferred);
	}
	
	__exception_setreload(8);

	DI_Init();

	InitDeviceThread();
	InitGCVideo(); // Initialise video
	ResetVideo_Menu (); // change to menu video mode
	SetupPads();
	MountAllFAT(); // Initialize libFAT for SD and USB
	InitMem2Manager();

	// Wii Power/Reset buttons
	WPAD_SetPowerButtonCallback((WPADShutdownCallback)ShutdownCB);
	SYS_SetPowerCallback(ShutdownCB);
	SYS_SetResetCallback(ResetCB);

	// store path app was loaded from
	if(argc > 0 && argv[0] != NULL)
		CreateAppPath(argv[0]);

	DefaultSettings (); // Set defaults
	S9xUnmapAllControls ();
	SetDefaultButtonMap ();

	// Allocate SNES Memory
	if (!Memory.Init ())
		ExitApp();

	// Allocate APU
	if (!S9xInitAPU ())
		ExitApp();

	S9xInitSound (64, 0); // Initialise Sound System
	InitAudio();

	// Initialise Graphics
	setGFX ();
	if (!S9xGraphicsInit ())
		ExitApp();
	
	AllocGfxMem();
	FrameTimer = 0;
	InitFreeType((u8*)font_ttf, font_ttf_size); // Initialize font system
	savebuffer = (unsigned char *)mem2_malloc(SAVEBUFFERSIZE);
	browserList = (BROWSERENTRY *)mem2_malloc(sizeof(BROWSERENTRY)*MAX_BROWSER_SIZE);
	InitGUIThreads();
	VIDEO_WaitVSync ();	 /**< Wait for video vertical blank */

	do
	{
		// go back to checking if devices were inserted/removed
		// since we're entering the menu
		ResumeDeviceThread();

		SwitchAudioMode(1);

		if(SNESROMSize == 0)
			MainMenu(MENU_GAMESELECTION);
		else
			MainMenu(MENU_GAME);
		ConfigRequested = 0;
		SwitchAudioMode(0);

		#if 0
		Settings.MultiPlayer5Master = (GCSettings.Controller == CTRL_PAD4 ? true : false);
		Settings.SuperScopeMaster = (GCSettings.Controller == CTRL_SCOPE ? true : false);
		Settings.MouseMaster = (GCSettings.Controller == CTRL_MOUSE ? true : false);
		Settings.JustifierMaster = (GCSettings.Controller == CTRL_JUST ? true : false);
		#endif
		SetControllers ();

		// stop checking if devices were removed/inserted
		// since we're starting emulation again
		HaltDeviceThread();

		AudioStart ();

		FrameTimer = 0;

		CheckVideo = 2;	// force video update

		do
		{
			S9xMainLoop ();
			ReportButtons ();
			while(!S9xSyncSound())
				LWP_ThreadSleep(audioqueue);

			if(ResetRequested)
			{
				S9xSoftReset (); // reset game
				ResetRequested = 0;
			}
			if (ConfigRequested)
			{
				AUDIO_StopDMA();
				ConfigRequested = 0;
				ResetVideo_Menu();
				break;
			}
			if(ShutdownRequested)
				ExitApp();
		}while(1); // emulation loop
	}while(1); // main loop
}
