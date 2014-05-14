dnl @synopsis AX_CHECK_MAD(major, minor, patch, action-if, action-if-not)
dnl
dnl @summary check for MAD of sufficient major, minor and patch version
dnl
dnl Defines MAD_LIBS, MAD_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_MAD], [
	AC_ARG_VAR(MAD_CFLAGS, [C compiler flags for MAD])
	AC_ARG_VAR(MAD_LIBS, [libraries to pass to the linker for MAD])

	if test -z "$MAD_CFLAGS"; then
		MAD_CFLAGS=""
	fi
	if test -z "$MAD_LIBS"; then
		MAD_LIBS="-lmad"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $MAD_CFLAGS"
	CXXFLAGS="$CXXFLAGS $MAD_CFLAGS"
	LIBS="$LIBS $MAD_LIBS"

	AC_CHECK_HEADER([mad.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <mad.h>

int main(int argc, char **argv) {
#if  (MAD_VERSION_MAJOR  < $1) || \
    ((MAD_VERSION_MAJOR == $1) && (MAD_VERSION_MINOR  < $2)) || \
    ((MAD_VERSION_MAJOR == $1) && (MAD_VERSION_MINOR == $2) && (MAD_VERSION_PATCH < $3))
	#error MAD version too low
#endif

	return 0;
}
]])], nomad=0, nomad=1), nomad=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nomad" = "1"; then
		ifelse([$5], , :, [$5])
	else
		ifelse([$4], , :, [$4])
	fi

	AC_SUBST(MAD_CFLAGS)
	AC_SUBST(MAD_LIBS)
])
