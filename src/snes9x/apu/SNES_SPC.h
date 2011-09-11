// SNES SPC-700 APU emulator

// snes_spc 0.9.0
#ifndef SNES_SPC_H
#define SNES_SPC_H

#include "blargg_endian.h"
#include "shiftmul.h"
#include "blargg_common.h"

extern "C" { typedef void (*dsp_copy_func_t)( unsigned char** io, void* state, size_t ); }

namespace {
	enum { rom_size = 0x40 };
#define port_count 4
	enum {
		r_test     = 0x0, r_control  = 0x1,
		r_dspaddr  = 0x2, r_dspdata  = 0x3,
		r_cpuio0   = 0x4, r_cpuio1   = 0x5,
		r_cpuio2   = 0x6, r_cpuio3   = 0x7,
		r_f8       = 0x8, r_f9       = 0x9,
		r_t0target = 0xA, r_t1target = 0xB, r_t2target = 0xC,
		r_t0out    = 0xD, r_t1out    = 0xE, r_t2out    = 0xF
	};
	enum env_mode_t { env_release, env_attack, env_decay, env_sustain };
	// DSP register addresses

	// Global registers
	enum {
		r_mvoll = 0x0C, r_mvolr = 0x1C,
		r_evoll = 0x2C, r_evolr = 0x3C,
		r_kon   = 0x4C, r_koff  = 0x5C,
		r_flg   = 0x6C, r_endx  = 0x7C,
		r_efb   = 0x0D, r_pmon  = 0x2D,
		r_non   = 0x3D, r_eon   = 0x4D,
		r_dir   = 0x5D, r_esa   = 0x6D,
		r_edl   = 0x7D,
		r_fir   = 0x0F // 8 coefficients at 0x0F, 0x1F ... 0x7F
	};

	// Voice registers
	enum {
		v_voll   = 0x00, v_volr   = 0x01,
		v_pitchl = 0x02, v_pitchh = 0x03,
		v_srcn   = 0x04, v_adsr0  = 0x05,
		v_adsr1  = 0x06, v_gain   = 0x07,
		v_envx   = 0x08, v_outx   = 0x09
	};
}

//SPC DSP

class SPC_DSP {
	public:
		typedef BOOST::uint8_t uint8_t;

		// Setup

		// Initializes DSP and has it use the 64K RAM provided
		void init( void* ram_64k );

		// Sets destination for output samples. If out is NULL or out_size is 0,
		// doesn't generate any.
		typedef short sample_t;
		void set_output( sample_t* out, int out_size );

		// Number of samples written to output since it was last set, always
		// a multiple of 2. Undefined if more samples were generated than
		// output buffer could hold.
		int sample_count() const;

		// Emulation

		// Resets DSP to power-on state
		void reset();

		// Emulates pressing reset switch on SNES
		void soft_reset();

		// Reads/writes DSP registers. For accuracy, you must first call run()
		// to catch the DSP up to present.
		int  read ( int addr ) const;
		void write( int addr, int data );

		// Runs DSP for specified number of clocks (~1024000 per second). Every 32 clocks
		// a pair of samples is be generated.
		void run( int clock_count );

		// Sound control

		// Mutes voices corresponding to non-zero bits in mask (issues repeated KOFF events).
		// Reduces emulation accuracy.
#define voice_count 8

		// State

		// Resets DSP and uses supplied values to initialize registers
#define register_count 128
		void load( uint8_t const regs [register_count] );

		// Saves/loads exact emulator state
		enum { state_size = 640 }; // maximum space needed when saving
		typedef dsp_copy_func_t copy_func_t;
		void copy_state( unsigned char** io, copy_func_t );

		// Returns non-zero if new key-on events occurred since last call
		//bool check_kon();

		// Snes9x Accessor

		//int  stereo_switch;
		int  take_spc_snapshot;
		int rom_enabled;        // mirror
		uint8_t *rom, *hi_ram; //mirror
		void (*spc_snapshot_callback) (void);

		void    set_spc_snapshot_callback( void (*callback) (void) );
		void    dump_spc_snapshot( void );
		//void    set_stereo_switch( int );
		uint8_t reg_value( int, int );
		int     envx_value( int );


