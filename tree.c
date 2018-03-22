/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




/* ******************************************************************
*
*  rdata:
*
*/

const gchar*
rdata_get_name( const row_data* rdata )
{
    const gchar* name = NULL;

    if ( rdata != NULL )
    {
        if ( rdata->rtype_ == RT_GRP )
        {
            name = rdata->group_;
        }
        else
        if ( rdata->rtype_ == RT_KEY )
        {
            name = rdata->key_;
        }
        else
        if ( rdata->rtype_ == RT_CTX )
        {
            name = conf_ctx_name( rdata->ctx_ );
            //
            // TODO: free() name
        }
    }

    return name;
}



row_data*
rdata_mk( EdaConfig*   ctx,
          const gchar* group,
          const gchar* key,
          const gchar* val,
          gboolean     ro,
          gboolean     inh,
          RowType      rtype )
{
    row_data* rdata = g_malloc( sizeof( row_data ) );

    rdata->ctx_   = ctx;
    rdata->group_ = group ? g_strdup( group ) : NULL;
    rdata->key_   = key   ? g_strdup( key )   : NULL;
    rdata->val_   = val   ? g_strdup( val )   : NULL;
    rdata->ro_    = ro;
    rdata->inh_   = inh;
    rdata->rtype_ = rtype;

    return rdata;
}



void
rdata_rm( row_data* rdata )
{
    if ( rdata == NULL )
        return;

    g_free( rdata->group_ );
    g_free( rdata->key_ );
    g_free( rdata->val_ );

    g_free( rdata );
}



gboolean
rdata_rm_func( GtkTreeModel* mod,
               GtkTreePath*  path,
               GtkTreeIter*  it,
               gpointer      p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    row_data* rdata = row_field_get_data( dlg, it );
    rdata_rm( rdata );

    return FALSE; // FALSE => continue gtk_tree_model_foreach()
}




/* ******************************************************************
*
*  tree:
*
*/

// tree columns:
//
enum
{
    COL_NAME,
    COL_VAL,
    COL_DATA, // rdata: hidden
    NUM_COLS
};




int tree_colid_name()  { return COL_NAME; }
int tree_colid_val()   { return COL_VAL;  }
int tree_colid_data()  { return COL_DATA; }
int tree_cols_cnt()    { return NUM_COLS; }



void
tree_set_focus( cfg_edit_dlg* dlg )
{
    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

    // XXX: hack:
    //
    // body of row_cur_get_iter( dlg, &it ):
    //
    // GtkTreeIter it;
    // GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    // gboolean res = gtk_tree_selection_get_selected( sel, NULL, &it );
    //
    // if ( !res )
    // {
    //     gtk_test_widget_send_key( GTK_WIDGET( dlg->tree_v_ ),
    //                               GDK_KEY_Return,
    //                               (GdkModifierType) 0 );
    // }

} // tree_set_focus()



// TODO: figure out how to use it:
//
gint
tree_sort_cmp_fun( GtkTreeModel* model,
                   GtkTreeIter*  a,
                   GtkTreeIter*  b,
                   gpointer      data )
{
    printf ( "tree_sort_cmp_fun()\n" );

    GtkTreePath* path_a = gtk_tree_model_get_path( model, a );
    gint depth = gtk_tree_path_get_depth( path_a );
    gtk_tree_path_free( path_a );

    // root node => do not sort:
    //
    if ( depth == 1 )
        return 0;

    cfg_edit_dlg* dlg = (cfg_edit_dlg*) data;

    row_data* rdata_a = row_field_get_data( dlg, a );
    row_data* rdata_b = row_field_get_data( dlg, b );

    // TODO: free [?] names:
    //
    const gchar* name_a = rdata_get_name( rdata_a );
    const gchar* name_b = rdata_get_name( rdata_b );

    printf ( "sort_cmp_fun( %s, %s )\n", name_a, name_b );

    return g_strcmp0( name_a, name_b );

} // tree_sort_cmp_fun()



static gboolean
tree_filter( GtkTreeModel* model, GtkTreeIter* it, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    // NOTE: models are different:
    //
    // printf( " >> tree_filter( model ):              model: %p\n",
    //         model );
    // printf( " >> tree_filter(): gtk_tree_view_get_model(): %p\n",
    //         gtk_tree_view_get_model( dlg->tree_v_ ) );

    row_data* rdata = NULL;
    gtk_tree_model_get( model, it, tree_colid_data(), &rdata, -1 );

    // FAIL: const row_data* rdata = row_field_get_data( dlg, it );

    if ( !rdata )
        return FALSE;

    if ( !dlg->showinh_ )
        return !rdata->inh_;

    return TRUE;

} // tree_filter()



void
tree_filter_setup( cfg_edit_dlg* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeModel* modf = gtk_tree_model_filter_new( mod, NULL );

    gtk_tree_model_filter_set_visible_func(
        GTK_TREE_MODEL_FILTER( modf ),
        &tree_filter,
        dlg,
        NULL);

    gtk_tree_view_set_model( dlg->tree_v_, modf );

} // tree_filter_setup()



void
tree_filter_remove( cfg_edit_dlg* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gtk_tree_view_set_model( dlg->tree_v_, GTK_TREE_MODEL( dlg->store_ ) );
}



static void
tree_cell_draw( GtkTreeViewColumn* col,
                GtkCellRenderer*   ren,
                GtkTreeModel*      model,
                GtkTreeIter*       it,
                gpointer           p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    if ( ren != dlg->ren_txt_ )
        return;


    const row_data* rdata = row_field_get_data( dlg, it );
    if ( !rdata )
        return;


    if ( rdata->inh_ )
    {
        g_object_set( ren, "foreground", "gray", NULL );
        g_object_set( ren, "style", PANGO_STYLE_ITALIC, NULL );
    }
    else
    {
        g_object_set( ren, "foreground", "black", NULL );
        g_object_set( ren, "style", PANGO_STYLE_NORMAL, NULL );
    }

} // tree_cell_draw()



void
tree_add_col( cfg_edit_dlg*    dlg,
              GtkCellRenderer* ren,
              const gchar*     prop,
              gint             col_id,
              const gchar*     title )
{
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, title );
    gtk_tree_view_column_pack_start( col, ren, TRUE );
    gtk_tree_view_column_add_attribute( col, ren, prop, col_id );

    gtk_tree_view_column_set_cell_data_func( col,
                                             ren,
                                             &tree_cell_draw,
                                             dlg,
                                             NULL );

    gtk_tree_view_append_column( dlg->tree_v_, col );

} // tree_add_col()



GtkTreeIter
tree_add_row( cfg_edit_dlg* dlg,
              const gchar*  name,
              const gchar*  val,
              gpointer      rdata,
              GtkTreeIter*  itParent )
{
    GtkTreeIter it;
    gtk_tree_store_append( dlg->store_, &it, itParent );

    // NOTE: gtk_tree_store_set() makes copies of strings:
    //
    gtk_tree_store_set( dlg->store_,
                        &it,
                        tree_colid_name(),     name,
                        tree_colid_val(),      val,
                        tree_colid_data(),     rdata,
                        -1 );

    return it;

} // tree_add_row()

