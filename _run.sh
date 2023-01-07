#!/bin/sh

# run with Guile 3.x:
#
# export GU3=/tmp/DMN/gu3
# LIB="${GU3}/lib:${lepton}/bin.master/lib"

LIB="${lepton}/bin.master/lib"
# LIB="${lepton}/bin.tb/lib"

export G_MESSAGES_DEBUG="lepton-conf"
# export G_MESSAGES_DEBUG="all"
# export G_ENABLE_DIAGNOSTIC=1

# export GTK2_RC_FILES=
export GTK2_RC_FILES=${HOME}/.gtkrc-2.0
export LD_LIBRARY_PATH=${LIB}:$LD_LIBRARY_PATH

EXE="./src/lepton-conf"

# $EXE -e -m -t "$@"
$EXE -e -t "$@"
# $EXE -e -m -t -r "$@"
# $EXE -e -m "$@"

