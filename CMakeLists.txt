# WARNING: when using CMAKE, build in separate directory,
#          because it will overwrite the Makefile:
#
# $ mkdir build/
# $ cd build/
# $ cmake ..
# $ make
#

cmake_minimum_required(VERSION 2.8)

set ( EXE lepton-conf )
project( ${EXE} )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra")

set ( SRC
  main.c
  gobj.c
  gui.c
  conf.c
  cfgreg.c
  dlg.c
  sett.c
  events.c
  tree.c
  row.c
  actions.c
)

add_executable( ${EXE} ${SRC} )


find_package( GTK2 2.6 REQUIRED gtk )
if( GTK2_FOUND )
  include_directories( ${GTK2_INCLUDE_DIRS} )
  target_link_libraries( ${EXE} ${GTK2_LIBRARIES} )
endif()

include( FindPkgConfig )
# set(CMAKE_PREFIX_PATH "/home/dmn/lepton/bin.master/lib/pkgconfig")
set( ENV{PKG_CONFIG_PATH} "$ENV{lepton}/bin.master/lib/pkgconfig" )
pkg_search_module( LL REQUIRED liblepton )
pkg_search_module( LLR REQUIRED libleptonrenderer )

if( LL_FOUND )
  include_directories( ${LL_INCLUDE_DIRS} )
  # target_link_libraries( ${EXE} ${LL_LIBRARIES} )
  target_link_libraries( ${EXE} ${LL_LDFLAGS} ) # libs & flags
endif()

if( LLR_FOUND )
  include_directories( ${LLR_INCLUDE_DIRS} )
  target_link_libraries( ${EXE} ${LLR_LDFLAGS} )
endif()

