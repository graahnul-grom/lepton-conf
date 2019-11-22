#!/bin/sh

export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# gmake clean
gmake -j4 DBG="-O0 -ggdb -DDEBUG"

