/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  BS Memory (Satellaview memory pack) flash-chip emulation.
  C port of ares' sfc/slot/bsmemory flash model. See bsflash.h.
 ***********************************************************************************/

#include <string.h>
#include "snes9x.h"
#include "bsflash.h"

struct BSFlash	BSFlashChip;

/* Block geometry: 16-bit blocks (65536 bytes each), like ares. */
#define BSF_BLOCK_BITS   16
#define BSF_BLOCK_BYTES  (1u << BSF_BLOCK_BITS)

static uint32_t bsf_block_count (void)
{
	return (BSFlashChip.size >> BSF_BLOCK_BITS);
}

/* Map an arbitrary block id onto the populated range (ares: id & (count-1)). */
static struct BSFlashBlock * bsf_block (uint32_t id)
{
	uint32_t count = bsf_block_count();
	if (count == 0)
		return (&BSFlashChip.blocks[0]);
	return (&BSFlashChip.blocks[id & (count - 1)]);
}

/* --- status register composition (ares bit layouts) --- */

static uint8_t bsf_block_status (struct BSFlashBlock *b)
{
	return (uint8_t) (
		  (b->status.vppLow    << 2)
		| (b->status.queueFull << 3)
		| (b->status.aborted   << 4)
		| (b->status.failed    << 5)
		| ((!b->status.locked) << 6)   /* d6 is the *unlocked* flag */
		| (b->status.ready     << 7));
}

static uint8_t bsf_compat_status (void)
{
	return (uint8_t) (
		  (BSFlashChip.c_vppLow         << 3)
		| (BSFlashChip.c_writeFailed    << 4)
		| (BSFlashChip.c_eraseFailed    << 5)
		| (BSFlashChip.c_eraseSuspended << 6)
		| (BSFlashChip.c_ready          << 7));
}

static uint8_t bsf_global_status (void)
{
	return (uint8_t) (
		  (BSFlashChip.g_page          << 0)
		| (BSFlashChip.g_pageReady     << 1)
		| (BSFlashChip.g_pageAvailable << 2)
		| (BSFlashChip.g_queueFull     << 3)
		| (BSFlashChip.g_sleeping      << 4)
		| (BSFlashChip.g_failed        << 5)
		| (BSFlashChip.g_suspended     << 6)
		| (BSFlashChip.g_ready         << 7));
}

/* --- page buffers --- */

static void bsf_page_swap (void)
{
	BSFlashChip.g_page ^= 1;
}

static uint8_t bsf_page_read (uint8_t address)
{
	return (BSFlashChip.page_buffer[BSFlashChip.g_page][address]);
}

static void bsf_page_write (uint8_t address, uint8_t data)
{
	BSFlashChip.page_buffer[BSFlashChip.g_page][address] = data;
}

/* --- block operations --- */

static void bsf_failed (void)
{
	BSFlashChip.c_writeFailed = 1;
	BSFlashChip.c_eraseFailed = 1;
	BSFlashChip.g_failed      = 1;
}

static uint8_t bsf_block_read (uint32_t id, uint32_t address)
{
	uint32_t count = bsf_block_count();
	id &= (count ? count - 1 : 0);
	address &= (BSF_BLOCK_BYTES - 1);
	return (BSFlashChip.memory[(id << BSF_BLOCK_BITS) | address]);
}

static void bsf_block_write (uint32_t id, uint32_t address, uint8_t data)
{
	struct BSFlashBlock *b = bsf_block(id);
	uint32_t count = bsf_block_count();

	if (!BSFlashChip.writable && b->status.locked)
	{
		b->status.failed = 1;
		bsf_failed();
		return;
	}

	/* Flash writes can only clear bits. id is masked to the populated range
	   (matching ares' block(id) -> blocks[id & count-1]). */
	id &= (count ? count - 1 : 0);
	address &= (BSF_BLOCK_BYTES - 1);
	{
		uint32_t off = (id << BSF_BLOCK_BITS) | address;
		data &= BSFlashChip.memory[off];
		BSFlashChip.memory[off] = data;
	}
}

