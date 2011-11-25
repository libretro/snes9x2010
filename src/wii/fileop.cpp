/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * fileop.cpp
 *
 * File operations
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogcsys.h>
#include <dirent.h>
#include <sys/stat.h>
#include <zlib.h>
#include <malloc.h>
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include <ogc/usbstorage.h>
#include <di/di.h>
#include <ogc/dvd.h>
#include <iso9660.h>

#include "snes9xgx.h"
#include "fileop.h"
#include "menu.h"
#include "filebrowser.h"
#include "gui/gui.h"

#define THREAD_SLEEP 100

unsigned char *savebuffer = NULL;
static mutex_t bufferLock = LWP_MUTEX_NULL;
FILE * file; // file pointer - the only one we should ever use!
bool unmountRequired[7] = { false, false, false, false, false, false, false };
bool isMounted[7] = { false, false, false, false, false, false, false };

const DISC_INTERFACE* sd = &__io_wiisd;
const DISC_INTERFACE* usb = &__io_usbstorage;
const DISC_INTERFACE* dvd = &__io_wiidvd;

// folder parsing thread
static lwp_t parsethread = LWP_THREAD_NULL;
static DIR *dir = NULL;
static bool parseHalt = true;
static bool parseFilter = true;
static bool ParseDirEntries();
int selectLoadedFile = 0;

// device thread
static lwp_t devicethread = LWP_THREAD_NULL;
static bool deviceHalt = true;

#define ZIPCHUNK 2048

/*
 * Zip file header definition
 */
typedef struct
{
  unsigned int zipid __attribute__ ((__packed__));	// 0x04034b50
  unsigned short zipversion __attribute__ ((__packed__));
  unsigned short zipflags __attribute__ ((__packed__));
  unsigned short compressionMethod __attribute__ ((__packed__));
  unsigned short lastmodtime __attribute__ ((__packed__));
  unsigned short lastmoddate __attribute__ ((__packed__));
  unsigned int crc32 __attribute__ ((__packed__));
  unsigned int compressedSize __attribute__ ((__packed__));
  unsigned int uncompressedSize __attribute__ ((__packed__));
  unsigned short filenameLength __attribute__ ((__packed__));
  unsigned short extraDataLength __attribute__ ((__packed__));
}
PKZIPHEADER;

/*
 * Zip files are stored little endian
 * Support functions for short and int types
 */
static u32 FLIP32 (u32 b)
{
	unsigned int c;

	c = (b & 0xff000000) >> 24;
	c |= (b & 0xff0000) >> 8;
	c |= (b & 0xff00) << 8;
	c |= (b & 0xff) << 24;

	return c;
}

static u16 FLIP16 (u16 b)
{
	u16 c;

	c = (b & 0xff00) >> 8;
	c |= (b & 0xff) << 8;

	return c;
}

/****************************************************************************
 * IsZipFile
 *
 * Returns TRUE when 0x504b0304 is first four characters of buffer
 ***************************************************************************/
static int IsZipFile (char *buffer)
{
	unsigned int *check = (unsigned int *) buffer;

	if (check[0] == 0x504b0304)
		return 1;

	return 0;
}

/*****************************************************************************
* UnZipBuffer
******************************************************************************/

