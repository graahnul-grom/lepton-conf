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
SRC=main.c \
    gobj.c \
    gui.c \
    conf.c \
    cfgreg.c \
    dlg.c \
    sett.c \
    events.c \
    tree.c \
    row.c \
    actions.c
OBJ=.obj/main.o \
    .obj/gobj.o \
    .obj/gui.o \
    .obj/conf.o \
    .obj/cfgreg.o \
    .obj/dlg.o \
    .obj/sett.o \
    .obj/events.o \
    .obj/tree.o \
    .obj/row.o \
    .obj/actions.o
HDR=proto.h cfgreg.h
CFLAGS=-Wall -std=c99 -pthread $(DBG)
CFLAGS_OBJ=-Wall -ansi -std=c99 -pthread $(DBG)

PK_LIBLEPTON_DIR=$(LEPTON_INST_ROOT)/lib/pkgconfig
PK_LIBLEPTON=liblepton

OPT_GTK=`pkg-config --libs --cflags $(PK_GTK)`
OPT_GTK_OBJ=`pkg-config --cflags $(PK_GTK)`
OPT_LIBLEPTON=`PKG_CONFIG_PATH=$(PK_LIBLEPTON_DIR) pkg-config --libs --cflags $(PK_LIBLEPTON)`
OPT_LIBLEPTON_OBJ=`PKG_CONFIG_PATH=$(PK_LIBLEPTON_DIR) pkg-config --cflags $(PK_LIBLEPTON)`


all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE) $(OBJ)
	@echo "done."

.obj/%.o: %.c $(HDR)
	cc $(CFLAGS) -c $< $(OPT_GTK_OBJ) $(OPT_LIBLEPTON_OBJ) -o $@

$(EXE): $(OBJ)
	cc $(CFLAGS) -o $(EXE) $(OBJ) $(OPT_GTK) $(OPT_LIBLEPTON)


# $(EXE): $(SRC) $(HDR)
	# cc $(CFLAGS) -o $(EXE) $(SRC) $(OPT_GTK) $(OPT_LIBLEPTON)

