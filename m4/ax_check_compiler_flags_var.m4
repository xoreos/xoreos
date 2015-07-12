#serial 1000
dnl @synopsis AX_CHECK_COMPILER_FLAGS_VAR(language, variable, flag)
dnl
dnl @summary Check whether the compiler for this language accepts this flag, and
dnl          add it to the variable if it does
dnl
dnl Depends on the AX_CHECK_COMPILER_FLAGS macro.
dnl
dnl @category Misc
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2015-07-12
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_COMPILER_FLAGS_VAR], [
	AC_LANG_PUSH([$1])

	AX_CHECK_COMPILER_FLAGS([$3], [-Werror], AS_VAR_APPEND([$2], [" $3"]))

	AC_LANG_POP([$1])
])
