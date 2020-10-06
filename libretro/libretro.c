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

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#ifndef _MSC_VER
#include <stdbool.h>
#include <unistd.h>
#endif

#include <libretro.h>
#include "libretro_core_options.h"

#ifdef _3DS
void* linearMemAlign(size_t size, size_t alignment);
void linearFree(void* mem);
#endif

#include "../src/apu.h"
#include "../src/boolean.h"
#include "../src/cheats.h"
#include "../src/controls.h"
#include "../src/cpuexec.h"
#include "../src/display.h"
#include "../src/fxemu.h"
#include "../src/memmap.h"
#include "../src/messages.h"
#include "../src/ppu.h"
#include "../src/snapshot.h"
#include "../src/snes9x.h"
#include "../src/srtc.h"
#include "../filter/snes_ntsc.h"

#define LR_MAP_BUTTON(id, name)		S9xMapButton((id), S9xGetCommandT((name)))
#define MAKE_BUTTON(pad, btn)			(((pad) << 4) | (btn))

#define BTN_POINTER		(RETRO_DEVICE_ID_JOYPAD_R + 1)
#define BTN_POINTER2		(BTN_POINTER + 1)

#define RETRO_DEVICE_JOYPAD_MULTITAP		RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)
#define RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE		RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)
#define RETRO_DEVICE_LIGHTGUN_JUSTIFIER		RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 1)
#define RETRO_DEVICE_LIGHTGUN_JUSTIFIERS		RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 2)

#define CORE_VERSION	"1.52.4"
#define LIBRETRO_LIB_NAME "Snes9x 2010"

/* Use a 64ms buffer. */
/* 32000*(64/1000) = 2048
 * 2048 * 2 = 4096 because of stereo. */
#define APU_BUF_FRAMES		(2048 * 2)
#define APU_BUF_SZ		(APU_BUF_FRAMES * sizeof(int16_t))

enum
{
	ASPECT_RATIO_4_3,
	ASPECT_RATIO_1_1,
	ASPECT_RATIO_NTSC,
	ASPECT_RATIO_PAL,
	ASPECT_RATIO_AUTO
};

static retro_log_printf_t		log_cb = NULL;
static retro_video_refresh_t		video_cb = NULL;
static retro_input_poll_t			poll_cb = NULL;
static retro_input_state_t		input_cb = NULL;
static retro_audio_sample_batch_t			audio_batch_cb = NULL;
static retro_environment_t		environ_cb = NULL;

extern s9xcommand_t keymap[1024];
bool overclock_cycles = false;
bool reduce_sprite_flicker = false;
extern uint16_t joypad[8];
int one_c, slow_one_c, two_c;

static struct
{
	int16_t retro_mouse_state[2][2];
	int16_t retro_justifier_state[2][2];
	int16_t retro_scope_state[2];
	uint8_t turbo_state[5];
	uint8_t turbo_delay;
	uint8_t turbo_enable;
	bool switch_state;
} input_vars;

static uint8_t aspect_ratio_mode = ASPECT_RATIO_4_3;
static bool libretro_supports_bitmasks = false;
static uint32_t retro_devices[2];

// filter
static int snes_ntsc_filter = 0;
static snes_ntsc_t snes_ntsc;
static snes_ntsc_setup_t ntsc_setup;
static uint16_t *ntsc_screen_buffer = NULL;
static const uint32_t MAX_SNES_WIDTH_NTSC = ((SNES_NTSC_OUT_WIDTH(256) + 3) / 4) * 4;

static void update_geometry();


