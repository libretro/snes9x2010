/******************************************************************************* 
 * menu.c - SNES9x PS3
 *
 *  Created on: Oct 10, 2010
********************************************************************************/

#include <cell/sysmodule.h>
#include <sysutil/sysutil_screenshot.h>

#include "cellframework2/input/pad_input.h"

/*emulator-specific*/
#include "../src/snes9x.h"
#include "emu-ps3.h"

#include "menu.h"

#include "cellframework2/fileio/file_browser.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NUM_ENTRY_PER_PAGE 19

#define ROM_EXTENSIONS "smc|fig|sfc|gd3|gd7|dx2|bsx|swc|zip|jma|SMC|FIG|SFC|BSX|GD3|GD7|DX2|SWC|ZIP|JMA"

menu menuStack[25];
int menuStackindex = 0;
uint32_t menu_is_running = false;		/* is the menu running?*/
bool update_item_colors = true;
static bool set_initial_dir_tmpbrowser;
filebrowser_t browser;				/* main file browser->for rom browser*/
filebrowser_t tmpBrowser;			/* tmp file browser->for everything else*/
uint32_t set_shader = 0;
static uint32_t currently_selected_controller_menu = 0;

#include "menu/menu-entries.h"

static menu menu_filebrowser = {
	"FILE BROWSER |",		/* title*/
	FILE_BROWSER_MENU,		/* enum*/
	0,				/* selected item*/
	0,				/* page*/
	1,				/* maxpages */
	1,				/* refreshpage*/
	NULL				/* items*/
};

static menu menu_generalvideosettings = {
	"VIDEO |",			/* title*/
	GENERAL_VIDEO_MENU,		/* enum*/
	FIRST_VIDEO_SETTING,		/* selected item*/
	0,				/* page*/
	MAX_NO_OF_VIDEO_SETTINGS/NUM_ENTRY_PER_PAGE,	/* max pages */
	1,				/* refreshpage*/
	FIRST_VIDEO_SETTING,		/* first setting*/
	MAX_NO_OF_VIDEO_SETTINGS,	/* max no of path settings*/
	items_generalsettings		/* items*/
};

static menu menu_generalaudiosettings = {
	"AUDIO |",			/* title*/
	GENERAL_AUDIO_MENU,		/* enum*/
	FIRST_AUDIO_SETTING,		/* selected item*/
	0,				/* page*/
	MAX_NO_OF_AUDIO_SETTINGS/NUM_ENTRY_PER_PAGE,	/* max pages */
	1,				/* refreshpage*/
	FIRST_AUDIO_SETTING,		/* first setting*/
	MAX_NO_OF_AUDIO_SETTINGS,	/* max no of path settings*/
	items_generalsettings		/* items*/
};

static menu menu_emu_settings = {
	"SNES9X |",						/* title*/
	EMU_GENERAL_MENU,					/* enum*/
	FIRST_EMU_SETTING,					/* selected item*/
	0,							/* page*/
	MAX_NO_OF_EMU_SETTINGS/NUM_ENTRY_PER_PAGE,		/* max pages*/
	1,                      				/* refreshpage*/
	FIRST_EMU_SETTING,					/* first setting*/
	MAX_NO_OF_EMU_SETTINGS,					/* max no of path settings*/
	items_generalsettings					/* items*/
};

static menu menu_emu_audiosettings = {
	"SNES9X AUDIO |",					/* title*/
	EMU_AUDIO_MENU,						/* enum*/
	FIRST_EMU_AUDIO_SETTING,				/* selected item*/
	0,							/* page*/
	MAX_NO_OF_EMU_AUDIO_SETTINGS/NUM_ENTRY_PER_PAGE,	/* max pages*/
	1,							/* refreshpage*/
	FIRST_EMU_AUDIO_SETTING,				/* first setting*/
	MAX_NO_OF_EMU_AUDIO_SETTINGS,				/* max no of path settings*/
	items_generalsettings					/* items*/
};

static menu menu_pathsettings = {
	"PATH |",						/* title*/
	PATH_MENU,						/* enum*/
	FIRST_PATH_SETTING,					/* selected item*/
	0,							/* page*/
	MAX_NO_OF_PATH_SETTINGS/NUM_ENTRY_PER_PAGE,		/* max pages*/
	1,							/* refreshpage*/
	FIRST_PATH_SETTING,					/* first setting*/
	MAX_NO_OF_PATH_SETTINGS,				/* max no of path settings*/
	items_generalsettings					/* items*/
};

