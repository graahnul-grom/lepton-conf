#!/bin/sh

GDB="/usr/local/bin/gdb"

LD_LIBRARY_PATH="${lepton}/bin.master/lib" $GDB ./lepton-conf

