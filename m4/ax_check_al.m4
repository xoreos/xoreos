#serial 1000
dnl @synopsis AX_CHECK_AL(action-if, action-if-not)
dnl
dnl @summary check for OpenAL Soft.
dnl
dnl Defines AL_LIBS, AL_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>, Matthew Hoops <clone2727@gmail.com>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_AL], [
	AC_ARG_VAR(AL_CFLAGS, [C compiler flags for OpenAL Soft])
	AC_ARG_VAR(AL_LIBS, [libraries to pass to the linker for OpenAL Soft])

	case "$target" in
		*darwin*)
			dnl TODO: Eventually add a test for OS X for OpenAL

			if test -z "$AL_CFLAGS"; then
				AL_CFLAGS=""
			fi
			if test -z "$AL_LIBS"; then
				AL_LIBS="-framework OpenAL"
			fi

			noal=0
			;;

		*)
			if test -z "$AL_CFLAGS"; then
				AL_CFLAGS=""
			fi
			if test -z "$AL_LIBS"; then
				AL_LIBS="-lopenal"
			fi

			save_CFLAGS="$CFLAGS"
			save_CXXFLAGS="$CXXFLAGS"
			save_LIBS="$LIBS"
			CFLAGS="$CFLAGS $AL_CFLAGS"
			CXXFLAGS="$CXXFLAGS $AL_CFLAGS"
			LIBS="$LIBS $AL_LIBS"

			AC_CHECK_HEADER([AL/al.h], AC_CHECK_HEADER([AL/alc.h], noal=0, noal=1), noal=1)

			CFLAGS="$save_CFLAGS"
			CXXFLAGS="$save_CXXFLAGS"
			LIBS="$save_LIBS"
			;;
	esac;

	if test "$noal" = "1"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(AL_CFLAGS)
	AC_SUBST(AL_LIBS)
])
