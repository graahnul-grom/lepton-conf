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




gboolean g_cfg_legacy_mode = FALSE;
gboolean g_close_with_esc = FALSE;
gboolean g_populate_default_ctx = TRUE;
const gchar* g_exted_default = "gvim";
gboolean g_warn_cfg_file_not_found = TRUE;
gboolean g_print_default_cfg = FALSE;
gboolean g_tst_btn_visible = FALSE;
gboolean g_restore_last_dir = FALSE;




void out_version()
{
    printf( "lepton-conf version 1.0 delta\n" );
    printf( "Configuration utility for Lepton EDA " );
    printf( "(https://github.com/lepton-eda/lepton-eda)\n" );
    printf( "Copyright (C) 2017-2020 dmn <graahnul.grom@gmail.com>\n" );
    printf( "WWW: https://github.com/graahnul-grom/lepton-conf\n" );

    exit( 0 );
}



void out_help( int exit_code )
{
    printf( "Usage: lepton-conf [OPTIONS] [PATH]\n" );
    printf( "Options:\n" );
    printf( "  -l    Legacy config mode: use geda*.conf configuration files\n" );
    printf( "  -e    Close window with 'Escape' key\n" );
    printf( "  -r    Restore last working directory\n" );
    printf( "  -m    Do not warn about missing configuration files\n" );
    printf( "  -d    Do not populate the DEFAULT configuration context on startup\n" );
    printf( "  -p    Print the DEFAULT context in the *.conf file format and exit\n" );
    printf( "  -h    Show usage information\n" );
    printf( "  -v    Show version information\n" );

    exit( exit_code );
}



int main( int argc, char* argv[] )
{
//    scm_init_guile();
//    edascm_init();

    gtk_init( &argc, &argv );


    int ch = -1;
    while ( (ch = getopt( argc, argv, "hvdemprtl" )) != -1 )
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
        if ( ch == 'r' )
            g_restore_last_dir = TRUE;
        else
        if ( ch == 't' )
            g_tst_btn_visible = TRUE; // TESTING: show tst btn on toolbar
        else
        if ( ch == 'l' )
            g_cfg_legacy_mode = TRUE;
        else
        if ( ch == '?' )
            out_help( 1 );
        else
            out_help( 1 );
    }


    // NOTE:  legacy mode: use geda*.conf   cfg files
    // NOTE: !legacy mode: use lepton*.conf cfg files
    //
    config_set_legacy_mode( g_cfg_legacy_mode );


#ifdef DEBUG
    printf( " >> g_get_current_dir(): [%s]\n", g_get_current_dir() );
#endif

    const gchar* dir = argv[ optind ];
    if ( dir != NULL )
    {
        chdir( dir );
    }
    else
    if ( g_restore_last_dir )
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

