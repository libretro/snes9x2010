// Highly accurate SNES SPC-700 DSP emulator

// snes_spc 0.9.0
#ifndef SPC_DSP_H
#define SPC_DSP_H

#include "blargg_common.h"

extern "C" { typedef void (*dsp_copy_func_t)( unsigned char** io, void* state, size_t ); }

#define ECHO_HIST_SIZE		8
#define ECHO_HIST_SIZE_X2	16

// Sound control

// Mutes voices corresponding to non-zero bits in mask (issues repeated KOFF events).
// Reduces emulation accuracy.
#define VOICE_COUNT 8

#define EXTRA_SIZE 16
#define EXTRA_SIZE_DIV_2 8

#define BRR_BUF_SIZE 12
#define BRR_BUF_SIZE_X2 24
#define BRR_BLOCK_SIZE 9

// DSP register addresses

// Global registers
#define R_MVOLL 0x0C
#define R_MVOLR 0x1C
#define R_EVOLL 0x2C
#define R_EVOLR 0x3C
#define R_KON	0x4C
#define R_KOFF	0x5C
#define R_FLG	0x6C
#define R_ENDX	0x7C
#define R_EFB	0x0D
#define R_EON	0x4D
#define R_PMON	0x2D
#define R_NON	0x3D
#define R_DIR	0x5D
#define R_ESA	0x6D
#define R_EDL	0x7D
#define R_FIR	0x0F	// 8 coefficients at 0x0F, 0x1F ... 0x7F

// Voice registers
#define V_VOLL 0x00
#define V_VOLR 0x01
#define V_PITCHL 0x02
#define V_PITCHH 0x03
#define V_SRCN 0x04
#define V_ADSR0 0x05
#define V_ADSR1 0x06
#define V_GAIN 0x07
#define V_ENVX 0x08
#define V_OUTX 0x09

#define REGISTER_COUNT 128

#define ENV_RELEASE	0
#define ENV_ATTACK	1
#define ENV_DECAY	2
#define ENV_SUSTAIN	3

class SPC_DSP {
public:
	typedef BOOST::uint8_t uint8_t;
	
// Setup

	// Initializes DSP and has it use the 64K RAM provided
	void init( void* ram_64k );

	// Sets destination for output samples. If out is NULL or out_size is 0,
	// doesn't generate any.
	void set_output( short* out, int out_size );

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
	

// State
	
	// Resets DSP and uses supplied values to initialize registers
	void load( uint8_t const regs [REGISTER_COUNT] );

	// Saves/loads exact emulator state
	void copy_state( unsigned char** io, dsp_copy_func_t );
public:
	short* extra()               { return m.extra; }
	short const* out_pos() const { return m.out; }
public:
	BLARGG_DISABLE_NOTHROW
	
	typedef BOOST::int8_t   int8_t;
	typedef BOOST::int16_t int16_t;
	
	
	struct voice_t
	{
		int buf [BRR_BUF_SIZE_X2];// decoded samples (twice the size to simplify wrap handling)
		int buf_pos;            // place in buffer where next samples will be decoded
		int interp_pos;         // relative fractional position in sample (0x1000 = 1.0)
		int brr_addr;           // address of current BRR block
		int brr_offset;         // current decoding offset in BRR block
		uint8_t* regs;          // pointer to voice's DSP registers
		int vbit;               // bitmask for voice: 0x01 for voice 0, 0x02 for voice 1, etc.
		int kon_delay;          // KON delay/current setup phase
		int env_mode;
		int env;                // current envelope level
		int hidden_env;         // used by GAIN mode 7, very obscure quirk
		uint8_t t_envx_out;
	};
private:
	
	struct state_t
	{
		uint8_t regs [REGISTER_COUNT];
		
		// Echo history keeps most recent 8 samples (twice the size to simplify wrap handling)
		int echo_hist [ECHO_HIST_SIZE_X2] [2];
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
		short* out;
		short* out_end;
		short* out_begin;
		short extra [EXTRA_SIZE];
	};
	state_t m;
	
	int  interpolate( voice_t const* v );
	void run_envelope( voice_t* const v );
	void decode_brr( voice_t* v );

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

	void echo_22();
	void echo_23();
	void echo_24();
	void echo_25();
	void echo_26();
	void echo_27();
	void echo_29();
	
	void soft_reset_common();
};

inline int SPC_DSP::sample_count() const { return m.out - m.out_begin; }

inline int SPC_DSP::read( int addr ) const
{
	return m.regs [addr];
}

inline void SPC_DSP::write( int addr, int data )
{
	m.regs [addr] = (uint8_t) data;
	switch ( addr & 0x0F )
	{
		case V_ENVX:
			m.envx_buf = (uint8_t) data;
			break;

		case V_OUTX:
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
	dsp_copy_func_t func;
	unsigned char** buf;
public:
	SPC_State_Copier( unsigned char** p, dsp_copy_func_t f ) { func = f; buf = p; }
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

