/******************************************************************************* 
 * emu-ps3.cpp - SNES9xNext PS3
 *
 *  Created on: May 4, 2011
********************************************************************************/
#include <sys/return_code.h>

#include <sys/process.h>
#include <cell/sysmodule.h>
#include <sysutil/sysutil_sysparam.h>
#include <sysutil/sysutil_screenshot.h>
#include <sysutil/sysutil_msgdialog.h>
#include <cell/cell_fs.h>

#if(CELL_SDK_VERSION > 0x340000)
#include <sysutil/sysutil_bgmplayback.h>
#endif

#include <sys/spu_initialize.h>

#ifndef __SNC__
#include <stdio.h>
#endif

#include <pthread.h>
#include <sys/synchronization.h>
#include "../src/snes9x.h"
#include "../src/memmap.h"
#include "../src/cpuexec.h"
#include "../src/apu.h"
#include "../src/ppu.h"
#include "../src/controls.h"
#include "../src/cheats.h"
#include "../src/display.h"
#include "../src/snapshot.h"

#include "snes_state/config_file.h"


#ifdef PS3_PROFILING
#include "cellframework/network-stdio/net_stdio.h"
#endif

#include "cellframework2/input/mouse_input.h"
#include "cellframework2/input/pad_input.h"
#include "emu-ps3.hpp"
#include "menu.hpp"

SYS_PROCESS_PARAM(1001, 0x100000)

/* PS3 frontend variables */

char contentInfoPath[MAX_PATH_LENGTH];
char usrDirPath[MAX_PATH_LENGTH];
char DEFAULT_PRESET_FILE[MAX_PATH_LENGTH];
char DEFAULT_BORDER_FILE[MAX_PATH_LENGTH];
char DEFAULT_MENU_BORDER_FILE[MAX_PATH_LENGTH];
char GAME_AWARE_SHADER_DIR_PATH[MAX_PATH_LENGTH];
char PRESETS_DIR_PATH[MAX_PATH_LENGTH];
char INPUT_PRESETS_DIR_PATH[MAX_PATH_LENGTH];
char BORDERS_DIR_PATH[MAX_PATH_LENGTH];
char SHADERS_DIR_PATH[MAX_PATH_LENGTH];
char DEFAULT_SHADER_FILE[MAX_PATH_LENGTH];
char DEFAULT_MENU_SHADER_FILE[MAX_PATH_LENGTH];
char SYS_CONFIG_FILE[MAX_PATH_LENGTH];
#ifdef MULTIMAN_SUPPORT
char MULTIMAN_GAME_TO_BOOT[MAX_PATH_LENGTH];
#endif

sys_lwmutex_t audio_lock;
cell_audio_handle_t audio_handle;
const struct cell_audio_driver *audio_driver = &cell_audio_audioport;
oskutil_params oskutil_handle;
uint32_t control_binds[MAX_PADS][BTN_DEF_MAX];

uint64_t mode_switch = MODE_MENU;			// mode the main loop is in

static uint32_t is_running;				// is the ROM currently running in the emulator?
static bool is_ingame_menu_running;			// is the ingame menu currently running?
bool return_to_MM = false;				// launch multiMAN on exit if ROM is passed
bool emulator_initialized = false;			// is the emulator loaded?
bool need_load_rom = false;				// need to load the current rom
char* current_rom = NULL;				// current filename of the ROM being emulated
bool dialog_is_running = false;				// is a dialog screen currently running?
char special_action_msg[256];				
uint32_t special_action_msg_expired;			// time at which the message no longer needs to be overlaid onscreen

uint64_t ingame_menu_item = 0;
bool need_load_settings = true;				// needs settings loaded
static uint32_t controller_settings = NO_JOYSTICKS;	// controller mode to run emulator in
bool audio_active = false;
extern bool8 pad_read_last;

//emulator-specific
extern s9xcommand_t keymap[1024];
extern uint16_t joypad[8];
static unsigned snes_devices[2];

/* PS3 frontend - save state/emulator SRAM related functions */

void set_text_message(const char * message, uint32_t speed)
{
	snprintf(special_action_msg, sizeof(special_action_msg), message);
	special_action_msg_expired = ps3graphics_set_text_message_speed(speed);
}

static void emulator_decrement_current_save_state_slot(void)
{
	char msg[512];

	if (Settings.CurrentSaveStateSlot != MIN_SAVE_STATE_SLOT)
		Settings.CurrentSaveStateSlot--;
	snprintf(msg, sizeof(msg), "Save state slot changed to: #%d", Settings.CurrentSaveStateSlot);

	set_text_message(msg, 60);
}

static void emulator_increment_current_save_state_slot(void)
{
	char msg[512];

	Settings.CurrentSaveStateSlot++;
	snprintf(msg, sizeof(msg), "Save state slot changed to: #%d", Settings.CurrentSaveStateSlot);
	
	set_text_message(msg, 60);
}

static void emulator_load_current_save_state_slot(void)
{
	char msg[512];

	int ret = S9xUnfreezeGame(S9xChooseFilename(FALSE));
	if(ret)
		snprintf(msg, sizeof(msg), "Loaded save state slot #%d", Settings.CurrentSaveStateSlot);
	else
		snprintf(msg, sizeof(msg), "Can't load from save state slot #%d", Settings.CurrentSaveStateSlot);

	set_text_message(msg, 60);
}

static void emulator_save_current_save_state_slot(void)
{
	char msg[512];

	S9xFreezeGame(S9xChooseFilename(FALSE));
	snprintf(msg, sizeof(msg), "Saved to save state slot #%d", Settings.CurrentSaveStateSlot);

	set_text_message(msg, 60);
}

/* emulator-specific */

#define S9xReportButton(i, id, action, pressed) \
      if(keymap[id].type == S9xButtonJoypad) \
      joypad[i] = ((joypad[i] | keymap[id].button.joypad) & (((pressed) | -(pressed)) >> 31)) | ((joypad[i] & ~keymap[id].button.joypad) & ~(((pressed) | -(pressed)) >> 31)); \
      else if(pressed) \
         special_action_to_execute = action;

static void callback_sysutil_exit(uint64_t status, uint64_t param, void *userdata)
{
	(void) param;
	(void) userdata;

	switch (status)
	{
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			menu_is_running = 0;
			is_running = 0;
			is_ingame_menu_running = 0;
			return_to_MM = false;
			mode_switch = MODE_EXIT;
			break;
		case CELL_SYSUTIL_DRAWING_BEGIN:
		case CELL_SYSUTIL_DRAWING_END:
		case CELL_SYSUTIL_OSKDIALOG_LOADED:
			break;
		case CELL_SYSUTIL_OSKDIALOG_FINISHED:
			oskutil_close(&oskutil_handle);
			oskutil_finished(&oskutil_handle);
			break;
		case CELL_SYSUTIL_OSKDIALOG_UNLOADED:
			oskutil_unload(&oskutil_handle);
			break;
		default:
			break;
	}
}

static void S9xAudioCallback()
{
	// Just pick a big buffer. We won't use it all.
	static int16_t audio_buf[0x10000];

	S9xFinalizeSamples();
	size_t avail = S9xGetSampleCount();
	S9xMixSamples(audio_buf, avail);
	if(Settings.Throttled)
		audio_driver->write(audio_handle, audio_buf, avail);
}

static void Emulator_SetControllerMode(int m)
{
	controller_settings = m;
}

#define audio_default_params() \
   cell_audio_params params; \
   memset(&params, 0, sizeof(params)); \
   params.channels=2; \
   params.samplerate=48000; \
   params.buffer_size=8192; \
   params.sample_cb = NULL; \
   params.userdata = NULL;

static void callback_rsound_dialog_ok(int button_type, void *userdata)
{
	switch(button_type)
	{
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
			dialog_is_running = false;
			audio_default_params();
			audio_driver = &cell_audio_audioport;
			audio_handle = audio_driver->init(&params);
			Settings.SoundMode = SOUND_MODE_NORMAL;
			break;
	}
}

static void callback_multitap_compatible(int button_type, void *userdata)
{
	switch(button_type)
	{
		case CELL_MSGDIALOG_BUTTON_YES:
			Emulator_SetControllerMode(MULTITAP);
			dialog_is_running = false;
			break;
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
		case CELL_MSGDIALOG_BUTTON_NO:
			dialog_is_running = false;
			break;
	}
}

static void callback_mouse_compatible(int button_type, void *userdata)
{
	switch(button_type)
	{
		case CELL_MSGDIALOG_BUTTON_YES:
			Emulator_SetControllerMode(MOUSE);
			dialog_is_running = false;
			break;
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
		case CELL_MSGDIALOG_BUTTON_NO:
			dialog_is_running = false;
			break;
	}
}

static void callback_superscope_compatible(int button_type, void *userdata)
{
	switch(button_type)
	{
		case CELL_MSGDIALOG_BUTTON_YES:
			Emulator_SetControllerMode(SUPERSCOPE);
			dialog_is_running = false;
			break;
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
		case CELL_MSGDIALOG_BUTTON_NO:
			dialog_is_running = false;
			break;
	}
}

static void emulator_implementation_add_cheat(uint32_t address, uint8_t byte, const char* description)
{
	S9xAddCheat(false, true, address, byte);
	S9xEnableCheat(Cheat.num_cheats - 1);
	strncpy(Cheat.c[Cheat.num_cheats - 1].name, description, 22);
}

void emulator_change_cheat_description(uint32 cheatposition, const char *description)
{
	strncpy(Cheat.c[cheatposition].name, description, 22);
	Cheat.c[cheatposition].saved = TRUE;
}

static void emulator_input_cheatlabel(char * msg)
{
	// (1) Let the user enter a label name for currently selected cheat code

	oskutil_write_initial_message(&oskutil_handle, L"");
	oskutil_write_message(&oskutil_handle, L"Enter cheat label");
	oskutil_start(&oskutil_handle);

	while(OSK_IS_RUNNING(oskutil_handle) && is_running)
	{
		/* OSK Util gets updated */
		S9xMainLoop();
		cell_console_poll();
		cellSysutilCheckCallback();
	}

	if(oskutil_handle.text_can_be_fetched)
		strncpy(msg, OUTPUT_TEXT_STRING(oskutil_handle), sizeof(msg));
	else
		strncpy(msg, "", sizeof(msg));

	// (2) add cheat label
	emulator_change_cheat_description(Settings.CurrentCheatPosition, msg);
}

#define GAME_GENIE_CODE_ADDED 0
#define PRO_ACTION_REPLAY_CODE_ADDED 1
#define GOLD_FINGER_CODE_ADDED 2
#define CODE_ADDED_INCORRECT 3
#define CODE_INPUT_CANCELLED 4

static uint32_t emulator_input_cheat()
{
	// (1) Let the user enter his cheat code
	oskutil_write_initial_message(&oskutil_handle, L"");
	oskutil_write_message(&oskutil_handle, L"Enter cheat code (Game Genie/Action Replay/GoldFinger Pro format)");
	oskutil_start(&oskutil_handle);

	while(OSK_IS_RUNNING(oskutil_handle) && is_running)
	{
		/* OSK Util gets updated */
		S9xMainLoop();
		cell_console_poll();
		cellSysutilCheckCallback();
	}

	// (2) Save the code to a char variable
	char newcode[128];

	if(oskutil_handle.text_can_be_fetched)
	{
		strncpy(newcode, OUTPUT_TEXT_STRING(oskutil_handle), sizeof(newcode));
		// (3) Check the cheat format of the cheat code
		uint32 address;
		uint8 byte;
		uint8 bytes [3];
		bool8 sram;
		uint8 num_bytes;

		int cheatformat = 0;

		if (!S9xGameGenieToRaw (newcode, &address, &byte))
		{
			// (4) add Game Genie cheat code
			emulator_implementation_add_cheat(address, byte, newcode);
			cheatformat = 1;
		}
		else if (!S9xProActionReplayToRaw(newcode, &address, &byte))
		{
			// (4) add Pro Action Replay cheat code
			emulator_implementation_add_cheat(address, byte, newcode);
			cheatformat = 2;
		}
		else if (!S9xGoldFingerToRaw(newcode, &address, &sram, &num_bytes, bytes))
		{
			// (4) add Gold Finger cheat code
			for (int c = 0; c < num_bytes; c++)
				emulator_implementation_add_cheat(address + c, bytes[c], newcode);
			cheatformat = 3;
		}
		else
			cheatformat = 4;

		// (5) Return message text string
		switch(cheatformat)
		{
			case 1:  //Game Genie
				return GAME_GENIE_CODE_ADDED;
			case 2:  //Action Replay
				return PRO_ACTION_REPLAY_CODE_ADDED;
			case 3:  //Gold Finger
				return GOLD_FINGER_CODE_ADDED;
			case 4:  //else
				return CODE_ADDED_INCORRECT;
		}
	}
	return CODE_INPUT_CANCELLED;
}

