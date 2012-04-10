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

#define LIBRETRO_CORE 1

#if defined(_MSC_VER) && defined(LIBRETRO_CORE)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include "libretro.h"

#include "../src/snes9x.h"
#include "../src/memmap.h"
#include "../src/cpuexec.h"
#include "../src/srtc.h"
#include "../src/apu.h"
#include "../src/ppu.h"
#include "../src/snapshot.h"
#include "../src/controls.h"
#include "../src/cheats.h"
#include "../src/display.h"

#define MAP_BUTTON(id, name) S9xMapButton((id), S9xGetCommandT((name)))
#define MAKE_BUTTON(pad, btn) (((pad)<<4)|(btn))

#define BTN_POINTER (RETRO_DEVICE_ID_JOYPAD_R + 1)
#define BTN_POINTER2 (BTN_POINTER + 1)

#ifdef _MSC_VER
typedef unsigned char bool;
#endif

static retro_video_refresh_t video_cb = NULL;
static retro_input_poll_t poll_cb = NULL;
static retro_input_state_t input_cb = NULL;
static retro_audio_sample_batch_t audio_batch_cb = NULL;
static retro_environment_t environ_cb = NULL;

EXPORT void * retro_get_memory_data(unsigned type)
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
      default:
	 data = NULL;
	 break;
   }

   return data;
}

EXPORT unsigned retro_get_memory_size(unsigned type)
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
      default:
	 size = 0;
	 break;
   }

   return size;
}

EXPORT unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

EXPORT void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

EXPORT void retro_set_audio_sample(retro_audio_sample_t cb)
{ }

EXPORT void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

EXPORT void retro_set_input_poll(retro_input_poll_t cb)
{
   poll_cb = cb;
}

EXPORT void retro_set_input_state(retro_input_state_t cb)
{
   input_cb = cb;
}

static bool use_overscan;

EXPORT void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;
}

EXPORT void retro_get_system_info(struct retro_system_info *info)
{
   info->need_fullpath = false;
   info->valid_extensions = "smc|fig|sfc|gd3|gd7|dx2|bsx|swc|zip|SMC|FIG|SFC|BSX|GD3|GD7|DX2|SWC|ZIP";
   info->library_version = "1.52.1";
   info->library_name = "SNES9x Next";
   info->block_extract = false;
}

static void S9xAudioCallback()
{
   size_t avail;
   /* Just pick a big buffer. We won't use it all. */
   static int16_t audio_buf[0x10000];

   S9xFinalizeSamples();
   avail = S9xGetSampleCount();
   S9xMixSamples(audio_buf, avail);
   audio_batch_cb(audio_buf, avail >> 1);
}

static unsigned retro_devices[2];

void retro_set_controller_port_device(unsigned in_port, unsigned device)
{
   int port = in_port;

   switch (device)
   {
      case RETRO_DEVICE_JOYPAD:
         S9xSetController(port, CTL_JOYPAD, 0, 0, 0, 0);
         retro_devices[port] = RETRO_DEVICE_JOYPAD;
         break;
      case RETRO_DEVICE_JOYPAD_MULTITAP:
         S9xSetController(port, CTL_MP5, 1, 2, 3, 4);
	 retro_devices[port] = RETRO_DEVICE_JOYPAD_MULTITAP;
	 break;
      case RETRO_DEVICE_MOUSE:
	 S9xSetController(port, CTL_MOUSE, 0, 0, 0, 0);
	 retro_devices[port] = RETRO_DEVICE_MOUSE;

	 /* mapping pointers here */
	 S9xMapPointer((BTN_POINTER), S9xGetCommandT("Pointer Mouse1+Superscope+Justifier1"));
	 S9xMapPointer((BTN_POINTER2), S9xGetCommandT("Pointer Mouse2"));

	 /* mapping extra buttons here */
	 MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_SELECT), "Mouse1 L");
	 MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_START), "Mouse1 R");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Mouse2 L");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Mouse2 R");
	 break;
      case RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE:
	 S9xSetController(port, CTL_SUPERSCOPE, 0, 0, 0, 0);
	 retro_devices[port] = RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE;

	 /* mapping pointers here */
	 S9xMapPointer((BTN_POINTER), S9xGetCommandT("Pointer Mouse1+Superscope+Justifier1"));
	 S9xMapPointer((BTN_POINTER2), S9xGetCommandT("Pointer Mouse2"));

	 /* mapping extra buttons here */
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Superscope Fire");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Superscope Cursor");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_UP), "Superscope ToggleTurbo");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_DOWN), "Superscope Pause");
	 break;
      case RETRO_DEVICE_LIGHTGUN_JUSTIFIER:
	 S9xSetController(port, CTL_JUSTIFIER, 0, 0, 0, 0);
	 retro_devices[port] = RETRO_DEVICE_LIGHTGUN_JUSTIFIER;

	 /* mapping extra buttons here */
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Justifier1 Trigger");
	 MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Justifier1 Start");
	 break;
      case RETRO_DEVICE_LIGHTGUN_JUSTIFIERS:
	 S9xSetController(port, CTL_JUSTIFIER, 1, 0, 0, 0);
	 retro_devices[port] = RETRO_DEVICE_LIGHTGUN_JUSTIFIERS;
	 break;
      default:
	 fprintf(stderr, "[libsnes]: Invalid device!\n");
   }
}