static size_t UnZipBuffer (unsigned char *outbuffer)
{
	PKZIPHEADER pkzip;
	size_t zipoffset = 0;
	size_t zipchunk = 0;
	char out[ZIPCHUNK];
	z_stream zs;
	int res;
	size_t bufferoffset = 0;
	size_t have = 0;
	char readbuffer[ZIPCHUNK];
	size_t sizeread = 0;

	// Read Zip Header
	fseek(file, 0, SEEK_SET);
	sizeread = fread (readbuffer, 1, ZIPCHUNK, file);

	if(sizeread <= 0)
		return 0;

	/*** Copy PKZip header to local, used as info ***/
	memcpy (&pkzip, readbuffer, sizeof (PKZIPHEADER));

	pkzip.uncompressedSize = FLIP32 (pkzip.uncompressedSize);

	ShowProgress ("Loading...", 0, pkzip.uncompressedSize);

	/*** Prepare the zip stream ***/
	memset (&zs, 0, sizeof (z_stream));
	zs.zalloc = Z_NULL;
	zs.zfree = Z_NULL;
	zs.opaque = Z_NULL;
	zs.avail_in = 0;
	zs.next_in = Z_NULL;
	res = inflateInit2 (&zs, -MAX_WBITS);

	if (res != Z_OK)
		goto done;

	/*** Set ZipChunk for first pass ***/
	zipoffset =
	(sizeof (PKZIPHEADER) + FLIP16 (pkzip.filenameLength) +
	FLIP16 (pkzip.extraDataLength));
	zipchunk = ZIPCHUNK - zipoffset;

	/*** Now do it! ***/
	do
	{
		zs.avail_in = zipchunk;
		zs.next_in = (Bytef *) & readbuffer[zipoffset];

		/*** Now inflate until input buffer is exhausted ***/
		do
		{
			zs.avail_out = ZIPCHUNK;
			zs.next_out = (Bytef *) & out;

			res = inflate (&zs, Z_NO_FLUSH);

			if (res == Z_MEM_ERROR)
			{
				goto done;
			}

			have = ZIPCHUNK - zs.avail_out;
			if (have)
			{
				/*** Copy to normal block buffer ***/
				memcpy (&outbuffer[bufferoffset], &out, have);
				bufferoffset += have;
			}
		}
		while (zs.avail_out == 0);

		// Readup the next 2k block
		zipoffset = 0;
		zipchunk = ZIPCHUNK;

		sizeread = fread (readbuffer, 1, ZIPCHUNK, file);
		if(sizeread <= 0)
			goto done; // read failure

		ShowProgress ("Loading...", bufferoffset, pkzip.uncompressedSize);
	}
	while (res != Z_STREAM_END);

done:
	inflateEnd (&zs);
	CancelAction();

	if (res == Z_STREAM_END)
		return pkzip.uncompressedSize;
	else
		return 0;
}

/****************************************************************************
* GetFirstZipFilename
*
* Returns the filename of the first file in the zipped archive
* The idea here is to do the least amount of work required
***************************************************************************/

char * GetFirstZipFilename ()
{
	char * firstFilename = NULL;
	char tempbuffer[ZIPCHUNK];
	char filepath[1024];

	if(!MakeFilePath(filepath, FILE_ROM))
		return NULL;

	// read start of ZIP
	if(LoadFile (tempbuffer, filepath, ZIPCHUNK, NOTSILENT) < 35)
		return NULL;

	tempbuffer[28] = 0; // truncate - filename length is 2 bytes long (bytes 26-27)
	int namelength = tempbuffer[26]; // filename length starts 26 bytes in

	if(namelength < 0 || namelength > 200) // filename is not a reasonable length
	{
		ErrorPrompt("Error - Invalid ZIP file!");
		return NULL;
	}
	
	firstFilename = &tempbuffer[30]; // first filename of a ZIP starts 31 bytes in
	firstFilename[namelength] = 0; // truncate at filename length
	return strdup(firstFilename);
}

/****************************************************************************
 * ResumeDeviceThread
 *
 * Signals the device thread to start, and resumes the thread.
 ***************************************************************************/
void ResumeDeviceThread()
{
	deviceHalt = false;
	LWP_ResumeThread(devicethread);
}

/****************************************************************************
 * HaltGui
 *
 * Signals the device thread to stop.
 ***************************************************************************/
void HaltDeviceThread()
{
	deviceHalt = true;

	// wait for thread to finish
	while(!LWP_ThreadIsSuspended(devicethread))
		usleep(THREAD_SLEEP);
}

/****************************************************************************
 * HaltParseThread
 *
 * Signals the parse thread to stop.
 ***************************************************************************/
void HaltParseThread()
{
	parseHalt = true;

	while(!LWP_ThreadIsSuspended(parsethread))
		usleep(THREAD_SLEEP);
}


/****************************************************************************
 * devicecallback
 *
 * This checks our devices for changes (SD/USB/DVD removed)
 ***************************************************************************/
static int devsleep;

