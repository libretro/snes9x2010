/* snes_ntsc 0.2.2. http://www.slack.net/~ant/ */

/* Common implementation of NTSC filters */

#include <assert.h>
#include <stdint.h>

/* Copyright (C) 2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#define DISABLE_CORRECTION 0

/* ------------------------------------------------------------------ */
/* Fixed-point (Q24) core. All kernel generation is done in fixed      */
/* point so the filter has no dependency on <math.h> or the FPU and    */
/* produces identical output on every target.                          */
/* ------------------------------------------------------------------ */

/* snes_ntsc_fx_t defined in snes_ntsc.h */

#define SNES_NTSC_FRAC 24
#define SNES_NTSC_ONE  ((snes_ntsc_fx_t)16777216)         /* 1.0 in Q24 */
#define SNES_NTSC_FMUL( a, b ) \
	((snes_ntsc_fx_t)(((snes_ntsc_fx_t)(a) * (snes_ntsc_fx_t)(b)) >> SNES_NTSC_FRAC))

/* Q24 constants (value * 2^24, rounded) */
#define SNES_NTSC_PI              52707179L  /* 3.14159265358979 */
#define SNES_NTSC_2PI            105414357L  /* 6.28318530717959 */
#define SNES_NTSC_PI2             26353589L  /* 1.57079632679490 */
#define SNES_NTSC_LN2             11629080L  /* 0.69314718055995 */
#define SNES_NTSC_LOG2E           24204406L  /* 1.44269504088896 */
#define SNES_NTSC_C_032             536871L  /* 0.032 */
#define SNES_NTSC_C_020            3355443L  /* 0.20 */
#define SNES_NTSC_C_050            8388608L  /* 0.5 */
#define SNES_NTSC_C_1056          17716740L  /* 1.056 */
#define SNES_NTSC_C_0981          16458449L  /* 0.981 */
#define SNES_NTSC_C_042            7046431L  /* 0.42 */
#define SNES_NTSC_C_008            1342177L  /* 0.08 */
#define SNES_NTSC_C_N003125        -524288L  /* -0.03125 */
#define SNES_NTSC_C_N30         -503316480L  /* -30.0 */
#define SNES_NTSC_C_065           10905190L  /* 0.65 */
#define SNES_NTSC_C_15            25165824L  /* 1.5 */
#define SNES_NTSC_C_11333         19013619L  /* 1.1333 */
#define SNES_NTSC_C_0866025       14529488L  /* 0.866025 */
#define SNES_NTSC_C_N0866025     -14529488L  /* -0.866025 */
#define SNES_NTSC_C_N05           -8388608L  /* -0.5 */
#define SNES_NTSC_D0              16039018L  /* 0.956 */
#define SNES_NTSC_D1              10418651L  /* 0.621 */
#define SNES_NTSC_D2              -4563403L  /* -0.272 */
#define SNES_NTSC_D3             -10854859L  /* -0.647 */
#define SNES_NTSC_D4             -18538824L  /* -1.105 */
#define SNES_NTSC_D5              28554822L  /* 1.702 */
#define SNES_NTSC_Y_R              5016388L  /* 0.299 */
#define SNES_NTSC_Y_G              9848226L  /* 0.587 */
#define SNES_NTSC_Y_B              1912603L  /* 0.114 */
#define SNES_NTSC_I_R              9999221L  /* 0.596 */
#define SNES_NTSC_I_G              4613734L  /* 0.275 */
#define SNES_NTSC_I_B              5385486L  /* 0.321 */
#define SNES_NTSC_Q_R              3556770L  /* 0.212 */
#define SNES_NTSC_Q_G              8774484L  /* 0.523 */
#define SNES_NTSC_Q_B              5217714L  /* 0.311 */
#define SNES_NTSC_K_6667          11185370L  /* 0.6667 */
#define SNES_NTSC_K_3333           5591846L  /* 0.3333 */