	public:
		enum { extra_size = 16 };
		sample_t* extra()               { return m.extra; }
	public:
		BLARGG_DISABLE_NOTHROW

			typedef BOOST::int8_t   int8_t;
		typedef BOOST::int16_t int16_t;

#define echo_hist_size 8

#define brr_buf_size 12
		struct voice_t
		{
			int buf [brr_buf_size*2];// decoded samples (twice the size to simplify wrap handling)
			int buf_pos;            // place in buffer where next samples will be decoded
			int interp_pos;         // relative fractional position in sample (0x1000 = 1.0)
			int brr_addr;           // address of current BRR block
			int brr_offset;         // current decoding offset in BRR block
			uint8_t* regs;          // pointer to voice's DSP registers
			int vbit;               // bitmask for voice: 0x01 for voice 0, 0x02 for voice 1, etc.
			int kon_delay;          // KON delay/current setup phase
			env_mode_t env_mode;
			int env;                // current envelope level
			int hidden_env;         // used by GAIN mode 7, very obscure quirk
			uint8_t t_envx_out;
			//int voice_number;
		};
#define brr_block_size 9

		struct state_t
		{
			uint8_t regs [register_count];

			// Echo history keeps most recent 8 samples (twice the size to simplify wrap handling)
			int echo_hist [echo_hist_size * 2] [2];
			int (*echo_hist_pos) [2]; // &echo_hist [0 to 7]

			int every_other_sample; // toggles every sample
			int kon;                // KON value when last checked
			int noise;
			int counter;
			int echo_offset;        // offset from ESA in echo buffer
			int echo_length;        // number of bytes that echo_offset will stop at
			int phase;              // next clock cycle to run (0-31)
			bool kon_check;         // set when a new KON occurs

			// Hidden registers also written to when main register is written to
			int new_kon;
			uint8_t endx_buf;
			uint8_t envx_buf;
			uint8_t outx_buf;

			// Temporary state between clocks

			// read once per sample
			int t_pmon;
			int t_non;
			int t_eon;
			int t_dir;
			int t_koff;

			// read a few clocks ahead then used
			int t_brr_next_addr;
			int t_adsr0;
			int t_brr_header;
			int t_brr_byte;
			int t_srcn;
			int t_esa;
			int t_echo_enabled;

			// internal state that is recalculated every sample
			int t_dir_addr;
			int t_pitch;
			int t_output;
			int t_looped;
			int t_echo_ptr;

			// left/right sums
			int t_main_out [2];
			int t_echo_out [2];
			int t_echo_in  [2];

			voice_t voices [voice_count];

			// non-emulation state
			uint8_t* ram; // 64K shared RAM between DSP and SMP
			int mute_mask;
			sample_t* out;
			sample_t* out_end;
			sample_t* out_begin;
			sample_t extra [extra_size];
		};
		state_t m;
	private:

		void init_counter();
		void run_counters();
		unsigned read_counter( int rate );

		int  interpolate( voice_t const* v );
		void run_envelope( voice_t* const v );
		void decode_brr( voice_t* v );

		void misc_27();
		void misc_28();
		void misc_29();
		void misc_30();

		void voice_V1( voice_t* const );
		void voice_V2( voice_t* const );
		void voice_V3( voice_t* const );
		void voice_V3a( voice_t* const );
		void voice_V3b( voice_t* const );
		void voice_V3c( voice_t* const );
		void voice_V4( voice_t* const );
		void voice_V5( voice_t* const );
		void voice_V6( voice_t* const );
		void voice_V7( voice_t* const );
		void voice_V8( voice_t* const );
		void voice_V9( voice_t* const );
		void voice_V7_V4_V1( voice_t* const );
		void voice_V8_V5_V2( voice_t* const );
		void voice_V9_V6_V3( voice_t* const );

		void echo_22();
		void echo_23();
		void echo_24();
		void echo_25();
		void echo_26();
		void echo_27();
		void echo_28();
		void echo_29();
		void echo_30();

};

