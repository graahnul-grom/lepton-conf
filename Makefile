
PACKAGE_NAME_GTK=gtk+-2.0
PACKAGE_NAME_LIBLEPTON=liblepton


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
    actions.c \
    attrs_dlg.c

OBJ=main.o \
    gobj.o \
    gui.o \
    conf.o \
    cfgreg.o \
    dlg.o \
    sett.o \
    events.o \
    tree.o \
    row.o \
    actions.o \
    attrs_dlg.o

HDR=proto.h attrs_dlg.h

CPPFLAGS=-DGLIB_DISABLE_DEPRECATION_WARNINGS=1
CFLAGS=-Wall -std=c99 -pthread $(DBG)
CFLAGS_OBJ=-Wall -ansi -std=c99 -pthread $(DBG)

OPT_GTK=`pkg-config --libs --cflags $(PACKAGE_NAME_GTK)`
OPT_GTK_OBJ=`pkg-config --cflags $(PACKAGE_NAME_GTK)`
OPT_LIBLEPTON=`pkg-config --libs --cflags $(PACKAGE_NAME_LIBLEPTON)`
OPT_LIBLEPTON_OBJ=`pkg-config --cflags $(PACKAGE_NAME_LIBLEPTON)`


all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE) $(OBJ) *.o
	@echo "done."

.c.o: $(HDR)
	cc $(CPPFLAGS) $(CFLAGS) $(OPT_GTK_OBJ) $(OPT_LIBLEPTON_OBJ) -c $< -o $@

$(EXE): $(OBJ)
	cc -o $(EXE) $(OBJ) $(CFLAGS) $(OPT_GTK) $(OPT_LIBLEPTON)

