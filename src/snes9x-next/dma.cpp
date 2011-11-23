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

#define ADD_CYCLES(n)	CPU.Cycles += (n)

extern uint8	*HDMAMemPointers[8];
static int HDMA_ModeByteCounts[8] =
{
	1, 2, 2, 4, 4, 4, 2, 4
};


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

			DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address, WRAP_NONE);
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
		DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address, WRAP_NONE);
		DMA[d].Address += 2;
		HDMAMemPointers[d] = S9xGetMemPointer((DMA[d].IndirectBank << 16) + DMA[d].IndirectAddress);
	}
	else
		HDMAMemPointers[d] = S9xGetMemPointer((DMA[d].ABank << 16) + DMA[d].Address);

	return (TRUE);
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

	for (uint8 i = 0; i < 8; i++)
	{
		if (PPU.HDMA & (1 << i))
		{
			CPU.CurrentDMAorHDMAChannel = i;

			DMA[i].Address = DMA[i].AAddress;

			if (!HDMAReadLineCount(i))
			{
				PPU.HDMA &= ~(1 << i);
				PPU.HDMAEnded |= (1 << i);
			}
		}
		else
			DMA[i].DoTransfer = FALSE;
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.HDMARanInDMA = CPU.InDMA ? PPU.HDMA : 0;
	CPU.CurrentDMAorHDMAChannel = tmpch;
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

						switch (p->TransferMode)
						{
							case 0:
								DOBYTE(IAddr, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 5:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 1:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 2:
							case 6:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 3:
							case 7:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 4:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
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

					switch (p->TransferMode)
					{
						case 0:
							DOBYTE(IAddr, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 5:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 1:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 2:
						case 6:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 3:
						case 7:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 4:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
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