static void check_variables(void)
{
	bool reset_sfx = false;
	int old_filter = snes_ntsc_filter;
	struct retro_variable var;

	var.key = "snes9x_2010_region";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		if (strcmp(var.value, "auto") == 0)
		{
			Settings.ForceNTSC = false;
			Settings.ForcePAL = false;
		}
		else if (strcmp(var.value, "ntsc") == 0)
		{
			Settings.ForceNTSC = true;
			Settings.ForcePAL = false;
			Settings.PAL = FALSE;
		}
		else if (strcmp(var.value, "pal") == 0)
		{
			Settings.ForceNTSC = false;
			Settings.ForcePAL = true;
			Settings.PAL = TRUE;
		}
	}

	var.key = "snes9x_2010_aspect";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		uint8_t newval = ASPECT_RATIO_AUTO;

		if (strcmp(var.value, "ntsc") == 0)
			newval = ASPECT_RATIO_NTSC;
		else if (strcmp(var.value, "pal") == 0)
			newval = ASPECT_RATIO_PAL;
		else if (strcmp(var.value, "4:3") == 0)
			newval = ASPECT_RATIO_4_3;
		else if (strcmp(var.value, "uncorrected") == 0)
			newval = ASPECT_RATIO_1_1;

		if (newval != aspect_ratio_mode)
			aspect_ratio_mode = newval;
	}

	var.key = "snes9x_2010_overclock";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		char *endptr;
		double freq = strtod(var.value, &endptr);

		/* There must be a space between the value and the unit. Therefore, we
		 * check that the character after the converter integer is a space. */
		if (*endptr != ' ' || freq == 0.0)
		{
			S9xMessage(S9X_MSG_WARN, S9X_CATEGORY_EXTERNAL, "Unable to obtain SuperFX overclock setting.");
			freq = 10.0;
		}

		/* Convert MHz value to Hz and multiply by required factors. */
		Settings.SuperFXSpeedPerLine = 0.417 * 1.5e6 * freq;
		reset_sfx = true;
	}

   	var.key = "snes9x_2010_turbodelay";
   	var.value = NULL;
   	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   	{
      		if (strcmp(var.value, "medium") == 0)
        		input_vars.turbo_delay = 5;
      		else if (strcmp(var.value, "slow") == 0)
        		input_vars.turbo_delay = 7;
      		else
        		input_vars.turbo_delay = 3;
   	}

	var.key = "snes9x_2010_overclock_cycles";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		if (strcmp(var.value, "compatible") == 0)
		{
			overclock_cycles = true;
			one_c = 4;
			slow_one_c = 5;
			two_c = 6;
		}
		else if (strcmp(var.value, "max") == 0)
		{
			overclock_cycles = true;
			one_c = 3;
			slow_one_c = 3;
			two_c = 3;
		}
		else if (strcmp(var.value, "light") == 0)
		{
			overclock_cycles = true;
			one_c = 6;
			slow_one_c = 6;
			two_c = 12;
		}
		else
			overclock_cycles = false;
	}

	var.key = "snes9x_2010_blargg";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		if (strcmp(var.value, "disabled") == 0)
			snes_ntsc_filter = 0;
		else
		{
			if (strcmp(var.value, "monochrome") == 0)
			{
				snes_ntsc_filter = 1;
				ntsc_setup = snes_ntsc_monochrome;
			}
			else if (strcmp(var.value, "rf") == 0)
			{
				snes_ntsc_filter = 2;
				ntsc_setup = snes_ntsc_composite;
				ntsc_setup.merge_fields = 0;
			}
			else if (strcmp(var.value, "composite") == 0)
			{
				snes_ntsc_filter = 3;
				ntsc_setup = snes_ntsc_composite;
			}
			else if (strcmp(var.value, "s-video") == 0)
			{
				snes_ntsc_filter = 4;
				ntsc_setup = snes_ntsc_svideo;
			}
			else if (strcmp(var.value, "rgb") == 0)
			{
				snes_ntsc_filter = 5;
				ntsc_setup = snes_ntsc_rgb;
			}
		}
	}

	var.key = "snes9x_2010_reduce_sprite_flicker";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		if (strcmp(var.value, "enabled") == 0)
			reduce_sprite_flicker = true;
		else
			reduce_sprite_flicker = false;
	}
//
	if (old_filter != snes_ntsc_filter)
		snes_ntsc_init(&snes_ntsc, &ntsc_setup);

	if (reset_sfx)
		S9xResetSuperFX();
}

void *retro_get_memory_data(unsigned type)
{
	uint8_t *data;

	switch (type)
	{
		case RETRO_MEMORY_SAVE_RAM:
			data = Memory.SRAM;
		break;
		case RETRO_MEMORY_RTC:
			data = RTCData.reg;
		break;
		case RETRO_MEMORY_SYSTEM_RAM:
			data = Memory.RAM;
		break;
		case RETRO_MEMORY_VIDEO_RAM:
			data = Memory.VRAM;
		break;
/*		case RETRO_MEMORY_ROM:
			data = Memory.ROM;
		break;		*/
		default:
			data = NULL;
		break;
	}

	return data;
}

size_t retro_get_memory_size(unsigned type)
{
	unsigned size;

	switch (type)
	{
		case RETRO_MEMORY_SAVE_RAM:
			size = (unsigned) (Memory.SRAMSize ? (1 << (Memory.SRAMSize + 3)) * 128 : 0);
			if (size > 0x20000)
				size = 0x20000;
		break;
		case RETRO_MEMORY_RTC:
			size = (Settings.SRTC || Settings.SPC7110RTC) ? 20 : 0;
		break;
		case RETRO_MEMORY_SYSTEM_RAM:
			size = 128 * 1024;
		break;
		case RETRO_MEMORY_VIDEO_RAM:
			size = 64 * 1024;
		break;
/*		case RETRO_MEMORY_ROM:
			data = Memory.CalculatedSize;
		break;		*/
		default:
			size = 0;
		break;
	}

	return size;
}

