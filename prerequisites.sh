#!/bin/sh

apt-get update -y -qq

apt-get install -y -qq				\
     autoconf-archive				\
     autotools-dev				\
     check					\
     debhelper					\
     dh-autoreconf				\
     dpkg-dev					\
     pkg-config
