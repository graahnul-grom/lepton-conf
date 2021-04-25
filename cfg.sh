#!/bin/sh

# debug:
#
export CFLAGS="-O0 -ggdb"
export CPPFLAGS="-DDEBUG"

# release:
#
# export CFLAGS="-O2"

prefix=$t/LC
export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# export PKG_CONFIG_PATH=$lepton/bin.tb/lib/pkgconfig

./configure --prefix=$prefix "$@"