void emulator_toggle_sound(uint64_t soundmode)
{
	audio_default_params();
	switch(soundmode)
	{
		case SOUND_MODE_RSOUND:
			params.device = Settings.RSoundServerIPAddress;
			params.headset = 0;
			break;
		case SOUND_MODE_HEADSET:
			params.device = NULL;
			params.headset = 1;
			break;
		case SOUND_MODE_NORMAL: 
			params.device = NULL;
			params.headset = 0;
			break;
	}

	if(audio_handle)
	{
		audio_driver->free(audio_handle);
		audio_handle = NULL; 
	}
	if(soundmode == SOUND_MODE_RSOUND)
	{
		audio_driver = &cell_audio_rsound;
		audio_handle =  audio_driver->init(&params);
		if(!audio_handle || !(strlen(Settings.RSoundServerIPAddress) > 0))
		{
			dialog_is_running = true;
			cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_ERROR|CELL_MSGDIALOG_TYPE_BG_VISIBLE| \
			CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF| \
			CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK, \
			"Couldn't connect to RSound server at specified IP address. Falling back to regular audio.", callback_rsound_dialog_ok,NULL,NULL);
			do
			{
				glClear(GL_COLOR_BUFFER_BIT);
				psglSwap();
				cellSysutilCheckCallback();
			}while(dialog_is_running && is_running);

			cellMsgDialogClose(0.0f);
		}
	}
	else
	{
		audio_driver = &cell_audio_audioport;
		audio_handle =  audio_driver->init(&params);
	}

}

static void emulator_save_sram(void)
{
	if(!Settings.SRAMWriteProtect)
		SaveSRAM(S9xGetFilename(".srm", SRAM_DIR));

	S9xSaveCheatFile(S9xGetFilename(".cht", CHEAT_DIR));
}

void emulator_switch_pal_60hz(bool pal60Hz)
{
	Settings.PS3PALTemporalMode60Hz = pal60Hz;
	ps3graphics_set_pal60hz(Settings.PS3PALTemporalMode60Hz);
	ps3graphics_switch_resolution(ps3graphics_get_current_resolution(), Settings.PS3PALTemporalMode60Hz, Settings.TripleBuffering, Settings.ScaleEnabled, Settings.ScaleFactor);
}

static void ingame_menu_enable (int enable)
{
	is_running = false;
	audio_active = false;
	is_ingame_menu_running = enable;
}

static void emulator_toggle_throttle(bool enable)
{
	char msg[512];

	ps3graphics_set_vsync(enable);
	if(enable)
		snprintf(msg, sizeof(msg), "Throttle mode: ON");
	else
		snprintf(msg, sizeof(msg), "Throttle mode: OFF");

	set_text_message(msg, 60);
}

static void special_actions(uint64_t number)
{
	switch (number)
	{
		case BTN_QUICKSAVE:
			{
				emulator_save_current_save_state_slot();
			}
			break;
		case BTN_QUICKLOAD:
			{
				emulator_load_current_save_state_slot();
			}
			break;
		case BTN_INCREMENTSAVE:
			{
				emulator_increment_current_save_state_slot();
			}
			break;
		case BTN_DECREMENTSAVE:
			{
				emulator_decrement_current_save_state_slot();
			}
			break;
		case BTN_INCREMENTCHEAT:
			{
				char msg[512];
				Settings.CurrentCheatPosition++;
				snprintf(msg, sizeof(msg), "Cheat pos. changed to: %d", Settings.CurrentSaveStateSlot);
				set_text_message(msg, 60);
			}
			break;
		case BTN_DECREMENTCHEAT:
			{
				char msg[512];
				if(Settings.CurrentCheatPosition > 0)
					Settings.CurrentCheatPosition--;
				snprintf(msg, sizeof(msg), "Cheat pos. changed to: %d", Settings.CurrentSaveStateSlot);
				set_text_message(msg, 60);
			}
			break;
		case BTN_EXITTOMENU:
		{
			if(frame_count < special_action_msg_expired)
			{
			}
			else
			{
				ingame_menu_enable(0);
				mode_switch = MODE_MENU;
				set_text_message("", 15);
			}
			break;
		}
		case BTN_CHEATTOGGLE:
			{
				char msg[512];

				if(S9xCheatEnabled(Settings.CurrentCheatPosition))
					S9xDisableCheat(Settings.CurrentCheatPosition);
				else
					S9xEnableCheat(Settings.CurrentCheatPosition);

				Cheat.c[Settings.CurrentCheatPosition].saved = TRUE;
				S9xApplyCheats();

				sprintf(msg, "%s cheat: %d", S9xCheatEnabled(Settings.CurrentCheatPosition) ? "Enabled" : "Disabled", Settings.CurrentCheatPosition);
				set_text_message(msg, 60);
			}
			break;
		case BTN_SOFTRESET:
			S9xSoftReset();
			break;
		case BTN_RESET:
			S9xReset();
			break;
		case BTN_FASTFORWARD:
			{
				if(frame_count < special_action_msg_expired)
				{
				}
				else
				{
					Settings.Throttled = !Settings.Throttled;
					emulator_toggle_throttle(Settings.Throttled);
				}
			}
			break;
		case BTN_SRAM_WRITEPROTECT:
			{
				if(frame_count < special_action_msg_expired)
				{
				}
				else
				{
					char msg[512];
					Settings.SRAMWriteProtect = !Settings.SRAMWriteProtect;
					snprintf(msg, sizeof(msg), "SRAM write protect turned %s", Settings.SRAMWriteProtect ? "on" : "off");
					set_text_message(msg, 60);
				}
			}
			break;
		case BTN_CHEATINPUT:
			{
				char msg[512];
				int ret;
				ret = emulator_input_cheat();

				switch(ret)
				{
					case GAME_GENIE_CODE_ADDED:
						strncpy(msg, "Game Genie code added.", sizeof(msg));	
						break;
					case PRO_ACTION_REPLAY_CODE_ADDED:
						strncpy(msg, "Pro Action Replay code added.", sizeof(msg));
						break;
					case GOLD_FINGER_CODE_ADDED:
						strncpy(msg, "Gold Finger code added.", sizeof(msg));
						break;
					case CODE_ADDED_INCORRECT:
						strncpy(msg, "ERROR: Code entered was incorrect.", sizeof(msg));
						break;
					case CODE_INPUT_CANCELLED:
						strncpy(msg, "ERROR: Code input cancelled.", sizeof(msg));
						break;
				}
				set_text_message(msg, 60);
			}
			break;
		case BTN_INGAME_MENU:
			if(frame_count < special_action_msg_expired)
			{
			}
			else
			{
				ingame_menu_enable(1);
			}
			break;
		default:
			break;
	}
}

static void emulator_implementation_input_loop_mouse(unsigned port, unsigned snes_device)
{
	const uint32_t pads_connected = cell_pad_input_pads_connected();

	static uint64_t old_state;
	uint64_t pad;
	uint64_t state;
	if(snes_device == CTL_SUPERSCOPE)
	{
		state = cell_pad_input_poll_device(0);
		pad = 1;
		port = 0;
	}
	else
	{
		state = cell_pad_input_poll_device(port);
		pad = port + 1;
	}

	const uint64_t button_was_pressed = old_state & (old_state ^ state);
	const uint64_t button_was_held = old_state & state;
	const uint64_t button_was_not_held = ~(old_state & state);
	const uint64_t button_was_not_pressed = ~(state);
	uint64_t special_action_to_execute = 0;
	static uint8_t old_mouse_buttons;
	static int mouse_old_x;
	static int mouse_old_y;

	static bool first = true;
	static int _x = 128;
	static int _y = 128;

	//USB/Bluetooth mouse
	if(Settings.AccessoryType)
	{
		const CellMouseData mouse_state = cell_mouse_input_poll_device(port);
		const uint64_t new_state_mouse_button_1 = (mouse_state.buttons & CELL_MOUSE_BUTTON_1);
		const uint64_t new_state_mouse_button_2 = (mouse_state.buttons & CELL_MOUSE_BUTTON_2);
		const uint64_t new_state_mouse_button_3 = (mouse_state.buttons & CELL_MOUSE_BUTTON_3);
		const uint64_t new_state_mouse_button_4 = (mouse_state.buttons & CELL_MOUSE_BUTTON_4);
		const uint64_t new_state_mouse_button_5 = (mouse_state.buttons & CELL_MOUSE_BUTTON_5);
		int16_t used_mouse_button_1;
		int16_t used_mouse_button_2;
		int16_t used_mouse_button_3;
		int16_t used_mouse_button_4;
		int16_t used_mouse_button_5;

		if(mouse_state.update == CELL_MOUSE_DATA_UPDATE)
		{
			used_mouse_button_1 = new_state_mouse_button_1;
			used_mouse_button_2 = new_state_mouse_button_2;
			used_mouse_button_3 = new_state_mouse_button_3;
			used_mouse_button_4 = new_state_mouse_button_4;
			used_mouse_button_5 = new_state_mouse_button_5;
			old_mouse_buttons = mouse_state.buttons;

			if(snes_device == CTL_SUPERSCOPE)
			{
				mouse_old_x += mouse_state.x_axis;
				mouse_old_y += mouse_state.y_axis;

				if(mouse_old_x > 258)
					mouse_old_x = 258;

				if(mouse_old_x < -3)
					mouse_old_x = -3;

				if(mouse_old_y > 239)
					mouse_old_y = 239;

				if(mouse_old_y < -3)
					mouse_old_y = -3;
			}
			else
			{
				mouse_old_x += mouse_state.x_axis;
				mouse_old_y += mouse_state.y_axis;
			}

			S9xApplyCommand(keymap[BTN_POINTER1], mouse_old_x, mouse_old_y);
			S9xApplyCommand(keymap[BTN_SCOPE_POINTER], mouse_old_x, mouse_old_y);
		}
		else
		{
			used_mouse_button_1 = old_mouse_buttons & CELL_MOUSE_BUTTON_1;
			used_mouse_button_2 = old_mouse_buttons & CELL_MOUSE_BUTTON_2;
			used_mouse_button_3 = old_mouse_buttons & CELL_MOUSE_BUTTON_3;
			used_mouse_button_4 = old_mouse_buttons & CELL_MOUSE_BUTTON_4;
			used_mouse_button_5 = old_mouse_buttons & CELL_MOUSE_BUTTON_5;
		}

		//five mouse buttons hooked up - report them
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_SQUARE_DEF])], (used_mouse_button_1), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_TRIANGLE_DEF])], (used_mouse_button_2), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_CIRCLE_DEF])], (used_mouse_button_3), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_CROSS_DEF])], (used_mouse_button_4), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_START_DEF])], (used_mouse_button_5), 0);

	}
	else	//Mapped to joystick analog
	{
		int16 x = CTRL_AXIS_LSTICK_X(state);
		int16 y = CTRL_AXIS_LSTICK_Y(state);

		if(first)
		{
			x = 128;
			y = 128;
			first = false;
		}
		else
		{
			_x += (x - 128) / 15;
			_y += (y - 128) / 15;
			if(snes_device == CTL_SUPERSCOPE)
			{
				//Super Scope
				if(_x > 277)
					_x = 255;
				else if(_x < -16)
					_x = 0;

				if(_y > 256)
					_y = 255;
				else if(_y < -16)
					_y = 0;
			}
			x = _x;
			y = _y;
		}
		S9xApplyCommand(keymap[BTN_POINTER1], x, y);
		S9xApplyCommand(keymap[BTN_SCOPE_POINTER], x, y);
		//five mouse buttons hooked up - report them
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_SQUARE_DEF])], (int16)(CTRL_SQUARE(state)), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_TRIANGLE_DEF])], (int16)(CTRL_TRIANGLE(state)), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_CIRCLE_DEF])], (int16)(CTRL_CIRCLE(state)), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_CROSS_DEF])], (int16)(CTRL_CROSS(state)), 0);
		S9xApplyCommand(keymap[MAKE_BUTTON(pad, control_binds[port][CTRL_START_DEF])], (int16)(CTRL_START(state)), 0);

	}

	if(snes_device == CTL_MOUSE)
	{
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_CIRCLE_DEF]), control_binds[0][CTRL_CIRCLE_DEF], (CTRL_CIRCLE(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_CROSS_DEF]), control_binds[0][CTRL_CROSS_DEF], (CTRL_CROSS(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_TRIANGLE_DEF]), control_binds[0][CTRL_TRIANGLE_DEF], (CTRL_TRIANGLE(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_SQUARE_DEF]), control_binds[0][CTRL_SQUARE_DEF], (CTRL_SQUARE(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_SELECT_DEF]), control_binds[0][CTRL_SELECT_DEF], (CTRL_SELECT(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_START_DEF]), control_binds[0][CTRL_START_DEF], (CTRL_START(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L1_DEF]), control_binds[0][CTRL_L1_DEF], (CTRL_L1(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_DEF]), control_binds[0][CTRL_L2_DEF], (CTRL_L2(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R1_DEF]), control_binds[0][CTRL_R1_DEF], (CTRL_R1(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_DEF]), control_binds[0][CTRL_R2_DEF], (CTRL_R2(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_UP_DEF]), control_binds[0][CTRL_UP_DEF], (CTRL_UP(state) | CTRL_LSTICK_UP(state)) != 0);
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_DOWN_DEF]), control_binds[0][CTRL_DOWN_DEF], (CTRL_DOWN(state) | CTRL_LSTICK_DOWN(state)) != 0);
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_LEFT_DEF]), control_binds[0][CTRL_LEFT_DEF], (CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state)) != 0);
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_RIGHT_DEF]), control_binds[0][CTRL_RIGHT_DEF], (CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state)) != 0);
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_R3_DEF]), control_binds[0][CTRL_R2_R3_DEF], (CTRL_R2(state) && CTRL_R3(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_R3_DEF]), control_binds[0][CTRL_L2_R3_DEF], (CTRL_L2(state) && CTRL_R3(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_L3_DEF]), control_binds[0][CTRL_L2_L3_DEF], (CTRL_L2(state) && CTRL_L3(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L3_DEF]), control_binds[0][CTRL_L3_DEF], (CTRL_L3(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R3_DEF]), control_binds[0][CTRL_R3_DEF], (CTRL_R3(state)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_RSTICK_RIGHT_DEF]), control_binds[0][CTRL_RSTICK_RIGHT_DEF], CTRL_RSTICK_RIGHT(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_RSTICK_LEFT_DEF]), control_binds[0][CTRL_RSTICK_LEFT_DEF], CTRL_RSTICK_LEFT(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_RSTICK_UP_DEF]), control_binds[0][CTRL_RSTICK_UP_DEF], CTRL_RSTICK_UP(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_RSTICK_DOWN_DEF]), control_binds[0][CTRL_RSTICK_DOWN_DEF], CTRL_RSTICK_DOWN(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_RSTICK_LEFT_DEF]), control_binds[0][CTRL_L2_RSTICK_LEFT_DEF], (CTRL_L2(button_was_held) && CTRL_RSTICK_LEFT(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_RSTICK_UP_DEF]), control_binds[0][CTRL_L2_RSTICK_UP_DEF], (CTRL_L2(state) && CTRL_RSTICK_UP(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_RSTICK_DOWN_DEF]), control_binds[0][CTRL_L2_RSTICK_DOWN_DEF], (CTRL_L2(state) && CTRL_R2(button_was_not_pressed) && CTRL_RSTICK_DOWN(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_L2_RSTICK_RIGHT_DEF]), control_binds[0][CTRL_L2_RSTICK_RIGHT_DEF], (CTRL_L2(state) && CTRL_RSTICK_RIGHT(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_RSTICK_LEFT_DEF]), control_binds[0][CTRL_R2_RSTICK_LEFT_DEF], (CTRL_R2(state) && CTRL_RSTICK_LEFT(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_RSTICK_RIGHT_DEF]), control_binds[0][CTRL_R2_RSTICK_RIGHT_DEF], (CTRL_R2(state) && CTRL_RSTICK_RIGHT(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_RSTICK_UP_DEF]), control_binds[0][CTRL_R2_RSTICK_UP_DEF], (CTRL_R2(state) && CTRL_RSTICK_UP(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R2_RSTICK_DOWN_DEF]), control_binds[0][CTRL_R2_RSTICK_DOWN_DEF], (CTRL_R2(state) && CTRL_L2(button_was_not_pressed) && CTRL_RSTICK_DOWN(button_was_pressed)));
		S9xReportButton(0, MAKE_BUTTON(pad, control_binds[0][CTRL_R3_L3_DEF]), control_binds[0][CTRL_R3_L3_DEF], (CTRL_R3(state) && CTRL_L3(state)));
	}
	old_state = state;

	if(special_action_to_execute)
	{
		special_actions(special_action_to_execute);
	}
}

