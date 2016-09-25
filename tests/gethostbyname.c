/* Copyright 2016 Canonical Ltd.
 *
 * Licensed under the AGPLv3, see LICENSE file for details.
 *
 * Name Service Switch module for resolving Juju host names.
 */

#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include "nss-juju.h"

#define NELEMENTS(A) ((sizeof((A)) / sizeof((A))[0]))

#define GETHOSTBYNAME_VARIABLES			\
	struct hostent	 hostent;		\
	char		 buffer[1024];		\
	int		 err;			\
	int		 h_err			\

#define GETHOSTBYNAME2_VARIABLES		\
	struct hostent	 hostent;		\
	char		 buffer[1024];		\
	int		 err;			\
	int		 h_err

#define GETHOSTBYNAME3_VARIABLES		\
	struct hostent	 hostent;		\
	char		 buffer[1024];		\
	int		 err;			\
	int		 h_err;			\
	int32_t		 ttl;			\
	char		*canon = NULL

static const char *invalid_names[] = {
	"juju",
	"juju-ip",
	"juju-ip-",
	"juju-ip-192",
	"juju-ip-192-168",
	"juju-ip-192-168-1",
	"juju-ip-192-168-1-1 ",
	"juju-ip-192-168-1-1-2 ",
	"juju-ip-172-16-30-1 trailing chars",
	"juju-ip-172-16-30-1-trailing chars",
	"juju-ip-172-16-30-11-trailing chars",
	"juju-ip-172-16-30-123-trailing chars",
	"juju-ip-255-255-255-256",
	"juju-ip-172.16.30.1234",
	"juju-ip-256-256-256-256",
	"leading chars juju-ip-255-255-255-255",
	"juju-ip-256-256-256-256",
	"juju-ip-192-168-200-1 ",
	"leading chars juju-ip-192-168-200-1",
	"juju-ip-10-11-12-256",
	"juju-ip-2001-0470-1f1d-08d8-c0db-9559-9417",
};

struct test_case {
	const char *name;
	const char *expected_ip_str;
	int af;
};

static struct test_case valid_names[] = {
	{ "juju-ip-1-1-1-1",
	  "1.1.1.1",
	  AF_INET },
	{ "juju-ip-10-10-10-10",
	  "10.10.10.10",
	  AF_INET },
	{ "juju-ip-192-168-199-101",
	  "192.168.199.101",
	  AF_INET },
	{ "juju-ip-2001-470-1f1d-8d8-c0db-9559-9417-2416",
	  "2001:470:1f1d:8d8:c0db:9559:9417:2416",
	  AF_INET6 },
	{ "juju-ip-2001-1470-1f1d-f8d8-c0db-9559-9417-2416",
	  "2001:1470:1f1d:f8d8:c0db:9559:9417:2416",
	  AF_INET6 }
};

static size_t nhosts(struct hostent *h)
{
	size_t nhosts = 0;
	char **pptr;
	for (pptr = h->h_addr_list; *pptr != NULL; pptr++, nhosts++) {}
	return nhosts;
}

static const char *hostent_str(struct hostent *h, size_t n, char *buf)
{
	return inet_ntop(h->h_addrtype, h->h_addr_list[n], buf, INET6_ADDRSTRLEN);
}

static enum nss_status _nss_juju_gethostbyname_wrapper(
	int		  version,
	const char	 *name,
	int		  af,
	struct hostent	 *hostent,
	char		 *buffer,
	size_t		  buflen,
	int		 *errnop,
	int		 *herrnop,
	int32_t		 *ttl,
	char		**canonp)
{
	*canonp = NULL;
	*ttl = -1;

	switch (version) {
	case 3:
		return _nss_juju_gethostbyname3_r(
			name,
			af,
			hostent,
			buffer,
			buflen,
			errnop,
			herrnop,
			ttl,
			canonp);
	case 2:
		return _nss_juju_gethostbyname2_r(
			name,
			af,
			hostent,
			buffer,
			buflen,
			errnop,
			herrnop);
	default:
		return _nss_juju_gethostbyname_r(
			name,
			hostent,
			buffer,
			buflen,
			errnop,
			herrnop);
	}
}

START_TEST(test_null_name)
{
	const char	*name	= NULL;
	struct hostent	 hostent;
	char		*buffer = NULL;
	int		 err;
	int		 h_err;
	int32_t		 ttl;
	char		*canonp = NULL;
	enum nss_status	 res;

	res = _nss_juju_gethostbyname_wrapper(
		_i,
		name,
		AF_UNSPEC,
		&hostent,
		buffer,
		0,
		&err,
		&h_err,
		&ttl,
		&canonp);

	fail_unless(res == NSS_STATUS_UNAVAIL);
	fail_unless(err == EINVAL);
}
END_TEST

