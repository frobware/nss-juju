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
