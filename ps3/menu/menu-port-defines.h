/******************************************************************************* 
 * menu-port-defines.h - SNES9x Next PS3
 *
 *  Created on: Jan 9, 2012
********************************************************************************/

#include "../src/snes9x.h"

/* allowed ROM extensions - ROMs are separated by a '|' character */
#define ROM_EXTENSIONS "smc|fig|sfc|gd3|gd7|dx2|bsx|swc|zip|jma|SMC|FIG|SFC|BSX|GD3|GD7|DX2|SWC|ZIP|JMA"

#define EMU_MENU_TITLE "SNES9X |"
#define VIDEO_MENU_TITLE "SNES9X VIDEO |"
#define AUDIO_MENU_TITLE "SNES9X AUDIO |"

/* fill in the extra menu settings here for your port */
#define EMU_EXTRA_SETTINGS \
	SETTING_EMU_CURRENT_SAVE_STATE_SLOT, \
	SETTING_SNES9X_AUTO_APPLY_CHEATS, \
	SETTING_SNES9X_AUTO_APPLY_PATCH, \
	SETTING_SNES9X_SRAM_WRITEPROTECT, \
	SETTING_SNES9X_ACCESSORY_AUTODETECTION, \
	SETTING_SNES9X_ACCESSORY_TYPE, \
	SETTING_EMU_DEFAULT_ALL, \
	SETTING_SNES9X_FORCE_PAL, \
	SETTING_SNES9X_FORCE_NTSC, \
	SETTING_SNES9X_PAL_TIMING, \
	SETTING_EMU_VIDEO_DEFAULT_ALL, \
	SETTING_SNES9X_SOUND_INPUT_RATE, \
	SETTING_SNES9X_MUTE_SOUND, \
	SETTING_EMU_AUDIO_DEFAULT_ALL

#define EMU_EXTRA_PATH_SETTINGS

#define EMU_DISPLAY_HELP_TEXT() \
		case SETTING_SNES9X_AUTO_APPLY_CHEATS: \
		case SETTING_SNES9X_AUTO_APPLY_PATCH: \
		case SETTING_SNES9X_SRAM_WRITEPROTECT: \
		case SETTING_SNES9X_ACCESSORY_TYPE: \
			PRINT_HELP_MESSAGE_YESNO(menu_emu_settings, currentsetting); \
			break; \
		case SETTING_SNES9X_FORCE_PAL: \
		case SETTING_SNES9X_FORCE_NTSC: \
			PRINT_HELP_MESSAGE_YESNO(menu_emu_videosettings, currentsetting); \
			break; \
		case SETTING_SNES9X_ACCESSORY_AUTODETECTION: \
			if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_ENABLED) \
				cellDbgFontPrintf(0.09f, 0.83f, 0.86f, LIGHTBLUE, "%s", "INFO - Accessory Autodetection is set to 'ON'. Games which support\nMouse/Scope/Multitap will be automatically detected and enabled."); \
			else if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM) \
				cellDbgFontPrintf(0.09f, 0.83f, 0.86f, LIGHTBLUE, "%s", "INFO - Accessory Autodetection is set to 'Confirm'. When detecting a Mouse/\nScope/Multitap-compatible game, you are asked if you want to enable it."); \
			else if(Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_NONE) \
				cellDbgFontPrintf(0.09f, 0.83f, 0.86f, LIGHTBLUE, "%s", "INFO - Accessory Autodetection is set to 'OFF'. Accessories will not be\ndetected or enabled - standard two joypad controls."); \
			break; \
		case SETTING_SNES9X_PAL_TIMING: \
			PRINT_HELP_MESSAGE(menu_emu_videosettings, currentsetting); \
			break; \
		case SETTING_SNES9X_SOUND_INPUT_RATE: \
		case SETTING_SNES9X_MUTE_SOUND: \
			PRINT_HELP_MESSAGE(menu_emu_audiosettings, currentsetting); \
			break;

