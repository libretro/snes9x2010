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

#include <stdio.h>
#include <stdint.h>
#ifndef _MSC_VER
#include <stdbool.h>
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <libretro.h>

#include "../src/boolean.h"
#include "../src/snes9x.h"
#include "../src/memmap.h"
#include "../src/cpuexec.h"
#include "../src/srtc.h"
#include "../src/fxemu.h"
#include "../src/apu.h"
#include "../src/ppu.h"
#include "../src/snapshot.h"
#include "../src/controls.h"
#include "../src/cheats.h"
#include "../src/display.h"

#define LR_MAP_BUTTON(id, name) S9xMapButton((id), S9xGetCommandT((name)))
#define MAKE_BUTTON(pad, btn) (((pad)<<4)|(btn))

#define BTN_POINTER (RETRO_DEVICE_ID_JOYPAD_R + 1)
#define BTN_POINTER2 (BTN_POINTER + 1)

#define RETRO_DEVICE_JOYPAD_MULTITAP       RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 0)
#define RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE  RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 0)
#define RETRO_DEVICE_LIGHTGUN_JUSTIFIER    RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 1)
#define RETRO_DEVICE_LIGHTGUN_JUSTIFIERS   RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_LIGHTGUN, 2)

#define CORE_VERSION "1.52.4"

static retro_log_printf_t log_cb = NULL;
static retro_video_refresh_t video_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;

extern s9xcommand_t			keymap[1024];
bool overclock_cycles = false;
bool reduce_sprite_flicker = false;
int one_c, slow_one_c, two_c;

static void check_variables(void)
{
   bool reset_sfx = false;
   struct retro_variable var;
   var.key = "snes9x_next_overclock";
   var.value = NULL;
   
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "Disabled(10MHz)") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 10.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "40MHz") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 40.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "60MHz") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 60.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "80MHz") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 80.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "100MHz") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 100.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "Underclock(5MHz)") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 5.5e6;
         reset_sfx = true;
      }
      else if (strcmp(var.value, "Underclock(8MHz)") == 0)
      {
         Settings.SuperFXSpeedPerLine = 0.417 * 8.5e6;
         reset_sfx = true;
      }
   }

   var.key = "snes9x_next_overclock_cycles";
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
        else
          overclock_cycles = false;
      }

   var.key = "snes9x_next_reduce_sprite_flicker";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
      {
        if (strcmp(var.value, "enabled") == 0)
          reduce_sprite_flicker = true;
        else
          reduce_sprite_flicker = false;
      }

   if (reset_sfx)
   S9xResetSuperFX();
}

void *retro_get_memory_data(unsigned type)
{
   uint8_t* data;

   switch(type)
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
      //case RETRO_MEMORY_ROM:
      //   data = Memory.ROM;
      //   break;
      default:
         data = NULL;
         break;
   }

   return data;
}

size_t retro_get_memory_size(unsigned type)
{
   unsigned size;

   switch(type)
   {
      case RETRO_MEMORY_SAVE_RAM:
         size = (unsigned) (Memory.SRAMSize ? (1 << (Memory.SRAMSize + 3)) * 128 : 0);
         if (size > 0x20000)
            size = 0x20000;
         break;
      case RETRO_MEMORY_RTC:
         size = (Settings.SRTC || Settings.SPC7110RTC)?20:0;
         break;
      case RETRO_MEMORY_SYSTEM_RAM:
         size = 128 * 1024;
         break;
      case RETRO_MEMORY_VIDEO_RAM:
         size = 64 * 1024;
         break;
      //case RETRO_MEMORY_ROM:
      //   data = Memory.CalculatedSize;
      //   break;
      default:
         size = 0;
         break;
   }

   return size;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_cb = cb;
}

static bool use_overscan;

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_variable vars[] = {
      { "snes9x_next_overclock", "SuperFX Overclock; Disabled(10MHz)|40MHz|60MHz|80MHz|100MHz|Underclock(5MHz)|Underclock(8MHz)" },
      { "snes9x_next_overclock_cycles", "Reduce Slowdown (Hack, Unsafe); disabled|compatible|max" },
      { "snes9x_next_reduce_sprite_flicker", "Reduce Flickering (Hack, Unsafe); disabled|enabled" },
      { NULL, NULL },
   };

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

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
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
   info->library_name     = "Snes9x 2010";
   info->block_extract    = false;
}