static snes_ntsc_fx_t snes_ntsc_fdiv( snes_ntsc_fx_t a, snes_ntsc_fx_t b )
{
	/* a is always small in magnitude here, so the shift cannot overflow */
	return (snes_ntsc_fx_t)(((snes_ntsc_fx_t)a << SNES_NTSC_FRAC) / b);
}

/* truncate Q24 to integer toward zero (matches C's (int) cast on the
   original float path) */
static snes_ntsc_fx_t snes_ntsc_toint( snes_ntsc_fx_t v )
{
	return v >= 0 ? (v >> SNES_NTSC_FRAC) : -((-v) >> SNES_NTSC_FRAC);
}

/* cos( x ) for x in [-pi/2, pi/2] via Taylor series */
static snes_ntsc_fx_t snes_ntsc_cos_core( snes_ntsc_fx_t x )
{
	snes_ntsc_fx_t x2 = SNES_NTSC_FMUL( x, x );
	snes_ntsc_fx_t t  = SNES_NTSC_ONE;
	snes_ntsc_fx_t s  = SNES_NTSC_ONE;
	t = SNES_NTSC_FMUL( t, x2 ); s -= t / 2;
	t = SNES_NTSC_FMUL( t, x2 ); s += t / 24;
	t = SNES_NTSC_FMUL( t, x2 ); s -= t / 720;
	t = SNES_NTSC_FMUL( t, x2 ); s += t / 40320;
	t = SNES_NTSC_FMUL( t, x2 ); s -= t / 3628800;
	t = SNES_NTSC_FMUL( t, x2 ); s += t / 479001600;
	return s;
}

static snes_ntsc_fx_t snes_ntsc_cos( snes_ntsc_fx_t x )
{
	while ( x >  SNES_NTSC_PI ) x -= SNES_NTSC_2PI;
	while ( x < -SNES_NTSC_PI ) x += SNES_NTSC_2PI;
	if ( x >  SNES_NTSC_PI2 ) return -snes_ntsc_cos_core( SNES_NTSC_PI - x );
	if ( x < -SNES_NTSC_PI2 ) return -snes_ntsc_cos_core( SNES_NTSC_PI + x );
	return snes_ntsc_cos_core( x );
}

static snes_ntsc_fx_t snes_ntsc_sin( snes_ntsc_fx_t x )
{
	return snes_ntsc_cos( x - SNES_NTSC_PI2 );
}

/* 2^f for f in [0,1) */
static snes_ntsc_fx_t snes_ntsc_exp2_frac( snes_ntsc_fx_t f )
{
	snes_ntsc_fx_t y = SNES_NTSC_FMUL( f, SNES_NTSC_LN2 );
	snes_ntsc_fx_t t = SNES_NTSC_ONE;
	snes_ntsc_fx_t s = SNES_NTSC_ONE;
	int k;
	for ( k = 1; k <= 9; k++ )
	{
		t = SNES_NTSC_FMUL( t, y ) / k;
		s += t;
	}
	return s;
}

static snes_ntsc_fx_t snes_ntsc_exp( snes_ntsc_fx_t x )
{
	snes_ntsc_fx_t y = SNES_NTSC_FMUL( x, SNES_NTSC_LOG2E );
	snes_ntsc_fx_t fl = y >= 0 ? (y >> SNES_NTSC_FRAC)
			: -(((-y) + SNES_NTSC_ONE - 1) >> SNES_NTSC_FRAC);
	int k = (int)fl;
	snes_ntsc_fx_t frac = y - ((snes_ntsc_fx_t)k << SNES_NTSC_FRAC);
	snes_ntsc_fx_t m = snes_ntsc_exp2_frac( frac );
	return k >= 0 ? m << k : m >> (-k);
}