static menu menu_controlssettings = {
	"CONTROLS |",						/* title*/
	CONTROLS_MENU,						/* enum*/
	FIRST_CONTROLS_SETTING_PAGE_1,				/* selected item*/
	0,							/* page*/
	MAX_NO_OF_CONTROLS_SETTINGS/NUM_ENTRY_PER_PAGE,		/* max pages*/
	1,							/* refreshpage*/
	FIRST_CONTROLS_SETTING_PAGE_1,				/* first setting*/
	MAX_NO_OF_CONTROLS_SETTINGS,				/* max no of path settings*/
	items_generalsettings					/* items*/
};

static void produce_menubar(uint32_t menu_enum)
{
	cellDbgFontPuts    (0.09f,  0.05f,  Emulator_GetFontSize(),  menu_enum == GENERAL_VIDEO_MENU ? RED : GREEN,   menu_generalvideosettings.title);
	cellDbgFontPuts    (0.19f,  0.05f,  Emulator_GetFontSize(),  menu_enum == GENERAL_AUDIO_MENU ? RED : GREEN,  menu_generalaudiosettings.title);
	cellDbgFontPuts    (0.29f,  0.05f,  Emulator_GetFontSize(),  menu_enum == EMU_GENERAL_MENU ? RED : GREEN,  menu_emu_settings.title);
	cellDbgFontPuts    (0.40f,  0.05f,  Emulator_GetFontSize(),  menu_enum == EMU_AUDIO_MENU ? RED : GREEN,   menu_emu_audiosettings.title);
	cellDbgFontPuts    (0.60f,  0.05f,  Emulator_GetFontSize(),  menu_enum == PATH_MENU ? RED : GREEN,  menu_pathsettings.title);
	cellDbgFontPuts    (0.70f,  0.05f,  Emulator_GetFontSize(), menu_enum == CONTROLS_MENU ? RED : GREEN,  menu_controlssettings.title); 
	cellDbgFontDraw();
}

static void UpdateBrowser(filebrowser_t * b)
{
	static uint64_t old_state = 0;
	uint64_t state, diff_state, button_was_pressed;

	state = cell_pad_input_poll_device(0);
	diff_state = old_state ^ state;
	button_was_pressed = old_state & diff_state;

	if(frame_count < special_action_msg_expired)
	{
	}
	else
	{
		if (CTRL_LSTICK_DOWN(state))
		{
			if(b->currently_selected < b->file_count-1)
			{
				FILEBROWSER_INCREMENT_ENTRY_POINTER(b);
				set_text_message("", 4);
			}
		}

		if (CTRL_DOWN(state))
		{
			if(b->currently_selected < b->file_count-1)
			{
				FILEBROWSER_INCREMENT_ENTRY_POINTER(b);
				set_text_message("", 7);
			}
		}

		if (CTRL_LSTICK_UP(state))
		{
			if(b->currently_selected > 0)
			{
				FILEBROWSER_DECREMENT_ENTRY_POINTER(b);
				set_text_message("", 4);
			}
		}

		if (CTRL_UP(state))
		{
			if(b->currently_selected > 0)
			{
				FILEBROWSER_DECREMENT_ENTRY_POINTER(b);
				set_text_message("", 7);
			}
		}

		if (CTRL_RIGHT(state))
		{
			b->currently_selected = (MIN(b->currently_selected + 5, b->file_count-1));
			set_text_message("", 7);
		}

		if (CTRL_LSTICK_RIGHT(state))
		{
			b->currently_selected = (MIN(b->currently_selected + 5, b->file_count-1));
			set_text_message("", 4);
		}

		if (CTRL_LEFT(state))
		{
			if (b->currently_selected <= 5)
				b->currently_selected = 0;
			else
				b->currently_selected -= 5;

			set_text_message("", 7);
		}

		if (CTRL_LSTICK_LEFT(state))
		{
			if (b->currently_selected <= 5)
				b->currently_selected = 0;
			else
				b->currently_selected -= 5;

			set_text_message("", 4);
		}

		if (CTRL_R1(state))
		{
			b->currently_selected = (MIN(b->currently_selected + NUM_ENTRY_PER_PAGE, b->file_count-1));
			set_text_message("", 7);
		}

		if (CTRL_L1(state))
		{
			if (b->currently_selected <= NUM_ENTRY_PER_PAGE)
				b->currently_selected= 0;
			else
				b->currently_selected -= NUM_ENTRY_PER_PAGE;

			set_text_message("", 7);
		}

		if (CTRL_CIRCLE(button_was_pressed))
		{
			old_state = state;
			filebrowser_pop_directory(b);
		}


		if (CTRL_L3(state) && CTRL_R3(state))
		{
			/* if a rom is loaded then resume it */
			if (Emulator_IsROMLoaded())
			{
				menu_is_running = 0;
				Emulator_StartROMRunning(1);
				set_text_message("", 15);
			}
		}

		old_state = state;
	}
}