START_TEST(test_zerolen_name)
{
	const char	*name	= "";
	struct hostent	 hostent;
	char		*buffer = NULL;
	int		 err;
	int		 h_err;
	int32_t		 ttl;
	char		*canonp = NULL;
	enum nss_status	 res;

	res = _nss_juju_gethostbyname_wrapper(
		_i,
		name,
		AF_UNSPEC,
		&hostent,
		buffer,
		0,
		&err,
		&h_err,
		&ttl,
		&canonp);

	fail_unless(res == NSS_STATUS_NOTFOUND);
	fail_unless(err == ENOENT);
}
END_TEST

START_TEST(test_invalid_name_gethostbyname)
{
	GETHOSTBYNAME_VARIABLES;

	enum nss_status res = _nss_juju_gethostbyname_r(
		invalid_names[_i],
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err);

	fail_unless(res == NSS_STATUS_NOTFOUND);
	fail_unless(err == EADDRNOTAVAIL);
}
END_TEST

START_TEST(test_invalid_name_gethostbyname2)
{
	GETHOSTBYNAME2_VARIABLES;

	enum nss_status res = _nss_juju_gethostbyname2_r(
		invalid_names[_i],
		AF_UNSPEC,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err);

	fail_unless(res == NSS_STATUS_NOTFOUND);
	fail_unless(err == EADDRNOTAVAIL);
}
END_TEST

START_TEST(test_invalid_name_gethostbyname3)
{
	GETHOSTBYNAME3_VARIABLES;

	enum nss_status res = _nss_juju_gethostbyname3_r(
		invalid_names[_i],
		AF_UNSPEC,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err,
		&ttl,
		&canon);

	fail_unless(res == NSS_STATUS_NOTFOUND);
	fail_unless(err == EADDRNOTAVAIL);
}
END_TEST

START_TEST(test_valid_ipv4_name_but_null_buffer)
{
	const char	*name	= "juju-ip-192-168-20-1";
	struct hostent	 hostent;
	char		*buffer = NULL;
	int		 err;
	int		 h_err;
	int32_t		 ttl;
	char		*canonp = NULL;
	enum nss_status	 res;

	res = _nss_juju_gethostbyname_wrapper(
		_i,
		name,
		AF_UNSPEC,
		&hostent,
		buffer,
		0,
		&err,
		&h_err,
		&ttl,
		&canonp);

	fail_unless(res == NSS_STATUS_UNAVAIL);
	fail_unless(err == EINVAL);
}
END_TEST

START_TEST(test_valid_ipv6_name_but_null_buffer)
{
	const char	*name	= "juju-ip-2001-470-1f1d-8d8-c0db-9559-9417-2416";
	struct hostent	 hostent;
	char		*buffer = NULL;
	int		 err;
	int		 h_err;
	int32_t		 ttl;
	char		*canonp = NULL;
	enum nss_status	 res;

	res = _nss_juju_gethostbyname_wrapper(
		_i,
		name,
		AF_UNSPEC,
		&hostent,
		buffer,
		0,
		&err,
		&h_err,
		&ttl,
		&canonp);

	fail_unless(res == NSS_STATUS_UNAVAIL);
	fail_unless(err == EINVAL);
}
END_TEST

START_TEST(test_valid_name_gethostbyname)
{
	GETHOSTBYNAME_VARIABLES;
	char ipbuf[INET6_ADDRSTRLEN];

	enum nss_status res = _nss_juju_gethostbyname_r(
		valid_names[_i].name,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err);

	fail_unless(res == NSS_STATUS_SUCCESS);
	fail_unless(err == 0);
	fail_unless(h_err == 0);
	fail_unless(*hostent.h_aliases == NULL);
	fail_unless(nhosts(&hostent) == 1);
	fail_unless(hostent.h_addrtype == valid_names[_i].af);
	ck_assert_str_eq(hostent.h_name, valid_names[_i].name);
	ck_assert_str_eq(hostent_str(&hostent, 0, ipbuf), valid_names[_i].expected_ip_str);
}
END_TEST

