#serial 1001
dnl @synopsis AX_CHECK_AL(version, action-if, action-if-not)
dnl
dnl @summary check for OpenAL (Soft).
dnl
dnl Defines AL_LIBS, AL_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>, Matthew Hoops <clone2727@gmail.com>
dnl @version 2018-06-19
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_AL], [
	AC_ARG_VAR(AL_CFLAGS, [C compiler flags for OpenAL (Soft)])
	AC_ARG_VAR(AL_LIBS, [libraries to pass to the linker for OpenAL (Soft)])

	case "$target" in
		*darwin*)
			dnl TODO: Eventually add a test for OS X for OpenAL

			if test -z "$AL_CFLAGS"; then
				AL_CFLAGS=""
			fi
			if test -z "$AL_LIBS"; then
				AL_LIBS="-framework OpenAL"
			fi
			;;

		*)
			PKG_CHECK_MODULES([AL], [openal >= $1], [$2], [$3])
			;;
	esac;

	AC_SUBST(AL_CFLAGS)
	AC_SUBST(AL_LIBS)
])
