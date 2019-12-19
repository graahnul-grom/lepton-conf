/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




// [path_mod]: path within model
//
void
row_select_by_path_mod( cfg_edit_dlg* dlg, GtkTreePath* path_mod )
{
    g_return_if_fail( path_mod != 0 && "row_select_by_path_mod()" );

    gtk_tree_view_expand_to_path( dlg->tree_v_, path_mod );
    gtk_tree_view_set_cursor( dlg->tree_v_, path_mod, NULL, FALSE );
}



// [path_tstore]: tstore path
//
static void
row_select_by_path_tstore( cfg_edit_dlg* dlg, GtkTreePath* path_tstore )
{
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    // path within current tree model:
    GtkTreePath* path_mod =
        gtk_tree_model_filter_convert_child_path_to_path(
            GTK_TREE_MODEL_FILTER( mod ), path_tstore );

    g_return_if_fail( path_tstore != 0 && "row_select_by_path_tstore()" );

    row_select_by_path_mod( dlg, path_mod );

    gtk_tree_path_free( path_mod );

} // row_select_by_path_tstore()



void
row_select_by_iter_tstore( cfg_edit_dlg* dlg, GtkTreeIter it_tstore )
{
    GtkTreeModel* mod = NULL;

    mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeModelSort* mods = GTK_TREE_MODEL_SORT( mod );

    mod = gtk_tree_model_sort_get_model( mods );
    GtkTreeModelFilter* modf = GTK_TREE_MODEL_FILTER( mod );

    GtkTreeIter it_f;
    gtk_tree_model_filter_convert_child_iter_to_iter( modf, &it_f, &it_tstore );

    GtkTreeIter it_s;
    gtk_tree_model_sort_convert_child_iter_to_iter( mods, &it_s, &it_f );


    // NOTE: exactly this call sequence:
    //       (expand to path -> set sursor -> scroll to cell)
    //
    mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it_s );
    gtk_tree_view_expand_to_path( dlg->tree_v_, path );
    gtk_tree_view_set_cursor( dlg->tree_v_, path, NULL, FALSE );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, FALSE, 0, 0 );
    gtk_tree_path_free( path );


    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    gtk_tree_selection_select_iter( sel, &it_s );
//    GtkTreeIter i;
//    gboolean res = gtk_tree_selection_get_selected( sel, NULL, &i );
//    printf( " >> RES: [%d]\n", res );

} // row_select_by_iter_tstore()



// selects nearest non-inherited row in hierarchy (upwards)
// i.e. select parent, then, if parent is inh too - parent of parent:
//
void
row_select_non_inh( cfg_edit_dlg* dlg, GtkTreeIter it )
{
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    g_return_if_fail( path != NULL && "row_select_non_inh()" );

    gtk_tree_path_up( path );
    row_select_by_path_mod( dlg, path );


    // check if this is also inherited:
    //
    GtkTreeIter it_2;
    if ( gtk_tree_model_get_iter( mod, &it_2, path ) )
    {
        GtkTreePath* path_2 = gtk_tree_model_get_path( mod, &it_2 );
        g_return_if_fail( path_2 != NULL && "row_select_non_inh()" );

        row_data* rdata_2 = row_field_get_data( dlg, &it_2 );
        g_return_if_fail( rdata_2 != NULL && "row_select_non_inh()" );

        if ( rdata_2->inh_ )
        {
            gtk_tree_path_up( path_2 );
            row_select_by_path_mod( dlg, path_2 );
        }

        gtk_tree_path_free( path );
        gtk_tree_path_free( path_2 );
    }
    else
    {
        g_return_if_fail( FALSE && "row_select_non_inh(): !it_2" );
    }

} // row_select_non_inh()