static void * devicecallback (void *arg)
{
	while (1)
	{
		if(isMounted[DEVICE_SD])
		{
			if(!sd->isInserted()) // check if the device was removed
			{
				unmountRequired[DEVICE_SD] = true;
				isMounted[DEVICE_SD] = false;
			}
		}

		if(isMounted[DEVICE_USB])
		{
			if(!usb->isInserted()) // check if the device was removed
			{
				unmountRequired[DEVICE_USB] = true;
				isMounted[DEVICE_USB] = false;
			}
		}

		if(isMounted[DEVICE_DVD])
		{
			if(!dvd->isInserted()) // check if the device was removed
			{
				unmountRequired[DEVICE_DVD] = true;
				isMounted[DEVICE_DVD] = false;
			}
		}

		devsleep = 1000*1000; // 1 sec

		while(devsleep > 0)
		{
			if(deviceHalt)
				LWP_SuspendThread(devicethread);
			usleep(THREAD_SLEEP);
			devsleep -= THREAD_SLEEP;
		}
	}
	return NULL;
}

static void * parsecallback (void *arg)
{
	while(1)
	{
		while(ParseDirEntries())
			usleep(THREAD_SLEEP);
		LWP_SuspendThread(parsethread);
	}
	return NULL;
}

/****************************************************************************
 * InitDeviceThread
 *
 * libOGC provides a nice wrapper for LWP access.
 * This function sets up a new local queue and attaches the thread to it.
 ***************************************************************************/
void InitDeviceThread()
{
	LWP_CreateThread (&devicethread, devicecallback, NULL, NULL, 0, 40);
	LWP_CreateThread (&parsethread, parsecallback, NULL, NULL, 0, 80);
}

/****************************************************************************
 * UnmountAllFAT
 * Unmounts all FAT devices
 ***************************************************************************/
void UnmountAllFAT()
{
	fatUnmount("sd:");
	fatUnmount("usb:");
}

/****************************************************************************
 * MountFAT
 * Checks if the device needs to be (re)mounted
 * If so, unmounts the device
 * Attempts to mount the device specified
 * Sets libfat to use the device by default
 ***************************************************************************/

static bool MountFAT(int device, int silent)
{
	bool mounted = false;
	int retry = 1;
	char name[10], name2[10];
	const DISC_INTERFACE* disc = NULL;

	switch(device)
	{
		case DEVICE_SD:
			sprintf(name, "sd");
			sprintf(name2, "sd:");
			disc = sd;
			break;
		case DEVICE_USB:
			sprintf(name, "usb");
			sprintf(name2, "usb:");
			disc = usb;
			break;
		default:
			return false; // unknown device
	}

	if(unmountRequired[device])
	{
		unmountRequired[device] = false;
		fatUnmount(name2);
		disc->shutdown();
		isMounted[device] = false;
	}

	while(retry)
	{
		if(disc->startup() && fatMountSimple(name, disc))
			mounted = true;

		if(mounted || silent)
			break;

		if(device == DEVICE_SD)
			retry = ErrorPromptRetry("SD card not found!");
		else
			retry = ErrorPromptRetry("USB drive not found!");
	}

	isMounted[device] = mounted;
	return mounted;
}

void MountAllFAT()
{
	MountFAT(DEVICE_SD, SILENT);
	MountFAT(DEVICE_USB, SILENT);
}

/****************************************************************************
 * MountDVD()
 *
 * Tests if a ISO9660 DVD is inserted and available, and mounts it
 ***************************************************************************/
bool MountDVD(bool silent)
{
	bool mounted = false;
	int retry = 1;

	if(unmountRequired[DEVICE_DVD])
	{
		unmountRequired[DEVICE_DVD] = false;
		ISO9660_Unmount("dvd:");
	}

	while(retry)
	{
		ShowAction("Loading DVD...");

		if(!dvd->isInserted())
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("No disc inserted!");
		}
		else if(!ISO9660_Mount("dvd", dvd))
		{
			if(silent)
				break;
			
			retry = ErrorPromptRetry("Unrecognized DVD format.");
		}
		else
		{
			mounted = true;
			break;
		}
	}
	CancelAction();
	isMounted[DEVICE_DVD] = mounted;
	return mounted;
}

