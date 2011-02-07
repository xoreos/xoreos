AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([m4])

AC_INIT(eos, 0.0.1)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(eos, 0.0.1)

AC_ARG_VAR(SDL_CONFIG, [sdl-config utility])
AC_ARG_VAR(SDL_CFLAGS, [C compiler flags for SDL])
AC_ARG_VAR(SDL_LIBS, [libraries to pass to the linker for SDL])

AC_PROG_LIBTOOL

AC_PROG_CXX
AC_PROG_INSTALL

AC_ARG_WITH([werror], [AS_HELP_STRING([--with-werror], [Compile with -Werror @<:@default=no@:>@])], [], [with_werror=no])
if test "x$with_werror" = "xyes"; then
	WERROR="-Werror"
fi

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

AC_CHECK_HEADER("mad.h", , nolibmad=1)

if test -n "$nolibmad"; then
	AC_MSG_ERROR([libMAD is required and could not be found])
fi

AC_CHECK_HEADER("vorbis/vorbisfile.h", , novorbis=1)

if test -n "$novorbis"; then
	AC_MSG_ERROR([libOgg and libVorbis are required and could not be found])
fi

AC_CHECK_HEADER("zlib.h", , nozlib=1)

if test -n "$nozlib"; then
	AC_MSG_ERROR([zlib is required and could not be found])
fi

AC_C_CONST
AC_HEADER_STDC

AC_C_BIGENDIAN()

AM_ICONV

if test "x$am_cv_func_iconv" != "xyes"; then
	AC_MSG_ERROR([No useable iconv() function found])
fi

BOOST_REQUIRE
BOOST_STRING_ALGO
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_REGEX
BOOST_UNORDERED
BOOST_HASH
BOOST_DATE_TIME

dnl Do not run the OpenGL checks on OS X, they will incorrectly find the X11 OpenGL libraries which we do not want.
dnl TODO: Eventually add a test for OS X for OpenGL
dnl TODO: Add OpenAL tests on non OS X systems
case "$target" in
	*darwin*)
		;;
	*)
		AX_CHECK_GL
		AX_CHECK_GLU

		AC_CHECK_HEADER("AL/al.h", , noopenal=1)
		AC_CHECK_HEADER("AL/alc.h", , noopenal=1)

		if test -n "$no_gl"; then
			AC_MSG_ERROR([OpenGL is required and could not be found!])
		fi

		if test -n "$no_glu"; then
			AC_MSG_ERROR([GLU is required and could not be found!])
		fi

		if test -n "$noopenal"; then
			AC_MSG_ERROR([OpenAL is required and could not be found!])
		fi
		;;
esac;

EOS_CXXFLAGS="-DUNIX `sdl-config --cflags`"
EOS_LIBS="`sdl-config --libs` -lmad -logg -lvorbis -lvorbisfile -lz"

dnl Add the OpenAL and OpenGL frameworks on Mac OS X
case "$target" in
	*darwin*)
		EOS_CXXFLAGS="$EOS_CXXFLAGS -DMACOSX -I/System/Library/Frameworks/OpenAL.framework/Headers"
		EOS_LIBS="$EOS_LIBS -framework OpenGL -framework OpenAL"
		;;
	*)
		EOS_LIBS="$EOS_LIBS -lopenal"
		;;
esac;


AC_SUBST(SDL_MAIN_OBJ, [$SDL_MAIN_OBJ])
AC_SUBST(SDL_LIBS, [$SDL_LIBS])
AC_SUBST(EOS_CXXFLAGS)
AC_SUBST(EOS_LIBS)
AC_SUBST(WERROR)

AC_OUTPUT(utf8cpp/Makefile \
          src/common/Makefile \
          src/graphics/Makefile \
          src/graphics/images/Makefile \
          src/graphics/video/Makefile \
          src/graphics/aurora/Makefile \
          src/sound/Makefile \
          src/sound/decoders/Makefile \
          src/events/Makefile \
          src/aurora/Makefile \
          src/engines/Makefile \
          src/engines/nwn/Makefile \
          src/engines/nwn2/Makefile \
          src/engines/kotor/Makefile \
          src/engines/kotor2/Makefile \
          src/engines/thewitcher/Makefile \
          src/engines/sonic/Makefile \
          src/engines/dragonage/Makefile \
          src/Makefile \
          Makefile)
