#ifndef PPU_IRQ_SOURCE
	#define PPU_IRQ_SOURCE		2
#endif

#ifndef GSU_IRQ_SOURCE
	#define GSU_IRQ_SOURCE		4
#endif

#ifndef SA1_DMA_IRQ_SOURCE
	#define SA1_DMA_IRQ_SOURCE	32
#endif

#ifndef SA1_IRQ_SOURCE
	#define SA1_IRQ_SOURCE		128
#endif

#define S9X_CLEAR_IRQ(source) \
	CPU.IRQActive &= ~source; \
	if (!CPU.IRQActive) \
		CPU.Flags &= ~IRQ_FLAG;

#define S9X_SET_IRQ(source) \
	CPU.IRQActive |= source; \
	CPU.IRQPending = Timings.IRQPendCount; \
	CPU.Flags |= IRQ_FLAG; \
	if (CPU.WaitingForInterrupt) \
	{ \
		/* Force IRQ to trigger immediately after WAI - \
		Final Fantasy Mystic Quest crashes without this. */ \
		CPU.WaitingForInterrupt = FALSE; \
		Registers.PCw++; \
	}

#define S9xUnpackStatus() \
	ICPU._Zero = (Registers.PL & Zero) == 0; \
	ICPU._Negative = (Registers.PL & Negative); \
	ICPU._Carry = (Registers.PL & Carry); \
	ICPU._Overflow = (Registers.PL & Overflow) >> 6;

#define S9xPackStatus() \
	Registers.PL &= ~(Zero | Negative | Carry | Overflow); \
	Registers.PL |= ICPU._Carry | ((ICPU._Zero == 0) << 1) | (ICPU._Negative & 0x80) | (ICPU._Overflow << 6);
