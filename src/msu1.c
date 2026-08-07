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
#include <streams/file_stream.h>
#include "msu1_pack.h"

struct SMSU1	MSU1;

/* Companion sources: "<rom>.msu" data ROM and "<rom>-<track>.pcm" audio, each
   either a loose file next to the ROM or an entry inside an "<rom>.msu1" pack.
   Both are read through msu1_src_*, which is offset-addressed, so nothing
   below this point cares which of the two it is. */
struct msu1_src
{
	RFILE			*loose;
	struct msu1_pack_file	 pack;
	uint8_t			 packed;
	uint32_t		 size;
};

static struct msu1_src	dataSrc;
static struct msu1_src	audioSrc;

static uint8_t msu1_src_open (struct msu1_src *src, const char *loose_path,
                              const char *pack_path, const char *suffix);
static void    msu1_src_close (struct msu1_src *src);
static uint32_t msu1_src_read (struct msu1_src *src, uint32_t offset, uint8_t *out, uint32_t len);

/* Kept as the "is it mounted" predicate the rest of the file already reads. */
#define dataFile   (dataSrc.loose  || dataSrc.packed)
#define audioFile  (audioSrc.loose || audioSrc.packed)
static char		 msu1_rom_path[PATH_MAX + 1] = { 0 };
static uint8_t		 msu1_have_path = FALSE;

/* Audio fast path: the per-sample generator must not touch the FILE stream
   (ftell/fseek/fgetc per sample throttles emulation badly). Instead we cache
   the track size once at open, track the play cursor as an integer byte
   offset, and stream PCM through a RAM buffer, refilling in bulk. */
static long		 audio_size = 0;                 /* cached track file size */
static uint32_t		 audio_cursor = 0;               /* absolute byte offset of next sample */
#define MSU1_AUDIO_BUFSZ	8192                     /* PCM read-ahead buffer (bytes) */
static uint8_t		 audio_buf[MSU1_AUDIO_BUFSZ];
static uint32_t		 audio_buf_base = 0;             /* file offset of audio_buf[0] */
static uint32_t		 audio_buf_len  = 0;             /* valid bytes in audio_buf */
/* Track number audioFile currently holds, or ~0U when nothing is open. Lets a
   savestate load skip the close/open/header-parse when the state names the
   track that is already mounted - which is every load under Preemptive
   Frames, up to once per displayed frame. */
static uint32_t		 audio_open_track = ~0U;

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

/* "<basename>.msu1" - the optional pack holding the companion files. */
static void msu1_pack_path (char *out, size_t out_size)
{
	char	base[PATH_MAX + 1];

	out[0] = '\0';
	if (!msu1_have_path)
		return;

	strcpy(base, msu1_rom_path);
	msu1_strip_ext(base);
	snprintf(out, out_size, "%s.msu1", base);
}

/* Open a companion source: the loose file if it is there, otherwise the
   matching entry in the pack. Loose wins so a user can override one track
   without rebuilding the pack, which is also mainline's order. */
static uint8_t msu1_src_open (struct msu1_src *src, const char *loose_path,
                              const char *pack_path, const char *suffix)
{
	msu1_src_close(src);

	if (loose_path && *loose_path)
	{
		src->loose = filestream_open(loose_path, RETRO_VFS_FILE_ACCESS_READ,
		                             RETRO_VFS_FILE_ACCESS_HINT_NONE);
		if (src->loose)
		{
			long cur = filestream_tell(src->loose);
			filestream_seek(src->loose, 0, RETRO_VFS_SEEK_POSITION_END);
			src->size = (uint32_t) filestream_tell(src->loose);
			filestream_seek(src->loose, cur, RETRO_VFS_SEEK_POSITION_START);
			return (TRUE);
		}
	}

	if (pack_path && *pack_path && msu1_pack_open(&src->pack, pack_path, suffix))
	{
		src->packed = TRUE;
		src->size   = msu1_pack_size(&src->pack);
		return (TRUE);
	}

	return (FALSE);
}