/* Begin (or, when time has elapsed, complete) a block erase. ares runs the
   erase inside the coprocessor thread; here the SNES-visible effect is the
   same: status goes not-ready, and after the erase interval the block fills
   with 0xFF. We start the timer here and finish in S9xBSFlashTick. */
static void bsf_block_erase (uint32_t id)
{
	struct BSFlashBlock *b = bsf_block(id);

	b->erasing = 1;
	b->status.ready = 0;
	BSFlashChip.c_ready = 0;
	BSFlashChip.g_ready = 0;

	BSFlashChip.erase_block = (int) (id & (bsf_block_count() ? bsf_block_count() - 1 : 0));
	/* ~300 ms at 60 fps ~= 18 frames; erase completes on tick countdown. */
	BSFlashChip.erase_ticks = 18;
}

static void bsf_block_erase_complete (struct BSFlashBlock *b, uint32_t id)
{
	b->erasing = 0;

	if (!BSFlashChip.writable && b->status.locked)
		return;  /* silently no-ops when locked; no failure bits (ares) */

	{
		uint32_t address;
		for (address = 0; address < BSF_BLOCK_BYTES; address++)
			BSFlashChip.memory[(id << BSF_BLOCK_BITS) | address] = 0xff;
	}

	b->erased++;
	b->locked = 0;
	b->status.locked = 0;
}

static void bsf_block_lock (uint32_t id)
{
	struct BSFlashBlock *b = bsf_block(id);

	if (!BSFlashChip.writable)
	{
		b->status.failed = 1;
		bsf_failed();
		return;
	}

	b->locked = 1;
	b->status.locked = 1;
}

static void bsf_block_update (uint32_t id)
{
	struct BSFlashBlock *b = bsf_block(id);
	b->status.locked = b->locked;
}

/* --- command queue --- */

static void bsf_queue_flush (void)
{
	memset(BSFlashChip.queue, 0, sizeof(BSFlashChip.queue));
}

static void bsf_queue_pop (void)
{
	int i;
	for (i = 3; i >= 0; i--)
	{
		if (BSFlashChip.queue[i].valid)
		{
			memset(&BSFlashChip.queue[i], 0, sizeof(BSFlashChip.queue[i]));
			return;
		}
	}
}

static void bsf_queue_push (uint32_t address, uint8_t data)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		if (!BSFlashChip.queue[i].valid)
		{
			BSFlashChip.queue[i].valid   = 1;
			BSFlashChip.queue[i].address = address & 0xffffff;
			BSFlashChip.queue[i].data    = data;
			return;
		}
	}
}

static uint32_t bsf_queue_size (void)
{
	int i;
	for (i = 3; i >= 0; i--)
		if (BSFlashChip.queue[i].valid)
			return (uint32_t) (i + 1);
	return (0);
}

static uint32_t bsf_queue_address (uint32_t index)
{
	if (index > 3 || !BSFlashChip.queue[index].valid)
		return (0);
	return (BSFlashChip.queue[index].address);
}

static uint8_t bsf_queue_data (uint32_t index)
{
	if (index > 3 || !BSFlashChip.queue[index].valid)
		return (0);
	return (BSFlashChip.queue[index].data);
}

/* --- lifecycle --- */

void S9xBSFlashInit (uint8_t *memory, uint32_t size, uint8_t is_rom)
{
	uint32_t i;

	memset(&BSFlashChip, 0, sizeof(BSFlashChip));
	BSFlashChip.memory = memory;
	BSFlashChip.size   = size;
	BSFlashChip.is_rom = is_rom;
	BSFlashChip.writable = 1;

	/* Sharp vendor + device by size (ares). */
	BSFlashChip.chip_vendor = 0x00b0;
	if (size == 0x100000) BSFlashChip.chip_device = 0x66a8;  /* LH28F800SU */
	else                  BSFlashChip.chip_device = 0x6688;  /* LH28F016SU/032SU */
	BSFlashChip.chip_serial = 0x0001234567890ull;

	for (i = 0; i < 64; i++)
	{
		BSFlashChip.blocks[i].erased = 1;
		BSFlashChip.blocks[i].locked = 1;
		BSFlashChip.blocks[i].status.locked = 1;
		BSFlashChip.blocks[i].status.ready  = 1;
	}

	S9xBSFlashReset();
}

