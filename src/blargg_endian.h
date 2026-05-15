/* CPU Byte Order Utilities */

/* snes_spc 0.9.0 */
#ifndef BLARGG_ENDIAN
#define BLARGG_ENDIAN

#include <stdint.h>

#include "boolean.h"

/* Endianness is a compile-time property; see port.h for the policy.
 *
 *   MSB_FIRST defined     -> host is big endian
 *   MSB_FIRST not defined -> host is little endian
 *
 * On little-endian hosts the SPC700's little-endian words match the
 * host, so a 16/32-bit access is a single (possibly unaligned) load
 * or store. On big-endian PowerPC we use the byte-reversed
 * load/store instructions. Anywhere else big endian, we fall back
 * to manual byte assembly. */

#ifndef MSB_FIRST

#define GET_LE16(addr)		(*(uint16_t *) (addr))
#define GET_LE32(addr)		(*(uint32_t *) (addr))
#define SET_LE16(addr, data)	((void) (*(uint16_t *) (addr) = (uint16_t) (data)))
#define SET_LE32(addr, data)	((void) (*(uint32_t *) (addr) = (uint32_t) (data)))

#else

#if defined(__SNC__)
#include <ppu_intrinsics.h>
#define GET_LE16(addr)		(__builtin_lhbrx(addr, 0))
#define GET_LE32(addr)		(__builtin_lwbrx(addr, 0))
#define SET_LE16(addr, in)	(__builtin_sthbrx(in, addr, 0))
#define SET_LE32(addr, in)	(__builtin_stwbrx(in, addr, 0))
#elif defined(_XBOX360)
#include <PPCIntrinsics.h>
#define GET_LE16(addr)		(__loadshortbytereverse(0, addr))
#define GET_LE32(addr)		(__loadwordbytereverse(0, addr))
#define SET_LE16(addr, in)	(__storeshortbytereverse(in, 0, addr))
#define SET_LE32(addr, in)	(__storewordbytereverse(in, 0, addr))
#elif defined(__MWERKS__)
#define GET_LE16(addr)		(__lhbrx(addr, 0))
#define GET_LE32(addr)		(__lwbrx(addr, 0))
#define SET_LE16(addr, in)	(__sthbrx(in, addr, 0))
#define SET_LE32(addr, in)	(__stwbrx(in, addr, 0))
#elif (defined(__GNUC__) || defined(__SNC__)) && (defined(__powerpc__) || defined(__ppc__) || defined(__POWERPC__))
#define GET_LE16(addr)		({unsigned ppc_lhbrx_; asm( "lhbrx %0,0,%1" : "=r" (ppc_lhbrx_) : "r" (addr), "0" (ppc_lhbrx_) ); ppc_lhbrx_;})
#define GET_LE32(addr)		({unsigned ppc_lwbrx_; asm( "lwbrx %0,0,%1" : "=r" (ppc_lwbrx_) : "r" (addr), "0" (ppc_lwbrx_) ); ppc_lwbrx_;})
#define SET_LE16(addr, in)	({asm( "sthbrx %0,0,%1" : : "r" (in), "r" (addr) );})
#define SET_LE32(addr, in)	({asm( "stwbrx %0,0,%1" : : "r" (in), "r" (addr) );})
#else
/* Generic big-endian fallback: assemble/disassemble byte by byte. */
#define GET_LE16(addr) \
	((uint16_t) (((unsigned char const *) (addr))[0]) \
	 | ((uint16_t) (((unsigned char const *) (addr))[1]) << 8))
#define GET_LE32(addr) \
	((uint32_t) (((unsigned char const *) (addr))[0]) \
	 | ((uint32_t) (((unsigned char const *) (addr))[1]) << 8) \
	 | ((uint32_t) (((unsigned char const *) (addr))[2]) << 16) \
	 | ((uint32_t) (((unsigned char const *) (addr))[3]) << 24))
#define SET_LE16(addr, data) \
	((void) (((unsigned char *) (addr))[0] = (unsigned char) (data), \
		 ((unsigned char *) (addr))[1] = (unsigned char) ((data) >> 8)))
#define SET_LE32(addr, data) \
	((void) (((unsigned char *) (addr))[0] = (unsigned char) (data), \
		 ((unsigned char *) (addr))[1] = (unsigned char) ((data) >> 8), \
		 ((unsigned char *) (addr))[2] = (unsigned char) ((data) >> 16), \
		 ((unsigned char *) (addr))[3] = (unsigned char) ((data) >> 24)))
#endif

#endif /* MSB_FIRST */

#define GET_LE16SA(addr)	((int16_t) GET_LE16(addr))
#define GET_LE16A(addr)		GET_LE16(addr)
#define SET_LE16A(addr, data)	SET_LE16(addr, data)

#endif
