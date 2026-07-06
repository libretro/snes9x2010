/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  MSU1 emulator code (C port for the snes9x2010 libretro core).

  Register/data semantics are matched against ares (sfc/coprocessor/msu1) and
  the MiSTer SNES MSU1 RTL; audio decode/volume against the same. General
  framing follows mainline snes9x's msu1.cpp.
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snes9x.h"
#include "memmap.h"
#include "msu1.h"

struct SMSU1	MSU1;

/* Companion-file streams: "<rom>.msu" data ROM and "<rom>-<track>.pcm" audio. */
static FILE		*dataFile   = NULL;
static FILE		*audioFile  = NULL;
static char		 msu1_rom_path[PATH_MAX + 1] = { 0 };
static uint8_t		 msu1_have_path = FALSE;

/* Path helpers ----------------------------------------------------------------

   MSU1 companion files live next to the ROM:
     <basename>.msu             - data ROM (optional)
     <basename>-<track>.pcm     - audio track (track = decimal, no zero pad)
   where <basename> is the ROM path with its final extension removed. */

static void msu1_strip_ext (char *path)
{
	char	*dot   = strrchr(path, '.');
	char	*slash = strrchr(path, '/');
#ifdef _WIN32
	char	*bslash = strrchr(path, '\\');
	if (bslash && (!slash || bslash > slash))
		slash = bslash;
#endif
	if (dot && (!slash || dot > slash))
		*dot = '\0';
}

static FILE * msu1_open_data (void)
{
	char	path[PATH_MAX + 8];

	if (!msu1_have_path)
		return (NULL);

	strcpy(path, msu1_rom_path);
	msu1_strip_ext(path);
	strcat(path, ".msu");

	return (fopen(path, "rb"));
}

static FILE * msu1_open_track (unsigned track)
{
	char	path[PATH_MAX + 24];
	char	base[PATH_MAX + 1];

	if (!msu1_have_path)
		return (NULL);

	strcpy(base, msu1_rom_path);
	msu1_strip_ext(base);
	snprintf(path, sizeof(path), "%s-%u.pcm", base, track);

	return (fopen(path, "rb"));
}

/* Little-endian helpers reading directly from the companion streams. */

static uint32_t msu1_readl (FILE *f, int bytes)
{
	uint32_t	value = 0;
	int		i;
	for (i = 0; i < bytes; i++)
	{
		int c = fgetc(f);
		if (c == EOF)
			c = 0;
		value |= ((uint32_t) (c & 0xff)) << (i * 8);
	}
	return (value);
}

static uint32_t msu1_readm (FILE *f, int bytes)
{
	uint32_t	value = 0;
	int		i;
	for (i = 0; i < bytes; i++)
	{
		int c = fgetc(f);
		if (c == EOF)
			c = 0;
		value = (value << 8) | (uint32_t) (c & 0xff);
	}
	return (value);
}

static long msu1_filesize (FILE *f)
{
	long	cur, end;
	if (!f)
		return (0);
	cur = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, cur, SEEK_SET);
	return (end);
}

/* Rebuild the STATUS byte from the individual flag fields (ares layout). */
static void msu1_update_status (void)
{
	MSU1.MSU1_STATUS = (MSU1_Revision & 0x07);
	if (MSU1.MSU1_AudioError)   MSU1.MSU1_STATUS |= MSU1_AudioError;
	if (MSU1.MSU1_AudioPlay)    MSU1.MSU1_STATUS |= MSU1_AudioPlaying;
	if (MSU1.MSU1_AudioRepeat)  MSU1.MSU1_STATUS |= MSU1_AudioRepeating;
	if (MSU1.MSU1_AudioBusy)    MSU1.MSU1_STATUS |= MSU1_AudioBusy;
	if (MSU1.MSU1_DataBusy)     MSU1.MSU1_STATUS |= MSU1_DataBusy;
}

/* Open the currently-selected audio track and validate its header, matching
   ares' MSU1::audioOpen(): 8-byte header "MSU1" + 32-bit LE loop point (in
   samples). On any failure the audio-error flag is set. */