void S9xBSFlashReset (void)
{
	uint32_t i;

	for (i = 0; i < 64; i++)
	{
		BSFlashChip.blocks[i].erasing = 0;
		memset(&BSFlashChip.blocks[i].status, 0, sizeof(BSFlashChip.blocks[i].status));
		BSFlashChip.blocks[i].status.locked = 1;
		BSFlashChip.blocks[i].status.ready  = 1;
	}

	BSFlashChip.c_vppLow = BSFlashChip.c_writeFailed = BSFlashChip.c_eraseFailed = 0;
	BSFlashChip.c_eraseSuspended = 0;
	BSFlashChip.c_ready = 1;

	BSFlashChip.g_page = BSFlashChip.g_queueFull = BSFlashChip.g_sleeping = 0;
	BSFlashChip.g_failed = BSFlashChip.g_suspended = 0;
	BSFlashChip.g_pageReady = BSFlashChip.g_pageAvailable = BSFlashChip.g_ready = 1;

	BSFlashChip.mode = BSF_MODE_FLASH;
	BSFlashChip.readyBusyMode = 3;  /* Disable */
	BSFlashChip.erase_ticks = 0;
	BSFlashChip.erase_block = -1;
	bsf_queue_flush();
}

void S9xBSFlashSetWritable (uint8_t writable)
{
	BSFlashChip.writable = (!BSFlashChip.is_rom && writable) ? 1 : 0;
}

uint8_t S9xBSFlashBusy (void)
{
	return (BSFlashChip.erase_block >= 0);
}

/* --- CPU read --- */

uint8_t S9xBSFlashRead (uint32_t address)
{
	if (!BSFlashChip.memory || BSFlashChip.size == 0)
		return (0);

	if (BSFlashChip.is_rom)
		return (BSFlashChip.memory[address % BSFlashChip.size]);

	switch (BSFlashChip.mode)
	{
		case BSF_MODE_CHIP:
			if ((address & 0xffffff) == 0) return ((uint8_t) BSFlashChip.chip_vendor);
			if ((address & 0xffffff) == 1) return ((uint8_t) BSFlashChip.chip_device);
			if ((address & 7) == 2)        return (0x63);  /* repeats every 8 bytes */
			return (0x20);

		case BSF_MODE_PAGE:
			return (bsf_page_read((uint8_t) address));

		case BSF_MODE_COMPAT_STATUS:
			return (bsf_compat_status());

		case BSF_MODE_EXT_STATUS:
			if ((address & 0xffff) == 0x0002)
				return (bsf_block_status(bsf_block(address >> BSF_BLOCK_BITS)));
			if ((address & 0xffff) == 0x0004)
				return (bsf_global_status());
			return (0x00);

		default:  /* BSF_MODE_FLASH */
			return (bsf_block_read((address >> BSF_BLOCK_BITS) &
				(bsf_block_count() ? bsf_block_count() - 1 : 0), address));
	}
}

/* --- CPU write (command processing) --- */