static void S9xAudioCallback(void)
{
   size_t avail;
   /* Just pick a big buffer. We won't use it all. */
   static int16_t audio_buf[0x20000];

   S9xFinalizeSamples();
   avail = S9xGetSampleCount();
   S9xMixSamples(audio_buf, avail);
   audio_batch_cb(audio_buf, avail >> 1);
}

static unsigned retro_devices[2];

void retro_set_controller_port_device(unsigned in_port, unsigned device)
{
   int port = in_port;

   if (port >= 2) return;

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
         if (log_cb)
            log_cb(RETRO_LOG_ERROR, "Invalid device!\n");
   }

   if (((retro_devices[0] == RETRO_DEVICE_JOYPAD) && retro_devices[1] == RETRO_DEVICE_JOYPAD) ||
      ((retro_devices[0] == RETRO_DEVICE_JOYPAD) && retro_devices[1] == RETRO_DEVICE_JOYPAD_MULTITAP) ||
         ((retro_devices[0] == RETRO_DEVICE_JOYPAD_MULTITAP) && retro_devices[1] == RETRO_DEVICE_JOYPAD))
      Settings.NormalControls = 1;
}

static void map_buttons (void)
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

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width = 256;
   info->geometry.base_height = use_overscan ? 239 : 224;
   info->geometry.max_width = 512;
   info->geometry.max_height = 512;
   info->geometry.aspect_ratio = 4.0 / 3.0;
   if (!Settings.PAL)
      info->timing.fps = 21477272.0 / 357366.0;
   else
      info->timing.fps = 21281370.0 / 425568.0;
   info->timing.sample_rate = 32040.5;
}

static void snes_init (void)
{
   memset(&Settings, 0, sizeof(Settings));
   Settings.SpeedhackGameID = SPEEDHACK_NONE;
   Settings.Transparency = TRUE;
   Settings.FrameTimePAL = 20000;
   Settings.FrameTimeNTSC = 16667;
   Settings.SoundPlaybackRate = 32000;
   Settings.SoundInputRate = 32000;
   Settings.HDMATimingHack = 100;
   Settings.BlockInvalidVRAMAccessMaster = TRUE;
   Settings.CartAName[0] = 0;
   Settings.CartBName[0] = 0;
   Settings.Crosshair = 1;

   CPU.Flags = 0;

   if (!Init() || !S9xInitAPU())
   {
      Deinit();
      S9xDeinitAPU();
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Failed to init Memory or APU.\n");
      exit(1);
   }
   
   //very slow devices will still pop
   
   //this needs to be applied to all snes9x cores
   
   //increasing the buffer size does not cause extra lag(tested with 1000ms buffer)
   //bool8 S9xInitSound (int buffer_ms, int lag_ms)

   S9xInitSound(1000, 0);//just give it a 1 second buffer

   S9xSetSamplesAvailableCallback(S9xAudioCallback);

   GFX.Pitch = use_overscan ? 1024 : 2048; // FIXME: What is this supposed to do? Overscan has nothing to do with anything like this. If this is the Wii performance hack, it should be done differently.

#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L) && !defined(GEKKO) && !defined(_3DS)
   /* request 128-bit alignment here if possible */
   posix_memalign((void**)&GFX.Screen, 16, GFX.Pitch * 512 * sizeof(uint16));
#elif defined(_3DS)
   GFX.Screen = (uint16*) memalign(16, GFX.Pitch * 512 * sizeof(uint16));
#else
   GFX.Screen = (uint16*) calloc(1, GFX.Pitch * 512 * sizeof(uint16));
#endif

   S9xGraphicsInit();

   retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
   retro_set_controller_port_device(1, RETRO_DEVICE_JOYPAD);

   S9xUnmapAllControls();
   map_buttons();

   /* Initialize SuperFX CPU to normal speed by default */
   Settings.SuperFXSpeedPerLine = 0.417 * 10.5e6;
}

static void check_system_specs(void)
{
   unsigned level = 7;
   environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);
}

