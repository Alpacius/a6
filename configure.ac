AC_INIT([a6], [0.1], [sceneviper@hotmail.com], [a6], [https://github.com/Alpacius/a6])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_PROG_AR
AM_PROG_AS

LT_INIT

AC_ARG_ENABLE([common-arch], AS_HELP_STRING([--enable-common-arch], [enable portable architecture supports]))
AM_CONDITIONAL([ENABLE_COMMON_ARCH], [test -n "${enable_common_arch}"])

AC_PROG_CC
AC_CHECK_LIB(pthread, pthread_create)

AC_OUTPUT([Makefile])