static void msu1_src_close (struct msu1_src *src)
{
	if (src->loose)
	{
		filestream_close(src->loose);
		src->loose = NULL;
	}
	if (src->packed)
	{
		msu1_pack_close(&src->pack);
		src->packed = FALSE;
	}
	src->size = 0;
}

static uint32_t msu1_src_read (struct msu1_src *src, uint32_t offset, uint8_t *out, uint32_t len)
{
	if (src->loose)
	{
		filestream_seek(src->loose, (long) offset, RETRO_VFS_SEEK_POSITION_START);
		return ((uint32_t) filestream_read(src->loose, out, (int64_t) len));
	}
	if (src->packed)
		return (msu1_pack_read(&src->pack, offset, out, len));
	return (0);
}

static uint8_t msu1_open_data (void)
{
	char	path[PATH_MAX + 8];
	char	pack[PATH_MAX + 8];

	if (!msu1_have_path)
		return (FALSE);

	strcpy(path, msu1_rom_path);
	msu1_strip_ext(path);
	strcat(path, ".msu");

	msu1_pack_path(pack, sizeof(pack));

	if (msu1_src_open(&dataSrc, path, pack, ".msu"))
		return (TRUE);

	/* Homebrew built against the MSU1 SDK ships the data ROM under this
	   fixed name instead of "<basename>.msu". */
	{
		char	dir[PATH_MAX + 16];
		char	*slash;

		strcpy(dir, msu1_rom_path);
		slash = strrchr(dir, '/');
#ifdef _WIN32
		{
			char *bslash = strrchr(dir, '\\');
			if (bslash && (!slash || bslash > slash))
				slash = bslash;
		}
#endif
		if (slash)
			slash[1] = '\0';
		else
			dir[0] = '\0';
		strcat(dir, "msu1.rom");

		if (msu1_src_open(&dataSrc, dir, pack, "msu1.rom"))
			return (TRUE);
	}

	return (FALSE);
}

static uint8_t msu1_open_track (unsigned track)
{
	char	path[PATH_MAX + 24];
	char	base[PATH_MAX + 1];
	char	pack[PATH_MAX + 8];
	char	suffix[24];

	if (!msu1_have_path)
		return (FALSE);

	strcpy(base, msu1_rom_path);
	msu1_strip_ext(base);
	snprintf(path, sizeof(path), "%s-%u.pcm", base, track);
	snprintf(suffix, sizeof(suffix), "-%u.pcm", track);

	msu1_pack_path(pack, sizeof(pack));

	return (msu1_src_open(&audioSrc, path, pack, suffix));
}

/* Discard the interpolator's in-flight frame pair and phase. Called on every
   track (re)mount and on reset; NOT on savestate load, where the deserialised
   state must survive so replay stays byte-exact. */
static void msu1_rsmp_reset (void)
{
	MSU1.MSU1_RsmpFrac   = 0;
	MSU1.MSU1_RsmpCurL   = 0;
	MSU1.MSU1_RsmpCurR   = 0;
	MSU1.MSU1_RsmpNxtL   = 0;
	MSU1.MSU1_RsmpNxtR   = 0;
	MSU1.MSU1_RsmpPrimed = FALSE;
}

/* Discard the enhanced-audio (SPC -> 44.1 kHz) upsampler's in-flight frame
   pair and phase. The libretro layer drives that resampler; the state lives in
   struct MSU1 so snapshot.c carries it, and the reset lives here so reset and
   savestate-validation paths have one place to clear it. */