unsigned retro_api_version(void) { return RETRO_API_VERSION; }

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { (void) cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_cb = cb; }

void retro_set_environment(retro_environment_t cb)
{
	static const struct retro_controller_description port_1[] = {
		{ "SNES Joypad", RETRO_DEVICE_JOYPAD },
		{ "SNES Mouse", RETRO_DEVICE_MOUSE },
		{ "RetroPad", RETRO_DEVICE_JOYPAD },
	};

	static const struct retro_controller_description port_2[] = {
		{ "SNES Joypad", RETRO_DEVICE_JOYPAD },
		{ "SNES Mouse", RETRO_DEVICE_MOUSE },
		{ "Multitap", RETRO_DEVICE_JOYPAD_MULTITAP },
		{ "SuperScope", RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE },
		{ "Justifier", RETRO_DEVICE_LIGHTGUN_JUSTIFIER },
		{ "Justifiers", RETRO_DEVICE_LIGHTGUN_JUSTIFIERS },
		{ "RetroPad", RETRO_DEVICE_JOYPAD },
	};

	static const struct retro_controller_info ports[] = {
		{ port_1, 3 },
		{ port_2, 7 },
		{ 0, 0 }
	};

	environ_cb = cb;

	libretro_set_core_options(environ_cb);
	environ_cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_get_system_info(struct retro_system_info *info)
{
	info->need_fullpath    = false;
	info->valid_extensions = "smc|fig|sfc|gd3|gd7|dx2|bsx|swc";
#ifdef GIT_VERSION
	info->library_version = CORE_VERSION GIT_VERSION;
#else
	info->library_version  = CORE_VERSION;
#endif
	info->library_name     = LIBRETRO_LIB_NAME;
	info->block_extract    = false;
}

static void S9xAudioCallbackQueue(void)
{
	size_t avail;
	/* Just pick a big buffer. We won't use it all. */
	static int16_t audio_buf[APU_BUF_SZ];

	S9xFinalizeSamples();
	avail = S9xGetSampleCount();

	if (avail > APU_BUF_FRAMES)
		avail = APU_BUF_FRAMES;

	S9xMixSamples(audio_buf, avail);
	audio_batch_cb(audio_buf, avail >> 1);
}

void retro_set_controller_port_device(unsigned in_port, unsigned device)
{
	int port = in_port;

	if (port >= 2)
		return;

	switch (device)
	{
		case RETRO_DEVICE_JOYPAD:
			retro_devices[port] = RETRO_DEVICE_JOYPAD;
			S9xSetController(port, CTL_JOYPAD, port, 0, 0, 0);
		break;
		case RETRO_DEVICE_JOYPAD_MULTITAP:
			retro_devices[port] = RETRO_DEVICE_JOYPAD_MULTITAP;
			S9xSetController(port, CTL_MP5, port+0, port+2, port+4, port+6);
		break;
		case RETRO_DEVICE_MOUSE:
			retro_devices[port] = RETRO_DEVICE_MOUSE;
			S9xSetController(port, CTL_MOUSE, port, 0, 0, 0);
			/* mapping pointers here */
			S9xMapPointer((BTN_POINTER), S9xGetCommandT("Pointer Mouse1+Superscope+Justifier1"));
			S9xMapPointer((BTN_POINTER2), S9xGetCommandT("Pointer Mouse2"));
			/* mapping extra buttons here */
			LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_SELECT), "Mouse1 L");
			LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_START), "Mouse1 R");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Mouse2 L");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Mouse2 R");
		break;
		case RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE:
			S9xSetController(port, CTL_SUPERSCOPE, 0, 0, 0, 0);
			retro_devices[port] = RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE;
			/* mapping pointers here */
			S9xMapPointer((BTN_POINTER), S9xGetCommandT("Pointer Mouse1+Superscope+Justifier1"));
			S9xMapPointer((BTN_POINTER2), S9xGetCommandT("Pointer Mouse2"));
			/* mapping extra buttons here */
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Superscope Fire");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Superscope Cursor");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_UP), "Superscope ToggleTurbo");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_DOWN), "Superscope Pause");
		break;
		case RETRO_DEVICE_LIGHTGUN_JUSTIFIER:
			S9xSetController(port, CTL_JUSTIFIER, 0, 0, 0, 0);
			retro_devices[port] = RETRO_DEVICE_LIGHTGUN_JUSTIFIER;
			/* mapping extra buttons here */
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Justifier1 Trigger");
			LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Justifier1 Start");
		break;
		case RETRO_DEVICE_LIGHTGUN_JUSTIFIERS:
			S9xSetController(port, CTL_JUSTIFIER, 1, 0, 0, 0);
			retro_devices[port] = RETRO_DEVICE_LIGHTGUN_JUSTIFIERS;
		break;
		default:
			S9xMessage(S9X_MSG_ERROR, S9X_CATEGORY_EXTERNAL, "Invalid device!");
		break;
	}

	if (((retro_devices[0] == RETRO_DEVICE_JOYPAD) && retro_devices[1] == RETRO_DEVICE_JOYPAD) ||
		((retro_devices[0] == RETRO_DEVICE_JOYPAD) && retro_devices[1] == RETRO_DEVICE_JOYPAD_MULTITAP) ||
			((retro_devices[0] == RETRO_DEVICE_JOYPAD_MULTITAP) && retro_devices[1] == RETRO_DEVICE_JOYPAD))
		Settings.NormalControls = 1;
}

