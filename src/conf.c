/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2020 dmn <graahnul.grom@gmail.com>
 * License: GPLv2+ - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




/* ******************************************************************
*
*  "config-changed" event handlers:
*  NOTE: not used
*
*/

static void
on_conf_chg_ctx_dflt( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
//    printf( " >> >> on_conf_chg_ctx_dflt(): [%d], [%s] [%s]\n",
//        ctx == eda_config_get_default_context(), g, k );
}


static void
on_conf_chg_ctx_sys( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
//    printf( " >> >> on_conf_chg_ctx_sys(): [%d], [%s] [%s]\n",
//        ctx == eda_config_get_system_context(), g, k );
}


static void
on_conf_chg_ctx_user( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
//    printf( " >> >> on_conf_chg_ctx_user(): [%d], [%s] [%s]\n",
//        ctx == eda_config_get_user_context(), g, k );
}


static void
on_conf_chg_ctx_path( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
//    printf( " >> >> on_conf_chg_ctx_path(): [%d], [%s] [%s]\n",
//        ctx == eda_config_get_context_for_path( "." ), g, k );
}




/* ******************************************************************&
*
*  config:
*
*/

gboolean
conf_is_hidden_key( const gchar* name )
{
    gboolean hidden = FALSE;
    hidden = strstr( name, "lepton-conf-hidden" ) != NULL;
    return hidden;
}



gboolean
conf_is_hidden_group( const gchar* name )
{
    gboolean hidden = FALSE;
    hidden = strstr( name, "dialog-geometry" ) != NULL;
    return hidden;
}



void
conf_load_keys( EdaConfig*    ctx,
                const gchar*  group,
                cfg_edit_dlg* dlg,
                GtkTreeIter   itParent,
                gboolean      wok,
                gboolean*     inh_all,
                gboolean      print )
{
    gsize len = 0;
    GError* err = NULL;

    gchar** pp = eda_config_get_keys( ctx, group, &len, &err );
    if ( pp == NULL )
    {
        printf( " >> conf_load_keys(): !eda_config_get_keys()\n" );
        if ( err != NULL )
        {
            printf( "    err: %s\n", err->message );
        }
        g_clear_error( &err );
        return;
    }

    // make sure empty groups will not be marked as inherited:
    //   see load_groups()
    //
    if ( len > 0 )
        *inh_all = TRUE;


    for ( gsize ndx = 0; ndx < len; ++ndx )
    {
        const gchar* name = pp[ ndx ];

        if ( conf_is_hidden_key( name ) )
            continue;

        gchar* val = eda_config_get_string( ctx, group, name, &err );
        if ( val == NULL )
        {
            printf( " >> conf_load_keys( %s ): !eda_config_get_string( %s )\n", group, name );
            if ( err != NULL )
            {
                printf( "    err: %s\n", err->message );
            }
            continue;
        }



        if ( print )
            printf( "%s=%s\n", name, val );



        gboolean inh = eda_config_is_inherited( ctx, group, name, &err );
        if ( err != NULL )
        {
            printf( " >> conf_load_keys(): !eda_config_is_inherited()\n" );
            printf( "    err: %s\n", err->message );
        }
        g_clear_error( &err );


        *inh_all = *inh_all && inh;

        // NOTE: rdata:
        //
        row_data* rdata = rdata_mk( ctx,
                                    group,          // group
                                    name,           // key
                                    val,            // val
                                    !wok, // ro
                                    inh,            // inh
                                    RT_KEY          // rtype
                                  );

        tree_add_row( dlg,
                      name,
                      val,
                      rdata,
                      &itParent
                    );

        g_free( val );

    } // for keys

    g_strfreev( pp );

} // conf_load_keys()



void
conf_load_groups( EdaConfig*    ctx,
                  gboolean      wok,
                  cfg_edit_dlg* dlg,
                  GtkTreeIter   itParent,
                  gboolean      print )
{
    gsize len = 0;
    gchar** pp = eda_config_get_groups( ctx, &len );
    if ( pp == NULL )
    {
        printf( " >> conf_load_groups(): !eda_config_get_groups()\n" );
        return;
    }

    for ( gsize ndx = 0; ndx < len; ++ndx )
    {
        const gchar* name = pp[ ndx ];

        if ( conf_is_hidden_group( name ) )
            continue;


//        gboolean wok = conf_ctx_file_writable( ctx );
//        gboolean wok = FALSE;
//        conf_ctx_fname( ctx, NULL, NULL, &wok );


        // NOTE: rdata:
        //
        row_data* rdata = rdata_mk( ctx,
                                    name,   // group
                                    NULL,   // key
                                    NULL,   // val
                                    !wok,   // ro
                                    FALSE,  // inh
                                    RT_GRP  // rtype
                                  );

        gchar* display_name = g_strdup_printf( "[%s]", name );



        if ( print )
            printf( "%s\n", display_name );



        GtkTreeIter it = tree_add_row( dlg,
                                       display_name, // name
                                       "",           // val
                                       rdata,        // rdata
                                       &itParent
                                     );

        g_free( display_name );

        // make sure empty groups will not be marked as inherited:
        //   see load_keys()
        //
        gboolean inh_all = FALSE;

        conf_load_keys( ctx, name, dlg, it, wok, &inh_all, print );



        if ( print )
            printf( "\n" );



        // mark group itself as inh if all children are inh:
        //
        rdata->inh_ = inh_all;

    } // for groups

    g_strfreev( pp );

} // conf_load_groups()



