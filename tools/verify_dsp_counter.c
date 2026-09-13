/* verify_dsp_counter.c - generator and checker for apu.c's dsp_counters[].
 *
 * The S-DSP envelope and noise clocks fire when (counter + offset) is an
 * exact multiple of the rate's period.  apu.c answers that with a
 * Granlund-Montgomery divisibility triple per rate rather than a modulo by
 * a variable divisor: for period p = 2^shift * q with q odd,
 *
 *     p divides x   iff   rotr32(x * (q^-1 mod 2^32), shift) <= (2^32-1)/p
 *
 * This program regenerates the triples from the period and offset lists and
 * then checks them against the plain modulo over the entire reachable input
 * domain: every rate 0..31 crossed with every counter value 0..COUNTER_RANGE-1.
 * That domain is exhaustive, so a clean run is a proof of equivalence rather
 * than a sample of one.  It is never compiled into the core.
 *
 *   cc -O2 -o verify_dsp_counter verify_dsp_counter.c && ./verify_dsp_counter
 *
 * Pass --emit to print the table for pasting into src/apu.c.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define COUNTER_RANGE 30720

static const uint32_t periods [32] =
{
	COUNTER_RANGE + 1, 2048, 1536,
	1280, 1024,  768,
	 640,  512,  384,
	 320,  256,  192,
	 160,  128,   96,
	  80,   64,   48,
	  40,   32,   24,
	  20,   16,   12,
	  10,    8,    6,
	   5,    4,    3,
	         2,
	         1
};

static const uint32_t offsets [32] =
{
	  1, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	536, 0, 1040,
	     0,
	     0
};

typedef struct
{
	uint32_t inv;
	uint32_t limit;
	uint32_t offset;
	uint32_t shift;
} triple_t;

static triple_t table [32];

/* Newton iteration for the inverse of an odd word modulo 2^32. */
static uint32_t modinv32 ( uint32_t d )
{
	uint32_t x = d;
	int i;

	for ( i = 0; i < 5; i++ )
		x *= 2u - d * x;

	return x;
}

static void build ( void )
{
	int i;

	for ( i = 0; i < 32; i++ )
	{
		uint32_t q = periods [i];
		uint32_t s = 0;

		while ( !( q & 1u ) )
		{
			q >>= 1;
			s++;
		}

		table [i].inv    = modinv32( q );
		table [i].limit  = 0xFFFFFFFFu / periods [i];
		table [i].offset = offsets [i];
		table [i].shift  = s;
	}
}

static int fired ( int rate, uint32_t counter )
{
	const triple_t *d = &table [rate];
	uint32_t v = ( counter + d->offset ) * d->inv;

	v = ( v >> d->shift ) | ( v << ( ( 32u - d->shift ) & 31u ) );

	return v <= d->limit;
}

static int reference ( int rate, uint32_t counter )
{
	return ( ( counter + offsets [rate] ) % periods [rate] ) == 0;
}

static void emit ( void )
{
	int i;

	printf( "static const dsp_counter_t dsp_counters [32] =\n{\n" );

	for ( i = 0; i < 32; i++ )
		printf( "\t{ 0x%08Xu, 0x%08Xu, %5uu, %2uu },   /* rate %5u */\n",
			(unsigned) table [i].inv, (unsigned) table [i].limit,
			(unsigned) table [i].offset, (unsigned) table [i].shift,
			(unsigned) periods [i] );

	printf( "};\n" );
}

int main ( int argc, char **argv )
{
	long checked = 0;
	long bad = 0;
	int rate;

	build();

	if ( argc > 1 && !strcmp( argv [1], "--emit" ) )
	{
		emit();
		return 0;
	}

	for ( rate = 0; rate < 32; rate++ )
	{
		uint32_t counter;

		for ( counter = 0; counter < COUNTER_RANGE; counter++ )
		{
			int a = fired( rate, counter );
			int b = reference( rate, counter );

			checked++;

			if ( a != b )
			{
				if ( bad < 8 )
					printf( "MISMATCH rate %d counter %lu: got %d want %d\n",
						rate, (unsigned long) counter, a, b );
				bad++;
			}
		}
	}

	/* Rate 0 must never fire for any counter the DSP can hold. */
	{
		uint32_t counter;

		for ( counter = 0; counter < COUNTER_RANGE; counter++ )
		{
			if ( fired( 0, counter ) )
			{
				printf( "MISMATCH rate 0 fired at counter %lu\n",
					(unsigned long) counter );
				bad++;
			}
		}
	}

	printf( "%ld inputs checked, %ld mismatches\n", checked, bad );

	return bad != 0;
}