static snes_ntsc_fx_t snes_ntsc_ln( snes_ntsc_fx_t x )
{
	int e = 0;
	snes_ntsc_fx_t m = x;
	snes_ntsc_fx_t u, u2, t, s;
	int k;
	if ( m <= 0 )
		return -(100L << SNES_NTSC_FRAC);
	while ( m >= 2 * SNES_NTSC_ONE ) { m >>= 1; e++; }
	while ( m <     SNES_NTSC_ONE ) { m <<= 1; e--; }
	/* ln(m) via atanh series, m in [1,2) */
	u  = snes_ntsc_fdiv( m - SNES_NTSC_ONE, m + SNES_NTSC_ONE );
	u2 = SNES_NTSC_FMUL( u, u );
	t  = u;
	s  = u;
	for ( k = 3; k <= 15; k += 2 )
	{
		t = SNES_NTSC_FMUL( t, u2 );
		s += t / k;
	}
	return (snes_ntsc_fx_t)e * SNES_NTSC_LN2 + 2 * s;
}

static snes_ntsc_fx_t snes_ntsc_pow( snes_ntsc_fx_t base, snes_ntsc_fx_t e )
{
	if ( base <= 0 )
		return 0;
	return snes_ntsc_exp( SNES_NTSC_FMUL( e, snes_ntsc_ln( base ) ) );
}

/* ------------------------------------------------------------------ */

#ifndef LUMA_CUTOFF
	#define LUMA_CUTOFF SNES_NTSC_C_020
#endif
#ifndef gamma_size
	#define gamma_size 1
#endif
#ifndef rgb_bits
	#define rgb_bits 8
#endif
#ifndef artifacts_max
	#define artifacts_max SNES_NTSC_FMUL( artifacts_mid, SNES_NTSC_C_15 )
#endif
#ifndef fringing_max
	#define fringing_max (fringing_mid * 2)
#endif
#ifndef STD_HUE_CONDITION
	#define STD_HUE_CONDITION( setup ) 1
#endif

#define ext_decoder_hue     (std_decoder_hue + 15)
#define rgb_unit            (1 << rgb_bits)
#define rgb_offset          (snes_ntsc_fx_t)((rgb_unit * 2) * SNES_NTSC_ONE + SNES_NTSC_ONE / 2)

enum { burst_size  = snes_ntsc_entry_size / burst_count };
enum { kernel_half = 16 };
enum { kernel_size = kernel_half * 2 + 1 };

typedef struct init_t
{
	snes_ntsc_fx_t to_rgb [burst_count * 6];
	snes_ntsc_fx_t to_float [gamma_size];
	snes_ntsc_fx_t contrast;
	snes_ntsc_fx_t brightness;
	snes_ntsc_fx_t artifacts;
	snes_ntsc_fx_t fringing;
	snes_ntsc_fx_t kernel [rescale_out * kernel_size * 2];
} init_t;

#define ROTATE_IQ( i, q, sin_b, cos_b ) {\
	snes_ntsc_fx_t t;\
	t = SNES_NTSC_FMUL( i, cos_b ) - SNES_NTSC_FMUL( q, sin_b );\
	q = SNES_NTSC_FMUL( i, sin_b ) + SNES_NTSC_FMUL( q, cos_b );\
	i = t;\
}

