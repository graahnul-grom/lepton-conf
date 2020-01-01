/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2020 dmn <graahnul.grom@gmail.com>
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

#include "proto.h"




gboolean g_close_with_esc = FALSE;
gboolean g_populate_default_ctx = TRUE;
const gchar* g_exted_default = "gvim";
gboolean g_warn_cfg_file_not_found = TRUE;
gboolean g_print_default_cfg = FALSE;
gboolean g_tst_btn_visible = FALSE;




void out_version()
{
    printf( "lepton-conf version 1.0 gamma\n" );
    printf( "Configuration utility for Lepton EDA " );
    printf( "(https://github.com/lepton-eda/lepton-eda)\n" );
    printf( "Copyright (C) 2017-2020 dmn <graahnul.grom@gmail.com>\n" );
    printf( "WWW: https://github.com/graahnul-grom/lepton-conf\n" );
    exit( 0 );
}



void out_help( int exit_code )
{
    printf( "Usage: lepton-conf [-h] [-v] [-e] [-d] [-p] [dirname]\n" );
    printf( "Options:\n" );
    printf( "  -h    Help (this message).\n" );
    printf( "  -v    Show version.\n" );
    printf( "  -e    Close window with 'Escape' key.\n" );
    printf( "  -m    Do not warn about missing config files.\n" );
    printf( "  -d    Do not populate DEFAULT config context on startup.\n" );
    printf( "  -p    Print DEFAULT cfg ctx (in the form of *.conf file) and exit.\n" );
    exit( exit_code );
}



int main( int argc, char* argv[] )
{
//    scm_init_guile();
//    edascm_init();

    gtk_init( &argc, &argv );


    int ch = -1;
    while ( (ch = getopt( argc, argv, "hvdempt" )) != -1 )
    {
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
        if ( ch == 'm' )
            g_warn_cfg_file_not_found = FALSE;
        else
        if ( ch == 'p' )
            g_print_default_cfg = TRUE;
        else
        if ( ch == 't' )
            g_tst_btn_visible = TRUE; // TESTING: show tst btn on toolbar
        else
        if ( ch == '?' )
            out_help( 1 );
        else
            out_help( 1 );
    }

#ifdef DEBUG
    printf( " >> g_get_current_dir(): [%s]\n", g_get_current_dir() );
#endif

    const gchar* dir = argv[ optind ];
    if ( dir != NULL )
    {
        chdir( dir );
    }
    else
    {
        settings_restore_last_dir();
    }

#ifdef DEBUG
    printf( " >> g_get_current_dir(): [%s]\n", g_get_current_dir() );
#endif


    cfgreg_init();

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

