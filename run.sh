#!/bin/sh

LIB="${lepton}/bin.master/lib"
EXE="./lepton-conf"
# EXE="./builddir/lepton-conf"
# EXE="./build/lepton-conf"
# EXE="${lepton}/lepton-conf.git/lepton-conf"

export G_MESSAGES_DEBUG="lepton-conf"
# export G_MESSAGES_DEBUG="all"
# export G_ENABLE_DIAGNOSTIC=1

# export GTK2_RC_FILES=
export GTK2_RC_FILES=${HOME}/.gtkrc-2.0
export LD_LIBRARY_PATH=${LIB}:$LD_LIBRARY_PATH

$EXE -e -m $*