static void init_filters( init_t* impl, snes_ntsc_setup_t const* setup )
{
#if rescale_out > 1
	snes_ntsc_fx_t kernels [kernel_size * 2];
#else
	snes_ntsc_fx_t* const kernels = impl->kernel;
#endif

	/* generate luma (y) filter using sinc kernel */
	{
		/* sinc with rolloff (dsf) */
		snes_ntsc_fx_t const rolloff = SNES_NTSC_ONE + SNES_NTSC_FMUL( setup->sharpness, SNES_NTSC_C_032 );
		snes_ntsc_fx_t const maxh = 32 * SNES_NTSC_ONE;
		snes_ntsc_fx_t pow_a_n;
		snes_ntsc_fx_t sum;
		int i;
		/* quadratic mapping to reduce negative (blurring) range */
		snes_ntsc_fx_t to_angle = setup->resolution + SNES_NTSC_ONE;
		{
			snes_ntsc_fx_t p = rolloff;
			int s2;
			for ( s2 = 0; s2 < 5; s2++ )   /* rolloff^32 */
				p = SNES_NTSC_FMUL( p, p );
			pow_a_n = p;
		}
		to_angle = SNES_NTSC_FMUL( snes_ntsc_fdiv( SNES_NTSC_PI, maxh ),
				SNES_NTSC_FMUL( LUMA_CUTOFF, SNES_NTSC_FMUL( to_angle, to_angle ) + SNES_NTSC_ONE ) );

		kernels [kernel_size * 3 / 2] = maxh; /* default center value */
		for ( i = 0; i < kernel_half * 2 + 1; i++ )
		{
			int x = i - kernel_half;
			snes_ntsc_fx_t angle = SNES_NTSC_FMUL( (snes_ntsc_fx_t)x * SNES_NTSC_ONE, to_angle );
			/* instability occurs at center point with rolloff very close to 1.0 */
			if ( x || pow_a_n > SNES_NTSC_C_1056 || pow_a_n < SNES_NTSC_C_0981 )
			{
				snes_ntsc_fx_t rolloff_cos_a = SNES_NTSC_FMUL( rolloff, snes_ntsc_cos( angle ) );
				snes_ntsc_fx_t num = SNES_NTSC_ONE - rolloff_cos_a -
						SNES_NTSC_FMUL( pow_a_n, snes_ntsc_cos( SNES_NTSC_FMUL( maxh, angle ) ) ) +
						SNES_NTSC_FMUL( SNES_NTSC_FMUL( pow_a_n, rolloff ),
								snes_ntsc_cos( SNES_NTSC_FMUL( maxh - SNES_NTSC_ONE, angle ) ) );
				snes_ntsc_fx_t den = SNES_NTSC_ONE - rolloff_cos_a - rolloff_cos_a +
						SNES_NTSC_FMUL( rolloff, rolloff );
				snes_ntsc_fx_t dsf = snes_ntsc_fdiv( num, den );
				kernels [kernel_size * 3 / 2 - kernel_half + i] = dsf - SNES_NTSC_C_050;
			}
		}

		/* apply blackman window and find sum */
		sum = 0;
		for ( i = 0; i < kernel_half * 2 + 1; i++ )
		{
			snes_ntsc_fx_t x = SNES_NTSC_FMUL( snes_ntsc_fdiv( SNES_NTSC_2PI, (kernel_half * 2) * SNES_NTSC_ONE ),
					(snes_ntsc_fx_t)i * SNES_NTSC_ONE );
			snes_ntsc_fx_t blackman = SNES_NTSC_C_042 - SNES_NTSC_FMUL( SNES_NTSC_C_050, snes_ntsc_cos( x ) ) +
					SNES_NTSC_FMUL( SNES_NTSC_C_008, snes_ntsc_cos( SNES_NTSC_FMUL( x, 2 * SNES_NTSC_ONE ) ) );
			kernels [kernel_size * 3 / 2 - kernel_half + i] =
					SNES_NTSC_FMUL( kernels [kernel_size * 3 / 2 - kernel_half + i], blackman );
			sum += kernels [kernel_size * 3 / 2 - kernel_half + i];
		}

		/* normalize kernel */
		sum = snes_ntsc_fdiv( SNES_NTSC_ONE, sum );
		for ( i = 0; i < kernel_half * 2 + 1; i++ )
		{
			int x = kernel_size * 3 / 2 - kernel_half + i;
			kernels [x] = SNES_NTSC_FMUL( kernels [x], sum );
		}
	}

	/* generate chroma (iq) filter using gaussian kernel */
	{
		snes_ntsc_fx_t const cutoff_factor = SNES_NTSC_C_N003125;
		snes_ntsc_fx_t cutoff = setup->bleed;
		int i;

		if ( cutoff < 0 )
		{
			/* keep extreme value accessible only near upper end of scale (1.0) */
			cutoff = SNES_NTSC_FMUL( cutoff, cutoff );
			cutoff = SNES_NTSC_FMUL( cutoff, cutoff );
			cutoff = SNES_NTSC_FMUL( cutoff, cutoff );
			cutoff = SNES_NTSC_FMUL( cutoff, snes_ntsc_fdiv( SNES_NTSC_C_N30, SNES_NTSC_C_065 ) );
		}
		cutoff = cutoff_factor - SNES_NTSC_FMUL( SNES_NTSC_FMUL( SNES_NTSC_C_065, cutoff_factor ), cutoff );

		for ( i = -kernel_half; i <= kernel_half; i++ )
			kernels [kernel_size / 2 + i] = snes_ntsc_exp( SNES_NTSC_FMUL( (snes_ntsc_fx_t)(i * i) * SNES_NTSC_ONE, cutoff ) );

		/* normalize even and odd phases separately */
		for ( i = 0; i < 2; i++ )
		{
			snes_ntsc_fx_t sum = 0;
			int x;
			for ( x = i; x < kernel_size; x += 2 )
				sum += kernels [x];

			sum = snes_ntsc_fdiv( SNES_NTSC_ONE, sum );
			for ( x = i; x < kernel_size; x += 2 )
				kernels [x] = SNES_NTSC_FMUL( kernels [x], sum );
		}
	}

	/* generate linear rescale kernels */
	#if rescale_out > 1
	{
		snes_ntsc_fx_t weight = SNES_NTSC_ONE;
		snes_ntsc_fx_t* out = impl->kernel;
		int n = rescale_out;
		do
		{
			snes_ntsc_fx_t remain = 0;
			int i;
			weight -= snes_ntsc_fdiv( SNES_NTSC_ONE, rescale_in * SNES_NTSC_ONE );
			for ( i = 0; i < kernel_size * 2; i++ )
			{
				snes_ntsc_fx_t cur = kernels [i];
				snes_ntsc_fx_t m = SNES_NTSC_FMUL( cur, weight );
				*out++ = m + remain;
				remain = cur - m;
			}
		}
		while ( --n );
	}
	#endif
}

