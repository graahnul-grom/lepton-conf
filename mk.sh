#!/bin/sh

export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# make clean
make -j4 DBG="-O0 -ggdb -DDEBUG" "$@"
# make -j4 "$@"

