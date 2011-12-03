/******************************************************************************* 
 *  -- Cellframework -  Open framework to abstract the common tasks related to
 *                      PS3 application development.
 *
 *  Copyright (C) 2010-2011
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

/******************************************************************************* 
 * FileBrowser.cpp - Cellframework
 *
 *  Created on:		Oct 29, 2010
 *  Last updated:	 
 ********************************************************************************/

#include <stdlib.h>
#include <algorithm>
#include "FileBrowser.hpp"

static bool less_than_key(CellFsDirent* a, CellFsDirent* b)
{
	// compare a directory to a file
	// directory is always lesser than
	if ((a->d_type == CELL_FS_TYPE_DIRECTORY && b->d_type == CELL_FS_TYPE_REGULAR))
		return true;
	else if (a->d_type == CELL_FS_TYPE_REGULAR && b->d_type == CELL_FS_TYPE_DIRECTORY)
		return false;

	return strcasecmp(a->d_name, b->d_name) < 0;
}

static const char * filebrowser_get_extension(const char * filename)
{
	const char * ext = strrchr(filename, '.');
	if (ext)
		return ext+1;
	else
		return "";
}

static bool filebrowser_parse_directory(filebrowser_t * filebrowser, const char * path, const char * extensions)
{
	int fd;

	// bad path
	if (strcmp(path,"") == 0)
		return false;

	// delete old path
	if (!filebrowser->cur.empty())
	{
		std::vector<CellFsDirent*>::const_iterator iter;
		for(iter = filebrowser->cur.begin(); iter != filebrowser->cur.end(); ++iter)
			delete (*iter);
		filebrowser->cur.clear();
	}

	// FIXME: add FsStat calls or use cellFsDirectoryEntry
	if (cellFsOpendir(path, &fd) == CELL_FS_SUCCEEDED)
	{
		uint64_t nread = 0;

		// set new dir
		strcpy(filebrowser->dir[filebrowser->directory_stack_size], path);

		// reset num entries
		filebrowser->file_count = 0;

		// reset currently selected variable for safety
		filebrowser->currently_selected = 0;

		// read the directory
		CellFsDirent dirent;
		while (cellFsReaddir(fd, &dirent, &nread) == CELL_FS_SUCCEEDED)
		{
			// no data read, something is wrong
			if (nread == 0)
				break;

			// if entry is not a file or directory, skip it
			if ((dirent.d_type != CELL_FS_TYPE_REGULAR) && (dirent.d_type != CELL_FS_TYPE_DIRECTORY))
				continue;

			// skip past the "." directory (current directory entry)
			if (dirent.d_type == CELL_FS_TYPE_DIRECTORY && !(strcmp(dirent.d_name, ".")))
				continue;

			// if entry is a file, check if extension is allowed 
			if (dirent.d_type == CELL_FS_TYPE_REGULAR)
			{
				char tmp_extensions[512];
				strncpy(tmp_extensions, extensions, sizeof(tmp_extensions));
				const char * current_extension = filebrowser_get_extension(dirent.d_name);
				bool found_rom = false;

				if(current_extension)
				{
					char * pch = strtok(tmp_extensions, "|");
					while (pch != NULL)
					{
						if(strcmp(current_extension, pch) == 0)
						{
							found_rom = true;
							break;
						}
						pch = strtok(NULL, "|");
					}
				}

				if(!found_rom)
					continue;
			}

			// AT THIS POINT WE HAVE A VALID ENTRY

			// allocate an entry
			CellFsDirent *entry = new CellFsDirent();
			memcpy(entry, &dirent, sizeof(CellFsDirent));

			filebrowser->cur.push_back(entry);

			// next file
			++filebrowser->file_count;
		}

		cellFsClosedir(fd);
	}
	else
		return false;

	std::sort(++filebrowser->cur.begin(), filebrowser->cur.end(), less_than_key);

	return true;
}

void filebrowser_new(filebrowser_t * filebrowser, const char * start_dir, const char * extensions)
{
	filebrowser->directory_stack_size = 0;
	strncpy(filebrowser->extensions, extensions, sizeof(filebrowser->extensions));

	filebrowser_parse_directory(filebrowser, start_dir, filebrowser->extensions);
}

void filebrowser_reset_start_directory(filebrowser_t * filebrowser, const char * start_dir, const char * extensions)
{
	std::vector<CellFsDirent*>::const_iterator iter;
	for(iter = filebrowser->cur.begin(); iter != filebrowser->cur.end(); ++iter)
		delete (*iter);
	filebrowser->cur.clear();
   
	filebrowser->directory_stack_size = 0;
	strncpy(filebrowser->extensions, extensions, sizeof(filebrowser->extensions));

	filebrowser_parse_directory(filebrowser, start_dir, filebrowser->extensions);
}

void filebrowser_push_directory(filebrowser_t * filebrowser, const char * path, bool with_extension)
{
	filebrowser->directory_stack_size++;
	if(with_extension)
		filebrowser_parse_directory(filebrowser, path, filebrowser->extensions);
	else
		filebrowser_parse_directory(filebrowser, path, "empty");
}

void filebrowser_pop_directory (filebrowser_t * filebrowser)
{
	if (filebrowser->directory_stack_size > 0)
		filebrowser->directory_stack_size--;

	filebrowser_parse_directory(filebrowser, filebrowser->dir[filebrowser->directory_stack_size], filebrowser->extensions);
}
