AC_PREREQ([2.65])

AC_INIT([xoreos],[0.0.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 -Wall no-dist-gzip dist-xz subdir-objects])
# Support silent build rules. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
AM_SILENT_RULES([yes])

dnl SDL params to configure
AC_ARG_VAR(SDL_CONFIG, [sdl-config utility])
AC_ARG_VAR(SDL_CFLAGS, [C compiler flags for SDL])
AC_ARG_VAR(SDL_LIBS, [libraries to pass to the linker for SDL])

dnl libtool
LT_PREREQ([2.2])
LT_INIT([disable-shared])

dnl We're C++
AC_PROG_CXX
AM_PROG_CC_C_O
dnl We want a make install
AC_PROG_INSTALL

dnl --with-werror
AC_ARG_WITH([werror], [AS_HELP_STRING([--with-werror], [Compile with -Werror @<:@default=no@:>@])], [], [with_werror=no])
if test "x$with_werror" = "xyes"; then
	WERROR="-Werror -Werror=unused-but-set-variable"
fi

dnl Standard C, C++
AC_C_CONST
AC_HEADER_STDC

dnl Endianness
AC_C_BIGENDIAN()

dnl Special variables of the size of pointers
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T

dnl math library and special float functions
AC_CHECK_LIB([m], [cos])
AC_CHECK_FUNCS([fabs])
AC_CHECK_FUNCS([fmin])
AC_CHECK_FUNCS([fmax])
AC_CHECK_FUNCS([fabsf])
AC_CHECK_FUNCS([fminf])
AC_CHECK_FUNCS([fmaxf])

dnl SDL
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

dnl faad
AC_CHECK_HEADER("neaacdec.h", , nolibfaad=1)

if test -n "$nolibfaad"; then
	AC_MSG_ERROR([libfaad is required and could not be found])
fi

FAAD_CFLAGS=""
FAAD_LIBS="-lfaad"

dnl MAD
AC_CHECK_HEADER("mad.h", , nolibmad=1)

if test -n "$nolibmad"; then
	AC_MSG_ERROR([libMAD is required and could not be found])
fi

MAD_CFLAGS=""
MAD_LIBS="-lmad"

dnl libvorbisfile and libogg
AC_CHECK_HEADER("vorbis/vorbisfile.h", , novorbis=1)

if test -n "$novorbis"; then
	AC_MSG_ERROR([libOgg and libVorbis are required and could not be found])
fi

VORBIS_CFLAGS=""
VORBIS_LIBS="-lvorbisfile"

OGG_CFLAGS=""
OGG_LIBS="-logg"

dnl Xvid
AC_CHECK_HEADER("xvid.h", , nolibxvid=1)

if test -n "$nolibxvid"; then
	AC_MSG_ERROR([libxvidcore is required and could not be found])
fi

XVID_CFLAGS=""
XVID_LIBS="-lxvidcore"

dnl zlib
AC_CHECK_HEADER("zlib.h", , nozlib=1)

if test -n "$nozlib"; then
	AC_MSG_ERROR([zlib is required and could not be found])
fi

Z_CFLAGS=""
Z_LIBS="-lz"

dnl iconv
AM_ICONV

if test "x$am_cv_func_iconv" != "xyes"; then
	AC_MSG_ERROR([No useable iconv() function found])
fi

dnl FreeType2
AC_CHECK_FT2([11.0.5], [], [AC_MSG_ERROR([FreeType2 is required to compile xoreos])])

dnl Boost
BOOST_REQUIRE
BOOST_STRING_ALGO
BOOST_SYSTEM
BOOST_FILESYSTEM
BOOST_REGEX
BOOST_UNORDERED
BOOST_HASH
BOOST_DATE_TIME
BOOST_FUNCTION
BOOST_BIND
BOOST_UUID
BOOST_SMART_PTR

