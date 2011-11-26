// Sets up common environment for Shay Green's libraries.
// To change configuration options, modify blargg_config.h, not this file.

// snes_spc 0.9.0
#ifndef BLARGG_COMMON_H
#define BLARGG_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

#undef BLARGG_COMMON_H
// allow blargg_config.h to #include blargg_common.h
#include "blargg_config.h"
#ifndef BLARGG_COMMON_H
#define BLARGG_COMMON_H

// blargg_err_t (0 on success, otherwise error string)
#ifndef blargg_err_t
	typedef const char* blargg_err_t;
#endif

#ifndef BLARGG_DISABLE_NOTHROW
	// throw spec mandatory in ISO C++ if operator new can return NULL
	#if __cplusplus >= 199711 || defined (__GNUC__)
		#define BLARGG_THROWS( spec ) throw spec
	#else
		#define BLARGG_THROWS( spec )
	#endif
	#define BLARGG_DISABLE_NOTHROW \
		void* operator new ( size_t s ) BLARGG_THROWS(()) { return malloc( s ); }\
		void operator delete ( void* p ) { free( p ); }
	#define BLARGG_NEW new
#else
	#include <new>
	#define BLARGG_NEW new (std::nothrow)
#endif

// BLARGG_COMPILER_HAS_BOOL: If 0, provides bool support for old compiler. If 1,
// compiler is assumed to support bool. If undefined, availability is determined.
#ifndef BLARGG_COMPILER_HAS_BOOL
	#if defined (__MWERKS__)
		#if !__option(bool)
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (_MSC_VER)
		#if _MSC_VER < 1100
			#define BLARGG_COMPILER_HAS_BOOL 0
		#endif
	#elif defined (__GNUC__)
		// supports bool
	#elif __cplusplus < 199711
		#define BLARGG_COMPILER_HAS_BOOL 0
	#endif
#endif
#if defined (BLARGG_COMPILER_HAS_BOOL) && !BLARGG_COMPILER_HAS_BOOL
	// If you get errors here, modify your blargg_config.h file
	typedef int bool;
	const bool true  = 1;
	const bool false = 0;
#endif

#if UINT_MAX < 0xFFFFFFFF || ULONG_MAX == 0xFFFFFFFF
	typedef unsigned long blargg_ulong;
#else
	typedef unsigned blargg_ulong;
#endif

// BOOST::int8_t etc.

// HAVE_STDINT_H: If defined, use <stdint.h> for int8_t etc.
#if defined (HAVE_STDINT_H)
	#include <stdint.h>
	#define BOOST

// HAVE_INTTYPES_H: If defined, use <stdint.h> for int8_t etc.
#elif defined (HAVE_INTTYPES_H)
	#include <inttypes.h>
	#define BOOST

#else
	struct BOOST
	{
		#if UCHAR_MAX == 0xFF && SCHAR_MAX == 0x7F
			typedef signed char     int8_t;
			typedef unsigned char   uint8_t;
		#else
			// No suitable 8-bit type available
			typedef struct see_blargg_common_h int8_t;
			typedef struct see_blargg_common_h uint8_t;
		#endif
		
		#if USHRT_MAX == 0xFFFF
			typedef short           int16_t;
			typedef unsigned short  uint16_t;
		#else
			// No suitable 16-bit type available
			typedef struct see_blargg_common_h int16_t;
			typedef struct see_blargg_common_h uint16_t;
		#endif
		
		#if ULONG_MAX == 0xFFFFFFFF
			typedef long            int32_t;
			typedef unsigned long   uint32_t;
		#elif UINT_MAX == 0xFFFFFFFF
			typedef int             int32_t;
			typedef unsigned int    uint32_t;
		#else
			// No suitable 32-bit type available
			typedef struct see_blargg_common_h int32_t;
			typedef struct see_blargg_common_h uint32_t;
		#endif
	};
#endif

#endif
#endif