static void map_buttons(void)
{
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_A), "Joypad1 A");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_B), "Joypad1 B");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_X), "Joypad1 X");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad1 Y");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad1 Select");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_START), "Joypad1 Start");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_L), "Joypad1 L");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_R), "Joypad1 R");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad1 Left");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad1 Right");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad1 Up");
	LR_MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad1 Down");

	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_A), "Joypad2 A");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_B), "Joypad2 B");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_X), "Joypad2 X");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad2 Y");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad2 Select");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Joypad2 Start");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_L), "Joypad2 L");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_R), "Joypad2 R");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad2 Left");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad2 Right");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad2 Up");
	LR_MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad2 Down");

	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_A), "Joypad3 A");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_B), "Joypad3 B");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_X), "Joypad3 X");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad3 Y");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad3 Select");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_START), "Joypad3 Start");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_L), "Joypad3 L");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_R), "Joypad3 R");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad3 Left");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad3 Right");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad3 Up");
	LR_MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad3 Down");

	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_A), "Joypad4 A");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_B), "Joypad4 B");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_X), "Joypad4 X");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad4 Y");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad4 Select");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_START), "Joypad4 Start");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_L), "Joypad4 L");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_R), "Joypad4 R");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad4 Left");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad4 Right");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad4 Up");
	LR_MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad4 Down");

	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_A), "Joypad5 A");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_B), "Joypad5 B");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_X), "Joypad5 X");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad5 Y");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad5 Select");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_START), "Joypad5 Start");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_L), "Joypad5 L");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_R), "Joypad5 R");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad5 Left");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad5 Right");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad5 Up");
	LR_MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad5 Down");
}

float get_aspect_ratio(unsigned width, unsigned height)
{
	double sample_frequency_ntsc;
	double sample_frequency_pal;
	double sample_freq;
	double dot_rate;
    double par;

	if (aspect_ratio_mode == ASPECT_RATIO_4_3)
		return (4.0f / 3.0f);
	else if (aspect_ratio_mode == ASPECT_RATIO_1_1)
		return (float)width / (float)height;

	// OV2: not sure if these really make sense - NTSC is similar to 4:3, PAL looks weird
	sample_frequency_ntsc = 135000000.0f / 11.0f;
	sample_frequency_pal = 14750000.0;
	sample_freq = (retro_get_region() == RETRO_REGION_NTSC) ? sample_frequency_ntsc : sample_frequency_pal;
	dot_rate = (Settings.PAL ? PAL_MASTER_CLOCK : NTSC_MASTER_CLOCK) / 4.0;

	if (aspect_ratio_mode == ASPECT_RATIO_NTSC) // ntsc
	{
		sample_freq = sample_frequency_ntsc;
		dot_rate = NTSC_MASTER_CLOCK / 4.0;
	}
	else if (aspect_ratio_mode == ASPECT_RATIO_PAL) // pal
	{
		sample_freq = sample_frequency_pal;
		dot_rate = PAL_MASTER_CLOCK / 4.0;
	}

	par = sample_freq / 2.0 / dot_rate;

	return (float)(width * par / height);
}

