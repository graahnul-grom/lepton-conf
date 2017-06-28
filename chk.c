#include <gtk/gtk.h>
#include <stdio.h>
#include "cfg_edit_dlg.h"



int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );

//    GtkWidget* w = g_object_new( CFG_EDIT_DLG_TYPE, NULL );
    GtkWidget* w = g_object_new( CFG_EDIT_DLG_TYPE, "prop1", 9, NULL );
    if ( w )
    {
        GValue gv = G_VALUE_INIT;
        g_value_init( &gv, G_TYPE_INT );
        g_object_get_property( G_OBJECT( w ), "prop1", &gv );

        printf( "chk cfg_edit_dlg: prop1: [%d]\n", g_value_get_int( &gv ) );


//        gtk_window_set_default_size( GTK_WINDOW(w), 600, 400 );
        gtk_widget_set_size_request( w, 900, 800 );


        gtk_dialog_run( GTK_DIALOG( w ) );

//        g_signal_connect( G_OBJECT( w ),
//                          "destroy",
//                          G_CALLBACK( &gtk_main_quit ),
//                          NULL );
//        gtk_widget_show_all( w );
//        gtk_main();
    }

    return 0;
}

