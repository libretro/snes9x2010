/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

/* Dreamer Nom wrote: */
/* Large thanks to John Weidman for all his initial research */
/* Thanks to Seph3 for his modem notes */


#include <stdint.h>
#include <stdio.h>
#include <streams/file_stream.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "snes9x.h"
#include "memmap.h"
#include "display.h"
#include "bsx.h"
#include "bsflash.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static int bsx_stream_get (RFILE *f)
{
	int c = filestream_getc(f);
	return (c == EOF) ? 0xFF : c;
}

/* Backported from mainline snes9x; uses this codebase's globals directly. */
extern uint8_t     OpenBus;

/*#define BSX_DEBUG */

#define BIOS_SIZE	0x100000
#define FLASH_SIZE	0x100000
#define PSRAM_SIZE	0x80000

#define Map			Memory.Map
#define BlockIsRAM	Memory.BlockIsRAM
#define BlockIsROM	Memory.BlockIsROM
#define RAM			Memory.RAM
#define SRAM		Memory.SRAM
#define PSRAM		Memory.BSRAM
#define BIOSROM		Memory.BIOSROM
#define ROM			Memory.ROM


#define BSXPPUBASE	0x2180

struct SBSX_RTC
{
	int year;
	int month;
	int dayweek;
	int day;
	int	hours;
	int	minutes;
	int	seconds;
	int	ticks;
};

static struct SBSX_RTC	BSX_RTC;

