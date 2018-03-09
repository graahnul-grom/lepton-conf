#####  user-defined vars:  #####################################
#
#

# comment out the next line to suppress debug messages:
#
DBG=-O0 -ggdb -DDEBUG

# [CHANGE IT]: pkgconfig packages' names:
#
PK_GTK=gtk+-2.0
PK_GUILE=guile-2.0

# [CHANGE IT]: Lepton EDA install root:
#
# LEPTON_INST_ROOT=/usr/local
LEPTON_INST_ROOT=$(lepton)/bin.master

#
#
################################################################



EXE=lepton-conf
SRC=main.c cfg_edit_dlg.c
HDR=cfg_edit_dlg.h cfg_registry.h
CFLAGS=-Wall -ansi -std=c99 -pthread $(DBG)

PK_LIBLEPTON_DIR=$(LEPTON_INST_ROOT)/lib/pkgconfig
PK_LIBLEPTON=liblepton

OPT_GTK=`pkg-config --libs --cflags $(PK_GTK)`
OPT_GUILE=`pkg-config --libs --cflags $(PK_GUILE)`
OPT_LIBLEPTON=`PKG_CONFIG_PATH=$(PK_LIBLEPTON_DIR) pkg-config --libs --cflags $(PK_LIBLEPTON)`


all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE)
	@echo "done."

$(EXE): $(SRC) $(HDR)
	cc $(CFLAGS) -o $(EXE) $(SRC) $(OPT_GTK) $(OPT_GUILE) $(OPT_LIBLEPTON)

