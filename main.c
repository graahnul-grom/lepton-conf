// how to display g_debug() messages:
// usage:
// > export G_MESSAGES_DEBUG="lepton-conf"
// > /path/to/lepton-conf
//
//#define G_LOG_DOMAIN "lepton-conf"


#include <libguile.h>

#include <gtk/gtk.h>

#include <stdio.h>

#include "cfg_edit_dlg.h"




int main( int argc, char* argv[] )
{
//    scm_init_guile();
//    edascm_init();

    gtk_init( &argc, &argv );

    if ( argc >= 2 )
        chdir( argv[ 1 ] );

    GtkWidget* wnd = cfg_edit_dlg_new();

    gtk_widget_set_size_request( wnd, 800, 650 );
//    gtk_window_set_default_size( GTK_WINDOW(wnd), 800, 650 );


//    gtk_dialog_run( GTK_DIALOG( wnd ) );


        g_signal_connect( G_OBJECT( wnd ),
                          "destroy",
                          G_CALLBACK( &gtk_main_quit ),
                          NULL );

        gtk_widget_show_all( wnd );
        gtk_main();

    printf( " -- -- -- main(): done\n\n");
    return 0;
}