static void emulator_implementation_input_loop()
{
	static uint64_t old_state[MAX_PADS];
	const uint32_t pads_connected = cell_pad_input_pads_connected();

	for (uint32_t i = 0; i < pads_connected; ++i)
	{
		const uint64_t state = cell_pad_input_poll_device(i);
		const uint64_t button_was_pressed = old_state[i] & (old_state[i] ^ state);
		const uint64_t button_was_held = old_state[i] & state;
		const uint64_t button_was_not_held = ~(old_state[i] & state);
		const uint64_t button_was_not_pressed = ~(state);
		const uint64_t pad = i + 1;
		uint64_t special_action_to_execute = 0;

		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_CIRCLE_DEF]), control_binds[i][CTRL_CIRCLE_DEF], (CTRL_CIRCLE(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_CROSS_DEF]), control_binds[i][CTRL_CROSS_DEF], (CTRL_CROSS(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_TRIANGLE_DEF]), control_binds[i][CTRL_TRIANGLE_DEF], (CTRL_TRIANGLE(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_SQUARE_DEF]), control_binds[i][CTRL_SQUARE_DEF], (CTRL_SQUARE(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_SELECT_DEF]), control_binds[i][CTRL_SELECT_DEF], (CTRL_SELECT(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_START_DEF]), control_binds[i][CTRL_START_DEF], (CTRL_START(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L1_DEF]), control_binds[i][CTRL_L1_DEF], (CTRL_L1(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_DEF]), control_binds[i][CTRL_L2_DEF], (CTRL_L2(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R1_DEF]), control_binds[i][CTRL_R1_DEF], (CTRL_R1(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_DEF]), control_binds[i][CTRL_R2_DEF], (CTRL_R2(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_UP_DEF]), control_binds[i][CTRL_UP_DEF], (CTRL_UP(state) | CTRL_LSTICK_UP(state)) != 0);
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_DOWN_DEF]), control_binds[i][CTRL_DOWN_DEF], (CTRL_DOWN(state) | CTRL_LSTICK_DOWN(state)) != 0);
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_LEFT_DEF]), control_binds[i][CTRL_LEFT_DEF], (CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state)) != 0);
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_RIGHT_DEF]), control_binds[i][CTRL_RIGHT_DEF], (CTRL_RIGHT(state) | CTRL_LSTICK_RIGHT(state)) != 0);
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_R3_DEF]), control_binds[i][CTRL_R2_R3_DEF], (CTRL_R2(state) && CTRL_R3(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_R3_DEF]), control_binds[i][CTRL_L2_R3_DEF], (CTRL_L2(state) && CTRL_R3(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_L3_DEF]), control_binds[i][CTRL_L2_L3_DEF], (CTRL_L2(state) && CTRL_L3(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L3_DEF]), control_binds[i][CTRL_L3_DEF], (CTRL_L3(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R3_DEF]), control_binds[i][CTRL_R3_DEF], (CTRL_R3(state)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_RSTICK_RIGHT_DEF]), control_binds[i][CTRL_RSTICK_RIGHT_DEF], CTRL_RSTICK_RIGHT(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_RSTICK_LEFT_DEF]), control_binds[i][CTRL_RSTICK_LEFT_DEF], CTRL_RSTICK_LEFT(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_RSTICK_UP_DEF]), control_binds[i][CTRL_RSTICK_UP_DEF], CTRL_RSTICK_UP(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_RSTICK_DOWN_DEF]), control_binds[i][CTRL_RSTICK_DOWN_DEF], CTRL_RSTICK_DOWN(state) && CTRL_R2(button_was_not_held) && CTRL_L2(button_was_not_held));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_RSTICK_LEFT_DEF]), control_binds[i][CTRL_L2_RSTICK_LEFT_DEF], (CTRL_L2(button_was_held) && CTRL_RSTICK_LEFT(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_RSTICK_UP_DEF]), control_binds[i][CTRL_L2_RSTICK_UP_DEF], (CTRL_L2(state) && CTRL_RSTICK_UP(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_RSTICK_DOWN_DEF]), control_binds[i][CTRL_L2_RSTICK_DOWN_DEF], (CTRL_L2(state) && CTRL_R2(button_was_not_pressed) && CTRL_RSTICK_DOWN(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_L2_RSTICK_RIGHT_DEF]), control_binds[i][CTRL_L2_RSTICK_RIGHT_DEF], (CTRL_L2(state) && CTRL_RSTICK_RIGHT(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_RSTICK_LEFT_DEF]), control_binds[i][CTRL_R2_RSTICK_LEFT_DEF], (CTRL_R2(state) && CTRL_RSTICK_LEFT(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_RSTICK_RIGHT_DEF]), control_binds[i][CTRL_R2_RSTICK_RIGHT_DEF], (CTRL_R2(state) && CTRL_RSTICK_RIGHT(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_RSTICK_UP_DEF]), control_binds[i][CTRL_R2_RSTICK_UP_DEF], (CTRL_R2(state) && CTRL_RSTICK_UP(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R2_RSTICK_DOWN_DEF]), control_binds[i][CTRL_R2_RSTICK_DOWN_DEF], (CTRL_R2(state) && CTRL_L2(button_was_not_pressed) && CTRL_RSTICK_DOWN(button_was_pressed)));
		S9xReportButton(i, MAKE_BUTTON(pad, control_binds[i][CTRL_R3_L3_DEF]), control_binds[i][CTRL_R3_L3_DEF], (CTRL_R3(state) && CTRL_L3(state)));

		if(special_action_to_execute)
		{
			special_actions(special_action_to_execute);
		}
		old_state[i] = state;
	}
}

#define string_concat(padno, string) "Joypad"#padno" "#string""

#define map_snes9x_standard_controls(padno) \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_A), string_concat(padno, A)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_B), string_concat(padno, B)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_X), string_concat(padno, X)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_Y), string_concat(padno, Y)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_SELECT), string_concat(padno, Select)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_START), string_concat(padno, Start)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_L), string_concat(padno,  L)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_R), string_concat(padno, R)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_LEFT), string_concat(padno, Left)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_RIGHT), string_concat(padno, Right)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_UP), string_concat(padno, Up)); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_DOWN), string_concat(padno, Down));

#define map_snes9x_special_controls(padno) \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_QUICKSAVE), "SaveFreezeFile"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_QUICKLOAD), "LoadFreezeFile"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_INCREMENTSAVE), "IncSave"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_DECREMENTSAVE), "DecSave"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_INCREMENTCHEAT), "IncCheat"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_DECREMENTCHEAT), "DecCheat"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_CHEATTOGGLE), "CheatToggle"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_CHEATINPUT), "CheatInput"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_CHEATINPUTLABEL), "CheatInputLabel"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_EXITTOMENU), "ExitEmu"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_PAUSE), "Pause"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_FASTFORWARD), "ToggleEmuTurbo"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_RESET), "Reset"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_SOFTRESET), "SoftReset"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_SRAM_WRITEPROTECT), "SramWriteProtect"); \
   MAP_BUTTON(MAKE_BUTTON(padno, BTN_INGAME_MENU), "ExitToMenu");


