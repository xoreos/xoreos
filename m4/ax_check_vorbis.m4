dnl @synopsis AX_CHECK_VORBIS(action-if, action-if-not)
dnl
dnl @summary check for libVorbisFile
dnl
dnl Defines VORBIS_LIBS, VORBIS_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2013-12-14
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_VORBIS], [
	AC_ARG_VAR(VORBIS_CFLAGS, [C compiler flags for libvorbisfile])
	AC_ARG_VAR(VORBIS_LIBS, [libraries to pass to the linker for libvorbisfile])

	if test -z "$VORBIS_CFLAGS"; then
		VORBIS_CFLAGS=""
	fi
	if test -z "$VORBIS_LIBS"; then
		VORBIS_LIBS="-lvorbisfile"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $VORBIS_CFLAGS"
	CXXFLAGS="$CXXFLAGS $VORBIS_CFLAGS"
	LIBS="$LIBS $VORBIS_LIBS"

	AC_CHECK_HEADER([vorbis/vorbisfile.h], , novorbis=1)

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$novorbis" = "1"; then
		ifelse([$2], , :, [$2])
	else
		ifelse([$1], , :, [$1])
	fi

	AC_SUBST(VORBIS_CFLAGS)
	AC_SUBST(VORBIS_LIBS)
])