void S9xBSFlashWrite (uint32_t address, uint8_t data)
{
	uint8_t cmd;

	if (!BSFlashChip.memory || BSFlashChip.size == 0 || BSFlashChip.is_rom)
		return;

	bsf_queue_push(address, data);
	cmd = bsf_queue_data(0);

	switch (cmd)
	{
		case 0x0c:  /* write page to flash */
		{
			uint16_t count;
			uint32_t addr;
			if (bsf_queue_size() < 3) return;
			count  = (uint16_t) (bsf_queue_data(!(bsf_queue_address(1) & 1) ? 1 : 2));
			count |= (uint16_t) (bsf_queue_data(!(bsf_queue_address(1) & 1) ? 2 : 1) << 8);
			addr   = bsf_queue_address(2);
			do {
				bsf_block_write(addr >> BSF_BLOCK_BITS, addr, bsf_page_read((uint8_t) addr));
				addr++;
			} while (count--);
			bsf_page_swap();
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;
		}

		case 0x10:  /* write byte */
		case 0x40:  /* write byte (alt) */
			if (bsf_queue_size() < 2) return;
			bsf_block_write(bsf_queue_address(1) >> BSF_BLOCK_BITS, bsf_queue_address(1), bsf_queue_data(1));
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;

		case 0x20:  /* erase block */
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			bsf_block_erase(bsf_queue_address(1) >> BSF_BLOCK_BITS);
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;

		case 0x38:  /* write MP signature/identifier/serial into page buffer */
		{
			int shift;
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			bsf_page_write(0x00, 0x4d);  /* 'M' */
			bsf_page_write(0x02, 0x50);  /* 'P' */
			bsf_page_write(0x04, 0x04);
			{
				uint32_t s = BSFlashChip.size >> 10;
				uint8_t  l = 0;
				while (s > 1) { s >>= 1; l++; }
				bsf_page_write(0x06, (uint8_t) (0x10 | (l & 0x0f)));
			}
			shift = 40;
			bsf_page_write(0x08, (uint8_t) (BSFlashChip.chip_serial >> 40));
			bsf_page_write(0x0a, (uint8_t) (BSFlashChip.chip_serial >> 32));
			bsf_page_write(0x0c, (uint8_t) (BSFlashChip.chip_serial >> 24));
			bsf_page_write(0x0e, (uint8_t) (BSFlashChip.chip_serial >> 16));
			bsf_page_write(0x10, (uint8_t) (BSFlashChip.chip_serial >>  8));
			bsf_page_write(0x12, (uint8_t) (BSFlashChip.chip_serial >>  0));
			(void) shift;
			bsf_page_swap();
			bsf_queue_flush();
			return;
		}

		case 0x50:  /* clear status register */
		{
			uint32_t id, count = bsf_block_count();
			for (id = 0; id < count; id++)
			{
				BSFlashChip.blocks[id].status.vppLow = 0;
				BSFlashChip.blocks[id].status.failed = 0;
			}
			BSFlashChip.c_vppLow = 0;
			BSFlashChip.c_writeFailed = 0;
			BSFlashChip.c_eraseFailed = 0;
			BSFlashChip.g_failed = 0;
			bsf_queue_flush();
			return;
		}

		case 0x70:  /* read compatible status register */
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;

		case 0x71:  /* read extended status registers */
			BSFlashChip.mode = BSF_MODE_EXT_STATUS;
			bsf_queue_flush();
			return;

		case 0x72:  /* page buffer swap */
			bsf_page_swap();
			bsf_queue_flush();
			return;

		case 0x74:  /* single load to page buffer */
			if (bsf_queue_size() < 2) return;
			bsf_page_write((uint8_t) bsf_queue_address(1), bsf_queue_data(1));
			bsf_queue_flush();
			return;

		case 0x75:  /* read page buffer */
			BSFlashChip.mode = BSF_MODE_PAGE;
			bsf_queue_flush();
			return;

		case 0x77:  /* lock block */
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			bsf_block_lock(bsf_queue_address(1) >> BSF_BLOCK_BITS);
			bsf_queue_flush();
			return;

		case 0x80:  /* abort -> sleep */
			BSFlashChip.g_sleeping = 1;
			bsf_queue_flush();
			return;

		case 0x90:  /* read chip identifiers */
			BSFlashChip.mode = BSF_MODE_CHIP;
			bsf_queue_flush();
			return;

		case 0x96:  /* update ry/by mode */
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) == 0x01) BSFlashChip.readyBusyMode = 0;
			if (bsf_queue_data(1) == 0x02) BSFlashChip.readyBusyMode = 1;
			if (bsf_queue_data(1) == 0x03) BSFlashChip.readyBusyMode = 2;
			if (bsf_queue_data(1) == 0x04) BSFlashChip.readyBusyMode = 3;
			bsf_queue_flush();
			return;

		case 0x97:  /* upload lock status bits */
		{
			uint32_t id, count = bsf_block_count();
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			for (id = 0; id < count; id++)
				bsf_block_update(id);
			bsf_queue_flush();
			return;
		}

		case 0x99:  /* upload device information */
		{
			uint32_t id, count = bsf_block_count();
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			bsf_page_write(0x06, 0x06);
			bsf_page_write(0x07, 0x00);
			for (id = 0; id < count; id++)
			{
				uint8_t  addr = 0;
				uint32_t erased;
				int      byte;
				addr += (uint8_t) ((id & 0x03) * 0x08);
				addr += (uint8_t) (((id >> 2) & 0x03) * 0x40);
				addr += (uint8_t) (((id >> 4) & 0x01) * 0x20);
				addr += (uint8_t) (((id >> 5) & 0x01) * 0x04);
				erased = (1u << 31) | BSFlashChip.blocks[id].erased;
				for (byte = 0; byte < 4; byte++)
					bsf_page_write((uint8_t) (addr + byte), (uint8_t) (erased >> (byte * 8)));
			}
			bsf_page_swap();
			bsf_queue_flush();
			return;
		}

		case 0xa7:  /* erase all blocks */
		{
			uint32_t id, count = bsf_block_count();
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			for (id = 0; id < count; id++)
				bsf_block_erase(id);
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;
		}

		case 0xb0:  /* erase suspend/resume (unsupported no-op) */
			if (bsf_queue_size() < 2) return;
			if (bsf_queue_data(1) != 0xd0) { bsf_failed(); bsf_queue_flush(); return; }
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;

		case 0xe0:  /* sequential load to page buffer */
		{
			uint16_t count;
			if (bsf_queue_size() < 4) return;
			count  = (uint16_t) bsf_queue_data(1);
			count |= (uint16_t) (bsf_queue_data(2) << 8);
			bsf_page_write((uint8_t) bsf_queue_address(3), bsf_queue_data(3));
			if (count--)
			{
				/* rewrite the count bytes and pop, avoiding a huge queue */
				BSFlashChip.queue[1].data = (uint8_t) count;
				BSFlashChip.queue[2].data = (uint8_t) (count >> 8);
				bsf_queue_pop();
				return;
			}
			bsf_queue_flush();
			return;
		}

		case 0xf0:  /* sleep */
			BSFlashChip.g_sleeping = 1;
			bsf_queue_flush();
			return;

		case 0xfb:  /* write word */
		{
			uint16_t value;
			uint32_t addr;
			if (bsf_queue_size() < 3) return;
			value  = (uint16_t) bsf_queue_data(!(bsf_queue_address(1) & 1) ? 1 : 2);
			value |= (uint16_t) (bsf_queue_data(!(bsf_queue_address(1) & 1) ? 2 : 1) << 8);
			addr   = bsf_queue_address(2);
			bsf_block_write(addr >> BSF_BLOCK_BITS, addr ^ 0, (uint8_t) value);
			bsf_block_write(addr >> BSF_BLOCK_BITS, addr ^ 1, (uint8_t) (value >> 8));
			BSFlashChip.mode = BSF_MODE_COMPAT_STATUS;
			bsf_queue_flush();
			return;
		}

		case 0xff:  /* read flash memory */
			BSFlashChip.mode = BSF_MODE_FLASH;
			bsf_queue_flush();
			return;

		default:    /* unknown command */
			bsf_queue_flush();
			return;
	}
}

void S9xBSFlashTick (void)
{
	if (BSFlashChip.erase_block < 0)
		return;

	if (--BSFlashChip.erase_ticks > 0)
		return;

	/* Erase interval elapsed: complete the erase and clear busy status. */
	{
		uint32_t id = (uint32_t) BSFlashChip.erase_block;
		bsf_block_erase_complete(&BSFlashChip.blocks[id], id);
		BSFlashChip.blocks[id].status.ready = 1;
		BSFlashChip.c_ready = 1;
		BSFlashChip.g_ready = 1;
		BSFlashChip.erase_block = -1;
		BSFlashChip.erase_ticks = 0;
	}
}
