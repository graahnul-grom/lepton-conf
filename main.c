/*
 * lepton-conf - Lepton EDA configuration utility.
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

// how to display g_debug() messages:
// usage:
// > export G_MESSAGES_DEBUG="lepton-conf"
// > /path/to/lepton-conf
// NOTE: G_LOG_DOMAIN needs to be defined in each source file
//
#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif


#include <libguile.h>
#include <gtk/gtk.h>

#include "cfg_edit_dlg.h"




void out_version()
{
    printf( "lepton-conf version 1.0\n" );
    exit( 0 );
}



void out_help( int exit_code )
{
    printf( "usage: lepton-conf [-h] [-v] [-d] [dirname]\n" );
    exit( exit_code );
}



int main( int argc, char* argv[] )
{
//    scm_init_guile();
//    edascm_init();

    gtk_init( &argc, &argv );


    int ch = -1;
    do
    {
        int ch = getopt( argc, argv, "hvd" );

        if ( ch == 'v' )
            out_version();
        else
        if ( ch == 'h' )
            out_help( 0 );
        else
        if ( ch == 'd' )
            g_populate_default_ctx = FALSE;
        else
        if ( ch == '?' )
            out_help( 1 );
    }
    while ( ch != -1 );

//    printf( " >> optind:         [ %d ]\n", optind );
//    printf( " >> argv[ optind ]: [ %s ]\n", argv[ optind ] );

    const gchar* dir = argv[ optind ];
    if ( dir != NULL )
    {
        chdir( dir );
    }


    /* w/o getopt():
    *
    if ( argc >= 2 )
    {
        const gchar* dir = NULL;
        dir = argv[ 1 ];
        chdir( dir );
        printf( " >> CD: [ %s ]\n\n", dir );
    }
    */


    GtkWidget* wnd = cfg_edit_dlg_new();

//    gtk_widget_set_size_request( wnd, 800, 650 );
//    gtk_window_set_default_size( GTK_WINDOW(wnd), 800, 1000 );
//    gtk_dialog_run( GTK_DIALOG( wnd ) );

    g_signal_connect( G_OBJECT( wnd ),
                      "destroy",
                      G_CALLBACK( &gtk_main_quit ),
                      NULL );

    gtk_widget_show_all( wnd );

    gtk_main();

    // printf( " -- -- -- main(): done\n\n");
    return 0;
}

