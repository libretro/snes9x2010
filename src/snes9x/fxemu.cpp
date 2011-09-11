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
#include "fxinst.h"
#include "fxemu.h"
#include "shiftmul.h"
#include "isel.h"

static void FxReset (struct FxInfo_s *);
//static void fx_readRegisterSpace (void);
//static void fx_writeRegisterSpace (void);
//static void fx_updateRamBank (uint8);
//static void fx_dirtySCBR (void);
static bool8 fx_checkStartAddress (void);
static uint32 FxEmulate (uint32);
//static void FxCacheWriteAccess (uint16);
//static void FxFlushCache (void);


void S9xInitSuperFX (void)
{
	memset((uint8 *) &GSU, 0, sizeof(struct FxRegs_s));
}

void S9xResetSuperFX (void)
{
	// FIXME: Snes9x can't execute CPU and SuperFX at a time. Don't ask me what is 0.417 :P
	SuperFX.speedPerLine = (uint32) (0.417 * 10.5e6 * ((1.0 / (float) Memory.ROMFramesPerSecond) / ((float) (Timings.V_Max))));
	SuperFX.oneLineDone = FALSE;
	SuperFX.vFlags = 0;
	FxReset(&SuperFX);
}

void S9xSetSuperFX (uint8 byte, uint16 address)
{
	switch (address)
	{
		case 0x3030:
			if ((Memory.FillRAM[0x3030] ^ byte) & FLG_G)
			{
				Memory.FillRAM[0x3030] = byte;
				if (byte & FLG_G)
				{
					if (!SuperFX.oneLineDone)
					{
						S9xSuperFXExec();
						SuperFX.oneLineDone = TRUE;
					}
				}
				else
            {
					//FxFlushCache();
	            GSU.vCacheFlags = 0;
	            GSU.vCacheBaseReg = 0;
	            GSU.bCacheActive = FALSE;
	            //GSU.vPipe = 0x1;
            }
			}
			else
				Memory.FillRAM[0x3030] = byte;

			break;

		case 0x3031:
			Memory.FillRAM[0x3031] = byte;
			break;

		case 0x3033:
			Memory.FillRAM[0x3033] = byte;
			break;

		case 0x3034:
			Memory.FillRAM[0x3034] = byte & 0x7f;
			break;

		case 0x3036:
			Memory.FillRAM[0x3036] = byte & 0x7f;
			break;

		case 0x3037:
			Memory.FillRAM[0x3037] = byte;
			break;

		case 0x3038:
			Memory.FillRAM[0x3038] = byte;
			//fx_dirtySCBR();
	      GSU.vSCBRDirty = TRUE;
			break;

		case 0x3039:
			Memory.FillRAM[0x3039] = byte;
			break;

		case 0x303a:
			Memory.FillRAM[0x303a] = byte;
			break;

		case 0x303b:
			break;

		case 0x303c:
			Memory.FillRAM[0x303c] = byte;
			//fx_updateRamBank(byte);
	      GSU.vRamBankReg = (uint32) byte & (FX_RAM_BANKS - 1);
	      GSU.pvRamBank = GSU.apvRamBank[byte & 0x3];
			break;

		case 0x303f:
			Memory.FillRAM[0x303f] = byte;
			break;

		case 0x301f:
			Memory.FillRAM[0x301f] = byte;
			Memory.FillRAM[0x3000 + GSU_SFR] |= FLG_G;
			if (!SuperFX.oneLineDone)
			{
				S9xSuperFXExec();
				SuperFX.oneLineDone = TRUE;
			}

			break;

		default:
			Memory.FillRAM[address] = byte;
			if (address >= 0x3100)
         {
				//FxCacheWriteAccess(address);
	/*
	if (!GSU.bCacheActive)
	{
		uint8	v = GSU.pvCache[GSU.pvCache[vAddress & 0x1ff];
		fx_setCache();
		GSU.pvCache[GSU.pvCache[vAddress & 0x1ff] = v;
	}
	*/

	if ((address & 0x00f) == 0x00f)
		GSU.vCacheFlags |= 1 << ((address & 0x1f0) >> 4);
         }

			break;
	}
}

void S9xSuperFXExec (void)
{
	if ((Memory.FillRAM[0x3000 + GSU_SFR] & FLG_G) && (Memory.FillRAM[0x3000 + GSU_SCMR] & 0x18) == 0x18)
	{
		FxEmulate((Memory.FillRAM[0x3000 + GSU_CLSR] & 1) ? (SuperFX.speedPerLine << 1) : SuperFX.speedPerLine);

		uint16 GSUStatus = Memory.FillRAM[0x3000 + GSU_SFR] | (Memory.FillRAM[0x3000 + GSU_SFR + 1] << 8);
		if ((GSUStatus & (FLG_G | FLG_IRQ)) == FLG_IRQ)
      {
			//S9xSetIRQ(GSU_IRQ_SOURCE);
	         CPU.IRQActive |= GSU_IRQ_SOURCE;
	         CPU.IRQPending = Timings.IRQPendCount;
	         CPU.Flags |= IRQ_FLAG;

	         if (CPU.WaitingForInterrupt)
	         {
		         // Force IRQ to trigger immediately after WAI -
		         // Final Fantasy Mystic Quest crashes without this.
		         CPU.WaitingForInterrupt = FALSE;
		         Registers.PCw++;
	         }
      }
	}
}