void retro_init (void)
{
   struct retro_log_callback log;
   enum retro_pixel_format rgb565;
   bool achievements             = true;
   if (!environ_cb(RETRO_ENVIRONMENT_GET_OVERSCAN, &use_overscan))
	   use_overscan = FALSE;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   // State that the core supports achievements.
   environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_ACHIEVEMENTS, &achievements);

   rgb565 = RETRO_PIXEL_FORMAT_RGB565;
   if(environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565) && log_cb)
         log_cb(RETRO_LOG_INFO, "Frontend supports RGB565 - will use that instead of XRGB1555.\n");

   snes_init();
   check_system_specs();
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
   
   free(GFX.Screen);
}

void retro_reset (void)
{
   S9xSoftReset();
}

static int16_t retro_mouse_state[2][2] = {{0}, {0}};
static int16_t retro_scope_state[2] = {0};
static int16_t retro_justifier_state[2][2] = {{0}, {0}};
extern uint16_t joypad[8];

uint16_t snes_lut[] = { 
SNES_B_MASK,
SNES_Y_MASK,
SNES_SELECT_MASK,
SNES_START_MASK,
SNES_UP_MASK,
SNES_DOWN_MASK,
SNES_LEFT_MASK,
SNES_RIGHT_MASK,
SNES_A_MASK,
SNES_X_MASK,
SNES_TL_MASK,
SNES_TR_MASK
};

#define TIMER_DELAY 10
//#define DEBUG_CONTROLS 1

extern bool8 coldata_update_screen;

static void report_buttons (void)
{
   int i, j, _x, _y, port;
#ifdef DEBUG_CONTROLS
   bool pressed_r2, pressed_l2, pressed_r3;
   static unsigned timeout = TIMER_DELAY;
#endif

   for ( port = 0; port <= 1; port++)
   {
      switch (retro_devices[port])
      {
	      case RETRO_DEVICE_JOYPAD:
		      for ( i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
		      {
			      bool pressed = input_cb(port, RETRO_DEVICE_JOYPAD, 0, i);
                              uint16_t button_press = snes_lut[i];

			      if (pressed)
				      joypad[port] |= button_press;
			      else
				      joypad[port] &= ~button_press;
		      }
#ifdef DEBUG_CONTROLS
		      pressed_l2 = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2);
		      if(pressed_l2 && timeout == 0)
		      {
			      coldata_update_screen = !coldata_update_screen;
               timeout = TIMER_DELAY;
               if (log_cb)
                  log_cb(RETRO_LOG_INFO, "coldata_update_screen: %d.\n", coldata_update_screen);
		      }
		      pressed_r2 = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2);
		      if(pressed_r2 && timeout == 0)
		      {
			      PPU.RenderSub = !PPU.RenderSub;
               timeout = TIMER_DELAY;
               if (log_cb)
                  log_cb(RETRO_LOG_INFO, "RenderSub: %d.\n", PPU.RenderSub);
		      }
		      pressed_r3 = input_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3);
		      if(pressed_r3 && timeout == 0)
		      {
			      PPU.SFXSpeedupHack = !PPU.SFXSpeedupHack;
               timeout = TIMER_DELAY;
               if (log_cb)
                  log_cb(RETRO_LOG_INFO, "SFXSpeedupHack: %d.\n", PPU.SFXSpeedupHack);
		      }
#endif
		      break;
	      case RETRO_DEVICE_JOYPAD_MULTITAP:
		      for ( j = 0; j < 4; j++)
		      {
			      for ( i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
			      {
					bool pressed;
					uint16 button_press;

					if (Settings.CurrentROMisMultitapCompatible==TRUE)
						pressed = input_cb(port+j, RETRO_DEVICE_JOYPAD, 0, i);
					else
						pressed = input_cb(port, RETRO_DEVICE_JOYPAD, 0, i);
						
				    button_press = snes_lut[i];

				      	if (pressed)
						joypad[j*2+port] |= button_press;
				      	else
						joypad[j*2+port] &= ~button_press;
			      }
		      }
		      break;

	      case RETRO_DEVICE_MOUSE:
		      _x = input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
		      _y = input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
		      retro_mouse_state[port][0] += _x;
		      retro_mouse_state[port][1] += _y;
            S9xApplyCommand(keymap[BTN_POINTER + port], retro_mouse_state[port][0], retro_mouse_state[port][1]);

		      for ( i = RETRO_DEVICE_ID_MOUSE_LEFT; i <= RETRO_DEVICE_ID_MOUSE_RIGHT; i++)
               S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_MOUSE, 0, i), 0);
		      break;

	      case RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE:
		      retro_scope_state[0] += input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
		      retro_scope_state[1] += input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
            S9xApplyCommand(keymap[BTN_POINTER], retro_scope_state[0], retro_scope_state[1]);

		      for ( i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_PAUSE; i++)
               S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, i), 0);
		      break;

	      case RETRO_DEVICE_LIGHTGUN_JUSTIFIER:
	      case RETRO_DEVICE_LIGHTGUN_JUSTIFIERS:
		      retro_justifier_state[0][0] += input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
		      retro_justifier_state[0][1] += input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
            S9xApplyCommand(keymap[BTN_POINTER], retro_justifier_state[0][0], retro_justifier_state[0][1]);

		      for ( i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_START; i++)
               S9xApplyCommand(keymap[MAKE_BUTTON(port + 1, i)], input_cb(port, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, i), 0);
		      break;

	      default:
            if (log_cb)
               log_cb(RETRO_LOG_ERROR, "Unknown device.\n");

      }
   }