gboolean
conf_load_ctx( EdaConfig* ctx )
{
    const gchar* fname = conf_ctx_fname( ctx, NULL, NULL, NULL );

    // TODO: this is for DEFAULT ctx. could it be loaded by this func?
    //
    if ( fname == NULL )
    {
        printf( ".. DEBUG WARNING: conf_load_ctx() called with [fname] == NULL\n" );
        return TRUE;
    }

    GError*  err = NULL;
    gboolean res = eda_config_load( ctx, &err );

    if ( err != NULL )
    {
        gboolean file_not_found =
            g_error_matches( err,
                             G_IO_ERROR,
                             G_IO_ERROR_NOT_FOUND );

        if ( file_not_found && g_warn_cfg_file_not_found )
        {
            printf( "warning: cfg file doesn't exist: %s\n", fname );
        }
    }

    g_clear_error( &err );
    return res;

} // conf_load_ctx()



GtkTreeIter
conf_mk_ctx_node( EdaConfig*    ctx,
                  gboolean      wok,
                  const gchar*  name,
                  cfg_edit_dlg* dlg )
{
//    gboolean wok = conf_ctx_file_writable( ctx );

//    printf( "conf_mk_ctx_node( %s ): wok: [%d]\n",
//            conf_ctx_name( ctx ),
//            wok );

    // NOTE: rdata:
    //
    row_data* rdata = rdata_mk( ctx,
                                NULL,  // group
                                NULL,  // key
                                NULL,  // val
                                !wok,  // ro
                                FALSE, // inh
                                RT_CTX // rtype
                              );

    GtkTreeIter
    it = tree_add_row( dlg,
                       name,  // name
                       "",    // val
                       rdata, // rdata
                       NULL   // itParent
                     );

    return it;

} // conf_mk_ctx_node()



/*

// reload groups and keys in [ctx]
//
// [path]: string:
//   "0" - ctx: default
//   "1" - ctx: system
//   "2" - ctx: user
//   "3" - ctx: path(.)
//
void
conf_reload_ctx( EdaConfig* ctx, const gchar* path, cfg_edit_dlg* dlg )
{
    tree_filter_remove( dlg ); // NOTE: !!!


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    GtkTreeIter it_ctx;
    gtk_tree_model_get_iter_from_string( mod, &it_ctx, path );


    GtkTreeIter it_child;
    gboolean res = gtk_tree_model_iter_children( mod,
                                                 &it_child,
                                                 &it_ctx );

    if ( res )
    {
        GtkTreeIter it_store = row_get_tstore_iter( dlg, it_child );

        // NOTE: gtk_tree_store_remove():
        //
        // 1) removes node pointed to by iter
        // 2) sets iter to the next node at the same level
        // 3) returns FALSE if there's no more nodes left
        //

        row_data* rdata = NULL;

        do
        {
            // NOTE: free rdata
            //
            rdata = row_field_get_data( dlg, &it_store );
            rm_rdata( rdata );
        }
        while ( gtk_tree_store_remove( dlg->store_, &it_store ) );

    }


    gboolean wok = conf_ctx_file_writable( ctx );

    conf_load_groups( ctx, wok, dlg, it_ctx );



    tree_set_focus( dlg );


    tree_filter_setup( dlg ); // NOTE: !!!

} // conf_reload_ctx()



void
conf_reload_ctx_user( cfg_edit_dlg* dlg )
{
    printf( "-- -- RELOAD CTX: [user] -- --\n" );
    conf_reload_ctx( eda_config_get_user_context(), "2", dlg );
}



void
conf_reload_ctx_path( cfg_edit_dlg* dlg )
{
    printf( "-- -- RELOAD CTX: [path] -- --\n" );
    conf_reload_ctx( eda_config_get_context_for_path( "." ), "3", dlg );
}

*/



