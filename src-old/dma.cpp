/***********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002  Gary Henderson (gary.henderson@ntlworld.com),
                             Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2002 - 2004  Matthew Kendora

  (c) Copyright 2002 - 2005  Peter Bortas (peter@bortas.org)

  (c) Copyright 2004 - 2005  Joel Yliluoma (http://iki.fi/bisqwit/)

  (c) Copyright 2001 - 2006  John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2006  funkyass (funkyass@spam.shaw.ca),
                             Kris Bleakley (codeviolation@hotmail.com)

  (c) Copyright 2002 - 2010  Brad Jorsch (anomie@users.sourceforge.net),
                             Nach (n-a-c-h@users.sourceforge.net),
                             zones (kasumitokoduck@yahoo.com)

  (c) Copyright 2006 - 2007  nitsuja

  (c) Copyright 2009 - 2010  BearOso,
                             OV2


  BS-X C emulator code
  (c) Copyright 2005 - 2006  Dreamer Nom,
                             zones

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003  _Demo_ (_demo_@zsnes.com),
                             Nach,
                             zsKnight (zsknight@zsnes.com)

  C4 C++ code
  (c) Copyright 2003 - 2006  Brad Jorsch,
                             Nach

  DSP-1 emulator code
  (c) Copyright 1998 - 2006  _Demo_,
                             Andreas Naive (andreasnaive@gmail.com),
                             Gary Henderson,
                             Ivar (ivar@snes9x.com),
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora,
                             Nach,
                             neviksti (neviksti@hotmail.com)

  DSP-2 emulator code
  (c) Copyright 2003         John Weidman,
                             Kris Bleakley,
                             Lord Nightmare (lord_nightmare@users.sourceforge.net),
                             Matthew Kendora,
                             neviksti

  DSP-3 emulator code
  (c) Copyright 2003 - 2006  John Weidman,
                             Kris Bleakley,
                             Lancer,
                             z80 gaiden

  DSP-4 emulator code
  (c) Copyright 2004 - 2006  Dreamer Nom,
                             John Weidman,
                             Kris Bleakley,
                             Nach,
                             z80 gaiden

  OBC1 emulator code
  (c) Copyright 2001 - 2004  zsKnight,
                             pagefault (pagefault@zsnes.com),
                             Kris Bleakley
                             Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code used in 1.39-1.51
  (c) Copyright 2002         Matthew Kendora with research by
                             zsKnight,
                             John Weidman,
                             Dark Force

  SPC7110 and RTC C++ emulator code used in 1.52+
  (c) Copyright 2009         byuu,
                             neviksti

  S-DD1 C emulator code
  (c) Copyright 2003         Brad Jorsch with research by
                             Andreas Naive,
                             John Weidman

  S-RTC C emulator code
  (c) Copyright 2001 - 2006  byuu,
                             John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003         Feather,
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003  _Demo_,
                             pagefault,
                             zsKnight

  Super FX C emulator code
  (c) Copyright 1997 - 1999  Ivar,
                             Gary Henderson,
                             John Weidman

  Sound emulator code used in 1.5-1.51
  (c) Copyright 1998 - 2003  Brad Martin
  (c) Copyright 1998 - 2006  Charles Bilyue'

  Sound emulator code used in 1.52+
  (c) Copyright 2004 - 2007  Shay Green (gblargg@gmail.com)

  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004  Marcus Comstedt (marcus@mc.pp.se)

  2xSaI filter
  (c) Copyright 1999 - 2001  Derek Liauw Kie Fa

  HQ2x, HQ3x, HQ4x filters
  (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)

  NTSC filter
  (c) Copyright 2006 - 2007  Shay Green

  GTK+ GUI code
  (c) Copyright 2004 - 2010  BearOso

  Win32 GUI code
  (c) Copyright 2003 - 2006  blip,
                             funkyass,
                             Matthew Kendora,
                             Nach,
                             nitsuja
  (c) Copyright 2009 - 2010  OV2

  Mac OS GUI code
  (c) Copyright 1998 - 2001  John Stiles
  (c) Copyright 2001 - 2010  zones


  Specific ports contains the works of other authors. See headers in
  individual files.


  Snes9x homepage: http://www.snes9x.com/

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without
  fee, providing that this license information and copyright notice appear
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
 ***********************************************************************************/


#include "snes9x.h"
#include "memmap.h"
#include "dma.h"
#include "apu/apu.h"
#include "sdd1emu.h"
#include "spc7110emu.h"

#define ADD_CYCLES(n)	CPU.Cycles += (n)

extern uint8	*HDMAMemPointers[8];
extern int		HDMA_ModeByteCounts[8];
extern SPC7110	s7emu;

static uint8	sdd1_decode_buffer[0x10000];

static inline bool8 addCyclesInDMA (uint8);
static inline bool8 addCyclesInDMA_SuperFX (uint8);
static inline bool8 HDMAReadLineCount (int);

static inline bool8 addCyclesInDMA_SuperFX(uint8 dma_channel)
{
	// Add 8 cycles per byte, sync APU, and do HC related events.
	// If HDMA was done in S9xDoHEventProcessing(), check if it used the same channel as DMA.
	ADD_CYCLES(SLOW_ONE_CYCLE);
	while (CPU.Cycles >= CPU.NextEvent)
		S9xDoHEventProcessing_SuperFX();

	if (CPU.HDMARanInDMA & (1 << dma_channel))
	{
		CPU.HDMARanInDMA = 0;
		// If HDMA triggers in the middle of DMA transfer and it uses the same channel,
		// it kills the DMA transfer immediately. $43x2 and $43x5 stop updating.
		return (FALSE);
	}

	CPU.HDMARanInDMA = 0;
	return (TRUE);
}

static inline bool8 addCyclesInDMA (uint8 dma_channel)
{
	// Add 8 cycles per byte, sync APU, and do HC related events.
	// If HDMA was done in S9xDoHEventProcessing(), check if it used the same channel as DMA.
	ADD_CYCLES(SLOW_ONE_CYCLE);
	while (CPU.Cycles >= CPU.NextEvent)
		S9xDoHEventProcessing();

	if (CPU.HDMARanInDMA & (1 << dma_channel))
	{
		CPU.HDMARanInDMA = 0;
		// If HDMA triggers in the middle of DMA transfer and it uses the same channel,
		// it kills the DMA transfer immediately. $43x2 and $43x5 stop updating.
		return (FALSE);
	}

	CPU.HDMARanInDMA = 0;
	return (TRUE);
}

bool8 S9xDoDMA_SuperFX (uint8 Channel)
{
	CPU.InDMA = TRUE;
    CPU.InDMAorHDMA = TRUE;
	CPU.CurrentDMAorHDMAChannel = Channel;

    SDMA	*d = &DMA[Channel];

	// Check invalid DMA first
	if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80 && !d->ReverseTransfer)
	{
		// Attempting a DMA from WRAM to $2180 will not work, WRAM will not be written.
		// Attempting a DMA from $2180 to WRAM will similarly not work,
		// the value written is (initially) the OpenBus value.
		// In either case, the address in $2181-3 is not incremented.

		// Does an invalid DMA actually take time?
		// I'd say yes, since 'invalid' is probably just the WRAM chip
		// not being able to read and write itself at the same time
		// And no, PPU.WRAM should not be updated.

		int32	c = d->TransferBytes;
		// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
		if (c == 0)
			c = 0x10000;

		// 8 cycles per channel
		ADD_CYCLES(SLOW_ONE_CYCLE);
		// 8 cycles per byte
		while (c)
		{
			d->TransferBytes--;
			d->AAddress++;
			c--;
			if (!addCyclesInDMA_SuperFX(Channel))
			{
				CPU.InDMA = FALSE;
				CPU.InDMAorHDMA = FALSE;
				CPU.CurrentDMAorHDMAChannel = -1;
				return (FALSE);
			}
		}


		CPU.InDMA = FALSE;
		CPU.InDMAorHDMA = FALSE;
		CPU.CurrentDMAorHDMAChannel = -1;
		return (TRUE);
	}

	// Prepare for accessing $2118-2119
	switch (d->BAddress)
	{
		case 0x18:
		case 0x19:
         #ifndef NO_FRAMESKIP
			if (IPPU.RenderThisFrame)
         #endif
	         if (IPPU.PreviousLine != IPPU.CurrentLine)
		         S9xUpdateScreen_SuperFX();
			break;
	}

	int32	inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);
	int32	count = d->TransferBytes;
	// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
	if (count == 0)
		count = 0x10000;

	// Prepare for custom chip DMA

	// S-DD1

