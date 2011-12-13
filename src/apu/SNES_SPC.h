// SNES SPC-700 APU emulator

// snes_spc 0.9.0
#ifndef SNES_SPC_H
#define SNES_SPC_H

#include "SPC_DSP.h"
#include "blargg_endian.h"

#define REG_COUNT 0x10

#define PORT_COUNT 4

#define TEMPO_UNIT 0x100

#define STATE_SIZE 68 * 1024L // maximum space needed when saving

#define TIMER_COUNT 3

#define ROM_SIZE 0x40

// 1024000 SPC clocks per second, sample pair every 32 clocks
#define CLOCKS_PER_SAMPLE 32

#define R_TEST 0x0
#define R_CONTROL 0x1
#define R_DSPADDR 0x2
#define R_DSPDATA 0x3
#define R_CPUIO0 0x4
#define R_CPUIO1 0x5
#define R_CPUIO2 0x6
#define R_CPUIO3 0x7
#define R_F8 0x8
#define R_F9 0x9
#define R_T0TARGET 0xA
#define R_T1TARGET 0xB
#define R_T2TARGET 0xC
#define R_T0OUT 0xD
#define R_T1OUT 0xE
#define R_T2OUT 0xF

#define ROM_ADDR 0xFFC0

// Value that padding should be filled with
#define CPU_PAD_FILL 0xFF

struct SNES_SPC {
public:
	typedef BOOST::uint8_t uint8_t;
	
	// Must be called once before using
	void init();

	// Emulator use

	// Sets destination for output samples
	void set_output( short* out, int out_size );

	// Number of samples written to output since last set
	int sample_count() const;

	// Resets SPC to power-on state. This resets your output buffer, so you must
	// call set_output() after this.
	void reset();

	// Emulates pressing reset switch on SNES. This resets your output buffer, so
	// you must call set_output() after this.
	void soft_reset();

	// Emulated port read/write at specified time
	int  read_port ( int, int port );
	void write_port( int, int port, int data );

	// Runs SPC to end_time and starts a new time frame at 0
	void end_frame( int end_time );
	
	// Sets tempo, where tempo_unit = normal, tempo_unit / 2 = half speed, etc.
	void set_tempo( int );
// State save/load (only available with accurate DSP)

#if !SPC_NO_COPY_STATE_FUNCS
	// Saves/loads state
	void copy_state( unsigned char** io, dsp_copy_func_t );
#endif

//// Snes9x Accessor
	void	spc_allow_time_overflow( bool );
public:
	BLARGG_DISABLE_NOTHROW
	
	typedef BOOST::uint16_t uint16_t;
	
	// rel_time_t - Time relative to m_spc_time. Speeds up code a bit by eliminating need to
	// constantly add m_spc_time to time from CPU. CPU uses time that ends at
	// 0 to eliminate reloading end time every instruction. It pays off.
	
	struct Timer
	{
		int next_time; // time of next event
		int prescaler;
		int period;
		int divider;
		int enabled;
		int counter;
	};
	
	// Support SNES_MEMORY_APURAM
	uint8_t *apuram();
	
private:
	SPC_DSP dsp;
	
	static signed char const reg_times_ [256];
	signed char reg_times [256];
	
	struct state_t
	{
		Timer timers [TIMER_COUNT];
		
		uint8_t smp_regs [2] [REG_COUNT];
		
		struct
		{
			int pc;
			int a;
			int x;
			int y;
			int psw;
			int sp;
		} cpu_regs;
		
		int  dsp_time;
		int  spc_time;
		
		int         tempo;
		
		int         extra_clocks;
		short*   buf_begin;
		short const* buf_end;
		short*   extra_pos;
		short    extra_buf [EXTRA_SIZE];
		
		int         rom_enabled;
		uint8_t     rom    [ROM_SIZE];
		uint8_t     hi_ram [ROM_SIZE];
		
		unsigned char cycle_table [256];
		
		struct
		{
			// padding to neutralize address overflow
			union {
				uint8_t padding1 [0x100];
				uint16_t align; // makes compiler align data for 16-bit access
			} padding1 [1];
			uint8_t ram      [0x10000];
			uint8_t padding2 [0x100];
		} ram;
	};
	state_t m;
	
	void enable_rom( int enable );
	void reset_common( int timer_counter_init );
	
	Timer* run_timer_      ( Timer* t, int );
	Timer* run_timer       ( Timer* t, int );
	void dsp_write         ( int data, int );
	void cpu_write_smp_reg_( int data, int, int addr );
	void cpu_write_smp_reg ( int data, int, int addr );
	void cpu_write         ( int data, int addr, int );
	int cpu_read           ( int addr, int );
	unsigned CPU_mem_bit   ( uint8_t const* pc, int );
	
	uint8_t* run_until_( int end_time );
	
// Snes9x timing hack
	bool allow_time_overflow;
};

inline int SNES_SPC::sample_count() const { return (m.extra_clocks >> 5) * 2; }

inline int SNES_SPC::read_port( int t, int port )
{
	return run_until_( t ) [port];
}

inline void SNES_SPC::write_port( int t, int port, int data )
{
	run_until_( t ) [0x10 + port] = data;
	m.ram.ram [0xF4 + port] = data;
}

inline void SNES_SPC::spc_allow_time_overflow( bool allow ) { allow_time_overflow = allow; }

#endif

