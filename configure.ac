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
	AC_MSG_ERROR([SDL is required and could not be found!])
fi

CPPFLAGSBAK="$CPPFLAGS"
CPPFLAGS=`$SDL_CONFIG --cflags`
AC_CHECK_HEADER("SDL_mixer.h", , nosdlmixer=1)
CPPFLAGS="$CPPFLAGSBAK"

if test -n "$nosdlmixer"; then
	AC_MSG_ERROR([SDL_Mixer is required and could not be found!])
fi

CPPFLAGSBAK="$CPPFLAGS"
CPPFLAGS=`$SDL_CONFIG --cflags`
AC_CHECK_HEADER("SDL_sound.h", , nosdlsound=1)
CPPFLAGS="$CPPFLAGSBAK"

if test -n "$nosdlsound"; then
	AC_MSG_ERROR([SDL_Sound is required and could not be found!])
fi

AC_C_CONST
AC_HEADER_STDC

AC_C_BIGENDIAN()

BOOST_REQUIRE
BOOST_STRING_ALGO
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_REGEX

AX_CHECK_GL

EOS_CXXFLAGS="-DUNIX `sdl-config --cflags`"
EOS_LIBS="`sdl-config --libs` -lSDL_mixer -lSDL_sound"

AC_SUBST(SDL_MAIN_OBJ, [$SDL_MAIN_OBJ])
AC_SUBST(SDL_LIBS, [$SDL_LIBS])
AC_SUBST(EOS_CXXFLAGS)
AC_SUBST(EOS_LIBS)

AC_OUTPUT(src/common/Makefile \
          src/graphics/Makefile \
          src/sound/Makefile \
          src/events/Makefile \
          src/aurora/Makefile \
          src/engines/Makefile \
          src/engines/nwn/Makefile \
          src/engines/nwn2/Makefile \
          src/engines/kotor/Makefile \
          src/engines/kotor2/Makefile \
          src/engines/thewitcher/Makefile \
          src/Makefile \
          Makefile)
