#serial 1000
dnl @synopsis AX_CHECK_ZLIB(major, minor, revision, subrevision, action-if, action-if-not)
dnl
dnl @summary check for ZLIB of sufficient major, minor, revision and subrevision version
dnl
dnl Defines ZLIB_LIBS, ZLIB_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_ZLIB], [
	AC_ARG_VAR(ZLIB_CFLAGS, [C compiler flags for zlib])
	AC_ARG_VAR(ZLIB_LIBS, [libraries to pass to the linker for zlib])

	if test -z "$ZLIB_CFLAGS"; then
		ZLIB_CFLAGS=""
	fi
	if test -z "$ZLIB_LIBS"; then
		ZLIB_LIBS="-lz"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $ZLIB_CFLAGS"
	CXXFLAGS="$CXXFLAGS $ZLIB_CFLAGS"
	LIBS="$LIBS $ZLIB_LIBS"

	AC_CHECK_HEADER([zlib.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <zlib.h>

int main(int argc, char **argv) {
#if ZLIB_VERNUM < (($1 * 4096) + ($2 * 256) + ($3 * 16) + $4)
	#error zlib version too low
#endif

	return 0;
}
]])], nozlib=0, nozlib=1), nozlib=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nozlib" = "1"; then
		ifelse([$6], , :, [$6])
	else
		ifelse([$5], , :, [$5])
	fi

	AC_SUBST(ZLIB_CFLAGS)
	AC_SUBST(ZLIB_LIBS)
])
