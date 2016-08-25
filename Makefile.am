AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(EXTRA_CFLAGS) -I$(top_srcdir)/src
AM_LDFLAGS = $(EXTRA_LDFLAGS)

EXTRA_DIST =					\
	COPYING					\
	LICENSE					\
	README.md				\
	NEWS.md					\
	autogen.sh				\
	m4/acx_debug_cflags.m4			\
	m4/ax_check_enable_debug.m4		\
	m4/gcov.m4				\
	src/nss-juju.map

noinst_HEADERS = src/nss-juju.h

#
# src
#

lib_LTLIBRARIES = libnss_juju.la
libnss_juju_la_SOURCES = src/nss-juju.c
libnss_juju_la_CFLAGS = $(AM_CFLAGS) $(COVERAGE_CFLAGS)
libnss_juju_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LT_VERSION_INFO) $(COVERAGE_LDFLAGS)
libnss_juju_la_LDFLAGS += -Wl,--version-script=$(srcdir)/src/nss-juju.map

#
# man
#

man_MANS = man/nss-juju.8
EXTRA_DIST += $(man_MANS) man/nss-juju.pod

man/%.8: man/%.pod
	$(POD2MAN) $(head -n1 man/nss-juju.pod | sed 's/^# pod2man //') $^ $@

#
# tests
#

noinst_PROGRAMS = tests/check_gethostbyname
TESTS = tests/check_gethostbyname

tests_check_gethostbyname_SOURCES = tests/gethostbyname.c
tests_check_gethostbyname_CFLAGS = $(AM_CFLAGS) $(CHECK_CFLAGS) $(COVERAGE_CFLAGS)
tests_check_gethostbyname_LDADD = $(AM_LDFLAGS) $(CHECK_LIBS) $(COVERAGE_LDFLAGS) libnss_juju.la

include $(top_srcdir)/Makefile.am.coverage

@VALGRIND_CHECK_RULES@