static void update_geometry(void)
{
	struct retro_system_av_info av_info;
	retro_get_system_av_info(&av_info);
	environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &av_info);
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	unsigned width = SNES_WIDTH;
	unsigned height = PPU.ScreenHeight;

	info->geometry.base_width = width;
	info->geometry.base_height = height;
	info->geometry.max_width = MAX_SNES_WIDTH_NTSC;
	info->geometry.max_height = MAX_SNES_HEIGHT;

	info->geometry.aspect_ratio = get_aspect_ratio(width, height);
	info->timing.sample_rate = SNES_AUDIO_FREQ;
	info->timing.fps = (retro_get_region() == RETRO_REGION_NTSC) ? (21477272.0 / 357366.0) : (21281370.0 / 425568.0);
}

static void set_system_specs(void)
{
	unsigned level = 7;
	environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);
}

void retro_init(void)
{
	struct retro_log_callback log;
	struct retro_variable var;
	enum retro_pixel_format rgb565;
	bool achievements = true;

	if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
		log_cb = log.log;
	else
		log_cb = NULL;

	/* State that the core supports achievements. */
	environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS, &achievements);

	rgb565 = RETRO_PIXEL_FORMAT_RGB565;
	if (environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565))
		S9xMessage(S9X_MSG_INFO, S9X_CATEGORY_EXTERNAL, "Frontend supports RGB565 - will use that instead of XRGB1555.");

	if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
		libretro_supports_bitmasks = true;

	memset(&input_vars, 0, sizeof(input_vars));
	memset(&Settings, 0, sizeof(Settings));

	Settings.SpeedhackGameID = SPEEDHACK_NONE;
	Settings.Transparency = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.SoundPlaybackRate = SNES_AUDIO_FREQ;
	Settings.SoundInputRate = SNES_AUDIO_FREQ;
	Settings.HDMATimingHack = 100;
	Settings.CartAName[0] = 0;
	Settings.CartBName[0] = 0;
	Settings.Crosshair = 1;
	Settings.BlockInvalidVRAMAccessMaster = TRUE;

	var.key = "snes9x_2010_block_invalid_vram_access";
	var.value = NULL;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		Settings.BlockInvalidVRAMAccessMaster = (strcmp(var.value, "disabled") == 0) ? FALSE : TRUE;

	CPU.Flags = 0;

	if (!Init() || !S9xInitAPU())
	{
		Deinit();
		S9xDeinitAPU();
		S9xMessage(S9X_MSG_ERROR, S9X_CATEGORY_EXTERNAL, "Failed to init Memory or APU.");
		exit(1);
	}

	if (S9xInitSound(APU_BUF_SZ, 0) != true)
	{
		const char *const aud_err = "Audio output is disabled due to an internal error";
		struct retro_message msg = { aud_err, 360 };

		if (environ_cb)
			environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);

		S9xDeinitAPU();
	}

	S9xSetSamplesAvailableCallback(S9xAudioCallbackQueue);

	GFX.Pitch = MAX_SNES_WIDTH_NTSC * sizeof(uint16_t);

#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L) && !defined(GEKKO) && !defined(_3DS) && !defined(__SWITCH__)
	/* request 128-bit alignment here if possible */
	posix_memalign((void**)&GFX.Screen, 16, GFX.Pitch * 512 * sizeof(uint16));
	posix_memalign( (void**)&ntsc_screen_buffer, 16, GFX.Pitch * MAX_SNES_HEIGHT * sizeof(uint16_t) );
#elif defined(_3DS)
	GFX.Screen = (uint16*) linearMemAlign(GFX.Pitch * 512 * sizeof(uint16), 0x80);
	ntsc_screen_buffer = (uint16_t*)linearMemAlign(GFX.Pitch *  MAX_SNES_HEIGHT * sizeof(uint16_t), 0x80);
#else
	GFX.Screen = (uint16*) calloc(1, GFX.Pitch * 512 * sizeof(uint16));
	ntsc_screen_buffer = (uint16_t *)calloc(1, GFX.Pitch * MAX_SNES_HEIGHT * sizeof(uint16));
#endif
	S9xGraphicsInit();

	retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
	retro_set_controller_port_device(1, RETRO_DEVICE_JOYPAD);

	S9xUnmapAllControls();
	map_buttons();

	/* Initialize SuperFX CPU to normal speed by default */
	Settings.SuperFXSpeedPerLine = 0.417 * 10.5e6;
}

/* libretro uses relative values for analogue devices. 
   S9x seems to use absolute values, but do convert these into relative values in the core. (Why?!)
   Hack around it. :) */

void retro_deinit(void)
{
	S9xDeinitAPU();
	Deinit();
	S9xGraphicsDeinit();
	S9xUnmapAllControls();
#if defined(_3DS)
	if (GFX.Screen)
		linearFree(GFX.Screen);
	if (ntsc_screen_buffer)
		linearFree(ntsc_screen_buffer);
#else
	free(GFX.Screen);
	free(ntsc_screen_buffer);
#endif
	libretro_supports_bitmasks = false;
}