static bool emulator_init_system(void)
{
	if(Settings.AccessoryType)
		cell_mouse_input_init();
	else
		cell_mouse_input_deinit();

	sys_lwmutex_lock(&audio_lock, SYS_NO_TIMEOUT);

	if(emulator_initialized)
		Deinit();

	Init();

	if(emulator_initialized)
		S9xDeinitAPU();

	S9xInitAPU();

	S9xInitSound(64, 0);

	S9xSetSamplesAvailableCallback(S9xAudioCallback);
	audio_active = true;
	sys_lwmutex_unlock(&audio_lock);

	if (current_rom == NULL)
		return false; //No ROM to load

	if (!LoadROM(current_rom))
		return false; //Load ROM failed

	if(Settings.CurrentROMisMultitapCompatible)
	{
		if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM)
		{
			dialog_is_running = true;
			cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL|CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_YES, "This game supports the [Multitap] accessory. Do you want to enable [Multitap] support?", callback_multitap_compatible ,NULL,NULL);

			do
			{
				glClear(GL_COLOR_BUFFER_BIT);
				psglSwap();
				cellSysutilCheckCallback();
			}while(dialog_is_running && is_running);

			cellMsgDialogClose(0.0f);
		}
		else if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_ENABLED)
			Emulator_SetControllerMode(MULTITAP);
	}
	else if(Settings.CurrentROMisMouseCompatible)
	{
		if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM)
		{
			char text[256];
			if(Settings.AccessoryType)
				strncpy(text, "This game supports the [SNES Mouse] accessory. Do you want to enable [SNES Mouse] support? (NOTE: [SNES Mouse] will be mapped to the USB/Bluetooth mouse)", sizeof(text));
			else
				strncpy(text, "This game supports the [SNES Mouse] accessory. Do you want to enable [SNES Mouse] support? (NOTE: [SNES Mouse] will be mapped to the left analog stick)", sizeof(text));

			dialog_is_running = true;

			cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL|CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_YES, text, callback_mouse_compatible ,NULL,NULL);
			do
			{
				glClear(GL_COLOR_BUFFER_BIT);
				psglSwap();
				cellSysutilCheckCallback();
			}while(dialog_is_running && is_running);

			cellMsgDialogClose(0.0f);
		}
		else if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_ENABLED)
			Emulator_SetControllerMode(MOUSE);
	}
	else if(Settings.CurrentROMisSuperScopeCompatible)
	{
		if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM)
		{
			char text[256];
			if(Settings.AccessoryType)
				strncpy(text, "This game supports the [Super Scope] accessory. Do you want to enable [Super Scope] support? (NOTE: [Super Scope] will be mapped to the USB/Bluetooth mouse)", sizeof(text));
			else
				strncpy(text, "This game supports the [Super Scope] accessory. Do you want to enable [Super Scope] support? (NOTE: [Super Scope] will be mapped to the left analog stick)", sizeof(text));

			dialog_is_running = true;
		
			cellMsgDialogOpen2(CELL_MSGDIALOG_DIALOG_TYPE_NORMAL|CELL_MSGDIALOG_TYPE_BG_VISIBLE | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_OFF | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_YES, text, callback_superscope_compatible ,NULL,NULL);
			do
			{
				glClear(GL_COLOR_BUFFER_BIT);
				psglSwap();
				cellSysutilCheckCallback();
			}while(dialog_is_running && is_running);

			cellMsgDialogClose(0.0f);
		}
		else if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_ENABLED)
			Emulator_SetControllerMode(SUPERSCOPE);
	}
	else
		Emulator_SetControllerMode(TWO_JOYSTICKS);

	LoadSRAM(S9xGetFilename(".srm", SRAM_DIR));

	GFX.Pitch = 1024;
	GFX.Screen = (uint16_t *)memalign(1024, 489472);

	if(emulator_initialized)
	{
		S9xGraphicsDeinit();
		S9xControlsSoftReset();
	}
	S9xGraphicsInit();


#define MAP_BUTTON(id, name) S9xMapButton((id), S9xGetCommandT((name)), false)
#define MAP_POINTER(id, name) S9xMapPointer((id), S9xGetCommandT((name)), false)

	// Controller initialization
	switch(controller_settings)
	{
		case TWO_JOYSTICKS:
			S9xSetController(0, CTL_JOYPAD,		0,	0,	0,	0);
			S9xSetController(1, CTL_JOYPAD,		1,	0,	0,	0);

			snes_devices[0] = CTL_JOYPAD;
			snes_devices[1] = CTL_JOYPAD;
			break;
		case MULTITAP:
			S9xSetController(0, CTL_JOYPAD,		0,	0,	0,	0);
			S9xSetController(1, CTL_MP5,		1,	2,	3,	4);

			snes_devices[0] = CTL_JOYPAD;
			snes_devices[1] = CTL_MP5;
			break;
		case MOUSE:
			S9xSetController(0, CTL_MOUSE,		0,	0,	0,	0);
			S9xSetController(1, CTL_JOYPAD,		1,	0,	0,	0);

			snes_devices[0] = CTL_MOUSE;
			snes_devices[1] = CTL_JOYPAD;
			break;
		case SUPERSCOPE:
			S9xSetController(0, CTL_JOYPAD,		0,	0,	0,	0);
			S9xSetController(1, CTL_SUPERSCOPE,	0,	0,	0,	0);

			snes_devices[0] = CTL_JOYPAD;
			snes_devices[1] = CTL_SUPERSCOPE;
			break;
		case JUSTIFIER:
			S9xSetController(0, CTL_JOYPAD,		0,	0,	0,	0);
			S9xSetController(1, CTL_JUSTIFIER,	0,	0,	0,	0);

			snes_devices[0] = CTL_JOYPAD;
			snes_devices[1] = CTL_JUSTIFIER;
			break;
		case TWO_JUSTIFIERS:
			break;
		default:
			break;
	}

	S9xUnmapAllControls();

	switch(controller_settings)
	{
		case MULTITAP:
			map_snes9x_standard_controls(PAD_3);      // Player 3
			map_snes9x_standard_controls(PAD_4);      // Player 4
			map_snes9x_standard_controls(PAD_5);      // Player 5

			map_snes9x_special_controls(PAD_3);       
			map_snes9x_special_controls(PAD_4);
			map_snes9x_special_controls(PAD_5);
			//fall-through
		case TWO_JOYSTICKS:
			map_snes9x_standard_controls(PAD_1);      // Player 1
			map_snes9x_standard_controls(PAD_2);      // Player 2
			break;
		case MOUSE:
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_X), "Mouse1 R");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_Y), "Mouse1 L");
			MAP_BUTTON(MAKE_BUTTON(PAD_2, BTN_X), "Mouse2 R");
			MAP_BUTTON(MAKE_BUTTON(PAD_2, BTN_Y), "Mouse2 L");
			MAP_POINTER(BTN_POINTER1, "Pointer Mouse1");
			MAP_POINTER(BTN_POINTER2, "Pointer Mouse2");
			break;
		case SUPERSCOPE:
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_A), "Superscope ToggleTurbo");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_B), "Superscope Cursor");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_X), "Superscope AimOffscreen");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_Y), "Superscope Fire");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_START), "Superscope Pause");
			MAP_BUTTON(MAKE_BUTTON(PAD_2, BTN_X), "Superscope AimOffscreen");
			MAP_BUTTON(MAKE_BUTTON(PAD_2, BTN_Y), "Superscope Fire");
			MAP_BUTTON(MAKE_BUTTON(PAD_1, BTN_START), "Justifier Pause");
			MAP_POINTER(BTN_SCOPE_POINTER, "Pointer Superscope");
			break;
		case JUSTIFIER:
		case TWO_JUSTIFIERS:
		default:
			break;
	}

	map_snes9x_special_controls(PAD_1);
	map_snes9x_special_controls(PAD_2);

	return true;
}

#define init_setting_uint(charstring, setting, defaultvalue) \
	if(!(config_get_uint(currentconfig, charstring, &setting))) \
		setting = defaultvalue; 

#define init_setting_int(charstring, setting, defaultvalue) \
	if(!(config_get_int(currentconfig, charstring, &setting))) \
		setting = defaultvalue; 

#define init_setting_bool(charstring, setting, defaultvalue) \
	if(!(config_get_bool(currentconfig, charstring, &setting))) \
		setting = defaultvalue; 

#define init_setting_bool(charstring, setting, defaultvalue) \
	if(!(config_get_bool(currentconfig, charstring, &setting))) \
		setting =	defaultvalue;

#define init_setting_char(charstring, setting, defaultvalue) \
	if(!(config_get_char_array(currentconfig, charstring, setting, sizeof(setting)))) \
		strncpy(setting,defaultvalue, sizeof(setting));

static bool file_exists(const char * filename)
{
	CellFsStat sb;
	if(cellFsStat(filename,&sb) == CELL_FS_SUCCEEDED)
		return true;
	else
		return false;
}

static void emulator_init_settings(void)
{
	bool config_file_newly_created = false;
	memset(&Settings, 0, sizeof(Settings));

	if(!file_exists(SYS_CONFIG_FILE))
	{
		FILE * f;
		f = fopen(SYS_CONFIG_FILE, "w");
		fclose(f);
		config_file_newly_created = true;
	}

	config_file_t * currentconfig = config_file_new(SYS_CONFIG_FILE);

	/* emulator-specific settings */

	Settings.Crosshair = 1;
#if 0
	Settings.CartAName[0] = 0;
	Settings.CartBName[0] = 0;
#endif

	// ROM

	init_setting_bool("ROM::Interleaved2", Settings.ForceInterleaved2, false);
	init_setting_bool("ROM::InterleaveGD24", Settings.ForceInterleaveGD24, false);
	init_setting_uint("ROM::Cheat", Settings.ApplyCheats, false);
	init_setting_uint("ROM::NoPatch", Settings.NoPatch, false);

	init_setting_bool("ROM::LoROM", Settings.ForceLoROM, false);
	init_setting_bool("ROM::HiROM", Settings.ForceHiROM, false);
	if (Settings.ForceLoROM)
	{
		Settings.ForceHiROM = false;
	}

	init_setting_uint("ROM::PAL", Settings.ForcePAL,  0);
	init_setting_uint("ROM::NTSC", Settings.ForceNTSC, 0);

	if (Settings.ForcePAL)
	{
		Settings.ForceNTSC = false;
	}

	init_setting_bool("ROM::Header", Settings.ForceHeader, false);
	Settings.ForceNoHeader = !Settings.ForceHeader;

	init_setting_bool("ROM::Interleaved", Settings.ForceInterleaved, false);
	Settings.ForceNotInterleaved = !Settings.ForceInterleaved;

	// Settings

	init_setting_bool("Settings::BSXBootup", Settings.BSXBootup, false);

	/*
	   if (conf.Exists("BSX::CartAName"))
	   {
	   Settings.CartAName[0]			= conf.GetString("BSX::CartAName");
	   }
	   if (conf.Exists("BSX::CartBName"))
	   {
	   Settings.CartBName[0]			= conf.GetString("BSX::CartBName");
	   }
	 */

	init_setting_uint("Settings::FrameTime", Settings.FrameTimePAL, 1667);
	init_setting_uint("Settings::FrameTime", Settings.FrameTimeNTSC, 1667);

	// Hack

	init_setting_bool("Hack::DisableGameSpecificHacks", Settings.DisableGameSpecificHacks, false);
	init_setting_bool("Hack::BlockInvalidVRAMAccess", Settings.BlockInvalidVRAMAccessMaster, true)
	init_setting_int("Hack::HDMATiming", Settings.HDMATimingHack, 100);

	char tmp_str[512];

	//Some additional settings that are not properly set by SNES9x's LoadConfigFiles
	init_setting_uint("Settings::FrameTimePAL", Settings.FrameTimePAL, 20000);

	//PS3 - General Settings
	init_setting_uint("PS3General::ApplyShaderPresetOnStartup", Settings.ApplyShaderPresetOnStartup, 0);
	init_setting_uint("PS3General::KeepAspect", Settings.PS3KeepAspect, ASPECT_RATIO_4_3);
	init_setting_uint("PS3General::Smooth", Settings.PS3Smooth, 1);
	init_setting_uint("PS3General::Smooth2", Settings.PS3Smooth2, 1);
	init_setting_char("PS3General::PS3CurrentShader", Settings.PS3CurrentShader, DEFAULT_SHADER_FILE);
	init_setting_char("PS3General::PS3CurrentShader2", Settings.PS3CurrentShader2, DEFAULT_SHADER_FILE);
	init_setting_char("PS3General::Border", Settings.PS3CurrentBorder, DEFAULT_BORDER_FILE);
	init_setting_uint("PS3General::PS3TripleBuffering",Settings.TripleBuffering, 1);
	init_setting_char("PS3General::ShaderPresetPath", Settings.ShaderPresetPath, "");
	init_setting_char("PS3General::ShaderPresetTitle", Settings.ShaderPresetTitle, "None");
	init_setting_uint("PS3General::ScaleFactor", Settings.ScaleFactor, 2);
	init_setting_uint("PS3General::ViewportX", Settings.ViewportX, 0);
	init_setting_uint("PS3General::ViewportY", Settings.ViewportY, 0);
	init_setting_uint("PS3General::ViewportWidth", Settings.ViewportWidth, 0);
	init_setting_uint("PS3General::ViewportHeight", Settings.ViewportHeight, 0);
	init_setting_uint("PS3General::ScaleEnabled", Settings.ScaleEnabled, 1);
	init_setting_uint("PS3General::PS3CurrentResolution", Settings.PS3CurrentResolution, NULL);
	init_setting_uint("PS3General::OverscanEnabled", Settings.PS3OverscanEnabled, 0);
	init_setting_int("PS3General::OverscanAmount", Settings.PS3OverscanAmount, 0);
	init_setting_uint("PS3General::PS3PALTemporalMode60Hz", Settings.PS3PALTemporalMode60Hz, 0);
	init_setting_uint("Sound::SoundMode", Settings.SoundMode, SOUND_MODE_NORMAL);
	init_setting_char("RSound::RSoundServerIPAddress",  Settings.RSoundServerIPAddress, "0.0.0.0");
	init_setting_uint("PS3General::Throttled", Settings.Throttled, 1);
	init_setting_uint("PS3General::PS3FontSize", Settings.PS3FontSize, 100);

	if (config_get_char_array(currentconfig,"PS3Paths::PathSaveStates", tmp_str, sizeof(tmp_str)))
		config_get_char_array(currentconfig, "PS3Paths::PathSaveStates", Settings.PS3PathSaveStates, sizeof(Settings.PS3PathSaveStates));
	else
		strcpy(Settings.PS3PathSaveStates, usrDirPath);

	if (config_get_char_array(currentconfig,"PS3Paths::PathCheats", tmp_str, sizeof(tmp_str)))
		config_get_char_array(currentconfig, "PS3Paths::PathCheats", Settings.PS3PathCheats, sizeof(Settings.PS3PathCheats));
	else
		strncpy(Settings.PS3PathCheats, usrDirPath, sizeof(Settings.PS3PathCheats));

	init_setting_char("PS3Paths::PathSRAM", Settings.PS3PathSRAM, usrDirPath);
	init_setting_char("PS3Paths::PathROMDirectory", Settings.PS3PathROMDirectory, "/");
	init_setting_int("PS3General::ControlScheme", Settings.ControlScheme, CONTROL_SCHEME_DEFAULT);
	init_setting_uint("PS3General::CurrentSaveStateSlot",  Settings.CurrentSaveStateSlot, 0);
	init_setting_uint("PS3General::ScreenshotsEnabled", Settings.ScreenshotsEnabled, 0);
	if (config_get_char_array(currentconfig,"PS3General::GameAwareShaderPath", tmp_str, sizeof(tmp_str)))
		config_get_char_array(currentconfig, "PS3General::GameAwareShaderPath", Settings.GameAwareShaderPath, sizeof(Settings.GameAwareShaderPath));

	//Emulator-specific Settings
	init_setting_uint("PS3SNES9x::ResetBeforeRecordingMovie", Settings.ResetBeforeRecordingMovie, 0);
	init_setting_int("PS3SNES9x::AccessoryAutoDetection", Settings.AccessoryAutoDetection, ACCESSORY_AUTODETECTION_CONFIRM);
	init_setting_uint("PS3SNES9x::AccessoryType", Settings.AccessoryType, 0);
	init_setting_uint("PS3SNES9x::SRAMWriteProtect", Settings.SRAMWriteProtect, 0);
	init_setting_uint("Sound::Rate", Settings.SoundPlaybackRate, 48000);
	init_setting_uint("Sound::InputRate", Settings.SoundInputRate, 31950);

	if(config_file_newly_created)
		emulator_set_controls(SYS_CONFIG_FILE, SET_ALL_CONTROLS_TO_DEFAULT, "Default");
	else
		emulator_set_controls(SYS_CONFIG_FILE, READ_CONTROLS, "Default");
}

