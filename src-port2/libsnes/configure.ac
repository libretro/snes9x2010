AC_PREREQ([2.60])
AC_INIT([Snes9x], [1.53], [], [snes9x])
AC_REVISION([$Revision: 1.53 $])

AC_CONFIG_SRCDIR([unix.cpp])

AC_CANONICAL_TARGET

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])

S9XFLGS=""
S9XDEFS=""
S9XLIBS=""

AC_DEFUN([AC_S9X_COMPILER_FLAG],
[
	AC_MSG_CHECKING([whether g++ accepts $1])

	AC_CACHE_VAL([snes9x_cv_option_$2],
	[
		OLD_CXXFLAGS="[$]CXXFLAGS"
		CXXFLAGS="[$]OLD_CXXFLAGS $1"

		AC_TRY_RUN(
		[
			int	foo;

			int	main (int argc, char **argv)
			{
				/* The following code triggs gcc:s generation of aline opcodes,
				   which some versions of as does not support. */

				if (argc > 0)
					argc = 0;

				return (argc);
			}
		],
		[snes9x_cv_option_$2="yes"], [snes9x_cv_option_$2="no"])
	])

	CXXFLAGS="[$]OLD_CXXFLAGS"

	if test "x[$]snes9x_cv_option_$2" = "xyes"; then
		S9XFLGS="[$]S9XFLGS $1"
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		$3
	fi
])

# *****************************
# *** Execution begins here ***
# *****************************

# Remove -g and -O2 flags manually.

if test "x$CFLAGS" != "x"; then
	CFLAGS="`echo \"$CFLAGS\" | sed -e 's/-g//'`"
	CFLAGS="`echo \"$CFLAGS\" | sed -e 's/-O2//'`"
fi

if test "x$CXXFLAGS" != "x"; then
	CXXFLAGS="`echo \"$CXXFLAGS\" | sed -e 's/-g//'`"
	CXXFLAGS="`echo \"$CXXFLAGS\" | sed -e 's/-O2//'`"
fi

# Test what compiler flags we should use.

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug],
		[leave debug information in the final binary (default: no)])],
	[], [enable_debug="no"])

if test "x$enable_debug" = "xyes"; then
	AC_S9X_COMPILER_FLAG([-g],  [g])
	AC_S9X_COMPILER_FLAG([-O0], [o0])
else
	AC_S9X_COMPILER_FLAG([-O3], [o3], [
		AC_S9X_COMPILER_FLAG([-O2], [o2], [
			AC_S9X_COMPILER_FLAG([-O1], [o1])])])
	AC_S9X_COMPILER_FLAG([-fomit-frame-pointer], [omit_frame_pointer])
fi

AC_ARG_ENABLE([mtune],
	[AS_HELP_STRING([--enable-mtune],
		[use the specified value for the -mtune/-mcpu flag (default: no)])],
	[], [enable_mtune="no"])

if test "x$enable_mtune" != "xno"; then
	AC_S9X_COMPILER_FLAG([-mtune="$enable_mtune"], [mtune],
	[
		AC_MSG_WARN([-mtune failed, trying -mcpu...])
		AC_S9X_COMPILER_FLAG([-mcpu="$enable_mtune"], [mcpu],
			[AC_MSG_ERROR([Please specify a working value for --enable-mtune.])])
	])
fi

AC_S9X_COMPILER_FLAG([-fno-exceptions],       [no_exceptions])
AC_S9X_COMPILER_FLAG([-fno-rtti],             [no_rtti])
AC_S9X_COMPILER_FLAG([-pedantic],             [pedantic])
AC_S9X_COMPILER_FLAG([-Wall],                 [Wall])
AC_S9X_COMPILER_FLAG([-W],                    [W])
AC_S9X_COMPILER_FLAG([-Wno-unused-parameter], [Wno_unused_parameter])

# Check if the OS is Linux.

AC_MSG_CHECKING([whether the OS is Linux])

AC_CACHE_VAL([snes9x_cv_linux_os],
[
	case "$target" in
		*-*-linux*)
			snes9x_cv_linux_os="yes"
			;;
		*)
			snes9x_cv_linux_os="no"
			;;
	esac
])

AC_MSG_RESULT([$snes9x_cv_linux_os])

# Check for functions

AC_CHECK_FUNC([mkstemp],
[
	S9XDEFS="$S9XDEFS -DHAVE_MKSTEMP"
])

# Check for headers

snes9x_have_stdint_h="";

AC_CHECK_HEADER([strings.h],
[
	S9XDEFS="$S9XDEFS -DHAVE_STRINGS_H"
])

AC_CHECK_HEADER([sys/ioctl.h],
[
	S9XDEFS="$S9XDEFS -DHAVE_SYS_IOCTL_H"
])

AC_CHECK_HEADER([stdint.h],
[
	S9XDEFS="$S9XDEFS -DHAVE_STDINT_H"
	snes9x_have_stdint_h="-DHAVE_STDINT_H"
])

AC_CHECK_HEADERS([unistd.h sys/socket.h])

