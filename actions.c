/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




void
a_chg_val( cfg_edit_dlg*   dlg,
           const row_data* rdata,
           GtkTreeIter     it,
           const gchar*    txt )
{
    if ( txt == NULL )
        return;

    if ( !rdata->inh_ && g_strcmp0( rdata->val_, txt ) == 0 )
        return;

    // NOTE: conf_chg_val() / conf_save()
    //
    conf_chg_val( rdata, txt );

    if ( conf_save( rdata->ctx_, dlg ) )
    {
        row_field_set_val( dlg, it, txt );




        // XXX:  fixes crash in subsequent calls to row_field_get_data()
        // NOTE: crashes only if sort model is used
        // TODO: does gtk_tree_store_set() invalidate iterators??
        //
        row_cur_get_iter( dlg, &it );
        //
        //
        //
        //




        // unset inherited:
        //
        row_key_unset_inh( dlg, it );


        // >>>> remember selected item:
        //
        const row_data* rd_cur = row_field_get_data( dlg, &it );
        row_cgk* cgk = cgk_mk( rd_cur );


        // NOTE: conf_reload_child_ctxs()
        //
        conf_reload_child_ctxs( rdata->ctx_, dlg );


        // >>>> restore selected item:
        //
        row_select_by_cgk( dlg, cgk );
        cgk_rm( cgk );
    }

} // a_chg_val()



void
a_toggle( cfg_edit_dlg* dlg )
{
    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    if ( rdata->rtype_ != RT_KEY || rdata->ro_ )
        return;


    const gchar* val = NULL;

    if ( strcmp( rdata->val_, "true" ) == 0 )
    {
        val = "false";
    }
    else
    if ( strcmp( rdata->val_, "false" ) == 0 )
    {
        val = "true";
    }

    a_chg_val( dlg, rdata, it, val );

} // a_toggle()



void
a_showinh( cfg_edit_dlg* dlg, gboolean show )
{
    dlg->showinh_ = show;


    GtkTreeModel*       mod  = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeModelSort*   mods = GTK_TREE_MODEL_SORT( mod );
                        mod  = gtk_tree_model_sort_get_model( mods );
    GtkTreeModelFilter* modf = GTK_TREE_MODEL_FILTER( mod );

    gtk_tree_model_filter_refilter( modf );


    tree_set_focus( dlg );

} // a_showinh()



void
a_reload( cfg_edit_dlg* dlg )
{
    gui_update_off();

    char* path = row_cur_pos_save( dlg );

    tree_filter_remove( dlg );

    // NOTE: free rdata
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_foreach( mod, &rdata_rm_func, dlg );

    gtk_tree_store_clear( dlg->store_ );

    conf_load( dlg );

    tree_filter_setup( dlg );

    tree_set_focus( dlg );

    row_cur_pos_restore( dlg, path );
    g_free( path );

    gui_update_on();

} // a_reload()



void
a_init( cfg_edit_dlg* dlg )
{
//    printf( " ++ a_init()\n" );

    dlg->showinh_ = FALSE;

    gchar* cwd = g_get_current_dir();
    gui_mk( dlg, cwd );
    g_free( cwd );

    conf_load( dlg );

    tree_filter_setup( dlg );

    gtk_widget_show_all( GTK_WIDGET( dlg ) );

    settings_restore_showinh( dlg );
    settings_restore_path( dlg );

    events_setup( dlg );

    tree_set_focus( dlg );

    gui_update_on();
    gui_update( dlg );

//    printf( " -- a_init()\n" );
}



void
a_delete( cfg_edit_dlg* dlg )
{
    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    if ( rdata->ro_ || rdata->inh_ )
        return;


    if ( rdata->rtype_ == RT_KEY && !conf_del_key( rdata ) )
    {
        printf( " >> a_delete(): !conf_del_key()\n" );
        return;
    }

    if ( rdata->rtype_ == RT_GRP && !conf_del_grp( rdata ) )
    {
        printf( " >> a_delete(): !conf_del_grp()\n" );
        return;
    }


    if ( conf_save( rdata->ctx_, dlg ) )
    {
        row_select_parent( dlg, it );

        // NOTE: do not delete [rdata] here

        // NOTE: conf_reload_child_ctxs()
        //
        conf_reload_child_ctxs( rdata->ctx_, dlg );
    }
    else
    {
        printf( " >> a_delete(): !conf_save()\n" );
    }

} // a_delete()