dnl OpenGL
case "$target" in
	*darwin*)
		dnl Do not run the OpenGL checks on OS X, they will incorrectly find the X11
		dnl OpenGL libraries which we do not want.
		dnl TODO: Eventually add a test for OS X for OpenGL

		GL_CFLAGS=""
		GL_LIBS="-framework OpenGL"
		;;
	*)
		AX_CHECK_GL

		if test -n "$no_gl"; then
			AC_MSG_ERROR([OpenGL is required and could not be found!])
		fi
		;;
esac;

dnl OpenAL
case "$target" in
	*darwin*)
		dnl TODO: Eventually add a test for OS X for OpenAL

		AL_CFLAGS=""
		AL_LIBS="-framework OpenAL"
		;;
	*)
		AC_CHECK_HEADER("AL/al.h", , no_al=1)
		AC_CHECK_HEADER("AL/alc.h", , no_al=1)

		if test -n "$no_al"; then
			AC_MSG_ERROR([OpenAL is required and could not be found!])
		fi

		AL_CFLAGS=""
		AL_LIBS="-lopenal"
		;;
esac;

dnl Extra flags
case "$target" in
	*darwin*)
		XOREOS_CFLAGS="-DUNIX -DMACOSX"
		XOREOS_LIBS=""
		;;
	*mingw*)
		XOREOS_CFLAGS=""
		XOREOS_LIBS="-static-libgcc -static-libstdc++"
		;;
	*)
		XOREOS_CFLAGS="-DUNIX"
		XOREOS_LIBS=""
		;;
esac;

AC_SUBST(SDL_LIBS)
AC_SUBST(SDL_CFLAGS)

AC_SUBST(FT2_CFLAGS)
AC_SUBST(FT2_LIBS)

AC_SUBST(GL_CFLAGS)
AC_SUBST(GL_LIBS)

AC_SUBST(AL_CFLAGS)
AC_SUBST(AL_LIBS)

AC_SUBST(FAAD_CFLAGS)
AC_SUBST(FAAD_LIBS)

AC_SUBST(MAD_CFLAGS)
AC_SUBST(MAD_LIBS)

AC_SUBST(OGG_CFLAGS)
AC_SUBST(OGG_LIBS)

AC_SUBST(VORBIS_CFLAGS)
AC_SUBST(VORBIS_LIBS)

AC_SUBST(XVID_CFLAGS)
AC_SUBST(XVID_LIBS)

AC_SUBST(Z_CFLAGS)
AC_SUBST(Z_LIBS)

AC_SUBST(XOREOS_CFLAGS)
AC_SUBST(XOREOS_LIBS)

AC_SUBST(WERROR)

AC_CONFIG_FILES([utf8cpp/Makefile])
AC_CONFIG_FILES([glew/Makefile])
AC_CONFIG_FILES([lua/Makefile])
AC_CONFIG_FILES([src/common/Makefile])
AC_CONFIG_FILES([src/graphics/Makefile])
AC_CONFIG_FILES([src/graphics/images/Makefile])
AC_CONFIG_FILES([src/graphics/aurora/Makefile])
AC_CONFIG_FILES([src/sound/Makefile])
AC_CONFIG_FILES([src/sound/decoders/Makefile])
AC_CONFIG_FILES([src/video/Makefile])
AC_CONFIG_FILES([src/video/codecs/Makefile])
AC_CONFIG_FILES([src/video/aurora/Makefile])
AC_CONFIG_FILES([src/events/Makefile])
AC_CONFIG_FILES([src/aurora/Makefile])
AC_CONFIG_FILES([src/aurora/nwscript/Makefile])
AC_CONFIG_FILES([src/engines/Makefile])
AC_CONFIG_FILES([src/engines/nwn/Makefile])
AC_CONFIG_FILES([src/engines/nwn2/Makefile])
AC_CONFIG_FILES([src/engines/kotor/Makefile])
AC_CONFIG_FILES([src/engines/kotor2/Makefile])
AC_CONFIG_FILES([src/engines/jade/Makefile])
AC_CONFIG_FILES([src/engines/thewitcher/Makefile])
AC_CONFIG_FILES([src/engines/sonic/Makefile])
AC_CONFIG_FILES([src/engines/dragonage/Makefile])
AC_CONFIG_FILES([src/engines/dragonage2/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