bool FindDevice(const char * filepath, int * device)
{
	if(!filepath || filepath[0] == 0)
		return false;

	if(strncmp(filepath, "sd:", 3) == 0)
	{
		*device = DEVICE_SD;
		return true;
	}
	else if(strncmp(filepath, "usb:", 4) == 0)
	{
		*device = DEVICE_USB;
		return true;
	}
	else if(strncmp(filepath, "dvd:", 4) == 0)
	{
		*device = DEVICE_DVD;
		return true;
	}
	return false;
}

char * StripDevice(char * path)
{
	if(path == NULL)
		return NULL;
	
	char * newpath = strchr(path,'/');
	
	if(newpath != NULL)
		newpath++;
	
	return newpath;
}

/****************************************************************************
 * ChangeInterface
 * Attempts to mount/configure the device specified
 ***************************************************************************/
bool ChangeInterface(int device, bool silent)
{
	if(isMounted[device])
		return true;

	bool mounted = false;

	switch(device)
	{
		case DEVICE_SD:
		case DEVICE_USB:
			mounted = MountFAT(device, silent);
			break;
		case DEVICE_DVD:
			mounted = MountDVD(silent);
			break;
	}

	return mounted;
}

bool ChangeInterface(char * filepath, bool silent)
{
	int device = -1;

	if(!FindDevice(filepath, &device))
		return false;

	return ChangeInterface(device, silent);
}

void CreateAppPath(char * origpath)
{
	if(!origpath || origpath[0] == 0)
		return;

	char * path = strdup(origpath); // make a copy so we don't mess up original

	if(!path)
		return;
	
	char * loc = strrchr(path,'/');
	if (loc != NULL)
		*loc = 0; // strip file name

	int pos = 0;

	// replace fat:/ with sd:/
	if(strncmp(path, "fat:/", 5) == 0)
	{
		pos++;
		path[1] = 's';
		path[2] = 'd';
	}
	if(ChangeInterface(&path[pos], SILENT))
		snprintf(appPath, MAXPATHLEN-1, "%s", &path[pos]);

	free(path);
}

static char *GetExt(char *file)
{
	if(!file)
		return NULL;

	char *ext = strrchr(file,'.');
	if(ext != NULL)
	{
		ext++;
		int extlen = strlen(ext);
		if(extlen > 5)
			return NULL;
	}
	return ext;
}

bool GetFileSize(int i)
{
	if(browserList[i].length > 0)
		return true;

	struct stat filestat;
	char path[MAXPATHLEN+1];
	snprintf(path, MAXPATHLEN, "%s%s", browser.dir, browserList[i].filename);

	if(stat(path, &filestat) < 0)
		return false;

	browserList[i].length = filestat.st_size;
	return true;
}

