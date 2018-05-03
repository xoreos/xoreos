#serial 1004
dnl @synopsis AX_CHECK_FT2(version, action-if, action-if-not)
dnl
dnl @summary check for the Freetype2 library of sufficient version.
dnl
dnl Defines FT2_LIBS, FT2_CFLAGS.
dnl
dnl The FreeType library version numbers are not the FreeType2
dnl release version numbers. For example, FreeType 2.4.0 comes
dnl with the FreeType library 11.0.5. This function needs the
dnl latter, the FreeType library number.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2018-05-03
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_FT2], [
	PKG_CHECK_MODULES([FT2], [freetype2 >= $1], [$2], [$3])

	AC_SUBST(FT2_CFLAGS)
	AC_SUBST(FT2_LIBS)
])

