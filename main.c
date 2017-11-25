#include <gtk/gtk.h>
#include <stdio.h>
#include "cfg_edit_dlg.h"



int main( int argc, char* argv[] )
{
    gtk_init( &argc, &argv );

//    GtkWidget* wnd = g_object_new( CFG_EDIT_DLG_TYPE, NULL );
    GtkWidget* wnd = g_object_new( CFG_EDIT_DLG_TYPE, "prop1", 9, NULL );
    if ( !wnd )
        return 1;


//    GValue gv = G_VALUE_INIT;
//    g_value_init( &gv, G_TYPE_INT );
//    g_object_get_property( G_OBJECT( wnd ), "prop1", &gv );
//    printf( "gedacfged: cfg_edit_dlg: prop1: [%d]\n", g_value_get_int( &gv ) );


//    gtk_window_set_default_size( GTK_WINDOW(wnd), 600, 400 );
//    gtk_widget_set_size_request( wnd, 900, 800 );
    gtk_widget_set_size_request( wnd, 900, 400 );


    gtk_dialog_run( GTK_DIALOG( wnd ) );


//        g_signal_connect( G_OBJECT( wnd ),
//                          "destroy",
//                          G_CALLBACK( &gtk_main_quit ),
//                          NULL );
//        gtk_widget_show_all( wnd );
//        gtk_main();

    return 0;
}