void emulator_implementation_set_texture(const char * fname)
{
	strcpy(Settings.PS3CurrentBorder, fname);
	ps3graphics_load_menu_texture(TEXTURE_BACKDROP, fname);
	ps3graphics_load_menu_texture(TEXTURE_MENU, DEFAULT_MENU_BORDER_FILE);
}


void emulator_implementation_set_shader_preset(const char * fname)
{
	config_file_t * currentconfig = config_file_new(fname);

	init_setting_uint("ScaleFactor", Settings.ScaleFactor, Settings.ScaleFactor);
	init_setting_uint("Smooth", Settings.PS3Smooth, Settings.PS3Smooth);
	init_setting_uint("Smooth2", Settings.PS3Smooth2, Settings.PS3Smooth2);
	init_setting_uint("ScaleEnabled", Settings.ScaleEnabled, Settings.ScaleEnabled);
	init_setting_char("PS3CurrentShader", Settings.PS3CurrentShader, DEFAULT_SHADER_FILE);
	init_setting_char("PS3CurrentShader2", Settings.PS3CurrentShader2, DEFAULT_SHADER_FILE);
	init_setting_char("Border", Settings.PS3CurrentBorder, DEFAULT_BORDER_FILE);

	strncpy(Settings.ShaderPresetPath, fname, sizeof(Settings.ShaderPresetPath));
	init_setting_char("ShaderPresetTitle", Settings.ShaderPresetTitle, "None");
	init_setting_uint("KeepAspect", Settings.PS3KeepAspect, Settings.PS3KeepAspect);
	init_setting_uint("OverscanEnabled", Settings.PS3OverscanEnabled, Settings.PS3OverscanEnabled);
	init_setting_int("OverscanAmount", Settings.PS3OverscanAmount, Settings.PS3OverscanAmount);
	init_setting_uint("ViewportX", Settings.ViewportX, Settings.ViewportX);
	init_setting_uint("ViewportY", Settings.ViewportY, Settings.ViewportY);
	init_setting_uint("ViewportWidth", Settings.ViewportWidth, Settings.ViewportWidth);
	init_setting_uint("ViewportHeight", Settings.ViewportHeight, Settings.ViewportHeight);
	ps3graphics_load_fragment_shader(Settings.PS3CurrentShader, 0);
	ps3graphics_load_fragment_shader(Settings.PS3CurrentShader2, 1);
	ps3graphics_set_fbo_scale(Settings.ScaleEnabled,Settings.ScaleFactor);
	ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, SCREEN_RENDER_TEXTURE_WIDTH, SCREEN_RENDER_TEXTURE_HEIGHT, 1);
	ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
	ps3graphics_set_smooth(Settings.PS3Smooth, 0);
	ps3graphics_set_smooth(Settings.PS3Smooth2, 1);
}

void emulator_implementation_set_gameaware(const char * fname)
{
	ps3graphics_init_state_uniforms(fname);
	strcpy(Settings.PS3CurrentShader, ps3graphics_get_fragment_shader_path(0));
	strcpy(Settings.PS3CurrentShader2, ps3graphics_get_fragment_shader_path(1));
}

static void map_ps3_standard_controls(const char * config_file)
{
	char filetitle_tmp[512];
	char string_tmp[256];
	config_file_t * currentconfig = config_file_new(config_file);
	for(uint32_t i = 0; i < MAX_PADS; i++)
	{
		for(uint32_t j = 0; j < BTN_DEF_MAX; j++)
		{
			snprintf(string_tmp, sizeof(string_tmp), "PS3Player%d::%d", i, j);
			config_set_uint(currentconfig, string_tmp,control_binds[i][j]);
		}
	}
	config_set_string(currentconfig, "InputPresetTitle", filetitle_tmp);
	config_file_write(currentconfig, config_file);
}

static void get_ps3_standard_controls(const char * config_file)
{
	config_file_t * currentconfig = config_file_new(config_file);
	char string_tmp[256];

	for(uint32_t i = 0; i < MAX_PADS; i++)
	{
		for(uint32_t j = 0; j < BTN_DEF_MAX; j++)
		{
			snprintf(string_tmp, sizeof(string_tmp), "PS3Player%d::%d", i, j);
			init_setting_uint(string_tmp, control_binds[i][j], default_control_binds[j]);
		}
	}

	init_setting_char("InputPresetTitle", Settings.PS3CurrentInputPresetTitle, "Default");
}

uint32_t default_control_binds[] = {
	BTN_UP,				// CTRL_UP_DEF
	BTN_DOWN,			// CTRL_DOWN_DEF
	BTN_LEFT,			// CTRL_LEFT_DEF
	BTN_RIGHT,			// CTRL_RIGHT_DEF
	BTN_A,				// CTRL_CIRCLE_DEF
	BTN_B,				// CTRL_CROSS_DEF
	BTN_X,				// CTRL_TRIANGLE_DEF
	BTN_Y,				// CTRL_SQUARE_DEF
	BTN_SELECT,			// CTRL_SELECT_DEF
	BTN_START,			// CTRL_START_DEF
	BTN_L,				// CTRL_L1_DEF
	BTN_R,				// CTRL_R1_DEF
	BTN_NONE,			// CTRL_L2_DEF
	BTN_NONE,			// CTRL_R2_DEF
	BTN_NONE,			// CTRL_L3_DEF
	BTN_INGAME_MENU,		// CTRL_R3_DEF
	BTN_NONE,			// CTRL_L2_L3_DEF
	BTN_NONE,			// CTRL_L2_R3_DEF
	BTN_INCREMENTCHEAT,		// CTRL_L2_RSTICK_RIGHT_DEF
	BTN_DECREMENTCHEAT,		// CTRL_L2_RSTICK_LEFT_DEF
	BTN_CHEATTOGGLE,		// CTRL_L2_RSTICK_UP_DEF
	BTN_CHEATINPUT,			// CTRL_L2_RSTICK_DOWN_DEF
	BTN_INCREMENTSAVE,		// CTRL_R2_RSTICK_RIGHT_DEF
	BTN_DECREMENTSAVE,		// CTRL_R2_RSTICK_LEFT_DEF
	BTN_QUICKLOAD,			// CTRL_R2_RSTICK_UP_DEF
	BTN_QUICKSAVE,			// CTRL_R2_RSTICK_DOWN_DEF
	BTN_NONE,			// CTRL_R2_R3_DEF
	BTN_EXITTOMENU,			// CTRL_R3_L3_DEF
	BTN_SRAM_WRITEPROTECT,		// CTRL_RSTICK_UP_DEF
	BTN_FASTFORWARD,		// CTRL_RSTICK_DOWN_DEF
	BTN_NONE,			// CTRL_RSTICK_LEFT_DEF
	BTN_NONE			// CTRL_RSTICK_RIGHT_DEF
};

static void map_ps3_button_array(void)
{
	for(int i = 0; i < MAX_PADS; i++)
		for(uint32_t j = 0; j < BTN_DEF_MAX; j++)
			Input_MapButton(control_binds[i][j],false,default_control_binds[j]);
}
   

void emulator_set_controls(const char * config_file, int mapping_enum, const char * title)
{
	switch(mapping_enum)
	{
		case WRITE_CONTROLS:
		{
			map_ps3_standard_controls(config_file);
			break;
		}
		case READ_CONTROLS:
		{
			get_ps3_standard_controls(config_file);
			break;
		}
		case SET_ALL_CONTROLS_TO_DEFAULT:
		{
			map_ps3_button_array();
			break;
		}
	}
}

