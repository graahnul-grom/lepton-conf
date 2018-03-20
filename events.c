#include "cfg_edit_dlg.h"




void
events_setup( cfg_edit_dlg* dlg )
{
    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( dlg->btn_showinh_ ),
                      "toggled",
                      G_CALLBACK( &on_btn_showinh ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-link",             // mouse click
                      G_CALLBACK( &on_lab_fname ),
                      dlg );
    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-current-link",     // press Enter key
                      G_CALLBACK( &on_lab_fname ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_reload_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_reload ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_tst_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_tst ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_add_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_add ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_edit_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_edit ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_toggle_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_toggle ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",         // tree sel changed
//                      "row-activated",
                      G_CALLBACK( &on_row_sel ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "button-press-event",
                      G_CALLBACK( &on_mouse_click ),
                      dlg );

//    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
//                      "start-interactive-search",
//                      G_CALLBACK( &on_tree_search ),
//                      dlg );


    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "key-press-event",
                      G_CALLBACK( &on_key_press ),
                      dlg );

} // events_setup()

