#!/bin/sh
# Feature-test-macro visibility check.
#
# The core is packaged for distributions that compile it under a strict
# feature profile -- the Debian/Launchpad recipe passes
# -D_XOPEN_SOURCE=600 -- which hides every POSIX.1-2008 and BSD
# interface glibc and musl keep behind __USE_XOPEN2K8/__USE_MISC.  A
# translation unit that reaches for one of those without asking for the
# profile it needs still builds here and fails there, so compile every
# source the packaging builds under each profile and reject an implicit
# declaration or an undeclared identifier.
#
# Run from the repository root:
#
#   sh tools/feature-macro-check.sh
#
# Add -v to list every file as it is compiled.

set -e

CC=${CC:-gcc}
JOBS=${JOBS:-8}
verbose=0
[ "$1" = "-v" ] && verbose=1

if [ ! -f tools/feature-macro-check.sh ]; then
   echo "run this from the repository root" >&2
   exit 1
fi

L=libretro/libretro-common
INC="-Ilibretro -Isrc -I$L/include"
DEF="-D__LIBRETRO__ -DHAVE_RPNG -DHAVE_RWEBP -DHAVE_RDDS -DINLINE=inline -DHAVE_THREADS"

# An implicit declaration is the symptom this lane exists to catch: it
# is only a warning on the compilers the PPAs still use (their recipe
# passes -Wno-error=implicit-function-declaration), and a hard error on
# GCC 14 and Clang 16 onwards.  Treat it as an error here either way.
STRICT="-Werror=implicit-function-declaration -Werror=implicit-int -Werror=int-conversion"

# Profiles a packager can plausibly hand us.  "none" is the default
# build and has to keep working; the rest are the strict ones.
PROFILES="none
-D_XOPEN_SOURCE=600
-D_XOPEN_SOURCE=700
-D_POSIX_C_SOURCE=200112L
-D_POSIX_C_SOURCE=200809L
-D_GNU_SOURCE"

SOURCES="src/apu.c src/hdpack.c src/bsx.c src/bsflash.c src/c4emu.c
src/cheats.c src/controls.c src/cpu.c src/cpuexec.c src/dsp.c src/fxemu.c
src/globals.c src/memmap.c src/obc1.c src/msu1.c src/msu1_pack.c src/ppu.c
src/sa1.c src/sdd1.c src/seta.c src/snapshot.c src/spc7110.c src/srtc.c
src/tile.c src/hwregisters.c filter/snes_ntsc.c libretro/libretro.c
$L/streams/memory_stream.c $L/rthreads/rthreads.c $L/rthreads/retro_eventcount.c
$L/formats/png/rpng.c $L/formats/png/rpng_encode.c $L/formats/png/rpng_apng.c
$L/file/rpng_file.c $L/features/features_cpu.c $L/formats/image_transfer.c
$L/formats/webp/rwebp.c $L/formats/vp8/rvp8.c $L/formats/dds/rdds.c
$L/streams/trans_stream.c $L/streams/trans_stream_pipe.c
$L/streams/trans_stream_deflate.c $L/encodings/encoding_deflate.c
$L/encodings/encoding_crc32.c $L/streams/interface_stream.c
$L/streams/rzip_stream.c $L/compat/compat_posix_string.c
$L/compat/compat_strcasestr.c $L/compat/compat_snprintf.c
$L/compat/compat_strl.c $L/compat/fopen_utf8.c $L/encodings/encoding_utf.c
$L/file/file_path.c $L/file/file_path_io.c $L/memmap/memalign.c
$L/streams/file_stream.c $L/streams/file_stream_transforms.c
$L/string/stdstring.c $L/string/rstrtod.c $L/time/rtime.c
$L/vfs/vfs_implementation.c $L/vfs/vfs_hybrid.c $L/file/retro_dirent.c"

log=$(mktemp)
trap 'rm -f "$log"' EXIT INT TERM
failed=0

for profile in $PROFILES; do
   flags=$profile
   [ "$profile" = "none" ] && flags=

   printf '%-28s ' "[$profile]"
   : > "$log"

   # shellcheck disable=SC2086
   echo $SOURCES | tr ' ' '\n' | sed '/^$/d' | \
      xargs -P "$JOBS" -I{} sh -c \
      "$CC -fsyntax-only {} $INC $DEF $flags -O2 -Wall $STRICT 2>&1 || echo '__FAILED__ {}'" \
      >> "$log" 2>&1 || true

   if grep -q '__FAILED__' "$log"; then
      echo "FAIL"
      grep -v '__FAILED__' "$log" | grep -E 'error|warning' | sed 's|^\./||'
      echo "  --- files:"
      grep '__FAILED__' "$log" | sed 's|__FAILED__|   |'
      failed=1
   else
      echo "ok"
      [ "$verbose" = 1 ] && cat "$log"
   fi
done

if [ "$failed" != 0 ]; then
   echo
   echo "A translation unit needs a feature profile the build did not give it."
   echo "Ask for it at the top of that file, before its first system header,"
   echo "raising the profile only when it is lower than what the file needs."
   exit 1
fi

echo
echo "all profiles clean"
