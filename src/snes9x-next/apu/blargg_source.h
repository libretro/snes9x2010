/* Included at the beginning of library source files, after all other #include lines.
Sets up helpful macros and services used in my source code. They don't need
module an annoying module prefix on their names since they are defined after
all other #include lines. */

// snes_spc 0.9.0
#ifndef BLARGG_SOURCE_H
#define BLARGG_SOURCE_H

// BLARGG_SOURCE_BEGIN: If defined, #included, allowing redefition of dprintf and check
#ifdef BLARGG_SOURCE_BEGIN
	#include BLARGG_SOURCE_BEGIN
#endif

#endif