static void FxReset (struct FxInfo_s *psFxInfo)
{
	// Clear all internal variables
	memset((uint8 *) &GSU, 0, sizeof(struct FxRegs_s));

	// Set default registers
	GSU.pvSreg = GSU.pvDreg = &R0;

	// Set RAM and ROM pointers
	GSU.pvRegisters       = psFxInfo->pvRegisters;
	GSU.nRamBanks         = psFxInfo->nRamBanks;
	GSU.pvRam             = psFxInfo->pvRam;
	GSU.nRomBanks         = psFxInfo->nRomBanks;
	GSU.pvRom             = psFxInfo->pvRom;
	GSU.vPrevScreenHeight = ~0;
	GSU.vPrevMode         = ~0;

	// The GSU can't access more than 2mb (16mbits)
	if (GSU.nRomBanks > 0x20)
		GSU.nRomBanks = 0x20;

	// Clear FxChip register space
	memset(GSU.pvRegisters, 0, 0x300);

	// Set FxChip version Number
	GSU.pvRegisters[0x3b] = 0;

	// Make ROM bank table
	for (int i = 0; i < 256; i++)
	{
		uint32	b = i & 0x7f;

		if (b >= 0x40)
		{
			if (GSU.nRomBanks > 1)
				b %= GSU.nRomBanks;
			else
				b &= 1;

			GSU.apvRomBank[i] = &GSU.pvRom[b << 16];
		}
		else
		{
			b %= GSU.nRomBanks << 1;
			GSU.apvRomBank[i] = &GSU.pvRom[(b << 16) + 0x200000];
		}
	}

	// Make RAM bank table
	for (int i = 0; i < 4; i++)
	{
		GSU.apvRamBank[i] = &GSU.pvRam[(i % GSU.nRamBanks) << 16];
		GSU.apvRomBank[0x70 + i] = GSU.apvRamBank[i];
	}

	// Start with a nop in the pipe
	GSU.vPipe = 0x01;

	// Set pointer to GSU cache
	GSU.pvCache = &GSU.pvRegisters[0x100];

	//fx_readRegisterSpace();
	static uint32	avHeight[] = { 128, 160, 192, 256 };
	static uint32	avMult[]   = {  16,  32,  32,  64 };

	uint8	*p;
	int		n;

	GSU.vErrorCode = 0;

	// Update R0-R15
	p = GSU.pvRegisters;

		GSU.avReg[0] = *p++;
		GSU.avReg[0] += ((uint32) (*p++)) << 8;

		GSU.avReg[1] = *p++;
		GSU.avReg[1] += ((uint32) (*p++)) << 8;

		GSU.avReg[2] = *p++;
		GSU.avReg[2] += ((uint32) (*p++)) << 8;

		GSU.avReg[3] = *p++;
		GSU.avReg[3] += ((uint32) (*p++)) << 8;

		GSU.avReg[4] = *p++;
		GSU.avReg[4] += ((uint32) (*p++)) << 8;

		GSU.avReg[5] = *p++;
		GSU.avReg[5] += ((uint32) (*p++)) << 8;

		GSU.avReg[6] = *p++;
		GSU.avReg[6] += ((uint32) (*p++)) << 8;

		GSU.avReg[7] = *p++;
		GSU.avReg[7] += ((uint32) (*p++)) << 8;

		GSU.avReg[8] = *p++;
		GSU.avReg[8] += ((uint32) (*p++)) << 8;

		GSU.avReg[9] = *p++;
		GSU.avReg[9] += ((uint32) (*p++)) << 8;

		GSU.avReg[10] = *p++;
		GSU.avReg[10] += ((uint32) (*p++)) << 8;

		GSU.avReg[11] = *p++;
		GSU.avReg[11] += ((uint32) (*p++)) << 8;

		GSU.avReg[12] = *p++;
		GSU.avReg[12] += ((uint32) (*p++)) << 8;

		GSU.avReg[13] = *p++;
		GSU.avReg[13] += ((uint32) (*p++)) << 8;

		GSU.avReg[14] = *p++;
		GSU.avReg[14] += ((uint32) (*p++)) << 8;

		GSU.avReg[15] = *p++;
		GSU.avReg[15] += ((uint32) (*p++)) << 8;

	// Update other registers
	p = GSU.pvRegisters;
	GSU.vStatusReg     =  (uint32) p[GSU_SFR];
	GSU.vStatusReg    |= ((uint32) p[GSU_SFR + 1]) << 8;
	GSU.vPrgBankReg    =  (uint32) p[GSU_PBR];
	GSU.vRomBankReg    =  (uint32) p[GSU_ROMBR];
	GSU.vRamBankReg    = ((uint32) p[GSU_RAMBR]) & (FX_RAM_BANKS - 1);
	GSU.vCacheBaseReg  =  (uint32) p[GSU_CBR];
	GSU.vCacheBaseReg |= ((uint32) p[GSU_CBR + 1]) << 8;

	// Update status register variables
	GSU.vZero     = !(GSU.vStatusReg & FLG_Z);
	GSU.vSign     =  (GSU.vStatusReg & FLG_S)  << 12;
	GSU.vOverflow =  (GSU.vStatusReg & FLG_OV) << 16;
	GSU.vCarry    =  (GSU.vStatusReg & FLG_CY) >> 2;

	// Set bank pointers
	GSU.pvRamBank = GSU.apvRamBank[GSU.vRamBankReg & 0x3];
	GSU.pvRomBank = GSU.apvRomBank[GSU.vRomBankReg];
	GSU.pvPrgBank = GSU.apvRomBank[GSU.vPrgBankReg];

	// Set screen pointers
	GSU.pvScreenBase = &GSU.pvRam[USEX8(p[GSU_SCBR]) << 10];
	n  =  (int) (!!(p[GSU_SCMR] & 0x04));
	n |= ((int) (!!(p[GSU_SCMR] & 0x20))) << 1;
	GSU.vScreenHeight = GSU.vScreenRealHeight = avHeight[n];
	GSU.vMode = p[GSU_SCMR] & 0x03;

	if (n == 3)
		GSU.vScreenSize = 32768;
	else
		GSU.vScreenSize = (mul<32>((GSU.vScreenHeight >> 3))) * avMult[GSU.vMode];

	if (GSU.vPlotOptionReg & 0x10) // OBJ Mode (for drawing into sprites)
		GSU.vScreenHeight = 256;

	if (GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks << 16))
		GSU.pvScreenBase = GSU.pvRam + (GSU.nRamBanks << 16) - GSU.vScreenSize;

	//GSU.pfPlot = fx_PlotTable[GSU.vMode];
	//GSU.pfRpix = fx_PlotTable[GSU.vMode + 5];

	fx_OpcodeTable[0x04c] = fx_PlotTable[GSU.vMode];
	fx_OpcodeTable[0x14c] = fx_PlotTable[GSU.vMode + 5];
	fx_OpcodeTable[0x24c] = fx_PlotTable[GSU.vMode];
	fx_OpcodeTable[0x34c] = fx_PlotTable[GSU.vMode + 5];

	//fx_computeScreenPointers();
   //begin of fx_computeScreenPointers()
	if (GSU.vMode != GSU.vPrevMode || GSU.vPrevScreenHeight != GSU.vScreenHeight || GSU.vSCBRDirty)
	{
		GSU.vSCBRDirty = FALSE;

		// Make a list of pointers to the start of each screen column
      uint8* pvScreenBase = GSU.pvScreenBase;
      uint32_t vmode = GSU.vMode;
      int32_t condition = vmode - 2;
      int32_t mask = (condition | -condition) >> 31;
      int32_t result = (vmode & mask) | (3 & ~mask);
      uint32_t screenheight = GSU.vScreenHeight;
      uint32_t incrementvalue = screenheight+screenheight;
      uint32_t tempvalue[32];
      vmode = result;
      vmode++;
		switch (screenheight)
		{
			case 128:
			case 160:
         case 192:
            for(int i = 0; i < 32; i++)
               tempvalue[i] = incrementvalue * i * vmode;

            GSU.apvScreen[0] = pvScreenBase;
				GSU.apvScreen[1] = pvScreenBase + mul<16>(vmode);
				GSU.apvScreen[2] = pvScreenBase + mul<32>(vmode);
				GSU.apvScreen[3] = pvScreenBase + mul<48>(vmode);
				GSU.apvScreen[4] = pvScreenBase + mul<64>(vmode);
				GSU.apvScreen[5] = pvScreenBase + mul<80>(vmode);
				GSU.apvScreen[6] = pvScreenBase + mul<96>(vmode);
				GSU.apvScreen[7] = pvScreenBase + mul<112>(vmode);
				GSU.apvScreen[8] = pvScreenBase + mul<128>(vmode);
				GSU.apvScreen[9] = pvScreenBase + mul<144>(vmode);
				GSU.apvScreen[10] = pvScreenBase + mul<160>(vmode);
				GSU.apvScreen[11] = pvScreenBase + mul<176>(vmode);
				GSU.apvScreen[12] = pvScreenBase + mul<192>(vmode);
				GSU.apvScreen[13] = pvScreenBase + mul<208>(vmode);
				GSU.apvScreen[14] = pvScreenBase + mul<224>(vmode);
				GSU.apvScreen[15] = pvScreenBase + mul<240>(vmode);
				GSU.apvScreen[16] = pvScreenBase + mul<256>(vmode);
				GSU.apvScreen[17] = pvScreenBase + mul<272>(vmode);
				GSU.apvScreen[18] = pvScreenBase + mul<288>(vmode);
				GSU.apvScreen[19] = pvScreenBase + mul<304>(vmode);
				GSU.apvScreen[20] = pvScreenBase + mul<320>(vmode);
				GSU.apvScreen[21] = pvScreenBase + mul<336>(vmode);
				GSU.apvScreen[22] = pvScreenBase + mul<352>(vmode);
				GSU.apvScreen[23] = pvScreenBase + mul<368>(vmode);
				GSU.apvScreen[24] = pvScreenBase + mul<384>(vmode);
				GSU.apvScreen[25] = pvScreenBase + mul<400>(vmode);
				GSU.apvScreen[26] = pvScreenBase + mul<416>(vmode);
				GSU.apvScreen[27] = pvScreenBase + mul<432>(vmode);
				GSU.apvScreen[28] = pvScreenBase + mul<448>(vmode);
				GSU.apvScreen[29] = pvScreenBase + mul<464>(vmode);
				GSU.apvScreen[30] = pvScreenBase + mul<480>(vmode);
				GSU.apvScreen[31] = pvScreenBase + mul<496>(vmode);
            for(int i = 0; i < 32; i++)
               GSU.x[i] = tempvalue[i];
            break;
			case 256:
            const uint32_t mul_8192 = mul<8192>(vmode);

			   GSU.apvScreen[0] = pvScreenBase;
			   GSU.apvScreen[1] = pvScreenBase + mul<256>(vmode);
			   GSU.apvScreen[2] = pvScreenBase + mul<512>(vmode);
			   GSU.apvScreen[3] = pvScreenBase + mul<768>(vmode);
			   GSU.apvScreen[4] = pvScreenBase + mul<1024>(vmode);
			   GSU.apvScreen[5] = pvScreenBase + mul<1280>(vmode);
			   GSU.apvScreen[6] = pvScreenBase + mul<1536>(vmode);
			   GSU.apvScreen[7] = pvScreenBase + mul<1792>(vmode);
			   GSU.apvScreen[8] = pvScreenBase + mul<2048>(vmode);
			   GSU.apvScreen[9] = pvScreenBase + mul<2304>(vmode);
			   GSU.apvScreen[10] = pvScreenBase + mul<2560>(vmode);
			   GSU.apvScreen[11] = pvScreenBase + mul<2816>(vmode);
			   GSU.apvScreen[12] = pvScreenBase + mul<3072>(vmode);
			   GSU.apvScreen[13] = pvScreenBase + mul<3328>(vmode);
			   GSU.apvScreen[14] = pvScreenBase + mul<3584>(vmode);
			   GSU.apvScreen[15] = pvScreenBase + mul<3840>(vmode);
            //check if correct for i = 16
			   GSU.apvScreen[16] = pvScreenBase + mul_8192;
			   GSU.apvScreen[17] = pvScreenBase + mul_8192 + mul<256>(vmode);
			   GSU.apvScreen[18] = pvScreenBase + mul_8192 + mul<512>(vmode);
			   GSU.apvScreen[19] = pvScreenBase + mul_8192 + mul<768>(vmode);
			   GSU.apvScreen[20] = pvScreenBase + mul_8192 + mul<1024>(vmode);
			   GSU.apvScreen[21] = pvScreenBase + mul_8192 + mul<1280>(vmode);
			   GSU.apvScreen[22] = pvScreenBase + mul_8192 + mul<1536>(vmode);
			   GSU.apvScreen[23] = pvScreenBase + mul_8192 + mul<1792>(vmode);
			   GSU.apvScreen[24] = pvScreenBase + mul_8192 + mul<2048>(vmode);
			   GSU.apvScreen[25] = pvScreenBase + mul_8192 + mul<2304>(vmode);
			   GSU.apvScreen[26] = pvScreenBase + mul_8192 + mul<2560>(vmode);
			   GSU.apvScreen[27] = pvScreenBase + mul_8192 + mul<2816>(vmode);
			   GSU.apvScreen[28] = pvScreenBase + mul_8192 + mul<3072>(vmode);
			   GSU.apvScreen[29] = pvScreenBase + mul_8192 + mul<3328>(vmode);
			   GSU.apvScreen[30] = pvScreenBase + mul_8192 + mul<3584>(vmode);
			   GSU.apvScreen[31] = pvScreenBase + mul_8192 + mul<3840>(vmode);

            const uint32_t mul_4096 = mul<4096>(vmode);
			   GSU.x[0] = 0; 
			   GSU.x[1] = mul<16>(vmode);
			   GSU.x[2] = mul<32>(vmode);
			   GSU.x[3] = mul<48>(vmode);
			   GSU.x[4] = mul<64>(vmode);
			   GSU.x[5] = mul<80>(vmode);
			   GSU.x[6] = mul<96>(vmode);
			   GSU.x[7] = mul<112>(vmode);
			   GSU.x[8] = mul<128>(vmode);
			   GSU.x[9] = mul<144>(vmode);
			   GSU.x[10] = mul<160>(vmode);
		      GSU.x[11] = mul<176>(vmode);
		      GSU.x[12] = mul<192>(vmode);
		      GSU.x[13] = mul<208>(vmode);
		      GSU.x[14] = mul<224>(vmode);
		      GSU.x[15] = mul<240>(vmode);
            //check if correct for i = 16
		      GSU.x[16] = mul_4096;
            GSU.x[17] = mul_4096 + mul<16>(vmode);
            GSU.x[18] = mul_4096 + mul<32>(vmode);
            GSU.x[19] = mul_4096 + mul<48>(vmode);
            GSU.x[20] = mul_4096 + mul<64>(vmode);
            GSU.x[21] = mul_4096 + mul<80>(vmode);
            GSU.x[22] = mul_4096 + mul<96>(vmode);
            GSU.x[23] = mul_4096 + mul<112>(vmode);
            GSU.x[24] = mul_4096 + mul<128>(vmode);
            GSU.x[25] = mul_4096 + mul<144>(vmode);
            GSU.x[26] = mul_4096 + mul<160>(vmode);
            GSU.x[27] = mul_4096 + mul<176>(vmode);
            GSU.x[28] = mul_4096 + mul<192>(vmode);
            GSU.x[29] = mul_4096 + mul<208>(vmode);
            GSU.x[30] = mul_4096 + mul<224>(vmode);
            GSU.x[31] = mul_4096 + mul<240>(vmode);
            break;
		}

		GSU.vPrevMode = GSU.vMode;
		GSU.vPrevScreenHeight = GSU.vScreenHeight;
	}
   //end of fx_computeScreenPointers()

	//fx_backupCache();
}

