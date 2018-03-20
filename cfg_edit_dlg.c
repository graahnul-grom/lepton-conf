/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif

#include "cfg_edit_dlg.h"




gboolean g_close_with_esc = FALSE;
gboolean g_populate_default_ctx = TRUE;
// TODO: const gchar g_exted_default[] = "gvim";




/* ******************************************************************
*
*  forward declarations:
*
*/




/* ******************************************************************
*
*  gobject stuff:
*
*/

G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);



GtkWidget* cfg_edit_dlg_new()
{
  gpointer obj = g_object_new (CFG_EDIT_DLG_TYPE, NULL);
  return GTK_WIDGET (obj);
}



static void
cfg_edit_dlg_get_property( GObject* obj, guint id, GValue* val, GParamSpec* spec )
{
    // cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );
    G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
}



static void
cfg_edit_dlg_set_property( GObject* obj, guint id, const GValue* val, GParamSpec* spec )
{
    // cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );
    G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
}



static void
cfg_edit_dlg_dispose( GObject* obj )
{
    // g_debug( "cfg_edit_dlg::dispose( %p ); refcnt: %d\n",
        // obj, obj ? obj->ref_count : 0 );

    cfg_edit_dlgClass* cls = CFG_EDIT_DLG_GET_CLASS( obj );

    GObjectClass* parent_cls = g_type_class_peek_parent( cls );
    parent_cls->dispose( obj );

    // g_debug( "cfg_edit_dlg::dispose(): done.\n" );
}



static void
cfg_edit_dlg_on_show( GtkWidget* widget )
{
    settings_restore( widget );
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->show(widget);
}



static void
cfg_edit_dlg_on_unmap( GtkWidget* widget )
{
    settings_save( widget );
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->unmap(widget);
}



static void
cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
//    printf( " >> cfg_edit_dlg_class_init()\n" );

    // NOTE: glib debug messages:
    // NOTE: g_debug() adds newline:
    //
    // g_debug( "cfg_edit_dlg::class_init()" );

    GObjectClass* gcls = G_OBJECT_CLASS( cls );

    gcls->dispose      = &cfg_edit_dlg_dispose;
    gcls->get_property = &cfg_edit_dlg_get_property;
    gcls->set_property = &cfg_edit_dlg_set_property;


    GtkWidgetClass* wcls = GTK_WIDGET_CLASS( cls );

    // setup onShow/onClose handlers for dialog:
    //
    wcls->show =  &cfg_edit_dlg_on_show;
    wcls->unmap = &cfg_edit_dlg_on_unmap;
}



static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
//    printf( " ++ cfg_edit_dlg_init()\n" );

    // g_debug( "cfg_edit_dlg::cfg_edit_dlg_init()\n" );

    // by default, do not show inherited:
    //
    dlg->showinh_ = FALSE;


    gchar* cwd = g_get_current_dir();
    gui_mk( dlg, cwd );
    g_free( cwd );

    // load data:
    //
    conf_load( dlg );

    tree_filter_setup( dlg );
    gtk_widget_show_all( GTK_WIDGET( dlg ) );
    events_setup( dlg );

    tree_set_focus( dlg );
    gui_update_on();
    xxx_update_gui( dlg );

//    printf( " -- cfg_edit_dlg_init()\n" );
}




/* ******************************************************************
*
*  rdata:
*
*/

const gchar*
rdata_get_name( const row_data* rdata )
{
    gchar* name = NULL;

    if ( rdata != NULL )
    {
        if ( rdata->rtype_ == RT_GRP )
            name = rdata->group_;
        else
        if ( rdata->rtype_ == RT_KEY )
            name = rdata->key_;
    }

    return name;
}



row_data*
mk_rdata( EdaConfig*   ctx,
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
rm_rdata( row_data* rdata )
{
    if ( rdata == NULL )
        return;

    g_free( rdata->group_ );
    g_free( rdata->key_ );
    g_free( rdata->val_ );

    g_free( rdata );
}



gboolean
rm_rdata_func( GtkTreeModel* mod,
               GtkTreePath*  path,
               GtkTreeIter*  it,
               gpointer      p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    row_data* rdata = row_field_get_data( dlg, it );
    rm_rdata( rdata );

    return FALSE; // FALSE => continue gtk_tree_model_foreach()
}




/* ******************************************************************
*
*  row:
*
*/


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
    // path within tstore:
    GtkTreePath* path_tstore =
        gtk_tree_model_get_path( GTK_TREE_MODEL( dlg->store_ ), &it_tstore );

    g_return_if_fail( path_tstore != 0 && "row_select_by_iter_tstore()" );

    row_select_by_path_tstore( dlg, path_tstore );

    gtk_tree_path_free( path_tstore );

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



// {ret}: tree store iter corresponding to model's iter [it]
//
GtkTreeIter
row_get_tstore_iter( cfg_edit_dlg* dlg, GtkTreeIter it )
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
    GtkTreeModel* model = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeModel* modelStore = GTK_TREE_MODEL( dlg->store_ );
    if ( model == modelStore )
        return it;

    GtkTreeModelFilter* modelFilt = GTK_TREE_MODEL_FILTER( model );
    GtkTreeIter itModel = it;
    GtkTreeIter itStore;
    gtk_tree_model_filter_convert_iter_to_child_iter( modelFilt,
                                                      &itStore,
                                                      &itModel );
    return itStore;

} // row_get_tstore_iter()



