AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([m4])

AC_INIT(eos, 0.0.1)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(eos, 0.0.1)

AC_ARG_VAR(SDL_CONFIG, [sdl-config utility])
AC_ARG_VAR(SDL_CFLAGS, [C compiler flags for SDL])
AC_ARG_VAR(SDL_LIBS, [libraries to pass to the linker for SDL])

AC_PROG_LIBTOOL

AC_PROG_CXX
AC_PROG_INSTALL
AC_CHECK_PROG([SDL_CONFIG], [sdl-config], [sdl-config])

if test -n "$SDL_CONFIG"; then
	if test -z "$SDL_CFLAGS"; then
		SDL_CFLAGS=`$SDL_CONFIG --cflags`
	fi
	if test -z "$SDL_LIBS"; then
	SDL_LIBS=`$SDL_CONFIG --libs`
	fi
fi

save_LIBS="$LIBS"
LIBS="$LIBS $SDL_LIBS"
AC_CHECK_FUNC(SDL_Init, , AC_CHECK_LIB(SDL, SDL_Init, SDL_LIBS="$SDL_LIBS -lSDL", nosdl=1))
LIBS="$save_LIBS"

if test -n "$nosdl"; then
	AC_MSG_WARN([SDL is required and could not be found!])
	missing_required=yes
fi

save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -x objective-c"
AC_CHECK_HEADERS(Cocoa/Cocoa.h)
CFLAGS="$save_CFLAGS"

if test "$ac_cv_header_Cocoa_Cocoa_h" = "yes"; then
	SDL_MAIN_OBJ="src/platform/macosx/SDLMain.o"
fi

AC_C_CONST
AC_HEADER_STDC

AC_C_BIGENDIAN()

BOOST_REQUIRE
BOOST_STRING_ALGO
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_REGEX

EOS_CXXFLAGS="-DUNIX"
EOS_LIBS="-lSDL"

if test "$ac_cv_header_Cocoa_Cocoa_h" = "yes"; then
	EOS_CXXFLAGS="$EOS_CXXFLAGS -DMACOSX"
else
	EOS_LIBS="$EOS_LIBS -lSDLmain"
fi

AC_SUBST(SDL_MAIN_OBJ, [$SDL_MAIN_OBJ])
AC_SUBST(SDL_LIBS, [$SDL_LIBS])
AC_SUBST(EOS_CXXFLAGS)
AC_SUBST(EOS_LIBS)

AC_OUTPUT(src/common/Makefile src/aurora/Makefile src/Makefile Makefile)