void emulator_save_settings(uint64_t filetosave)
{
	config_file_t * currentconfig = config_file_new(SYS_CONFIG_FILE);
	char filepath[MAX_PATH_LENGTH];

	switch(filetosave)
	{
		case CONFIG_FILE:
			strcpy(filepath, SYS_CONFIG_FILE);

			config_set_uint(currentconfig, "PS3General::ControlScheme",Settings.ControlScheme);
			config_set_uint(currentconfig, "PS3General::CurrentSaveStateSlot",Settings.CurrentSaveStateSlot);
			config_set_uint(currentconfig, "PS3General::KeepAspect",Settings.PS3KeepAspect);
			config_set_uint(currentconfig, "PS3General::ApplyShaderPresetOnStartup", Settings.ApplyShaderPresetOnStartup);
			config_set_uint(currentconfig, "PS3General::ViewportX", ps3graphics_get_viewport_x());
			config_set_uint(currentconfig, "PS3General::ViewportY", ps3graphics_get_viewport_y());
			config_set_uint(currentconfig, "PS3General::ViewportWidth", ps3graphics_get_viewport_width());
			config_set_uint(currentconfig, "PS3General::ViewportHeight", ps3graphics_get_viewport_height());
			config_set_uint(currentconfig, "PS3General::Smooth", Settings.PS3Smooth);
			config_set_uint(currentconfig, "PS3General::Smooth2", Settings.PS3Smooth2);
			config_set_uint(currentconfig, "PS3General::ScaleFactor", Settings.ScaleFactor);
			config_set_uint(currentconfig, "PS3General::OverscanEnabled", Settings.PS3OverscanEnabled);
			config_set_uint(currentconfig, "PS3General::OverscanAmount",Settings.PS3OverscanAmount);
			config_set_uint(currentconfig, "PS3General::PS3FontSize",Settings.PS3FontSize);
			config_set_uint(currentconfig, "PS3General::Throttled",Settings.Throttled);
			config_set_uint(currentconfig, "PS3General::PS3PALTemporalMode60Hz",Settings.PS3PALTemporalMode60Hz);
			config_set_uint(currentconfig, "PS3General::PS3TripleBuffering",Settings.TripleBuffering);
			config_set_uint(currentconfig, "PS3General::ScreenshotsEnabled",Settings.ScreenshotsEnabled);
			config_set_uint(currentconfig, "Sound::SoundMode",Settings.SoundMode);
			config_set_uint(currentconfig, "PS3General::PS3CurrentResolution",ps3graphics_get_current_resolution());
			config_set_string(currentconfig, "PS3General::ShaderPresetPath", Settings.ShaderPresetPath);
			config_set_string(currentconfig, "PS3General::ShaderPresetTitle", Settings.ShaderPresetTitle);
			config_set_string(currentconfig, "PS3General::PS3CurrentShader",ps3graphics_get_fragment_shader_path(0));
			config_set_string(currentconfig, "PS3General::PS3CurrentShader2", ps3graphics_get_fragment_shader_path(1));
			config_set_string(currentconfig, "PS3General::Border", Settings.PS3CurrentBorder);
			config_set_string(currentconfig, "PS3General::GameAwareShaderPath", Settings.GameAwareShaderPath);
			config_set_string(currentconfig, "PS3Paths::PathSaveStates", Settings.PS3PathSaveStates);
			config_set_string(currentconfig, "PS3Paths::PathSRAM", Settings.PS3PathSRAM);
			config_set_string(currentconfig, "PS3Paths::PathROMDirectory", Settings.PS3PathROMDirectory);
			config_set_string(currentconfig, "PS3Paths::PathCheats", Settings.PS3PathCheats);
			config_set_string(currentconfig, "RSound::RSoundServerIPAddress", Settings.RSoundServerIPAddress); config_set_uint(currentconfig, "PS3General::ScaleEnabled", Settings.ScaleEnabled);

			//Emulator-specific settings
			config_set_uint(currentconfig, "Sound::InputRate",Settings.SoundInputRate);
			config_set_uint(currentconfig, "PS3SNES9x::ResetBeforeRecordingMovie",Settings.ResetBeforeRecordingMovie);
			config_set_uint(currentconfig, "PS3SNES9x::AccessoryAutoDetection",Settings.AccessoryAutoDetection);
			config_set_uint(currentconfig, "PS3SNES9x::AccessoryType",Settings.AccessoryType);
			config_set_uint(currentconfig, "PS3SNES9x::SRAMWriteProtect",Settings.SRAMWriteProtect);
			config_set_uint(currentconfig, "ROM::PAL",Settings.ForcePAL);
			config_set_uint(currentconfig, "ROM::NTSC",Settings.ForceNTSC);
			config_set_uint(currentconfig, "ROM::Cheat",Settings.ApplyCheats);
			config_set_uint(currentconfig, "ROM::Patch", !Settings.NoPatch);
			config_set_bool(currentconfig, "Settings::BSXBootup",Settings.BSXBootup);

			config_file_write(currentconfig, filepath);
			emulator_set_controls(filepath, WRITE_CONTROLS, "Default");
			break;
		case SHADER_PRESET_FILE:
			{
				bool filename_entered = false;
				char filename_tmp[256], filetitle_tmp[256];
				oskutil_write_initial_message(&oskutil_handle, L"example");
				oskutil_write_message(&oskutil_handle, L"Enter filename for preset (with no file extension)");
				oskutil_start(&oskutil_handle);

				while(OSK_IS_RUNNING(oskutil_handle))
				{
					/* OSK Util gets updated */
					glClear(GL_COLOR_BUFFER_BIT);
					ps3graphics_draw_menu();
					psglSwap();
					cell_console_poll();
					cellSysutilCheckCallback();
				}

				if(oskutil_handle.text_can_be_fetched)
				{
					strncpy(filename_tmp, OUTPUT_TEXT_STRING(oskutil_handle), sizeof(filename_tmp));
					snprintf(filepath, sizeof(filepath), "%s/%s.conf", PRESETS_DIR_PATH, filename_tmp);
					filename_entered = true;
				}

				if(filename_entered)
				{

					oskutil_write_initial_message(&oskutil_handle, L"Example file title");
					oskutil_write_message(&oskutil_handle, L"Enter title for preset");
					oskutil_start(&oskutil_handle);

					while(OSK_IS_RUNNING(oskutil_handle))
					{
						/* OSK Util gets updated */
						glClear(GL_COLOR_BUFFER_BIT);
						ps3graphics_draw_menu();
						psglSwap();
						cell_console_poll();
						cellSysutilCheckCallback();
					}

					if(oskutil_handle.text_can_be_fetched)
						snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", OUTPUT_TEXT_STRING(oskutil_handle));
					else
						snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", filename_tmp);

					if(!file_exists(filepath))
					{
						FILE * f = fopen(filepath, "w");
						fclose(f);
					}

					currentconfig = config_file_new(filepath);

					config_set_string(currentconfig, "PS3CurrentShader", Settings.PS3CurrentShader);
					config_set_string(currentconfig, "PS3CurrentShader2", Settings.PS3CurrentShader2);
					config_set_string(currentconfig, "Border", Settings.PS3CurrentBorder);
					config_set_uint(currentconfig, "Smooth", Settings.PS3Smooth);
					config_set_uint(currentconfig, "Smooth2", Settings.PS3Smooth2);
					config_set_string(currentconfig, "ShaderPresetTitle", filetitle_tmp);
					config_set_uint(currentconfig, "ViewportX", ps3graphics_get_viewport_x());
					config_set_uint(currentconfig, "ViewportY", ps3graphics_get_viewport_y());
					config_set_uint(currentconfig, "ViewportWidth", ps3graphics_get_viewport_width());
					config_set_uint(currentconfig, "ViewportHeight", ps3graphics_get_viewport_height());
					config_set_uint(currentconfig, "ScaleFactor", Settings.ScaleFactor);
					config_set_uint(currentconfig, "ScaleEnabled", Settings.ScaleEnabled);
					config_set_uint(currentconfig, "OverscanEnabled", Settings.PS3OverscanEnabled);
					config_set_uint(currentconfig, "OverscanAmount", Settings.PS3OverscanAmount);
					config_file_write(currentconfig, filepath);
				}
			}
				break;
		case INPUT_PRESET_FILE:
			{
				bool filename_entered = false;
				char filename_tmp[256];
				oskutil_write_initial_message(&oskutil_handle, L"example");
				oskutil_write_message(&oskutil_handle, L"Enter filename for preset (with no file extension)");
				oskutil_start(&oskutil_handle);

				while(OSK_IS_RUNNING(oskutil_handle))
				{
					/* OSK Util gets updated */
					glClear(GL_COLOR_BUFFER_BIT);
					ps3graphics_draw_menu();
					psglSwap();
					cell_console_poll();
					cellSysutilCheckCallback();
				}

				if(oskutil_handle.text_can_be_fetched)
				{
					strncpy(filename_tmp, OUTPUT_TEXT_STRING(oskutil_handle), sizeof(filename_tmp));
					snprintf(filepath, sizeof(filepath), "%s/%s.conf", INPUT_PRESETS_DIR_PATH, filename_tmp);
					filename_entered = true;
				}

				if(filename_entered)
				{
					char filetitle_tmp[512];
					oskutil_write_initial_message(&oskutil_handle, L"Example file title");
					oskutil_write_message(&oskutil_handle, L"Enter title for preset");
					oskutil_start(&oskutil_handle);

					while(OSK_IS_RUNNING(oskutil_handle))
					{
						/* OSK Util gets updated */
						glClear(GL_COLOR_BUFFER_BIT);
						ps3graphics_draw_menu();
						psglSwap();
						cell_console_poll();
						cellSysutilCheckCallback();
					}

					if(oskutil_handle.text_can_be_fetched)
						snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", OUTPUT_TEXT_STRING(oskutil_handle));
					else
						snprintf(filetitle_tmp, sizeof(filetitle_tmp), "%s", "Custom");


					if(!file_exists(filepath))
					{
						FILE * f = fopen(filepath, "w");
						fclose(f);
					}

					emulator_set_controls(filepath, WRITE_CONTROLS, filetitle_tmp);
				}
			}
				break;
	}
}

void S9xMessage (int type, int number, const char *message)
{
	set_text_message(message, 60);
}

const char * S9xGetFilename (const char *ex, enum s9x_getdirtype dirtype)
{
	static char	s[PATH_MAX + 1];
	char		drive[_MAX_DRIVE + 1];
	char		dir[_MAX_DIR + 1];
	char		fname[_MAX_FNAME + 1];
	char		ext[_MAX_EXT + 1];

	_splitpath(Memory.ROMFilename, drive, dir, fname, ext);
	snprintf(s, PATH_MAX + 1, "%s%s%s%s", S9xGetDirectory(dirtype), SLASH_STR, fname, ex);

	return (s);
}

const char * S9xGetDirectory (enum s9x_getdirtype dirtype)
{
	static char s[PATH_MAX + 1];

	switch (dirtype)
	{
		case SNAPSHOT_DIR:
			return Settings.PS3PathSaveStates;
		case CHEAT_DIR:
			return Settings.PS3PathCheats;
		case SRAM_DIR:
			return Settings.PS3PathSRAM;
		case ROMFILENAME_DIR:
			strncpy(s, Memory.ROMFilename, PATH_MAX + 1);
			s[PATH_MAX] = 0;

			for (unsigned int i = strlen(s); i >= 0; --i)
			{
				if (s[i] == SLASH_CHAR)
				{
					s[i] = 0;
					break;
				}
			}

			return s;
		default:
			return usrDirPath;
	}
}

const char *S9xChooseFilename (bool8 read_only)
{
	static char	filename[PATH_MAX + 1];
	static char	drive[_MAX_DRIVE + 1];
	static char	dir[_MAX_DIR + 1];
	static char	def[_MAX_FNAME + 1];
	static char	ext[_MAX_EXT + 1];

	_splitpath(Memory.ROMFilename, drive, dir, def, ext);
	snprintf(filename, PATH_MAX + 1, "%s%s%s.%03d", S9xGetDirectory(SNAPSHOT_DIR), SLASH_STR, def, Settings.CurrentSaveStateSlot);

	return filename;
}

void _splitpath (const char *path, char *drive, char *dir, char *fname, char *ext)
{
	*drive = 0;

	const char	*slash = strrchr(path, SLASH_CHAR);
	const char	*dot   = strrchr(path, '.');

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

void _makepath (char *path, const char *, const char *dir, const char *fname, const char *ext)
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

void S9xDeinitUpdate(int width, int height)
{
	ps3graphics_draw(width, height, GFX.Screen);
	if(frame_count < special_action_msg_expired)
	{
		cellDbgFontPrintf (0.09f, 0.90f, 1.51f, BLUE,	special_action_msg);
		cellDbgFontPrintf (0.09f, 0.90f, 1.50f, WHITE,	special_action_msg);
		cellDbgFontDraw();
	}
	else
		special_action_msg_expired = 0;
	psglSwap();
}

static void Emulator_Initialize(void)
{
	if(emulator_init_system())
	{
		if(!emulator_initialized)
			emulator_initialized = 1;

		need_load_rom = false;
	}
	else
		need_load_rom = true;
}

static void emulator_start(void)
{
	if(need_load_rom)
		Emulator_Initialize();

	if (ps3graphics_get_current_resolution() == CELL_VIDEO_OUT_RESOLUTION_576)
	{
		if(ps3graphics_check_resolution(CELL_VIDEO_OUT_RESOLUTION_576))
		{
			if(Settings.PAL)
			{
				//Game is PAL
				if(ps3graphics_get_pal60hz())
					//PAL60 is ON, turn it off for PAL
					emulator_switch_pal_60hz(false);
			}
			else
			{
				//Game is NTSC
				if(!ps3graphics_get_pal60hz())
					//PAL60 is OFF, turn it on for NTSC
					emulator_switch_pal_60hz(true);
			}
		}
	}

	if (ps3graphics_calculate_aspect_ratio_before_game_load())
		ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);

	if(Settings.Throttled)
		audio_driver->unpause(audio_handle);

	audio_active = true;

	do{
		S9xMainLoop();
		if(pad_read_last)
		{
			// Port 1
			if(snes_devices[0] == CTL_JOYPAD)
					emulator_implementation_input_loop();
			else if(snes_devices[0] == CTL_MOUSE)
					emulator_implementation_input_loop_mouse(0, snes_devices[0]);

			// Port 2
			if(snes_devices[1] == CTL_MOUSE || snes_devices[1] == CTL_SUPERSCOPE)
					emulator_implementation_input_loop_mouse(1, snes_devices[1]);
		}
		cell_console_poll();
		cellSysutilCheckCallback();
	}while(is_running);

	emulator_save_sram();
}

float Emulator_GetFontSize(void)
{
	return Settings.PS3FontSize/100.0;
}


bool Emulator_IsROMLoaded(void)
{
	return current_rom != NULL && need_load_rom == false;
}

static void emulator_shutdown(void)
{
	emulator_save_settings(CONFIG_FILE);

#ifdef PS3_PROFILING
	// When profiling we have to do exit(), so that gcov can be hooked properly. sys_process_exit() bypasses all this.
	net_stdio_enable(1);
	gfx_deinit();

	cellSysmoduleUnloadModule(CELL_SYSMODULE_FS);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_IO);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_AVCONF_EXT);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);
	cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
	cellSysutilUnregisterCallback(0);

	exit(0);
