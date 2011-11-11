/******************************************************************************* 
 * emu-ps3-constants.h - SNES9xNext PS3
 *
 *  Created on: Aug 19, 2011
********************************************************************************/

#ifndef EMUPS3_CONSTANTS_H_
#define EMUPS3_CONSTANT_H_

/* PS3 frontend - constants */

#define MAX_PATH_LENGTH			1024

#define MIN_SAVE_STATE_SLOT		0

#define CONFIG_FILE			0
#define SHADER_PRESET_FILE		1

#define EMULATOR_VERSION		"1.0"
#define EMULATOR_NAME			"SNES9xNext PS3"
#define EMULATOR_CONTENT_DIR		"SNES90000"

#define MODE_MENU		0
#define MODE_EMULATION		1
#define MODE_EXIT		2
#define MODE_MULTIMAN_STARTUP	3

// helpers
#ifdef CELL_DEBUG_CONSOLE
   #define cell_console_poll() cellConsolePoll();
#else
   #define cell_console_poll()
#endif


#define FILEBROWSER_DELAY         100000

/* PS3 frontend - file I/O */


#define extract_filename_only_without_extension(fname) \
	/* we get the filename path without a extension */ \
	char fname_without_extension[MAX_PATH_LENGTH]; \
	const char * fname_extension = strrchr(fname, '.'); \
	int characters_to_copy = strlen(fname) - strlen(fname_extension); \
	strncpy(fname_without_extension, fname, characters_to_copy); \
	extract_filename_only(fname_without_extension);

#define extract_filename_only(fname) \
	/* we get the file name only */ \
	const char * fname_without_filepath = strrchr(fname, '/'); \
	int offset = strlen(fname) - strlen(fname_without_filepath); \
	int fname_without_extension_length = strlen(fname); \
	char fname_without_path_extension[MAX_PATH_LENGTH]; \
	for(int i = offset+1, no = 0; i < fname_without_extension_length; i++, no++) \
	{ \
		fname_without_path_extension[no] = fname[i]; \
		fname_without_path_extension[no+1] = '\0'; \
	}

#define create_extension(fname, output) \
{ \
	/* we extract the extension from the filename(fname) */ \
	const char * fname_extension = strrchr(fname, '.'); \
	uint32_t offset = strlen(fname) - strlen(fname_extension); \
	int fname_length = strlen(fname); \
	for(int i = offset+1, no = 0; i < fname_length; i++, no++) \
	{ \
		output[no] = fname[i]; \
		output[no+1] = '\0'; \
	} \
}

#define create_filename_only_without_extension(fname, output) \
{ \
	const char * fname_extension = strrchr(fname, '.'); \
	char * fname_without_filepath_and_extension = strrchr(fname, '/'); \
	uint32_t offset = strlen(fname_without_filepath_and_extension) - strlen(fname_extension); \
	uint32_t length = strlen(fname_without_filepath_and_extension) + strlen(fname_extension); \
	for(int i = offset+1; i < length; i++) \
		fname_without_filepath_and_extension[i] = '\0'; \
	snprintf(output, sizeof(output), "%s", fname_without_filepath_and_extension); \
}

#define create_filename_only(fname, output) \
{ \
	char * fname_without_filepath = strrchr(fname, '/'); \
	memmove(fname_without_filepath, fname_without_filepath+1, strlen(fname_without_filepath)); \
	snprintf(output, sizeof(output), "%s", fname_without_filepath); \
}

#define create_basedir(fname, output) \
{ \
	char * split = strrchr(fname, '/'); \
	if(!split) split = strrchr(fname, '\\'); \
	uint32_t length_filename = strlen(fname); \
	uint32_t length_split = strlen(split); \
	snprintf(output, sizeof(output), "%.*s", length_filename-length_split, fname); \
}

#endif