// {ret}: tree store iter corresponding to model's iter [it_smod]
//
GtkTreeIter
row_get_tstore_iter( cfg_edit_dlg* dlg, GtkTreeIter it_smod )
{
//    NOTE: manual impl:
//
//    GtkTreeIter itModel = it;
//    GtkTreeModelFilter* filtModel = GTK_TREE_MODEL_FILTER( dlg_model( dlg ) );
//    GtkTreeModel* childModel = gtk_tree_model_filter_get_model( filtModel );
//    GtkTreePath* path = gtk_tree_model_get_path( dlg_model( dlg ), &itModel );
//    GtkTreeIter itStore;
//    gtk_tree_model_get_iter( childModel, &itStore, path );
//    return itStore;


    // NOTE: no filter model set:
    //
//    GtkTreeModel* model = gtk_tree_view_get_model( dlg->tree_v_ );
//    GtkTreeModel* modelStore = GTK_TREE_MODEL( dlg->store_ );
//    if ( model == modelStore )
//        return it;
//
//    GtkTreeModelFilter* modelFilt = GTK_TREE_MODEL_FILTER( model );
//    GtkTreeIter itModel = it;
//    GtkTreeIter itStore;
//    gtk_tree_model_filter_convert_iter_to_child_iter( modelFilt,
//                                                      &itStore,
//                                                      &itModel );
//    return itStore;




    // tree models relationshp:
    //
    //         v---------------> "child" model of mods
    // mods -> modf -> store
    //                 ^-------> "child" model of modf
    //
    GtkTreeIter it_fmod;
    GtkTreeIter it_stor;

    // mod => mods:
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeModelSort* mods = GTK_TREE_MODEL_SORT( mod );

    // it_smod => it_fmod:
    //
    gtk_tree_model_sort_convert_iter_to_child_iter( mods, &it_fmod, &it_smod );

    // mods => modf:
    //
    mod = gtk_tree_model_sort_get_model( mods );
    GtkTreeModelFilter* modf = GTK_TREE_MODEL_FILTER( mod );

    // it_fmod => it_stor:
    //
    gtk_tree_model_filter_convert_iter_to_child_iter( modf, &it_stor, &it_fmod );

    return it_stor;
    //
    //

} // row_get_tstore_iter()



// {ret}: iterator of currently selected row
//
gboolean
row_cur_get_iter( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );


    GtkTreeModel* model = NULL;
    gboolean res = gtk_tree_selection_get_selected( sel, &model, it );

#ifdef DEBUG
    if ( !res )
        printf( " ** ** ** row_cur_get_iter(): !sel\n" );
    // g_return_val_if_fail( res && "row_cur_get_iter(): !sel\n", res );
#endif

    return res;

} // row_cur_get_iter()



// {post}: allocates return string on success
//
gchar*
row_cur_pos_save( cfg_edit_dlg* dlg )
{
    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
    {
        printf( " >> >> row_cur_pos_save(): !it\n");
        return NULL;
    }

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gchar* path_str = gtk_tree_model_get_string_from_iter( mod, &it );

    return path_str;
}



void
row_cur_pos_restore( cfg_edit_dlg* dlg, gchar* path_str )
{
    if ( !path_str )
    {
        printf( " >> row_cur_pos_restore(): !path_str\n");
        return;
    }

    GtkTreePath* path = gtk_tree_path_new_from_string( path_str );
    if ( !path )
    {
        printf( " >> row_cur_pos_restore(): !path\n");
        return;
    }


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeIter it;
    if ( !gtk_tree_model_get_iter( mod, &it, path ) )
    {
        // TODO: when this happens?
        printf( " >> row_cur_pos_restore(): !it\n" );
        // was: gtk_tree_path_up( path );
    }


    //
    // NOTE: hack: temporary select the first node ("0" path):
    // restoring saved row sometimes breaks scrolling - the row
    // goes out of sight and scroll bar is displayed incorrectly
    //
    GtkTreePath* p0 = gtk_tree_path_new_from_string( "0" );
    // gtk_tree_view_expand_to_path( dlg->tree_v_, p0 );
    gtk_tree_view_set_cursor( dlg->tree_v_, p0, NULL, FALSE );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, p0, NULL, FALSE, 0, 0 );
    gtk_tree_path_free( p0 );
    //
    //


    gtk_tree_view_expand_to_path( dlg->tree_v_, path );
    gtk_tree_view_set_cursor( dlg->tree_v_, path, NULL, FALSE );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, TRUE, 0.5, 0 );

    gtk_tree_path_free( path );

} // row_cur_pos_restore()



row_data*
row_field_get_data( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    row_data* rdata = NULL;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_get( mod, it, tree_colid_data(), &rdata, -1 );

    return rdata;
}



void
row_field_set_val( cfg_edit_dlg* dlg, GtkTreeIter it, const gchar* val )
{
    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    g_free( rdata->val_ );
    rdata->val_ = g_strdup( val );

    GtkTreeIter itStore = row_get_tstore_iter( dlg, it );
    gtk_tree_store_set( dlg->store_,
                        &itStore,
                        tree_colid_val(), val,
                        -1 );

} // row_field_set_val()