static void map_buttons (void)
{
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_A), "Joypad1 A");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_B), "Joypad1 B");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_X), "Joypad1 X");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad1 Y");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad1 Select");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_START), "Joypad1 Start");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_L), "Joypad1 L");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_R), "Joypad1 R");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad1 Left");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad1 Right");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad1 Up");
   MAP_BUTTON(MAKE_BUTTON(1, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad1 Down");

   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_A), "Joypad2 A");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_B), "Joypad2 B");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_X), "Joypad2 X");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad2 Y");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad2 Select");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_START), "Joypad2 Start");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_L), "Joypad2 L");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_R), "Joypad2 R");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad2 Left");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad2 Right");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad2 Up");
   MAP_BUTTON(MAKE_BUTTON(2, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad2 Down");

   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_A), "Joypad3 A");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_B), "Joypad3 B");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_X), "Joypad3 X");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad3 Y");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad3 Select");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_START), "Joypad3 Start");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_L), "Joypad3 L");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_R), "Joypad3 R");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad3 Left");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad3 Right");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad3 Up");
   MAP_BUTTON(MAKE_BUTTON(3, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad3 Down");

   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_A), "Joypad4 A");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_B), "Joypad4 B");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_X), "Joypad4 X");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad4 Y");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad4 Select");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_START), "Joypad4 Start");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_L), "Joypad4 L");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_R), "Joypad4 R");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad4 Left");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad4 Right");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad4 Up");
   MAP_BUTTON(MAKE_BUTTON(4, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad4 Down");

   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_A), "Joypad5 A");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_B), "Joypad5 B");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_X), "Joypad5 X");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_Y), "Joypad5 Y");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_SELECT), "Joypad5 Select");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_START), "Joypad5 Start");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_L), "Joypad5 L");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_R), "Joypad5 R");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_LEFT), "Joypad5 Left");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_RIGHT), "Joypad5 Right");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_UP), "Joypad5 Up");
   MAP_BUTTON(MAKE_BUTTON(5, RETRO_DEVICE_ID_JOYPAD_DOWN), "Joypad5 Down");
}

EXPORT void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width = 256;
   info->geometry.base_height = 239;
   info->geometry.max_width = 512;
   info->geometry.max_height = 512;
   if (!Settings.PAL)
      info->timing.fps = 21477272.0 / 357366.0;
   else
      info->timing.fps = 21281370.0 / 425568.0;
   info->timing.sample_rate = 32040.5;
}