#define EMU_DISPLAY_LABEL_VALUE() \
		case SETTING_SNES9X_SOUND_INPUT_RATE: \
			cellDbgFontPrintf	(0.5f,	menu_emu_audiosettings.items[switchvalue].text_ypos,	Emulator_GetFontSize(),	Settings.SoundInputRate == 31950 ? GREEN : ORANGE, "%d", Settings.SoundInputRate); \
			cellDbgFontDraw(); \
			break; \
		case SETTING_SNES9X_MUTE_SOUND: \
			cellDbgFontPrintf	(0.5f,	menu_emu_audiosettings.items[switchvalue].text_ypos,	Emulator_GetFontSize(),	*(menu_emu_audiosettings.items[switchvalue].setting_ptr) ? GREEN : ORANGE, *(menu_emu_audiosettings.items[switchvalue].setting_ptr) ? "OFF" : "ON"); \
			break; \
		case SETTING_SNES9X_AUTO_APPLY_PATCH: \
		case SETTING_SNES9X_AUTO_APPLY_CHEATS: \
			cellDbgFontPuts(0.5f,  menu_emu_settings.items[switchvalue].text_ypos, Emulator_GetFontSize(), *(menu_emu_settings.items[switchvalue].setting_ptr) ? ORANGE : GREEN, *(menu_emu_settings.items[switchvalue].setting_ptr) ? "OFF" : "ON"); \
			break; \
		case SETTING_SNES9X_FORCE_PAL: \
		case SETTING_SNES9X_FORCE_NTSC: \
			cellDbgFontPuts(0.5f,  menu_emu_videosettings.items[switchvalue].text_ypos, Emulator_GetFontSize(), *(menu_emu_videosettings.items[switchvalue].setting_ptr) == 0 ? GREEN : ORANGE, *(menu_emu_videosettings.items[switchvalue].setting_ptr) ? "ON" : "OFF"); \
			cellDbgFontDraw(); \
			break; \
		case SETTING_SNES9X_SRAM_WRITEPROTECT: \
			cellDbgFontPuts(0.5f,  menu_emu_settings.items[switchvalue].text_ypos, Emulator_GetFontSize(), *(menu_emu_settings.items[switchvalue].setting_ptr) == 0 ? GREEN : ORANGE, *(menu_emu_settings.items[switchvalue].setting_ptr) ? "ON" : "OFF"); \
			cellDbgFontDraw(); \
			break; \
		case SETTING_SNES9X_PAL_TIMING: \
			cellDbgFontPrintf(0.5f, menu_emu_videosettings.items[switchvalue].text_ypos, Emulator_GetFontSize(), Settings.FrameTimePAL == 20000 ? GREEN : ORANGE, "%d", Settings.FrameTimePAL); \
			break; \
		case SETTING_SNES9X_ACCESSORY_AUTODETECTION: \
			cellDbgFontPrintf(0.5f,	menu_emu_settings.items[switchvalue].text_ypos,	Emulator_GetFontSize(),	Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM ? GREEN : ORANGE, "%s", Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_ENABLED ? "ON" : Settings.AccessoryAutoDetection == ACCESSORY_AUTODETECTION_CONFIRM ? "Confirm" : "OFF"); \
			cellDbgFontDraw(); \
			break; \
		case SETTING_SNES9X_ACCESSORY_TYPE: \
			cellDbgFontPrintf(0.5f,	menu_emu_settings.items[switchvalue].text_ypos,	Emulator_GetFontSize(),	Settings.AccessoryAutoDetection == SETTING_SNES9X_ACCESSORY_TYPE ? ORANGE : GREEN,  Settings.AccessoryType ? "USB/Bluetooth Mouse" : "Left analog stick"); \
			cellDbgFontDraw(); \
			break;


