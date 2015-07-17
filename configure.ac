AC_PREREQ([2.65])

AC_INIT([xoreos],[0.0.1],[xoreos-devel@xoreos.org],[xoreos],[https://xoreos.org/])

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

dnl Compiler warning flags
AC_ARG_WITH([warnings], [AS_HELP_STRING([--without-warnings], [Compile without extra warnings @<:@default=no@:>@])], [], [with_warnings=yes])

if test "x$with_warnings" = "xno"; then
	WARN_C_CXX=""
	WARN_CXX=""
else
	WARN_C_CXX="-Wall"
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wignored-qualifiers])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wpointer-arith])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wshadow])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wsign-compare])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wtype-limits])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wuninitialized])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wunused-parameter])
	AX_CHECK_COMPILER_FLAGS_VAR([C], [WARN_C_CXX], [-Wunused-but-set-parameter])

	WARN_CXX=""
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN_CXX], [-Wnon-virtual-dtor])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN_CXX], [-Wno-overloaded-virtual])
fi

AC_SUBST(WARN_C_CXX)
AC_SUBST(WARN_CXX)

dnl Link-time optimization
AC_ARG_WITH([lto], [AS_HELP_STRING([--with-lto], [Compile with link-time optimization @<:@default=no@:>@])], [], [with_lto=no])

LTO=""
if test "x$with_lto" = "xyes"; then
	AX_CHECK_COMPILER_FLAGS_VAR([C], [LTO], [-flto])
fi

AC_SUBST(LTO)

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

dnl String to integer/float conversion functions
AC_CHECK_FUNCS([strtoll])
AC_CHECK_FUNCS([strtoull])
AC_CHECK_FUNCS([strtof])

dnl General purpose libraries
AX_CHECK_ICONV( , AC_MSG_ERROR([No useable iconv() function found!]))
AX_CHECK_ZLIB(1, 2, 3, 4, , AC_MSG_ERROR([zlib(>= 1.2.3.4) is required and could not be found!]))
AX_CHECK_LZMA(5, 0, 5, 2, , AC_MSG_ERROR([liblzma(>= 5.0.5) is required and could not be found!]))
AX_CHECK_XML2(2, 8, 0, , AC_MSG_ERROR([libxml2(>= 2.8.0) is required and could not be found!]))

dnl Graphic libraries
AX_CHECK_SDL2(2000, , AC_MSG_ERROR([SDL2 (>= 2.0.0) is required and could not be found!]))
AX_CHECK_GL( , AC_MSG_ERROR([OpenGL is required and could not be found!]))
AX_CHECK_FT2([11.0.5], , AC_MSG_ERROR([FreeType2 (>= 11.0.5) is required and could not be found!]))
AX_CHECK_XVID(1, 2, 2, , AC_MSG_ERROR([libxvidcore (>= 1.2.2) is required and could not be found!]))

dnl Sound libraries
AX_CHECK_AL( , AC_MSG_ERROR([OpenAL Soft (>= 1.12) is required and could not be found!]))
AX_CHECK_FAAD(, AC_MSG_ERROR([libfaad (>= 2.7) is required and could not be found!]))
AX_CHECK_MAD(0, 15, 1, , AC_MSG_ERROR([libMAD (>= 0.15.1) is required and could not be found!]))
AX_CHECK_OGG(, AC_MSG_ERROR([libOgg (>= 1.2.0) is required and could not be found!]))
AX_CHECK_VORBIS(, AC_MSG_ERROR([libVorbisFile (>= 1.3.1) is required and could not be found!]))

dnl Boost
BOOST_REQUIRE(1.53.0)
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
BOOST_ATOMIC
BOOST_LOCALE

dnl Extra flags
case "$target" in
	*darwin*)
		XOREOS_CFLAGS="-DUNIX -DMACOSX"
		XOREOS_LIBS=""
		AM_CONDITIONAL(WIN32, false)
		;;
	*mingw*)
		AC_CHECK_TOOL([RC], [windres], [:])
		if test "x$RC" = "x:"; then
			AC_MSG_ERROR([A Windows resource compiler (windres) is required and could not be found!])
		fi

		XOREOS_CFLAGS="-mconsole"
		XOREOS_LIBS=""
		AM_CONDITIONAL(WIN32, true)
		;;
	*)
		XOREOS_CFLAGS="-DUNIX"
		XOREOS_LIBS=""
		AM_CONDITIONAL(WIN32, false)
		;;
esac;

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
AC_CONFIG_FILES([src/graphics/mesh/Makefile])
AC_CONFIG_FILES([src/graphics/render/Makefile])
AC_CONFIG_FILES([src/graphics/shader/Makefile])
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
AC_CONFIG_FILES([src/engines/witcher/Makefile])
AC_CONFIG_FILES([src/engines/sonic/Makefile])
AC_CONFIG_FILES([src/engines/dragonage/Makefile])
AC_CONFIG_FILES([src/engines/dragonage2/Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
