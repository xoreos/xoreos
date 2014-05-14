dnl @synopsis AX_CHECK_OGG(action-if, action-if-not)
dnl
dnl @summary check for libOgg
dnl
dnl Defines OGG_LIBS, OGG_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_OGG], [
	AC_ARG_VAR(OGG_CFLAGS, [C compiler flags for libogg])
	AC_ARG_VAR(OGG_LIBS, [libraries to pass to the linker for libogg])

	if test -z "$OGG_CFLAGS"; then
		OGG_CFLAGS=""
	fi
	if test -z "$OGG_LIBS"; then
		OGG_LIBS="-logg"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $OGG_CFLAGS"
	CXXFLAGS="$CXXFLAGS $OGG_CFLAGS"
	LIBS="$LIBS $OGG_LIBS"

	AC_CHECK_HEADER([ogg/ogg.h], , noogg=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$noogg" = "1"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(OGG_CFLAGS)
	AC_SUBST(OGG_LIBS)
])