/*
static void fx_readRegisterSpace (void)
{
	static uint32	avHeight[] = { 128, 160, 192, 256 };
	static uint32	avMult[]   = {  16,  32,  32,  64 };

	uint8	*p;
	int		n;

	GSU.vErrorCode = 0;

	// Update R0-R15
	p = GSU.pvRegisters;
	for (int i = 0; i < 16; i++)
	{
		GSU.avReg[i] = *p++;
		GSU.avReg[i] += ((uint32) (*p++)) << 8;
	}

	// Update other registers
	p = GSU.pvRegisters;
	GSU.vStatusReg     =  (uint32) p[GSU_SFR];
	GSU.vStatusReg    |= ((uint32) p[GSU_SFR + 1]) << 8;
	GSU.vPrgBankReg    =  (uint32) p[GSU_PBR];
	GSU.vRomBankReg    =  (uint32) p[GSU_ROMBR];
	GSU.vRamBankReg    = ((uint32) p[GSU_RAMBR]) & (FX_RAM_BANKS - 1);
	GSU.vCacheBaseReg  =  (uint32) p[GSU_CBR];
	GSU.vCacheBaseReg |= ((uint32) p[GSU_CBR + 1]) << 8;

	// Update status register variables
	GSU.vZero     = !(GSU.vStatusReg & FLG_Z);
	GSU.vSign     =  (GSU.vStatusReg & FLG_S)  << 12;
	GSU.vOverflow =  (GSU.vStatusReg & FLG_OV) << 16;
	GSU.vCarry    =  (GSU.vStatusReg & FLG_CY) >> 2;

	// Set bank pointers
	GSU.pvRamBank = GSU.apvRamBank[GSU.vRamBankReg & 0x3];
	GSU.pvRomBank = GSU.apvRomBank[GSU.vRomBankReg];
	GSU.pvPrgBank = GSU.apvRomBank[GSU.vPrgBankReg];

	// Set screen pointers
	GSU.pvScreenBase = &GSU.pvRam[USEX8(p[GSU_SCBR]) << 10];
	n  =  (int) (!!(p[GSU_SCMR] & 0x04));
	n |= ((int) (!!(p[GSU_SCMR] & 0x20))) << 1;
	GSU.vScreenHeight = GSU.vScreenRealHeight = avHeight[n];
	GSU.vMode = p[GSU_SCMR] & 0x03;

	if (n == 3)
		GSU.vScreenSize = 32768;
	else
		GSU.vScreenSize = (mul<32>((GSU.vScreenHeight >> 3))) * avMult[GSU.vMode];

	if (GSU.vPlotOptionReg & 0x10) // OBJ Mode (for drawing into sprites)
		GSU.vScreenHeight = 256;

	if (GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks << 16))
		GSU.pvScreenBase = GSU.pvRam + (GSU.nRamBanks << 16) - GSU.vScreenSize;

	GSU.pfPlot = fx_PlotTable[GSU.vMode];
	GSU.pfRpix = fx_PlotTable[GSU.vMode + 5];

	fx_OpcodeTable[0x04c] = GSU.pfPlot;
	fx_OpcodeTable[0x14c] = GSU.pfRpix;
	fx_OpcodeTable[0x24c] = GSU.pfPlot;
	fx_OpcodeTable[0x34c] = GSU.pfRpix;

	fx_computeScreenPointers();

	//fx_backupCache();
}
*/

