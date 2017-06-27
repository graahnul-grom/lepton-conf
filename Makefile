EXE=chk
SRC=chk.c cfg_edit_dlg.c
CFLAGS=-Wall -ansi -std=c99 -O0 -ggdb -pthread
# CPPFLAGS=-DG_DISABLE_CHECKS

OPTGTK=`pkg-config --libs --cflags gtk+-2.0`

all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE)
	@echo "done."

$(EXE): $(SRC)
	cc $(CFLAGS) -o $(EXE) $(SRC) $(OPTGTK)

