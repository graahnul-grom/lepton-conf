#!/bin/sh

# debug:
#
export CFLAGS="-O0 -ggdb -Wall"
# export CPPFLAGS="-DDEBUG -DGLIB_DISABLE_DEPRECATION_WARNINGS=1"
export CPPFLAGS="-DDEBUG"

# release:
#
# export CFLAGS="-O2 -Wall -pedantic"

prefix=/usr/local
export PKG_CONFIG_PATH=$LB/lib/pkgconfig
# export PKG_CONFIG_PATH=$LTB/lib/pkgconfig

cd $LCG
./configure --prefix=$prefix "$@"

