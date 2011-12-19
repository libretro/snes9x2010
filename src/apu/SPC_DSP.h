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

typedef BOOST::uint8_t uint8_t;

typedef struct
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
} dsp_voice_t;

// Initializes DSP and has it use the 64K RAM provided
void dsp_init( void* ram_64k );

// Sets destination for output samples. If out is NULL or out_size is 0,
// doesn't generate any.
void dsp_set_output( short* out, int out_size );

// Resets DSP to power-on state
void dsp_reset();

// Emulates pressing reset switch on SNES
void dsp_soft_reset();

// Runs DSP for specified number of clocks (~1024000 per second). Every 32 clocks
// a pair of samples is be generated.
void dsp_run( int clock_count );

// Saves/loads exact emulator state
void dsp_copy_state( unsigned char** io, dsp_copy_func_t );

void dsp_soft_reset_common();

typedef struct
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

	dsp_voice_t voices [VOICE_COUNT];

	// non-emulation state
	uint8_t* ram; // 64K shared RAM between DSP and SMP
	short* out;
	short* out_end;
	short* out_begin;
	short extra [EXTRA_SIZE];
} dsp_state_t;

extern dsp_state_t dsp_m;

typedef BOOST::int8_t   int8_t;
typedef BOOST::int16_t int16_t;

#if !SPC_NO_COPY_STATE_FUNCS

typedef struct {
	dsp_copy_func_t func;
	unsigned char** buf;
} spc_state_copy_t;

void spc_copier_copy(spc_state_copy_t * copier, void* state, size_t size );
int spc_copier_copy_int(spc_state_copy_t * copier, int state, int size);
void spc_copier_extra(spc_state_copy_t * copier);

#define SPC_COPY( type, state )\
{\
	state = (BOOST::type) spc_copier_copy_int(&copier, state, sizeof (BOOST::type) );\
}

#endif
#endif
