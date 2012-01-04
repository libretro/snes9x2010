/******************************************************************************* 
 * emu-ps3.h - SNES9xNext PS3
 *
 *  Created on: May 4, 2011
********************************************************************************/

#ifndef EMUPS3_H_
#define EMUPS3_H_

/* System includes */

#ifdef CELL_DEBUG_CONSOLE
#include <cell/control_console.h>
#endif

#include <sysutil/sysutil_gamecontent.h>

#include "cellframework2/audio/stream.h"
#include "cellframework2/utility/oskutil.h"

#include "emu-ps3-constants.h"
#include "ps3input.h"
#include "ps3video.h"

/* PS3 frontend constants */

#define SOUND_MODE_NORMAL  1
#define SOUND_MODE_HEADSET 2
#define SOUND_MODE_RSOUND  3

#define WRITE_CONTROLS			0
#define READ_CONTROLS			1
#define SET_ALL_CONTROLS_TO_DEFAULT	2

enum {
   MENU_ITEM_LOAD_STATE = 0,
   MENU_ITEM_SAVE_STATE,
   MENU_ITEM_KEEP_ASPECT_RATIO,
   MENU_ITEM_OVERSCAN_AMOUNT,
   MENU_ITEM_ORIENTATION,
   MENU_ITEM_RESIZE_MODE,
   MENU_ITEM_FRAME_ADVANCE,
   MENU_ITEM_SCREENSHOT_MODE,
   MENU_ITEM_RESET,
   MENU_ITEM_RESET_FORCE_NTSC_PAL,
   MENU_ITEM_RESET_FORCE_PAL_NTSC,
   MENU_ITEM_RETURN_TO_GAME,
   MENU_ITEM_RETURN_TO_MENU,
#ifdef MULTIMAN_SUPPORT
   MENU_ITEM_RETURN_TO_MULTIMAN,
#endif
   MENU_ITEM_RETURN_TO_XMB
};

#define MENU_ITEM_LAST           MENU_ITEM_RETURN_TO_XMB+1

#define FILEBROWSER_DELAY         100000

/* Emulator-specific constants */

#define MAKE_BUTTON(pad, btn)  (((pad)<<6)+btn)
#define MAKE_POINTER(pad, btn) (((pad)<<4)|(btn))

#define AUDIO_INPUT_RATE (48000)
#define AUDIO_BUFFER_SAMPLES (4096)

#define NO_JOYSTICKS    1 << 0
#define TWO_JOYSTICKS   1 << 1
#define MULTITAP        1 << 2
#define MOUSE           1 << 3
#define JUSTIFIER       1 << 5

#define PAD_1           1
#define PAD_2           2
#define PAD_3           3
#define PAD_4           4
#define PAD_5           5

void emulator_implementation_set_texture (const char * fname);
void emulator_set_controls(const char * config_file, int mapping_enum, const char * title);
void emulator_toggle_sound(uint64_t soundmode);
void emulator_implementation_set_shader_preset(const char * fname);
void emulator_implementation_set_gameaware(const char * fname);
float Emulator_GetFontSize(void);
void Emulator_RequestLoadROM(const char* rom);
bool Emulator_IsROMLoaded(void);
void Emulator_StartROMRunning(uint32_t set_is_running);
void emulator_save_settings(uint64_t filetosave);
void set_text_message(const char * message, uint32_t speed);

extern char contentInfoPath[MAX_PATH_LENGTH];
extern char usrDirPath[MAX_PATH_LENGTH];
extern char DEFAULT_PRESET_FILE[MAX_PATH_LENGTH];
extern char DEFAULT_BORDER_FILE[MAX_PATH_LENGTH];
extern char DEFAULT_MENU_BORDER_FILE[MAX_PATH_LENGTH];
extern char GAME_AWARE_SHADER_DIR_PATH[MAX_PATH_LENGTH];
extern char INPUT_PRESETS_DIR_PATH[MAX_PATH_LENGTH];
extern char PRESETS_DIR_PATH[MAX_PATH_LENGTH];
extern char BORDERS_DIR_PATH[MAX_PATH_LENGTH];
extern char SHADERS_DIR_PATH[MAX_PATH_LENGTH];
extern char DEFAULT_SHADER_FILE[MAX_PATH_LENGTH];
extern char DEFAULT_MENU_SHADER_FILE[MAX_PATH_LENGTH];
extern char SYS_CONFIG_FILE[MAX_PATH_LENGTH];
extern oskutil_params oskutil_handle;
extern uint64_t mode_switch;

extern uint32_t special_action_msg_expired;			// time at which the message no longer needs to be overlaid onscreen

#endif /* EMULATOR_IMPLEMENTATION_H_ */