//inline int SPC_DSP::sample_count() const { return m.out - m.out_begin; }

#if 0
inline int SPC_DSP::read( int addr ) const
{
	return m.regs [addr];
}
#endif

#if 0
inline void SPC_DSP::write( int addr, int data )
{
	m.regs [addr] = (uint8_t) data;
	switch ( addr & 0x0F )
	{
	case v_envx:
		m.envx_buf = (uint8_t) data;
		break;
		
	case v_outx:
		m.outx_buf = (uint8_t) data;
		break;
	
	case 0x0C:
		if ( addr == r_kon )
			m.new_kon = (uint8_t) data;
		
		if ( addr == r_endx ) // always cleared, regardless of data written
		{
			m.endx_buf = 0;
			m.regs [r_endx] = 0;
		}
		break;
	}
}
#endif

#if 0
inline bool SPC_DSP::check_kon()
{
	bool old = m.kon_check;
	m.kon_check = 0;
	return old;
}
#endif

#if !SPC_NO_COPY_STATE_FUNCS

class SPC_State_Copier {
	SPC_DSP::copy_func_t func;
	unsigned char** buf;
	public:
	SPC_State_Copier( unsigned char** p, SPC_DSP::copy_func_t f ) { func = f; buf = p; }
	void copy( void* state, size_t size );
	int copy_int( int state, int size );
	void skip( int count );
	void extra();
};

#define SPC_COPY( type, state )\
{\
	state = (BOOST::type) copier.copy_int( state, sizeof (BOOST::type) );\
}

#endif

struct SNES_SPC {
	public:
		typedef BOOST::uint8_t uint8_t;

		// Must be called once before using
		blargg_err_t init();

		// Emulator use

		// Sets IPL ROM data. Library does not include ROM data. Most SPC music files
		// don't need ROM, but a full emulator must provide this.
		void init_rom( uint8_t const rom [rom_size] );

		// Sets destination for output samples
		typedef short sample_t;
		void set_output( sample_t* out, int out_size );

		// Number of samples written to output since last set
		int sample_count() const;

		// Resets SPC to power-on state. This resets your output buffer, so you must
		// call set_output() after this.
		void reset();

		// Emulates pressing reset switch on SNES. This resets your output buffer, so
		// you must call set_output() after this.
		void soft_reset();

		// 1024000 SPC clocks per second, sample pair every 32 clocks
		typedef int time_t;
#define clocks_per_sample 32

		// Emulated port read/write at specified time
		//int  read_port ( time_t, int port );
		//void write_port( time_t, int port, int data );

		// Runs SPC to end_time and starts a new time frame at 0
		void end_frame( time_t end_time );

		// Sound control

		// Mutes voices corresponding to non-zero bits in mask (issues repeated KOFF events).
		// Reduces emulation accuracy.
#define voice_count 8
		void mute_voices( int mask );

		// Loads SPC data into emulator
		enum { spc_min_file_size = 0x10180 };
		enum { spc_file_size     = 0x10200 };

		// If true, prevents channels and global volumes from being phase-negated.
		// Only supported by fast DSP.
		void disable_surround( bool disable = true );

		// Sets tempo, where tempo_unit = normal, tempo_unit / 2 = half speed, etc.
#define tempo_unit 256
		void set_tempo( int );

		// SPC music files


		// Skips count samples. Several times faster than play() when using fast DSP.
		blargg_err_t skip( int count );

		// State save/load (only available with accurate DSP)

#if !SPC_NO_COPY_STATE_FUNCS
		// Saves/loads state
		enum { state_size = 68 * 1024L }; // maximum space needed when saving
		typedef SPC_DSP::copy_func_t copy_func_t;
		void copy_state( unsigned char** io, copy_func_t );

		// Writes minimal header to spc_out
		static void init_header( void* spc_out );

		// Saves emulator state as SPC file data. Writes spc_file_size bytes to spc_out.
		// Does not set up SPC header; use init_header() for that.
		void save_spc( void* spc_out );

		// Returns true if new key-on events occurred since last check. Useful for
		// trimming silence while saving an SPC.
		//bool check_kon();
#endif

		//// Snes9x Accessor

