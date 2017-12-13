#include <libguile.h>

#include <liblepton/liblepton.h>
#include <liblepton/libgedaguile.h>

#include <gtk/gtk.h>
#include <stdio.h>
#include "cfg_edit_dlg.h"



void dlg_resp(GtkDialog *dialog,
              gint       response_id,
              gpointer   user_data)
{
    printf( "  dlg_resp(): resp: %d\n", response_id );
}

void test_dlg()
{
    GtkWidget* vdlg = gtk_dialog_new();
//    GtkWidget* vdlg = gtk_dialog_new_with_buttons(
//        "Edit value:",
//        NULL,
//        GTK_DIALOG_MODAL, // | GTK_DIALOG_DESTROY_WITH_PARENT,
//        GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
//        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
//        NULL );

    GtkWidget* cabtn = gtk_dialog_add_button( GTK_DIALOG( vdlg ),
                                              GTK_STOCK_CANCEL,
                                              GTK_RESPONSE_REJECT );

    GtkWidget* okbtn = gtk_dialog_add_button( GTK_DIALOG( vdlg ),
                                              GTK_STOCK_OK,
                                              GTK_RESPONSE_ACCEPT );



    GtkWidget* ent = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent ), "txt" );

    GtkWidget* vbox = gtk_vbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent, TRUE, TRUE, 5 );

    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( vdlg ) );
//    gtk_box_pack_start( GTK_BOX( ca ), ent, TRUE, TRUE, 10 );
    gtk_box_pack_start( GTK_BOX( ca ), vbox, TRUE, TRUE, 10 );




//    gtk_dialog_set_alternative_button_order(GTK_DIALOG(vdlg),
//                                            GTK_RESPONSE_ACCEPT,
//                                            GTK_RESPONSE_REJECT,
//                                            -1);
//    gtk_dialog_set_default_response (GTK_DIALOG (vdlg),
//                                     GTK_RESPONSE_ACCEPT);

//    g_signal_connect_swapped (vdlg,
//                                "response",
//                                G_CALLBACK (gtk_widget_destroy),
//                                vdlg);

    g_signal_connect_swapped(vdlg,
                            "response",
                            G_CALLBACK (&dlg_resp),
                            vdlg);


    gtk_widget_show_all( vdlg );
//    gtk_widget_set_size_request( vdlg, 300, -1 );


    gtk_dialog_set_default_response (GTK_DIALOG (vdlg),
                                     GTK_RESPONSE_ACCEPT);
//    gtk_widget_grab_default( okbtn );



    gint res = 0;
    res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  edit_val_dlg(): resp: %d\n", res );

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        // ret = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent ) ) );
    }

//    gtk_widget_destroy( vdlg );
}



int main( int argc, char* argv[] )
{
//    scm_init_guile();
//    edascm_init();

    gtk_init( &argc, &argv );

    GtkWidget* wnd = g_object_new( CFG_EDIT_DLG_TYPE, NULL );
//    GtkWidget* wnd = g_object_new( CFG_EDIT_DLG_TYPE, "prop1", 9, NULL );
//    if ( !wnd )
//        return 1;


//    GValue gv = G_VALUE_INIT;
//    g_value_init( &gv, G_TYPE_INT );
//    g_object_get_property( G_OBJECT( wnd ), "prop1", &gv );
//    printf( "gedacfged: cfg_edit_dlg: prop1: [%d]\n", g_value_get_int( &gv ) );


//    gtk_window_set_default_size( GTK_WINDOW(wnd), 600, 400 );
//    gtk_widget_set_size_request( wnd, 900, 800 );
//    gtk_widget_set_size_request( wnd, 900, 400 );


//    test_dlg();
    gtk_dialog_run( GTK_DIALOG( wnd ) );


//        g_signal_connect( G_OBJECT( wnd ),
//                          "destroy",
//                          G_CALLBACK( &gtk_main_quit ),
//                          NULL );
//        gtk_widget_show_all( wnd );
//        gtk_main();

    return 0;
}