/*
static void fx_writeRegisterSpace (void)
{
	uint8	*p;

	p = GSU.pvRegisters;
	for (int i = 0; i < 16; i++)
	{
		*p++ = (uint8)  GSU.avReg[i];
		*p++ = (uint8) (GSU.avReg[i] >> 8);
	}

	// Update status register
	if (USEX16(GSU.vZero) == 0)
		SF(Z);
	else
		CF(Z);

	if (GSU.vSign & 0x8000)
		SF(S);
	else
		CF(S);

	if (GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000)
		SF(OV);
	else
		CF(OV);

	if (GSU.vCarry)
		SF(CY);
	else
		CF(CY);

	p = GSU.pvRegisters;
	p[GSU_SFR]     = (uint8)  GSU.vStatusReg;
	p[GSU_SFR + 1] = (uint8) (GSU.vStatusReg >> 8);
	p[GSU_PBR]     = (uint8)  GSU.vPrgBankReg;
	p[GSU_ROMBR]   = (uint8)  GSU.vRomBankReg;
	p[GSU_RAMBR]   = (uint8)  GSU.vRamBankReg;
	p[GSU_CBR]     = (uint8)  GSU.vCacheBaseReg;
	p[GSU_CBR + 1] = (uint8) (GSU.vCacheBaseReg >> 8);

	//fx_restoreCache();
}
*/

// Update RamBankReg and RAM Bank pointer
/*
static void fx_updateRamBank (uint8 byte)
{
	// Update BankReg and Bank pointer
	GSU.vRamBankReg = (uint32) byte & (FX_RAM_BANKS - 1);
	GSU.pvRamBank = GSU.apvRamBank[byte & 0x3];
}
*/

// SCBR write seen. We need to update our cached screen pointers
/*
static void fx_dirtySCBR (void)
{
	GSU.vSCBRDirty = TRUE;
}
*/

static bool8 fx_checkStartAddress (void)
{
	// Check if we start inside the cache
	if (GSU.bCacheActive && R15 >= GSU.vCacheBaseReg && R15 < (GSU.vCacheBaseReg + 512))
		return (TRUE);

	/*
	// Check if we're in an unused area
	if (GSU.vPrgBankReg < 0x40 && R15 < 0x8000)
		return (FALSE);
	*/

	if (GSU.vPrgBankReg >= 0x60 && GSU.vPrgBankReg <= 0x6f)
		return (FALSE);

	if (GSU.vPrgBankReg >= 0x74)
		return (FALSE);

	// Check if we're in RAM and the RAN flag is not set
	if (GSU.vPrgBankReg >= 0x70 && GSU.vPrgBankReg <= 0x73 && !(SCMR & (1 << 3)))
		return (FALSE);

	// If not, we're in ROM, so check if the RON flag is set
	if (!(SCMR & (1 << 4)))
		return (FALSE);

	return (TRUE);
}

