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
