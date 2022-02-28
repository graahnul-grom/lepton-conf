#!/bin/sh

# debug:
#
export CFLAGS="-O0 -ggdb"
# export CPPFLAGS="-DDEBUG -DGLIB_DISABLE_DEPRECATION_WARNINGS=1"
export CPPFLAGS="-DDEBUG"

# release:
#
# export CFLAGS="-O2"

prefix=/usr/local
# prefix=$t/LC
export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# export PKG_CONFIG_PATH=$lepton/bin.tb/lib/pkgconfig

./configure --prefix=$prefix "$@"

