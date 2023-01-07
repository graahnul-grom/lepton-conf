#!/bin/sh

LIB=$LB/lib
# LIB=$LTB/lib
# export LD_LIBRARY_PATH=$LIB:$LD_LIBRARY_PATH

GDB=gdb
if [ "`uname -s`" = "FreeBSD" ]
then
    GDB=/usr/local/bin/gdb
fi

$GDB -q --args $LCG/src/lepton-conf -t "$@"