static void RenderBrowser(filebrowser_t * b)
{
	uint32_t file_count = b->file_count;
	int current_index, page_number, page_base, i;
	float currentX, currentY, ySpacing;

	current_index = b->currently_selected;
	page_number = current_index / NUM_ENTRY_PER_PAGE;
	page_base = page_number * NUM_ENTRY_PER_PAGE;

	currentX = 0.09f;
	currentY = 0.09f;
	ySpacing = 0.035f;

	for ( i = page_base; i < file_count && i < page_base + NUM_ENTRY_PER_PAGE; ++i)
	{
		currentY = currentY + ySpacing;
		cellDbgFontPuts(currentX, currentY, Emulator_GetFontSize(), i == current_index ? RED : b->cur[i].d_type == CELL_FS_TYPE_DIRECTORY ? GREEN : WHITE, b->cur[i].d_name);
		cellDbgFontDraw();
	}
	cellDbgFontDraw();
}


#include "menu/menu-choice.h"
#include "menu/menu-helpmessages.h"
#include "menu/settings-logic.h"

#define do_controls_refreshpage(beginvalue, endvalue) \
{ \
	float increment = 0.13f; \
	for (int i= beginvalue; i < endvalue; i++) \
	{ \
		menu_controlssettings.items[i].text_xpos = 0.09f; \
		menu_controlssettings.items[i].text_ypos = increment;  \
		increment += 0.03f; \
	} \
	menu_controlssettings.refreshpage = 0; \
}

#define FIRST_CONTROLS_SETTING_PAGE_2 menu_controlssettings.items[FIRST_CONTROLS_SETTING_PAGE_1+18].enum_id
#define FIRST_CONTROLS_SETTING_PAGE_3 menu_controlssettings.items[FIRST_CONTROLS_SETTING_PAGE_1+36].enum_id

#define IS_PAGE(x) (x/NUM_ENTRY_PER_PAGE)