static bool ParseDirEntries()
{
	if(!dir)
		return false;

	char *ext;
	struct dirent *entry = NULL;
	int isdir;

	int i = 0;

	while(i < 20 && !parseHalt)
	{
		entry = readdir(dir);

		if(entry == NULL)
			break;

		if(entry->d_name[0] == '.' && entry->d_name[1] != '.')
			continue;

		if(strcmp(entry->d_name, "..") == 0)
		{
			isdir = 1;
		}
		else
		{
			if(entry->d_type==DT_DIR)
				isdir = 1;
			else
				isdir = 0;
			
			// don't show the file if it's not a valid ROM
			if(parseFilter && !isdir)
			{
				ext = GetExt(entry->d_name);
				
				if(ext == NULL)
					continue;

				if(	stricmp(ext, "smc") != 0 && stricmp(ext, "fig") != 0 &&
					stricmp(ext, "sfc") != 0 && stricmp(ext, "swc") != 0 &&
					stricmp(ext, "zip") != 0)
					continue;
			}
		}

		if(!AddBrowserEntry())
		{
			parseHalt = true;
			break;
		}

		snprintf(browserList[browser.numEntries+i].filename, MAXJOLIET, "%s", entry->d_name);
		browserList[browser.numEntries+i].isdir = isdir; // flag this as a dir

		if(isdir)
		{
			if(strcmp(entry->d_name, "..") == 0)
				sprintf(browserList[browser.numEntries+i].displayname, "Up One Level");
			else
				snprintf(browserList[browser.numEntries+i].displayname, MAXJOLIET, "%s", browserList[browser.numEntries+i].filename);
			browserList[browser.numEntries+i].icon = ICON_FOLDER;
		}
		else
		{
			StripExt(browserList[browser.numEntries+i].displayname, browserList[browser.numEntries+i].filename); // hide file extension
		}
		i++;
	}

	if(!parseHalt)
	{
		// Sort the file list
		if(i >= 0)
			qsort(browserList, browser.numEntries+i, sizeof(BROWSERENTRY), FileSortCallback);
	
		browser.numEntries += i;
	}

	if(entry == NULL || parseHalt)
	{
		closedir(dir); // close directory
		dir = NULL;

		// try to find and select the last loaded file
		if(selectLoadedFile == 1 && !parseHalt && loadedFile[0] != 0 && browser.dir[0] != 0)
		{
			int indexFound = -1;
			
			for(int j=1; j < browser.numEntries; j++)
			{
				if(strcmp(browserList[j].filename, loadedFile) == 0)
				{
					indexFound = j;
					break;
				}
			}

			// move to this file
			if(indexFound > 0)
			{
				if(indexFound >= FILE_PAGESIZE)
				{			
					int newIndex = (floor(indexFound/(float)FILE_PAGESIZE)) * FILE_PAGESIZE;

					if(newIndex + FILE_PAGESIZE > browser.numEntries)
						newIndex = browser.numEntries - FILE_PAGESIZE;

					if(newIndex < 0)
						newIndex = 0;

					browser.pageIndex = newIndex;
				}
				browser.selIndex = indexFound;
			}
			selectLoadedFile = 2; // selecting done
		}
		return false; // no more entries
	}
	return true; // more entries
}

/***************************************************************************
 * Browse subdirectories
 **************************************************************************/
int ParseDirectory(bool waitParse, bool filter)
{
	int retry = 1;
	bool mounted = false;
	parseFilter = filter;
	
	ResetBrowser(); // reset browser
	
	// add trailing slash
	if(browser.dir[strlen(browser.dir)-1] != '/')
		strcat(browser.dir, "/");

	// open the directory
	while(dir == NULL && retry == 1)
	{
		mounted = ChangeInterface(browser.dir, NOTSILENT);

		if(mounted)
			dir = opendir(browser.dir);
		else
			return -1;

		if(dir == NULL)
		{
			retry = ErrorPromptRetry("Error opening directory!");
		}
	}

	// if we can't open the dir, try higher levels
	if (dir == NULL)
	{
		char * devEnd = strrchr(browser.dir, '/');

		while(!IsDeviceRoot(browser.dir))
		{
			devEnd[0] = 0; // strip slash
			devEnd = strrchr(browser.dir, '/');

			if(devEnd == NULL)
				break;

			devEnd[1] = 0; // strip remaining file listing
			dir = opendir(browser.dir);
			if (dir)
				break;
		}
	}
	
	if(dir == NULL)
		return -1;

	if(IsDeviceRoot(browser.dir))
	{
		AddBrowserEntry();
		sprintf(browserList[0].filename, "..");
		sprintf(browserList[0].displayname, "Up One Level");
		browserList[0].length = 0;
		browserList[0].isdir = 1; // flag this as a dir
		browserList[0].icon = ICON_FOLDER;
		browser.numEntries++;
	}

	parseHalt = false;
	ParseDirEntries(); // index first 20 entries

	LWP_ResumeThread(parsethread); // index remaining entries

	if(waitParse) // wait for complete parsing
	{
		ShowAction("Loading...");

		while(!LWP_ThreadIsSuspended(parsethread))
			usleep(THREAD_SLEEP);

		CancelAction();
	}

	return browser.numEntries;
}

/****************************************************************************
 * AllocSaveBuffer ()
 * Clear and allocate the savebuffer
 ***************************************************************************/
