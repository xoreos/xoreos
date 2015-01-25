#serial 1000
dnl @synopsis AX_CHECK_SDL2(version, action-if, action-if-not)
dnl
dnl @summary check for SDL2 of sufficient version (2.1.3 -> 2103)
dnl
dnl Defines SDL2_LIBS, SDL2_CFLAGS. See sdl2-config(1) man
dnl page.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_SDL2], [
	AC_ARG_VAR(SDL2_CONFIG, [sdl2-config utility])
	AC_ARG_VAR(SDL2_CFLAGS, [C compiler flags for SDL2])
	AC_ARG_VAR(SDL2_LIBS, [libraries to pass to the linker for SDL2])

	AC_CHECK_PROG([SDL2_CONFIG], [sdl2-config], [sdl2-config])

	if test -n "$SDL2_CONFIG"; then
		if test -z "$SDL2_CFLAGS"; then
			SDL2_CFLAGS=`$SDL2_CONFIG --cflags`
		fi
		if test -z "$SDL2_LIBS"; then
			SDL2_LIBS=`$SDL2_CONFIG --libs`
		fi
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $SDL2_CFLAGS"
	CXXFLAGS="$CXXFLAGS $SDL2_CFLAGS"
	LIBS="$LIBS $SDL2_LIBS"

	AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Quit();

#if SDL_COMPILEDVERSION < $1
	#error SDL2 version too low
#endif

	return 0;
}
]])], nosdl2=0, nosdl2=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nosdl2" = "1"; then
		ifelse([$3], , :, [$3])
	else
		ifelse([$2], , :, [$2])
	fi

	AC_SUBST(SDL2_CFLAGS)
	AC_SUBST(SDL2_LIBS)
])
