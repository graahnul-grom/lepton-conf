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

OPT_GTK=`pkg-config --libs --cflags $(PACKAGE_NAME_GTK)`
OPT_GTK_OBJ=`pkg-config --cflags $(PACKAGE_NAME_GTK)`
OPT_LIBLEPTON=`pkg-config --libs --cflags $(PACKAGE_NAME_LIBLEPTON)`
OPT_LIBLEPTON_OBJ=`pkg-config --cflags $(PACKAGE_NAME_LIBLEPTON)`


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

