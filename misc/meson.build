#
# meson/ninja build system file for lepton-conf
#
# usage:
# $ export PKG_CONFIG_PATH=/path/to/lepton/lib/pkgconfig
# $ meson builddir
# $ cd builddir
# $ ninja
#

project( 'lepton-conf', 'c' )

gtk = dependency( 'gtk+-2.0' )
LL  = dependency( 'liblepton' )

srcs = []
srcs += [ 'main.c'       ]
srcs += [ 'gobj.c'       ]
srcs += [ 'gui.c'        ]
srcs += [ 'conf.c'       ]
srcs += [ 'cfgreg.c'     ]
srcs += [ 'dlg.c'        ]
srcs += [ 'sett.c'       ]
srcs += [ 'events.c'     ]
srcs += [ 'tree.c'       ]
srcs += [ 'row.c'        ]
srcs += [ 'actions.c'    ]

deps = [ gtk, LL ]

exe = 'lepton-conf'
executable( exe, srcs, dependencies : deps )