# Check whether the size of pointer is int.

if test "x$snes9x_have_stdint_h" = "x"; then
	AC_MSG_CHECKING([whether the size of pointer is int])
	
	AC_TRY_RUN(
	[
		int main (void)
		{
			return (!(sizeof(void *) == sizeof(int)));
		}
	],
	[snes9x_ptr_is_int="yes"], [snes9x_ptr_is_int="no"])
	
	if test "x$snes9x_ptr_is_int" = "xyes"; then
		AC_MSG_RESULT(yes)
	else
		AC_MSG_RESULT(no)
		S9XDEFS="$S9XDEFS -DPTR_NOT_INT"
	fi
fi

# Check whether right shift is arithmetic or not

AC_DEFUN([AC_S9X_CHECK_SAR],
[
	AC_MSG_CHECKING([whether right shift $1 is arithmetic])

	OLD_CXXFLAGS="[$]CXXFLAGS"
	CXXFLAGS="[$]OLD_CXXFLAGS $snes9x_have_stdint_h"

	AC_TRY_RUN(
	[
		#ifdef HAVE_STDINT_H
		#include <stdint.h>
		typedef int8_t			int8;
		typedef int16_t			int16;
		typedef int32_t			int32;
		typedef int64_t			int64;
		#else 
		typedef signed char		int8;
		typedef signed short	int16;
		typedef signed int		int32;
		#ifdef __GNUC__
		__extension__
		#endif
		typedef long long		int64;
		#endif

		int main (void)
		{
			$1	i;

			i = -1;
			i >>= 1;

			return (i < 0 ? 0 : 1);
		}
	],
	[snes9x_sar_$1="yes"], [snes9x_sar_$1="no"])

	CXXFLAGS="[$]OLD_CXXFLAGS"

	if test "x$snes9x_sar_$1" = "xno"; then
		AC_MSG_RESULT([no])
	else
		S9XDEFS="$S9XDEFS -DRIGHTSHIFT_$1_IS_SAR"
		AC_MSG_RESULT([yes])
	fi
])

AC_S9X_CHECK_SAR([int8])
AC_S9X_CHECK_SAR([int16])
AC_S9X_CHECK_SAR([int32])
AC_S9X_CHECK_SAR([int64])

if test "x$snes9x_sar_int8" = "xyes" -a "x$snes9x_sar_int16" = "xyes" -a "x$snes9x_sar_int32" = "xyes" -a "x$snes9x_sar_int64" = "xyes"; then
	S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/-DRIGHTSHIFT_int8_IS_SAR//'`"
	S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/-DRIGHTSHIFT_int16_IS_SAR//'`"
	S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/-DRIGHTSHIFT_int32_IS_SAR//'`"
	S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/-DRIGHTSHIFT_int64_IS_SAR//'`"
	S9XDEFS="$S9XDEFS -DRIGHTSHIFT_IS_SAR"
fi

# Check if we have sound code for this platform.

AC_ARG_ENABLE([sound],
	[AS_HELP_STRING([--enable-sound],
		[enable sound if available (default: yes)])],
	[], [enable_sound="yes"])

if test "x$enable_sound" = "xyes"; then
	AC_MSG_CHECKING([whether sound is supported on this platform])
	if test "x$snes9x_cv_linux_os" = "xyes"; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		AC_MSG_WARN([Your OS is not Linux. Build without sound support.])
		enable_sound="no"
	fi
fi

if test "x$enable_sound" = "xyes"; then
	AC_CHECK_HEADER([pthread.h],
	[
		S9XDEFS="$S9XDEFS -DUSE_THREADS"
		S9XLIBS="$S9XLIBS -lpthread"
	])
else
	S9XDEFS="$S9XDEFS -DNOSOUND"
fi

# Output.

S9XFLGS="$CXXFLAGS $CPPFLAGS $LDFLAGS $S9XFLGS"
S9XLIBS="$LIBS $S9XLIBS"

S9XFLGS="`echo \"$S9XFLGS\" | sed -e 's/  */ /g'`"
S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/  */ /g'`"
S9XLIBS="`echo \"$S9XLIBS\" | sed -e 's/  */ /g'`"
S9XFLGS="`echo \"$S9XFLGS\" | sed -e 's/^  *//'`"
S9XDEFS="`echo \"$S9XDEFS\" | sed -e 's/^  *//'`"
S9XLIBS="`echo \"$S9XLIBS\" | sed -e 's/^  *//'`"

AC_SUBST(S9XFLGS)
AC_SUBST(S9XDEFS)
AC_SUBST(S9XLIBS)
AC_SUBST(S9XZIP)

rm config.info 2>/dev/null

cat >config.info <<EOF

build information:
cc...............,,,. $CC
c++.................. $CXX
options.............. $S9XFLGS
defines.............. $S9XDEFS
libs................. $S9XLIBS

features:
GZIP support......... $enable_gzip
ZIP support.......... $enable_zip

EOF

cat config.info

AC_OUTPUT(Makefile)