// Execute until the next stop instruction
static uint32 FxEmulate (uint32 nInstructions)
{
	uint32	vCount;

	// Read registers and initialize GSU session
	//fx_readRegisterSpace();
	static uint32	avHeight[] = { 128, 160, 192, 256 };
	static uint32	avMult[]   = {  16,  32,  32,  64 };

	uint8	*p;
	int		n;

	GSU.vErrorCode = 0;

	// Update R0-R15
	p = GSU.pvRegisters;

		GSU.avReg[0] = *p++;
		GSU.avReg[0] += ((uint32) (*p++)) << 8;

		GSU.avReg[1] = *p++;
		GSU.avReg[1] += ((uint32) (*p++)) << 8;

		GSU.avReg[2] = *p++;
		GSU.avReg[2] += ((uint32) (*p++)) << 8;

		GSU.avReg[3] = *p++;
		GSU.avReg[3] += ((uint32) (*p++)) << 8;

		GSU.avReg[4] = *p++;
		GSU.avReg[4] += ((uint32) (*p++)) << 8;

		GSU.avReg[5] = *p++;
		GSU.avReg[5] += ((uint32) (*p++)) << 8;

		GSU.avReg[6] = *p++;
		GSU.avReg[6] += ((uint32) (*p++)) << 8;

		GSU.avReg[7] = *p++;
		GSU.avReg[7] += ((uint32) (*p++)) << 8;

		GSU.avReg[8] = *p++;
		GSU.avReg[8] += ((uint32) (*p++)) << 8;

		GSU.avReg[9] = *p++;
		GSU.avReg[9] += ((uint32) (*p++)) << 8;

		GSU.avReg[10] = *p++;
		GSU.avReg[10] += ((uint32) (*p++)) << 8;

		GSU.avReg[11] = *p++;
		GSU.avReg[11] += ((uint32) (*p++)) << 8;

		GSU.avReg[12] = *p++;
		GSU.avReg[12] += ((uint32) (*p++)) << 8;

		GSU.avReg[13] = *p++;
		GSU.avReg[13] += ((uint32) (*p++)) << 8;

		GSU.avReg[14] = *p++;
		GSU.avReg[14] += ((uint32) (*p++)) << 8;

		GSU.avReg[15] = *p++;
		GSU.avReg[15] += ((uint32) (*p++)) << 8;

	// Update other registers
	p = GSU.pvRegisters;
	GSU.vStatusReg     =  (uint32) p[GSU_SFR];
	GSU.vStatusReg    |= ((uint32) p[GSU_SFR + 1]) << 8;
	GSU.vPrgBankReg    =  (uint32) p[GSU_PBR];
	GSU.vRomBankReg    =  (uint32) p[GSU_ROMBR];
	GSU.vRamBankReg    = ((uint32) p[GSU_RAMBR]) & (FX_RAM_BANKS - 1);
	GSU.vCacheBaseReg  =  (uint32) p[GSU_CBR];
	GSU.vCacheBaseReg |= ((uint32) p[GSU_CBR + 1]) << 8;

	// Update status register variables
	GSU.vZero     = !(GSU.vStatusReg & FLG_Z);
	GSU.vSign     =  (GSU.vStatusReg & FLG_S)  << 12;
	GSU.vOverflow =  (GSU.vStatusReg & FLG_OV) << 16;
	GSU.vCarry    =  (GSU.vStatusReg & FLG_CY) >> 2;

	// Set bank pointers
	GSU.pvRamBank = GSU.apvRamBank[GSU.vRamBankReg & 0x3];
	GSU.pvRomBank = GSU.apvRomBank[GSU.vRomBankReg];
	GSU.pvPrgBank = GSU.apvRomBank[GSU.vPrgBankReg];

	// Set screen pointers
	GSU.pvScreenBase = &GSU.pvRam[USEX8(p[GSU_SCBR]) << 10];
	n  =  (int) (!!(p[GSU_SCMR] & 0x04));
	n |= ((int) (!!(p[GSU_SCMR] & 0x20))) << 1;
	GSU.vScreenHeight = GSU.vScreenRealHeight = avHeight[n];
	GSU.vMode = p[GSU_SCMR] & 0x03;

   /*
	if (n == 3)
		GSU.vScreenSize = 32768;
	else
		GSU.vScreenSize = (mul<32>((GSU.vScreenHeight >> 3))) * avMult[GSU.vMode];
   */
   ISEL_IF32(GSU.vScreenSize, n == 3, 32768, (mul<32>((GSU.vScreenHeight >> 3))) * avMult[GSU.vMode]);

   int32_t condition1 = GSU.vPlotOptionReg & 0x10;
   int32_t mask1 = ((condition1) | -(condition1)) >> 31;
   int32_t result1 = (((256) & mask1) | ((GSU.vScreenHeight) & ~mask1));
   GSU.vScreenHeight = result1;
/*
	if (GSU.vPlotOptionReg & 0x10) // OBJ Mode (for drawing into sprites)
		GSU.vScreenHeight = 256;
*/

	if (GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks << 16))
		GSU.pvScreenBase = GSU.pvRam + (GSU.nRamBanks << 16) - GSU.vScreenSize;

	//GSU.pfPlot = fx_PlotTable[GSU.vMode];
	//GSU.pfRpix = fx_PlotTable[GSU.vMode + 5];

	fx_OpcodeTable[0x04c] = fx_PlotTable[GSU.vMode];
	fx_OpcodeTable[0x14c] = fx_PlotTable[GSU.vMode + 5];
	fx_OpcodeTable[0x24c] = fx_PlotTable[GSU.vMode];
	fx_OpcodeTable[0x34c] = fx_PlotTable[GSU.vMode + 5];

	//fx_computeScreenPointers();
   //begin of fx_computeScreenPointers()
	if (GSU.vMode != GSU.vPrevMode || GSU.vPrevScreenHeight != GSU.vScreenHeight || GSU.vSCBRDirty)
	{
		GSU.vSCBRDirty = FALSE;

		// Make a list of pointers to the start of each screen column
      uint8* pvScreenBase = GSU.pvScreenBase;
      uint32_t vmode = GSU.vMode;
      int32_t condition = vmode - 2;
      int32_t mask = (condition | -condition) >> 31;
      int32_t result = (vmode & mask) | (3 & ~mask);
      uint32_t screenheight = GSU.vScreenHeight;
      uint32_t incrementvalue = screenheight+screenheight;
      uint32_t tempvalue[32];
      vmode = result;
      vmode++;
		switch (screenheight)
		{
			case 128:
			case 160:
         case 192:
            for(int i = 0; i < 32; i++)
               tempvalue[i] = incrementvalue * i * vmode;

            GSU.apvScreen[0] = pvScreenBase;
				GSU.apvScreen[1] = pvScreenBase + mul<16>(vmode);
				GSU.apvScreen[2] = pvScreenBase + mul<32>(vmode);
				GSU.apvScreen[3] = pvScreenBase + mul<48>(vmode);
				GSU.apvScreen[4] = pvScreenBase + mul<64>(vmode);
				GSU.apvScreen[5] = pvScreenBase + mul<80>(vmode);
				GSU.apvScreen[6] = pvScreenBase + mul<96>(vmode);
				GSU.apvScreen[7] = pvScreenBase + mul<112>(vmode);
				GSU.apvScreen[8] = pvScreenBase + mul<128>(vmode);
				GSU.apvScreen[9] = pvScreenBase + mul<144>(vmode);
				GSU.apvScreen[10] = pvScreenBase + mul<160>(vmode);
				GSU.apvScreen[11] = pvScreenBase + mul<176>(vmode);
				GSU.apvScreen[12] = pvScreenBase + mul<192>(vmode);
				GSU.apvScreen[13] = pvScreenBase + mul<208>(vmode);
				GSU.apvScreen[14] = pvScreenBase + mul<224>(vmode);
				GSU.apvScreen[15] = pvScreenBase + mul<240>(vmode);
				GSU.apvScreen[16] = pvScreenBase + mul<256>(vmode);
				GSU.apvScreen[17] = pvScreenBase + mul<272>(vmode);
				GSU.apvScreen[18] = pvScreenBase + mul<288>(vmode);
				GSU.apvScreen[19] = pvScreenBase + mul<304>(vmode);
				GSU.apvScreen[20] = pvScreenBase + mul<320>(vmode);
				GSU.apvScreen[21] = pvScreenBase + mul<336>(vmode);
				GSU.apvScreen[22] = pvScreenBase + mul<352>(vmode);
				GSU.apvScreen[23] = pvScreenBase + mul<368>(vmode);
				GSU.apvScreen[24] = pvScreenBase + mul<384>(vmode);
				GSU.apvScreen[25] = pvScreenBase + mul<400>(vmode);
				GSU.apvScreen[26] = pvScreenBase + mul<416>(vmode);
				GSU.apvScreen[27] = pvScreenBase + mul<432>(vmode);
				GSU.apvScreen[28] = pvScreenBase + mul<448>(vmode);
				GSU.apvScreen[29] = pvScreenBase + mul<464>(vmode);
				GSU.apvScreen[30] = pvScreenBase + mul<480>(vmode);
				GSU.apvScreen[31] = pvScreenBase + mul<496>(vmode);
            for(int i = 0; i < 32; i++)
               GSU.x[i] = tempvalue[i];
            break;
			case 256:
            const uint32_t mul_8192 = mul<8192>(vmode);

			   GSU.apvScreen[0] = pvScreenBase;
			   GSU.apvScreen[1] = pvScreenBase + mul<256>(vmode);
			   GSU.apvScreen[2] = pvScreenBase + mul<512>(vmode);
			   GSU.apvScreen[3] = pvScreenBase + mul<768>(vmode);
			   GSU.apvScreen[4] = pvScreenBase + mul<1024>(vmode);
			   GSU.apvScreen[5] = pvScreenBase + mul<1280>(vmode);
			   GSU.apvScreen[6] = pvScreenBase + mul<1536>(vmode);
			   GSU.apvScreen[7] = pvScreenBase + mul<1792>(vmode);
			   GSU.apvScreen[8] = pvScreenBase + mul<2048>(vmode);
			   GSU.apvScreen[9] = pvScreenBase + mul<2304>(vmode);
			   GSU.apvScreen[10] = pvScreenBase + mul<2560>(vmode);
			   GSU.apvScreen[11] = pvScreenBase + mul<2816>(vmode);
			   GSU.apvScreen[12] = pvScreenBase + mul<3072>(vmode);
			   GSU.apvScreen[13] = pvScreenBase + mul<3328>(vmode);
			   GSU.apvScreen[14] = pvScreenBase + mul<3584>(vmode);
			   GSU.apvScreen[15] = pvScreenBase + mul<3840>(vmode);
            //check if correct for i = 16
			   GSU.apvScreen[16] = pvScreenBase + mul_8192;
			   GSU.apvScreen[17] = pvScreenBase + mul_8192 + mul<256>(vmode);
			   GSU.apvScreen[18] = pvScreenBase + mul_8192 + mul<512>(vmode);
			   GSU.apvScreen[19] = pvScreenBase + mul_8192 + mul<768>(vmode);
			   GSU.apvScreen[20] = pvScreenBase + mul_8192 + mul<1024>(vmode);
			   GSU.apvScreen[21] = pvScreenBase + mul_8192 + mul<1280>(vmode);
			   GSU.apvScreen[22] = pvScreenBase + mul_8192 + mul<1536>(vmode);
			   GSU.apvScreen[23] = pvScreenBase + mul_8192 + mul<1792>(vmode);
			   GSU.apvScreen[24] = pvScreenBase + mul_8192 + mul<2048>(vmode);
			   GSU.apvScreen[25] = pvScreenBase + mul_8192 + mul<2304>(vmode);
			   GSU.apvScreen[26] = pvScreenBase + mul_8192 + mul<2560>(vmode);
			   GSU.apvScreen[27] = pvScreenBase + mul_8192 + mul<2816>(vmode);
			   GSU.apvScreen[28] = pvScreenBase + mul_8192 + mul<3072>(vmode);
			   GSU.apvScreen[29] = pvScreenBase + mul_8192 + mul<3328>(vmode);
			   GSU.apvScreen[30] = pvScreenBase + mul_8192 + mul<3584>(vmode);
			   GSU.apvScreen[31] = pvScreenBase + mul_8192 + mul<3840>(vmode);

            const uint32_t mul_4096 = mul<4096>(vmode);
			   GSU.x[0] = 0; 
			   GSU.x[1] = mul<16>(vmode);
			   GSU.x[2] = mul<32>(vmode);
			   GSU.x[3] = mul<48>(vmode);
			   GSU.x[4] = mul<64>(vmode);
			   GSU.x[5] = mul<80>(vmode);
			   GSU.x[6] = mul<96>(vmode);
			   GSU.x[7] = mul<112>(vmode);
			   GSU.x[8] = mul<128>(vmode);
			   GSU.x[9] = mul<144>(vmode);
			   GSU.x[10] = mul<160>(vmode);
		      GSU.x[11] = mul<176>(vmode);
		      GSU.x[12] = mul<192>(vmode);
		      GSU.x[13] = mul<208>(vmode);
		      GSU.x[14] = mul<224>(vmode);
		      GSU.x[15] = mul<240>(vmode);
            //check if correct for i = 16
		      GSU.x[16] = mul_4096;
            GSU.x[17] = mul_4096 + mul<16>(vmode);
            GSU.x[18] = mul_4096 + mul<32>(vmode);
            GSU.x[19] = mul_4096 + mul<48>(vmode);
            GSU.x[20] = mul_4096 + mul<64>(vmode);
            GSU.x[21] = mul_4096 + mul<80>(vmode);
            GSU.x[22] = mul_4096 + mul<96>(vmode);
            GSU.x[23] = mul_4096 + mul<112>(vmode);
            GSU.x[24] = mul_4096 + mul<128>(vmode);
            GSU.x[25] = mul_4096 + mul<144>(vmode);
            GSU.x[26] = mul_4096 + mul<160>(vmode);
            GSU.x[27] = mul_4096 + mul<176>(vmode);
            GSU.x[28] = mul_4096 + mul<192>(vmode);
            GSU.x[29] = mul_4096 + mul<208>(vmode);
            GSU.x[30] = mul_4096 + mul<224>(vmode);
            GSU.x[31] = mul_4096 + mul<240>(vmode);
            break;
		}


		GSU.vPrevMode = GSU.vMode;
		GSU.vPrevScreenHeight = GSU.vScreenHeight;
	}
   //end of fx_computeScreenPointers()

	//fx_backupCache();
   //end of fx_readRegisterSpace

	// Check if the start address is valid
	if (!fx_checkStartAddress())
	{
		CF(G);
		//fx_writeRegisterSpace();
   //begin of fx_writeRegisterSpace()
	uint8	*p;

	p = GSU.pvRegisters;
   /*
	for (int i = 0; i < 16; i++)
	{
		*p++ = (uint8)  GSU.avReg[i];
		*p++ = (uint8) (GSU.avReg[i] >> 8);
	}
   */

		*p++ = (uint8)  GSU.avReg[0];
		*p++ = (uint8) (GSU.avReg[0] >> 8);

		*p++ = (uint8)  GSU.avReg[1];
		*p++ = (uint8) (GSU.avReg[1] >> 8);

		*p++ = (uint8)  GSU.avReg[2];
		*p++ = (uint8) (GSU.avReg[2] >> 8);

		*p++ = (uint8)  GSU.avReg[3];
		*p++ = (uint8) (GSU.avReg[3] >> 8);

		*p++ = (uint8)  GSU.avReg[4];
		*p++ = (uint8) (GSU.avReg[4] >> 8);

		*p++ = (uint8)  GSU.avReg[5];
		*p++ = (uint8) (GSU.avReg[5] >> 8);

		*p++ = (uint8)  GSU.avReg[6];
		*p++ = (uint8) (GSU.avReg[6] >> 8);

		*p++ = (uint8)  GSU.avReg[7];
		*p++ = (uint8) (GSU.avReg[7] >> 8);

		*p++ = (uint8)  GSU.avReg[8];
		*p++ = (uint8) (GSU.avReg[8] >> 8);

		*p++ = (uint8)  GSU.avReg[9];
		*p++ = (uint8) (GSU.avReg[9] >> 8);

		*p++ = (uint8)  GSU.avReg[10];
		*p++ = (uint8) (GSU.avReg[10] >> 8);

		*p++ = (uint8)  GSU.avReg[11];
		*p++ = (uint8) (GSU.avReg[11] >> 8);

		*p++ = (uint8)  GSU.avReg[12];
		*p++ = (uint8) (GSU.avReg[12] >> 8);

		*p++ = (uint8)  GSU.avReg[13];
		*p++ = (uint8) (GSU.avReg[13] >> 8);

		*p++ = (uint8)  GSU.avReg[14];
		*p++ = (uint8) (GSU.avReg[14] >> 8);

		*p++ = (uint8)  GSU.avReg[15];
		*p++ = (uint8) (GSU.avReg[15] >> 8);

	// Update status register
/*
#define SF(a)			(GSU.vStatusReg |=  FLG_##a)
#define CF(a)			(GSU.vStatusReg &= ~FLG_##a)
*/

   int32_t condition = (USEX16(GSU.vZero) == 0);
   int32_t mask = ((condition) | -(condition)) >> 31;
   int32_t result = ((GSU.vStatusReg | FLG_Z) & mask) | ((GSU.vStatusReg & ~FLG_Z) & ~mask);
   GSU.vStatusReg = result;
   //ISEL_IF32(GSU.vStatusReg,USEX16(GSU.vZero) == 0,GSU.vStatusReg | FLG_Z,GSU.vStatusReg & ~FLG_Z); 
   //GSU.vStatusReg = isel_if32(USEX16(GSU.vZero) == 0,GSU.vStatusReg | FLG_Z,GSU.vStatusReg & ~FLG_Z);

/*

	if (USEX16(GSU.vZero) == 0)
		SF(Z);
	else
		CF(Z);
*/

   //ISEL_IF32(GSU.vStatusReg, GSU.vSign & 0x8000,GSU.vStatusReg | FLG_S,GSU.vStatusReg & ~FLG_S);
   condition = GSU.vSign & 0x8000;
   mask = (condition | -condition) >> 31;
   result = ((GSU.vStatusReg | FLG_S) & mask) | ((GSU.vStatusReg & ~FLG_S) & ~mask);
   GSU.vStatusReg = result;
/*
	if (GSU.vSign & 0x8000)
		SF(S);
	else
		CF(S);
*/

   condition = GSU.vOverflow >= 0x8000;
   mask = (condition | -condition) >> 31;
   result = ((GSU.vStatusReg | FLG_OV) & mask) | ((GSU.vStatusReg & ~FLG_OV) & ~mask);
   GSU.vStatusReg = result;
   //ISEL_IF32(GSU.vStatusReg,GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000,GSU.vStatusReg | FLG_OV,GSU.vStatusReg & ~FLG_OV);
/*
	if (GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000)
		SF(OV);
	else
		CF(OV);
*/

   condition = GSU.vCarry;
   mask = (condition | -condition) >> 31;
   result = ((GSU.vStatusReg | FLG_CY) & mask) | ((GSU.vStatusReg & ~FLG_CY) & ~mask);
   GSU.vStatusReg = result;
   //ISEL_IF32(GSU.vStatusReg,GSU.vCarry,GSU.vStatusReg | FLG_CY, GSU.vStatusReg & ~FLG_CY);
/*
	if (GSU.vCarry)
		SF(CY);
	else
		CF(CY);
*/

	p = GSU.pvRegisters;
	p[GSU_SFR]     = (uint8)  GSU.vStatusReg;
	p[GSU_SFR + 1] = (uint8) (GSU.vStatusReg >> 8);
	p[GSU_PBR]     = (uint8)  GSU.vPrgBankReg;
	p[GSU_ROMBR]   = (uint8)  GSU.vRomBankReg;
	p[GSU_RAMBR]   = (uint8)  GSU.vRamBankReg;
	p[GSU_CBR]     = (uint8)  GSU.vCacheBaseReg;
	p[GSU_CBR + 1] = (uint8) (GSU.vCacheBaseReg >> 8);

	//fx_restoreCache();
//end of fx_writeRegisterSpace()

		/*
		GSU.vIllegalAddress = (GSU.vPrgBankReg << 24) | R15;
		return (FX_ERROR_ILLEGAL_ADDRESS);
		*/

		return (0);
	}

	// Execute GSU session
	CF(IRQ);

	/*
	if (GSU.bBreakPoint)
		vCount = fx_run_to_breakpoint(nInstructions);
	else
	*/

