/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-2002 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* @(#) $Id$ */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include "zlib.h"

#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef unsigned long  ulg;

extern const char *z_errmsg[10]; /* indexed by 2-zlib_error */
/* (size given to avoid silly warnings with Visual C++) */

#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
  return (strm->msg = (char*)ERR_MSG(err), (err))
/* To be used only when the state is known to be valid */

        /* common constants */

#ifndef DEF_WBITS
#  define DEF_WBITS MAX_WBITS
#endif
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

        /* target dependencies */

#ifdef WIN32 /* Window 95 & Windows NT */
#  define OS_CODE  0x0b
#endif

#if defined(ATARI) || defined(atarist)
#  define OS_CODE  0x05
#endif

#if defined(MACOS) || defined(TARGET_OS_MAC)
#  define OS_CODE  0x07
#  if defined(__MWERKS__) && __dest_os != __be_os && __dest_os != __win32_os
#    include <unix.h> /* for fdopen */
#  else
#    ifndef fdopen
#      define fdopen(fd,mode) NULL /* No fdopen() */
#    endif
#  endif
#endif

#ifdef __50SERIES /* Prime/PRIMOS */
#  define OS_CODE  0x0F
#endif

#ifdef TOPS20
#  define OS_CODE  0x0a
#endif

#if defined(_BEOS_) || defined(RISCOS)
#  define fdopen(fd,mode) NULL /* No fdopen() */
#endif

#if (defined(_MSC_VER) && (_MSC_VER > 600))
#  define fdopen(fd,type)  _fdopen(fd,type)
#endif


        /* Common defaults */

#ifndef OS_CODE
#  define OS_CODE  0x03  /* assume Unix */
#endif

/* functions */

#ifdef HAVE_STRERROR
   extern char *strerror OF((int));
#  define zstrerror(errnum) strerror(errnum)
#else
#  define zstrerror(errnum) ""
#endif

typedef uLong (ZEXPORT *check_func) OF((uLong check, const Bytef *buf,
				       uInt len));
voidpf zcalloc OF((voidpf opaque, unsigned items, unsigned size));
void   zcfree  OF((voidpf opaque, voidpf ptr));

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* _Z_UTIL_H */