static void do_controls_settings(void)
{
	uint64_t state, diff_state, button_was_pressed;
	static uint64_t old_state = 0;

	state = cell_pad_input_poll_device(0);
	diff_state = old_state ^ state;
	button_was_pressed = old_state & diff_state;

	if(frame_count < special_action_msg_expired)
	{
	}
	else
	{
		/* back to ROM menu if CIRCLE is pressed*/
		if (CTRL_L1(button_was_pressed) || CTRL_CIRCLE(button_was_pressed))
		{
			menuStackindex--;
			old_state = state;
			return;
		}

		if (CTRL_DOWN(state)  || CTRL_LSTICK_DOWN(state))	/* down to next setting*/
		{
			menu_controlssettings.selected++;

			if (menu_controlssettings.selected >= MAX_NO_OF_CONTROLS_SETTINGS)
				menu_controlssettings.selected = FIRST_CONTROLS_SETTING_PAGE_1;

			if (menu_controlssettings.items[menu_controlssettings.selected].page != menu_controlssettings.page)
				menu_controlssettings.page = menu_controlssettings.items[menu_controlssettings.selected].page;

			set_text_message("", 7);

		}

		if (CTRL_UP(state)  || CTRL_LSTICK_UP(state))	/* up to previous setting*/
		{
			menu_controlssettings.selected--;

			if (menu_controlssettings.selected < FIRST_CONTROLS_SETTING_PAGE_1)
				menu_controlssettings.selected = MAX_NO_OF_CONTROLS_SETTINGS-1;

			if (menu_controlssettings.items[menu_controlssettings.selected].page != menu_controlssettings.page)
				menu_controlssettings.page = menu_controlssettings.items[menu_controlssettings.selected].page;

			set_text_message("", 7);
		}

		if (menu_controlssettings.refreshpage)
		{
			int i, j, page;
			float increment;

			page = 0;

			i = menu_controlssettings.first_setting;

			do{
				for ( j = 0, increment = 0.13f; j < (NUM_ENTRY_PER_PAGE); j++)
				{
					menu_controlssettings.items[i].text_xpos = 0.09f;
					menu_controlssettings.items[i].text_ypos = increment; 
					menu_controlssettings.items[i].page = page;
					increment += 0.03f;
					i++;
				}
				page++;
			}while(i < menu_controlssettings.max_settings);
			menu_controlssettings.refreshpage = 0;
		}

		if (CTRL_L3(state) && CTRL_R3(state))
		{
			/* if a rom is loaded then resume it*/
			if (Emulator_IsROMLoaded())
			{
				menu_is_running = 0;
				Emulator_StartROMRunning(1);
				set_text_message("", 15);
			}
			old_state = state;
			return;
		}

		producesettingentry(menu_controlssettings.selected);
	}

	produce_menubar(menu_controlssettings.enum_id);
	cellDbgFontDraw();

	for(int i = SETTING_CONTROLS_SCHEME; i < SETTING_CONTROLS_DEFAULT_ALL+1; i++)
	{
		if(menu_controlssettings.items[i].page == menu_controlssettings.page)
		{
			cellDbgFontPuts(0.09f,	menu_controlssettings.items[i].text_ypos,	Emulator_GetFontSize(),	menu_controlssettings.selected == menu_controlssettings.items[i].enum_id ? YELLOW : WHITE,	menu_controlssettings.items[i].text);
			producelabelvalue(i);
			cellDbgFontDraw();
		}
	}

	cellDbgFontDraw();

	DisplayHelpMessage(menu_controlssettings.selected);

	cellDbgFontPuts(0.09f, 0.91f, Emulator_GetFontSize(), YELLOW,
	"UP/DOWN - select  L3+R3 - resume game   X/LEFT/RIGHT - change");
	cellDbgFontPuts(0.09f, 0.95f, Emulator_GetFontSize(), YELLOW,
	"START - default   L1/CIRCLE - go back");
	cellDbgFontDraw();
	old_state = state;
}


