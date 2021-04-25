#!/bin/sh

# build with Guile 3.x:
#
# export GU3=/tmp/DMN/gu3
# export PKG_CONFIG_PATH=${GU3}/lib/pkgconfig:$lepton/bin.master/lib/pkgconfig

export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# export PKG_CONFIG_PATH=$lepton/bin.tb/lib/pkgconfig

make -j4 DBG="-O0 -ggdb -DDEBUG" "$@"

