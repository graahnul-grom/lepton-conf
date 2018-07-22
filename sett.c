/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




void
settings_restore_showinh( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx = eda_config_get_cache_context();

    eda_config_load( ctx, NULL );

    gboolean showinh = eda_config_get_boolean( ctx,
                                               "lepton-conf",
                                               "showinh",
                                               NULL );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  showinh );

    a_showinh( dlg, showinh );

} // settings_restore_showinh()



void
settings_restore_path( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx = eda_config_get_cache_context();

    eda_config_load( ctx, NULL );

    gchar* path = eda_config_get_string( ctx,
                                         "lepton-conf",
                                         "path",
                                         NULL );

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

} // settings_restore_path()



void
settings_restore_geom( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx = eda_config_get_cache_context();

    eda_config_load( ctx, NULL );

    gint x = eda_config_get_int( ctx,
                                 "lepton-conf",
                                 "x",
                                 NULL );
    gint y = eda_config_get_int( ctx,
                                 "lepton-conf",
                                 "y",
                                 NULL );

    gtk_window_move( GTK_WINDOW( dlg ), x, y );


    gint width = eda_config_get_int( ctx,
                                     "lepton-conf",
                                     "width",
                                     NULL );
    gint height = eda_config_get_int( ctx,
                                      "lepton-conf",
                                      "height",
                                      NULL );

    if ( width != 0 && height != 0 )
    {
        gtk_window_resize( GTK_WINDOW( dlg ), width, height );
    }

} // settings_restore_geom()




void
settings_save( cfg_edit_dlg* dlg )
{
    gint x = 0;
    gint y = 0;
    gtk_window_get_position( GTK_WINDOW( dlg ), &x, &y );

    gint width  = 0;
    gint height = 0;
    gtk_window_get_size( GTK_WINDOW( dlg ), &width, &height );


    EdaConfig* ctx = eda_config_get_cache_context();

    // show inh bn state:
    //
    eda_config_set_boolean( ctx, "lepton-conf", "showinh", dlg->showinh_ );


    // geometry:
    //
    eda_config_set_int( ctx, "lepton-conf", "x", x );
    eda_config_set_int( ctx, "lepton-conf", "y", y );
    eda_config_set_int( ctx, "lepton-conf", "width",  width );
    eda_config_set_int( ctx, "lepton-conf", "height", height );


    // tree path:
    //
    gchar* path = row_cur_pos_save( dlg );
    if ( path != NULL )
        eda_config_set_string( ctx, "lepton-conf", "path", path );
    g_free( path );


    // save config:
    //
    eda_config_save( ctx, NULL );

} // settings_save()