static void do_settings(menu * menu_obj)
{
	uint64_t state, diff_state, button_was_pressed, i, j;
	static uint64_t old_state = 0;
	float increment;

	state = cell_pad_input_poll_device(0);
	diff_state = old_state ^ state;
	button_was_pressed = old_state & diff_state;

	if(update_item_colors)
	{
		toggle_settings_items();
	}


	if (menu_obj->refreshpage)
	{
		int page;
		
		page = 0;

		i = menu_obj->first_setting;

		do{
			for ( j = 0, increment = 0.13f; j < (NUM_ENTRY_PER_PAGE); j++)
			{
				menu_obj->items[i].text_xpos = 0.09f;
				menu_obj->items[i].text_ypos = increment; 
				menu_obj->items[i].page = page;
				increment += 0.03f;
				i++;
			}
			page++;
		}while(i < menu_obj->max_settings);
		menu_obj->refreshpage = 0;
	}

	if(frame_count < special_action_msg_expired)
	{
	}
	else
	{
		/* back to ROM menu if CIRCLE is pressed */
		if (CTRL_L1(button_was_pressed) || CTRL_CIRCLE(button_was_pressed))
		{
			menuStackindex--;
			old_state = state;
			return;
		}

		if (CTRL_R1(button_was_pressed))
		{
			menuStackindex++;
			switch(menu_obj->enum_id)
			{
				case GENERAL_VIDEO_MENU:
					menuStack[menuStackindex] = menu_generalaudiosettings;
					old_state = state;
					break;
				case GENERAL_AUDIO_MENU:
					menuStack[menuStackindex] = menu_emu_settings;
					old_state = state;
					break;
				case EMU_GENERAL_MENU:
					menuStack[menuStackindex] = menu_emu_audiosettings;
					old_state = state;
					break;
				case EMU_AUDIO_MENU:
					menuStack[menuStackindex] = menu_pathsettings;
					old_state = state;
					break;
				case PATH_MENU:
					menuStack[menuStackindex] = menu_controlssettings;
					old_state = state;
					break;
			}
		}

		if (CTRL_DOWN(state) || CTRL_LSTICK_DOWN(state))	/* down to next setting */
		{
			menu_obj->selected++;

			while(menu_obj->items[menu_obj->selected].enabled == 0)
				menu_obj->selected++;

			if (menu_obj->selected >= menu_obj->max_settings)
				menu_obj->selected = menu_obj->first_setting; 

			if (menu_obj->items[menu_obj->selected].page != menu_obj->page)
				menu_obj->page = menu_obj->items[menu_obj->selected].page;

			set_text_message("", 7);
		}

		if (CTRL_UP(state) || CTRL_LSTICK_UP(state))	/* up to previous setting */
		{
			if (menu_obj->selected == menu_obj->first_setting)
				menu_obj->selected = menu_obj->max_settings-1;
			else
				menu_obj->selected--;

			while (menu_obj->items[menu_obj->selected].enabled == 0)
				menu_obj->selected--;

			if (menu_obj->items[menu_obj->selected].page != menu_obj->page)
				menu_obj->page = menu_obj->items[menu_obj->selected].page;

			set_text_message("", 7);
		}

		if (CTRL_L3(state) && CTRL_R3(state))
		{
			/* if a rom is loaded then resume it */
			if (Emulator_IsROMLoaded())
			{
				menu_is_running = 0;
				Emulator_StartROMRunning(1);
				set_text_message("", 15);
			}
			old_state = state;
			return;
		}


		producesettingentry(menu_obj->selected);
	}

	produce_menubar(menu_obj->enum_id);
	cellDbgFontDraw();

	for ( i = menu_obj->first_setting; i < menu_obj->max_settings; i++)
	{
		if(menu_obj->items[i].page == menu_obj->page)
		{
			cellDbgFontPuts(menu_obj->items[i].text_xpos, menu_obj->items[i].text_ypos, Emulator_GetFontSize(), menu_obj->selected == menu_obj->items[i].enum_id ? menu_obj->items[i].text_selected_color : menu_obj->items[i].text_unselected_color, menu_obj->items[i].text);
			producelabelvalue(i);
		}
	}

	DisplayHelpMessage(menu_obj->selected);

	cellDbgFontPuts(0.09f, 0.91f, Emulator_GetFontSize(), YELLOW, "UP/DOWN - select  L3+R3 - resume game   X/LEFT/RIGHT - change");
	cellDbgFontPuts(0.09f, 0.95f, Emulator_GetFontSize(), YELLOW, "START - default   L1/CIRCLE - go back   R1 - go forward");
	cellDbgFontDraw();
	old_state = state;
}

