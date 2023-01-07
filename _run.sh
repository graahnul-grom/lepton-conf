#!/bin/sh

LIB=$LB/lib
# LIB=$LTB/lib

export G_MESSAGES_DEBUG="lepton-conf"
# export G_MESSAGES_DEBUG="all"
# export G_ENABLE_DIAGNOSTIC=1

# export GTK2_RC_FILES=
export GTK2_RC_FILES=$HOME/.gtkrc-2.0
# export LD_LIBRARY_PATH=$LIB:$LD_LIBRARY_PATH

exe=$LCG/src/lepton-conf

$exe -e -t "$@"
# $exe -e -m -t "$@"
# $exe -e -m -t -r "$@"
# $exe -e -m "$@"

