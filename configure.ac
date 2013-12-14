AC_PREREQ([2.65])

AC_INIT([xoreos],[0.0.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 -Wall -Wno-portability no-dist-gzip dist-xz subdir-objects])
# Support silent build rules. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
AM_SILENT_RULES([yes])

dnl If AM_PROG_AR exists, call it, to shut up a libtool warning (and make it more portable, I guess?)
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

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

dnl SDL2
AX_CHECK_SDL2(2000, , AC_MSG_ERROR([SDL2 (>= 2.0.0) is required and could not be found!]))

dnl faad
AX_CHECK_FAAD(, AC_MSG_ERROR([libfaad is required and could not be found!]))

dnl MAD
AX_CHECK_MAD(0, 15, 1, , AC_MSG_ERROR([libMAD (>= 0.15.1) is required and could not be found!]))

#dnl Ogg
AX_CHECK_OGG(, AC_MSG_ERROR([libOgg is required and could not be found!]))

#dnl VorbisFile
AX_CHECK_VORBIS(, AC_MSG_ERROR([libVorbisFile is required and could not be found!]))

dnl Xvid
AX_CHECK_XVID(1, 2, 2, , AC_MSG_ERROR([libxvidcore (>= 1.2.2) is required and could not be found!]))

dnl zlib
AX_CHECK_ZLIB(1, 2, 3, 4, , AC_MSG_ERROR([zlib(>= 1.2.3.4) is required and could not be found!]))

dnl iconv
AX_CHECK_ICONV( , AC_MSG_ERROR([No useable iconv() function found]))

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

# OpenAL
AX_CHECK_AL( , AC_MSG_ERROR([OpenAL Soft is required and could not be found!]))

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

AC_SUBST(GL_CFLAGS)
AC_SUBST(GL_LIBS)

AC_SUBST(XOREOS_CFLAGS)
AC_SUBST(XOREOS_LIBS)

AC_SUBST(WERROR)

AC_CONFIG_FILES([utf8cpp/Makefile])
AC_CONFIG_FILES([glew/Makefile])
AC_CONFIG_FILES([lua/Makefile])
AC_CONFIG_FILES([gitstamp/Makefile])
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