/* flash card vendor information */
static const uint8_t	flashcard[20] =
{
	0x4D, 0x00, 0x50, 0x00,	/* vendor id */
	0x00, 0x00,				/* ? */
	0x1A, 0x00,				/* 2MB Flash (1MB = 0x2A) */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#if 0
static const uint8_t	init2192[32] =	/* FIXME */
{
	/* Layout follows the MiSTer BSX RTL channel-0 data stream: */
	/* [4]=0x10, [5]=[6]=0x01 fixed; [10..12]=sec/min/hour; */
	/* [13]=weekday, [14]=day, [15]=month, [16..17]=year (LE), seeded later. */
	0x00, 0x00, 0x00, 0x00, 0x10,
	0x01, 0x01, 0x00, 0x00, 0x00,
	0x00,					/* seconds */
	0x00,					/* minutes */
	0x00,					/* hours */
	0x00, 0x00, 0x00, 0x00, 0x00,	/* weekday, day, month, year lo/hi */
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

static uint8_t	FlashMode;
static uint32_t	FlashSize;
static uint8_t	*MapROM, *FlashROM;

static void BSX_Map_SNES (void);
static void BSX_Map_LoROM (void);
static void BSX_Map_HiROM (void);
static void BSX_Map_MMC (void);
static void BSX_Map_FlashIO (void);
static void BSX_Map_SRAM (void);
static void BSX_Map_PSRAM (void);
static void BSX_Map_BIOS (void);
static void BSX_Map_RAM (void);
static void BSX_Map (void);
static void S9xBSXSetStream1 (uint8_t count);
static void S9xBSXSetStream2 (uint8_t count);
static uint8_t S9xBSXGetRTC (void);
static uint8_t BSX_LoadBIOS (void);
static void map_psram_mirror_sub (uint32_t);
static int is_bsx (unsigned char *);


static void BSX_Map_SNES (void)
{
	/* These maps will be partially overwritten */

	int	c;

	/* Banks 00->3F and 80->BF */
	for (c = 0; c < 0x400; c += 16)
	{
		Map[c + 0] = Map[c + 0x800] = RAM;
		Map[c + 1] = Map[c + 0x801] = RAM;
		BlockIsRAM[c + 0] = BlockIsRAM[c + 0x800] = TRUE;
		BlockIsRAM[c + 1] = BlockIsRAM[c + 0x801] = TRUE;

		Map[c + 2] = Map[c + 0x802] = (uint8_t *) MAP_PPU;
		Map[c + 3] = Map[c + 0x803] = (uint8_t *) MAP_PPU;
		Map[c + 4] = Map[c + 0x804] = (uint8_t *) MAP_CPU;
		Map[c + 5] = Map[c + 0x805] = (uint8_t *) MAP_CPU;
		Map[c + 6] = Map[c + 0x806] = (uint8_t *) MAP_NONE;
		Map[c + 7] = Map[c + 0x807] = (uint8_t *) MAP_NONE;
	}
}

static void BSX_Map_LoROM (void)
{
	/* These maps will be partially overwritten */

	int	i, c;

	/* Banks 00->3F and 80->BF */
	for (c = 0; c < 0x400; c += 16)
	{
		for (i = c + 8; i < c + 16; i++)
		{
			Map[i] = Map[i + 0x800] = &MapROM[(c << 11) % FlashSize] - 0x8000;
			BlockIsRAM[i] = BlockIsRAM[i + 0x800] = BSX.write_enable;
			BlockIsROM[i] = BlockIsROM[i + 0x800] = !BSX.write_enable;
		}
	}

	/* Banks 40->7F and C0->FF */
	for (c = 0; c < 0x400; c += 16)
	{
		for (i = c; i < c + 8; i++)
			Map[i + 0x400] = Map[i + 0xC00] = &MapROM[(c << 11) % FlashSize];

		for (i = c + 8; i < c + 16; i++)
			Map[i + 0x400] = Map[i + 0xC00] = &MapROM[(c << 11) % FlashSize] - 0x8000;

		for (i = c; i < c + 16; i++)
		{
			BlockIsRAM[i + 0x400] = BlockIsRAM[i + 0xC00] = BSX.write_enable;
			BlockIsROM[i + 0x400] = BlockIsROM[i + 0xC00] = !BSX.write_enable;
		}
	}
}

static void BSX_Map_HiROM (void)
{
	/* These maps will be partially overwritten */

	int	i, c;

	/* Banks 00->3F and 80->BF */
	for (c = 0; c < 0x400; c += 16)
	{
		for (i = c + 8; i < c + 16; i++)
		{
			Map[i] = Map[i + 0x800] = &MapROM[(c << 12) % FlashSize];
			BlockIsRAM[i] = BlockIsRAM[i + 0x800] = BSX.write_enable;
			BlockIsROM[i] = BlockIsROM[i + 0x800] = !BSX.write_enable;
		}
	}

	/* Banks 40->7F and C0->FF */
	for (c = 0; c < 0x400; c += 16)
	{
		for (i = c; i < c + 16; i++)
		{
			Map[i + 0x400] = Map[i + 0xC00] = &MapROM[(c << 12) % FlashSize];
			BlockIsRAM[i + 0x400] = BlockIsRAM[i + 0xC00] = BSX.write_enable;
			BlockIsROM[i + 0x400] = BlockIsROM[i + 0xC00] = !BSX.write_enable;
		}
	}
}

static void BSX_Map_MMC (void)
{
	int	c;

	/* Banks 01->0E:5000-5FFF */
	for (c = 0x010; c < 0x0F0; c += 16)
	{
		Map[c + 5] = (uint8_t *) MAP_BSX;
		BlockIsRAM[c + 5] = BlockIsROM[c + 5] = FALSE;
	}
}

static void BSX_Map_FlashIO (void)
{
	int	i, c;

	if (BSX.prevMMC[0x0C])
	{
		/* Banks 00->3F and 80->BF */
		for (c = 0; c < 0x400; c += 16)
		{
			for (i = c + 8; i < c + 16; i++)
			{
				Map[i] = Map[i + 0x800] = (uint8_t *)MAP_BSX;
				BlockIsRAM[i] = BlockIsRAM[i + 0x800] = TRUE;
				BlockIsROM[i] = BlockIsROM[i + 0x800] = FALSE;
			}
		}

		/* Banks 40->7F and C0->FF */
		for (c = 0; c < 0x400; c += 16)
		{
			for (i = c; i < c + 16; i++)
			{
				Map[i + 0x400] = Map[i + 0xC00] = (uint8_t *)MAP_BSX;
				BlockIsRAM[i + 0x400] = BlockIsRAM[i + 0xC00] = TRUE;
				BlockIsROM[i + 0x400] = BlockIsROM[i + 0xC00] = FALSE;
			}
		}
	}	
}

static void BSX_Map_SRAM (void)
{
	int	c;

	/* Banks 10->17:5000-5FFF */
	for (c = 0x100; c < 0x180; c += 16)
	{
		Map[c + 5] = (uint8_t *) SRAM + ((c & 0x70) << 8) - 0x5000;
		BlockIsRAM[c + 5] = TRUE;
		BlockIsROM[c + 5] = FALSE;
	}
}

static void map_psram_mirror_sub (uint32_t bank)
{
	int	i, c;

	bank <<= 4;

	if (BSX.prevMMC[0x02])
	{
		/*HiROM */
		for (c = 0; c < 0x80; c += 16)
		{
			if ((bank & 0x7F0) >= 0x400)
			{
				for (i = c; i < c + 16; i++)
				{
					Map[i + bank] = &PSRAM[(c << 12) % PSRAM_SIZE];
					BlockIsRAM[i + bank] = TRUE;
					BlockIsROM[i + bank] = FALSE;
				}
			}
			else
			{
				for (i = c + 8; i < c + 16; i++)
				{
					Map[i + bank] = &PSRAM[(c << 12) % PSRAM_SIZE];
					BlockIsRAM[i + bank] = TRUE;
					BlockIsROM[i + bank] = FALSE;
				}
			}
		}
	}
	else
	{
		/*LoROM */
		for (c = 0; c < 0x100; c += 16)
		{
			if ((bank & 0x7F0) >= 0x400)
			{
				for (i = c; i < c + 8; i++)
				{
					Map[i + bank] = &PSRAM[(c << 11) % PSRAM_SIZE];
					BlockIsRAM[i + bank] = TRUE;
					BlockIsROM[i + bank] = FALSE;
				}
			}

			for (i = c + 8; i < c + 16; i++)
			{
				Map[i + bank] = &PSRAM[(c << 11) % PSRAM_SIZE] - 0x8000;
				BlockIsRAM[i + bank] = TRUE;
				BlockIsROM[i + bank] = FALSE;
			}
		}
	}
}

static void BSX_Map_PSRAM(void)
{
	int	c;

	if (!BSX.prevMMC[0x02])
	{
		/*LoROM Mode */
		if (!BSX.prevMMC[0x05] && !BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 00-0F/80-8F */
			if (BSX.prevMMC[0x03])
				map_psram_mirror_sub(0x00);

			if (BSX.prevMMC[0x04])
				map_psram_mirror_sub(0x80);
		}
		else if (BSX.prevMMC[0x05] && !BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 20-2F/A0-AF */
			if (BSX.prevMMC[0x03])
				map_psram_mirror_sub(0x20);

			if (BSX.prevMMC[0x04])
				map_psram_mirror_sub(0xA0);
		}
		else if (!BSX.prevMMC[0x05] && BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 40-4F/C0-CF */
			if (BSX.prevMMC[0x03])
				map_psram_mirror_sub(0x40);

			if (BSX.prevMMC[0x04])
				map_psram_mirror_sub(0xC0);
		}
		else
		{
			/*Map PSRAM to 60-6F/E0-EF */
			if (BSX.prevMMC[0x03])
				map_psram_mirror_sub(0x60);

			if (BSX.prevMMC[0x04])
				map_psram_mirror_sub(0xE0);
		}

		/*Map PSRAM to 70-7D/F0-FF */
		if (BSX.prevMMC[0x03])
			map_psram_mirror_sub(0x70);

		if (BSX.prevMMC[0x04])
			map_psram_mirror_sub(0xF0);
	}
	else
	{
		/*HiROM Mode */
		if (!BSX.prevMMC[0x05] && !BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 00-07/40-47 / 80-87/C0-C7 */
			if (BSX.prevMMC[0x03])
			{
				map_psram_mirror_sub(0x00);
				map_psram_mirror_sub(0x40);
			}

			if (BSX.prevMMC[0x04])
			{
				map_psram_mirror_sub(0x80);
				map_psram_mirror_sub(0xC0);
			}
		}
		else if (BSX.prevMMC[0x05] && !BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 10-17/50-57 / 90-97-D0-D7 */
			if (BSX.prevMMC[0x03])
			{
				map_psram_mirror_sub(0x10);
				map_psram_mirror_sub(0x50);
			}

			if (BSX.prevMMC[0x04])
			{
				map_psram_mirror_sub(0x90);
				map_psram_mirror_sub(0xD0);
			}
		}
		else if (!BSX.prevMMC[0x05] && BSX.prevMMC[0x06])
		{
			/*Map PSRAM to 20-27/60-67 / A0-A7/E0-E7 */
			if (BSX.prevMMC[0x03])
			{
				map_psram_mirror_sub(0x20);
				map_psram_mirror_sub(0x60);
			}

			if (BSX.prevMMC[0x04])
			{
				map_psram_mirror_sub(0xA0);
				map_psram_mirror_sub(0xE0);
			}
		}
		else
		{
			/*Map PSRAM to 30-37/70-77 / B0-B7/F0-F7 */
			if (BSX.prevMMC[0x03])
			{
				map_psram_mirror_sub(0x30);
				map_psram_mirror_sub(0x70);
			}

			if (BSX.prevMMC[0x04])
			{
				map_psram_mirror_sub(0xB0);
				map_psram_mirror_sub(0xF0);
			}
		}

		if (BSX.prevMMC[0x03])
		{
			/*Map PSRAM to 20->3F:6000-7FFF */
			for (c = 0x200; c < 0x400; c += 16)
			{
				Map[c + 6] = &PSRAM[((c & 0x70) << 12) % PSRAM_SIZE];
				Map[c + 7] = &PSRAM[((c & 0x70) << 12) % PSRAM_SIZE];
				BlockIsRAM[c + 6] = TRUE;
				BlockIsRAM[c + 7] = TRUE;
				BlockIsROM[c + 6] = FALSE;
				BlockIsROM[c + 7] = FALSE;
			}
		}

		if (BSX.prevMMC[0x04])
		{
			/*Map PSRAM to A0->BF:6000-7FFF */
			for (c = 0xA00; c < 0xC00; c += 16)
			{
				Map[c + 6] = &PSRAM[((c & 0x70) << 12) % PSRAM_SIZE];
				Map[c + 7] = &PSRAM[((c & 0x70) << 12) % PSRAM_SIZE];
				BlockIsRAM[c + 6] = TRUE;
				BlockIsRAM[c + 7] = TRUE;
				BlockIsROM[c + 6] = FALSE;
				BlockIsROM[c + 7] = FALSE;
			}
		}
	}
}

static void BSX_Map_BIOS (void)
{
	int	i,c;

	/* Banks 00->1F:8000-FFFF */
	if (BSX.prevMMC[0x07])
	{
		for (c = 0; c < 0x200; c += 16)
		{
			for (i = c + 8; i < c + 16; i++)
			{
				Map[i] = &BIOSROM[(c << 11) % BIOS_SIZE] - 0x8000;
				BlockIsRAM[i] = FALSE;
				BlockIsROM[i] = TRUE;
			}
		}
	}

	/* Banks 80->9F:8000-FFFF */
	if (BSX.prevMMC[0x08])
	{
		for (c = 0; c < 0x200; c += 16)
		{
			for (i = c + 8; i < c + 16; i++)
			{
				Map[i + 0x800] = &BIOSROM[(c << 11) % BIOS_SIZE] - 0x8000;
				BlockIsRAM[i + 0x800] = FALSE;
				BlockIsROM[i + 0x800] = TRUE;
			}
		}
	}
}

static void BSX_Map_RAM (void)
{
	int	c;

	/* Banks 7E->7F */
	for (c = 0; c < 16; c++)
	{
		Map[c + 0x7E0] = RAM;
		Map[c + 0x7F0] = RAM + 0x10000;
		BlockIsRAM[c + 0x7E0] = TRUE;
		BlockIsRAM[c + 0x7F0] = TRUE;
		BlockIsROM[c + 0x7E0] = FALSE;
		BlockIsROM[c + 0x7F0] = FALSE;
	}
}

static void BSX_Map (void)
{
#ifdef BSX_DEBUG
	int i;

	printf("BS: Remapping\n");
	for (i = 0; i < 32; i++)
		printf("BS: MMC %02X: %d\n", i, BSX.MMC[i]);
#endif

	memcpy(BSX.prevMMC, BSX.MMC, sizeof(BSX.MMC));

	MapROM = FlashROM;
	FlashSize = FLASH_SIZE;
	
	if (BSX.prevMMC[0x02])
		BSX_Map_HiROM();
	else
		BSX_Map_LoROM();
	
	BSX_Map_FlashIO();
	BSX_Map_PSRAM();

	BSX_Map_SNES();
	BSX_Map_SRAM();
	BSX_Map_RAM();

	BSX_Map_BIOS();
	BSX_Map_MMC();

	/* Monitor new register changes */
	BSX.dirty  = FALSE;
	BSX.dirty2 = FALSE;

	map_WriteProtectROM();
}

static uint8_t BSX_Get_Bypass_FlashIO (uint32_t offset)
{
	/*For games other than BS-X */
	FlashROM = ROM + Multi.cartOffsetB;

	if (BSX.prevMMC[0x02])
		return (FlashROM[offset & 0x0FFFFF]);
	else
		return (FlashROM[(offset & 0x1F0000) >> 1 | (offset & 0x7FFF)]);
}

static void BSX_Set_Bypass_FlashIO (uint32_t offset, uint8_t byte)
{
	/*For games other than BS-X */
	FlashROM = ROM + Multi.cartOffsetB;

	if (BSX.prevMMC[0x02])
		FlashROM[offset & 0x0FFFFF] = FlashROM[offset & 0x0FFFFF] & byte;
	else
		FlashROM[(offset & 0x1F0000) >> 1 | (offset & 0x7FFF)] = FlashROM[(offset & 0x1F0000) >> 1 | (offset & 0x7FFF)] & byte;
}

uint8_t S9xGetBSX (uint32_t address)
{
	uint8_t	bank = (address >> 16) & 0xFF;
	uint16_t	offset = address & 0xFFFF;
	uint8_t	t = 0;

	/* MMC */
	if ((bank >= 0x01 && bank <= 0x0E) && ((address & 0xF000) == 0x5000))
		return (BSX.MMC[bank]);

	/* Flash Mapping */

	/* default: read-through mode */
	t = BSX_Get_Bypass_FlashIO(address);

	/* When the ares-accurate flash chip is in one of the command modes it */
	/* owns (chip-id, page buffer, compatible/extended status), its response */
	/* is authoritative: these are position-independent register reads the */
	/* BS-X BIOS performs after a Sharp command. In plain flash-read mode */
	/* the chip is not consulted and the legacy status flags below apply. */
	if (BSFlashChip.memory && BSFlashChip.size && !BSFlashChip.is_rom &&
		BSFlashChip.mode != BSF_MODE_FLASH)
		return (S9xBSFlashRead(offset));

	/* note: may be more registers, purposes unknown */
	switch (offset)
	{
		case 0x0002:
		case 0x8002:
			if (BSX.flash_bsr)
				t = 0xC0; /* Page Status Register */
			break;

		case 0x0004:
		case 0x8004:
			if (BSX.flash_gsr)
				t = 0x82; /* Global Status Register */
			break;

		case 0x5555:
			if (BSX.flash_enable)
				t = 0x80; /* ??? */
			break;

		case 0xFF00:
		case 0xFF02:
		case 0xFF04:
		case 0xFF06:
		case 0xFF08:
		case 0xFF0A:
		case 0xFF0C:
		case 0xFF0E:
		case 0xFF10:
		case 0xFF12:
			/* return flash vendor information */
			if (BSX.read_enable)
				t = flashcard[offset - 0xFF00];
			break;
	}

	if (BSX.flash_csr)
	{
		t = 0x80; /* Compatible Status Register */
		BSX.flash_csr = FALSE;
	}

	return (t);
}

void S9xSetBSX (uint8_t byte, uint32_t address)
{
	uint8_t	bank = (address >> 16) & 0xFF;

	/* MMC */
	if ((bank >= 0x01 && bank <= 0x0E) && ((address & 0xF000) == 0x5000))
	{
		/*Avoid updating the memory map when it is not needed. */
		/* ares MCC: register 14 commits the latched registers only when */
		/* written with bit 7 set; a write with bit 7 clear must not commit. */
		if (bank == 0x0E && (byte & 0x80) && BSX.dirty)
		{
			BSX_Map();
			BSX.dirty = FALSE;
		}
		else if (bank != 0x0E && BSX.MMC[bank] != byte)
		{
			BSX.dirty = TRUE;
		}

		BSX.MMC[bank] = byte;
	}

	/* Flash IO */

	/* Feed every $C0-bank write to the ares-accurate flash chip first so its */
	/* mode, page buffers and status registers track the real Sharp command */
	/* sequence (program data bytes after 0x10/0x40 arrive this way too). The */
	/* legacy state machine below is kept in lock-step so the verified boot */
	/* path is unchanged; reads consult the chip only for the command modes */
	/* it owns (see BSX_Get_FlashIO above). */
	if (bank == 0xC0 && BSFlashChip.memory && BSFlashChip.size &&
		!BSFlashChip.is_rom)
		S9xBSFlashWrite(address & 0xFFFF, byte);

	/* Write to Flash */
	if (BSX.write_enable)
	{
		BSX_Set_Bypass_FlashIO(address, byte);
		BSX.write_enable = FALSE;
		return;
	}

	/* Flash Command Handling */
	
	/*Memory Pack Type 1 & 3 & 4 */
	BSX.flash_command <<= 8;
	BSX.flash_command |= byte;

	switch (BSX.flash_command & 0xFF)
	{
		case 0x00:
		case 0xFF:
			/*Reset to normal */
			BSX.flash_enable = FALSE;
			BSX.flash_bsr = FALSE;
			BSX.flash_csr = FALSE;
			BSX.flash_gsr = FALSE;
			BSX.read_enable = FALSE;
			BSX.write_enable = FALSE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0x10:
		case 0x40:
			/*Write Byte */
			BSX.flash_enable = FALSE;
			BSX.flash_bsr = FALSE;
			BSX.flash_csr = TRUE;
			BSX.flash_gsr = FALSE;
			BSX.read_enable = FALSE;
			BSX.write_enable = TRUE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0x50:
			/*Clear Status Register */
			BSX.flash_enable = FALSE;
			BSX.flash_bsr = FALSE;
			BSX.flash_csr = FALSE;
			BSX.flash_gsr = FALSE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0x70:
			/*Read CSR */
			BSX.flash_enable = FALSE;
			BSX.flash_bsr = FALSE;
			BSX.flash_csr = TRUE;
			BSX.flash_gsr = FALSE;
			BSX.read_enable = FALSE;
			BSX.write_enable = FALSE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0x71:
			/*Read Extended Status Registers (Page and Global) */
			BSX.flash_enable = FALSE;
			BSX.flash_bsr = TRUE;
			BSX.flash_csr = FALSE;
			BSX.flash_gsr = TRUE;
			BSX.read_enable = FALSE;
			BSX.write_enable = FALSE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0x75:
			/*Show Page Buffer / Vendor Info */
			BSX.flash_csr = FALSE;
			BSX.read_enable = TRUE;
			BSX.flash_cmd_done = TRUE;
			break;

		case 0xD0:
			/*DO COMMAND */
			switch (BSX.flash_command & 0xFFFF)
			{
				case 0x20D0: /*Block Erase */
				{
					uint32_t x;
					for (x = 0; x < 0x10000; x++) {
						/*BSX_Set_Bypass_FlashIO(((address & 0xFF0000) + x), 0xFF); */
						if (BSX.MMC[0x02])
							FlashROM[(address & 0x0F0000) + x] = 0xFF;
						else
							FlashROM[((address & 0x1E0000) >> 1) + x] = 0xFF;
					}
					break;
				}

				case 0xA7D0: /*Chip Erase (ONLY IN TYPE 1 AND 4) */
					if ((flashcard[6] & 0xF0) == 0x10 || (flashcard[6] & 0xF0) == 0x40)
					{
						uint32_t x;
						for (x = 0; x < FLASH_SIZE; x++) {
							/*BSX_Set_Bypass_FlashIO(x, 0xFF); */
							FlashROM[x] = 0xFF;
						}
					}
					break;

				case 0x38D0: /*Flashcart Reset */
					break;
			}
			break;
	}
}

static void S9xBSXSetStream1 (uint8_t count)
{
	char path[PATH_MAX + 32];
	long str1size;
	float QueueSize;

	if ((BSX.sat_stream1 != NULL))
		{ filestream_close(BSX.sat_stream1); BSX.sat_stream1 = NULL; } /* already open: close it */


	sprintf(path, "%s%sBSX%04X-%d.bin", S9xGetDirectory(SAT_DIR), SLASH_STR,
		(BSX.PPU[0x2188 - BSXPPUBASE] | (BSX.PPU[0x2189 - BSXPPUBASE] * 256)), count); /* BSXHHHH-DDD.bin */

	BSX.sat_stream1 = filestream_open(path, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
	if (BSX.sat_stream1 != NULL)
	{
		filestream_seek(BSX.sat_stream1, 0, RETRO_VFS_SEEK_POSITION_END);
		str1size = filestream_tell(BSX.sat_stream1);
		filestream_seek(BSX.sat_stream1, 0, RETRO_VFS_SEEK_POSITION_START);
		QueueSize = str1size / 22.;
		BSX.sat_stream1_queue = (uint16_t)(ceil(QueueSize));
		BSX.PPU[0x218D - BSXPPUBASE] = 0;
		BSX.sat_stream1_first = TRUE;
		BSX.sat_stream1_loaded = TRUE;
	}
	else
	{
		BSX.sat_stream1_loaded = FALSE;
	}
}

static void S9xBSXSetStream2 (uint8_t count)
{
	char path[PATH_MAX + 32];
	long str2size;
	float QueueSize;

	if ((BSX.sat_stream2 != NULL))
		{ filestream_close(BSX.sat_stream2); BSX.sat_stream2 = NULL; } /* already open: close it */



	sprintf(path, "%s%sBSX%04X-%d.bin", S9xGetDirectory(SAT_DIR), SLASH_STR,
		(BSX.PPU[0x218E - BSXPPUBASE] | (BSX.PPU[0x218F - BSXPPUBASE] * 256)), count); /* BSXHHHH-DDD.bin */

	BSX.sat_stream2 = filestream_open(path, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
	if (BSX.sat_stream2 != NULL)
	{
		filestream_seek(BSX.sat_stream2, 0, RETRO_VFS_SEEK_POSITION_END);
		str2size = filestream_tell(BSX.sat_stream2);
		filestream_seek(BSX.sat_stream2, 0, RETRO_VFS_SEEK_POSITION_START);
		QueueSize = str2size / 22.;
		BSX.sat_stream2_queue = (uint16_t)(ceil(QueueSize));
		BSX.PPU[0x2193 - BSXPPUBASE] = 0;
		BSX.sat_stream2_first = TRUE;
		BSX.sat_stream2_loaded = TRUE;
	}
	else
	{
		BSX.sat_stream2_loaded = FALSE;
	}
}

static uint8_t S9xBSXGetRTC (void)
{
	/*Get Time */
	time_t		t;
	struct tm	*tmr;

	time(&t);
	tmr = localtime(&t);

	BSX.test2192[0] = 0x00;
	BSX.test2192[1] = 0x00;
	BSX.test2192[2] = 0x00;
	BSX.test2192[3] = 0x00;
	BSX.test2192[4] = 0x10;
	BSX.test2192[5] = 0x01;
	BSX.test2192[6] = 0x01;
	BSX.test2192[7] = 0x00;
	BSX.test2192[8] = 0x00;
	BSX.test2192[9] = 0x00;
	BSX.test2192[10] = BSX_RTC.seconds = tmr->tm_sec;
	BSX.test2192[11] = BSX_RTC.minutes = tmr->tm_min;
	BSX.test2192[12] = BSX_RTC.hours = tmr->tm_hour;
	BSX.test2192[13] = BSX_RTC.dayweek = (tmr->tm_wday) + 1;
	BSX.test2192[14] = BSX_RTC.day = tmr->tm_mday;
	BSX.test2192[15] = BSX_RTC.month = (tmr->tm_mon) + 1;
	BSX_RTC.year = tmr->tm_year + 1900;
	BSX.test2192[16] = (BSX_RTC.year) & 0xFF;
	BSX.test2192[17] = (BSX_RTC.year) >> 8;

	t = BSX.test2192[BSX.out_index++];

	/* Full 32-byte channel-0 cycle per the MiSTer BSX RTL capture. */
	if (BSX.out_index > 31)
		BSX.out_index = 0;

	return t;
}

uint8_t S9xGetBSXPPU (uint16_t address)
{
	uint8_t	t;

	/* known read registers */
	switch (address)
	{
		/*Stream 1 */
		/* Logical Channel 1 + Data Structure (R/W) */
		case 0x2188:
			t = BSX.PPU[0x2188 - BSXPPUBASE];
			break;

		/* Logical Channel 2 (R/W) [6bit] */
		case 0x2189:
			t = BSX.PPU[0x2189 - BSXPPUBASE];
			break;

		/* Prefix Count (R) */
		case 0x218A:
			if (!BSX.sat_pf_latch1_enable || !BSX.sat_dt_latch1_enable)
			{
				t = 0;
				break;
			}

			if (BSX.PPU[0x2188 - BSXPPUBASE] == 0 && BSX.PPU[0x2189 - BSXPPUBASE] == 0)
			{
				t = 1;
				break;
			}

			if (BSX.sat_stream1_queue <= 0)
			{
				BSX.sat_stream1_count++;
				S9xBSXSetStream1(BSX.sat_stream1_count - 1);
			}

			if (!BSX.sat_stream1_loaded && (BSX.sat_stream1_count - 1) > 0)
			{
				BSX.sat_stream1_count = 1;
				S9xBSXSetStream1(BSX.sat_stream1_count - 1);
			}

			if (BSX.sat_stream1_loaded)
			{
				/*Lock at 0x7F for bigger packets */
				if (BSX.sat_stream1_queue >= 128)
					BSX.PPU[0x218A - BSXPPUBASE] = 0x7F;
				else
					BSX.PPU[0x218A - BSXPPUBASE] = BSX.sat_stream1_queue;
				t = BSX.PPU[0x218A - BSXPPUBASE];
			}
			else
				t = 0;
			break;

		/* Prefix Latch (R/W) */
		case 0x218B:
			if (BSX.sat_pf_latch1_enable)
			{
				if (BSX.PPU[0x2188 - BSXPPUBASE] == 0 && BSX.PPU[0x2189 - BSXPPUBASE] == 0)
				{
					BSX.PPU[0x218B - BSXPPUBASE] = 0x90;
				}

				if (BSX.sat_stream1_loaded)
				{
					uint8_t temp = 0;
					if (BSX.sat_stream1_first)
					{
						/* First packet */
						temp |= 0x10;
						BSX.sat_stream1_first = FALSE;
					}

					BSX.sat_stream1_queue--;

					if (BSX.sat_stream1_queue == 0)
					{
						/*Last packet */
						temp |= 0x80;
					}

					BSX.PPU[0x218B - BSXPPUBASE] = temp;
				}

				BSX.PPU[0x218D - BSXPPUBASE] |= BSX.PPU[0x218B - BSXPPUBASE];
				t = BSX.PPU[0x218B - BSXPPUBASE];
			}
			else
			{
				t = 0;
			}
			break;

		/* Data Latch (R/W) */
		case 0x218C:
			if (BSX.sat_dt_latch1_enable)
			{
				if (BSX.PPU[0x2188 - BSXPPUBASE] == 0 && BSX.PPU[0x2189 - BSXPPUBASE] == 0)
				{
					BSX.PPU[0x218C - BSXPPUBASE] = S9xBSXGetRTC();
				}
				else if (BSX.sat_stream1_loaded)
				{
					if (filestream_eof(BSX.sat_stream1))
						BSX.PPU[0x218C - BSXPPUBASE] = 0xFF;
					else
						BSX.PPU[0x218C - BSXPPUBASE] = bsx_stream_get(BSX.sat_stream1);
				}
				t = BSX.PPU[0x218C - BSXPPUBASE];
			}
			else
			{
				t = 0;
			}
			break;

		/* OR gate (R) */
		case 0x218D:
			t = BSX.PPU[0x218D - BSXPPUBASE];
			BSX.PPU[0x218D - BSXPPUBASE] = 0;
			break;

		/*Stream 2 */
		/* Logical Channel 1 + Data Structure (R/W) */
		case 0x218E:
			t = BSX.PPU[0x218E - BSXPPUBASE];
			break;

		/* Logical Channel 2 (R/W) [6bit] */
		case 0x218F:
			t = BSX.PPU[0x218F - BSXPPUBASE];
			break;

		/* Prefix Count (R) */
		case 0x2190:
			if (!BSX.sat_pf_latch2_enable || !BSX.sat_dt_latch2_enable)
			{
				t = 0;
				break;
			}

			if (BSX.PPU[0x218E - BSXPPUBASE] == 0 && BSX.PPU[0x218F - BSXPPUBASE] == 0)
			{
				t = 1;
				break;
			}

			if (BSX.sat_stream2_queue <= 0)
			{
				BSX.sat_stream2_count++;
				S9xBSXSetStream2(BSX.sat_stream2_count - 1);
			}

			if (!BSX.sat_stream2_loaded && (BSX.sat_stream2_count - 1) > 0)
			{
				BSX.sat_stream2_count = 1;
				S9xBSXSetStream2(BSX.sat_stream2_count - 1);
			}

			if (BSX.sat_stream2_loaded)
			{
				if (BSX.sat_stream2_queue >= 128)
					BSX.PPU[0x2190 - BSXPPUBASE] = 0x7F;
				else
					BSX.PPU[0x2190 - BSXPPUBASE] = BSX.sat_stream2_queue;
				t = BSX.PPU[0x2190 - BSXPPUBASE];
			}
			else
				t = 0;
			break;

		/* Prefix Latch (R/W) */
		case 0x2191:
			if (BSX.sat_pf_latch2_enable)
			{
				if (BSX.PPU[0x218E - BSXPPUBASE] == 0 && BSX.PPU[0x218F - BSXPPUBASE] == 0)
				{
					BSX.PPU[0x2191 - BSXPPUBASE] = 0x90;
				}

				if (BSX.sat_stream2_loaded)
				{
					uint8_t temp = 0;
					if (BSX.sat_stream2_first)
					{
						/* First packet */
						temp |= 0x10;
						BSX.sat_stream2_first = FALSE;
					}

					BSX.sat_stream2_queue--;

					if (BSX.sat_stream2_queue == 0)
					{
						/*Last packet */
						temp |= 0x80;
					}

					BSX.PPU[0x2191 - BSXPPUBASE] = temp;
				}

				BSX.PPU[0x2193 - BSXPPUBASE] |= BSX.PPU[0x2191 - BSXPPUBASE];
				t = BSX.PPU[0x2191 - BSXPPUBASE];
			}
			else
			{
				t = 0;
			}
			break;

		/* Data Latch (R/W) */
		case 0x2192:
			if (BSX.sat_dt_latch2_enable)
			{
				if (BSX.PPU[0x218E - BSXPPUBASE] == 0 && BSX.PPU[0x218F - BSXPPUBASE] == 0)
				{
					BSX.PPU[0x2192 - BSXPPUBASE] = S9xBSXGetRTC();
				}
				else if (BSX.sat_stream2_loaded)
				{
					if (filestream_eof(BSX.sat_stream2))
						BSX.PPU[0x2192 - BSXPPUBASE] = 0xFF;
					else
						BSX.PPU[0x2192 - BSXPPUBASE] = bsx_stream_get(BSX.sat_stream2);
				}
				t = BSX.PPU[0x2192 - BSXPPUBASE];
			}
			else
			{
				t = 0;
			}
			break;

		/* OR gate (R) */
		case 0x2193:
			t = BSX.PPU[0x2193 - BSXPPUBASE];
			BSX.PPU[0x2193 - BSXPPUBASE] = 0;
			break;

		/*Other */
		/* Satellaview LED / Stream Enable (R/W) [4bit] */
		case 0x2194:
			t = BSX.PPU[0x2194 - BSXPPUBASE];
			break;

		/* Unknown */
		case 0x2195:
			t = BSX.PPU[0x2195 - BSXPPUBASE];
			break;

		/* Satellaview Status (R) */
		case 0x2196:
			t = BSX.PPU[0x2196 - BSXPPUBASE];
			break;

		/* Soundlink Settings (R/W) */
		case 0x2197:
			t = BSX.PPU[0x2197 - BSXPPUBASE];
			break;

		/* Serial I/O - Serial Number (R/W) */
		case 0x2198:
			t = BSX.PPU[0x2198 - BSXPPUBASE];
			break;

		/* Serial I/O - Unknown (R/W) */
		case 0x2199:
			t = BSX.PPU[0x2199 - BSXPPUBASE];
			break;

		default:
			t = OpenBus;
			break;
	}

	return (t);
}

void S9xSetBSXPPU (uint8_t byte, uint16_t address)
{
	/* known write registers */
	switch (address)
	{
		/*Stream 1 */
		/* Logical Channel 1 + Data Structure (R/W) */
		case 0x2188:
			if (BSX.PPU[0x2188 - BSXPPUBASE] == byte)
			{
				BSX.sat_stream1_count = 0;
			}
			BSX.PPU[0x2188 - BSXPPUBASE] = byte;
			break;

		/* Logical Channel 2 (R/W) [6bit] */
		case 0x2189:
			if (BSX.PPU[0x2188 - BSXPPUBASE] == (byte & 0x3F))
			{
				BSX.sat_stream1_count = 0;
			}
			BSX.PPU[0x2189 - BSXPPUBASE] = byte & 0x3F;
			break;

		/* Prefix Latch (R/W) */
		case 0x218B:
			BSX.sat_pf_latch1_enable = (byte != 0);
			break;

		/* Data Latch (R/W) */
		case 0x218C:
			if (BSX.PPU[0x2188 - BSXPPUBASE] == 0 && BSX.PPU[0x2189 - BSXPPUBASE] == 0)
			{
				BSX.out_index = 0;
			}
			BSX.sat_dt_latch1_enable = (byte != 0);
			break;

		/*Stream 2 */
		/* Logical Channel 1 + Data Structure (R/W) */
		case 0x218E:
			if (BSX.PPU[0x218E - BSXPPUBASE] == byte)
			{
				BSX.sat_stream2_count = 0;
			}
			BSX.PPU[0x218E - BSXPPUBASE] = byte;
			break;

		/* Logical Channel 2 (R/W) [6bit] */
		case 0x218F:
			if (BSX.PPU[0x218F - BSXPPUBASE] == (byte & 0x3F))
			{
				BSX.sat_stream2_count = 0;
			}
			BSX.PPU[0x218F - BSXPPUBASE] = byte & 0x3F;
			break;

		/* Prefix Latch (R/W) */
		case 0x2191:
			BSX.sat_pf_latch2_enable = (byte != 0);
			break;

		/* Data Latch (R/W) */
		case 0x2192:
			if (BSX.PPU[0x218E - BSXPPUBASE] == 0 && BSX.PPU[0x218F - BSXPPUBASE] == 0)
			{
				BSX.out_index = 0;
			}
			BSX.sat_dt_latch2_enable = (byte != 0);
			break;

		/*Other */
		/* Satellaview LED / Stream Enable (R/W) [4bit] */
		case 0x2194:
			BSX.PPU[0x2194 - BSXPPUBASE] = byte & 0x0F;
			break;

		/* Soundlink Settings (R/W) */
		case 0x2197:
			BSX.PPU[0x2197 - BSXPPUBASE] = byte;
			break;
	}
}

uint8_t * S9xGetBasePointerBSX (uint32_t address)
{
	(void) address;
	return (MapROM);
}

static uint8_t BSX_LoadBIOS (void)
{
	RFILE *fp;
	uint8_t	r = FALSE;

	char name[PATH_MAX + 16];

	sprintf(name, "%s%sBS-X.bin", S9xGetDirectory(BIOS_DIR), SLASH_STR);
	fp = filestream_open(name, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
	if (!fp)
	{
		sprintf(name, "%s%sBS-X.bios", S9xGetDirectory(BIOS_DIR), SLASH_STR);
		fp = filestream_open(name, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);
	}

	if (fp)
	{
		int64_t	size;

		size = filestream_read(fp, (void *) BIOSROM, BIOS_SIZE);
		filestream_close(fp);
		if (size == BIOS_SIZE)
			r = TRUE;
	}

#ifdef BSX_DEBUG
	if (r)
		printf("BS: BIOS found.\n");
	else
		printf("BS: BIOS not found!\n");
#endif

	return (r);
}

static uint8_t is_BSX_BIOS (const uint8_t *data, uint32_t size)
{
	if (size == BIOS_SIZE && strncmp((char *) (data + 0x7FC0), "Satellaview BS-X     ", 21) == 0)
		return (TRUE);
	else
		return (FALSE);
}

void S9xInitBSX (void)
{
	int r1, r2;
	uint8_t *header;
#ifdef BSX_DEBUG
	int dbg_i;
#endif
	Settings.BS = FALSE;

	if (is_BSX_BIOS(ROM,Memory.CalculatedSize))
	{
		/* BS-X itself */

		Settings.BS = TRUE;
		Settings.BSXItself = TRUE;

		Memory.LoROM = TRUE, Memory.HiROM = FALSE;

		memmove(BIOSROM, ROM, BIOS_SIZE);

		FlashMode = FALSE;
		FlashSize = FLASH_SIZE;

		BSX.bootup = TRUE;
	}
	else
	{
		Settings.BSXItself = FALSE;

		r1 = (is_bsx(ROM + 0x7FC0) == 1);
		r2 = (is_bsx(ROM + 0xFFC0) == 1);
		Settings.BS = (r1 | r2) ? TRUE : FALSE;

		if (Settings.BS)
		{
			/* BS games */

			Memory.LoROM = r1 ? TRUE : FALSE, Memory.HiROM = r2 ? TRUE : FALSE;

			header = r1 ? ROM + 0x7FC0 : ROM + 0xFFC0;

			FlashMode = (header[0x18] & 0xEF) == 0x20 ? FALSE : TRUE;
			FlashSize = FLASH_SIZE;

			/* Fix Block Allocation Flags */
			/* (for games that don't have it setup properly, */
			/* for exemple when taken seperately from the upper memory of the Memory Pack, */
			/* else the game will error out on BS-X) */
			for (; (((header[0x10] & 1) == 0) && header[0x10] != 0); (header[0x10] >>= 1));

#ifdef BSX_DEBUG
			for (dbg_i = 0; dbg_i <= 0x1F; dbg_i++)
				printf("BS: ROM Header %02X: %02X\n", dbg_i, header[dbg_i]);
			printf("BS: FlashMode: %d, FlashSize: %x\n", FlashMode, FlashSize);
#endif

			BSX.bootup = Settings.BSXBootup;

			if (!BSX_LoadBIOS() && !is_BSX_BIOS(BIOSROM,BIOS_SIZE))
			{
				BSX.bootup = FALSE;
				memset(BIOSROM, 0, BIOS_SIZE);
			}
		}
	}

	if (Settings.BS)
	{
		MapROM = NULL;
		FlashROM = ROM;
		/*
		time_t		t;
		struct tm	*tmr;

		time(&t);
		tmr = localtime(&t);

		BSX_RTC.ticks = 0;
		memcpy(BSX.test2192, init2192, sizeof(init2192));
		BSX.test2192[10] = BSX_RTC.seconds = tmr->tm_sec;
		BSX.test2192[11] = BSX_RTC.minutes = tmr->tm_min;
		BSX.test2192[12] = BSX_RTC.hours   = tmr->tm_hour;
#ifdef BSX_DEBUG
		printf("BS: Current Time: %02d:%02d:%02d\n",  BSX_RTC.hours, BSX_RTC.minutes, BSX_RTC.seconds);
#endif
		*/
		SNESGameFixes.SRAMInitialValue = 0x00;
	}
}

void S9xResetBSX (void)
{
	/* Initialise the ares-accurate BS Memory flash chip over the same image */
	/* the legacy mapping uses. The BS-X base unit hosts a writable Flash */
	/* cassette; dumped BS-game ROM cassettes are read-only MaskROM and read */
	/* straight through the chip untouched. */
	S9xBSFlashInit(FlashROM, FlashSize,
		(Settings.BSXItself || FlashMode) ? FALSE : TRUE);

	if (Settings.BSXItself)
		memset(ROM, 0, FLASH_SIZE);

	memset(BSX.PPU, 0, sizeof(BSX.PPU));
	memset(BSX.MMC, 0, sizeof(BSX.MMC));
	memset(BSX.prevMMC, 0, sizeof(BSX.prevMMC));

	BSX.dirty         = FALSE;
	BSX.dirty2        = FALSE;
	BSX.flash_enable  = FALSE;
	BSX.write_enable  = FALSE;
	BSX.read_enable   = FALSE;
	BSX.flash_command = 0;
	BSX.old_write     = 0;
	BSX.new_write     = 0;

	BSX.out_index = 0;
	memset(BSX.output, 0, sizeof(BSX.output));

	/* starting from the bios */
	BSX.MMC[0x02] = BSX.MMC[0x03] = BSX.MMC[0x05] = BSX.MMC[0x06] = 0x80;
	BSX.MMC[0x09] = BSX.MMC[0x0B] = 0x80;

	BSX.MMC[0x07] = BSX.MMC[0x08] = 0x80;
	BSX.MMC[0x0E] = 0x80;

	/* default register values */
	BSX.PPU[0x2196 - BSXPPUBASE] = 0x10;
	BSX.PPU[0x2197 - BSXPPUBASE] = 0x80;

	/* stream reset */
	BSX.sat_pf_latch1_enable = BSX.sat_dt_latch1_enable = FALSE;
	BSX.sat_pf_latch2_enable = BSX.sat_dt_latch2_enable = FALSE;

	BSX.sat_stream1_loaded = BSX.sat_stream2_loaded = FALSE;
	BSX.sat_stream1_first = BSX.sat_stream2_first = FALSE;
	BSX.sat_stream1_count = BSX.sat_stream2_count = 0;

    if (BSX.sat_stream1 != NULL)
    {
        filestream_close(BSX.sat_stream1);
        BSX.sat_stream1 = NULL;
    }
    if (BSX.sat_stream2 != NULL)
    {
        filestream_close(BSX.sat_stream2);
        BSX.sat_stream2 = NULL;
    }

    if (Settings.BS)
	    BSX_Map();
}

void S9xBSXPostLoadState (void)
{
	uint8_t	temp[16];
	uint8_t	pd1, pd2;

	pd1 = BSX.dirty;
	pd2 = BSX.dirty2;
	memcpy(temp, BSX.MMC, sizeof(BSX.MMC));

	memcpy(BSX.MMC, BSX.prevMMC, sizeof(BSX.MMC));
	BSX_Map();

	memcpy(BSX.MMC, temp, sizeof(BSX.MMC));
	BSX.dirty  = pd1;
	BSX.dirty2 = pd2;
}

static uint8_t valid_normal_bank (unsigned char bankbyte)
{
	switch (bankbyte)
	{
		case 32: case 33: case 48: case 49:
			return (TRUE);
			break;
	}

	return (FALSE);
}

static int is_bsx (unsigned char *p)
{
	if ((p[26] == 0x33 || p[26] == 0xFF) && (!p[21] || (p[21] & 131) == 128) && valid_normal_bank(p[24]))
	{
		unsigned char	m = p[22];

		if (!m && !p[23])
			return (2);

		if ((m == 0xFF && p[23] == 0xFF) || (!(m & 0xF) && ((m >> 4) - 1 < 12)))
			return (1);
	}

	return (0);
}
