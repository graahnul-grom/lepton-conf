#!/bin/sh

LIB="${lepton}/bin.master/lib"
GDB="gdb"

if [ "`uname -s`" = "FreeBSD" ]; then
    GDB="/usr/local/bin/gdb"
fi

LD_LIBRARY_PATH="${LIB}" $GDB ./lepton-conf

