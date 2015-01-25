#serial 1000
dnl @synopsis AX_CHECK_FAAD(action-if, action-if-not)
dnl
dnl @summary check for libfaad
dnl
dnl Defines FAAD_LIBS, FAAD_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_FAAD], [
	AC_ARG_VAR(FAAD_CFLAGS, [C compiler flags for libfaad])
	AC_ARG_VAR(FAAD_LIBS, [libraries to pass to the linker for libfaad])

	if test -z "$FAAD_CFLAGS"; then
		FAAD_CFLAGS=""
	fi
	if test -z "$FAAD_LIBS"; then
		FAAD_LIBS="-lfaad"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $FAAD_CFLAGS"
	CXXFLAGS="$CXXFLAGS $FAAD_CFLAGS"
	LIBS="$LIBS $FAAD_LIBS"

	AC_CHECK_HEADER([neaacdec.h], , nofaad=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$nofaad" = "1"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(FAAD_CFLAGS)
	AC_SUBST(FAAD_LIBS)
])
