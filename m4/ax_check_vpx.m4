#serial 1000
dnl @synopsis AX_CHECK_VPX(abi-version, action-if, action-if-not)
dnl
dnl @summary check for libvpx of sufficient ABI version
dnl
dnl Defines VPX_LIBS, VPX_CFLAGS.
dnl
dnl @category InstalledPackages
dnl @author Matthew Hoops <clone2727@gmail.com>
dnl @version 2018-03-20
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_VPX], [
	AC_ARG_VAR(VPX_CFLAGS, [C compiler flags for libvpx])
	AC_ARG_VAR(VPX_LIBS, [libraries to pass to the linker for libvpx])

	if test -z "$VPX_CFLAGS"; then
		VPX_CFLAGS=""
	fi
	if test -z "$VPX_LIBS"; then
		VPX_LIBS="-lvpx"
	fi

	save_CFLAGS="$CFLAGS"
	save_CXXFLAGS="$CXXFLAGS"
	save_LIBS="$LIBS"
	CFLAGS="$CFLAGS $VPX_CFLAGS"
	CXXFLAGS="$CXXFLAGS $VPX_CFLAGS"
	LIBS="$LIBS $VPX_LIBS"

	AC_CHECK_HEADER([vpx/vpx_decoder.h], novpxdec=0, novpxdec=1)

	if test "$novpxdec" = "0"; then
		AC_CHECK_HEADER([vpx/vp8dx.h], AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

int main() {
#if VPX_IMAGE_ABI_VERSION < $1
#error VPX Image ABI version too old
#endif

	// Dummy decode init to actually check linkage
	vpx_codec_ctx decoder;
	vpx_codec_dec_cfg cfg;
	vpx_codec_dec_init(&decoder, &vpx_codec_vp8_dx_algo, &cfg, 0);
}
]])], novp8dx=0, novp8dx=1), novp8dx=1)
	else
		novp8dx=1
	fi

	CFLAGS="$save_CFLAGS"
	CXXFLAGS="$save_CXXFLAGS"
	LIBS="$save_LIBS"

	if test "$novp8dx" = "1"; then
		ifelse([$3], , :, [$3])
	else
		ifelse([$2], , :, [$2])
	fi

	AC_SUBST(VPX_CFLAGS)
	AC_SUBST(VPX_LIBS)
])