static snes_ntsc_fx_t const default_decoder [6] =
	{ SNES_NTSC_D0, SNES_NTSC_D1, SNES_NTSC_D2, SNES_NTSC_D3, SNES_NTSC_D4, SNES_NTSC_D5 };

static void init( init_t* impl, snes_ntsc_setup_t const* setup )
{
	impl->brightness = SNES_NTSC_FMUL( setup->brightness, SNES_NTSC_FMUL( SNES_NTSC_C_050, rgb_unit * SNES_NTSC_ONE ) ) + rgb_offset;
	impl->contrast   = SNES_NTSC_FMUL( setup->contrast,   SNES_NTSC_FMUL( SNES_NTSC_C_050, rgb_unit * SNES_NTSC_ONE ) ) + rgb_unit * SNES_NTSC_ONE;
	#ifdef default_palette_contrast
		if ( !setup->palette )
			impl->contrast = SNES_NTSC_FMUL( impl->contrast, default_palette_contrast );
	#endif

	impl->artifacts = setup->artifacts;
	if ( impl->artifacts > 0 )
		impl->artifacts = SNES_NTSC_FMUL( impl->artifacts, artifacts_max - artifacts_mid );
	impl->artifacts = SNES_NTSC_FMUL( impl->artifacts, artifacts_mid ) + artifacts_mid;

	impl->fringing = setup->fringing;
	if ( impl->fringing > 0 )
		impl->fringing = SNES_NTSC_FMUL( impl->fringing, fringing_max - fringing_mid );
	impl->fringing = SNES_NTSC_FMUL( impl->fringing, fringing_mid ) + fringing_mid;

	init_filters( impl, setup );

	/* generate gamma table */
	if ( gamma_size > 1 )
	{
		snes_ntsc_fx_t const to_float = snes_ntsc_fdiv( SNES_NTSC_ONE, (gamma_size - (gamma_size > 1)) * SNES_NTSC_ONE );
		snes_ntsc_fx_t const gamma = SNES_NTSC_C_11333 - SNES_NTSC_FMUL( setup->gamma, SNES_NTSC_C_050 );
		/* match common PC's 2.2 gamma to TV's 2.65 gamma */
		int i;
		for ( i = 0; i < gamma_size; i++ )
			impl->to_float [i] =
					SNES_NTSC_FMUL( snes_ntsc_pow( SNES_NTSC_FMUL( (snes_ntsc_fx_t)i * SNES_NTSC_ONE, to_float ), gamma ),
							impl->contrast ) + impl->brightness;
	}

	/* setup decoder matricies */
	{
		snes_ntsc_fx_t hue = SNES_NTSC_FMUL( setup->hue, SNES_NTSC_PI ) +
				SNES_NTSC_FMUL( snes_ntsc_fdiv( SNES_NTSC_PI, 180 * SNES_NTSC_ONE ), ext_decoder_hue * SNES_NTSC_ONE );
		snes_ntsc_fx_t sat = setup->saturation + SNES_NTSC_ONE;
		snes_ntsc_fx_t const* decoder = setup->decoder_matrix;
		if ( !decoder )
		{
			decoder = default_decoder;
			if ( STD_HUE_CONDITION( setup ) )
				hue += SNES_NTSC_FMUL( snes_ntsc_fdiv( SNES_NTSC_PI, 180 * SNES_NTSC_ONE ),
						(std_decoder_hue - ext_decoder_hue) * SNES_NTSC_ONE );
		}

		{
			snes_ntsc_fx_t s = SNES_NTSC_FMUL( snes_ntsc_sin( hue ), sat );
			snes_ntsc_fx_t c = SNES_NTSC_FMUL( snes_ntsc_cos( hue ), sat );
			snes_ntsc_fx_t* out = impl->to_rgb;
			int n;

			n = burst_count;
			do
			{
				snes_ntsc_fx_t const* in = decoder;
				int n2 = 3;
				do
				{
					snes_ntsc_fx_t i = *in++;
					snes_ntsc_fx_t q = *in++;
					*out++ = SNES_NTSC_FMUL( i, c ) - SNES_NTSC_FMUL( q, s );
					*out++ = SNES_NTSC_FMUL( i, s ) + SNES_NTSC_FMUL( q, c );
				}
				while ( --n2 );
				if ( burst_count <= 1 )
					break;
				ROTATE_IQ( s, c, SNES_NTSC_C_0866025, SNES_NTSC_C_N05 ); /* +120 degrees */
			}
			while ( --n );
		}
	}
}