void retro_reset (void)
{
	S9xSoftReset();
}

uint16_t snes_lut[] = {
	SNES_B_MASK,		// 32768
	SNES_Y_MASK,		// 16384
	SNES_SELECT_MASK,	// 8192
	SNES_START_MASK,	// 4096
	SNES_UP_MASK,		// 2048
	SNES_DOWN_MASK,		// 1024
	SNES_LEFT_MASK,		// 512
	SNES_RIGHT_MASK,	// 256
	SNES_A_MASK,		// 128
	SNES_X_MASK,		// 64
	SNES_TL_MASK,		// 32
	SNES_TR_MASK		// 16
};

#define SWITCH_L2	(input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2))
#define PRESSED_R2 	(input_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2))

static void report_buttons(void)
{
	uint32_t i, port;
	int16_t ret;

	for (port = 0; port < 2; port++)
	{
		ret = 0;
		switch (retro_devices[port])
		{
			case RETRO_DEVICE_JOYPAD:
			{
				bool pressed;
				uint16_t button_press;

				if (libretro_supports_bitmasks)
					ret = input_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
				else
				{
					for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
						if (input_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
							ret |= (1 << i);
				}

				for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
				{
					pressed = ret & (1 << i);
					button_press = snes_lut[i];

					if (pressed)
						joypad[port] |= button_press;
					else
						joypad[port] &= ~button_press;
				}

				if (input_vars.turbo_enable == 1)
				{
					if (PRESSED_R2)
					{
						if (input_vars.turbo_state[port])
							input_vars.turbo_state[port]--;
						else
						{
							input_vars.turbo_state[port] = input_vars.turbo_delay;
							joypad[port] |= 0x4000;		// Button Y
						}
					}
					else
						input_vars.turbo_state[port] = 0;
				}

				if (input_vars.turbo_enable == 2)
				{
					if (PRESSED_R2)
					{
						if (input_vars.turbo_state[port])
							input_vars.turbo_state[port]--;
						else
						{
							input_vars.turbo_state[port] = input_vars.turbo_delay;
							joypad[port] |= 0x8000;		// Button B
						}
					}
					else
						input_vars.turbo_state[port] = 0;
				}
			}
			break;
			case RETRO_DEVICE_JOYPAD_MULTITAP:
			{
				uint32_t j;
				for (j = 0; j < 4; j++)
				{
					unsigned new_port = Settings.CurrentROMisMultitapCompatible ? (port + j) : (port);
					if (libretro_supports_bitmasks)
						ret = input_cb(new_port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
					else
					{
						for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
							if (input_cb(new_port, RETRO_DEVICE_JOYPAD, 0, i))
								ret |= (1 << i);
					}

					for (i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
					{
						bool pressed = ret & (1 << i);
						uint16_t button_press = snes_lut[i];

						if (pressed)
							joypad[j * 2 + port] |= button_press;
						else
							joypad[j * 2 + port] &= ~button_press;
					}
				}
			}
			break;
			case RETRO_DEVICE_MOUSE:
			{
				int32_t _x, _y;
				_x = input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
				_y = input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
				input_vars.retro_mouse_state[port][0] += _x;
				input_vars.retro_mouse_state[port][1] += _y;
				S9xApplyCommand(keymap[BTN_POINTER + port], input_vars.retro_mouse_state[port][0], input_vars.retro_mouse_state[port][1]);
				for (i = RETRO_DEVICE_ID_MOUSE_LEFT; i <= RETRO_DEVICE_ID_MOUSE_RIGHT; i++)
					S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_MOUSE, 0, i), 0);
			}
			break;
			case RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE:
				input_vars.retro_scope_state[0] += input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
				input_vars.retro_scope_state[1] += input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
				S9xApplyCommand(keymap[BTN_POINTER], input_vars.retro_scope_state[0], input_vars.retro_scope_state[1]);

				for (i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_PAUSE; i++)
					S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, i), 0);
			break;
			case RETRO_DEVICE_LIGHTGUN_JUSTIFIER:
			case RETRO_DEVICE_LIGHTGUN_JUSTIFIERS:
				input_vars.retro_justifier_state[0][0] += input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
				input_vars.retro_justifier_state[0][1] += input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
				S9xApplyCommand(keymap[BTN_POINTER], input_vars.retro_justifier_state[0][0], input_vars.retro_justifier_state[0][1]);

				for (i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_START; i++)
					S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, i), 0);
			break;
			default:
				S9xMessage(S9X_MSG_ERROR, S9X_CATEGORY_EXTERNAL,"Unknown input device.");
			break;
		}
	}

   	if (SWITCH_L2 && !input_vars.switch_state)
   	{
		struct retro_message message;
        input_vars.switch_state = true;
		message.frames = 120;
		message.msg = NULL;

		switch (input_vars.turbo_enable)
		{
			case 0:
				input_vars.turbo_enable = 1;
				message.msg = "The button Y is mapped to R2 and autofire enabled.";
			break;
			case 1:
				input_vars.turbo_enable = 2;
				message.msg = "The button B is mapped to R2 and autofire enabled.";
			break;
			case 2:
				input_vars.turbo_enable = 0;
				message.msg = "The button mapping has been cancelled.";
			break;
		}
		environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, (void *)&message);
   	}
   	else if (!SWITCH_L2 && input_vars.switch_state)
		input_vars.switch_state = false;
}
#undef SWITCH_L2
#undef PRESSED_R2

