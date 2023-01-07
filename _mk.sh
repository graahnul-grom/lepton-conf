#!/bin/sh

# export PKG_CONFIG_PATH=$LB/lib/pkgconfig
# export PKG_CONFIG_PATH=$LTB/lib/pkgconfig

cd $LCG
time make -j4 "$@"

