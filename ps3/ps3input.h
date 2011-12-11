/******************************************************************************* 
 * ps3input.h - SNES9x PS3
 *
 *  Created on: Dec, 2010
********************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
/* Input bitmasks */
#define BTN_A				1
#define BTN_B				2
#define BTN_X				3
#define BTN_Y				4
#define BTN_SELECT			5
#define BTN_START			6
#define BTN_L				7
#define BTN_R				8
#define BTN_LEFT			9
#define BTN_RIGHT			10
#define BTN_UP				11
#define BTN_DOWN			12
#define BTN_QUICKSAVE			13
#define BTN_QUICKLOAD			14
#define BTN_INCREMENTSAVE		15
#define BTN_DECREMENTSAVE		16
#define BTN_INCREMENTCHEAT		17
#define BTN_DECREMENTCHEAT		18
#define BTN_EXITTOMENU			19
#define BTN_CHEATTOGGLE			22
#define BTN_LOAD_MOVIE			23
#define BTN_SOFTRESET			24
#define BTN_RESET			25
#define BTN_PAUSE			26
#define BTN_FASTFORWARD			30
#define BTN_SRAM_WRITEPROTECT		34
#define BTN_POINTER1			35
#define BTN_POINTER2			36
#define BTN_SCOPE_POINTER		37
#define BTN_CHEATINPUT			38
#define BTN_MOUSE1_LEFTBUTTON		39
#define BTN_MOUSE1_RIGHTBUTTON		40
#define BTN_NONE			41
#define BTN_CHEATINPUTLABEL		42
#define BTN_INGAME_MENU			43

#define CONTROL_SCHEME_DEFAULT      0
#define CONTROL_SCHEME_CUSTOM       1

#define MAX_PADS 7

enum {
	CTRL_UP_DEF,
	CTRL_DOWN_DEF,
	CTRL_LEFT_DEF,
	CTRL_RIGHT_DEF,
	CTRL_CIRCLE_DEF,
	CTRL_CROSS_DEF,
	CTRL_TRIANGLE_DEF,
	CTRL_SQUARE_DEF,
	CTRL_SELECT_DEF,
	CTRL_START_DEF,
	CTRL_L1_DEF,
	CTRL_R1_DEF,
	CTRL_L2_DEF,
	CTRL_R2_DEF,
	CTRL_L3_DEF,
	CTRL_R3_DEF,
	CTRL_L2_L3_DEF,
	CTRL_L2_R3_DEF,
	CTRL_L2_RSTICK_RIGHT_DEF,
	CTRL_L2_RSTICK_LEFT_DEF,
	CTRL_L2_RSTICK_UP_DEF,
	CTRL_L2_RSTICK_DOWN_DEF,
	CTRL_R2_RSTICK_RIGHT_DEF,
	CTRL_R2_RSTICK_LEFT_DEF,
	CTRL_R2_RSTICK_UP_DEF,
	CTRL_R2_RSTICK_DOWN_DEF,
	CTRL_R2_R3_DEF,
	CTRL_R3_L3_DEF,
	CTRL_RSTICK_UP_DEF,
	CTRL_RSTICK_DOWN_DEF,
	CTRL_RSTICK_LEFT_DEF,
	CTRL_RSTICK_RIGHT_DEF
};

#define BTN_DEF_MAX CTRL_RSTICK_RIGHT_DEF+1

extern uint32_t control_binds[MAX_PADS][BTN_DEF_MAX];
extern uint32_t default_control_binds[BTN_DEF_MAX];

#define Input_MapButton(buttonmap, next, defaultbutton) \
	if(defaultbutton == NULL) \
		buttonmap = Input_GetAdjacentButtonmap(buttonmap, next); \
	else \
		buttonmap = defaultbutton;

const char * Input_PrintMappedButton(uint32_t mappedbutton);
uint32_t Input_GetAdjacentButtonmap(uint32_t buttonmap, uint32_t next);

#ifdef __cplusplus
}
#endif
