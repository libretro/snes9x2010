/******************************************************************************* 
 *  -- Cellframework -  Open framework to abstract the common tasks related to
 *                      PS3 application development.
 *
 *  Copyright (C) 2010
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ********************************************************************************/

#ifndef FILEBROWSER_H_
#define FILEBROWSER_H_

#define MAXJOLIET	255
#define MAX_PATH_LENGTH	1024
#define STRING_NPOS	-1

#include <cell/cell_fs.h>

#include <string>
#include <string.h>
#include <vector>

#include <sys/types.h>

#include "../../emu-ps3-constants.h"

struct DirectoryInfo
{
	char dir[CELL_FS_MAX_FS_PATH_LENGTH];
	std::string extensions;
};


typedef struct
{
        uint32_t file_count;		// amount of files in current dir
	uint32_t currently_selected;	// currently select browser entry
	uint32_t directory_stack_size;
	DirectoryInfo dir[128];		// info of the current directory
	std::vector<CellFsDirent*> cur;	// current file listing
} filebrowser_t;

void filebrowser_new(filebrowser_t * filebrowser, const char * start_dir, std::string extensions);
void filebrowser_reset_start_directory(filebrowser_t * filebrowser, const char * start_dir, std::string extensions);
void filebrowser_push_directory(filebrowser_t * filebrowser, const char * path, std::string extensions);
void filebrowser_pop_directory (filebrowser_t * filebrowser);

#define FILEBROWSER_GET_CURRENT_DIRECTORY_NAME(filebrowser) (filebrowser.dir[filebrowser.directory_stack_size].dir)
#define FILEBROWSER_GET_CURRENT_DIRECTORY_FILE_COUNT(filebrowser) (filebrowser.dir[filebrowser.directory_stack_size].file_count)
#define FILEBROWSER_GOTO_ENTRY(filebrowser, i)	filebrowser.currently_selected = i;

#define FILEBROWSER_INCREMENT_ENTRY(filebrowser) \
{ \
	filebrowser.currently_selected++; \
	if (filebrowser.currently_selected >= filebrowser.cur.size()) \
		filebrowser.currently_selected = 0; \
}

#define FILEBROWSER_INCREMENT_ENTRY_POINTER(filebrowser) \
{ \
	filebrowser->currently_selected++; \
	if (filebrowser->currently_selected >= filebrowser->cur.size()) \
		filebrowser->currently_selected = 0; \
}

#define FILEBROWSER_DECREMENT_ENTRY(filebrowser) \
{ \
	filebrowser.currently_selected--; \
	if (filebrowser.currently_selected >= filebrowser.cur.size()) \
		filebrowser.currently_selected = filebrowser.cur.size() - 1; \
}

#define FILEBROWSER_DECREMENT_ENTRY_POINTER(filebrowser) \
{ \
	filebrowser->currently_selected--; \
	if (filebrowser->currently_selected >= filebrowser->cur.size()) \
		filebrowser->currently_selected = filebrowser->cur.size() - 1; \
}

#define FILEBROWSER_GET_CURRENT_FILENAME(filebrowser) (filebrowser.cur[filebrowser.currently_selected]->d_name)
#define FILEBROWSER_GET_CURRENT_ENTRY_INDEX(filebrowser) (filebrowser.currently_selected)
#define FILEBROWSER_IS_CURRENT_A_FILE(filebrowser)	(filebrowser.cur[filebrowser.currently_selected]->d_type == CELL_FS_TYPE_REGULAR)
#define FILEBROWSER_IS_CURRENT_A_DIRECTORY(filebrowser)	(filebrowser.cur[filebrowser.currently_selected]->d_type == CELL_FS_TYPE_DIRECTORY)

#endif /* FILEBROWSER_H_ */
