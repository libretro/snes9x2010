/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#ifndef _BSX_H_
#define _BSX_H_

#include <stdio.h>
#include <stdint.h>
#include <streams/file_stream.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Bridges to the C++ side (defined in bsx.c, wired in memmap.cpp). */
extern uint8_t **BSXMemMap;
extern uint8_t  *BSXBlockIsRAM;
extern uint8_t  *BSXBlockIsROM;
extern uint8_t  *BSXRAMBase;
extern uint8_t  *BSXSRAMBase;
extern uint8_t  *BSXPSRAMBase;
extern uint8_t  *BSXBIOSROMBase;
extern uint8_t  *BSXROMBase;


struct SBSX
{
	uint8_t	dirty;			/* Changed register values */
	uint8_t	dirty2;			/* Changed register values */
	uint8_t	bootup;			/* Start in bios mapping */
	uint8_t	flash_enable;	/* Flash state */
	uint8_t	write_enable;	/* ROM write protection */
	uint8_t	read_enable;	/* Allow card vendor reading */
	uint32_t	flash_command;	/* Flash command */
	uint32_t	old_write;		/* Previous flash write address */
	uint32_t	new_write;		/* Current flash write address */
	uint8_t	out_index;
	uint8_t	output[32];
	uint8_t	PPU[32];
	uint8_t	MMC[16];
	uint8_t	prevMMC[16];
	uint8_t	test2192[32];

	uint8_t	flash_csr;
	uint8_t	flash_gsr;
	uint8_t	flash_bsr;
	uint8_t	flash_cmd_done;

	RFILE *sat_stream1;
	RFILE *sat_stream2;

	uint8_t	sat_pf_latch1_enable, sat_dt_latch1_enable;
	uint8_t	sat_pf_latch2_enable, sat_dt_latch2_enable;

	uint8_t	sat_stream1_loaded, sat_stream2_loaded;
	uint8_t	sat_stream1_first, sat_stream2_first;
	uint8_t	sat_stream1_count, sat_stream2_count;
	uint16_t	sat_stream1_queue, sat_stream2_queue;
};

extern struct SBSX	BSX;

uint8_t S9xGetBSX (uint32_t);
void S9xSetBSX (uint8_t, uint32_t);
uint8_t S9xGetBSXPPU (uint16_t);
void S9xSetBSXPPU (uint8_t, uint16_t);
uint8_t * S9xGetBasePointerBSX (uint32_t);
void S9xInitBSX (void);
void S9xResetBSX (void);
void S9xBSXPostLoadState (void);

#ifdef __cplusplus
}
#endif

#endif