/* kernel generation */

#define RGB_TO_YIQ( r, g, b, y, i ) (\
	(y = SNES_NTSC_FMUL( r, SNES_NTSC_Y_R ) + SNES_NTSC_FMUL( g, SNES_NTSC_Y_G ) + SNES_NTSC_FMUL( b, SNES_NTSC_Y_B )),\
	(i = SNES_NTSC_FMUL( r, SNES_NTSC_I_R ) - SNES_NTSC_FMUL( g, SNES_NTSC_I_G ) - SNES_NTSC_FMUL( b, SNES_NTSC_I_B )),\
	(SNES_NTSC_FMUL( r, SNES_NTSC_Q_R ) - SNES_NTSC_FMUL( g, SNES_NTSC_Q_G ) + SNES_NTSC_FMUL( b, SNES_NTSC_Q_B ))\
)

#define YIQ_TO_RGB( y, i, q, to_rgb, type, r, g ) (\
	r = (type) snes_ntsc_toint( y + SNES_NTSC_FMUL( to_rgb [0], i ) + SNES_NTSC_FMUL( to_rgb [1], q ) ),\
	g = (type) snes_ntsc_toint( y + SNES_NTSC_FMUL( to_rgb [2], i ) + SNES_NTSC_FMUL( to_rgb [3], q ) ),\
	(type) snes_ntsc_toint( y + SNES_NTSC_FMUL( to_rgb [4], i ) + SNES_NTSC_FMUL( to_rgb [5], q ) )\
)