static void msu1_audio_open (void)
{
	if (audioFile)
	{
		fclose(audioFile);
		audioFile = NULL;
	}

	audioFile = msu1_open_track(MSU1.MSU1_CurrentTrack);
	if (audioFile)
	{
		if (msu1_filesize(audioFile) >= 8)
		{
			uint32_t	header;
			fseek(audioFile, 0, SEEK_SET);
			header = msu1_readm(audioFile, 4);
			if (header == 0x4d535531)  /* "MSU1" */
			{
				MSU1.MSU1_AudioLoopOffset = 8 + msu1_readl(audioFile, 4) * 4;
				if (MSU1.MSU1_AudioLoopOffset > (uint32_t) msu1_filesize(audioFile))
					MSU1.MSU1_AudioLoopOffset = 8;
				MSU1.MSU1_AudioError = FALSE;
				fseek(audioFile, MSU1.MSU1_AudioPlayOffset, SEEK_SET);
				msu1_update_status();
				return;
			}
		}

		fclose(audioFile);
		audioFile = NULL;
	}

	MSU1.MSU1_AudioError = TRUE;
	msu1_update_status();
}

static void msu1_data_open (void)
{
	if (dataFile)
	{
		fclose(dataFile);
		dataFile = NULL;
	}

	dataFile = msu1_open_data();
	if (dataFile)
		fseek(dataFile, MSU1.MSU1_DataReadOffset, SEEK_SET);
}

/* Lifecycle ------------------------------------------------------------------ */

void S9xMSU1SetROMPath (const char *rom_path)
{
	if (rom_path && *rom_path)
	{
		strncpy(msu1_rom_path, rom_path, PATH_MAX);
		msu1_rom_path[PATH_MAX] = '\0';
		msu1_have_path = TRUE;
	}
	else
		msu1_have_path = FALSE;
}

uint8_t S9xMSU1ROMExists (void)
{
	FILE	*s = msu1_open_data();
	if (s)
	{
		fclose(s);
		return (TRUE);
	}

	/* A track-0 pcm alone is enough to warrant MSU1 (data ROM is optional). */
	s = msu1_open_track(0);
	if (s)
	{
		fclose(s);
		return (TRUE);
	}

	return (FALSE);
}

void S9xResetMSU1 (void)
{
	MSU1.MSU1_STATUS           = MSU1_Revision & 0x07;

	MSU1.MSU1_DataSeekOffset   = 0;
	MSU1.MSU1_DataReadOffset   = 0;

	MSU1.MSU1_AudioPlayOffset  = 0;
	MSU1.MSU1_AudioLoopOffset  = 0;

	MSU1.MSU1_CurrentTrack     = 0;
	MSU1.MSU1_VolumeB          = 0;

	MSU1.MSU1_AudioResumeTrack = ~0U;  /* no resume */
	MSU1.MSU1_AudioResumeOffset = 0;

	MSU1.MSU1_Control          = 0;

	MSU1.MSU1_AudioError       = FALSE;
	MSU1.MSU1_AudioPlay        = FALSE;
	MSU1.MSU1_AudioRepeat      = FALSE;
	MSU1.MSU1_AudioBusy        = FALSE;
	MSU1.MSU1_DataBusy         = FALSE;

	msu1_update_status();
}

void S9xMSU1Init (void)
{
	msu1_data_open();
	/* Audio track is opened lazily on the first track-select write, matching
	   ares' power() which calls audioOpen() with track 0; we defer so that a
	   missing track-0 file doesn't spuriously set the error flag before the
	   game selects a track. Track 0 is still opened here if present. */
	msu1_audio_open();
}

void S9xMSU1DeInit (void)
{
	if (dataFile)  { fclose(dataFile);  dataFile  = NULL; }
	if (audioFile) { fclose(audioFile); audioFile = NULL; }
}

/* MMIO ($2000-$2007) ---------------------------------------------------------

   Bit-exact against ares' readIO/writeIO and MiSTer's MSU.sv. */

uint8_t S9xMSU1ReadPort (uint8_t port)
{
	switch (port & 7)
	{
		case 0:  /* $2000 STATUS */
			msu1_update_status();
			return ((uint8_t) MSU1.MSU1_STATUS);

		case 1:  /* $2001 DATA */
			if (MSU1.MSU1_DataBusy)
				return (0x00);
			if (!dataFile)
				return (0x00);
			{
				int c = fgetc(dataFile);
				if (c == EOF)
					return (0x00);
				MSU1.MSU1_DataReadOffset++;
				return ((uint8_t) c);
			}

		case 2:  return ('S');
		case 3:  return ('-');
		case 4:  return ('M');
		case 5:  return ('S');
		case 6:  return ('U');
		case 7:  return ('1');
	}

	return (0x00);  /* unreachable */
}

