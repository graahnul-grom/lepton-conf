#include "cfg_edit_dlg.h"




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

        // unset inherited:
        //
        row_key_unset_inh( dlg, it );

        // NOTE: conf_reload_child_ctxs()
        //
        conf_reload_child_ctxs( rdata->ctx_, dlg );
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


    if ( g_strcmp0( rdata->val_, "true" ) == 0 )
    {
        a_chg_val( dlg, rdata, it, "false" );
    }
    else
    if ( g_strcmp0( rdata->val_, "false" ) == 0 )
    {
        a_chg_val( dlg, rdata, it, "true" );
    }
    else
    if ( g_strcmp0( rdata->val_, "enabled" ) == 0 )
    {
        a_chg_val( dlg, rdata, it, "disabled" );
    }
    else
    if ( g_strcmp0( rdata->val_, "disabled" ) == 0 )
    {
        a_chg_val( dlg, rdata, it, "enabled" );
    }

} // a_toggle()



void
a_showinh( cfg_edit_dlg* dlg, gboolean show )
{
    dlg->showinh_ = show;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( mod ) );
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