#define PACK_RGB( r, g, b ) ((r) << 21 | (g) << 11 | (b) << 1)

enum { rgb_kernel_size = burst_size / alignment_count };
enum { rgb_bias = rgb_unit * 2 * snes_ntsc_rgb_builder };

typedef struct pixel_info_t
{
	int offset;
	snes_ntsc_fx_t negate;
	snes_ntsc_fx_t kernel [4];
} pixel_info_t;

#if rescale_in > 1
	#define PIXEL_OFFSET_( ntsc, scaled ) \
		(kernel_size / 2 + ntsc + (scaled != 0) + (rescale_out - scaled) % rescale_out + \
				(kernel_size * 2 * scaled))

	#define PIXEL_OFFSET( ntsc, scaled ) \
		PIXEL_OFFSET_( ((ntsc) - (scaled) / rescale_out * rescale_in),\
				(((scaled) + rescale_out * 10) % rescale_out) ),\
		(snes_ntsc_fx_t)((1 - (((ntsc) + 100) & 2)) * SNES_NTSC_ONE)
#else
	#define PIXEL_OFFSET( ntsc, scaled ) \
		(kernel_size / 2 + (ntsc) - (scaled)),\
		(snes_ntsc_fx_t)((1 - (((ntsc) + 100) & 2)) * SNES_NTSC_ONE)
#endif

extern pixel_info_t const snes_ntsc_pixels [alignment_count];

/* Generate pixel at all burst phases and column alignments */
static void gen_kernel( init_t* impl, snes_ntsc_fx_t y, snes_ntsc_fx_t i, snes_ntsc_fx_t q, snes_ntsc_rgb_t* out )
{
	/* generate for each scanline burst phase */
	snes_ntsc_fx_t const* to_rgb = impl->to_rgb;
	int burst_remain = burst_count;
	y -= rgb_offset;
	do
	{
		/* Encode yiq into *two* composite signals (to allow control over artifacting).
		Convolve these with kernels which: filter respective components, apply
		sharpening, and rescale horizontally. Convert resulting yiq to rgb and pack
		into integer. Based on algorithm by NewRisingSun. */
		pixel_info_t const* pixel = snes_ntsc_pixels;
		int alignment_remain = alignment_count;
		do
		{
			/* negate is -1 when composite starts at odd multiple of 2 */
			snes_ntsc_fx_t const yy = SNES_NTSC_FMUL( SNES_NTSC_FMUL( y, impl->fringing ), pixel->negate );
			snes_ntsc_fx_t const ic0 = SNES_NTSC_FMUL( i + yy, pixel->kernel [0] );
			snes_ntsc_fx_t const qc1 = SNES_NTSC_FMUL( q + yy, pixel->kernel [1] );
			snes_ntsc_fx_t const ic2 = SNES_NTSC_FMUL( i - yy, pixel->kernel [2] );
			snes_ntsc_fx_t const qc3 = SNES_NTSC_FMUL( q - yy, pixel->kernel [3] );

			snes_ntsc_fx_t const factor = SNES_NTSC_FMUL( impl->artifacts, pixel->negate );
			snes_ntsc_fx_t const ii = SNES_NTSC_FMUL( i, factor );
			snes_ntsc_fx_t const yc0 = SNES_NTSC_FMUL( y + ii, pixel->kernel [0] );
			snes_ntsc_fx_t const yc2 = SNES_NTSC_FMUL( y - ii, pixel->kernel [2] );

			snes_ntsc_fx_t const qq = SNES_NTSC_FMUL( q, factor );
			snes_ntsc_fx_t const yc1 = SNES_NTSC_FMUL( y + qq, pixel->kernel [1] );
			snes_ntsc_fx_t const yc3 = SNES_NTSC_FMUL( y - qq, pixel->kernel [3] );

			snes_ntsc_fx_t const* k = &impl->kernel [pixel->offset];
			int n;
			++pixel;
			for ( n = rgb_kernel_size; n; --n )
			{
				snes_ntsc_fx_t i = SNES_NTSC_FMUL( k[0], ic0 ) + SNES_NTSC_FMUL( k[2], ic2 );
				snes_ntsc_fx_t q = SNES_NTSC_FMUL( k[1], qc1 ) + SNES_NTSC_FMUL( k[3], qc3 );
				snes_ntsc_fx_t y = SNES_NTSC_FMUL( k[kernel_size+0], yc0 ) + SNES_NTSC_FMUL( k[kernel_size+1], yc1 ) +
				          SNES_NTSC_FMUL( k[kernel_size+2], yc2 ) + SNES_NTSC_FMUL( k[kernel_size+3], yc3 ) + rgb_offset;
				if ( rescale_out <= 1 )
					k--;
				else if ( k < &impl->kernel [kernel_size * 2 * (rescale_out - 1)] )
					k += kernel_size * 2 - 1;
				else
					k -= kernel_size * 2 * (rescale_out - 1) + 2;
				{
					int r, g, b = YIQ_TO_RGB( y, i, q, to_rgb, int, r, g );
					*out++ = PACK_RGB( r, g, b ) - rgb_bias;
				}
			}
		}
		while ( alignment_count > 1 && --alignment_remain );

		if ( burst_count <= 1 )
			break;

		to_rgb += 6;

		ROTATE_IQ( i, q, SNES_NTSC_C_N0866025, SNES_NTSC_C_N05 ); /* -120 degrees */
	}
	while ( --burst_remain );
}

