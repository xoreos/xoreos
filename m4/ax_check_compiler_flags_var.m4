#serial 1000
dnl @synopsis AX_CHECK_COMPILER_FLAGS_VAR(language, variable, flag-to-test, flag-to-use)
dnl
dnl @summary Check whether the compiler for this language accepts flag-to-test,
dnl          and add flag-to-add to the variable if it does. flag-to-test and
dnl          flag-to-add can be different, because a compiler might allow
dnl          a stray -Wno-foobar, while not actually supporting it. In this
dnl          case, flag-to-test should be -Wfoobar. If flag-to-use is empty
dnl          flag-to-test will be used.
dnl
dnl Depends on the AX_CHECK_COMPILER_FLAGS macro.
dnl
dnl @category Misc
dnl @author Sven Hesse <drmccoy@drmccoy.de>
dnl @version 2015-07-12
dnl @license Creative Commons CC0 1.0 Universal Public Domain Dedication

AC_DEFUN([AX_CHECK_COMPILER_FLAGS_VAR], [
	AC_LANG_PUSH([$1])

	FLAG_TO_USE="$4"
	if test -z "$FLAG_TO_USE"; then
		FLAG_TO_USE="$3"
	fi

	AX_CHECK_COMPILER_FLAGS([$3], [-Werror], AS_VAR_APPEND([$2], [" $FLAG_TO_USE"]))

	AC_LANG_POP([$1])
])