START_TEST(test_valid_name_gethostbyname2)
{
	GETHOSTBYNAME2_VARIABLES;
	char ipbuf[INET6_ADDRSTRLEN];

	enum nss_status res = _nss_juju_gethostbyname2_r(
		valid_names[_i].name,
		valid_names[_i].af,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err);

	fail_unless(res == NSS_STATUS_SUCCESS);
	fail_unless(err == 0);
	fail_unless(h_err == 0);
	fail_unless(*hostent.h_aliases == NULL);
	fail_unless(nhosts(&hostent) == 1);
	fail_unless(hostent.h_addrtype == valid_names[_i].af);
	ck_assert_str_eq(hostent.h_name, valid_names[_i].name);
	ck_assert_str_eq(hostent_str(&hostent, 0, ipbuf), valid_names[_i].expected_ip_str);
}
END_TEST

START_TEST(test_valid_name_gethostbyname3)
{
	GETHOSTBYNAME3_VARIABLES;
	char ipbuf[INET6_ADDRSTRLEN];

	enum nss_status res = _nss_juju_gethostbyname3_r(
		valid_names[_i].name,
		valid_names[_i].af,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err,
		&ttl,
		&canon);

	fail_unless(res == NSS_STATUS_SUCCESS);
	fail_unless(err == 0);
	fail_unless(h_err == 0);
	fail_unless(*hostent.h_aliases == NULL);
	fail_unless(nhosts(&hostent) == 1);
	fail_unless(hostent.h_addrtype == valid_names[_i].af);
	ck_assert_str_eq(hostent.h_name, valid_names[_i].name);
	ck_assert_str_eq(hostent_str(&hostent, 0, ipbuf), valid_names[_i].expected_ip_str);
	fail_unless(ttl == 0);
	ck_assert_str_eq(canon, valid_names[_i].name);
}
END_TEST

START_TEST(test_valid_name_with_small_buffer)
{
	GETHOSTBYNAME3_VARIABLES;
	char ipbuf[INET6_ADDRSTRLEN];

	enum nss_status res = _nss_juju_gethostbyname_wrapper(
		_i,
		valid_names[_i].name,
		valid_names[_i].af,
		&hostent,
		buffer,
		1,
		&err,
		&h_err,
		&ttl,
		&canon);

	fail_unless(res == NSS_STATUS_TRYAGAIN);
	fail_unless(err == ERANGE);

	res = _nss_juju_gethostbyname_wrapper(
		_i,
		valid_names[_i].name,
		valid_names[_i].af,
		&hostent,
		buffer,
		sizeof(buffer),
		&err,
		&h_err,
		&ttl,
		&canon);

	fail_unless(res == NSS_STATUS_SUCCESS);
	fail_unless(err == 0);
	fail_unless(h_err == 0);
	fail_unless(*hostent.h_aliases == NULL);
	fail_unless(nhosts(&hostent) == 1);
	fail_unless(hostent.h_addrtype == valid_names[_i].af);
	ck_assert_str_eq(hostent.h_name, valid_names[_i].name);
	ck_assert_str_eq(hostent_str(&hostent, 0, ipbuf), valid_names[_i].expected_ip_str);
	switch (_i) {
	case 3:
		fail_unless(ttl == 0);
		ck_assert_str_eq(canon, valid_names[_i].name);
		break;
	}
}
END_TEST

static Suite *libnss_juju_suite(void)
{
	Suite *s = suite_create("nss-juju");
	TCase *tc = tcase_create("gethostbyname");
	tcase_add_loop_test(tc, test_null_name, 1, 4);
	tcase_add_loop_test(tc, test_zerolen_name, 1, 4);
	tcase_add_loop_test(tc, test_invalid_name_gethostbyname, 0, NELEMENTS(invalid_names));
	tcase_add_loop_test(tc, test_invalid_name_gethostbyname2, 0, NELEMENTS(invalid_names));
	tcase_add_loop_test(tc, test_invalid_name_gethostbyname3, 0, NELEMENTS(invalid_names));
	tcase_add_loop_test(tc, test_valid_ipv4_name_but_null_buffer, 1, 4);
	tcase_add_loop_test(tc, test_valid_ipv6_name_but_null_buffer, 1, 4);
	tcase_add_loop_test(tc, test_valid_name_gethostbyname, 0, NELEMENTS(valid_names));
	tcase_add_loop_test(tc, test_valid_name_gethostbyname2, 0, NELEMENTS(valid_names));
	tcase_add_loop_test(tc, test_valid_name_gethostbyname3, 0, NELEMENTS(valid_names));
	tcase_add_loop_test(tc, test_valid_name_with_small_buffer, 1, 4);
	suite_add_tcase(s, tc);
	return s;
}

int main(void)
{
	int number_failed;
	Suite *s = libnss_juju_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
