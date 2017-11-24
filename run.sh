#!/bin/sh

LIB="${lepton}/bin.master/lib"
EXE="/home/dmn/lepton/gedacfged.git/gedacfged"

export GTK2_RC_FILES=/home/dmn/.gtkrc-2.0

export LD_LIBRARY_PATH=${LIB}
$EXE