static void msu1_enh_reset (void)
{
	MSU1.MSU1_EnhFrac = 0;
	MSU1.MSU1_EnhCurL = 0;
	MSU1.MSU1_EnhCurR = 0;
	MSU1.MSU1_EnhNxtL = 0;
	MSU1.MSU1_EnhNxtR = 0;
	MSU1.MSU1_EnhFill = 0;
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
	msu1_src_close(&audioSrc);

	if (msu1_open_track(MSU1.MSU1_CurrentTrack))
	{
		long fsz = (long) audioSrc.size;
		if (fsz >= 8)
		{
			uint8_t		hdr[8];
			uint32_t	header;
			if (msu1_src_read(&audioSrc, 0, hdr, 8) != 8)
				hdr[0] = 0;
			header = ((uint32_t) hdr[0] << 24) | ((uint32_t) hdr[1] << 16) |
			         ((uint32_t) hdr[2] << 8)  | (uint32_t) hdr[3];
			if (header == 0x4d535531)  /* "MSU1" */
			{
				MSU1.MSU1_AudioLoopOffset = 8 + (((uint32_t) hdr[4]) |
					((uint32_t) hdr[5] << 8) | ((uint32_t) hdr[6] << 16) |
					((uint32_t) hdr[7] << 24)) * 4;
				if (MSU1.MSU1_AudioLoopOffset > (uint32_t) fsz)
					MSU1.MSU1_AudioLoopOffset = 8;
				MSU1.MSU1_AudioError = FALSE;
				/* Cache size and reset the read-ahead buffer; the play cursor
				   follows AudioPlayOffset (restored on resume / savestate). */
				audio_size     = fsz;
				audio_cursor   = MSU1.MSU1_AudioPlayOffset;
				audio_buf_base = 0;
				audio_buf_len  = 0;
				audio_open_track = MSU1.MSU1_CurrentTrack;
				msu1_update_status();
				return;
			}
		}

		msu1_src_close(&audioSrc);
	}

	audio_size   = 0;
	audio_cursor = 0;
	audio_buf_len = 0;
	audio_open_track = ~0U;
	MSU1.MSU1_AudioError = TRUE;
	msu1_update_status();
}