#else
	cellSysutilUnregisterCallback(0);
#ifdef MULTIMAN_SUPPORT
	if(return_to_MM)
	{
		audio_active = false;
		if(audio_handle)
		{
			audio_driver->free(audio_handle);
			audio_handle = NULL; 
		}

		cellSysmoduleUnloadModule(CELL_SYSMODULE_AVCONF_EXT);
		sys_spu_initialize(6, 0);
		char multiMAN[512];
		snprintf(multiMAN, sizeof(multiMAN), "%s", "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF");
		sys_game_process_exitspawn2((char*) multiMAN, NULL, NULL, NULL, 0, 2048, SYS_PROCESS_PRIMARY_STACK_SIZE_64K);		
		sys_process_exit(0);
	}
	else
#endif
		sys_process_exit(0);
#endif
}

void Emulator_RequestLoadROM(const char* rom)
{
	if (current_rom == NULL || strcmp(rom, current_rom) != 0)
	{
		if (current_rom != NULL)
			free(current_rom);

		current_rom = strdup(rom);
		need_load_rom = true;
	}
	else
	{
		need_load_rom = false;
		S9xSoftReset();
	}
}

void Emulator_StartROMRunning(uint32_t set_is_running)
{
	if(set_is_running)
		is_running = 1;
	mode_switch = MODE_EMULATION;
}



//FIXME: Turn GREEN into WHITE and RED into LIGHTBLUE once the overlay is in
#define ingame_menu_reset_entry_colors(ingame_menu_item) \
{ \
   for(int i = 0; i < MENU_ITEM_LAST; i++) \
      menuitem_colors[i] = GREEN; \
   menuitem_colors[ingame_menu_item] = RED; \
}


static void ingame_menu(void)
{
	uint32_t menuitem_colors[MENU_ITEM_LAST];
	char comment[256];

	do
	{
		uint64_t state = cell_pad_input_poll_device(0);
		static uint64_t old_state = 0;
		uint64_t stuck_in_loop = 1;
		const uint64_t button_was_pressed = old_state & (old_state ^ state);
		const uint64_t button_was_held = old_state & state;
		static uint64_t blocking = 0;


		ps3graphics_draw(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Screen);

		if(frame_count < special_action_msg_expired && blocking)
		{
		}
		else
		{
			if(CTRL_CIRCLE(state))
			{
				is_running = 1;
				ingame_menu_item = 0;
				is_ingame_menu_running = 0;
				Emulator_StartROMRunning(0);
			}

			switch(ingame_menu_item)
			{
				case MENU_ITEM_LOAD_STATE:
					if(CTRL_CROSS(button_was_pressed))
					{
						emulator_load_current_save_state_slot();
						is_running = 1;
						ingame_menu_item = 0;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
					}
					if(CTRL_LEFT(button_was_pressed) || CTRL_LSTICK_LEFT(button_was_pressed))
					{
						emulator_decrement_current_save_state_slot();
						blocking = 0;
					}
					if(CTRL_RIGHT(button_was_pressed) || CTRL_LSTICK_RIGHT(button_was_pressed))
					{
						emulator_increment_current_save_state_slot();
						blocking = 0;
					}

					ingame_menu_reset_entry_colors(ingame_menu_item);
					strcpy(comment, "Press LEFT or RIGHT to change the current save state slot.\nPress CROSS to load the state from the currently selected save state slot.");
					break;
				case MENU_ITEM_SAVE_STATE:
					if(CTRL_CROSS(button_was_pressed))
					{
						emulator_save_current_save_state_slot();
						is_running = 1;
						ingame_menu_item = 0;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
					}
					if(CTRL_LEFT(button_was_pressed) || CTRL_LSTICK_LEFT(button_was_pressed))
					{
						emulator_decrement_current_save_state_slot();
						blocking = 0;
					}
					if(CTRL_RIGHT(button_was_pressed) || CTRL_LSTICK_RIGHT(button_was_pressed))
					{
						emulator_increment_current_save_state_slot();
						blocking = 0;
					}

					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press LEFT or RIGHT to change the current save state slot.\nPress CROSS to save the state to the currently selected save state slot.");
					break;
				case MENU_ITEM_KEEP_ASPECT_RATIO:
					if(CTRL_LEFT(button_was_pressed) || CTRL_LSTICK_LEFT(button_was_pressed))
					{
						if(Settings.PS3KeepAspect > 0)
						{
							Settings.PS3KeepAspect--;
							ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);
							if (ps3graphics_calculate_aspect_ratio_before_game_load())
								ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);
						}
					}
					if(CTRL_RIGHT(button_was_pressed)  || CTRL_LSTICK_RIGHT(button_was_pressed))
					{
						if(Settings.PS3KeepAspect < LAST_ASPECT_RATIO)
						{
							Settings.PS3KeepAspect++;
							ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);
							if (ps3graphics_calculate_aspect_ratio_before_game_load())
								ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);
						}
					}
					if(CTRL_START(button_was_pressed))
					{
						Settings.PS3KeepAspect = ASPECT_RATIO_4_3;
						ps3graphics_set_aspect_ratio(Settings.PS3KeepAspect, IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, 1);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press LEFT or RIGHT to change the [Aspect Ratio].\nPress START to reset back to default values.");
					break;
				case MENU_ITEM_OVERSCAN_AMOUNT:
					if(CTRL_LEFT(button_was_pressed)  ||  CTRL_LSTICK_LEFT(button_was_pressed) || CTRL_CROSS(button_was_pressed) || CTRL_LSTICK_LEFT(button_was_held))
					{
						Settings.PS3OverscanAmount--;
						Settings.PS3OverscanEnabled = 1;

						if(Settings.PS3OverscanAmount == 0)
							Settings.PS3OverscanEnabled = 0;

						ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
					}
					if(CTRL_RIGHT(button_was_pressed) || CTRL_LSTICK_RIGHT(button_was_pressed) || CTRL_CROSS(button_was_pressed) || CTRL_LSTICK_RIGHT(button_was_held))
					{
						Settings.PS3OverscanAmount++;
						Settings.PS3OverscanEnabled = 1;
						if(Settings.PS3OverscanAmount == 0)
							Settings.PS3OverscanEnabled = 0;

						ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
					}
					if(CTRL_START(button_was_pressed))
					{
						Settings.PS3OverscanAmount = 0;
						Settings.PS3OverscanEnabled = 0;
						ps3graphics_set_overscan(Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, 1);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press LEFT or RIGHT to change the [Overscan] settings.\nPress START to reset back to default values.");
					break;
				case MENU_ITEM_FRAME_ADVANCE:
					if(CTRL_CROSS(state) || CTRL_R2(state) || CTRL_L2(state))
					{
						is_running = 0;
						ingame_menu_item = MENU_ITEM_FRAME_ADVANCE;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS', 'L2' or 'R2' button to step one frame.\nNOTE: Pressing the button rapidly will advance the frame more slowly\nand prevent buttons from being input.");
					break;
				case MENU_ITEM_RESIZE_MODE:
					if(CTRL_CROSS(state))
					{
						ps3graphics_set_aspect_ratio(ASPECT_RATIO_CUSTOM, IPPU.RenderedScreenWidth, IPPU.RenderedScreenWidth, 1);
						do
						{
							ps3graphics_draw(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Screen);
							state = cell_pad_input_poll_device(0);
							ps3graphics_resize_aspect_mode_input_loop(state);

							if(CTRL_CIRCLE(state))
							{
								set_text_message("", 7);
								blocking = 1;
								stuck_in_loop = 0;
							}

							psglSwap();
							cellSysutilCheckCallback();
							old_state = state;
						}while(stuck_in_loop && is_ingame_menu_running);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Allows you to resize the screen by moving around the two analog sticks.\nPress TRIANGLE to reset to default values, and CIRCLE to go back to the\nin-game menu.");
					break;
				case MENU_ITEM_SCREENSHOT_MODE:
					if(CTRL_CROSS(state))
					{
						while(stuck_in_loop && is_ingame_menu_running)
						{
							state = cell_pad_input_poll_device(0);
							if(CTRL_CIRCLE(state))
							{
								set_text_message("", 7);
								blocking = 1;
								stuck_in_loop = 0;
							}

							ps3graphics_draw(IPPU.RenderedScreenWidth, IPPU.RenderedScreenHeight, GFX.Screen);
							psglSwap();
							cellSysutilCheckCallback();
							old_state = state;
						}
					}

					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Allows you to take a screenshot without any text clutter.\nPress CIRCLE to go back to the in-game menu while in 'Screenshot Mode'.");
					break;
				case MENU_ITEM_RETURN_TO_GAME:
					if(CTRL_CROSS(button_was_pressed))
					{
						is_running = 1;
						ingame_menu_item = 0;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to return back to the game.");
					break;
				case MENU_ITEM_RESET:
					if(CTRL_CROSS(button_was_pressed))
					{
						S9xSoftReset();
						is_running = 1;
						ingame_menu_item = 0;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to reset the game.");
					break;
				case MENU_ITEM_RESET_FORCE_NTSC_PAL:
					if(CTRL_CROSS(button_was_pressed))
					{
						Settings.ForceNTSC = false;
						Settings.ForcePAL = true;
						need_load_rom = 1;
						is_running = 1;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
						ingame_menu_item = 0;
					}
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to reset the game and force it into PAL videomode.");
					break;
				case MENU_ITEM_RESET_FORCE_PAL_NTSC:
					if(CTRL_CROSS(button_was_pressed))
					{
						Settings.ForceNTSC = true;
						Settings.ForcePAL = false;
						need_load_rom = 1;
						is_running = 1;
						is_ingame_menu_running = 0;
						Emulator_StartROMRunning(0);
						ingame_menu_item = 0;
					} 
					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to reset the game and force it into NTSC videomode.");
					break;
				case MENU_ITEM_RETURN_TO_MENU:
					if(CTRL_CROSS(button_was_pressed))
					{
						is_running = 0;
						ingame_menu_item = 0;
						is_ingame_menu_running = 0;
						mode_switch = MODE_MENU;
					}

					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to return to the ROM Browser menu.");
					break;
#ifdef MULTIMAN_SUPPORT
				case MENU_ITEM_RETURN_TO_MULTIMAN:
					if(CTRL_CROSS(button_was_pressed))
					{
						is_running = 0;
						is_ingame_menu_running = 0;
						mode_switch = MODE_EXIT; 
					}

					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to quit the emulator and return to multiMAN.");
					break;
#endif
				case MENU_ITEM_RETURN_TO_XMB:
					if(CTRL_CROSS(button_was_pressed))
					{
						is_running = 0;
						is_ingame_menu_running = 0;
#ifdef MULTIMAN_SUPPORT
						return_to_MM = false;
#endif
						mode_switch = MODE_EXIT; 
					}

					ingame_menu_reset_entry_colors (ingame_menu_item);
					strcpy(comment, "Press 'CROSS' to quit the emulator and return to the XMB.");
					break;
			}

			if(CTRL_UP(button_was_pressed) || CTRL_LSTICK_UP(button_was_pressed))
			{
				if(ingame_menu_item > 0)
					ingame_menu_item--;
			}

			if(CTRL_DOWN(button_was_pressed) || CTRL_LSTICK_DOWN(button_was_pressed))
			{
				if(ingame_menu_item < MENU_ITEM_LAST)
					ingame_menu_item++;
			}
		}

		float x_position = 0.3f;
		float font_size = 1.1f;
		float ypos = 0.19f;
		float ypos_increment = 0.04f;

		cellDbgFontPrintf	(x_position,	0.10f,	1.4f+0.01f,	BLUE,               "Quick Menu");
		cellDbgFontPrintf(x_position,	0.10f,	1.4f,	WHITE,               "Quick Menu");

		cellDbgFontPrintf	(x_position,	ypos,	font_size+0.01f,	BLUE,	"Load State #%d", Settings.CurrentSaveStateSlot);
		cellDbgFontPrintf(x_position,	ypos,	font_size,	menuitem_colors[MENU_ITEM_LOAD_STATE],	"Load State #%d", Settings.CurrentSaveStateSlot);

		cellDbgFontPrintf	(x_position,	ypos+(ypos_increment*MENU_ITEM_SAVE_STATE),	font_size+0.01f,	BLUE,	"Save State #%d", Settings.CurrentSaveStateSlot);
		cellDbgFontPrintf(x_position,	ypos+(ypos_increment*MENU_ITEM_SAVE_STATE),	font_size,	menuitem_colors[MENU_ITEM_SAVE_STATE],	"Save State #%d", Settings.CurrentSaveStateSlot);
		cellDbgFontDraw();

		cellDbgFontPrintf	(x_position,	(ypos+(ypos_increment*MENU_ITEM_KEEP_ASPECT_RATIO)),	font_size+0.01f,	BLUE,	"Aspect Ratio: %s %s %d:%d", ps3graphics_calculate_aspect_ratio_before_game_load() ?"(Auto)" : "", Settings.PS3KeepAspect == LAST_ASPECT_RATIO ? "Custom" : "", (int)ps3graphics_get_aspect_ratio_int(0), (int)ps3graphics_get_aspect_ratio_int(1));
		cellDbgFontPrintf(x_position,	(ypos+(ypos_increment*MENU_ITEM_KEEP_ASPECT_RATIO)),	font_size,	menuitem_colors[MENU_ITEM_KEEP_ASPECT_RATIO],	"Aspect Ratio: %s %s %d:%d", ps3graphics_calculate_aspect_ratio_before_game_load() ?"(Auto)" : "", Settings.PS3KeepAspect == LAST_ASPECT_RATIO ? "Custom" : "", (int)ps3graphics_get_aspect_ratio_int(0), (int)ps3graphics_get_aspect_ratio_int(1));

		cellDbgFontPrintf(x_position,	(ypos+(ypos_increment*MENU_ITEM_OVERSCAN_AMOUNT)),	font_size,	menuitem_colors[MENU_ITEM_OVERSCAN_AMOUNT],	"Overscan: %f", (float)Settings.PS3OverscanAmount/100);

		cellDbgFontPrintf	(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESIZE_MODE)),	font_size+0.01f,	BLUE,	"Resize Mode");
		cellDbgFontPrintf(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESIZE_MODE)),	font_size,	menuitem_colors[MENU_ITEM_RESIZE_MODE],	"Resize Mode");

		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_FRAME_ADVANCE)),	font_size+0.01f,	BLUE,	"Frame Advance");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_FRAME_ADVANCE)),	font_size,	menuitem_colors[MENU_ITEM_FRAME_ADVANCE],	"Frame Advance");

		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_SCREENSHOT_MODE)),	font_size+0.01f,	BLUE,	"Screenshot Mode");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_SCREENSHOT_MODE)),	font_size,	menuitem_colors[MENU_ITEM_SCREENSHOT_MODE],	"Screenshot Mode");

		cellDbgFontDraw();

		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET)),	font_size+0.01f,	BLUE,	"Reset");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET)),	font_size,	menuitem_colors[MENU_ITEM_RESET],	"Reset");

		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET_FORCE_NTSC_PAL)),	font_size+0.01f, BLUE,	"Reset - Force NTSC to PAL");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET_FORCE_NTSC_PAL)),	font_size,	menuitem_colors[MENU_ITEM_RESET_FORCE_NTSC_PAL],	"Reset - Force NTSC to PAL");

		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET_FORCE_PAL_NTSC)),	font_size+0.01f,	BLUE,	"Reset - Force PAL to NTSC");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RESET_FORCE_PAL_NTSC)),	font_size,	menuitem_colors[MENU_ITEM_RESET_FORCE_PAL_NTSC],	"Reset - Force PAL to NTSC");

		cellDbgFontPuts   (x_position,   (ypos+(ypos_increment*MENU_ITEM_RETURN_TO_GAME)),   font_size+0.01f,  BLUE,  "Return to Game");
		cellDbgFontPuts(x_position,   (ypos+(ypos_increment*MENU_ITEM_RETURN_TO_GAME)),   font_size,  menuitem_colors[MENU_ITEM_RETURN_TO_GAME],  "Return to Game");

		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_MENU)),	font_size+0.01f,	BLUE,	"Return to Menu");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_MENU)),	font_size,	menuitem_colors[MENU_ITEM_RETURN_TO_MENU],	"Return to Menu");