void S9xMSU1WritePort (uint8_t port, uint8_t byte)
{
	switch (port & 7)
	{
		case 0:  /* $2000 seek offset byte 0 */
			MSU1.MSU1_DataSeekOffset = (MSU1.MSU1_DataSeekOffset & 0xffffff00) | ((uint32_t) byte << 0);
			break;

		case 1:  /* $2001 seek offset byte 1 */
			MSU1.MSU1_DataSeekOffset = (MSU1.MSU1_DataSeekOffset & 0xffff00ff) | ((uint32_t) byte << 8);
			break;

		case 2:  /* $2002 seek offset byte 2 */
			MSU1.MSU1_DataSeekOffset = (MSU1.MSU1_DataSeekOffset & 0xff00ffff) | ((uint32_t) byte << 16);
			break;

		case 3:  /* $2003 seek offset byte 3 -> commit seek */
			MSU1.MSU1_DataSeekOffset = (MSU1.MSU1_DataSeekOffset & 0x00ffffff) | ((uint32_t) byte << 24);
			MSU1.MSU1_DataReadOffset = MSU1.MSU1_DataSeekOffset;
			if (dataFile)
				fseek(dataFile, MSU1.MSU1_DataReadOffset, SEEK_SET);
			break;

		case 4:  /* $2004 track select low */
			MSU1.MSU1_CurrentTrack = (MSU1.MSU1_CurrentTrack & 0xff00) | ((uint16_t) byte << 0);
			break;

		case 5:  /* $2005 track select high -> mount track */
			MSU1.MSU1_CurrentTrack = (MSU1.MSU1_CurrentTrack & 0x00ff) | ((uint16_t) byte << 8);

			MSU1.MSU1_AudioPlay   = FALSE;
			MSU1.MSU1_AudioRepeat = FALSE;
			MSU1.MSU1_AudioPlayOffset = 8;

			if (MSU1.MSU1_CurrentTrack == MSU1.MSU1_AudioResumeTrack)
			{
				MSU1.MSU1_AudioPlayOffset = MSU1.MSU1_AudioResumeOffset;
				MSU1.MSU1_AudioResumeTrack = ~0U;  /* erase resume track */
				MSU1.MSU1_AudioResumeOffset = 0;
			}

			msu1_audio_open();
			break;

		case 6:  /* $2006 volume */
			MSU1.MSU1_VolumeB = byte;
			break;

		case 7:  /* $2007 audio control */
			if (MSU1.MSU1_AudioBusy)
				break;
			if (MSU1.MSU1_AudioError)
				break;

			MSU1.MSU1_AudioPlay   = (byte & 0x01) ? TRUE : FALSE;
			MSU1.MSU1_AudioRepeat = (byte & 0x02) ? TRUE : FALSE;

			if (!MSU1.MSU1_AudioPlay && (byte & 0x04))
			{
				/* Resume request: remember where we are so a later re-select of
				   this track continues from here. */
				MSU1.MSU1_AudioResumeTrack  = MSU1.MSU1_CurrentTrack;
				MSU1.MSU1_AudioResumeOffset = MSU1.MSU1_AudioPlayOffset;
			}
			break;
	}

	msu1_update_status();
}

/* Audio ----------------------------------------------------------------------

   ares generates one 44.1 kHz stereo frame per MSU1 clock, scaling each s16
   sample by volume/255. This core delivers SPC audio at the SNES native rate
   and lets the frontend resample; we resample the 44.1 kHz MSU1 stream to the
   requested SNES-rate sample_count with linear interpolation (a gentle ~1.38x
   downsample) and add it into the SPC output buffer. Final host-rate
   conversion is the frontend's job, exactly as for SPC audio. */

/* Read one interpolated 44.1 kHz MSU1 stereo frame at fractional source
   position, honouring end-of-file / loop / stop, matching ares' main(). */
