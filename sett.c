#include "cfg_edit_dlg.h"




/* ******************************************************************
*
*  save/restore app settings:
*
*/

// "show" handler for dialog:
// NOTE: called *after* cfg_edit_dlg_init()
//
void
settings_restore( GtkWidget* widget )
{
//    printf( " ++ settings_restore()\n" );

    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

    EdaConfig* ctx = eda_config_get_user_context();

    // show inherited:
    //
    gboolean showinh = eda_config_get_boolean(
        ctx,
        "lepton-conf",
        "lepton-conf-hidden-showinh", NULL );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  showinh );

    xxx_showinh( dlg, showinh );
    // was: on_btn_showinh( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ), (gpointer*) dlg );


    // geometry:
    //
    gint x = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-x", NULL );
    gint y = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-y", NULL );
    gtk_window_move( GTK_WINDOW( dlg ), x, y );

    gint width = eda_config_get_int(  ctx, "lepton-conf", "lepton-conf-hidden-width", NULL );
    gint height = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-height", NULL );
    if ( width != 0 && height != 0 )
        gtk_window_resize( GTK_WINDOW( dlg ), width, height );


    // tree path:
    //
    GError* err = NULL;
    gchar* path = eda_config_get_string( eda_config_get_user_context(),
                                          "lepton-conf",
                                          "lepton-conf-hidden-path",
                                          &err );
    g_clear_error( &err );


    tree_set_focus( dlg );

    if ( path != NULL )
    {
        row_cur_pos_restore( dlg, path );
        g_free( path );
    }
    else
    {
        row_cur_pos_restore( dlg, "0" );
    }

//    printf( " -- settings_restore()\n" );

} // settings_restore()



// "unmap" handler for dialog:
// called when dialog box is closed
//
void
settings_save( GtkWidget* widget )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

    gint x = 0;
    gint y = 0;
    gtk_window_get_position( GTK_WINDOW( dlg ), &x, &y );

    gint width  = 0;
    gint height = 0;
    gtk_window_get_size( GTK_WINDOW( dlg ), &width, &height );


    EdaConfig* ctx = eda_config_get_user_context();

    // show inh bn state:
    //
    eda_config_set_boolean( ctx, "lepton-conf", "lepton-conf-hidden-showinh", dlg->showinh_ );


    // geometry:
    //
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-x", x );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-y", y );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-width",  width );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-height", height );


    // tree path:
    //
    gchar* path = row_cur_pos_save( dlg );
    if ( path != NULL )
        eda_config_set_string( ctx, "lepton-conf", "lepton-conf-hidden-path", path );
    g_free( path );


    // save config:
    //
    eda_config_save( ctx, NULL );

} // settings_save()

