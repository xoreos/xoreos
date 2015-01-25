#serial 1000
dnl @synopsis AX_CHECK_ICONV(action-if, action-if-not)
dnl
dnl @summary check for a useable iconv() function
dnl
dnl Defines LIBICONV and LTLIBICONV.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_ICONV], [
	AM_ICONV

	if test "x$am_cv_func_iconv" != "xyes"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(LIBICONV)
	AC_SUBST(LTLIBICONV)
])
