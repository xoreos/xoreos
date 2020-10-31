#serial 1001
dnl @synopsis AX_CHECK_CAIRO(version, action-if, action-if-not)
dnl
dnl @summary check for the Cairo library of sufficient version.
dnl
dnl Defines CAIRO_LIBS, CAIRO_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2020-11-01
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_CAIRO], [
	PKG_CHECK_MODULES([CAIRO], [cairo >= $1], [$2], [$3])

	AC_SUBST(CAIRO_CFLAGS)
	AC_SUBST(CAIRO_LIBS)
])

