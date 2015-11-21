#serial 1000
dnl @synopsis AX_CHECK_GLEW(action-if, action-if-not)
dnl
dnl @summary check for GLEW.
dnl
dnl Defines GLEW_LIBS, GLEW_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2015-11-21
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_GLEW], [
	AC_ARG_VAR(GLEW_CFLAGS, [C compiler flags for GLEW])
	AC_ARG_VAR(GLEW_LIBS, [libraries to pass to the linker for GLEW])

	if test -z "$GLEW_CFLAGS"; then
		GLEW_CFLAGS=""
	fi
	if test -z "$GLEW_LIBS"; then
		GLEW_LIBS="-lGLEW"
	fi

	GLEW_CFLAGS="$GLEW_CFLAGS -DGLEW_NO_GLU"

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $GLEW_CFLAGS"
	CXXFLAGS="$CXXFLAGS $GLEW_CFLAGS"
	LIBS="$LIBS $GLEW_LIBS"

	AC_CHECK_HEADER([GL/glew.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <GL/glew.h>

int main(int argc, char **argv) {
	glewInit();

	return 0;
}
]])], noglew=0, noglew=1), noglew=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$noglew" = "1"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(GLEW_CFLAGS)
	AC_SUBST(GLEW_LIBS)
])
