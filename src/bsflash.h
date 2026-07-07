/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  BS Memory (Satellaview memory pack) flash-chip emulation.

  This is a C port of ares' sfc/slot/bsmemory flash model (Sharp LH28F800SU
  family): the Intel/Sharp-style command queue, per-block erase/lock state,
  dual page buffers, compatible/extended/global status registers, and the
  "writes can only clear bits" flash semantics. Behaviour is matched against
  ares command-for-command; block-erase timing is modelled as a busy interval
  rather than ares' threaded coprocessor step (the SNES side observes the same
  ready/busy status progression).

  ROM cassettes (MaskROM, not Flash) bypass all of this and read straight
  through, exactly as in ares.
 ***********************************************************************************/

#ifndef _BSFLASH_H_
#define _BSFLASH_H_

#include <stdint.h>
#include "port.h"

/* Flash operating modes (what a CPU read returns). */
enum
{
	BSF_MODE_FLASH = 0,
	BSF_MODE_CHIP,
	BSF_MODE_PAGE,
	BSF_MODE_COMPAT_STATUS,
	BSF_MODE_EXT_STATUS
};

struct BSFlashBlockStatus
{
	uint8_t	vppLow;
	uint8_t	queueFull;
	uint8_t	aborted;
	uint8_t	failed;
	uint8_t	locked;   /* reset value 1 */
	uint8_t	ready;    /* reset value 1 */
};

struct BSFlashBlock
{
	uint32_t	erased;   /* erase-cycle count */
	uint8_t		locked;
	uint8_t		erasing;
	struct BSFlashBlockStatus status;
};

struct BSFlashQueueEntry
{
	uint8_t		valid;
	uint32_t	address;  /* 24-bit */
	uint8_t		data;
};

struct BSFlash
{
	uint8_t		*memory;     /* points at the flash image (Memory.ROM region) */
	uint32_t	 size;        /* flash size in bytes (0 = no flash / ROM cassette) */
	uint8_t		 is_rom;      /* 1 = MaskROM cassette: reads bypass flash logic */
	uint8_t		 writable;    /* /WP pin: 1 = write-protect off */

	/* Chip identity (vendor/device/serial), reported via Mode::Chip. */
	uint16_t	 chip_vendor;
	uint16_t	 chip_device;
	uint64_t	 chip_serial;

	/* Dual 256-byte page buffers; global.status.page selects the active one. */
	uint8_t		 page_buffer[2][256];

	struct BSFlashBlock	blocks[64];  /* up to 64 blocks (32 Mbit) */

	/* Compatible status register. */
	uint8_t		 c_vppLow, c_writeFailed, c_eraseFailed, c_eraseSuspended, c_ready;

	/* Global status register. */
	uint8_t		 g_page, g_pageReady, g_pageAvailable, g_queueFull;
	uint8_t		 g_sleeping, g_failed, g_suspended, g_ready;

	uint8_t		 mode;           /* BSF_MODE_* */
	uint8_t		 readyBusyMode;

	struct BSFlashQueueEntry queue[4];

	/* Erase-timing model: countdown of emulated ticks until an in-progress
	   block erase completes (ares uses a 300 ms threaded step; we expose the
	   same not-ready window and complete on tick expiry). */
	int32_t		 erase_ticks;
	int		 erase_block;    /* block id currently erasing, -1 = none */
};

extern struct BSFlash	BSFlashChip;

/* Lifecycle */
void	S9xBSFlashInit (uint8_t *memory, uint32_t size, uint8_t is_rom);
void	S9xBSFlashReset (void);
void	S9xBSFlashSetWritable (uint8_t writable);

/* CPU access to the flash chip (address is an offset into the flash image). */
uint8_t	S9xBSFlashRead (uint32_t address);
void	S9xBSFlashWrite (uint32_t address, uint8_t data);

/* Called once per emulated frame to advance block-erase timing. */
void	S9xBSFlashTick (void);

uint8_t	S9xBSFlashBusy (void);

#endif
