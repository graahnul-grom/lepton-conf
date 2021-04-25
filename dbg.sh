#!/bin/sh

LIB="${lepton}/bin.master/lib"
# LIB="${lepton}/bin.tb/lib"
export LD_LIBRARY_PATH="${LIB}"
GDB="gdb"

if [ "`uname -s`" = "FreeBSD" ]
then
    GDB="/usr/local/bin/gdb"
fi

$GDB ./src/lepton-conf "$@"
# $GDB ./src/lepton-conf -t "$@"

