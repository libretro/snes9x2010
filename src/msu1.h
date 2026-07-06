/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  MSU1 emulator code

  MSU1 is the audio + data-ROM expansion "chip" from byuu/near. It exposes
  eight MMIO registers at $2000-$2007 (mirrored into banks $00-$3F / $80-$BF)
  and streams:
    - arbitrary data from a companion "<rom>.msu" data ROM via a seek/read port
    - 44.1 kHz signed-16 stereo PCM audio from "<rom>-<track>.pcm" track files

  The register/data semantics here are matched bit-for-bit against ares'
  sfc/coprocessor/msu1 and the MiSTer SNES MSU1 RTL. The audio is decoded
  exactly (s16 LE, volume * n/255) and mixed into the SPC output stream at the
  SNES native rate, with the frontend performing final resampling to host rate
  (this core carries no internal resampler).
 ***********************************************************************************/

#ifndef _MSU1_H_
#define _MSU1_H_

#include <stdint.h>
#include "port.h"

/* MSU1 status register ($2000) flag bits. */
enum
{
	MSU1_Revision       = 0x02,  /* hardware revision reported in bits 0-2 (max 0x07) */
	MSU1_AudioError     = 0x08,
	MSU1_AudioPlaying   = 0x10,
	MSU1_AudioRepeating = 0x20,
	MSU1_AudioBusy      = 0x40,
	MSU1_DataBusy       = 0x80
};

struct SMSU1
{
	uint32_t	MSU1_STATUS;

	uint32_t	MSU1_DataSeekOffset;
	uint32_t	MSU1_DataReadOffset;

	uint32_t	MSU1_AudioPlayOffset;
	uint32_t	MSU1_AudioLoopOffset;

	uint16_t	MSU1_CurrentTrack;
	uint8_t		MSU1_VolumeB;

	uint32_t	MSU1_AudioResumeTrack;
	uint32_t	MSU1_AudioResumeOffset;

	uint8_t		MSU1_Control;

	uint8_t		MSU1_AudioError;
	uint8_t		MSU1_AudioPlay;
	uint8_t		MSU1_AudioRepeat;
	uint8_t		MSU1_AudioBusy;
	uint8_t		MSU1_DataBusy;
};

extern struct SMSU1	MSU1;

/* Lifecycle */
void	S9xResetMSU1 (void);
void	S9xMSU1Init (void);
void	S9xMSU1DeInit (void);
uint8_t	S9xMSU1ROMExists (void);
void	S9xMSU1SetROMPath (const char *rom_path);

/* MMIO ($2000-$2007) */
uint8_t	S9xMSU1ReadPort (uint8_t port);
void	S9xMSU1WritePort (uint8_t port, uint8_t byte);

/* Audio: mix `sample_count` stereo frames of MSU1 audio (at the SNES native
 * output rate) into the caller's 16-bit interleaved LR buffer. Adds to what is
 * already there (SPC output); a no-op when nothing is playing. */
void	S9xMSU1Mix (int16_t *buffer, size_t sample_count);

/* Savestate */
void	S9xMSU1PreSaveState (void);
void	S9xMSU1PostLoadState (void);

#endif
