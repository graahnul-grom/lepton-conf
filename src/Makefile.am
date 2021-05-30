bin_PROGRAMS = lepton-conf

lepton_conf_SOURCES = \
	actions.c \
	attrs_dlg.c \
	cfgreg.c \
	conf.c \
	dlg.c \
	events.c \
	gobj.c \
	gui.c \
	main.c \
	row.c \
	sett.c \
	tree.c \
	attrs_dlg.h \
	proto.h \
	sett_list.c \
	sett_list.h

lepton_conf_CFLAGS = $(GTK_CFLAGS) $(LIBLEPTON_CFLAGS)
lepton_conf_LDFLAGS = $(GTK_LIBS) $(LIBLEPTON_LIBS)

AM_CPPFLAGS = -DGLIB_DISABLE_DEPRECATION_WARNINGS=1