static void msu1_data_open (void)
{
	msu1_src_close(&dataSrc);
	msu1_open_data();
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
	if (msu1_open_data())
	{
		msu1_src_close(&dataSrc);
		return (TRUE);
	}

	/* A track-0 pcm alone is enough to warrant MSU1 (data ROM is optional). */
	if (msu1_open_track(0))
	{
		msu1_src_close(&audioSrc);
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

	msu1_rsmp_reset();
	msu1_enh_reset();
	msu1_update_status();
}

void S9xMSU1Init (void)
{
	msu1_data_open();
	/* Audio track is opened lazily on the first track-select write, matching
	   ares' power() which calls audioOpen() with track 0; we defer so that a
	   missing track-0 file doesn't spuriously set the error flag before the
	   game selects a track. Track 0 is still opened here if present. */
	msu1_rsmp_reset();
	msu1_audio_open();
}

void S9xMSU1DeInit (void)
{
	msu1_src_close(&dataSrc);
	msu1_src_close(&audioSrc);
	audio_open_track = ~0U;
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
				uint8_t b;
				if (msu1_src_read(&dataSrc, MSU1.MSU1_DataReadOffset, &b, 1) != 1)
					return (0x00);
				MSU1.MSU1_DataReadOffset++;
				return (b);
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

			msu1_rsmp_reset();
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
/* Read one signed-16 LE sample at the current audio_cursor, streaming through
   audio_buf. Returns 0 past EOF. Advances audio_cursor by 2. No FILE calls
   unless the buffer needs refilling (once per ~4096 samples). */
static int16_t msu1_audio_sample (void)
{
	uint32_t	off;
	int16_t		v;

	if (!audioFile || audio_cursor + 2 > (uint32_t) audio_size)
		return (0);

	/* Refill if the cursor is outside the buffered window. */
	if (audio_cursor < audio_buf_base ||
	    audio_cursor + 2 > audio_buf_base + audio_buf_len)
	{
		uint32_t got = msu1_src_read(&audioSrc, audio_cursor, audio_buf, MSU1_AUDIO_BUFSZ);
		audio_buf_base = audio_cursor;
		audio_buf_len  = got;
		if (audio_buf_len < 2)
			return (0);
	}

	off = audio_cursor - audio_buf_base;
	v = (int16_t) ((uint16_t) audio_buf[off] | ((uint16_t) audio_buf[off + 1] << 8));
	audio_cursor += 2;
	return (v);
}

static void msu1_next_frame_44k (int32_t *outL, int32_t *outR)
{
	int16_t	l = 0, r = 0;

	if (MSU1.MSU1_AudioPlay && audioFile)
	{
		/* End-of-track: cursor reached the file size (cached, no FILE call). */
		if (audio_cursor + 4 > (uint32_t) audio_size || audio_cursor < 8)
		{
			if (!MSU1.MSU1_AudioRepeat)
			{
				MSU1.MSU1_AudioPlay = FALSE;
				MSU1.MSU1_AudioPlayOffset = 8;
				audio_cursor = 8;
				msu1_update_status();
			}
			else
			{
				MSU1.MSU1_AudioPlayOffset = MSU1.MSU1_AudioLoopOffset;
				audio_cursor = MSU1.MSU1_AudioLoopOffset;
			}
		}

		if (MSU1.MSU1_AudioPlay && audioFile)
		{
			int32_t	sl, sr;
			l = msu1_audio_sample();
			r = msu1_audio_sample();
			MSU1.MSU1_AudioPlayOffset = audio_cursor;
			/* Round to nearest rather than truncate toward zero. ares
			   computes this scale in double; plain integer division loses
			   up to 1 LSB toward zero at every level. sl mod 255 is never
			   exactly half, so ties can't occur and round-half-away here
			   agrees with the double-precision reference everywhere.
			   Range stays exact: 32767*255 + 127 -> 32767. */
			sl = (int32_t) l * (int32_t) MSU1.MSU1_VolumeB;
			sr = (int32_t) r * (int32_t) MSU1.MSU1_VolumeB;
			l = (int16_t) ((sl + ((sl >= 0) ? 127 : -127)) / 255);
			r = (int16_t) ((sr + ((sr >= 0) ? 127 : -127)) / 255);
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

void S9xMSU1Mix (int16_t *buffer, size_t sample_count, uint32_t output_rate)
{
	/* Resample the 44.1 kHz MSU1 stream to the caller's output rate with a
	   16.16 fixed-point step. In normal mode output_rate is the SPC's ~32040
	   Hz, so MSU1 is gently downsampled to match. In MSU-1 Enhanced Audio mode
	   the whole pipeline runs at 44100 Hz, so step == 1.0 and the MSU1 stream
	   passes through at native rate with no resampling loss. */
	uint32_t	step;
	uint32_t	frac;
	int32_t		curL, curR, nxtL, nxtR;
	size_t		i;

	if (!Settings.MSU1)
		return;
	if (!sample_count)
		return;
	if (!MSU1.MSU1_AudioPlay || !audioFile)
		return;
	if (output_rate == 0)
		output_rate = 32040;

	step = (uint32_t) (((uint64_t) 44100 << 16) / output_rate);

	/* The frame pair and phase persist in struct MSU1 across calls (and
	   across savestates), so consecutive batches interpolate seamlessly:
	   no source frames are dropped at batch boundaries, and at unity step
	   (enhanced mode, frac == 0) the stream passes through bit-exactly.
	   Priming happens once per track mount. */
	if (!MSU1.MSU1_RsmpPrimed)
	{
		msu1_next_frame_44k(&MSU1.MSU1_RsmpCurL, &MSU1.MSU1_RsmpCurR);
		msu1_next_frame_44k(&MSU1.MSU1_RsmpNxtL, &MSU1.MSU1_RsmpNxtR);
		MSU1.MSU1_RsmpFrac   = 0;
		MSU1.MSU1_RsmpPrimed = TRUE;
	}

	frac = MSU1.MSU1_RsmpFrac;
	curL = MSU1.MSU1_RsmpCurL;  curR = MSU1.MSU1_RsmpCurR;
	nxtL = MSU1.MSU1_RsmpNxtL;  nxtR = MSU1.MSU1_RsmpNxtR;

	for (i = 0; i < sample_count; i++)
	{
		/* The interpolation product needs 33 bits: |nxt - cur| reaches
		   65535 (full-scale adjacent-sample swing) and t reaches 65535,
		   overflowing int32 (signed UB, wraps to full-scale spikes).
		   Widen to 64-bit for the multiply. */
		uint32_t	t = frac & 0xffff;
		int32_t		mixL = curL + (int32_t) (((int64_t) (nxtL - curL) * (int32_t) t) >> 16);
		int32_t		mixR = curR + (int32_t) (((int64_t) (nxtR - curR) * (int32_t) t) >> 16);
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

	MSU1.MSU1_RsmpFrac = frac;
	MSU1.MSU1_RsmpCurL = curL;  MSU1.MSU1_RsmpCurR = curR;
	MSU1.MSU1_RsmpNxtL = nxtL;  MSU1.MSU1_RsmpNxtR = nxtR;
}

/* Savestate ------------------------------------------------------------------ */

void S9xMSU1PreSaveState (void)
{
	/* All persisted MSU1 state lives directly in struct MSU1 (serialised by
	   snapshot.c). MSU1_AudioPlayOffset already tracks the play cursor; the
	   data-stream offset is captured here so it can be re-seeked on load. */
	MSU1.MSU1_AudioPlayOffset = audio_cursor;
}

void S9xMSU1PostLoadState (void)
{
	/* Restore stream positions from the deserialised offsets, reopening only
	   what actually has to be reopened. Under Preemptive Frames this runs once
	   per displayed frame with dirty input; a close/open/header-parse per load
	   is a frame-time spike on VFS-backed platforms, and dropping the read-ahead
	   window forces a fresh 8 KB seek+read for a cursor that typically moved
	   back only a few hundred bytes. Seeking an already-open handle is free, and
	   the read-ahead buffer is content-addressed by audio_buf_base/audio_buf_len
	   so msu1_audio_sample() re-validates it on its own. */
	if (!dataFile)
		msu1_data_open();

	if (audioFile && audio_open_track == (uint32_t) MSU1.MSU1_CurrentTrack)
	{
		/* Same track already mounted: audio_size and MSU1_AudioLoopOffset are
		   derived from this same file and MSU1_AudioLoopOffset is itself
		   restored from the state, so only the play cursor needs reseating.
		   This path also leaves MSU1_AudioError as deserialised rather than
		   clearing it the way a reopen does, which is the more faithful
		   restore. */
		audio_cursor = MSU1.MSU1_AudioPlayOffset;
	}
	else
		msu1_audio_open();

	/* v8 states carry the interpolator state and replay byte-exactly. States
	   older than v8 leave these fields untouched by the unfreezer; reject
	   anything out of range and fall back to a clean re-prime (costs at most
	   a sub-sample phase step on legacy states). */
	if (MSU1.MSU1_RsmpPrimed > 1 ||
	    MSU1.MSU1_RsmpFrac >= 0x10000 ||
	    MSU1.MSU1_RsmpCurL < -32768 || MSU1.MSU1_RsmpCurL > 32767 ||
	    MSU1.MSU1_RsmpCurR < -32768 || MSU1.MSU1_RsmpCurR > 32767 ||
	    MSU1.MSU1_RsmpNxtL < -32768 || MSU1.MSU1_RsmpNxtL > 32767 ||
	    MSU1.MSU1_RsmpNxtR < -32768 || MSU1.MSU1_RsmpNxtR > 32767)
		msu1_rsmp_reset();

	/* Same treatment for the v9 enhanced-audio upsampler fields. */
	if (MSU1.MSU1_EnhFill > 2 ||
	    MSU1.MSU1_EnhFrac >= ((uint64_t) 1 << 32) ||
	    MSU1.MSU1_EnhCurL < -32768 || MSU1.MSU1_EnhCurL > 32767 ||
	    MSU1.MSU1_EnhCurR < -32768 || MSU1.MSU1_EnhCurR > 32767 ||
	    MSU1.MSU1_EnhNxtL < -32768 || MSU1.MSU1_EnhNxtL > 32767 ||
	    MSU1.MSU1_EnhNxtR < -32768 || MSU1.MSU1_EnhNxtR > 32767)
		msu1_enh_reset();

	msu1_update_status();
}