#if 1
   //hardcode fx_run here
   GSU.vCounter = nInstructions;
   READR14;
   while(TF(G) && (GSU.vCounter-- > 0))
      FX_STEP;

	vCount = nInstructions - GSU.vInstCount;
#endif

	// Store GSU registers
	//fx_writeRegisterSpace();
   //begin of fx_writeRegisterSpace()

	p = GSU.pvRegisters;
   /*
	for (int i = 0; i < 16; i++)
	{
		*p++ = (uint8)  GSU.avReg[i];
		*p++ = (uint8) (GSU.avReg[i] >> 8);
	}
   */

		*p++ = (uint8)  GSU.avReg[0];
		*p++ = (uint8) (GSU.avReg[0] >> 8);

		*p++ = (uint8)  GSU.avReg[1];
		*p++ = (uint8) (GSU.avReg[1] >> 8);

		*p++ = (uint8)  GSU.avReg[2];
		*p++ = (uint8) (GSU.avReg[2] >> 8);

		*p++ = (uint8)  GSU.avReg[3];
		*p++ = (uint8) (GSU.avReg[3] >> 8);

		*p++ = (uint8)  GSU.avReg[4];
		*p++ = (uint8) (GSU.avReg[4] >> 8);

		*p++ = (uint8)  GSU.avReg[5];
		*p++ = (uint8) (GSU.avReg[5] >> 8);

		*p++ = (uint8)  GSU.avReg[6];
		*p++ = (uint8) (GSU.avReg[6] >> 8);

		*p++ = (uint8)  GSU.avReg[7];
		*p++ = (uint8) (GSU.avReg[7] >> 8);

		*p++ = (uint8)  GSU.avReg[8];
		*p++ = (uint8) (GSU.avReg[8] >> 8);

		*p++ = (uint8)  GSU.avReg[9];
		*p++ = (uint8) (GSU.avReg[9] >> 8);

		*p++ = (uint8)  GSU.avReg[10];
		*p++ = (uint8) (GSU.avReg[10] >> 8);

		*p++ = (uint8)  GSU.avReg[11];
		*p++ = (uint8) (GSU.avReg[11] >> 8);

		*p++ = (uint8)  GSU.avReg[12];
		*p++ = (uint8) (GSU.avReg[12] >> 8);

		*p++ = (uint8)  GSU.avReg[13];
		*p++ = (uint8) (GSU.avReg[13] >> 8);

		*p++ = (uint8)  GSU.avReg[14];
		*p++ = (uint8) (GSU.avReg[14] >> 8);

		*p++ = (uint8)  GSU.avReg[15];
		*p++ = (uint8) (GSU.avReg[15] >> 8);

	// Update status register
   //GSU.vStatusReg = isel_if32(USEX16(GSU.vZero) == 0,GSU.vStatusReg | FLG_Z,GSU.vStatusReg & ~FLG_Z);

   int32_t condition_vstatusreg = (USEX16(GSU.vZero) == 0);
   int32_t mask_vstatusreg = ((condition_vstatusreg) | -(condition_vstatusreg)) >> 31;
   int32_t result_vstatusreg = ((GSU.vStatusReg | FLG_Z) & mask_vstatusreg) | ((GSU.vStatusReg & ~FLG_Z) & ~mask_vstatusreg);
   GSU.vStatusReg = result_vstatusreg;

   /*
	if (USEX16(GSU.vZero) == 0)
		SF(Z);
	else
		CF(Z);
   */
