#serial 1000
dnl @synopsis AX_CHECK_XVID(major, minor, patch, action-if, action-if-not)
dnl
dnl @summary check for libxvidcore of sufficient major, minor and patch version
dnl
dnl Defines XVID_LIBS, XVID_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_XVID], [
	AC_ARG_VAR(XVID_CFLAGS, [C compiler flags for libxvidcore])
	AC_ARG_VAR(XVID_LIBS, [libraries to pass to the linker for libxvidcore])

	if test -z "$XVID_CFLAGS"; then
		XVID_CFLAGS=""
	fi
	if test -z "$XVID_LIBS"; then
		XVID_LIBS="-lxvidcore"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $XVID_CFLAGS"
	CXXFLAGS="$CXXFLAGS $XVID_CFLAGS"
	LIBS="$LIBS $XVID_LIBS"

	AC_CHECK_HEADER([xvid.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <xvid.h>

int main(int argc, char **argv) {
#if XVID_VERSION < XVID_MAKE_VERSION($1,$2,$3)
	#error libxvidcore version too low
#endif

	return 0;
}
]])], noxvid=0, noxvid=1), noxvid=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$noxvid" = "1"; then
		ifelse([$5], , :, [$5])
	else
		ifelse([$4], , :, [$4])
	fi

	AC_SUBST(XVID_CFLAGS)
	AC_SUBST(XVID_LIBS)
])
