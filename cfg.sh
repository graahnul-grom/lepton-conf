#!/bin/sh

prefix=$t/LC
export PKG_CONFIG_PATH=$lepton/bin.master/lib/pkgconfig
# export PKG_CONFIG_PATH=$lepton/bin.tb/lib/pkgconfig

./configure --prefix=$prefix