#ifdef DEBUG_CONTROLS
   if(timeout != 0)   timeout--;
#endif
}

void retro_run (void)
{
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
   check_variables();

   int result = -1;
   bool okay = environ_cb(RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE, &result);
   if (!okay) result |= 3;
   bool audioEnabled = 0 != (result & 2);
   bool videoEnabled = 0 != (result & 1);

   IPPU.RenderThisFrame = videoEnabled;
   S9xSetSoundMute(!audioEnabled);

   poll_cb();
   report_buttons();
   S9xMainLoop();
   
   //Force emptying the audio buffer at the end of the frame
   S9xAudioCallback();

}

size_t retro_serialize_size (void)
{
   uint8_t *tmpbuf;

   tmpbuf = (uint8_t*)malloc(5000000);
   memstream_set_buffer(tmpbuf, 5000000);
   S9xFreezeGame("");
   free(tmpbuf);
   return memstream_get_last_size();
}

bool retro_serialize(void *data, size_t size)
{
   memstream_set_buffer((uint8_t*)data, size);
   if (S9xFreezeGame("") == FALSE)
      return FALSE;

   return TRUE;
}

bool retro_unserialize(const void * data, size_t size)
{
   memstream_set_buffer((uint8_t*)data, size);
   if (S9xUnfreezeGame("") == FALSE)
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
   uint8 bytes[3];//used only by GoldFinger, ignored for now 
   
   if (S9xGameGenieToRaw(code, &address, &val)!=NULL &&
       S9xProActionReplayToRaw(code, &address, &val)!=NULL &&
       S9xGoldFingerToRaw(code, &address, &sram, &val, bytes)!=NULL)
   { // bad code, ignore
      return;
   }
   if (index>Cheat.num_cheats) return; // cheat added in weird order, ignore
   if (index==Cheat.num_cheats) Cheat.num_cheats++;
   
   Cheat.c[index].address = address;
   Cheat.c[index].byte = val;
   Cheat.c[index].enabled = enabled;
   
   Cheat.c[index].saved = FALSE; // it'll be saved next time cheats run anyways
   
   Settings.ApplyCheats=true;
   S9xApplyCheats();
} 

#define MAX_MAPS 32
static struct retro_memory_descriptor memorydesc[MAX_MAPS];
static unsigned memorydesc_c;

static bool merge_mapping(void)
{
    struct retro_memory_descriptor *a, *b;
	uint32 len;

	if (memorydesc_c==1)
		return false;//can't merge the only one
	a= &memorydesc[MAX_MAPS - (memorydesc_c-1)];
	b= &memorydesc[MAX_MAPS - memorydesc_c];
//printf("test %x/%x\n",a->start,b->start);
	if (a->flags != b->flags)
		return false;
	if (a->disconnect != b->disconnect)
		return false;
	if (a->len != b->len)
		return false;
	if (a->addrspace || b->addrspace)
		return false;//we don't use these
	if (((char*)a->ptr)+a->offset==((char*)b->ptr)+b->offset && a->select==b->select)
	{
//printf("merge/mirror\n");
		a->select&=~(a->start^b->start);
		memorydesc_c--;
		return true;
	}
	len = a->len;
	if (!len)
		len=(0x1000000 - a->select);
	if (len && ((len-1) & (len | a->disconnect))==0 && ((char*)a->ptr)+a->offset+len == ((char*)b->ptr)+b->offset)
	{
//printf("merge/consec\n");
		a->select &=~ len;
		a->disconnect &=~ len;
		memorydesc_c--;
		return true;
	}
//printf("nomerge\n");
	return false;
}

