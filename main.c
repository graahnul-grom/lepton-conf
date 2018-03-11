/*
 * lepton-conf - Lepton EDA configuration utility.
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

//
// how to display g_debug() messages:
// - define G_LOG_DOMAIN macro in each source file
// - export G_MESSAGES_DEBUG="lepton-conf"
// - launch /path/to/lepton-conf
//

#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif


#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "cfg_edit_dlg.h"




void out_version()
{
    printf( "lepton-conf version 1.0 beta\n" );
    printf( "Configuration utility for Lepton EDA " );
    printf( "(https://github.com/lepton-eda/lepton-eda)\n" );
    printf( "Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>\n" );
    printf( "WWW: https://github.com/graahnul-grom/lepton-conf\n" );
    exit( 0 );
}



void out_help( int exit_code )
{
    printf( "Usage: lepton-conf [-h] [-v] [-e] [-d] [dirname]\n" );
    printf( "Options:\n" );
    printf( "  -h    Help (this message).\n" );
    printf( "  -v    Show version.\n" );
    printf( "  -e    Close window with 'Escape' key.\n" );
    printf( "  -d    Do not populate DEFAULT config context on startup.\n" );
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
        int ch = getopt( argc, argv, "hvde" );

        if ( ch == 'v' )
            out_version();
        else
        if ( ch == 'h' )
            out_help( 0 );
        else
        if ( ch == 'd' )
            g_populate_default_ctx = FALSE;
        else
        if ( ch == 'e' )
            g_close_with_esc = TRUE;
        else
        if ( ch == '?' )
            out_help( 1 );
    }
    while ( ch != -1 );


    const gchar* dir = argv[ optind ];
    if ( dir != NULL )
    {
        chdir( dir );
    }


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