/*
#define SF(a)			(GSU.vStatusReg |=  FLG_##a)
#define CF(a)			(GSU.vStatusReg &= ~FLG_##a)
*/

   int32_t condition = GSU.vSign & 0x8000;
   int32_t mask = (condition | -condition) >> 31;
   int32_t result = ((GSU.vStatusReg | FLG_S) & mask) | ((GSU.vStatusReg & ~FLG_S) & ~mask);
   GSU.vStatusReg = result;
   
/*
	if (GSU.vSign & 0x8000)
		SF(S);
	else
		CF(S);
*/

   condition = GSU.vOverflow >= 0x8000;
   mask = (condition | -condition) >> 31;
   result = ((GSU.vStatusReg | FLG_OV) & mask) | ((GSU.vStatusReg & ~FLG_OV) & ~mask);
   GSU.vStatusReg = result;

/*
	if (GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000)
		SF(OV);
	else
		CF(OV);
*/

   condition = GSU.vCarry;
   mask = (condition | -condition) >> 31;
   result = ((GSU.vStatusReg | FLG_CY) & mask) | ((GSU.vStatusReg & ~FLG_CY) & ~mask);
   GSU.vStatusReg = result;

/*
	if (GSU.vCarry)
		SF(CY);
	else
		CF(CY);
*/

	p = GSU.pvRegisters;
	p[GSU_SFR]     = (uint8)  GSU.vStatusReg;
	p[GSU_SFR + 1] = (uint8) (GSU.vStatusReg >> 8);
	p[GSU_PBR]     = (uint8)  GSU.vPrgBankReg;
	p[GSU_ROMBR]   = (uint8)  GSU.vRomBankReg;
	p[GSU_RAMBR]   = (uint8)  GSU.vRamBankReg;
	p[GSU_CBR]     = (uint8)  GSU.vCacheBaseReg;
	p[GSU_CBR + 1] = (uint8) (GSU.vCacheBaseReg >> 8);

	//fx_restoreCache();
//end of fx_writeRegisterSpace()

	// Check for error code
   int32_t errorcode;
   ISEL_IF32(errorcode, GSU.vErrorCode,GSU.vErrorCode,vCount);
   /*
	if (GSU.vErrorCode)
		return (GSU.vErrorCode);
	else
		return (vCount);
   */
   return errorcode;
}

