#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([nss-juju], [0.1], [andrew.mcdermott@canonical.com], [nss-juju], [https://github.com/frobware/nss-juju/])

AC_CONFIG_SRCDIR([src/nss-juju.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

m4_include([m4/ax_check_enable_debug.m4])
AX_CHECK_ENABLE_DEBUG([yes])

AM_INIT_AUTOMAKE([foreign silent-rules subdir-objects -Wall -Werror])
AM_MAINTAINER_MODE([enable])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_DEFUN([NSS_JUJU_CHECK_MACRO], [m4_ifndef([$1], [m4_fatal([macro ]$1[ is not defined. Is ]$2[ installed?])])])
NSS_JUJU_CHECK_MACRO([AX_APPEND_COMPILE_FLAGS], [autoconf-archive])

AC_ARG_VAR([V], [Verbose output])
AC_ARG_VAR([EXTRA_CFLAGS], [Extra C compiler flags])
AC_ARG_VAR([EXTRA_LDFLAGS], [Extra linker flags])

LT_INIT([disable-static])
AC_SUBST([LT_VERSION_INFO], [2:0:0])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_LANG([C])

# We have to be a shared library.
AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_SED

AC_CHECK_PROGS([POD2MAN], [pod2man])

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h])
AC_CHECK_HEADERS([netdb.h])
AC_CHECK_HEADERS([netinet/in.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

AX_APPEND_COMPILE_FLAGS([\
	-Wall \
	-Werror \
	-Wextra \
	-Wstrict-prototypes \
	-Werror=format-security \
	-Werror=init-self \
	-Werror=missing-include-dirs \
	-Werror=implicit-function-declaration \
	-Werror=pointer-arith \
	-Wno-missing-field-initializers \
	-Werror=declaration-after-statement \
])

m4_include([m4/gcov.m4])
AC_TDD_GCOV
AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AM_CONDITIONAL([HAVE_LCOV], [test "x$ac_cv_check_lcov" = xyes])
AM_CONDITIONAL([HAVE_GCOVR], [test "x$ac_cv_check_gcovr" = xyes])
AC_SUBST(COVERAGE_CFLAGS)
AC_SUBST(COVERAGE_LDFLAGS)

AX_VALGRIND_CHECK

# Checks for library functions.
AC_CHECK_FUNCS(m4_normalize([
	memset
	regcomp
	gethostbyname
	gethostbyname2
	gethostbyname3
	inet_aton]))

# List of Makefiles
AC_CONFIG_FILES(m4_normalize([Makefile]))

AC_OUTPUT
