/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




GList* g_bookmarks = NULL;



static void
settings_save_last_dir()
{
    gchar* cwd = g_get_current_dir();

    EdaConfig* ctx = eda_config_get_cache_context();
    eda_config_set_string( ctx, "lepton-conf", "lastdir", cwd );

    g_free( cwd );
}



void
settings_restore_last_dir()
{
    EdaConfig* ctx = eda_config_get_cache_context();

    eda_config_load( ctx, NULL );

    gchar* path =
        eda_config_get_string( ctx, "lepton-conf", "lastdir", NULL );

    if ( path != NULL )
    {
        gboolean exists = g_file_test( path, G_FILE_TEST_EXISTS );
        if ( exists )
        {
            chdir( path );
        }

        g_free( path );
    }

} // settings_restore_last_dir()



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
settings_save( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx = eda_config_get_cache_context();

    // show inh bn state:
    //
    eda_config_set_boolean( ctx, "lepton-conf", "showinh", dlg->showinh_ );

    // geometry:
    //
    settings_save_wnd_geom( GTK_WINDOW( dlg ), NULL );

    // tree path:
    //
    gchar* path = row_cur_pos_save( dlg );
    if ( path != NULL )
        eda_config_set_string( ctx, "lepton-conf", "path", path );
    g_free( path );

    // last working dir:
    //
    settings_save_last_dir();


    // save config:
    //
    eda_config_save( ctx, NULL );

} // settings_save()



void
settings_save_wnd_geom( GtkWindow* wnd, const gchar* name )
{
    gint x = 0;
    gint y = 0;
    gtk_window_get_position( wnd, &x, &y );

    gint width  = 0;
    gint height = 0;
    gtk_window_get_size( wnd, &width, &height );

    EdaConfig* ctx = eda_config_get_cache_context();

    gchar grp[ 64 ] = "";
    if ( name != NULL )
        sprintf( grp, "lepton-conf.%s", name );
    else
        sprintf( grp, "lepton-conf" );

    eda_config_set_int( ctx, grp, "x", x );
    eda_config_set_int( ctx, grp, "y", y );
    eda_config_set_int( ctx, grp, "width",  width );
    eda_config_set_int( ctx, grp, "height", height );

    eda_config_save( ctx, NULL );

} // settings_save_wnd_geom()



void
settings_restore_wnd_geom( GtkWindow* wnd, const gchar* name )
{
    EdaConfig* ctx = eda_config_get_cache_context();
    eda_config_load( ctx, NULL );

    gchar grp[ 64 ] = "";
    if ( name != NULL )
        sprintf( grp, "lepton-conf.%s", name );
    else
        sprintf( grp, "lepton-conf" );

    gint x = eda_config_get_int( ctx, grp, "x", NULL );
    gint y = eda_config_get_int( ctx, grp, "y", NULL );
    gtk_window_move( wnd, x, y );

    gint width  = eda_config_get_int( ctx, grp, "width",  NULL );
    gint height = eda_config_get_int( ctx, grp, "height", NULL );
    if ( width != 0 && height != 0 )
    {
        gtk_window_resize( wnd, width, height );
    }

} // settings_restore_wnd_geom()



void
settings_load_bookmarks()
{
    EdaConfig* ctx = eda_config_get_cache_context();

    gsize len = 0;
    GError* err = NULL;
    gchar** strs = eda_config_get_string_list( ctx,
                                               "lepton-conf",
                                               "bookmarks",
                                               &len,
                                               &err );
    if ( len > 0 && err == NULL )
    {
        for ( int i = 0; i < len; ++i )
        {
            gchar* path = strs[ i ];
            settings_bookmark_add( g_strdup( path ) );
        }
    }

    g_strfreev( strs );

} // settings_load_bookmarks()



gchar*
settings_bookmarks_to_string()
{
    GString* gstr = g_string_new( NULL );

    for ( GList* p = g_bookmarks; p != NULL; p = p->next )
    {
        gchar* path = (gchar*) p->data;

        gstr = g_string_append( gstr, path );
        gstr = g_string_append( gstr, ";" );
    }

    gchar* str = g_string_free( gstr, FALSE );

    return str;

} // settings_bookmarks_to_string()



void
settings_save_bookmarks_string( const gchar* str )
{
    EdaConfig* ctx = eda_config_get_cache_context();
    eda_config_set_string( ctx, "lepton-conf", "bookmarks", str );
    eda_config_save( ctx, NULL );
}



void
settings_save_bookmarks()
{
    gchar* str = settings_bookmarks_to_string();

    settings_save_bookmarks_string( str );

    g_free( str );
}



void
settings_bookmark_add( gchar* path )
{
    GList* found = g_list_find_custom( g_bookmarks,
                                       path,
                                       (GCompareFunc) &g_strcmp0 );

    if ( found == NULL )
    {
        g_bookmarks = g_list_append( g_bookmarks, path );
        settings_save_bookmarks();
    }
}



void
settings_bookmarks_clear()
{
    g_list_free_full( g_bookmarks, &g_free );

    g_bookmarks = NULL;
}