#ifdef MULTIMAN_SUPPORT
		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_MULTIMAN)),	font_size+0.01f,	BLUE,	"Return to multiMAN");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_MULTIMAN)),	font_size,	menuitem_colors[MENU_ITEM_RETURN_TO_MULTIMAN],	"Return to multiMAN");
#endif
		cellDbgFontDraw();

		cellDbgFontPuts	(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_XMB)),	font_size+0.01f,	BLUE,	"Return to XMB");
		cellDbgFontPuts(x_position,	(ypos+(ypos_increment*MENU_ITEM_RETURN_TO_XMB)),	font_size,	menuitem_colors[MENU_ITEM_RETURN_TO_XMB],	"Return to XMB");

		if(frame_count < special_action_msg_expired)
		{
			cellDbgFontPrintf (0.09f, 0.90f, 1.51f, BLUE,	special_action_msg);
			cellDbgFontPrintf (0.09f, 0.90f, 1.50f, WHITE,	special_action_msg);
			cellDbgFontDraw();
		}
		else
		{
			special_action_msg_expired = 0;
			cellDbgFontPrintf (0.09f,   0.90f,   0.98f+0.01f,      BLUE,           comment);
			cellDbgFontPrintf (0.09f,   0.90f,   0.98f,      LIGHTBLUE,           comment);
		}
		cellDbgFontDraw();
		psglSwap();
		old_state = state;
		cellSysutilCheckCallback();
	}while(is_ingame_menu_running);
}

static void get_path_settings(bool multiman_support)
{
	unsigned int get_type;
	unsigned int get_attributes;
	CellGameContentSize size;
	char dirName[CELL_GAME_DIRNAME_SIZE];

	memset(&size, 0x00, sizeof(CellGameContentSize));

	int ret = cellGameBootCheck(&get_type, &get_attributes, &size, dirName); 
	if(ret < 0)
	{
		printf("cellGameBootCheck() Error: 0x%x\n", ret);
	}
	else
	{
		printf("cellGameBootCheck() OK\n");
		printf("  get_type = [%d] get_attributes = [0x%08x] dirName = [%s]\n", get_type, get_attributes, dirName);
		printf("  hddFreeSizeKB = [%d] sizeKB = [%d] sysSizeKB = [%d]\n", size.hddFreeSizeKB, size.sizeKB, size.sysSizeKB);

		ret = cellGameContentPermit(contentInfoPath, usrDirPath);

		if(multiman_support)
		{
			snprintf(contentInfoPath, sizeof(contentInfoPath), "/dev_hdd0/game/%s", EMULATOR_CONTENT_DIR);
			snprintf(usrDirPath, sizeof(usrDirPath), "/dev_hdd0/game/%s/USRDIR", EMULATOR_CONTENT_DIR);
		}

		if(ret < 0)
		{
			printf("cellGameContentPermit() Error: 0x%x\n", ret);
		}
		else
		{
			printf("cellGameContentPermit() OK\n");
			printf("contentInfoPath:[%s]\n", contentInfoPath);
			printf("usrDirPath:[%s]\n",  usrDirPath);
		}

		/* now we fill in all the variables */
		snprintf(DEFAULT_PRESET_FILE, sizeof(DEFAULT_PRESET_FILE), "%s/presets/stock.conf", usrDirPath);
		snprintf(DEFAULT_BORDER_FILE, sizeof(DEFAULT_BORDER_FILE), "%s/borders/Centered-1080p/super-metroid.png", usrDirPath);
		snprintf(DEFAULT_MENU_BORDER_FILE, sizeof(DEFAULT_MENU_BORDER_FILE), "%s/borders/Menu/main-menu.jpg", usrDirPath);
		snprintf(GAME_AWARE_SHADER_DIR_PATH, sizeof(GAME_AWARE_SHADER_DIR_PATH), "%s/gameaware", usrDirPath);
		snprintf(PRESETS_DIR_PATH, sizeof(PRESETS_DIR_PATH), "%s/presets", usrDirPath); 
		snprintf(INPUT_PRESETS_DIR_PATH, sizeof(INPUT_PRESETS_DIR_PATH), "%s/input-presets", usrDirPath); 
		snprintf(BORDERS_DIR_PATH, sizeof(BORDERS_DIR_PATH), "%s/borders", usrDirPath); 
		snprintf(SHADERS_DIR_PATH, sizeof(SHADERS_DIR_PATH), "%s/shaders", usrDirPath);
		snprintf(DEFAULT_SHADER_FILE, sizeof(DEFAULT_SHADER_FILE), "%s/shaders/stock.cg", usrDirPath);
		snprintf(DEFAULT_MENU_SHADER_FILE, sizeof(DEFAULT_MENU_SHADER_FILE), "%s/shaders/Borders/Menu/border-only.cg", usrDirPath);
		snprintf(SYS_CONFIG_FILE, sizeof(SYS_CONFIG_FILE), "%s/snes9x.conf", usrDirPath);
	}
}

int main(int argc, char **argv)
{
	sys_spu_initialize(4, 3);
	cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
	cellSysmoduleLoadModule(CELL_SYSMODULE_AVCONF_EXT);
	cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
	cellSysmoduleLoadModule(CELL_SYSMODULE_JPGDEC);
	cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_GAME);

	cellSysutilRegisterCallback(0, callback_sysutil_exit, NULL);

#ifdef PS3_PROFILING
	net_stdio_set_target(PS3_PROFILING_IP, PS3_PROFILING_PORT);
	net_stdio_set_paths("/", 2);
	net_stdio_enable(0);
#endif

	// parse input roms
#ifdef MULTIMAN_SUPPORT
	return_to_MM = true;

	if(argc > 1)
	{
		mode_switch = MODE_MULTIMAN_STARTUP;
		strncpy(MULTIMAN_GAME_TO_BOOT, argv[1], sizeof(MULTIMAN_GAME_TO_BOOT));
	}
#endif

	get_path_settings(return_to_MM);

	if (need_load_settings)
	{
		emulator_init_settings();
		need_load_settings =  false;
	}


#if(CELL_SDK_VERSION > 0x340000)
	if (Settings.ScreenshotsEnabled)
	{
		cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
		CellScreenShotSetParam  screenshot_param = {0, 0, 0, 0};

		screenshot_param.photo_title = "SNES9x PS3";
		screenshot_param.game_title = "SNES9x PS3";
		cellScreenShotSetParameter (&screenshot_param);
		cellScreenShotEnable();
	}
#endif

	ps3graphics_new((uint32_t)Settings.PS3CurrentResolution, Settings.PS3KeepAspect, Settings.PS3Smooth, Settings.PS3Smooth2, Settings.PS3CurrentShader, Settings.PS3CurrentShader2, DEFAULT_MENU_SHADER_FILE, Settings.PS3OverscanEnabled, (float)Settings.PS3OverscanAmount/100, Settings.PS3PALTemporalMode60Hz, Settings.Throttled, Settings.TripleBuffering, Settings.ViewportX, Settings.ViewportY, Settings.ViewportWidth, Settings.ViewportHeight, Settings.ScaleEnabled, Settings.ScaleFactor);

	if(Settings.ApplyShaderPresetOnStartup)
		emulator_implementation_set_shader_preset(Settings.ShaderPresetPath); 

	cell_pad_input_init();

	oskutil_init(&oskutil_handle, 0);

#if 0
	if (strcmp(Settings.GameAwareShaderPath,"") != 0)
	{
		emulator_implementation_set_gameaware(Settings.GameAwareShaderPath);
	}
#endif

	frame_count = 0;
	ps3graphics_init_dbgfont();

#ifdef CELL_DEBUG_CONSOLE
	cellConsoleInit();
	cellConsoleNetworkInitialize();
	cellConsoleNetworkServerInit(-1);
	cellConsoleScreenShotPluginInit();
#endif

	emulator_implementation_set_texture(Settings.PS3CurrentBorder);
	emulator_toggle_sound(Settings.SoundMode);

#if(CELL_SDK_VERSION > 0x340000)
	cellSysutilEnableBgmPlayback();
#endif
	sys_lwmutex_attribute_t attr;
	sys_lwmutex_attribute_initialize(attr);
	sys_lwmutex_create(&audio_lock, &attr);

	MenuInit();

	do{
		switch(mode_switch)
		{
			case MODE_MENU:
				MenuMainLoop();
				break;
			case MODE_EMULATION:
				if(ingame_menu_item != 0)
					is_ingame_menu_running = 1;

				emulator_start();

				if(Settings.Throttled)
					audio_driver->pause(audio_handle);

				if(is_ingame_menu_running)
				{
					ingame_menu();
				}
				break;
#ifdef MULTIMAN_SUPPORT
			case MODE_MULTIMAN_STARTUP:
				Emulator_StartROMRunning(1);
				Emulator_RequestLoadROM(MULTIMAN_GAME_TO_BOOT);
				break;
#endif
			case MODE_EXIT:
				emulator_shutdown();
				return(0);
		}
	}while(1);
}
