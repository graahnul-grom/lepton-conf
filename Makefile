# comment out the next line to suppress debug messages:
#
DBG=-O0 -ggdb -DDEBUG
EXE=lepton-conf
SRC=main.c cfg_edit_dlg.c
CFLAGS=-Wall -ansi -std=c99 -pthread $(DBG)



# [CHANGE IT]: gtk and guile package names
#
OPT_GTK=`pkg-config --libs --cflags gtk+-2.0`
OPT_GUILE=`pkg-config --libs --cflags guile-2.0`

# [CHANGE IT]: path to lepton or gaf
#
gaf_dir=$(lepton)/bin.master

# [CHANGE IT]: liblepton or libgeda
#
libgeda_name=liblepton



gaf_pk_dir=$(gaf_dir)/lib/pkgconfig
OPT_LIBGEDA=`PKG_CONFIG_PATH=$(gaf_pk_dir) pkg-config --libs --cflags $(libgeda_name)`



all: $(EXE)
	@echo "done."

clean:
	@rm -f $(EXE)
	@echo "done."

$(EXE): $(SRC) cfg_edit_dlg.h cfg_registry.h
	cc $(CFLAGS) -o $(EXE) $(SRC) \
	$(OPT_GTK) $(OPT_GUILE) $(OPT_LIBGEDA)