// reload all child contexts of [parent_ctx]
//
void
conf_reload_child_ctxs( EdaConfig* parent_ctx, cfg_edit_dlg* dlg )
{
//    gchar* path = row_cur_pos_save( dlg );


    tree_set_focus( dlg );
    a_reload( dlg );
    gui_update( dlg );


//    gui_update_off();
//
//    if ( parent_ctx == eda_config_get_system_context() )
//    {
//        conf_reload_ctx_user( dlg );
//        conf_reload_ctx_path( dlg );
//    }
//    else
//    if ( parent_ctx == eda_config_get_user_context() )
//    {
//        conf_reload_ctx_path( dlg );
//    }
//
//    gui_update_on();
//
//    row_cur_pos_restore( dlg, path );
//    g_free( path );

} // conf_reload_child_ctxs()



void
conf_load( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx_dflt = eda_config_get_default_context();
    EdaConfig* ctx_sys  = eda_config_get_system_context();
    EdaConfig* ctx_user = eda_config_get_user_context();
    EdaConfig* ctx_path = eda_config_get_context_for_path( "." );

    gchar* name_dflt = g_strdup_printf( "1: %s", conf_ctx_name( ctx_dflt ) );
    gchar* name_sys  = g_strdup_printf( "2: %s", conf_ctx_name( ctx_sys  ) );
    gchar* name_user = g_strdup_printf( "3: %s", conf_ctx_name( ctx_user ) );
    gchar* name_path = g_strdup_printf( "4: %s", conf_ctx_name( ctx_path ) );


    // 1: DEFAULT ctx:
    //
    gboolean wok_dflt = FALSE;
    GtkTreeIter it_dflt = conf_mk_ctx_node( ctx_dflt, wok_dflt, name_dflt, dlg );
    g_free( name_dflt );
    if ( g_populate_default_ctx )
    {
        cfgreg_populate_ctx( ctx_dflt );
    }
    conf_load_groups( ctx_dflt, wok_dflt, dlg, it_dflt, g_print_default_cfg );

    if ( g_print_default_cfg ) // TODO: what if g_populate_default_ctx == FALSE?
    {
        if ( !g_populate_default_ctx )
            fprintf( stderr,
                     "nothing to do: you have passed -d,"
                     " hence the DEFAULT config context is empty.\n" );
        exit( 0 );
    }


    // 2: SYSTEM ctx:
    //
    gboolean wok_sys = conf_ctx_file_writable( ctx_sys );
    GtkTreeIter it_sys = conf_mk_ctx_node( ctx_sys, wok_sys, name_sys, dlg );
    g_free( name_sys );
    conf_load_ctx( ctx_sys );
    conf_load_groups( ctx_sys, wok_sys, dlg, it_sys, FALSE );


    // 3: USER ctx:
    //
    gboolean exist_user = FALSE;
    gboolean wok_user = FALSE;
    // DEBUG: const gchar* fname =
        conf_ctx_fname( ctx_user, &exist_user, NULL, NULL );

    if ( !exist_user )
    {
        // creates ~/.config/lepton-eda/ if it doesn't exist
        // and writes empty lepton-user.conf there:
        //
        if ( conf_save( ctx_user, dlg ) )
            wok_user = TRUE;
    }
    else
    {
        wok_user = conf_ctx_file_writable( ctx_user );
    }

    // DEBUG: printf( " .. fname:      [%s]\n", fname );
    // DEBUG: printf( " .. exist_user: [%d]\n", exist_user );
    // DEBUG: printf( " .. wok_user:   [%d]\n", wok_user );

    GtkTreeIter it_user = conf_mk_ctx_node( ctx_user, wok_user, name_user, dlg );
    g_free( name_user );
    conf_load_ctx( ctx_user );
    conf_load_groups( ctx_user, wok_user, dlg, it_user, FALSE );


    // 4: PATH ctx:
    //
    gboolean wok_path = conf_ctx_file_writable( ctx_path );
    GtkTreeIter it_path = conf_mk_ctx_node( ctx_path, wok_path, name_path, dlg );
    g_free( name_path );
    conf_load_ctx( ctx_path );
    conf_load_groups( ctx_path, wok_path, dlg, it_path, FALSE );


    // setup "config-changed" handlers:
    // NOTE: not used for now
    //
    g_signal_connect( G_OBJECT( ctx_dflt ),
                      "config-changed",
                      G_CALLBACK( &on_conf_chg_ctx_dflt ),
                      dlg );

    g_signal_connect( G_OBJECT( ctx_sys ),
                      "config-changed",
                      G_CALLBACK( &on_conf_chg_ctx_sys ),
                      dlg );

    g_signal_connect( G_OBJECT( ctx_user ),
                      "config-changed",
                      G_CALLBACK( &on_conf_chg_ctx_user ),
                      dlg );

    g_signal_connect( G_OBJECT( ctx_path ),
                      "config-changed",
                      G_CALLBACK( &on_conf_chg_ctx_path ),
                      dlg );

} // conf_load()