// {post}: caller must gtk_tree_path_free( {ret} )
// {ret}: path of row with name [name] if it's a child of [it_parent] row
//
GtkTreePath*
row_find_child_by_name( cfg_edit_dlg* dlg,
                        GtkTreeIter it_parent,
                        const gchar* name )
{
    row_data* rdata = row_field_get_data( dlg, &it_parent );
    if ( !rdata )
        return NULL;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    GtkTreeIter it_child;
    gboolean next = gtk_tree_model_iter_children( mod,
                                                  &it_child,
                                                  &it_parent );
    GtkTreePath* ret = NULL;

    for ( ; next; next = gtk_tree_model_iter_next( mod, &it_child ) )
    {
        row_data* rd = row_field_get_data( dlg, &it_child );

        const gchar* rdata_name = rdata_get_name( rd );
        if ( rdata_name == NULL )
            continue;

        if ( g_strcmp0( rdata_name, name ) == 0 )
        {
            gchar* str = gtk_tree_model_get_string_from_iter( mod, &it_child );
            ret = gtk_tree_path_new_from_string( str );
            g_free( str );
            break;
        }
    }

    return ret;

} // row_find_child_by_name()



void
row_key_unset_inh( cfg_edit_dlg* dlg, GtkTreeIter it )
{
    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    g_assert( rdata->rtype_ == RT_KEY && " >> row_key_unset_inh(): !key" );

    rdata->inh_ = FALSE;


    GtkTreeIter it_parent;
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    gtk_tree_path_up( path );

    if ( gtk_tree_model_get_iter( mod, &it_parent, path ) )
    {
        rdata = row_field_get_data( dlg, &it_parent );
        if ( rdata )
            rdata->inh_ = FALSE;
    }

} // row_unset_inh()



void
row_select_parent( cfg_edit_dlg* dlg, GtkTreeIter it )
{
    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    if ( !gtk_tree_path_up( path ) )
    {
        printf( " >> row_select_parent(): !gtk_tree_path_up()\n" );
        return;
    }

    GtkTreeIter parent_it;
    if ( gtk_tree_model_get_iter( mod, &parent_it, path ) )
    {
        row_select_by_path_mod( dlg, path );
    }
    else
    {
        printf( " >> row_select_parent(): !parent_it\n" );
    }

} // row_select_parent()



void
row_select_by_ctx_grp_key( cfg_edit_dlg* dlg,
                           const gchar* ctx_name,
                           const gchar* grp_name,
                           const gchar* key_name )
{
    g_return_if_fail( ctx_name && "row_select_ctx_grp_key(): !ctx_name" );

    gchar* ctx = g_strdup( ctx_name );
    gchar* grp = g_strdup( grp_name );
    gchar* key = g_strdup( key_name );


    // determine ctx path by name:
    gchar* str_ctx = "";
    if ( strcmp( ctx, "DEFAULT" ) == 0 )
        str_ctx = "0";
    else
    if ( strcmp( ctx, "SYSTEM" ) == 0 )
        str_ctx = "1";
    else
    if ( strcmp( ctx, "USER" ) == 0 )
        str_ctx = "2";
    else
    if ( strcmp( ctx, "PATH (.)" ) == 0 )
        str_ctx = "3";

    g_assert( strlen( str_ctx ) > 0 && "row_select_ctx_grp_key(): !pstr" );


    // select ctx:
    GtkTreePath* path_ctx = gtk_tree_path_new_from_string( str_ctx );
    gtk_tree_view_expand_to_path( dlg->tree_v_, path_ctx );
    gtk_tree_view_set_cursor( dlg->tree_v_, path_ctx, NULL, FALSE );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, path_ctx, NULL, TRUE, 0.5, 0 );
    gtk_tree_path_free( path_ctx );


    // 3: if [grp]: find grp by name under selected ctx, select it if found:
    if ( grp )
    {
        GtkTreeIter it_ctx;
        row_cur_get_iter( dlg, &it_ctx );

        GtkTreePath* path_grp = row_find_child_by_name( dlg, it_ctx, grp );
        if ( path_grp != NULL ) // can be NULL if not found
        {
            row_select_by_path_mod( dlg, path_grp );
            gtk_tree_path_free( path_grp );
        }


        // 4: if [key]: find key by name under selected grp, select it if found:
        if ( key )
        {
            GtkTreeIter it_grp;
            row_cur_get_iter( dlg, &it_grp );

            GtkTreePath* path_key = row_find_child_by_name( dlg, it_grp, key );
            if ( path_key != NULL ) // can be NULL if not found
            {
                row_select_by_path_mod( dlg, path_key );
                gtk_tree_path_free( path_key );
            }

        } // if [key]

    } // if [grp]

    g_free( ctx );
    g_free( grp );
    g_free( key );

} // row_select_by_ctx_grp_key()



void
row_select_by_cgk( cfg_edit_dlg* dlg, const row_cgk* cgk )
{
    row_select_by_ctx_grp_key( dlg, cgk->ctx_, cgk->grp_, cgk->key_);
}

