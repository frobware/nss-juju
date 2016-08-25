/* Copyright 2016 Canonical Ltd.
 *
 * Licensed under the AGPLv3, see LICENSE file for details.
 */

#ifndef _NSS_JUJU_H_
#define _NSS_JUJU_H_

#include <nss.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

extern enum nss_status _nss_juju_gethostbyname3_r
	(const char	 *name,
	 int		  af,
	 struct hostent	 *result,
	 char		 *buffer,
	 size_t		  buflen,
	 int		 *errnop,
	 int		 *herrnop,
	 int32_t	 *ttlp,
	 char		**canonp);

extern enum nss_status _nss_juju_gethostbyname2_r
	(const char	*name,
	 int		 af,
	 struct hostent *result,
	 char		*buffer,
	 size_t		 buflen,
	 int		*errnop,
	 int		*herrnop);

extern enum nss_status _nss_juju_gethostbyname_r
	(const char	*name,
	 struct hostent *result,
	 char		*buffer,
	 size_t		 buflen,
	 int		*errnop,
	 int		*herrnop);

#endif	/* _NSS_JUJU_H_ */