#define EMU_PRODUCE_SETTING_ENTRY() \
		case SETTING_SNES9X_SOUND_INPUT_RATE: \
			if(CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state) ) \
			{ \
				if(Settings.SoundInputRate > 30000) \
					Settings.SoundInputRate--; \
				\
				set_text_message("", 7); \
			} \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state)) \
			{ \
				if(Settings.SoundInputRate < 32000) /* SNES APU has an input rate of 32000, no higher */ \
					Settings.SoundInputRate++; \
				\
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.SoundInputRate = 31950; \
			break; \
		case SETTING_SNES9X_MUTE_SOUND: \
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state)) \
			{ \
				*(menu_emu_audiosettings.items[switchvalue].setting_ptr) = !(*(menu_emu_audiosettings.items[switchvalue].setting_ptr)); \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				*(menu_emu_audiosettings.items[switchvalue].setting_ptr) = 1; \
			break; \
		case SETTING_SNES9X_AUTO_APPLY_CHEATS: \
		case SETTING_SNES9X_AUTO_APPLY_PATCH: \
		case SETTING_SNES9X_SRAM_WRITEPROTECT: \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state)) \
			{ \
				*(menu_emu_settings.items[switchvalue].setting_ptr) = !(*(menu_emu_settings.items[switchvalue].setting_ptr)); \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				*(menu_emu_settings.items[switchvalue].setting_ptr) = menu_emu_settings.items[switchvalue].default_value; \
			break; \
		case SETTING_SNES9X_FORCE_PAL: \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state)) \
			{ \
				Settings.ForcePAL = !Settings.ForcePAL; \
				if(Settings.ForceNTSC) \
					Settings.ForceNTSC = 0; \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.ForcePAL = 0; \
			break; \
		case SETTING_SNES9X_FORCE_NTSC: \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LSTICK_LEFT(state) || CTRL_LEFT(state) || CTRL_CROSS(state)) \
			{ \
				Settings.ForceNTSC = !Settings.ForceNTSC; \
				if(Settings.ForcePAL) \
					Settings.ForcePAL = 0; \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.ForceNTSC = 0; \
			break; \
		case SETTING_SNES9X_PAL_TIMING: \
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) ) \
			{ \
				Settings.FrameTimePAL = 16667; \
				set_text_message("", 7); \
			} \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) ) \
			{ \
				Settings.FrameTimePAL = 20000; \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.FrameTimePAL = 20000; \
			break; \
		case SETTING_SNES9X_ACCESSORY_AUTODETECTION: \
			if(CTRL_LEFT(state) | CTRL_LSTICK_LEFT(state)) \
			{ \
				if(Settings.AccessoryAutoDetection != ACCESSORY_AUTODETECTION_CONFIRM) \
					Settings.AccessoryAutoDetection--; \
				set_text_message("", 7); \
			} \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_CROSS(state)) \
			{ \
				if(Settings.AccessoryAutoDetection != ACCESSORY_AUTODETECTION_NONE) \
					Settings.AccessoryAutoDetection++; \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.AccessoryAutoDetection = ACCESSORY_AUTODETECTION_CONFIRM; \
			break; \
		case SETTING_SNES9X_ACCESSORY_TYPE: \
			if(CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state)) \
			{ \
				if(Settings.AccessoryType) \
					Settings.AccessoryType = 0; \
				else \
					Settings.AccessoryType = 1; \
				set_text_message("", 7); \
			} \
			if(CTRL_START(state)) \
				Settings.AccessoryType = 0; \
			break; \
			case SETTING_EMU_DEFAULT_ALL: \
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state)) \
			{ \
				Settings.ForceNTSC = 0; \
				Settings.ForcePAL = 0; \
				Settings.FrameTimePAL = 20000; \
				Settings.NoPatch = 0; \
				Settings.ApplyCheats = 1; \
				Settings.ResetBeforeRecordingMovie = 0; \
				Settings.AccessoryAutoDetection = ACCESSORY_AUTODETECTION_CONFIRM; \
				Settings.AccessoryType = 0; \
				Settings.SRAMWriteProtect = 0; \
			} \
			break; \
			case SETTING_EMU_VIDEO_DEFAULT_ALL: \
			break; \
			case SETTING_EMU_AUDIO_DEFAULT_ALL: \
			if(CTRL_LEFT(state) || CTRL_LSTICK_LEFT(state) || CTRL_RIGHT(state) || CTRL_LSTICK_RIGHT(state) || CTRL_START(state) || CTRL_CROSS(state)) \
			{ \
				Settings.SoundInputRate = 31950; \
			} \
			break;

#define EXTRA_SELECT_FILE_PART1()
#define EXTRA_SELECT_FILE_PART2()
