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

typedef struct{
	uint32_t		DPad_Up[MAX_PADS];
	uint32_t		DPad_Down[MAX_PADS];
	uint32_t		DPad_Left[MAX_PADS];
	uint32_t		DPad_Right[MAX_PADS];
	uint32_t		ButtonCircle[MAX_PADS];
	uint32_t		ButtonCross[MAX_PADS];
	uint32_t		ButtonTriangle[MAX_PADS];
	uint32_t		ButtonSquare[MAX_PADS];
	uint32_t		ButtonSelect[MAX_PADS];
	uint32_t		ButtonStart[MAX_PADS];
	uint32_t		ButtonL1[MAX_PADS];
	uint32_t		ButtonR1[MAX_PADS];
	uint32_t		ButtonL2[MAX_PADS];
	uint32_t		ButtonR2[MAX_PADS];
	uint32_t		ButtonL3[MAX_PADS];
	uint32_t		ButtonR3[MAX_PADS];
	uint32_t		ButtonL2_ButtonL3[MAX_PADS];
	uint32_t		ButtonL2_ButtonR3[MAX_PADS];
	uint32_t		ButtonL2_AnalogR_Right[MAX_PADS];
	uint32_t		ButtonL2_AnalogR_Left[MAX_PADS];
	uint32_t		ButtonL2_AnalogR_Up[MAX_PADS];
	uint32_t		ButtonL2_AnalogR_Down[MAX_PADS];
	uint32_t		ButtonR2_AnalogR_Right[MAX_PADS];
	uint32_t		ButtonR2_AnalogR_Left[MAX_PADS];
	uint32_t		ButtonR2_AnalogR_Up[MAX_PADS];
	uint32_t		ButtonR2_AnalogR_Down[MAX_PADS];
	uint32_t		ButtonR2_ButtonR3[MAX_PADS];
	uint32_t		ButtonR3_ButtonL3[MAX_PADS];
	uint32_t		AnalogR_Up[MAX_PADS];
	uint32_t		AnalogR_Down[MAX_PADS];
	uint32_t		AnalogR_Left[MAX_PADS];
	uint32_t		AnalogR_Right[MAX_PADS];
	uint32_t		AnalogR_Up_Type[MAX_PADS];
	uint32_t		AnalogR_Down_Type[MAX_PADS];
	uint32_t		AnalogR_Left_Type[MAX_PADS];
	uint32_t		AnalogR_Right_Type[MAX_PADS];
	uint32_t		AnalogL_Left[MAX_PADS];
	uint32_t		AnalogL_Right[MAX_PADS];
	uint32_t		AnalogL_Up[MAX_PADS];
	uint32_t		AnalogL_Down[MAX_PADS];
} PS3InputList;

#define Input_MapButton(buttonmap, next, defaultbutton) \
	if(defaultbutton == NULL) \
		buttonmap = Input_GetAdjacentButtonmap(buttonmap, next); \
	else \
		buttonmap = defaultbutton;

extern PS3InputList PS3Input;

const char * Input_PrintMappedButton(uint32_t mappedbutton);
uint32_t Input_GetAdjacentButtonmap(uint32_t buttonmap, uint32_t next);

#ifdef __cplusplus
}
#endif
