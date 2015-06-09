#serial 1000
dnl @synopsis AX_CHECK_XML2(major, minor, patch, action-if, action-if-not)
dnl
dnl @summary check for libxml2 of sufficient major, minor and patch version
dnl
dnl Defines XML2_LIBS, XML2_CFLAGS. See xml2-config(1) man page.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2015-06-09
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_XML2], [
	AC_ARG_VAR(XML2_CONFIG, [xml2-config utility])
	AC_ARG_VAR(XML2_CFLAGS, [C compiler flags for XML2])
	AC_ARG_VAR(XML2_LIBS, [libraries to pass to the linker for XML2])

	AC_CHECK_PROG([XML2_CONFIG], [xml2-config], [xml2-config])

	if test -n "$XML2_CONFIG"; then
		if test -z "$XML2_CFLAGS"; then
			XML2_CFLAGS=`$XML2_CONFIG --cflags`
		fi
		if test -z "$XML2_LIBS"; then
			XML2_LIBS=`$XML2_CONFIG --libs`
		fi
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_CPPFLAGS="$CPPFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $XML2_CFLAGS"
	CXXFLAGS="$CXXFLAGS $XML2_CFLAGS"
	CPPFLAGS="$CPPFLAGS $XML2_CFLAGS"
	LIBS="$LIBS $XML2_LIBS"

	AC_CHECK_HEADER([libxml/xmlversion.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <libxml/xmlversion.h>

int main(int argc, char **argv) {
#if LIBXML_VERSION < (($1 * 10000) + ($2 * 100) + $3)
	#error libxml2 version too low
#endif

	return 0;
}
]])], noxml2=0, noxml2=1), noxml2=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	CPPFLAGS="$save_CPPFLAGS"
	LIBS="$save_LIBS"

	if test "$noxml2" = "1"; then
		ifelse([$5], , :, [$5])
	else
		ifelse([$4], , :, [$4])
	fi

	AC_SUBST(XML2_CFLAGS)
	AC_SUBST(XML2_LIBS)
])