/*
void fx_computeScreenPointers (void)
{
	if (GSU.vMode != GSU.vPrevMode || GSU.vPrevScreenHeight != GSU.vScreenHeight || GSU.vSCBRDirty)
	{
		GSU.vSCBRDirty = FALSE;

		// Make a list of pointers to the start of each screen column
		switch (GSU.vScreenHeight)
		{
			case 128:
				switch (GSU.vMode)
				{
					case 0:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
							GSU.x[i] = i <<  8;
						}

						break;

					case 1:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
							GSU.x[i] = i <<  9;
						}

						break;

					case 2:
					case 3:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
							GSU.x[i] = i << 10;
						}

						break;
				}

				break;

			case 160:
				switch (GSU.vMode)
				{
					case 0:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
							GSU.x[i] = (i <<  8) + (i << 6);
						}

						break;

					case 1:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
							GSU.x[i] = (i <<  9) + (i << 7);
						}

						break;

					case 2:
					case 3:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
							GSU.x[i] = (i << 10) + (i << 8);
						}

						break;
				}

				break;

			case 192:
				switch (GSU.vMode)
				{
					case 0:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 4);
							GSU.x[i] = (i <<  8) + (i << 7);
						}

						break;

					case 1:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 5);
							GSU.x[i] = (i <<  9) + (i << 8);
						}

						break;

					case 2:
					case 3:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + (i << 6);
							GSU.x[i] = (i << 10) + (i << 9);
						}

						break;
				}

				break;

			case 256:
				switch (GSU.vMode)
				{
					case 0:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + ((i & 0x10) <<  9) + ((i & 0xf) <<  8);
							GSU.x[i] = ((i & 0x10) <<  8) + ((i & 0xf) << 4);
						}

						break;

					case 1:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + ((i & 0x10) << 10) + ((i & 0xf) <<  9);
							GSU.x[i] = ((i & 0x10) <<  9) + ((i & 0xf) << 5);
						}

						break;

					case 2:
					case 3:
						for (int i = 0; i < 32; i++)
						{
							GSU.apvScreen[i] = GSU.pvScreenBase + ((i & 0x10) << 11) + ((i & 0xf) << 10);
							GSU.x[i] = ((i & 0x10) << 10) + ((i & 0xf) << 6);
						}

						break;
				}

				break;
		}

		GSU.vPrevMode = GSU.vMode;
		GSU.vPrevScreenHeight = GSU.vScreenHeight;
	}
}
*/

// Write access to the cache
#if 0
static void FxCacheWriteAccess (uint16 vAddress)
{
	/*
	if (!GSU.bCacheActive)
	{
		uint8	v = GSU.pvCache[GSU.pvCache[vAddress & 0x1ff];
		fx_setCache();
		GSU.pvCache[GSU.pvCache[vAddress & 0x1ff] = v;
	}
	*/

	if ((vAddress & 0x00f) == 0x00f)
		GSU.vCacheFlags |= 1 << ((vAddress & 0x1f0) >> 4);
}
#endif

/*
static void FxFlushCache (void)
{
	GSU.vCacheFlags = 0;
	GSU.vCacheBaseReg = 0;
	GSU.bCacheActive = FALSE;
	//GSU.vPipe = 0x1;
}
*/

/*
void fx_flushCache (void)
{
	//fx_restoreCache();
	GSU.vCacheFlags = 0;
	GSU.bCacheActive = FALSE;
}
*/

/*
static void fx_setCache (void)
{
	uint32	c;

	GSU.bCacheActive = TRUE;
	GSU.pvRegisters[0x3e] &= 0xf0;

	c  =  (uint32) GSU.pvRegisters[0x3e];
	c |= ((uint32) GSU.pvRegisters[0x3f]) << 8;
	if (c == GSU.vCacheBaseReg)
		return;

	GSU.vCacheBaseReg = c;
	GSU.vCacheFlags = 0;

	if (c < (0x10000 - 512))
	{
		const uint8	*t = &ROM(c);
		memcpy(GSU.pvCache, t, 512);
	}
	else
	{
		const uint8	*t1, *t2;
		uint32		i = 0x10000 - c;

		t1 = &ROM(c);
		t2 = &ROM(0);
		memcpy(GSU.pvCache, t1, i);
		memcpy(&GSU.pvCache[i], t2, 512 - i);
	}
}
*/

/*
static void fx_backupCache (void)
{
	uint32	v = GSU.vCacheFlags;
	uint32	c = USEX16(GSU.vCacheBaseReg);

	if (v)
	{
		for (int i = 0; i < 32; i++)
		{
			if (v & 1)
			{
				if (c < (0x10000 - 16))
				{
					uint8	*t = &GSU.pvPrgBank[c];
					memcpy(&GSU.avCacheBackup[i << 4], t, 16);
					memcpy(t, &GSU.pvCache[i << 4], 16);
				}
				else
				{
					uint8	*t1, *t2;
					uint32	a = 0x10000 - c;

					t1 = &GSU.pvPrgBank[c];
					t2 = &GSU.pvPrgBank[0];
					memcpy(&GSU.avCacheBackup[i << 4], t1, a);
					memcpy(t1, &GSU.pvCache[i << 4], a);
					memcpy(&GSU.avCacheBackup[(i << 4) + a], t2, 16 - a);
					memcpy(t2, &GSU.pvCache[(i << 4) + a], 16 - a);
				}
			}

			c = USEX16(c + 16);
			v >>= 1;
		}
	}
}
*/

/*
static void fx_restoreCache()
{
	uint32	v = GSU.vCacheFlags;
	uint32	c = USEX16(GSU.vCacheBaseReg);

	if (v)
	{
		for (int i = 0; i < 32; i++)
		{
			if (v & 1)
			{
				if (c < (0x10000 - 16))
				{
					uint8	*t = &GSU.pvPrgBank[c];
					memcpy(t, &GSU.avCacheBackup[i << 4], 16);
					memcpy(&GSU.pvCache[i << 4], t, 16);
				}
				else
				{
					uint8	*t1, *t2;
					uint32	a = 0x10000 - c;

					t1 = &GSU.pvPrgBank[c];
					t2 = &GSU.pvPrgBank[0];
					memcpy(t1, &GSU.avCacheBackup[i << 4], a);
					memcpy(&GSU.pvCache[i << 4], t1, a);
					memcpy(t2, &GSU.avCacheBackup[(i << 4) + a], 16 - a);
					memcpy(&GSU.pvCache[(i << 4) + a], t2, 16 - a);
				}
			}

			c = USEX16(c + 16);
			v >>= 1;
		}
	}
}
*/

// Breakpoints
/*
static void FxBreakPointSet (uint32 vAddress)
{
	GSU.bBreakPoint = TRUE;
	GSU.vBreakPoint = USEX16(vAddress);
}
*/

/*
static void FxBreakPointClear (void)
{
	GSU.bBreakPoint = FALSE;
}
*/

// Step by step execution
/*
static uint32 FxStepOver (uint32 nInstructions)
{
	uint32	vCount;

	fx_readRegisterSpace();

	if (!fx_checkStartAddress())
	{
		CF(G);
	#if 0
		GSU.vIllegalAddress = (GSU.vPrgBankReg << 24) | R15;
		return (FX_ERROR_ILLEGAL_ADDRESS);
	#else
		return (0);
	#endif
	}

	if (PIPE >= 0xf0)
		GSU.vStepPoint = USEX16(R15 + 3);
	else
	if ((PIPE >= 0x05 && PIPE <= 0x0f) || (PIPE >= 0xa0 && PIPE <= 0xaf))
		GSU.vStepPoint = USEX16(R15 + 2);
	else
		GSU.vStepPoint = USEX16(R15 + 1);

	vCount = fx_step_over(nInstructions);

	fx_writeRegisterSpace();

	if (GSU.vErrorCode)
		return (GSU.vErrorCode);
	else
		return (vCount);
}
*/

// Errors
/*
static int FxGetErrorCode (void)
{
	return (GSU.vErrorCode);
}
*/

/*
static int FxGetIllegalAddress (void)
{
	return (GSU.vIllegalAddress);
}
*/

// Access to internal registers
/*
static uint32 FxGetColorRegister (void)
{
	return (GSU.vColorReg & 0xff);
}
*/

/*
static uint32 FxGetPlotOptionRegister (void)
{
	return (GSU.vPlotOptionReg & 0x1f);
}
*/

/*
static uint32 FxGetSourceRegisterIndex (void)
{
	return (GSU.pvSreg - GSU.avReg);
}
*/

/*
static uint32 FxGetDestinationRegisterIndex (void)
{
	return (GSU.pvDreg - GSU.avReg);
}
*/

// Get the byte currently in the pipe
/*
static uint8 FxPipe (void)
{
	return (GSU.vPipe);
}
*/
