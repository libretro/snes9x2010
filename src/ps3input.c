/******************************************************************************* 
 * ps3input.c - SNES9x PS3
 *
 *  Created on: Dec, 2010
********************************************************************************/
#include "ps3input.h"

const char * Input_PrintMappedButton(uint32_t mappedbutton)
{
	switch(mappedbutton)
	{
		case BTN_A:
			return "Button A/Scope ToggleTurbo";
		case BTN_B:
			return "Button B/Scope Cursor Button";
		case BTN_X:
			return "Button X/Mouse R/Scope AimOffScreen";
		case BTN_Y:
			return "Button Y/Mouse L/Scope Fire";
		case BTN_SELECT:
			return "Button Select";
		case BTN_START:
			return "Button Start/Scope Pause";
		case BTN_L:
			return "L button";
		case BTN_R:
			return "R button";
		case BTN_LEFT:
			return "D-Pad Left";
		case BTN_RIGHT:
			return "D-Pad Right";
		case BTN_UP:
			return "D-Pad Up";
		case BTN_DOWN:
			return "D-Pad Down";
		case BTN_QUICKSAVE:
			return "Save State";
		case BTN_QUICKLOAD:
			return "Load State";
		case BTN_INCREMENTSAVE:
			return "Increment savestate slot";
		case BTN_DECREMENTSAVE:
			return "Decrement savestate slot";
		case BTN_INCREMENTCHEAT:
			return "Increment cheat position";
		case BTN_DECREMENTCHEAT:
			return "Decrement cheat position";
		case BTN_CHEATENABLE:
			return "Enable selected cheat";
		case BTN_CHEATDISABLE:
			return "Disable selected cheat";
		case BTN_CHEATTOGGLE:
			return "Toggle selected cheat";
		case BTN_CHEATINPUT:
			return "Input cheatcode";
		case BTN_CHEATINPUTLABEL:
			return "Input cheatcode label";
		case BTN_EXITTOMENU:
			return "Exit to menu";
		case BTN_NONE:
			return "None";
		case BTN_SOFTRESET:
			return "Software Reset";
		case BTN_RESET:
			return "Reset";
		case BTN_PAUSE:
			return "Pause";
		case BTN_FASTFORWARD:
			return "Fast forward";
		case BTN_INCREMENTTURBO:
			return "Increment Fast-forward speed";
		case BTN_DECREMENTTURBO:
			return "Decrement Fast-forward speed";
		case BTN_SWAPJOYPADS:
			return "Swap Joypads";
		case BTN_SRAM_WRITEPROTECT:
			return "SRAM Write-Protect";
		case BTN_INGAME_MENU:
			return "Ingame Menu";
		default:
			return "Unknown";
	}
}

//bool next: true is next, false is previous
uint32_t Input_GetAdjacentButtonmap(uint32_t buttonmap, uint32_t next)
{
	switch(buttonmap)
	{
		case BTN_UP:
			return next ? BTN_DOWN : BTN_NONE;
		case BTN_DOWN:
			return next ? BTN_LEFT : BTN_UP;
		case BTN_LEFT:
			return next ? BTN_RIGHT : BTN_DOWN;
		case BTN_RIGHT:
			return next ?  BTN_A : BTN_LEFT;
		case BTN_A:
			return next ? BTN_B : BTN_RIGHT;
		case BTN_B:
			return next ? BTN_X : BTN_A;
		case BTN_X:
			return next ? BTN_Y : BTN_B;
		case BTN_Y:
			return next ? BTN_SELECT : BTN_X;
		case BTN_SELECT:
			return next ? BTN_START : BTN_Y;
		case BTN_START:
			return next ? BTN_L : BTN_SELECT;
		case BTN_L:
			return next ? BTN_R : BTN_START;
		case BTN_R:
			return next ? BTN_QUICKSAVE : BTN_L;
		case BTN_QUICKSAVE:
			return next ? BTN_QUICKLOAD : BTN_R;
		case BTN_QUICKLOAD:
			return next ? BTN_INCREMENTCHEAT : BTN_QUICKSAVE;
		case BTN_INCREMENTCHEAT:
			return next ? BTN_DECREMENTCHEAT : BTN_QUICKLOAD;
		case BTN_DECREMENTCHEAT:
			return next ? BTN_EXITTOMENU : BTN_INCREMENTCHEAT;
		case BTN_EXITTOMENU:
			return next ? BTN_CHEATENABLE : BTN_DECREMENTCHEAT;
		case BTN_CHEATENABLE:
			return next ? BTN_CHEATDISABLE : BTN_EXITTOMENU;
		case BTN_CHEATDISABLE:
			return next ? BTN_CHEATTOGGLE : BTN_CHEATENABLE;
		case BTN_CHEATTOGGLE:
			return next ? BTN_CHEATINPUT : BTN_CHEATDISABLE;
		case BTN_CHEATINPUT:
			return next ? BTN_CHEATINPUTLABEL : BTN_CHEATTOGGLE;
		case BTN_CHEATINPUTLABEL:
			return next ? BTN_DECREMENTSAVE : BTN_CHEATINPUT;
		case BTN_DECREMENTSAVE:
			return next ? BTN_INCREMENTSAVE : BTN_CHEATINPUTLABEL;
		case BTN_INCREMENTSAVE:
			return next ? BTN_FASTFORWARD : BTN_DECREMENTSAVE;
		case BTN_FASTFORWARD:
			return next ? BTN_DECREMENTTURBO : BTN_INCREMENTSAVE;
		case BTN_DECREMENTTURBO:
			return next ? BTN_INCREMENTTURBO : BTN_FASTFORWARD;
		case BTN_INCREMENTTURBO:
			return next ? BTN_RESET : BTN_DECREMENTTURBO;
		case BTN_RESET:
			return next ? BTN_SOFTRESET : BTN_INCREMENTTURBO;
		case BTN_SOFTRESET:
			return next ? BTN_PAUSE : BTN_RESET;
		case BTN_PAUSE:
			return next ? BTN_SWAPJOYPADS : BTN_SOFTRESET;
		case BTN_SWAPJOYPADS:
			return next ? BTN_SRAM_WRITEPROTECT : BTN_PAUSE;
		case BTN_SRAM_WRITEPROTECT:
			return next ? BTN_INGAME_MENU : BTN_SWAPJOYPADS;
		case BTN_INGAME_MENU:
			return next ? BTN_NONE : BTN_SRAM_WRITEPROTECT;
		case BTN_NONE:
			return next ? BTN_UP : BTN_INGAME_MENU;
		default:
			return BTN_NONE;
	}
}