/*
	uint8	*in_sdd1_dma = NULL;

	if (Settings.SDD1)
	{
		if (d->AAddressFixed && Memory.FillRAM[0x4801] > 0)
		{
			// XXX: Should probably verify that we're DMAing from ROM?
			// And somewhere we should make sure we're not running across a mapping boundary too.
			// Hacky support for pre-decompressed S-DD1 data
			inc = !d->AAddressDecrement ? 1 : -1;

			uint8	*in_ptr = S9xGetBasePointer_SuperFX(((d->ABank << 16) | d->AAddress));
			if (in_ptr)
			{
				in_ptr += d->AAddress;
				SDD1_decompress(sdd1_decode_buffer, in_ptr, d->TransferBytes);
			}

			in_sdd1_dma = sdd1_decode_buffer;
		}

		Memory.FillRAM[0x4801] = 0;
	}

	// SPC7110

	uint8	*spc7110_dma = NULL;

	if (Settings.SPC7110)
	{
		if (d->AAddress == 0x4800 || d->ABank == 0x50)
		{
			spc7110_dma = new uint8[d->TransferBytes];
			for (int i = 0; i < d->TransferBytes; i++)
				spc7110_dma[i] = s7emu.decomp.read();

			int32	icount = s7emu.r4809 | (s7emu.r480a << 8);
			icount -= d->TransferBytes;
			s7emu.r4809 =  icount & 0x00ff;
			s7emu.r480a = (icount & 0xff00) >> 8;

			inc = 1;
			d->AAddress -= count;
		}
	}

	// SA-1

	bool8	in_sa1_dma = FALSE;

	if (Settings.SA1)
	{
		if (SA1.in_char_dma && d->BAddress == 0x18 && (d->ABank & 0xf0) == 0x40)
		{
			// Perform packed bitmap to PPU character format conversion on the data
			// before transmitting it to V-RAM via-DMA.
			int32	num_chars = 1 << ((Memory.FillRAM[0x2231] >> 2) & 7);
			int32	depth = (Memory.FillRAM[0x2231] & 3) == 0 ? 8 : (Memory.FillRAM[0x2231] & 3) == 1 ? 4 : 2;
			int32	bytes_per_char = 8 * depth;
			int32	bytes_per_line = depth * num_chars;
			int32	char_line_bytes = bytes_per_char * num_chars;
			uint32	addr = (d->AAddress / char_line_bytes) * char_line_bytes;

			uint8	*base = S9xGetBasePointer_SuperFX((d->ABank << 16) + addr);
			if (!base)
			{
				//sprintf(String, "SA-1: DMA from non-block address $%02X:%04X", d->ABank, addr);
				//S9xMessage(S9X_WARNING, S9X_DMA_TRACE, String);
				base = Memory.ROM;
			}

			base += addr;

			uint8	*buffer = &Memory.ROM[CMemory::MAX_ROM_SIZE - 0x10000];
			uint8	*p = buffer;
			uint32	inc_sa1 = char_line_bytes - (d->AAddress % char_line_bytes);
			uint32	char_count = inc_sa1 / bytes_per_char;

			in_sa1_dma = TRUE;

		#if 0
			printf("SA-1 DMA: %08x,", base);
			printf("depth = %d, count = %d, bytes_per_char = %d, bytes_per_line = %d, num_chars = %d, char_line_bytes = %d\n",
				depth, count, bytes_per_char, bytes_per_line, num_chars, char_line_bytes);
		#endif

			switch (depth)
			{
				case 2:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 1);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 2)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 2; b++)
								{
									uint8	r = *(q + b);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 0) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 1) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 2) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 3) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 4) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 5) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 6) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}
						}
					}

					break;

				case 4:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 2);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 4)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 4; b++)
								{
									uint8	r = *(q + b);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 4) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 5) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 6) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}

							p += 32 - 16;
						}
					}

					break;

				case 8:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 3);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 8)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 8; b++)
								{
									uint8	r = *(q + b);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
									*(p + 32) = (*(p + 32) << 1) | ((r >> 4) & 1);
									*(p + 33) = (*(p + 33) << 1) | ((r >> 5) & 1);
									*(p + 48) = (*(p + 48) << 1) | ((r >> 6) & 1);
									*(p + 49) = (*(p + 49) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}

							p += 64 - 16;
						}
					}

					break;
			}
		}
	}
*/


	// Do Transfer

	uint8	Work;

	// 8 cycles per channel
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!d->ReverseTransfer)
    {
		// CPU -> PPU
		int32	b = 0;
		uint16	p = d->AAddress;
		uint8	*base = S9xGetBasePointer_SuperFX((d->ABank << 16) + d->AAddress);
		bool8	inWRAM_DMA;

		int32	rem = count;
		// Transfer per block if d->AAdressFixed is FALSE
		count = d->AAddressFixed ? rem : (d->AAddressDecrement ? ((p & MEMMAP_MASK) + 1) : (MEMMAP_BLOCK_SIZE - (p & MEMMAP_MASK)));

		// Settings for custom chip DMA
/*
		if (in_sa1_dma)
		{
			base = &Memory.ROM[CMemory::MAX_ROM_SIZE - 0x10000];
			p = 0;
			count = rem;
		}
		else
		if (in_sdd1_dma)
		{
			base = in_sdd1_dma;
			p = 0;
			count = rem;
		}
		else
		if (spc7110_dma)
		{
			base = spc7110_dma;
			p = 0;
			count = rem;
		}
*/

#if 0
		inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
			(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
#endif
		inWRAM_DMA = (d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000));

		// 8 cycles per byte
      #undef UPDATE_COUNTERS
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			p += inc; \
			if (!addCyclesInDMA_SuperFX(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		while (1)
		{
			if (count > rem)
				count = rem;
			rem -= count;

			CPU.InWRAMDMAorHDMA = inWRAM_DMA;

			if (!base)
			{
				// DMA SLOW PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					do
					{
						Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
						S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
					} while (--count > 0);
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					// This is a variation on Duff's Device. It is legal C/C++.
					switch (b)
					{
						default:
						while (count > 1)
						{
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							count--;

						case 1:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							count--;
						}
					}

					if (count == 1)
					{
						Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
						S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
						b = 1;
					}
					else
						b = 0;
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte_SuperFX((d->ABank << 16) + p);
							S9xSetPPU_SuperFX(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}
			else
			{
				// DMA FAST PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					switch (d->BAddress)
					{
						case 0x04: // OAMDATA
							do
							{
								Work = *(base + p);
	if (PPU.OAMAddr & 0x100)
	{
		int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
		if (Work != PPU.OAMData[addr])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = Work;
			IPPU.OBJChanged = TRUE;

			// X position high bit, and sprite size (x4)
			struct SOBJ *pObj = &PPU.OBJ[((addr & 0x1f) << 2)];
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 0) & 1];
			pObj++->Size = Work & 2;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 2) & 1];
			pObj++->Size = Work & 8;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 4) & 1];
			pObj++->Size = Work & 32;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 6) & 1];
			pObj->Size = Work & 128;
		}

		PPU.OAMFlip ^= 1;
		if (!(PPU.OAMFlip & 1))
		{
			++PPU.OAMAddr;
			PPU.OAMAddr &= 0x1ff;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			}
		}
		else
		{
			if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
				IPPU.OBJChanged = TRUE;
		}
	}
	else
	if (!(PPU.OAMFlip & 1))
	{
		PPU.OAMWriteRegister &= 0xff00;
		PPU.OAMWriteRegister |= Work;
		PPU.OAMFlip |= 1;
		if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
			IPPU.OBJChanged = TRUE;
	}
	else
	{
		PPU.OAMWriteRegister &= 0x00ff;
		uint8 lowbyte = (uint8) (PPU.OAMWriteRegister);
		uint8 highbyte = Work;
		PPU.OAMWriteRegister |= Work << 8;

		int addr = (PPU.OAMAddr << 1);
		if (lowbyte != PPU.OAMData[addr] || highbyte != PPU.OAMData[addr + 1])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = lowbyte;
			PPU.OAMData[addr + 1] = highbyte;
			IPPU.OBJChanged = TRUE;
			if (addr & 2)
			{
				// Tile
				PPU.OBJ[addr = PPU.OAMAddr >> 1].Name = PPU.OAMWriteRegister & 0x1ff;
				// priority, h and v flip.
				PPU.OBJ[addr].Palette  = (highbyte >> 1) & 7;
				PPU.OBJ[addr].Priority = (highbyte >> 4) & 3;
				PPU.OBJ[addr].HFlip    = (highbyte >> 6) & 1;
				PPU.OBJ[addr].VFlip    = (highbyte >> 7) & 1;
			}
			else
			{
				// X position (low)
				PPU.OBJ[addr = PPU.OAMAddr >> 1].HPos &= 0xff00;
				PPU.OBJ[addr].HPos |= lowbyte;
				// Sprite Y position
				PPU.OBJ[addr].VPos = highbyte;
			}
		}

		PPU.OAMFlip &= ~1;
		++PPU.OAMAddr;
		if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
		{
			PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
			IPPU.OBJChanged = TRUE;
		}
	}
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x18: // VMDATAL
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
                           //Register 2118 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (!PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x19: // VMDATAH
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x22: // CGDATA
							do
							{
								Work = *(base + p);

	if (PPU.CGFLIP)
	{
		if ((Work & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x00ff;
			PPU.CGDATA[PPU.CGADD] |= (Work & 0x7f) << 8;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Blue[PPU.CGADD] = IPPU.XB[(Work >> 2) & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}

		PPU.CGADD++;
	}
	else
	{
		if (Work != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x7f00;
			PPU.CGDATA[PPU.CGADD] |= Work;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Red[PPU.CGADD] = IPPU.XB[Work & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}
	}

	PPU.CGFLIP ^= 1;


								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x80: // WMDATA
							if (!CPU.InWRAMDMAorHDMA)
							{
								do
								{
									Work = *(base + p);
                     
									//REGISTER_2180(Work);
	                        Memory.RAM[PPU.WRAM++] = Work;
	                        PPU.WRAM &= 0x1ffff;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						default:
							do
							{
								Work = *(base + p);
								S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;
					}
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					if (d->BAddress == 0x18)
					{
						// VMDATAL
					#ifndef CORRECT_VRAM_READS
						IPPU.FirstVRAMRead = TRUE;
					#endif
						if (!PPU.VMA.FullGraphicCount)
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
	                     //CHECK_INBLANK();

	                     uint32	address;

	                     Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;
									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	addresstemp;

	                        Memory.VRAM[addresstemp = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
	                     //CHECK_INBLANK();

	                     uint32	addresstemp;

	                     Memory.VRAM[addresstemp = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
						else
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
								REGISTER_2118_tile(Work);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
					}
					else
					{
						// DMA mode 1 general case
						switch (b)
						{
							default:
							while (count > 1)
							{
								Work = *(base + p);
								S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
								count--;

							case 1:
								Work = *(base + p);
								S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
								UPDATE_COUNTERS;
								count--;
							}
						}

						if (count == 1)
						{
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							b = 1;
						}
						else
							b = 0;
					}
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU_SuperFX(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}

			if (rem <= 0)
				break;

			base = S9xGetBasePointer_SuperFX((d->ABank << 16) + d->AAddress);
			count = MEMMAP_BLOCK_SIZE;
#if 0
			inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
				(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
#endif
			inWRAM_DMA = (d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000));
		}

		#undef UPDATE_COUNTERS
	}
    else
    {
		// PPU -> CPU

		// 8 cycles per byte
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			if (!addCyclesInDMA_SuperFX(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		if (d->BAddress > 0x80 - 4 && d->BAddress <= 0x83 && !(d->ABank & 0x40))
		{
			// REVERSE-DMA REALLY-SLOW PATH
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2102 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU_SuperFX(0x2103 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
		else
		{
			// REVERSE-DMA FASTER PATH
			CPU.InWRAMDMAorHDMA = (d->ABank == 0x7e || d->ABank == 0x7f);
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						Work = S9xGetPPU_SuperFX(0x2100 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2101 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2102 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU_SuperFX(0x2103 + d->BAddress);
						S9xSetByte_SuperFX(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
	}

	if ((CPU.Flags & NMI_FLAG) && (Timings.NMITriggerPos != 0xffff))
	{
		Timings.NMITriggerPos = CPU.Cycles + 24;
		if (Timings.NMITriggerPos >= Timings.H_Max)
			Timings.NMITriggerPos -= Timings.H_Max;
	}

	// Release the memory used in SPC7110 DMA
#if 0
    if (Settings.SPC7110)
    {
        if (spc7110_dma)
            delete [] spc7110_dma;
    }
#endif

#if 0
	// sanity check
    if (d->TransferBytes != 0)
		fprintf(stderr,"DMA[%d] TransferBytes not 0! $21%02x Reverse:%d %04x\n", Channel, d->BAddress, d->ReverseTransfer, d->TransferBytes);
#endif

	CPU.InDMA = FALSE;
	CPU.InDMAorHDMA = FALSE;
	CPU.InWRAMDMAorHDMA = FALSE;
	CPU.CurrentDMAorHDMAChannel = -1;

	return (TRUE);
}

bool8 S9xDoDMA_NoSA1 (uint8 Channel)
{
	CPU.InDMA = TRUE;
    CPU.InDMAorHDMA = TRUE;
	CPU.CurrentDMAorHDMAChannel = Channel;

    SDMA	*d = &DMA[Channel];

	// Check invalid DMA first
	if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80 && !d->ReverseTransfer)
	{
		// Attempting a DMA from WRAM to $2180 will not work, WRAM will not be written.
		// Attempting a DMA from $2180 to WRAM will similarly not work,
		// the value written is (initially) the OpenBus value.
		// In either case, the address in $2181-3 is not incremented.

		// Does an invalid DMA actually take time?
		// I'd say yes, since 'invalid' is probably just the WRAM chip
		// not being able to read and write itself at the same time
		// And no, PPU.WRAM should not be updated.

		int32	c = d->TransferBytes;
		// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
		if (c == 0)
			c = 0x10000;

		// 8 cycles per channel
		ADD_CYCLES(SLOW_ONE_CYCLE);
		// 8 cycles per byte
		while (c)
		{
			d->TransferBytes--;
			d->AAddress++;
			c--;
			if (!addCyclesInDMA(Channel))
			{
				CPU.InDMA = FALSE;
				CPU.InDMAorHDMA = FALSE;
				CPU.CurrentDMAorHDMAChannel = -1;
				return (FALSE);
			}
		}


		CPU.InDMA = FALSE;
		CPU.InDMAorHDMA = FALSE;
		CPU.CurrentDMAorHDMAChannel = -1;
		return (TRUE);
	}

	// Prepare for accessing $2118-2119
	switch (d->BAddress)
	{
		case 0x18:
		case 0x19:
         #ifndef NO_FRAMESKIP
			if (IPPU.RenderThisFrame)
         #endif
	         if (IPPU.PreviousLine != IPPU.CurrentLine)
		         S9xUpdateScreen();
			break;
	}

	int32	inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);
	int32	count = d->TransferBytes;
	// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
	if (count == 0)
		count = 0x10000;

	// Prepare for custom chip DMA

	// S-DD1

	uint8	*in_sdd1_dma = NULL;

	if (Settings.SDD1)
	{
		if (d->AAddressFixed && Memory.FillRAM[0x4801] > 0)
		{
			// XXX: Should probably verify that we're DMAing from ROM?
			// And somewhere we should make sure we're not running across a mapping boundary too.
			// Hacky support for pre-decompressed S-DD1 data
			inc = !d->AAddressDecrement ? 1 : -1;

			uint8	*in_ptr = S9xGetBasePointer(((d->ABank << 16) | d->AAddress));
			if (in_ptr)
			{
				in_ptr += d->AAddress;
				SDD1_decompress(sdd1_decode_buffer, in_ptr, d->TransferBytes);
			}

			in_sdd1_dma = sdd1_decode_buffer;
		}

		Memory.FillRAM[0x4801] = 0;
	}

	// SPC7110

	uint8	*spc7110_dma = NULL;

	if (Settings.SPC7110)
	{
		if (d->AAddress == 0x4800 || d->ABank == 0x50)
		{
			spc7110_dma = new uint8[d->TransferBytes];
			for (int i = 0; i < d->TransferBytes; i++)
				spc7110_dma[i] = s7emu.decomp.read();

			int32	icount = s7emu.r4809 | (s7emu.r480a << 8);
			icount -= d->TransferBytes;
			s7emu.r4809 =  icount & 0x00ff;
			s7emu.r480a = (icount & 0xff00) >> 8;

			inc = 1;
			d->AAddress -= count;
		}
	}

	// Do Transfer

	uint8	Work;

	// 8 cycles per channel
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!d->ReverseTransfer)
    {
		// CPU -> PPU
		int32	b = 0;
		uint16	p = d->AAddress;
		uint8	*base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
		bool8	inWRAM_DMA;

		int32	rem = count;
		// Transfer per block if d->AAdressFixed is FALSE
		count = d->AAddressFixed ? rem : (d->AAddressDecrement ? ((p & MEMMAP_MASK) + 1) : (MEMMAP_BLOCK_SIZE - (p & MEMMAP_MASK)));

		// Settings for custom chip DMA
		if (in_sdd1_dma)
		{
			base = in_sdd1_dma;
			p = 0;
			count = rem;
		}
		else
		if (spc7110_dma)
		{
			base = spc7110_dma;
			p = 0;
			count = rem;
		}

		inWRAM_DMA = ((!in_sdd1_dma && !spc7110_dma) &&
			(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));

		// 8 cycles per byte
      #undef UPDATE_COUNTERS
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			p += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		while (1)
		{
			if (count > rem)
				count = rem;
			rem -= count;

			CPU.InWRAMDMAorHDMA = inWRAM_DMA;

			if (!base)
			{
				// DMA SLOW PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					do
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
					} while (--count > 0);
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					// This is a variation on Duff's Device. It is legal C/C++.
					switch (b)
					{
						default:
						while (count > 1)
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							count--;

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							count--;
						}
					}

					if (count == 1)
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
						b = 1;
					}
					else
						b = 0;
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}
			else
			{
				// DMA FAST PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					switch (d->BAddress)
					{
						case 0x04: // OAMDATA
							do
							{
								Work = *(base + p);
	if (PPU.OAMAddr & 0x100)
	{
		int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
		if (Work != PPU.OAMData[addr])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = Work;
			IPPU.OBJChanged = TRUE;

			// X position high bit, and sprite size (x4)
			struct SOBJ *pObj = &PPU.OBJ[((addr & 0x1f) << 2)];
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 0) & 1];
			pObj++->Size = Work & 2;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 2) & 1];
			pObj++->Size = Work & 8;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 4) & 1];
			pObj++->Size = Work & 32;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 6) & 1];
			pObj->Size = Work & 128;
		}

		PPU.OAMFlip ^= 1;
		if (!(PPU.OAMFlip & 1))
		{
			++PPU.OAMAddr;
			PPU.OAMAddr &= 0x1ff;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			}
		}
		else
		{
			if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
				IPPU.OBJChanged = TRUE;
		}
	}
	else
	if (!(PPU.OAMFlip & 1))
	{
		PPU.OAMWriteRegister &= 0xff00;
		PPU.OAMWriteRegister |= Work;
		PPU.OAMFlip |= 1;
		if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
			IPPU.OBJChanged = TRUE;
	}
	else
	{
		PPU.OAMWriteRegister &= 0x00ff;
		uint8 lowbyte = (uint8) (PPU.OAMWriteRegister);
		uint8 highbyte = Work;
		PPU.OAMWriteRegister |= Work << 8;

		int addr = (PPU.OAMAddr << 1);
		if (lowbyte != PPU.OAMData[addr] || highbyte != PPU.OAMData[addr + 1])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = lowbyte;
			PPU.OAMData[addr + 1] = highbyte;
			IPPU.OBJChanged = TRUE;
			if (addr & 2)
			{
				// Tile
				PPU.OBJ[addr = PPU.OAMAddr >> 1].Name = PPU.OAMWriteRegister & 0x1ff;
				// priority, h and v flip.
				PPU.OBJ[addr].Palette  = (highbyte >> 1) & 7;
				PPU.OBJ[addr].Priority = (highbyte >> 4) & 3;
				PPU.OBJ[addr].HFlip    = (highbyte >> 6) & 1;
				PPU.OBJ[addr].VFlip    = (highbyte >> 7) & 1;
			}
			else
			{
				// X position (low)
				PPU.OBJ[addr = PPU.OAMAddr >> 1].HPos &= 0xff00;
				PPU.OBJ[addr].HPos |= lowbyte;
				// Sprite Y position
				PPU.OBJ[addr].VPos = highbyte;
			}
		}

		PPU.OAMFlip &= ~1;
		++PPU.OAMAddr;
		if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
		{
			PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
			IPPU.OBJChanged = TRUE;
		}
	}
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x18: // VMDATAL
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
                           //Register 2118 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (!PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x19: // VMDATAH
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x22: // CGDATA
							do
							{
								Work = *(base + p);

	if (PPU.CGFLIP)
	{
		if ((Work & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x00ff;
			PPU.CGDATA[PPU.CGADD] |= (Work & 0x7f) << 8;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Blue[PPU.CGADD] = IPPU.XB[(Work >> 2) & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}

		PPU.CGADD++;
	}
	else
	{
		if (Work != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x7f00;
			PPU.CGDATA[PPU.CGADD] |= Work;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Red[PPU.CGADD] = IPPU.XB[Work & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}
	}

	PPU.CGFLIP ^= 1;

								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x80: // WMDATA
							if (!CPU.InWRAMDMAorHDMA)
							{
								do
								{
									Work = *(base + p);
                     
									//REGISTER_2180(Work);
	                        Memory.RAM[PPU.WRAM++] = Work;
	                        PPU.WRAM &= 0x1ffff;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						default:
							do
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;
					}
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					if (d->BAddress == 0x18)
					{
						// VMDATAL
					#ifndef CORRECT_VRAM_READS
						IPPU.FirstVRAMRead = TRUE;
					#endif
						if (!PPU.VMA.FullGraphicCount)
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
                           //Register 2118 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (!PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	addresstemp;

	                        Memory.VRAM[addresstemp = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
                        //Register 2118 linear
	                     //CHECK_INBLANK();

	                     uint32	address;

	                     Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;

								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
						else
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
								REGISTER_2118_tile(Work);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
					}
					else
					{
						// DMA mode 1 general case
						switch (b)
						{
							default:
							while (count > 1)
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
								count--;

							case 1:
								Work = *(base + p);
								S9xSetPPU(Work, 0x2101 + d->BAddress);
								UPDATE_COUNTERS;
								count--;
							}
						}

						if (count == 1)
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							b = 1;
						}
						else
							b = 0;
					}
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}

			if (rem <= 0)
				break;

			base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
			count = MEMMAP_BLOCK_SIZE;
			inWRAM_DMA = ((!in_sdd1_dma && !spc7110_dma) &&
				(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
		}

		#undef UPDATE_COUNTERS
	}
    else
    {
		// PPU -> CPU

		// 8 cycles per byte
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		if (d->BAddress > 0x80 - 4 && d->BAddress <= 0x83 && !(d->ABank & 0x40))
		{
			// REVERSE-DMA REALLY-SLOW PATH
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
		else
		{
			// REVERSE-DMA FASTER PATH
			CPU.InWRAMDMAorHDMA = (d->ABank == 0x7e || d->ABank == 0x7f);
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
	}

	if ((CPU.Flags & NMI_FLAG) && (Timings.NMITriggerPos != 0xffff))
	{
		Timings.NMITriggerPos = CPU.Cycles + 24;
		if (Timings.NMITriggerPos >= Timings.H_Max)
			Timings.NMITriggerPos -= Timings.H_Max;
	}

	// Release the memory used in SPC7110 DMA
    if (Settings.SPC7110)
    {
        if (spc7110_dma)
            delete [] spc7110_dma;
    }

#if 0
	// sanity check
    if (d->TransferBytes != 0)
		fprintf(stderr,"DMA[%d] TransferBytes not 0! $21%02x Reverse:%d %04x\n", Channel, d->BAddress, d->ReverseTransfer, d->TransferBytes);
#endif

	CPU.InDMA = FALSE;
	CPU.InDMAorHDMA = FALSE;
	CPU.InWRAMDMAorHDMA = FALSE;
	CPU.CurrentDMAorHDMAChannel = -1;

	return (TRUE);
}

bool8 S9xDoDMA (uint8 Channel)
{
	CPU.InDMA = TRUE;
    CPU.InDMAorHDMA = TRUE;
	CPU.CurrentDMAorHDMAChannel = Channel;

    SDMA	*d = &DMA[Channel];

	// Check invalid DMA first
	if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80 && !d->ReverseTransfer)
	{
		// Attempting a DMA from WRAM to $2180 will not work, WRAM will not be written.
		// Attempting a DMA from $2180 to WRAM will similarly not work,
		// the value written is (initially) the OpenBus value.
		// In either case, the address in $2181-3 is not incremented.

		// Does an invalid DMA actually take time?
		// I'd say yes, since 'invalid' is probably just the WRAM chip
		// not being able to read and write itself at the same time
		// And no, PPU.WRAM should not be updated.

		int32	c = d->TransferBytes;
		// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
		if (c == 0)
			c = 0x10000;

		// 8 cycles per channel
		ADD_CYCLES(SLOW_ONE_CYCLE);
		// 8 cycles per byte
		while (c)
		{
			d->TransferBytes--;
			d->AAddress++;
			c--;
			if (!addCyclesInDMA(Channel))
			{
				CPU.InDMA = FALSE;
				CPU.InDMAorHDMA = FALSE;
				CPU.CurrentDMAorHDMAChannel = -1;
				return (FALSE);
			}
		}


		CPU.InDMA = FALSE;
		CPU.InDMAorHDMA = FALSE;
		CPU.CurrentDMAorHDMAChannel = -1;
		return (TRUE);
	}

	// Prepare for accessing $2118-2119
	switch (d->BAddress)
	{
		case 0x18:
		case 0x19:
         #ifndef NO_FRAMESKIP
			if (IPPU.RenderThisFrame)
         #endif
	         if (IPPU.PreviousLine != IPPU.CurrentLine)
		         S9xUpdateScreen();
			break;
	}

	int32	inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);
	int32	count = d->TransferBytes;
	// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
	if (count == 0)
		count = 0x10000;

	// Prepare for custom chip DMA

	// S-DD1

	uint8	*in_sdd1_dma = NULL;

	if (Settings.SDD1)
	{
		if (d->AAddressFixed && Memory.FillRAM[0x4801] > 0)
		{
			// XXX: Should probably verify that we're DMAing from ROM?
			// And somewhere we should make sure we're not running across a mapping boundary too.
			// Hacky support for pre-decompressed S-DD1 data
			inc = !d->AAddressDecrement ? 1 : -1;

			uint8	*in_ptr = S9xGetBasePointer(((d->ABank << 16) | d->AAddress));
			if (in_ptr)
			{
				in_ptr += d->AAddress;
				SDD1_decompress(sdd1_decode_buffer, in_ptr, d->TransferBytes);
			}

			in_sdd1_dma = sdd1_decode_buffer;
		}

		Memory.FillRAM[0x4801] = 0;
	}

	// SPC7110

	uint8	*spc7110_dma = NULL;

	if (Settings.SPC7110)
	{
		if (d->AAddress == 0x4800 || d->ABank == 0x50)
		{
			spc7110_dma = new uint8[d->TransferBytes];
			for (int i = 0; i < d->TransferBytes; i++)
				spc7110_dma[i] = s7emu.decomp.read();

			int32	icount = s7emu.r4809 | (s7emu.r480a << 8);
			icount -= d->TransferBytes;
			s7emu.r4809 =  icount & 0x00ff;
			s7emu.r480a = (icount & 0xff00) >> 8;

			inc = 1;
			d->AAddress -= count;
		}
	}

	// SA-1

	bool8	in_sa1_dma = FALSE;

	if (Settings.SA1)
	{
		if (SA1.in_char_dma && d->BAddress == 0x18 && (d->ABank & 0xf0) == 0x40)
		{
			// Perform packed bitmap to PPU character format conversion on the data
			// before transmitting it to V-RAM via-DMA.
			int32	num_chars = 1 << ((Memory.FillRAM[0x2231] >> 2) & 7);
			int32	depth = (Memory.FillRAM[0x2231] & 3) == 0 ? 8 : (Memory.FillRAM[0x2231] & 3) == 1 ? 4 : 2;
			int32	bytes_per_char = 8 * depth;
			int32	bytes_per_line = depth * num_chars;
			int32	char_line_bytes = bytes_per_char * num_chars;
			uint32	addr = (d->AAddress / char_line_bytes) * char_line_bytes;

			uint8	*base = S9xGetBasePointer((d->ABank << 16) + addr);
			if (!base)
			{
				//sprintf(String, "SA-1: DMA from non-block address $%02X:%04X", d->ABank, addr);
				//S9xMessage(S9X_WARNING, S9X_DMA_TRACE, String);
				base = Memory.ROM;
			}

			base += addr;

			uint8	*buffer = &Memory.ROM[CMemory::MAX_ROM_SIZE - 0x10000];
			uint8	*p = buffer;
			uint32	inc_sa1 = char_line_bytes - (d->AAddress % char_line_bytes);
			uint32	char_count = inc_sa1 / bytes_per_char;

			in_sa1_dma = TRUE;

		#if 0
			printf("SA-1 DMA: %08x,", base);
			printf("depth = %d, count = %d, bytes_per_char = %d, bytes_per_line = %d, num_chars = %d, char_line_bytes = %d\n",
				depth, count, bytes_per_char, bytes_per_line, num_chars, char_line_bytes);
		#endif

			switch (depth)
			{
				case 2:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 1);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 2)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 2; b++)
								{
									uint8	r = *(q + b);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 0) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 1) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 2) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 3) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 4) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 5) & 1);
									*(p + 0) = (*(p + 0) << 1) | ((r >> 6) & 1);
									*(p + 1) = (*(p + 1) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}
						}
					}

					break;

				case 4:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 2);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 4)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 4; b++)
								{
									uint8	r = *(q + b);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 4) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 5) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 6) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}

							p += 32 - 16;
						}
					}

					break;

				case 8:
					for (int32 i = 0; i < count; i += inc_sa1, base += char_line_bytes, inc_sa1 = char_line_bytes, char_count = num_chars)
					{
						uint8	*line = base + ((num_chars - char_count) << 3);
						for (uint32 j = 0; j < char_count && p - buffer < count; j++, line += 8)
						{
							uint8	*q = line;
							for (int32 l = 0; l < 8; l++, q += bytes_per_line)
							{
								for (int32 b = 0; b < 8; b++)
								{
									uint8	r = *(q + b);
									*(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
									*(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
									*(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
									*(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
									*(p + 32) = (*(p + 32) << 1) | ((r >> 4) & 1);
									*(p + 33) = (*(p + 33) << 1) | ((r >> 5) & 1);
									*(p + 48) = (*(p + 48) << 1) | ((r >> 6) & 1);
									*(p + 49) = (*(p + 49) << 1) | ((r >> 7) & 1);
								}

								p += 2;
							}

							p += 64 - 16;
						}
					}

					break;
			}
		}
	}


	// Do Transfer

	uint8	Work;

	// 8 cycles per channel
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!d->ReverseTransfer)
    {
		// CPU -> PPU
		int32	b = 0;
		uint16	p = d->AAddress;
		uint8	*base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
		bool8	inWRAM_DMA;

		int32	rem = count;
		// Transfer per block if d->AAdressFixed is FALSE
		count = d->AAddressFixed ? rem : (d->AAddressDecrement ? ((p & MEMMAP_MASK) + 1) : (MEMMAP_BLOCK_SIZE - (p & MEMMAP_MASK)));

		// Settings for custom chip DMA
		if (in_sa1_dma)
		{
			base = &Memory.ROM[CMemory::MAX_ROM_SIZE - 0x10000];
			p = 0;
			count = rem;
		}
		else
		if (in_sdd1_dma)
		{
			base = in_sdd1_dma;
			p = 0;
			count = rem;
		}
		else
		if (spc7110_dma)
		{
			base = spc7110_dma;
			p = 0;
			count = rem;
		}

		inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
			(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));

		// 8 cycles per byte
      #undef UPDATE_COUNTERS
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			p += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		while (1)
		{
			if (count > rem)
				count = rem;
			rem -= count;

			CPU.InWRAMDMAorHDMA = inWRAM_DMA;

			if (!base)
			{
				// DMA SLOW PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					do
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
					} while (--count > 0);
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					// This is a variation on Duff's Device. It is legal C/C++.
					switch (b)
					{
						default:
						while (count > 1)
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							count--;

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							count--;
						}
					}

					if (count == 1)
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
						b = 1;
					}
					else
						b = 0;
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}
			else
			{
				// DMA FAST PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					switch (d->BAddress)
					{
						case 0x04: // OAMDATA
							do
							{
								Work = *(base + p);
	if (PPU.OAMAddr & 0x100)
	{
		int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
		if (Work != PPU.OAMData[addr])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = Work;
			IPPU.OBJChanged = TRUE;

			// X position high bit, and sprite size (x4)
			struct SOBJ *pObj = &PPU.OBJ[((addr & 0x1f) << 2)];
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 0) & 1];
			pObj++->Size = Work & 2;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 2) & 1];
			pObj++->Size = Work & 8;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 4) & 1];
			pObj++->Size = Work & 32;
			pObj->HPos = (pObj->HPos & 0xFF) | SignExtend[(Work >> 6) & 1];
			pObj->Size = Work & 128;
		}

		PPU.OAMFlip ^= 1;
		if (!(PPU.OAMFlip & 1))
		{
			++PPU.OAMAddr;
			PPU.OAMAddr &= 0x1ff;
			if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
			{
				PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
				IPPU.OBJChanged = TRUE;
			}
		}
		else
		{
			if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
				IPPU.OBJChanged = TRUE;
		}
	}
	else
	if (!(PPU.OAMFlip & 1))
	{
		PPU.OAMWriteRegister &= 0xff00;
		PPU.OAMWriteRegister |= Work;
		PPU.OAMFlip |= 1;
		if (PPU.OAMPriorityRotation && (PPU.OAMAddr & 1))
			IPPU.OBJChanged = TRUE;
	}
	else
	{
		PPU.OAMWriteRegister &= 0x00ff;
		uint8 lowbyte = (uint8) (PPU.OAMWriteRegister);
		uint8 highbyte = Work;
		PPU.OAMWriteRegister |= Work << 8;

		int addr = (PPU.OAMAddr << 1);
		if (lowbyte != PPU.OAMData[addr] || highbyte != PPU.OAMData[addr + 1])
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.OAMData[addr] = lowbyte;
			PPU.OAMData[addr + 1] = highbyte;
			IPPU.OBJChanged = TRUE;
			if (addr & 2)
			{
				// Tile
				PPU.OBJ[addr = PPU.OAMAddr >> 1].Name = PPU.OAMWriteRegister & 0x1ff;
				// priority, h and v flip.
				PPU.OBJ[addr].Palette  = (highbyte >> 1) & 7;
				PPU.OBJ[addr].Priority = (highbyte >> 4) & 3;
				PPU.OBJ[addr].HFlip    = (highbyte >> 6) & 1;
				PPU.OBJ[addr].VFlip    = (highbyte >> 7) & 1;
			}
			else
			{
				// X position (low)
				PPU.OBJ[addr = PPU.OAMAddr >> 1].HPos &= 0xff00;
				PPU.OBJ[addr].HPos |= lowbyte;
				// Sprite Y position
				PPU.OBJ[addr].VPos = highbyte;
			}
		}

		PPU.OAMFlip &= ~1;
		++PPU.OAMAddr;
		if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
		{
			PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
			IPPU.OBJChanged = TRUE;
		}
	}
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x18: // VMDATAL
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
	                     //CHECK_INBLANK();

	                     uint32	address;

	                     Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x19: // VMDATAH
						#ifndef CORRECT_VRAM_READS
							IPPU.FirstVRAMRead = TRUE;
						#endif
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	address;

	                        Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x22: // CGDATA
							do
							{
								Work = *(base + p);

	if (PPU.CGFLIP)
	{
		if ((Work & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x00ff;
			PPU.CGDATA[PPU.CGADD] |= (Work & 0x7f) << 8;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Blue[PPU.CGADD] = IPPU.XB[(Work >> 2) & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}

		PPU.CGADD++;
	}
	else
	{
		if (Work != (uint8) (PPU.CGDATA[PPU.CGADD] & 0xff))
		{
	      if (IPPU.PreviousLine != IPPU.CurrentLine)
		      S9xUpdateScreen();
			PPU.CGDATA[PPU.CGADD] &= 0x7f00;
			PPU.CGDATA[PPU.CGADD] |= Work;
			//IPPU.ColorsChanged = TRUE;
			IPPU.Red[PPU.CGADD] = IPPU.XB[Work & 0x1f];
			IPPU.Green[PPU.CGADD] = IPPU.XB[(PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f];
         IPPU.ScreenColors[PPU.CGADD] = (uint16) (((int) (IPPU.Red[PPU.CGADD]) << 10) | ((int) (IPPU.Green[PPU.CGADD]) << 5) | (int) (IPPU.Blue[PPU.CGADD]));
		}
	}

	PPU.CGFLIP ^= 1;

								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x80: // WMDATA
							if (!CPU.InWRAMDMAorHDMA)
							{
								do
								{
									Work = *(base + p);
                     
									//REGISTER_2180(Work);
	                        Memory.RAM[PPU.WRAM++] = Work;
	                        PPU.WRAM &= 0x1ffff;

									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						default:
							do
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;
					}
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					if (d->BAddress == 0x18)
					{
						// VMDATAL
					#ifndef CORRECT_VRAM_READS
						IPPU.FirstVRAMRead = TRUE;
					#endif
						if (!PPU.VMA.FullGraphicCount)
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
	                     //CHECK_INBLANK();

	                     uint32	address;

	                     Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;
									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           // Register 2119 linear
	                        //CHECK_INBLANK();

	                        uint32	addresstemp;

	                        Memory.VRAM[addresstemp = ((PPU.VMA.Address << 1) + 1) & 0xffff] = Work;

	                        IPPU.TileCached[TILE_2BIT][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][addresstemp >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [addresstemp >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((addresstemp >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [addresstemp >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((addresstemp >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
	                     //CHECK_INBLANK();

	                     uint32	address;

	                     Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xffff] = Work;

	                     IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                     IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                     IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                     if (!PPU.VMA.High)
		                    PPU.VMA.Address += PPU.VMA.Increment;
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
						else
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
									count--;

								case 1:
									Work = *(base + p);
                           //Register 2119 tile
	                        //CHECK_INBLANK();

	                        uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
	                        uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xffff;

	                        Memory.VRAM[address] = Work;

	                        IPPU.TileCached[TILE_2BIT][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_4BIT][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_8BIT][address >> 6] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_EVEN][((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [address >> 4] = FALSE;
	                        IPPU.TileCached[TILE_2BIT_ODD] [((address >> 4) - 1) & (MAX_2BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_EVEN][((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [address >> 5] = FALSE;
	                        IPPU.TileCached[TILE_4BIT_ODD] [((address >> 5) - 1) & (MAX_4BIT_TILES - 1)] = FALSE;

	                        if (PPU.VMA.High)
		                        PPU.VMA.Address += PPU.VMA.Increment;

									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
								REGISTER_2118_tile(Work);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
					}
					else
					{
						// DMA mode 1 general case
						switch (b)
						{
							default:
							while (count > 1)
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
								count--;

							case 1:
								Work = *(base + p);
								S9xSetPPU(Work, 0x2101 + d->BAddress);
								UPDATE_COUNTERS;
								count--;
							}
						}

						if (count == 1)
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							b = 1;
						}
						else
							b = 0;
					}
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}

						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}

						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}

						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			}

			if (rem <= 0)
				break;

			base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
			count = MEMMAP_BLOCK_SIZE;
			inWRAM_DMA = ((!in_sa1_dma && !in_sdd1_dma && !spc7110_dma) &&
				(d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
		}

		#undef UPDATE_COUNTERS
	}
    else
    {
		// PPU -> CPU

		// 8 cycles per byte
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		if (d->BAddress > 0x80 - 4 && d->BAddress <= 0x83 && !(d->ABank & 0x40))
		{
			// REVERSE-DMA REALLY-SLOW PATH
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
		else
		{
			// REVERSE-DMA FASTER PATH
			CPU.InWRAMDMAorHDMA = (d->ABank == 0x7e || d->ABank == 0x7f);
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
	}

	if ((CPU.Flags & NMI_FLAG) && (Timings.NMITriggerPos != 0xffff))
	{
		Timings.NMITriggerPos = CPU.Cycles + 24;
		if (Timings.NMITriggerPos >= Timings.H_Max)
			Timings.NMITriggerPos -= Timings.H_Max;
	}

	// Release the memory used in SPC7110 DMA
    if (Settings.SPC7110)
    {
        if (spc7110_dma)
            delete [] spc7110_dma;
    }

#if 0
	// sanity check
    if (d->TransferBytes != 0)
		fprintf(stderr,"DMA[%d] TransferBytes not 0! $21%02x Reverse:%d %04x\n", Channel, d->BAddress, d->ReverseTransfer, d->TransferBytes);
#endif

	CPU.InDMA = FALSE;
	CPU.InDMAorHDMA = FALSE;
	CPU.InWRAMDMAorHDMA = FALSE;
	CPU.CurrentDMAorHDMAChannel = -1;

	return (TRUE);
}

static inline bool8 HDMAReadLineCount_SuperFX (int d)
{
	// CPU.InDMA is set, so S9xGetXXX() / S9xSetXXX() incur no charges.

	uint8	line;

	line = S9xGetByte_SuperFX((DMA[d].ABank << 16) + DMA[d].Address);
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!line)
	{
		DMA[d].Repeat = FALSE;
		DMA[d].LineCount = 128;

		if (DMA[d].HDMAIndirectAddressing)
		{
			if (PPU.HDMA & (0xfe << d))
			{
				DMA[d].Address++;
				ADD_CYCLES(SLOW_ONE_CYCLE << 1);
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);

			DMA[d].IndirectAddress = S9xGetWord_WrapNone_SuperFX((DMA[d].ABank << 16) + DMA[d].Address);
			DMA[d].Address++;
		}

		DMA[d].Address++;
		HDMAMemPointers[d] = NULL;

		return (FALSE);
	}
	else
	if (line == 0x80)
	{
		DMA[d].Repeat = TRUE;
		DMA[d].LineCount = 128;
	}
	else
	{
		DMA[d].Repeat = !(line & 0x80);
		DMA[d].LineCount = line & 0x7f;
	}

	DMA[d].Address++;
	DMA[d].DoTransfer = TRUE;

	if (DMA[d].HDMAIndirectAddressing)
	{
		ADD_CYCLES(SLOW_ONE_CYCLE << 1);
		DMA[d].IndirectAddress = S9xGetWord_WrapNone_SuperFX((DMA[d].ABank << 16) + DMA[d].Address);
		DMA[d].Address += 2;
		HDMAMemPointers[d] = S9xGetMemPointer_SuperFX((DMA[d].IndirectBank << 16) + DMA[d].IndirectAddress);
	}
	else
		HDMAMemPointers[d] = S9xGetMemPointer_SuperFX((DMA[d].ABank << 16) + DMA[d].Address);

	return (TRUE);
}

static inline bool8 HDMAReadLineCount (int d)
{
	// CPU.InDMA is set, so S9xGetXXX() / S9xSetXXX() incur no charges.

	uint8	line;

	line = S9xGetByte((DMA[d].ABank << 16) + DMA[d].Address);
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!line)
	{
		DMA[d].Repeat = FALSE;
		DMA[d].LineCount = 128;

		if (DMA[d].HDMAIndirectAddressing)
		{
			if (PPU.HDMA & (0xfe << d))
			{
				DMA[d].Address++;
				ADD_CYCLES(SLOW_ONE_CYCLE << 1);
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);

			DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address);
			DMA[d].Address++;
		}

		DMA[d].Address++;
		HDMAMemPointers[d] = NULL;

		return (FALSE);
	}
	else
	if (line == 0x80)
	{
		DMA[d].Repeat = TRUE;
		DMA[d].LineCount = 128;
	}
	else
	{
		DMA[d].Repeat = !(line & 0x80);
		DMA[d].LineCount = line & 0x7f;
	}

	DMA[d].Address++;
	DMA[d].DoTransfer = TRUE;

	if (DMA[d].HDMAIndirectAddressing)
	{
		ADD_CYCLES(SLOW_ONE_CYCLE << 1);
		DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address);
		DMA[d].Address += 2;
		HDMAMemPointers[d] = S9xGetMemPointer((DMA[d].IndirectBank << 16) + DMA[d].IndirectAddress);
	}
	else
		HDMAMemPointers[d] = S9xGetMemPointer((DMA[d].ABank << 16) + DMA[d].Address);

	return (TRUE);
}

void S9xStartHDMA_SuperFX (void)
{

	PPU.HDMA = Memory.FillRAM[0x420c];

	PPU.HDMAEnded = 0;

	int32	tmpch;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	if (PPU.HDMA != 0)
		ADD_CYCLES(Timings.DMACPUSync);

      //0
		if (PPU.HDMA & 1)
		{
			CPU.CurrentDMAorHDMAChannel = 0;

			DMA[0].Address = DMA[0].AAddress;

			if (!HDMAReadLineCount_SuperFX(0))
			{
				PPU.HDMA &= ~(1);
				PPU.HDMAEnded |= 1;
			}
		}
		else
			DMA[0].DoTransfer = FALSE;

      //1
		if (PPU.HDMA & 2)
		{
			CPU.CurrentDMAorHDMAChannel = 1;

			DMA[1].Address = DMA[1].AAddress;

			if (!HDMAReadLineCount_SuperFX(1))
			{
				PPU.HDMA &= ~(2);
				PPU.HDMAEnded |= 2;
			}
		}
		else
			DMA[1].DoTransfer = FALSE;

      //2
		if (PPU.HDMA & 4)
		{
			CPU.CurrentDMAorHDMAChannel = 2;

			DMA[2].Address = DMA[2].AAddress;

			if (!HDMAReadLineCount_SuperFX(2))
			{
				PPU.HDMA &= ~4;
				PPU.HDMAEnded |= 4;
			}
		}
		else
			DMA[2].DoTransfer = FALSE;

      //3
		if (PPU.HDMA & 8)
		{
			CPU.CurrentDMAorHDMAChannel = 3;

			DMA[3].Address = DMA[3].AAddress;

			if (!HDMAReadLineCount_SuperFX(3))
			{
				PPU.HDMA &= ~(8);
				PPU.HDMAEnded |= 8;
			}
		}
		else
			DMA[3].DoTransfer = FALSE;

      //4
		if (PPU.HDMA & 16)
		{
			CPU.CurrentDMAorHDMAChannel = 4;

			DMA[4].Address = DMA[4].AAddress;

			if (!HDMAReadLineCount_SuperFX(4))
			{
				PPU.HDMA &= ~(16);
				PPU.HDMAEnded |= (16);
			}
		}
		else
			DMA[4].DoTransfer = FALSE;

      //5
		if (PPU.HDMA & 32)
		{
			CPU.CurrentDMAorHDMAChannel = 5;

			DMA[5].Address = DMA[5].AAddress;

			if (!HDMAReadLineCount_SuperFX(5))
			{
				PPU.HDMA &= ~(32);
				PPU.HDMAEnded |= 32;
			}
		}
		else
			DMA[5].DoTransfer = FALSE;

      //6
		if (PPU.HDMA & 64)
		{
			CPU.CurrentDMAorHDMAChannel = 6;

			DMA[6].Address = DMA[6].AAddress;

			if (!HDMAReadLineCount_SuperFX(6))
			{
				PPU.HDMA &= ~(64);
				PPU.HDMAEnded |= 64;
			}
		}
		else
			DMA[6].DoTransfer = FALSE;

      //7
		if (PPU.HDMA & 128)
		{
			CPU.CurrentDMAorHDMAChannel = 7;

			DMA[7].Address = DMA[7].AAddress;

			if (!HDMAReadLineCount_SuperFX(7))
			{
				PPU.HDMA &= ~(128);
				PPU.HDMAEnded |= 128;
			}
		}
		else
			DMA[7].DoTransfer = FALSE;

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.HDMARanInDMA = CPU.InDMA ? PPU.HDMA : 0;
	CPU.CurrentDMAorHDMAChannel = tmpch;
}

void S9xStartHDMA (void)
{

	PPU.HDMA = Memory.FillRAM[0x420c];

	PPU.HDMAEnded = 0;

	int32	tmpch;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	if (PPU.HDMA != 0)
		ADD_CYCLES(Timings.DMACPUSync);

      //0
		if (PPU.HDMA & 1)
		{
			CPU.CurrentDMAorHDMAChannel = 0;

			DMA[0].Address = DMA[0].AAddress;

			if (!HDMAReadLineCount_SuperFX(0))
			{
				PPU.HDMA &= ~(1);
				PPU.HDMAEnded |= 1;
			}
		}
		else
			DMA[0].DoTransfer = FALSE;

      //1
		if (PPU.HDMA & 2)
		{
			CPU.CurrentDMAorHDMAChannel = 1;

			DMA[1].Address = DMA[1].AAddress;

			if (!HDMAReadLineCount_SuperFX(1))
			{
				PPU.HDMA &= ~(2);
				PPU.HDMAEnded |= 2;
			}
		}
		else
			DMA[1].DoTransfer = FALSE;

      //2
		if (PPU.HDMA & 4)
		{
			CPU.CurrentDMAorHDMAChannel = 2;

			DMA[2].Address = DMA[2].AAddress;

			if (!HDMAReadLineCount_SuperFX(2))
			{
				PPU.HDMA &= ~4;
				PPU.HDMAEnded |= 4;
			}
		}
		else
			DMA[2].DoTransfer = FALSE;

      //3
		if (PPU.HDMA & 8)
		{
			CPU.CurrentDMAorHDMAChannel = 3;

			DMA[3].Address = DMA[3].AAddress;

			if (!HDMAReadLineCount_SuperFX(3))
			{
				PPU.HDMA &= ~(8);
				PPU.HDMAEnded |= 8;
			}
		}
		else
			DMA[3].DoTransfer = FALSE;

      //4
		if (PPU.HDMA & 16)
		{
			CPU.CurrentDMAorHDMAChannel = 4;

			DMA[4].Address = DMA[4].AAddress;

			if (!HDMAReadLineCount_SuperFX(4))
			{
				PPU.HDMA &= ~(16);
				PPU.HDMAEnded |= (16);
			}
		}
		else
			DMA[4].DoTransfer = FALSE;

      //5
		if (PPU.HDMA & 32)
		{
			CPU.CurrentDMAorHDMAChannel = 5;

			DMA[5].Address = DMA[5].AAddress;

			if (!HDMAReadLineCount_SuperFX(5))
			{
				PPU.HDMA &= ~(32);
				PPU.HDMAEnded |= 32;
			}
		}
		else
			DMA[5].DoTransfer = FALSE;

      //6
		if (PPU.HDMA & 64)
		{
			CPU.CurrentDMAorHDMAChannel = 6;

			DMA[6].Address = DMA[6].AAddress;

			if (!HDMAReadLineCount_SuperFX(6))
			{
				PPU.HDMA &= ~(64);
				PPU.HDMAEnded |= 64;
			}
		}
		else
			DMA[6].DoTransfer = FALSE;

      //7
		if (PPU.HDMA & 128)
		{
			CPU.CurrentDMAorHDMAChannel = 7;

			DMA[7].Address = DMA[7].AAddress;

			if (!HDMAReadLineCount_SuperFX(7))
			{
				PPU.HDMA &= ~(128);
				PPU.HDMAEnded |= 128;
			}
		}
		else
			DMA[7].DoTransfer = FALSE;


	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.HDMARanInDMA = CPU.InDMA ? PPU.HDMA : 0;
	CPU.CurrentDMAorHDMAChannel = tmpch;
}

uint8 S9xDoHDMA_NoUniracers_SuperFX (uint8 byte)
{
	struct SDMA	*p = &DMA[0];

	uint32	ShiftedIBank;
	uint16	IAddr;
	bool8	temp;
	int32	tmpch;
	int		d = 0;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	CPU.HDMARanInDMA = CPU.InDMA ? byte : 0;
	temp = CPU.InWRAMDMAorHDMA;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	ADD_CYCLES(Timings.DMACPUSync);

	for (uint8 mask = 1; mask; mask <<= 1, p++, d++)
	{
		if (byte & mask)
		{
			CPU.InWRAMDMAorHDMA = FALSE;
			CPU.CurrentDMAorHDMAChannel = d;

			if (p->HDMAIndirectAddressing)
			{
				ShiftedIBank = (p->IndirectBank << 16);
				IAddr = p->IndirectAddress;
			}
			else
			{
				ShiftedIBank = (p->ABank << 16);
				IAddr = p->Address;
			}

			if (!HDMAMemPointers[d])
				HDMAMemPointers[d] = S9xGetMemPointer_SuperFX(ShiftedIBank + IAddr);

			if (p->DoTransfer)
			{
				// XXX: Hack for Uniracers, because we don't understand
				// OAM Address Invalidation
            /*
				if (p->BAddress == 0x04)
				{
					if (SNESGameFixes.Uniracers)
					{
						PPU.OAMAddr = 0x10c;
						PPU.OAMFlip = 0;
					}
				}
            */

				if (!p->ReverseTransfer)
				{
					if ((IAddr & MEMMAP_MASK) + HDMA_ModeByteCounts[p->TransferMode] >= MEMMAP_BLOCK_SIZE)
					{
						// HDMA REALLY-SLOW PATH
						HDMAMemPointers[d] = NULL;

						#define DOBYTE(Addr, RegOff) \
							CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
								(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
							S9xSetPPU(S9xGetByte(ShiftedIBank + ((uint16) (Addr))), 0x2100 + p->BAddress + (RegOff));

						DOBYTE(IAddr, 0);
						ADD_CYCLES(SLOW_ONE_CYCLE);
						switch (p->TransferMode)
						{
							case 0:
								break;

							case 5:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 1:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 2:
							case 6:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 3:
							case 7:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 4:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 2);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 3);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;
						}

						#undef DOBYTE
					}
					else
					{
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 ||
							(!(ShiftedIBank & 0x400000) && IAddr < 0x2000));

						if (!HDMAMemPointers[d])
						{
							// HDMA SLOW PATH
							uint32	Addr = ShiftedIBank + IAddr;

							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(S9xGetByte(Addr), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									Addr += 2;
									/* fall through */
								case 1:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 2:
								case 6:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 3:
								case 7:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 4:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;
							}
						}
						else
						{
							// HDMA FAST PATH
							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(*HDMAMemPointers[d]++, 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									/* fall through */
								case 1:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 2:
								case 6:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 3:
								case 7:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;

								case 4:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;
							}
						}
					}
				}
				else
				{
					// REVERSE HDMA REALLY-SLOW PATH
					// anomie says: Since this is apparently never used
					// (otherwise we would have noticed before now), let's not bother with faster paths.
					HDMAMemPointers[d] = NULL;

					#define DOBYTE(Addr, RegOff) \
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
							(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
						S9xSetByte(S9xGetPPU(0x2100 + p->BAddress + (RegOff)), ShiftedIBank + ((uint16) (Addr)));

					DOBYTE(IAddr, 0);
					ADD_CYCLES(SLOW_ONE_CYCLE);

					switch (p->TransferMode)
					{
						case 0:
							break;

						case 5:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 1:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 2:
						case 6:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 3:
						case 7:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 4:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 2);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 3);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;
					}

					#undef DOBYTE
				}

				if (p->HDMAIndirectAddressing)
					p->IndirectAddress += HDMA_ModeByteCounts[p->TransferMode];
				else
					p->Address += HDMA_ModeByteCounts[p->TransferMode];
			}

			p->DoTransfer = !p->Repeat;

			if (!--p->LineCount)
			{
				if (!HDMAReadLineCount_SuperFX(d))
				{
					byte &= ~mask;
					PPU.HDMAEnded |= mask;
					p->DoTransfer = FALSE;
					continue;
				}
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);
		}
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.InWRAMDMAorHDMA = temp;
	CPU.CurrentDMAorHDMAChannel = tmpch;

	return (byte);
}

uint8 S9xDoHDMA_NoUniracers (uint8 byte)
{
	struct SDMA	*p = &DMA[0];

	uint32	ShiftedIBank;
	uint16	IAddr;
	bool8	temp;
	int32	tmpch;
	int		d = 0;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	CPU.HDMARanInDMA = CPU.InDMA ? byte : 0;
	temp = CPU.InWRAMDMAorHDMA;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	ADD_CYCLES(Timings.DMACPUSync);

	for (uint8 mask = 1; mask; mask <<= 1, p++, d++)
	{
		if (byte & mask)
		{
			CPU.InWRAMDMAorHDMA = FALSE;
			CPU.CurrentDMAorHDMAChannel = d;

			if (p->HDMAIndirectAddressing)
			{
				ShiftedIBank = (p->IndirectBank << 16);
				IAddr = p->IndirectAddress;
			}
			else
			{
				ShiftedIBank = (p->ABank << 16);
				IAddr = p->Address;
			}

			if (!HDMAMemPointers[d])
				HDMAMemPointers[d] = S9xGetMemPointer(ShiftedIBank + IAddr);

			if (p->DoTransfer)
			{
				// XXX: Hack for Uniracers, because we don't understand
				// OAM Address Invalidation
            /*
				if (p->BAddress == 0x04)
				{
					if (SNESGameFixes.Uniracers)
					{
						PPU.OAMAddr = 0x10c;
						PPU.OAMFlip = 0;
					}
				}
            */

				if (!p->ReverseTransfer)
				{
					if ((IAddr & MEMMAP_MASK) + HDMA_ModeByteCounts[p->TransferMode] >= MEMMAP_BLOCK_SIZE)
					{
						// HDMA REALLY-SLOW PATH
						HDMAMemPointers[d] = NULL;

						#define DOBYTE(Addr, RegOff) \
							CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
								(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
							S9xSetPPU(S9xGetByte(ShiftedIBank + ((uint16) (Addr))), 0x2100 + p->BAddress + (RegOff));

						DOBYTE(IAddr, 0);
						ADD_CYCLES(SLOW_ONE_CYCLE);
						switch (p->TransferMode)
						{
							case 0:
								break;

							case 5:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 1:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 2:
							case 6:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 3:
							case 7:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 4:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 2);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 3);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;
						}

						#undef DOBYTE
					}
					else
					{
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 ||
							(!(ShiftedIBank & 0x400000) && IAddr < 0x2000));

						if (!HDMAMemPointers[d])
						{
							// HDMA SLOW PATH
							uint32	Addr = ShiftedIBank + IAddr;

							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(S9xGetByte(Addr), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									Addr += 2;
									/* fall through */
								case 1:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 2:
								case 6:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 3:
								case 7:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 4:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;
							}
						}
						else
						{
							// HDMA FAST PATH
							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(*HDMAMemPointers[d]++, 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									/* fall through */
								case 1:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 2:
								case 6:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 3:
								case 7:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;

								case 4:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;
							}
						}
					}
				}
				else
				{
					// REVERSE HDMA REALLY-SLOW PATH
					// anomie says: Since this is apparently never used
					// (otherwise we would have noticed before now), let's not bother with faster paths.
					HDMAMemPointers[d] = NULL;

					#define DOBYTE(Addr, RegOff) \
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
							(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
						S9xSetByte(S9xGetPPU(0x2100 + p->BAddress + (RegOff)), ShiftedIBank + ((uint16) (Addr)));

					DOBYTE(IAddr, 0);
					ADD_CYCLES(SLOW_ONE_CYCLE);
					switch (p->TransferMode)
					{
						case 0:
							break;

						case 5:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 1:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 2:
						case 6:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 3:
						case 7:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 4:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 2);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 3);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;
					}

					#undef DOBYTE
				}

				if (p->HDMAIndirectAddressing)
					p->IndirectAddress += HDMA_ModeByteCounts[p->TransferMode];
				else
					p->Address += HDMA_ModeByteCounts[p->TransferMode];
			}

			p->DoTransfer = !p->Repeat;

			if (!--p->LineCount)
			{
				if (!HDMAReadLineCount(d))
				{
					byte &= ~mask;
					PPU.HDMAEnded |= mask;
					p->DoTransfer = FALSE;
					continue;
				}
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);
		}
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.InWRAMDMAorHDMA = temp;
	CPU.CurrentDMAorHDMAChannel = tmpch;

	return (byte);
}

uint8 S9xDoHDMA (uint8 byte)
{
	struct SDMA	*p = &DMA[0];

	uint32	ShiftedIBank;
	uint16	IAddr;
	bool8	temp;
	int32	tmpch;
	int		d = 0;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	CPU.HDMARanInDMA = CPU.InDMA ? byte : 0;
	temp = CPU.InWRAMDMAorHDMA;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	ADD_CYCLES(Timings.DMACPUSync);

	for (uint8 mask = 1; mask; mask <<= 1, p++, d++)
	{
		if (byte & mask)
		{
			CPU.InWRAMDMAorHDMA = FALSE;
			CPU.CurrentDMAorHDMAChannel = d;

			if (p->HDMAIndirectAddressing)
			{
				ShiftedIBank = (p->IndirectBank << 16);
				IAddr = p->IndirectAddress;
			}
			else
			{
				ShiftedIBank = (p->ABank << 16);
				IAddr = p->Address;
			}

			if (!HDMAMemPointers[d])
				HDMAMemPointers[d] = S9xGetMemPointer(ShiftedIBank + IAddr);

			if (p->DoTransfer)
			{
				// XXX: Hack for Uniracers, because we don't understand
				// OAM Address Invalidation
				if (p->BAddress == 0x04)
				{
					if (SNESGameFixes.Uniracers)
					{
						PPU.OAMAddr = 0x10c;
						PPU.OAMFlip = 0;
					}
				}


				if (!p->ReverseTransfer)
				{
					if ((IAddr & MEMMAP_MASK) + HDMA_ModeByteCounts[p->TransferMode] >= MEMMAP_BLOCK_SIZE)
					{
						// HDMA REALLY-SLOW PATH
						HDMAMemPointers[d] = NULL;

						#define DOBYTE(Addr, RegOff) \
							CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
								(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
							S9xSetPPU(S9xGetByte(ShiftedIBank + ((uint16) (Addr))), 0x2100 + p->BAddress + (RegOff));

						DOBYTE(IAddr, 0);
						ADD_CYCLES(SLOW_ONE_CYCLE);
						switch (p->TransferMode)
						{
							case 0:
								break;

							case 5:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 1:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 2:
							case 6:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 3:
							case 7:
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 4:
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 2);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 3);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;
						}

						#undef DOBYTE
					}
					else
					{
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 ||
							(!(ShiftedIBank & 0x400000) && IAddr < 0x2000));

						if (!HDMAMemPointers[d])
						{
							// HDMA SLOW PATH
							uint32	Addr = ShiftedIBank + IAddr;

							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(S9xGetByte(Addr), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									Addr += 2;
									/* fall through */
								case 1:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 2:
								case 6:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 3:
								case 7:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 4:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;
							}
						}
						else
						{
							// HDMA FAST PATH
							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(*HDMAMemPointers[d]++, 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									/* fall through */
								case 1:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 2:
								case 6:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 2;
									break;

								case 3:
								case 7:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;

								case 4:
									S9xSetPPU(*(HDMAMemPointers[d] + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(HDMAMemPointers[d] + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									HDMAMemPointers[d] += 4;
									break;
							}
						}
					}
				}
				else
				{
					// REVERSE HDMA REALLY-SLOW PATH
					// anomie says: Since this is apparently never used
					// (otherwise we would have noticed before now), let's not bother with faster paths.
					HDMAMemPointers[d] = NULL;

					#define DOBYTE(Addr, RegOff) \
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
							(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
						S9xSetByte(S9xGetPPU(0x2100 + p->BAddress + (RegOff)), ShiftedIBank + ((uint16) (Addr)));

					DOBYTE(IAddr, 0);
					ADD_CYCLES(SLOW_ONE_CYCLE);
					switch (p->TransferMode)
					{
						case 0:
							break;

						case 5:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 1:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 2:
						case 6:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 3:
						case 7:
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 4:
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 2);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 3);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;
					}

					#undef DOBYTE
				}

				if (p->HDMAIndirectAddressing)
					p->IndirectAddress += HDMA_ModeByteCounts[p->TransferMode];
				else
					p->Address += HDMA_ModeByteCounts[p->TransferMode];
			}

			p->DoTransfer = !p->Repeat;

			if (!--p->LineCount)
			{
				if (!HDMAReadLineCount(d))
				{
					byte &= ~mask;
					PPU.HDMAEnded |= mask;
					p->DoTransfer = FALSE;
					continue;
				}
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);
		}
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.InWRAMDMAorHDMA = temp;
	CPU.CurrentDMAorHDMAChannel = tmpch;

	return (byte);
}

void S9xResetDMA (void)
{
	for (int d = 0; d < 8; d++)
	{
		DMA[d].ReverseTransfer = TRUE;
		DMA[d].HDMAIndirectAddressing = TRUE;
		DMA[d].AAddressFixed = TRUE;
		DMA[d].AAddressDecrement = TRUE;
		DMA[d].TransferMode = 7;
		DMA[d].BAddress = 0xff;
		DMA[d].AAddress = 0xffff;
		DMA[d].ABank = 0xff;
		DMA[d].DMACount_Or_HDMAIndirectAddress = 0xffff;
		DMA[d].IndirectBank = 0xff;
		DMA[d].Address = 0xffff;
		DMA[d].Repeat = FALSE;
		DMA[d].LineCount = 0x7f;
		DMA[d].UnknownByte = 0xff;
		DMA[d].DoTransfer = FALSE;
		DMA[d].UnusedBit43x0 = 1;
	}
}

