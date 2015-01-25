#serial 1000
dnl @synopsis AX_CHECK_LZMA(major, minor, patch, stability, action-if, action-if-not)
dnl
dnl @summary check for LZMA of sufficient major, minor, patch and stability version
dnl
dnl Stability: 0 = alpha, 1 = beta, 2 = stable
dnl Defines LZMA_LIBS, LZMA_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2014-05-06
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_LZMA], [
	AC_ARG_VAR(LZMA_CFLAGS, [C compiler flags for lzma])
	AC_ARG_VAR(LZMA_LIBS, [libraries to pass to the linker for lzma])

	if test -z "$LZMA_CFLAGS"; then
		LZMA_CFLAGS=""
	fi
	if test -z "$LZMA_LIBS"; then
		LZMA_LIBS="-llzma"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $LZMA_CFLAGS"
	CXXFLAGS="$CXXFLAGS $LZMA_CFLAGS"
	LIBS="$LIBS $LZMA_LIBS"

	AC_CHECK_HEADER([lzma.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <lzma.h>

int main(int argc, char **argv) {
#if  (LZMA_VERSION_MAJOR  < $1) || \
    ((LZMA_VERSION_MAJOR == $1) && (LZMA_VERSION_MINOR  < $2)) || \
    ((LZMA_VERSION_MAJOR == $1) && (LZMA_VERSION_MINOR == $2) && (LZMA_VERSION_PATCH  < $3)) || \
    ((LZMA_VERSION_MAJOR == $1) && (LZMA_VERSION_MINOR == $2) && (LZMA_VERSION_PATCH == $3) && (LZMA_VERSION_STABILITY < $4))
	#error lzma version too low
#endif

	return 0;
}
]])], nolzma=0, nolzma=1), nolzma=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nolzma" = "1"; then
		ifelse([$6], , :, [$6])
	else
		ifelse([$5], , :, [$5])
	fi

	AC_SUBST(LZMA_CFLAGS)
	AC_SUBST(LZMA_LIBS)
])