void retro_run(void)
{
	int result = -1;
	bool okay = false;
	bool updated = false;

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
	{
		check_variables();
		update_geometry();
	}

	okay = (environ_cb(RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE, &result));
	if (okay)
	{
		bool audioEnabled = 0 != (result & 0x02);
		bool videoEnabled = 0 != (result & 0x01);
		bool hardDisableAudio = 0 != (result & 0x08);
		IPPU.RenderThisFrame = videoEnabled;

		S9xSetSoundMute(!audioEnabled || hardDisableAudio);
		Settings.HardDisableAudio = hardDisableAudio;
	}
	else
	{
		IPPU.RenderThisFrame = true;
		S9xSetSoundMute(false);
		Settings.HardDisableAudio = false;
	}

	poll_cb();
	report_buttons();

	S9xMainLoop();

	/* Force emptying the audio buffer at the end of the frame. */
	S9xAudioCallbackQueue();
}

size_t retro_serialize_size(void)
{
	/* FIXME: No fail check, magic large number, etc. */
	uint8_t *tmpbuf = (uint8_t*)malloc(5000000);
	memstream_set_buffer(tmpbuf, (uint64_t)5000000);

	S9xFreezeGame();
	free(tmpbuf);

	return memstream_get_last_size();
}

bool retro_serialize(void *data, size_t size)
{
	int result = -1;
	if (environ_cb(RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE, &result))
		Settings.FastSavestates = 0 != (result & 0x04);

	memstream_set_buffer((uint8_t*)data, (uint64_t)size);

	if (S9xFreezeGame() == FALSE)
		return FALSE;

	return TRUE;
}

bool retro_unserialize(const void * data, size_t size)
{
	int result = -1;
	if (environ_cb(RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE, &result))
		Settings.FastSavestates = 0 != (result & 0x04);

	memstream_set_buffer((uint8_t*)data, (uint64_t)size);

	if (S9xUnfreezeGame() == FALSE)
		return FALSE;

	return TRUE;
}

void retro_cheat_reset(void)
{
	S9xDeleteCheats();
	S9xApplyCheats();
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	uint32 address;
	uint8 val;

	bool8 sram;
	uint8 bytes[3]; /* used only by GoldFinger, ignored for now */

	if (S9xGameGenieToRaw(code, &address, &val) != NULL &&
		S9xProActionReplayToRaw(code, &address, &val) != NULL &&
		S9xGoldFingerToRaw(code, &address, &sram, &val, bytes) != NULL)
	{
		/* Ignore bad codes. */
		return;
	}
	if (index > Cheat.num_cheats)
		return; /* Ignore cheat added in weird order. */

	if (index == Cheat.num_cheats)
		Cheat.num_cheats++;

	Cheat.c[index].address = address;
	Cheat.c[index].byte = val;
	Cheat.c[index].enabled = enabled;

	Cheat.c[index].saved = FALSE; /* it'll be saved next time cheats run */

	Settings.ApplyCheats = true;
	S9xApplyCheats();
}

#define MAX_MAPS 32
static struct retro_memory_descriptor memorydesc[MAX_MAPS];
static unsigned memorydesc_c;

static bool merge_mapping(void)
{
	struct retro_memory_descriptor *a, *b;
	uint32_t len;

	if (memorydesc_c == 1)
		return false; /* can't merge the only one */

	a = &memorydesc[MAX_MAPS - (memorydesc_c - 1)];
	b = &memorydesc[MAX_MAPS - memorydesc_c];

	if (a->flags != b->flags)
		return false;
	if (a->disconnect != b->disconnect)
		return false;
	if (a->len != b->len)
		return false;
	if (a->addrspace || b->addrspace)
		return false; /* we don't use these */

	if (((char*)a->ptr) + a->offset == ((char*)b->ptr) + b->offset && a->select == b->select)
	{
		a->select &= ~(a->start ^ b->start);
		memorydesc_c--;
		return true;
	}

	len = a->len;
	if (!len)
		len=(0x1000000 - a->select);

	if (len && ((len - 1) & (len | a->disconnect)) == 0 && ((char*)a->ptr) + a->offset + len == ((char*)b->ptr) + b->offset)
	{
		a->select &= ~len;
		a->disconnect &= ~len;
		memorydesc_c--;
		return true;
	}
	return false;
}