void S9xAppendMapping(struct retro_memory_descriptor *desc)
{
	//do it backwards - snes9x defines the last one to win, while we define the first one to win
	memcpy(&memorydesc[MAX_MAPS - (++memorydesc_c)], desc, sizeof(struct retro_memory_descriptor));
	while (merge_mapping()) {}
}

static void init_descriptors(void)
{
   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 2, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 3, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },

      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "B" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "A" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "X" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Y" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "L" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "R" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,   "Select" },
      { 4, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,    "Start" },
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
   map.descriptors    = memorydesc + MAX_MAPS - memorydesc_c;
   map.num_descriptors = memorydesc_c;

   /* Hack. S9x cannot do stuff from RAM. <_< */
   memstream_set_buffer((uint8_t*)game->data, game->size);

   loaded = LoadROM("");
   if (!loaded)
   {
      struct retro_message msg; 
      char msg_local[256];

      snprintf(msg_local,
            sizeof(msg_local), "ROM loading failed...");
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "ROM loading failed...\n");
      msg.msg    = msg_local;
      msg.frames = 360;
      if (environ_cb)
         environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, (void*)&msg);
      return FALSE;
   }

   check_variables();


   environ_cb(RETRO_ENVIRONMENT_SET_MEMORY_MAPS, &map);

   return TRUE;
}

bool retro_load_game_special(
  unsigned game_type,
  const struct retro_game_info *info, size_t num_info
)
{
   init_descriptors();
   return false;
}

void retro_unload_game (void)
{ }

unsigned retro_get_region (void)
{ 
   return Settings.PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC; 
}

void S9xDeinitUpdate(int width, int height)
{
   if (height == 448 || height == 478){
		GFX.Pitch = 1024;	/* Pitch 2048 -> 1024 */
   }
   else GFX.Pitch = 2048;		/* Pitch 1024 -> 2048 */

   
   // TODO: Reverse case.
   if (!use_overscan)
   {
      const uint16_t *frame = (const uint16_t*)GFX.Screen;
      if (height == 239)
      {
         frame += 7 * 1024;
         height = 224;
      }
      else if (height == 478)
      {
         frame += 15 * 512;
         height = 448;
      }

      video_cb(frame, width, height, GFX.Pitch);
   }
   else
      video_cb(GFX.Screen, width, height, GFX.Pitch);
}

/* Dummy functions that should probably be implemented correctly later. */
const char* S9xGetDirectory(uint32_t dirtype) { return NULL; }

void S9xMessage(int a, int b, const char* msg)
{
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "%s\n", msg);
}

/* S9x weirdness. */
#ifndef _MSC_VER
void _splitpath (const char * path, char * drive, char * dir, char * fname, char * ext)
{
	char *slash, *dot;

	slash = strrchr((char*)path, SLASH_CHAR);
	dot   = strrchr((char*)path, '.');

	if (dot && slash && dot < slash)
		dot = NULL;

	if (!slash)
	{
		*dir = 0;

		strcpy(fname, path);

		if (dot)
		{
			fname[dot - path] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
	else
	{
		strcpy(dir, path);
		dir[slash - path] = 0;

		strcpy(fname, slash + 1);

		if (dot)
		{
			fname[dot - slash - 1] = 0;
			strcpy(ext, dot + 1);
		}
		else
			*ext = 0;
	}
}

void _makepath (char *path, const char * a, const char *dir, const char *fname, const char *ext)
{
   if (dir && *dir)
   {
      strcpy(path, dir);
      strcat(path, SLASH_STR);
   }
   else
      *path = 0;

   strcat(path, fname);

   if (ext && *ext)
   {
      strcat(path, ".");
      strcat(path, ext);
   }
}
#endif
