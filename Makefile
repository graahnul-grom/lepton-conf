#####  user-defined vars:  #####################################
#
#

# comment out the next line to disable debugging support:
#
DBG=-O0 -ggdb -DDEBUG

# [CHANGE IT]: GTK pkgconfig package name:
#
PK_GTK=gtk+-2.0

# [CHANGE IT]: where Lepton EDA is installed:
# (--prefix configure argument used when building lepton)
#
# LEPTON_INST_ROOT=/usr/local
LEPTON_INST_ROOT=$(lepton)/bin.master

#
#
###### end of user-editable section  ###########################



EXE=lepton-conf
SRC=main.c cfg_edit_dlg.c gui_mk.c
HDR=cfg_edit_dlg.h cfgreg.h
CFLAGS=-Wall -ansi -std=c99 -pthread $(DBG)

PK_LIBLEPTON_DIR=$(LEPTON_INST_ROOT)/lib/pkgconfig
PK_LIBLEPTON=liblepton

OPT_GTK=`pkg-config --libs --cflags $(PK_GTK)`
OPT_LIBLEPTON=`PKG_CONFIG_PATH=$(PK_LIBLEPTON_DIR) pkg-config --libs --cflags $(PK_LIBLEPTON)`


all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE)
	@echo "done."

$(EXE): $(SRC) $(HDR)
	cc $(CFLAGS) -o $(EXE) $(SRC) $(OPT_GTK) $(OPT_LIBLEPTON)

