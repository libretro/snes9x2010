/* Included at the beginning of library source files, after all other #include lines.
Sets up helpful macros and services used in my source code. They don't need
module an annoying module prefix on their names since they are defined after
all other #include lines. */

// snes_spc 0.9.0
#ifndef BLARGG_SOURCE_H
#define BLARGG_SOURCE_H

// Avoid any macros which evaluate their arguments multiple times
#undef min
#undef max

#define DEF_MIN_MAX( type ) \
	static inline type min( type x, type y ) { if ( x < y ) return x; return y; }\
	static inline type max( type x, type y ) { if ( y < x ) return x; return y; }

DEF_MIN_MAX( int )
DEF_MIN_MAX( unsigned )
DEF_MIN_MAX( long )
DEF_MIN_MAX( unsigned long )
DEF_MIN_MAX( float )
DEF_MIN_MAX( double )

#undef DEF_MIN_MAX

// BLARGG_SOURCE_BEGIN: If defined, #included, allowing redefition of dprintf and check
#ifdef BLARGG_SOURCE_BEGIN
	#include BLARGG_SOURCE_BEGIN
#endif

#endif
