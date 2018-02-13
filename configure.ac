dnl xoreos - A reimplementation of BioWare's Aurora engine
dnl
dnl xoreos is the legal property of its developers, whose names
dnl can be found in the AUTHORS file distributed with this source
dnl distribution.
dnl
dnl xoreos is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public License
dnl as published by the Free Software Foundation; either version 3
dnl of the License, or (at your option) any later version.
dnl
dnl xoreos is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with xoreos. If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.65])

AC_INIT([xoreos],[0.0.4],[xoreos-devel@xoreos.org],[xoreos],[https://xoreos.org/])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 foreign -Wall -Wno-portability no-dist-gzip dist-xz subdir-objects])
dnl Support silent build rules. Disable by either passing
dnl --disable-silent-rules to configure or passing V=1 to make
AM_SILENT_RULES([yes])

dnl If AM_PROG_AR exists, call it, to shut up a libtool warning (and make it more portable, I guess?)
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl libtool
LT_PREREQ([2.2])
LT_INIT([disable-shared])

dnl We're C++
AC_PROG_CXX
AC_LANG([C++])

dnl We want a make install
AC_PROG_INSTALL

dnl Standard C++
AC_HEADER_STDC

dnl Endianness
AC_C_BIGENDIAN

dnl Special variables of the size of pointers
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T

dnl Check the sizes of integer types
AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([void *])

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

dnl Check for -ggdb support
AX_CHECK_COMPILER_FLAGS_VAR([C++], [GGDB], [-ggdb])

AC_SUBST(GGDB)

dnl --with-werror
AC_ARG_WITH([werror], [AS_HELP_STRING([--with-werror], [Compile with -Werror @<:@default=no@:>@])], [], [with_werror=no])

WERROR=""
if test "x$with_werror" = "xyes"; then
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WERROR], [-Werror])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WERROR], [-Werror=unused-but-set-variable])
fi

AC_SUBST(WERROR)

dnl Compiler warning flags
AC_ARG_WITH([warnings], [AS_HELP_STRING([--without-warnings], [Compile without extra warnings @<:@default=no@:>@])], [], [with_warnings=yes])

WARN=""
if test "x$with_warnings" = "xyes"; then
	WARN="-Wall"

	dnl Extra warnings valid for both C and C++
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wignored-qualifiers])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wpointer-arith])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wshadow])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wsign-compare])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wtype-limits])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wuninitialized])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wunused-parameter])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wunused-but-set-parameter])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wduplicated-cond])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wduplicated-branches])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wlogical-op])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wshift-negative-value])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wshift-overflow=2])
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wimplicit-fallthrough])

	dnl Extra warnings valid for C++
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wnon-virtual-dtor])
fi

dnl Disable warnings about hiding virtual functions from a base class. We don't care
AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Woverloaded-virtual], [-Wno-overloaded-virtual])

dnl Disable warnings about the long long type. We need it
AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wlong-long], [-Wno-long-long])
AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wc++11-long-long], [-Wno-c++11-long-long])

dnl clang is far too trigger-happy on this warning, so let's disable it
AX_CHECK_COMPILER_FLAGS_VAR([C++], [WARN], [-Wundefined-var-template], [-Wno-undefined-var-template])

AC_SUBST(WARN)

dnl C++ dialect
AC_ARG_WITH([std], [AS_HELP_STRING([--with-std], [Compile with specifying the C++ standard to follow @<:@default=no@:>@])], [], [with_std=no])

STD=""
if test "x$with_std" = "xyes"; then
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [STD], [-std=c++03])
fi

AC_SUBST(STD)

dnl Link-time optimization
AC_ARG_WITH([lto], [AS_HELP_STRING([--with-lto], [Compile with link-time optimization @<:@default=no@:>@])], [], [with_lto=no])

LTO=""
if test "x$with_lto" = "xyes"; then
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [LTO], [-flto])
fi

AC_SUBST(LTO)

dnl Optimize against local CPU
AC_ARG_WITH([native], [AS_HELP_STRING([--with-native], [Pass -march=native to the compiler to optimize directly against the local CPU instead of generating a more generic build @<:@default=no@:>@])], [], [with_native=no])

NATIVE=""
if test "x$with_native" = "xyes"; then
	AX_CHECK_COMPILER_FLAGS_VAR([C++], [NATIVE], [-march=native])
fi

AC_SUBST(NATIVE)

dnl Release version number
AC_ARG_WITH([release], [AS_HELP_STRING([--with-release=VER], [Set the version suffix to VER instead of the git revision. If no VER is given, do not add a version suffix at all])], [], [with_release=no])

if test "x$with_release" != "xno"; then
	AC_DEFINE([XOREOS_RELEASE], 1, [Define to 1 if we are in release version mode])

	if test "x$with_release" != "xyes"; then
		AC_DEFINE_UNQUOTED([XOREOS_DISTRO], ["+$with_release"], [Release version suffix])
	fi
fi

dnl Force compiling against the internal GLEW library
AC_ARG_ENABLE([external-glew], [AS_HELP_STRING([--disable-external-glew], [Do not check for an external GLEW library and always compile against the internal GLEW library @<:@default=no@:>@])], [], [enable_external_glew=yes])

dnl General purpose libraries
AX_CHECK_ICONV( , AC_MSG_ERROR([No useable iconv() function found!]))
AX_CHECK_ZLIB(1, 2, 3, 0, , AC_MSG_ERROR([zlib(>= 1.2.3) is required and could not be found!]))
AX_CHECK_LZMA(5, 0, 3, 2, , AC_MSG_ERROR([liblzma(>= 5.0.3) is required and could not be found!]))
AX_CHECK_XML2(2, 8, 0, , AC_MSG_ERROR([libxml2(>= 2.8.0) is required and could not be found!]))

dnl GLEW
if test "x$enable_external_glew" = "xyes"; then
	AX_CHECK_GLEW( , [enable_external_glew=no])
fi

if test "x$enable_external_glew" = "xyes"; then
	AC_MSG_NOTICE([using external GLEW library])

	AM_CONDITIONAL(INTERNAL_GLEW, false)
else
	AC_MSG_NOTICE([using internal GLEW library])

	GLEW_CFLAGS="-DGLEW_NO_GLU -DGLEW_STATIC"
	GLEW_LIBS=""

	AC_SUBST(GLEW_CFLAGS)
	AC_SUBST(GLEW_LIBS)

	AC_DEFINE([XOREOS_INTERNAL_GLEW], 1, [Defined to 1 if we are using our internal GLEW sources])
	AM_CONDITIONAL(INTERNAL_GLEW, true)
fi

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
BOOST_UTILITY
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
BOOST_SCOPE_EXIT
BOOST_ATOMIC
BOOST_LOCALE

dnl pthread
AX_PTHREAD()
AM_CONDITIONAL([HAVE_PTHREAD], [test x"$ax_pthread_ok" = xyes])

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

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
