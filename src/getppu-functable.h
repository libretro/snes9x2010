static uint8 GetPPU_RAM(uint16 Address)
{
   return OpenBus;
}

static uint8 GetPPU_RAM1(uint16 Address)
{
   return PPU.OpenBus1;
}

static uint8 GetPPU_213x(uint16 Address)
{
   if (PPU.Need16x8Mulitply)
   {
      int32 r = (int32) PPU.MatrixA * (int32) (PPU.MatrixB >> 8);
      Memory.FillRAM[0x2134] = (uint8) r;
      Memory.FillRAM[0x2135] = (uint8) (r >> 8);
      Memory.FillRAM[0x2136] = (uint8) (r >> 16);
      PPU.Need16x8Mulitply = FALSE;
   }
   return (PPU.OpenBus1 = Memory.FillRAM[Address]);
}

static uint8 GetPPU_2137(uint16 Address) /* SLHV */
{
   if ((Memory.FillRAM[0x4213] & 0x80))
      S9xLatchCounters();
   if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
      PPU.GunVLatch = 1000;
   return (OpenBus);
}

static uint8 GetPPU_2138(uint16 Address) /* OAMDATAREAD */
{
   uint8	byte = 0;

   if (PPU.OAMAddr & 0x100)
   {
      if (!(PPU.OAMFlip & 1))
         byte = PPU.OAMData[(PPU.OAMAddr & 0x10f) << 1];
      else
      {
         byte = PPU.OAMData[((PPU.OAMAddr & 0x10f) << 1) + 1];
         PPU.OAMAddr = (PPU.OAMAddr + 1) & 0x1ff;
         if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
         {
            PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
            IPPU.OBJChanged = TRUE;
         }
      }
   }
   else
   {
      if (!(PPU.OAMFlip & 1))
         byte = PPU.OAMData[PPU.OAMAddr << 1];
      else
      {
         byte = PPU.OAMData[(PPU.OAMAddr << 1) + 1];
         ++PPU.OAMAddr;
         if (PPU.OAMPriorityRotation && PPU.FirstSprite != (PPU.OAMAddr >> 1))
         {
            PPU.FirstSprite = (PPU.OAMAddr & 0xfe) >> 1;
            IPPU.OBJChanged = TRUE;
         }
      }
   }

   PPU.OAMFlip ^= 1;
   return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_2139(uint16 Address) /* VMDATALREAD */
{
   uint8	byte = 0;
#ifdef CORRECT_VRAM_READS
   byte = IPPU.VRAMReadBuffer & 0xff;
   if (!PPU.VMA.High)
   {
      if (PPU.VMA.FullGraphicCount)
      {
         uint32 addr = PPU.VMA.Address;
         uint32 rem = addr & PPU.VMA.Mask1;
         uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
         IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
      }
      else
         IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

      PPU.VMA.Address += PPU.VMA.Increment;
   }
#else
   if (IPPU.FirstVRAMRead)
      byte = Memory.VRAM[(PPU.VMA.Address << 1) & 0xffff];
   else
      if (PPU.VMA.FullGraphicCount)
      {
         uint32 addr = PPU.VMA.Address - 1;
         uint32 rem = addr & PPU.VMA.Mask1;
         uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
         byte = Memory.VRAM[((address << 1) - 2) & 0xffff];
      }
      else
         byte = Memory.VRAM[((PPU.VMA.Address << 1) - 2) & 0xffff];

   if (!PPU.VMA.High)
   {
      PPU.VMA.Address += PPU.VMA.Increment;
      IPPU.FirstVRAMRead = FALSE;
   }
#endif
   return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213A(uint16 Address) /* VMDATAHREAD */
{
   uint8	byte = 0;
#ifdef CORRECT_VRAM_READS
   byte = (IPPU.VRAMReadBuffer >> 8) & 0xff;
   if (PPU.VMA.High)
   {
      if (PPU.VMA.FullGraphicCount)
      {
         uint32 addr = PPU.VMA.Address;
         uint32 rem = addr & PPU.VMA.Mask1;
         uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
         IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((address << 1) & 0xffff));
      }
      else
         IPPU.VRAMReadBuffer = READ_WORD(Memory.VRAM + ((PPU.VMA.Address << 1) & 0xffff));

      PPU.VMA.Address += PPU.VMA.Increment;
   }
#else
   if (IPPU.FirstVRAMRead)
      byte = Memory.VRAM[((PPU.VMA.Address << 1) + 1) & 0xffff];
   else
      if (PPU.VMA.FullGraphicCount)
      {
         uint32 addr = PPU.VMA.Address - 1;
         uint32 rem = addr & PPU.VMA.Mask1;
         uint32 address = (addr & ~PPU.VMA.Mask1) + (rem >> PPU.VMA.Shift) + ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3);
         byte = Memory.VRAM[((address << 1) - 1) & 0xffff];
      }
      else
         byte = Memory.VRAM[((PPU.VMA.Address << 1) - 1) & 0xffff];

   if (PPU.VMA.High)
   {
      PPU.VMA.Address += PPU.VMA.Increment;
      IPPU.FirstVRAMRead = FALSE;
   }
#endif
   return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213B(uint16 Address) /* CGDATAREAD */
{
   uint8	byte = 0;
   
   if (PPU.CGFLIPRead)
      byte = (PPU.OpenBus2 & 0x80) | ((PPU.CGDATA[PPU.CGADD++] >> 8) & 0x7f);
   else
      byte = PPU.CGDATA[PPU.CGADD] & 0xff;
   PPU.CGFLIPRead ^= 1;
   return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213C(uint16 Address) /* OPHCT */
{
   uint8 byte = 0;

   if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
   {
      if ((Memory.FillRAM[0x4213] & 0x80))
      {
         S9X_TRY_GUN_LATCH();
      }
      PPU.GunVLatch = 1000;
   }
   if (PPU.HBeamFlip)
      byte = (PPU.OpenBus2 & 0xfe) | ((PPU.HBeamPosLatched >> 8) & 0x01);
   else
      byte = (uint8) PPU.HBeamPosLatched;
   PPU.HBeamFlip ^= 1;
   return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213D(uint16 Address) /* OPVCT */
{
   uint8 byte = 0;

   if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
   {
      if ((Memory.FillRAM[0x4213] & 0x80))
      {
         S9X_TRY_GUN_LATCH();
      }
      PPU.GunVLatch = 1000;
   }
   if (PPU.VBeamFlip)
      byte = (PPU.OpenBus2 & 0xfe) | ((PPU.VBeamPosLatched >> 8) & 0x01);
   else
      byte = (uint8) PPU.VBeamPosLatched;
   PPU.VBeamFlip ^= 1;
   return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_213E(uint16 Address) /* STAT77 */
{
   uint8 byte = 0;
   FLUSH_REDRAW();
   byte = (PPU.OpenBus1 & 0x10) | PPU.RangeTimeOver | MAX_5C77_VERSION;
   return (PPU.OpenBus1 = byte);
}

static uint8 GetPPU_213F(uint16 Address) /* STAT78 */
{
   uint8 byte = 0;
   if (CPU.V_Counter >  PPU.GunVLatch || (CPU.V_Counter == PPU.GunVLatch && CPU.Cycles >= PPU.GunHLatch * ONE_DOT_CYCLE))
   {
      if ((Memory.FillRAM[0x4213] & 0x80))
      {
         S9X_TRY_GUN_LATCH();
      }
      PPU.GunVLatch = 1000;
   }
   PPU.VBeamFlip = PPU.HBeamFlip = 0;
   byte = (PPU.OpenBus2 & 0x20) | (Memory.FillRAM[0x213f] & 0xc0) | (Settings.PAL ? 0x10 : 0) | MAX_5C78_VERSION;
   Memory.FillRAM[0x213f] &= ~0x40;
   return (PPU.OpenBus2 = byte);
}

static uint8 GetPPU_APUR (uint16 Address) /* APUIO0, APUIO1, APUIO2, APUIO3 */
{
   /* will run the APU until given APU time before reading value */
   return (S9xAPUReadPort(Address & 3));
}

static uint8 GetPPU_2180 (uint16 Address) /* WMDATA */
{
   uint8 byte = 0;
   if (!CPU.InWRAMDMAorHDMA)
   {
      byte = Memory.RAM[PPU.WRAM++];
      PPU.WRAM &= 0x1ffff;
   }
   else
      byte = OpenBus;
   return (byte);
}

uint8 (*GetPPU[])(uint16 Address) = {
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2107
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x210F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2117
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x211F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  //0x2127
	GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM1, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  //0x212F
	GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM,  GetPPU_213x, GetPPU_213x, GetPPU_213x, GetPPU_2137, //0x2137
	GetPPU_2138, GetPPU_2139, GetPPU_213A, GetPPU_213B, GetPPU_213C, GetPPU_213D, GetPPU_213E, GetPPU_213F, //0x213F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2147
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x214F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2157
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x215F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2167
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x216F
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x2177
	GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, GetPPU_APUR, //0x217F
	GetPPU_2180, GetPPU_RAM,  GetPPU_RAM,  GetPPU_RAM
};