// {ret}: iterator of currently selected row
//
gboolean
row_cur_get_iter( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );


    GtkTreeModel* model = NULL;
    gboolean res = gtk_tree_selection_get_selected( sel, &model, it );

//    gboolean res = gtk_tree_selection_get_selected( sel, NULL, it );
    g_return_val_if_fail( res && "row_cur_get_iter(): !sel\n", res );

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




/* ******************************************************************
*
*  TODO: refactoring:
*
*/

void
xxx_chg_val( cfg_edit_dlg*   dlg,
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

} // xxx_chg_val()



void
xxx_toggle( cfg_edit_dlg* dlg )
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
        xxx_chg_val( dlg, rdata, it, "false" );
    }
    else
    if ( g_strcmp0( rdata->val_, "false" ) == 0 )
    {
        xxx_chg_val( dlg, rdata, it, "true" );
    }
    else
    if ( g_strcmp0( rdata->val_, "enabled" ) == 0 )
    {
        xxx_chg_val( dlg, rdata, it, "disabled" );
    }
    else
    if ( g_strcmp0( rdata->val_, "disabled" ) == 0 )
    {
        xxx_chg_val( dlg, rdata, it, "enabled" );
    }

} // xxx_toggle()



// update labels, disable/enable controls
// {pre}: tree has focus, row selected
//
void
xxx_update_gui( cfg_edit_dlg* dlg )
{
//    printf( " >> xxx_update_gui(): enabled: [%d]\n", gui_update_enabled() );

    if ( !gui_update_enabled() )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gtk_label_set_text( GTK_LABEL( dlg->lab_ctx_ ), conf_ctx_name( rdata->ctx_ ) );

    if ( rdata->rtype_ == RT_GRP || rdata->rtype_ == RT_KEY )
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_grp_ ), rdata->group_ );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_grp_ ), NULL );
    }

    if ( rdata->rtype_ == RT_KEY )
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), rdata->key_ );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  rdata->val_ );

        const gchar* desc = cfgreg_lookup_descr( rdata->group_, rdata->key_ );
        gtk_text_buffer_set_text( dlg->txtbuf_desc_, desc ? desc : "", -1 );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  NULL );

        gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );
    }

    // set sensitivity for add btn
    //
    if ( (rdata->rtype_ == RT_CTX || rdata->rtype_ == RT_GRP) && !rdata->ro_)
    {
        gtk_widget_set_sensitive( dlg->btn_add_, TRUE );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_add_, FALSE );
    }

    // set sensitivity for edit btn
    //
    if ( (rdata->rtype_ == RT_KEY) && !rdata->ro_)
    {
        gtk_widget_set_sensitive( dlg->btn_edit_, TRUE );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_edit_, FALSE );
    }

    // set sensitivity for toggle btn
    //
    if ( (rdata->rtype_ == RT_KEY) && !rdata->ro_ )
    {
        gboolean can_toggle = cfgreg_can_toggle( rdata->val_ );
        gtk_widget_set_sensitive( dlg->btn_toggle_, can_toggle );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_toggle_, FALSE );
    }


    gboolean exist = FALSE;
    gboolean rok   = FALSE;
    gboolean wok   = FALSE;

    const gchar* fname = conf_ctx_fname( rdata->ctx_, &exist, &rok, &wok );

    gchar* str_access = NULL;
    gchar* str_markup = NULL;

    if ( fname != NULL )
    {
        if ( !exist )
            str_access = g_strdup( " <b>[doesn't exist]</b>" );
        else
        if ( !wok )
            str_access = g_strdup( " <b>[read only]</b>" );

//        str_access = g_strdup_printf( "[%s%s%s]",
//                                      exist ? "f" : "-",
//                                      rok   ? "r" : "-",
//                                      wok   ? "w" : "-" );

        str_markup = g_strdup_printf( "<a href='%s'>%s</a>%s",
                                     fname, fname,
                                     str_access ? str_access : "" );
    }

    gtk_label_set_markup( GTK_LABEL( dlg->lab_fname_ ), str_markup ? str_markup : "" );

    g_free( str_access );
    g_free( str_markup );

//    printf( " >> on_row_sel(): ctx fname: [%s]\n", fname );
//    printf( " >> on_row_sel(): name: [%s], val: [%s]\n", name, val );

} // xxx_update_gui()



void
xxx_showinh( cfg_edit_dlg* dlg, gboolean show )
{
    dlg->showinh_ = show;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( mod ) );
    tree_set_focus( dlg );

}



void
xxx_reload( cfg_edit_dlg* dlg )
{
    gui_update_off();

    char* path = row_cur_pos_save( dlg );

    tree_filter_remove( dlg );

    // NOTE: free rdata
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_foreach( mod, &rm_rdata_func, dlg );

    gtk_tree_store_clear( dlg->store_ );

    conf_load( dlg );

    tree_filter_setup( dlg );

    tree_set_focus( dlg );

    row_cur_pos_restore( dlg, path );
    g_free( path );

    gui_update_on();

} // xxx_reload()