static void correct_errors( snes_ntsc_rgb_t color, snes_ntsc_rgb_t* out );

#if DISABLE_CORRECTION
	#define CORRECT_ERROR( a ) { out [i] += rgb_bias; }
	#define DISTRIBUTE_ERROR( a, b, c ) { out [i] += rgb_bias; }
#else
	#define CORRECT_ERROR( a ) { out [a] += error; }
	#define DISTRIBUTE_ERROR( a, b, c ) {\
		snes_ntsc_rgb_t fourth = (error + 2 * snes_ntsc_rgb_builder) >> 2;\
		fourth &= (rgb_bias >> 1) - snes_ntsc_rgb_builder;\
		fourth -= rgb_bias >> 2;\
		out [a] += fourth;\
		out [b] += fourth;\
		out [c] += fourth;\
		out [i] += error - (fourth * 3);\
	}
#endif

#define RGB_PALETTE_OUT( rgb, out_ )\
{\
	unsigned char* out = (out_);\
	snes_ntsc_rgb_t clamped = (rgb);\
	SNES_NTSC_CLAMP_( clamped, (8 - rgb_bits) );\
	out [0] = (unsigned char) (clamped >> 21);\
	out [1] = (unsigned char) (clamped >> 11);\
	out [2] = (unsigned char) (clamped >>  1);\
}

/* blitter related */

#ifndef restrict
	#if defined (__GNUC__)
		#define restrict __restrict__
	#elif defined (_MSC_VER) && _MSC_VER > 1300
		#define restrict __restrict
	#else
		/* no support for restricted pointers */
		#define restrict
	#endif
#endif

#include <limits.h>

#if SNES_NTSC_OUT_DEPTH <= 16
	#if USHRT_MAX == 0xFFFF
		typedef unsigned short snes_ntsc_out_t;
	#else
		#error "Need 16-bit int type"
	#endif

#else
	#if UINT_MAX == 0xFFFFFFFF
		typedef unsigned int  snes_ntsc_out_t;
	#elif ULONG_MAX == 0xFFFFFFFF
		typedef unsigned long snes_ntsc_out_t;
	#else
		#error "Need 32-bit int type"
	#endif

#endif