void AllocSaveBuffer ()
{
	if(bufferLock == LWP_MUTEX_NULL)
		LWP_MutexInit(&bufferLock, false);

	if(bufferLock != LWP_MUTEX_NULL)
		LWP_MutexLock(bufferLock);
	memset (savebuffer, 0, SAVEBUFFERSIZE);
}

/****************************************************************************
 * FreeSaveBuffer ()
 * Free the savebuffer memory
 ***************************************************************************/
void FreeSaveBuffer ()
{
	if(bufferLock != LWP_MUTEX_NULL)
		LWP_MutexUnlock(bufferLock);
}

/****************************************************************************
 * LoadFile
 ***************************************************************************/
size_t LoadFile(char * rbuffer, const char *filepath, size_t length, bool silent)
{
	char zipbuffer[2048];
	size_t size = 0, offset = 0, readsize = 0;
	int retry = 1;
	int device;

	if(!FindDevice(filepath, &device))
		return 0;

	// stop checking if devices were removed/inserted
	// since we're loading a file
	HaltDeviceThread();

	// halt parsing
	HaltParseThread();

	// open the file
	while(retry)
	{
		if(!ChangeInterface(device, silent))
			break;

		file = fopen (filepath, "rb");

		if(!file)
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("Error opening file!");
			continue;
		}

		if(length > 0 && length <= 2048) // do a partial read (eg: to check file header)
		{
			size = fread (rbuffer, 1, length, file);
		}
		else // load whole file
		{
			readsize = fread (zipbuffer, 1, 32, file);

			if(!readsize)
			{
				unmountRequired[device] = true;
				retry = ErrorPromptRetry("Error reading file!");
				fclose (file);
				continue;
			}

			if (IsZipFile (zipbuffer))
			{
				size = UnZipBuffer ((unsigned char *)rbuffer); // unzip
			}
			else
			{
				fseeko(file,0,SEEK_END);
				size = ftello(file);
				fseeko(file,0,SEEK_SET);

				while(!feof(file))
				{
					ShowProgress ("Loading...", offset, size);
					readsize = fread (rbuffer + offset, 1, 4096, file); // read in next chunk

					if(readsize <= 0)
						break; // reading finished (or failed)

					offset += readsize;
				}
				size = offset;
				CancelAction();
			}
		}
		retry = 0;
		fclose (file);
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();
	CancelAction();
	return size;
}

size_t LoadFile(char * filepath, bool silent)
{
	return LoadFile((char *)savebuffer, filepath, 0, silent);
}

/****************************************************************************
 * SaveFile
 * Write buffer to file
 ***************************************************************************/
size_t SaveFile (char * buffer, char *filepath, size_t datasize, bool silent)
{
	size_t written = 0;
	size_t writesize, nextwrite;
	int retry = 1;
	int device;
		
	if(!FindDevice(filepath, &device))
		return 0;

	if(datasize == 0)
		return 0;

	// stop checking if devices were removed/inserted
	// since we're saving a file
	HaltDeviceThread();

	// halt parsing
	HaltParseThread();

	ShowAction("Saving...");

	while(!written && retry == 1)
	{
		if(!ChangeInterface(device, silent))
			break;

		file = fopen (filepath, "wb");

		if(!file)
		{
			if(silent)
				break;

			retry = ErrorPromptRetry("Error creating file!");
			continue;
		}

		while(written < datasize)
		{
			if(datasize - written > 4096) nextwrite=4096;
			else nextwrite = datasize-written;
			writesize = fwrite (buffer+written, 1, nextwrite, file);
			if(writesize != nextwrite) break; // write failure
			written += writesize;
		}
		fclose (file);

		if(written != datasize) written = 0;

		if(!written)
		{
			unmountRequired[device] = true;
			if(silent) break;
			retry = ErrorPromptRetry("Error saving file!");
		}
	}

	// go back to checking if devices were inserted/removed
	ResumeDeviceThread();

	CancelAction();
	return written;
}

size_t SaveFile(char * filepath, size_t datasize, bool silent)
{
	return SaveFile((char *)savebuffer, filepath, datasize, silent);
}