static void do_ROMMenu(void)
{
	char rom_path[MAX_PATH_LENGTH], newpath[1024], *separatorslash;
	uint64_t state, diff_state, button_was_pressed;
	static uint64_t old_state = 0;

	state = cell_pad_input_poll_device(0);
	diff_state = old_state ^ state;
	button_was_pressed = old_state & diff_state;

	UpdateBrowser(&browser);

	if (CTRL_SELECT(button_was_pressed))
	{
		menuStackindex++;
		menuStack[menuStackindex] = menu_generalvideosettings;
	}

	if (CTRL_START(button_was_pressed))
		filebrowser_reset_start_directory(&browser, "/", ROM_EXTENSIONS);

	if (CTRL_CROSS(button_was_pressed))
	{
		if(FILEBROWSER_IS_CURRENT_A_DIRECTORY(browser))
		{
			/*if 'filename' is in fact '..' - then pop back directory 
			instead of adding '..' to filename path */

			if(browser.currently_selected == 0)
			{
				old_state = state;
				filebrowser_pop_directory(&browser);
			}
			else
			{
				separatorslash = (strcmp(FILEBROWSER_GET_CURRENT_DIRECTORY_NAME(browser),"/") == 0) ? "" : "/";
				snprintf(newpath, sizeof(newpath), "%s%s%s", FILEBROWSER_GET_CURRENT_DIRECTORY_NAME(browser), separatorslash, FILEBROWSER_GET_CURRENT_FILENAME(browser));
				filebrowser_push_directory(&browser, newpath, true);
			}
		}
		else if (FILEBROWSER_IS_CURRENT_A_FILE(browser))
		{
			snprintf(rom_path, sizeof(rom_path), "%s/%s", FILEBROWSER_GET_CURRENT_DIRECTORY_NAME(browser), FILEBROWSER_GET_CURRENT_FILENAME(browser));

			menu_is_running = 0;

			/* switch emulator to emulate mode*/
			Emulator_StartROMRunning(1);

			Emulator_RequestLoadROM(rom_path);

			old_state = state;
			return;
		}
	}


	if (FILEBROWSER_IS_CURRENT_A_DIRECTORY(browser))
	{
		if(!strcmp(FILEBROWSER_GET_CURRENT_FILENAME(browser),"app_home") || !strcmp(FILEBROWSER_GET_CURRENT_FILENAME(browser),"host_root"))
			cellDbgFontPrintf(0.09f, 0.83f, 0.91f, RED, "WARNING - This path only works on DEX PS3 systems. Do not attempt to open\n this directory on CEX PS3 systems, or you might have to restart!");
		else if(!strcmp(FILEBROWSER_GET_CURRENT_FILENAME(browser),".."))
			cellDbgFontPrintf(0.09f, 0.83f, 0.91f, LIGHTBLUE, "INFO - Press X to go back to the previous directory.");
		else
			cellDbgFontPrintf(0.09f, 0.83f, 0.91f, LIGHTBLUE, "INFO - Press X to enter the directory.");
	}

	if (FILEBROWSER_IS_CURRENT_A_FILE(browser))
		cellDbgFontPrintf(0.09f, 0.83f, 0.91f, LIGHTBLUE, "INFO - Press X to load the game. ");

	cellDbgFontPuts	(0.09f,	0.05f,	Emulator_GetFontSize(),	RED,	"FILE BROWSER");
	cellDbgFontPrintf (0.7f, 0.05f, 0.82f, WHITE, "%s v%s", EMULATOR_NAME, EMULATOR_VERSION);
	cellDbgFontPrintf (0.09f, 0.09f, Emulator_GetFontSize(), YELLOW,
	"PATH: %s", FILEBROWSER_GET_CURRENT_DIRECTORY_NAME(browser));
	cellDbgFontPuts   (0.09f, 0.93f, Emulator_GetFontSize(), YELLOW,
	"L3 + R3 - resume game           SELECT - Settings screen");
	cellDbgFontDraw();

	RenderBrowser(&browser);
	old_state = state;
}

void MenuInit(void)
{
	filebrowser_new(&browser, Settings.PS3PathROMDirectory, ROM_EXTENSIONS);
}

void MenuMainLoop(void)
{
	menuStack[0] = menu_filebrowser;
	menuStack[0].enum_id = FILE_BROWSER_MENU;

	menu_is_running = true;

	do
	{
		glClear(GL_COLOR_BUFFER_BIT);
		ps3graphics_draw_menu();

		switch(menuStack[menuStackindex].enum_id)
		{
			case FILE_BROWSER_MENU:
				do_ROMMenu();
				break;
			case GENERAL_VIDEO_MENU:
			case GENERAL_AUDIO_MENU:
			case EMU_GENERAL_MENU:
			case EMU_AUDIO_MENU:
			case PATH_MENU:
				do_settings(&menuStack[menuStackindex]);
				break;
			case CONTROLS_MENU:
				do_controls_settings();
				break;
			case GAME_AWARE_SHADER_CHOICE:
			case SHADER_CHOICE:
			case PRESET_CHOICE:
			case BORDER_CHOICE:
			case INPUT_PRESET_CHOICE:
				do_select_file(menuStack[menuStackindex].enum_id);
				break;
			case PATH_SAVESTATES_DIR_CHOICE:
			case PATH_DEFAULT_ROM_DIR_CHOICE:
			case PATH_CHEATS_DIR_CHOICE:
			case PATH_SRAM_DIR_CHOICE:
				do_pathChoice(menuStack[menuStackindex].enum_id);
				break;
		}

		psglSwap();
		cell_console_poll();
		cellSysutilCheckCallback();
	}while (menu_is_running);
}
