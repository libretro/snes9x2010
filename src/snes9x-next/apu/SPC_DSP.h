// Highly accurate SNES SPC-700 DSP emulator

// snes_spc 0.9.0
#ifndef SPC_DSP_H
#define SPC_DSP_H

#include "blargg_common.h"

extern "C" { typedef void (*dsp_copy_func_t)( unsigned char** io, void* state, size_t ); }

// Mutes voices corresponding to non-zero bits in mask (issues repeated KOFF events).
// Reduces emulation accuracy.
#define VOICE_COUNT 8

// Global DSP registers
enum {
    R_MVOLL = 0x0C, R_MVOLR = 0x1C,
    R_EVOLL = 0x2C, R_EVOLR = 0x3C,
    R_KON   = 0x4C, R_KOFF  = 0x5C,
    R_FLG   = 0x6C, R_ENDX  = 0x7C,
    R_EFB   = 0x0D, R_PMON  = 0x2D,
    R_NON   = 0x3D, R_EON   = 0x4D,
    R_DIR   = 0x5D, R_ESA   = 0x6D,
    R_EDL   = 0x7D,
    R_FIR   = 0x0F // 8 coefficients at 0x0F, 0x1F ... 0x7F
};

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
	
// State
	
	// Resets DSP and uses supplied values to initialize registers
	enum { register_count = 128 };
	void load( uint8_t const regs [register_count] );

	// Saves/loads exact emulator state
	enum { state_size = 640 }; // maximum space needed when saving
	typedef dsp_copy_func_t copy_func_t;
	void copy_state( unsigned char** io, copy_func_t );

// Snes9x Accessor
	int rom_enabled; // mirror
	uint8_t *rom, *hi_ram; // mirror

// DSP register addresses


	// Voice registers
	enum {
		v_voll   = 0x00, v_volr   = 0x01,
		v_pitchl = 0x02, v_pitchh = 0x03,
		v_srcn   = 0x04, v_adsr0  = 0x05,
		v_adsr1  = 0x06, v_gain   = 0x07,
		v_envx   = 0x08, v_outx   = 0x09
	};

public:
	enum { extra_size = 16 };
	sample_t* extra()               { return m.extra; }
	sample_t const* out_pos() const { return m.out; }
public:
	BLARGG_DISABLE_NOTHROW
	
	typedef BOOST::int8_t   int8_t;
	typedef BOOST::int16_t int16_t;
	
	enum { echo_hist_size = 8 };
	
	enum env_mode_t { env_release, env_attack, env_decay, env_sustain };
	enum { brr_buf_size = 12 };
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
	};
private:
	enum { brr_block_size = 9 };
	
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
		
		voice_t voices [VOICE_COUNT];
		
		// non-emulation state
		uint8_t* ram; // 64K shared RAM between DSP and SMP
		sample_t* out;
		sample_t* out_end;
		sample_t* out_begin;
		sample_t extra [extra_size];
	};
	state_t m;
	
	unsigned read_counter( int rate );
	
	int  interpolate( voice_t const* v );
	void run_envelope( voice_t* const v );
	void decode_brr( voice_t* v );

	void misc_27();
	void misc_28();
	void misc_29();
	void misc_30();

	void voice_output( voice_t const* v, int ch );
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

	int  echo_output( int ch );
	void echo_22();
	void echo_23();
	void echo_24();
	void echo_25();
	void echo_26();
	void echo_27();
	void echo_28();
	void echo_29();
	void echo_30();
	
	void soft_reset_common();
};

inline int SPC_DSP::read( int addr ) const
{
	return m.regs [addr];
}

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
		if ( addr == R_KON )
			m.new_kon = (uint8_t) data;
		
		if ( addr == R_ENDX ) // always cleared, regardless of data written
		{
			m.endx_buf = 0;
			m.regs [R_ENDX] = 0;
		}
		break;
	}
}

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

#endif