		void	spc_allow_time_overflow( bool );

		void    dsp_set_spc_snapshot_callback( void (*callback) (void) );
		void    dsp_dump_spc_snapshot( void );
		//void    dsp_set_stereo_switch( int );

	public:
		BLARGG_DISABLE_NOTHROW

			typedef BOOST::uint16_t uint16_t;

		// Time relative to m_spc_time. Speeds up code a bit by eliminating need to
		// constantly add m_spc_time to time from CPU. CPU uses time that ends at
		// 0 to eliminate reloading end time every instruction. It pays off.
		typedef int rel_time_t;

		struct Timer
		{
			rel_time_t next_time; // time of next event
			int prescaler;
			int period;
			int divider;
			int enabled;
			int counter;
		};
		enum { reg_count = 0x10 };
#define timer_count 3
		enum { extra_size = SPC_DSP::extra_size };

		enum { signature_size = 35 };
		struct state_t
		{
			Timer timers [timer_count];

			uint8_t smp_regs [2] [reg_count];

			struct
			{
				int pc;
				int a;
				int x;
				int y;
				int psw;
				int sp;
			} cpu_regs;

			rel_time_t  dsp_time;
			time_t      spc_time;
			bool        echo_accessed;

			int         tempo;
			int         skipped_kon;
			int         skipped_koff;
			const char* cpu_error;

			int         extra_clocks;
			sample_t*   buf_begin;
			sample_t const* buf_end;
			sample_t*   extra_pos;
			sample_t    extra_buf [extra_size];

			int         rom_enabled;
			uint8_t     rom    [rom_size];
			uint8_t     hi_ram [rom_size];

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
		uint8_t* run_until_( time_t end_time );
	private:
		SPC_DSP dsp;

#if SPC_LESS_ACCURATE
		static signed char const reg_times_ [256];
		signed char reg_times [256];
#endif


		enum { rom_addr = 0xFFC0 };

		enum { skipping_time = 127 };

		// Value that padding should be filled with
		enum { cpu_pad_fill = 0xFF };


		void timers_loaded();
		void ram_loaded();
		void reset_time_regs();

		Timer* run_timer_      ( Timer* t, rel_time_t );
		Timer* run_timer       ( Timer* t, rel_time_t );
		//void dsp_write         ( int data, rel_time_t );
		void cpu_write_smp_reg_( int data, rel_time_t, int addr );
		void cpu_write_smp_reg ( int data, rel_time_t, int addr );
		void cpu_write_high    ( int data, int i, rel_time_t );
		void cpu_write         ( int data, int addr, rel_time_t );
		int cpu_read_smp_reg   ( int i, rel_time_t );
		int cpu_read           ( int addr, rel_time_t );

		bool check_echo_access ( int addr );

		struct spc_file_t
		{
			char    signature [signature_size];
			uint8_t has_id666;
			uint8_t version;
			uint8_t pcl, pch;
			uint8_t a;
			uint8_t x;
			uint8_t y;
			uint8_t psw;
			uint8_t sp;
			char    text [212];
			uint8_t ram [0x10000];
			uint8_t dsp [128];
			uint8_t unused [0x40];
			uint8_t ipl_rom [0x40];
		};

		static char const signature [signature_size + 1];

		void save_regs( uint8_t out [reg_count] );

		// Snes9x timing hack
		bool allow_time_overflow;
};

//inline int SNES_SPC::sample_count() const { return mul<2>((m.extra_clocks >> 5)); }

#if 0
inline int SNES_SPC::read_port( time_t t, int port )
{
	return run_until_( t ) [port];
}
#endif

#if 0
inline void SNES_SPC::write_port( time_t t, int port, int data )
{
	run_until_( t ) [0x10 + port] = data;
	m.ram.ram [0xF4 + port] = data;
}
#endif

#if 0
#if !SPC_NO_COPY_STATE_FUNCS
inline bool SNES_SPC::check_kon() { return dsp.check_kon(); }
#endif
#endif

inline void SNES_SPC::spc_allow_time_overflow( bool allow ) { allow_time_overflow = allow; }

#endif