static void snes_init (void)
{
   memset(&Settings, 0, sizeof(Settings));
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
      fprintf(stderr, "[libsnes]: Failed to init Memory or APU.\n");
      exit(1);
   }

   S9xInitSound(16, 0);
   S9xSetSamplesAvailableCallback(S9xAudioCallback);

   GFX.Pitch = use_overscan ? 1024 : 2048;
   GFX.Screen = (uint16*) calloc(1, GFX.Pitch * 512 * sizeof(uint16));
   S9xGraphicsInit();

   /* controller port 1 */
   S9xSetController(0, CTL_JOYPAD, 0, 0, 0, 0);
   retro_devices[0] = RETRO_DEVICE_JOYPAD;

   /* controller port 2 */
   S9xSetController(1, CTL_JOYPAD, 1, 0, 0, 0);
   retro_devices[1] = RETRO_DEVICE_JOYPAD;

   S9xUnmapAllControls();
   map_buttons();
}

EXPORT void retro_init (void)
{
   if (!environ_cb(RETRO_ENVIRONMENT_GET_OVERSCAN, &use_overscan))
	   use_overscan = FALSE;

   snes_init();
}

/* libsnes uses relative values for analogue devices. 
   S9x seems to use absolute values, but do convert these into relative values in the core. (Why?!)
   Hack around it. :) */

EXPORT void retro_deinit(void)
{
   S9xDeinitAPU();
   Deinit();
   S9xGraphicsDeinit();
   S9xUnmapAllControls();
}

void retro_reset (void)
{
   S9xSoftReset();
}

static int16_t retro_mouse_state[2][2] = {{0}, {0}};
static int16_t retro_scope_state[2] = {0};
static int16_t retro_justifier_state[2][2] = {{0}, {0}};
extern uint16_t joypad[8];
extern s9xcommand_t keymap[1024];

static void report_buttons (void)
{
   int i, j, _x, _y, port;
   for ( port = 0; port <= 1; port++)
   {
      switch (retro_devices[port])
      {
	      case RETRO_DEVICE_JOYPAD:
		      for ( i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
		      {
			      s9xcommand_t cmd = keymap[MAKE_BUTTON(port + 1, i)];
			      uint16 r = cmd.commandunion.button.joypad;
			      bool pressed = input_cb(port == 1, RETRO_DEVICE_JOYPAD, 0, i);

			      if (pressed)
				      joypad[port] |= r;
			      else
				      joypad[port] &= ~r;
		      }
		      break;
	      case RETRO_DEVICE_JOYPAD_MULTITAP:
		      for ( j = 0; j < 4; j++)
		      {
			      for ( i = RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R; i++)
			      {
				      s9xcommand_t cmd = keymap[MAKE_BUTTON(j + 2, i)];
				      uint16 r = cmd.commandunion.button.joypad;
				      bool pressed = input_cb(port == 1, RETRO_DEVICE_JOYPAD_MULTITAP, j, i);

				      if (pressed)
					      joypad[j] |= r;
				      else
					      joypad[j] &= ~r;
			      }
		      }
		      break;

	      case RETRO_DEVICE_MOUSE:
		      _x = input_cb(port == 1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
		      _y = input_cb(port == 1, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
		      retro_mouse_state[port][0] += _x;
		      retro_mouse_state[port][1] += _y;
		      S9xReportPointer(BTN_POINTER + port, retro_mouse_state[port][0], retro_mouse_state[port][1]);
		      for ( i = RETRO_DEVICE_ID_MOUSE_LEFT; i <= RETRO_DEVICE_ID_MOUSE_RIGHT; i++)
			      S9xReportButton(MAKE_BUTTON(port + 1, i), input_cb(port == 1, RETRO_DEVICE_MOUSE, 0, i));
		      break;

	      case RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE:
		      retro_scope_state[0] += input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
		      retro_scope_state[1] += input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
		      S9xReportPointer(BTN_POINTER, retro_scope_state[0], retro_scope_state[1]);
		      for ( i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_PAUSE; i++)
			      S9xReportButton(MAKE_BUTTON(port + 1, i), input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE, 0, i));
		      break;

	      case RETRO_DEVICE_LIGHTGUN_JUSTIFIER:
	      case RETRO_DEVICE_LIGHTGUN_JUSTIFIERS:
		      retro_justifier_state[0][0] += input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_X);
		      retro_justifier_state[0][1] += input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, RETRO_DEVICE_ID_LIGHTGUN_Y);
		      S9xReportPointer(BTN_POINTER, retro_justifier_state[0][0], retro_justifier_state[0][1]);
		      for ( i = RETRO_DEVICE_ID_LIGHTGUN_TRIGGER; i <= RETRO_DEVICE_ID_LIGHTGUN_START; i++)
			      S9xReportButton(MAKE_BUTTON(port + 1, i), input_cb(port == 1, RETRO_DEVICE_LIGHTGUN_JUSTIFIER, 0, i));
		      break;

	      default:
		      fprintf(stderr, "[libsnes]: Unknown device...\n");

      }
   }
}