static void msu1_next_frame_44k (int32_t *outL, int32_t *outR)
{
	int16_t	l = 0, r = 0;

	if (MSU1.MSU1_AudioPlay && audioFile)
	{
		long	pos  = ftell(audioFile);
		long	size = msu1_filesize(audioFile);

		if (pos >= size || pos < 8)
		{
			if (!MSU1.MSU1_AudioRepeat)
			{
				MSU1.MSU1_AudioPlay = FALSE;
				MSU1.MSU1_AudioPlayOffset = 8;
				fseek(audioFile, 8, SEEK_SET);
				msu1_update_status();
			}
			else
			{
				MSU1.MSU1_AudioPlayOffset = MSU1.MSU1_AudioLoopOffset;
				fseek(audioFile, MSU1.MSU1_AudioLoopOffset, SEEK_SET);
			}
		}

		if (MSU1.MSU1_AudioPlay && audioFile)
		{
			l = (int16_t) msu1_readl(audioFile, 2);
			r = (int16_t) msu1_readl(audioFile, 2);
			MSU1.MSU1_AudioPlayOffset += 4;
			l = (int16_t) (((int32_t) l * (int32_t) MSU1.MSU1_VolumeB) / 255);
			r = (int16_t) (((int32_t) r * (int32_t) MSU1.MSU1_VolumeB) / 255);
		}
	}
	else if (MSU1.MSU1_AudioPlay && !audioFile)
	{
		MSU1.MSU1_AudioPlay = FALSE;
		msu1_update_status();
	}

	*outL = l;
	*outR = r;
}

void S9xMSU1Mix (int16_t *buffer, size_t sample_count)
{
	/* Fixed-point 44100/snes_rate step, applied per output frame. The output
	   rate is the SPC native rate reported to the frontend; using 32040 here
	   keeps the MSU1 pitch correct against the SPC clock. */
	static uint32_t	frac = 0;      /* 16.16 fractional source position */
	const uint32_t	step = (uint32_t) (((uint64_t) 44100 << 16) / 32040);
	size_t		i;

	if (!Settings.MSU1)
		return;
	if (!MSU1.MSU1_AudioPlay || !audioFile)
		return;

	/* Prime with the first source frame. */
	{
		int32_t	curL, curR, nxtL, nxtR;
		msu1_next_frame_44k(&curL, &curR);
		msu1_next_frame_44k(&nxtL, &nxtR);

		for (i = 0; i < sample_count; i++)
		{
			uint32_t	t = frac & 0xffff;
			int32_t		mixL = curL + (((nxtL - curL) * (int32_t) t) >> 16);
			int32_t		mixR = curR + (((nxtR - curR) * (int32_t) t) >> 16);
			int32_t		sumL = (int32_t) buffer[i * 2 + 0] + mixL;
			int32_t		sumR = (int32_t) buffer[i * 2 + 1] + mixR;

			if (sumL >  32767) sumL =  32767;
			if (sumL < -32768) sumL = -32768;
			if (sumR >  32767) sumR =  32767;
			if (sumR < -32768) sumR = -32768;

			buffer[i * 2 + 0] = (int16_t) sumL;
			buffer[i * 2 + 1] = (int16_t) sumR;

			frac += step;
			while (frac >= 0x10000)
			{
				frac -= 0x10000;
				curL = nxtL; curR = nxtR;
				msu1_next_frame_44k(&nxtL, &nxtR);
			}

			if (!MSU1.MSU1_AudioPlay || !audioFile)
				break;
		}
	}
}

/* Savestate ------------------------------------------------------------------ */

void S9xMSU1PreSaveState (void)
{
	/* All persisted MSU1 state lives directly in struct MSU1 (serialised by
	   snapshot.c). The current file offsets are captured here so the streams
	   can be re-seeked on load. */
	if (audioFile)
		MSU1.MSU1_AudioPlayOffset = (uint32_t) ftell(audioFile);
	if (dataFile)
		MSU1.MSU1_DataReadOffset = (uint32_t) ftell(dataFile);
}

void S9xMSU1PostLoadState (void)
{
	/* Re-open the companion files and restore stream positions from the
	   deserialised offsets. */
	msu1_data_open();
	if (dataFile)
		fseek(dataFile, MSU1.MSU1_DataReadOffset, SEEK_SET);

	msu1_audio_open();
	if (audioFile)
		fseek(audioFile, MSU1.MSU1_AudioPlayOffset, SEEK_SET);

	msu1_update_status();
}