const gchar*
conf_ctx_name( EdaConfig* ctx )
{
    static gchar def[]  = "default values";
    static gchar sys[]  = "system configuration";
    static gchar user[] = "user configuration";
    static gchar path[] = "configuration for current directory";
//    static gchar def[]  = "DEFAULT";
//    static gchar sys[]  = "SYSTEM";
//    static gchar user[] = "USER";
//    static gchar path[] = "PATH (.)";

    gchar* ret = NULL;

    if ( ctx == eda_config_get_default_context() )
        ret = def;
    if ( ctx == eda_config_get_system_context() )
        ret = sys;
    if ( ctx == eda_config_get_user_context() )
        ret = user;
    if ( ctx == eda_config_get_context_for_path( "." ) )
        ret = path;

    g_assert( ret != NULL && "conf_ctx_name(): ! ctx name" );

    return ret;
}



// {post}: {ret} owned by geda cfg api
//
const gchar*
conf_ctx_fname( EdaConfig* ctx, gboolean* exist, gboolean* rok, gboolean* wok )
{
    const gchar* fname = eda_config_get_filename( ctx );

    if ( fname == NULL )
        return NULL;

    if (exist != NULL)
        *exist = access( fname, F_OK ) == 0;
    if (rok != NULL)
        *rok =   access( fname, R_OK ) == 0;
    if (wok != NULL)
        *wok =   access( fname, W_OK ) == 0;

    return fname;
}



const gboolean
conf_ctx_file_writable( EdaConfig* ctx )
{
    gboolean exist = FALSE;
    gboolean rok   = FALSE;
    gboolean wok   = FALSE;

    const gchar* fname = conf_ctx_fname( ctx, &exist, &rok, &wok );

    gboolean ret = FALSE;

    if ( fname == NULL )
    {
        ret = FALSE;
    }
    else
    if ( exist && wok )
    {
        ret = TRUE;
    }
    else
    if ( !exist )
    {
        gchar* dir = g_path_get_dirname( fname );
        ret = access( dir, W_OK ) == 0;

        // g_debug( " .. conf_ctx_file_writable( %s ): fname: [%s], dir: [%s]: wok: [%d]\n",
                 // conf_ctx_name( ctx ),
                 // fname,
                 // dir,
                 // ret );

        g_free( dir );
    }

    return ret;

} // conf_ctx_file_writable()



void
conf_add_val( row_data* rdata, const gchar* key, const gchar* val )
{
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           key,
                           val );
}



void
conf_chg_val( const row_data* rdata, const gchar* txt )
{
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           rdata->key_,
                           txt );
}



gboolean
conf_del_key( const row_data* rdata )
{
    GError* err = NULL;
    gboolean res =
        eda_config_remove_key( rdata->ctx_,
                               rdata->group_,
                               rdata->key_,
                               &err );
    g_clear_error( &err );

#ifdef DEBUG
    printf( " >> eda_config_remove_key( %s ): [ %d ]\n", rdata->key_, res );
#endif

    return res;
}



gboolean
conf_del_grp( const row_data* rdata )
{
    GError* err = NULL;
    gboolean res =
        eda_config_remove_group( rdata->ctx_,
                                 rdata->group_,
                                 &err );
    g_clear_error( &err );

    printf( " >> eda_config_remove_grp( %s ): [ %d ]\n", rdata->group_, res );

    return res;
}




gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg )
{
    GError*  err = NULL;
    gboolean res = eda_config_save( ctx, &err );

    if ( !res )
    {
#ifdef DEBUG
        printf( " >> conf_save(): !eda_config_save(): [%s]\n",
                err ? err->message : "" );
#endif

        GtkWidget* msgdlg =
        gtk_message_dialog_new( GTK_WINDOW( dlg ),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "An error occurred while trying to"
                                " access the configuration file.\n"
                                "The error message:"
                                "\n\n"
                                "conf_save() => eda_config_save():\n"
                                "%s",
                                err ? err->message : "No error message available." );

        gtk_window_set_title( GTK_WINDOW( msgdlg ), "lepton-conf" );

        gtk_dialog_run( GTK_DIALOG( msgdlg ) );
        gtk_widget_destroy( msgdlg );

        g_clear_error( &err );
    }

    return res;

} // conf_save()