EXPORT void retro_run (void)
{
   S9xMainLoop();

   poll_cb();

   report_buttons();
}

static char fullpath[1024];

EXPORT unsigned retro_serialize_size (void)
{
   uint8_t *tmpbuf;

   tmpbuf = (uint8_t*)malloc(5000000);
   memstream_set_buffer(tmpbuf, 5000000);
   S9xFreezeGame(fullpath);
   free(tmpbuf);
   return memstream_get_last_size();
}

EXPORT bool retro_serialize(void *data, size_t size)
{
   memstream_set_buffer((uint8_t*)data, size);
   if (S9xFreezeGame(fullpath) == FALSE)
      return FALSE;

   return TRUE;
}

EXPORT bool retro_unserialize(const void * data, size_t size)
{
   memstream_set_buffer((uint8_t*)data, size);
   if (S9xUnfreezeGame(fullpath) == FALSE)
      return FALSE;

   return TRUE;
}

void retro_cheat_reset(void)
{ }

void retro_cheat_set(unsigned unused, bool unused1, const char* unused2)
{ }

EXPORT bool retro_load_game(const struct retro_game_info *game)
{
   int loaded;

   strcpy(fullpath, game->path);

   /* Hack. S9x cannot do stuff from RAM. <_< */
   memstream_set_buffer((uint8_t*)game->data, game->size);

   loaded = LoadROM(fullpath);
   if (!loaded)
   {
      fprintf(stderr, "[libsnes]: Rom loading failed...\n");
      return FALSE;
   }

   
   return TRUE;
}

EXPORT bool retro_load_game_special(
  unsigned game_type,
  const struct retro_game_info *info, size_t num_info
)
{ return false; }

EXPORT void retro_unload_game (void)
{ }

EXPORT unsigned retro_get_region (void)
{ 
   return Settings.PAL ? RETRO_REGION_PAL : RETRO_REGION_NTSC; 
}

void S9xDeinitUpdate(int width, int height)
{
	int y;
	if (height == 448 || height == 478)
	{
		/* Pitch 2048 -> 1024, only done once per res-change. */
		if (GFX.Pitch == 2048)
		{
			for ( y = 1; y < height; y++)
			{
				uint16_t *src = GFX.Screen + y * 1024;
				uint16_t *dst = GFX.Screen + y * 512;
				memcpy(dst, src, width * sizeof(uint16_t));
			}
		}
		GFX.Pitch = 1024;
	}
	else
	{
		/* Pitch 1024 -> 2048, only done once per res-change. */
		if (GFX.Pitch == 1024)
		{
			for ( y = height - 1; y >= 0; y--)
			{
				uint16_t *src = GFX.Screen + y * 512;
				uint16_t *dst = GFX.Screen + y * 1024;
				memcpy(dst, src, width * sizeof(uint16_t));
			}
		}
		GFX.Pitch = 2048;
	}

	video_cb(GFX.Screen, width, height, GFX.Pitch);
}

/* Dummy functions that should probably be implemented correctly later. */
const char* S9xGetFilename(const char* in, uint32_t dirtype) { return in; }
const char* S9xGetDirectory(uint32_t dirtype) { return NULL; }
const char* S9xChooseFilename(bool8 a) { return NULL; }

void S9xMessage(int a, int b, const char* msg)
{
   fprintf(stderr, "%s\n", msg);
}

/* S9x weirdness. */
#ifndef _MSC_VER
void _splitpath (const char * path, char * drive, char * dir, char * fname, char * ext)
{
	char *slash, *dot;

	slash = strrchr(path, SLASH_CHAR);
	dot   = strrchr(path, '.');

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
