#include "cfg_edit_dlg.h"




int tree_colid_name()  { return COL_NAME; }
int tree_colid_val()   { return COL_VAL;  }
int tree_colid_data()  { return COL_DATA; }
int tree_cols_cnt()    { return NUM_COLS; }



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

