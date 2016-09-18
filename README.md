# Name Service Switch (NSS) module for Juju (Proof of Concept).

This Name Service Switch (NSS) module provides address resolution for
Juju encoded hostnames.

## Supporting Juju charms

Some charms require a hostname in lieu of an IP address to work
properly. To support this you can use ```network-get
--primary-hostname``` in a charm hook to get an always-resolvable
hostname. Address resolution is provided by this plugin.

The encoding of the name is *TBD* but this proof of concept currently
supports the following patterns:

* IPv4

	juju-ip-172-31-38-207

* IPv6

	juju-ip-2001-470-1f1d-8d8-c0db-9559-9417-2416

IPv6 names must be fully expanded (i.e., no collapsed 0's (zeros)).

### Juju Charm Example

	$ network-get --primary-hostname
	juju-ip-192-168-20-1

### General Address Resolution

	$ getent hosts juju-ip-192-168-20-1
	192.168.20.1    juju-ip-192-168-20-1

	$ ping -c 4 juju-ip-192-168-20-1
	PING juju-ip-192-168-20-1 (192.168.20.1) 56(84) bytes of data.
	64 bytes from 192.168.20.1: icmp_seq=1 ttl=64 time=0.069 ms
	64 bytes from 192.168.20.1: icmp_seq=2 ttl=64 time=0.052 ms
	64 bytes from 192.168.20.1: icmp_seq=3 ttl=64 time=0.048 ms
	64 bytes from 192.168.20.1: icmp_seq=4 ttl=64 time=0.048 ms

## Limitations

### Reverse lookup needs to be handled by the host

The module itself cannot be authoritative for arbitrary IP addresses,
particularly if the module is listed before the ```dns``` entry.

**It can, however, rely on the host to do reverse lookup**.

#### Reverse lookup on AWS

	$ hostname
	ip-172-31-0-139

	$ network-get --primary-hostname
	juju-ip-172-31-0-139

	$ getent hosts juju-ip-172-31-0-139
	172.31.0.139    juju-ip-172-31-0-139

	# Reverse lookup
	$ dig -x 172.31.0.139 +short
	ip-172-31-0-139.ec2.internal.

	# Forward lookup
	$ dig ip-172-31-0-139.ec2.internal. +short
	172.31.0.139

This is also true for GCE too.

**But this is _not_ true on Azure**; the reverse lookup does not yield
any hostname (though this may be a limitation of the way Juju is
currently provisioning the machine).

### Only IPv4 or IPv6 for a single hostname

Because the IP address is encoded in the name it is not (currently)
possible to get both an IPv4 and IPv6 address associated for one
hostname. This can be fixed by encoding both addresses in the name
(but it doesn't read terribly well).

   $ juju-ip-172-31-38-207-2001-470-1f1d-8d8-c0db-9559-9417-2416

# Building from source

Installing this module from source requires the following packages:

	$ sudo apt-get update
	$ sudo apt-get install -y debhelper autotools-dev dh-autoreconf dpkg-dev autoconf-archive

And configured and built using autoconf:

	$ ./autogen.sh
	$ ./configure --prefix=/usr
	$ make
	$ sudo make install
	$ sudo ldconfig /usr/lib

## Installing the module

To activate the NSS module, add ```juju``` to the line starting with
```hosts``` in ```/etc/nsswitch.conf```. It is recommended to place
```juju``` early in the list, but after the ```files``` entry.

For example:

	$ cat /etc/nsswitch.conf

	passwd:         compat
	group:          compat
	shadow:         compat
	gshadow:        files

	hosts:          files juju dns
	networks:       files

	protocols:      db files
	services:       db files
	ethers:         db files
	rpc:            db files
	netgroup:       nis

You can verify the module is working using ```getent(1)```.

	$ getent hosts juju-ip-192-168-20-1
	192.168.20.1    juju-ip-192-168-20-1

And if you had a machine with that address on your network you can
ping it by name:

	$ ping -c 4 juju-192-168-20-1
	PING juju-192-168-20-1 (192.168.20.1) 56(84) bytes of data.
	64 bytes from 192.168.20.1: icmp_seq=1 ttl=64 time=0.069 ms
	64 bytes from 192.168.20.1: icmp_seq=2 ttl=64 time=0.052 ms
	64 bytes from 192.168.20.1: icmp_seq=3 ttl=64 time=0.048 ms
	64 bytes from 192.168.20.1: icmp_seq=4 ttl=64 time=0.048 ms

# Hacking

## Coding style

This project adheres to the spirit of the Linux kernel coding style
but we favour readability over an overly strict line width of 80
characters; use good judgement as readability matters.

## Debug builds and invoking GDB

	$ ./configure --enable-debug
	$ make
	$ libtool --mode=execute gdb tests/check_gethostbyname

## Code coverage

	$ ./configure --enable-gcov
	$ make
	$ make check
	$ make coverage-html
	$ xdg-open coveragereport/index.html

Old coverage results and reports can be removed via:

	$ make clean-coverage-html
	$ make clean-coverage-gcovr

## Valgrind support

The presence/absence of valgrind is detected when running configure.
If found, the tests can be run under the auspices of valgrind:

	$ ./configure
	$ make
	$ make check-valgrind