void S9xAppendMapping(struct retro_memory_descriptor *desc)
{
	/* do it backwards - snes9x defines the last one to win, while we define
	 * the first one to win */
	memcpy(&memorydesc[MAX_MAPS - (++memorydesc_c)], desc, sizeof(struct retro_memory_descriptor));
	while (merge_mapping())
	{
	}
}

static void init_descriptors(void)
{
#define describe_buttons(INDEX)	\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },	\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },		\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },	\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },		\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },		\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },			\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },		\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },			\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },		\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },			\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },	\
		{ INDEX, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

	struct retro_input_descriptor desc[] = {
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     "Switch Autofire Button" },
		{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     "AutoFire Button" },
		describe_buttons(0)
		describe_buttons(1)
		describe_buttons(2)
		describe_buttons(3)
		describe_buttons(4)
		{ 0, 0, 0, 0, NULL }
	};
	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);
}

bool retro_load_game(const struct retro_game_info *game)
{
	int loaded;
	struct retro_memory_map map;

	init_descriptors();
	memorydesc_c = 0;

	map.descriptors = memorydesc + MAX_MAPS - memorydesc_c;
	map.num_descriptors = memorydesc_c;

	/* Hack. S9x cannot do stuff from RAM. <_< */
	memstream_set_buffer((uint8_t*)game->data, (uint64_t)game->size);

	loaded = LoadROM("");
	if (!loaded)
	{
		const char *const err_msg = "ROM loading failed.";
		struct retro_message msg = { err_msg, 360 };

		S9xMessage(S9X_MSG_ERROR, S9X_CATEGORY_EXTERNAL, err_msg);

		if (environ_cb)
			environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &msg);

		return FALSE;
	}

	check_variables();

	set_system_specs();
	environ_cb(RETRO_ENVIRONMENT_SET_MEMORY_MAPS, &map);

	return TRUE;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
	(void)game_type;
	(void)info;
	(void)num_info;

	return false;
}

void retro_unload_game (void) { }

unsigned retro_get_region (void)
{
	return Settings.PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC;
}

void S9xDeinitUpdate(int width, int height)
{
	static int burst_phase = 0;

	if (snes_ntsc_filter)
	{
		burst_phase = (burst_phase + 1) % 3;

		if (width == 512)
			snes_ntsc_blit_hires(&snes_ntsc, GFX.Screen, GFX.Pitch / 2, burst_phase, width, height, ntsc_screen_buffer, GFX.Pitch);
		else
			snes_ntsc_blit(&snes_ntsc, GFX.Screen, GFX.Pitch / 2, burst_phase, width, height, ntsc_screen_buffer, GFX.Pitch);

		video_cb(ntsc_screen_buffer, SNES_NTSC_OUT_WIDTH(width), height, GFX.Pitch);
	}
	else
		/* GFX.Pitch = width * 2; */
		video_cb(GFX.Screen, width, height, GFX.Pitch);
}

/* Dummy functions that should probably be implemented correctly later. */
const char* S9xGetDirectory(uint32_t dirtype) { return NULL; }

static enum retro_log_level s9x_msg_priority_to_retro_log(S9xMessagePriority p)
{
   switch (p)
   {
      case S9X_MSG_VERBOSE:
         return RETRO_LOG_DEBUG;
      case S9X_MSG_INFO:
         return RETRO_LOG_INFO;
      case S9X_MSG_WARN:
         return RETRO_LOG_WARN;
      case S9X_MSG_ERROR:
         return RETRO_LOG_ERROR;
   }
   return RETRO_LOG_INFO;
}

void S9xMessage (S9xMessagePriority p, S9xMessageCategory c, const char *msg)
{
	const char *const S9xMessageCategoryStr[] = {
		"ROM", "PPU", "CPU", "APU", "MAP", "CONTROLS", "SNAPSHOT", "EXT"
	};
   enum retro_log_level p_info = s9x_msg_priority_to_retro_log(p);

	if (log_cb)
		log_cb(p_info, "%s: %s\n", S9xMessageCategoryStr[c], msg);
}
