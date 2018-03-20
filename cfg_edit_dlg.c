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

#include <gdk/gdkkeysyms.h>




gboolean g_close_with_esc = FALSE;
gboolean g_populate_default_ctx = TRUE;
static const gchar g_exted_default[] = "gvim";




GtkWidget* cfg_edit_dlg_new()
{
  gpointer obj = g_object_new (CFG_EDIT_DLG_TYPE, NULL);
  return GTK_WIDGET (obj);
}




/* ******************************************************************
*
*  forward declarations:
*
*/

static void
events_setup( cfg_edit_dlg* dlg );

static void
tree_filter_setup( cfg_edit_dlg* p );

static void
row_select_non_inh( cfg_edit_dlg* dlg, GtkTreeIter it );

static void
on_btn_showinh( GtkToggleButton* btn, gpointer* p );

static void
xxx_chg_val( cfg_edit_dlg* dlg, const row_data* rdata, GtkTreeIter it, const gchar* txt );

static void
xxx_toggle( cfg_edit_dlg* dlg );

static void
xxx_update_gui( cfg_edit_dlg* dlg );




/* ******************************************************************
*
*  gobject stuff: define class:
*
*/
G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);




/* ******************************************************************
*
*  gobject stuff:
*
*/

// NOTE: moved above: G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);


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

static const gchar*
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



static void
rm_rdata( row_data* rdata )
{
    if ( rdata == NULL )
        return;

    g_free( rdata->group_ );
    g_free( rdata->key_ );
    g_free( rdata->val_ );

    g_free( rdata );
}



static gboolean
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
static void
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



static void
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
static void
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
static GtkTreeIter
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



/* unused:
 *
// {post}: caller must free {ret}
//
static gchar*
row_field_get_name( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    gchar* val = NULL;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_get( mod, it, tree_colid_name(), &val, -1 );

    return val;
}



// {post}: caller must free {ret}
//
static gchar*
row_field_get_val( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    gchar* val = NULL;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_get( mod, it, tree_colid_val(), &val, -1 );

    return val;
}
*
*/



row_data*
row_field_get_data( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    row_data* rdata = NULL;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_get( mod, it, tree_colid_data(), &rdata, -1 );

    return rdata;
}



static void
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
static GtkTreePath*
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



static void
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
*  tree:
*
*/

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



static void
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



static void
tree_filter_remove( cfg_edit_dlg* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gtk_tree_view_set_model( dlg->tree_v_, GTK_TREE_MODEL( dlg->store_ ) );
}








/* ******************************************************************
*
*  event handlers:
*
*/

static void
on_delete_event( cfg_edit_dlg* dlg, GdkEvent* e, gpointer* p )
{
    // printf( "cfg_edit_dlg::on_delete_event()\n" );

    // NOTE: free rdata
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_foreach( mod, &rm_rdata_func, dlg );

    gtk_widget_destroy( GTK_WIDGET( dlg ) );
}



static void
on_row_sel( GtkTreeView* tree, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    xxx_update_gui( dlg );
}



static void
on_btn_reload( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    xxx_reload( dlg );

    tree_set_focus( dlg );
    xxx_update_gui( dlg );

} // on_btn_reload()



static void
on_btn_tst( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    return;

//    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

//    GdkEvent* ev = gdk_event_new( GDK_KEY_PRESS );
//    ev->key.window = gtk_widget_get_window( GTK_WIDGET( dlg->tree_v_ ) );
//    ev->key.window = gtk_widget_get_window( GTK_WIDGET( dlg->tree_v_ ) );
//    ev->key.keyval = GDK_KEY_space;
//    ev->key.  is_modifier keyval = GDK_KEY_space;
//    ev->key.hardware_keycode = 32;
//    gdk_event_put( ev );

    // XXX: OK!
    //
//    gtk_test_widget_send_key( GTK_WIDGET( dlg->tree_v_ ),
//                              GDK_KEY_Return,
//                              (GdkModifierType) 0 );

//    gboolean res = gtk_widget_activate( GTK_WIDGET( dlg->tree_v_ ) );
//    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
//    printf( " >>  res(): [%d]\n", res ); // => false


    // EdaConfig* ctx_parent = eda_config_get_user_context();
    // EdaConfig* ctx        = eda_config_get_context_for_path( "." );
    // conf_reload_child_ctxs( ctx_parent, dlg );

    gchar* path = row_cur_pos_save( dlg );

    gui_update_off();

//    GtkAdjustment* adj = gtk_tree_view_get_vadjustment( dlg->tree_v_ );
//    gtk_tree_view_set_vadjustment( dlg->tree_v_, NULL );

    xxx_reload( dlg );
    // conf_reload_ctx( ctx, "3", dlg );

//    gtk_tree_view_set_vadjustment( dlg->tree_v_, adj );

    tree_set_focus( dlg );

    gui_update_on();

    row_cur_pos_restore( dlg, path );
    g_free( path );

    // char* path_str = row_cur_pos_save( dlg );
    // conf_reload_ctx_path( dlg );
    // row_cur_pos_restore( dlg, path_str );
    // row_cur_pos_restore( dlg, "0:1:0" );
    // GtkTreePath* path = gtk_tree_path_new_from_string( "0:1:0" );
    // gtk_tree_path_up( path );

} // on_btn_tst()



static void
on_btn_add( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    if ( rdata->rtype_ == RT_CTX )
    {
        on_mitem_ctx_add( NULL, dlg );
    }
    else
    if ( rdata->rtype_ == RT_GRP )
    {
        on_mitem_grp_add( NULL, dlg );
    }

} // on_btn_add()



static void
on_btn_edit( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

    xxx_chg_val( dlg, rdata, it, txt );

    g_free( txt );

} // on_btn_edit()



static void
on_btn_toggle( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    xxx_toggle( dlg );

}



static void
on_lab_fname( GtkLabel* lab, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    const gchar* fname = gtk_label_get_current_uri( lab );

    // external editor [read config]:
    //
    GError* err = NULL;
    gchar* exted = eda_config_get_string( eda_config_get_user_context(),
                                          "lepton-conf",
                                          "editor",
                                          &err );
    g_clear_error( &err );

    if (!exted)
        exted = g_strdup( g_exted_default );


    GAppInfo* ai =
    g_app_info_create_from_commandline( exted,
                                        NULL,
                                        G_APP_INFO_CREATE_NONE,
                                        &err );
    if ( err )
    {
        printf( " >> on_lab_fname(): GAppInfo err: [%d], msg: [%s]\n",
                err->code, err->message );
        g_clear_error( &err );
    }

    if ( ai )
    {
        GFile* gfile = g_file_new_for_path( fname );
        GList* args = g_list_append( NULL, gfile );

        if ( !g_app_info_launch( ai, args, NULL, &err ) )
        {
            printf( " >> on_lab_fname(): LAUNCH err: [%d], msg: [%s]\n",
                    err->code, err->message );

            GtkWidget* mdlg = gtk_message_dialog_new(
                NULL,
                // GTK_WINDOW( dlg ), // TODO: => SIGBUS
                (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Could not launch external editor [%s].\n"
                "Please set it via [lepton-conf]::editor config key."
                , exted );

            gtk_dialog_run( GTK_DIALOG( mdlg ) );
            gtk_widget_destroy( mdlg );
        }

        g_clear_error( &err );
        g_list_free( args );
    }


    // external editor [write config]: TODO: reload config here?
    //
    eda_config_set_string( eda_config_get_user_context(),
                           "lepton-conf",
                           "editor",
                           exted );

    eda_config_save( eda_config_get_user_context(), &err );
    g_clear_error( &err );

    g_free( exted );

} // on_lab_fname()



static void
on_btn_showinh( GtkToggleButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gboolean showinh = gtk_toggle_button_get_active( btn );

    // - about to hide all inherited rows
    // - current row is inherited
    // => change to nearest non-inherited row upwards in hierarchy:
    //
    if ( !showinh && rdata->inh_ )
    {
        row_select_non_inh( dlg, it );
    }

    xxx_showinh( dlg, showinh );


    // ensure that current node is visible after refiltering:
    //
    row_cur_get_iter( dlg, &it );

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    if ( path != NULL )
    {
        gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, TRUE, 0.5, 0 );
        gtk_tree_path_free( path );
    }

} // on_btn_showinh()



// key node: "edit" mitem
//
void
on_mitem_key_edit( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

    xxx_chg_val( dlg, rdata, it, txt );

    g_free( txt );

} // on_mitem_key_edit()



// group node: "add" mitem
//
void
on_mitem_grp_add( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    gchar* key = NULL;
    gchar* val = NULL;

    if ( !run_dlg_add_val( dlg, NULL, &key, &val ) )
        return;

    GtkTreePath* path1 = row_find_child_by_name( dlg, it, key );

    if ( path1 != NULL )
    {
        printf( "on_mitem_add(): [%s] EXISTS\n", key );

        row_select_by_path_mod( dlg, path1 );

        GtkTreeIter it_child;
        row_cur_get_iter( dlg, &it_child );

//        GtkTreeIter it_child;
//        GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
//        gtk_tree_model_get_iter( mod, &it_child, path1 );

        row_data* rdata_child = row_field_get_data( dlg, &it_child );


        if ( rdata_child->inh_ || g_strcmp0( val, rdata_child->val_ ) != 0 )
        {
            // NOTE: conf_chg_val() / conf_save()
            //
            conf_chg_val( rdata_child, val );

            if ( conf_save( rdata->ctx_, dlg ) )
            {
                row_field_set_val( dlg, it_child, val );

                // NOTE: unset inherited:
                //
                row_key_unset_inh( dlg, it_child );

                // NOTE: conf_reload_child_ctxs()
                //
                conf_reload_child_ctxs( rdata->ctx_, dlg );
            }
        }


        gtk_tree_path_free( path1 );
        g_free( key );
        g_free( val );

        return;

    } // if key already exists


    // NOTE: conf_add_val() / conf_save()
    //
    conf_add_val( rdata, key, val );
    if ( !conf_save( rdata->ctx_, dlg ) )
        return;

    printf( "on_mitem_add(): [%s] = [%s]\n", key, val );

    // NOTE: rdata:
    //
    row_data* rdata_new = mk_rdata( rdata->ctx_,
                                    rdata->group_,  // group
                                    key,            // key
                                    val,            // val
                                    FALSE,          // ro
                                    FALSE,          // inh
                                    RT_KEY          // rtype
                                  );

    GtkTreeIter it_grp_tstrore = row_get_tstore_iter( dlg, it );

    GtkTreeIter it_new = row_add( dlg,
                                  key,
                                  val,
                                  rdata_new,
                                  &it_grp_tstrore
                                );


    // NOTE: select newly added key node:
    //
    row_select_by_iter_tstore( dlg, it_new );

    // NOTE: unset inherited:
    //
    GtkTreeIter it_cur;
    if ( row_cur_get_iter( dlg, &it_cur ) )
        row_key_unset_inh( dlg, it_cur );

    // NOTE: conf_reload_child_ctxs()
    //
    conf_reload_child_ctxs( rdata->ctx_, dlg );


    g_free( key );
    g_free( val );

} // on_mitem_grp_add()



// context node: "add" mitem
//
void
on_mitem_ctx_add( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it_ctx;
    if ( !row_cur_get_iter( dlg, &it_ctx ) )
        return;

    row_data* rdata_ctx = row_field_get_data( dlg, &it_ctx );
    if ( !rdata_ctx )
        return;


    gchar* grp = NULL;
    gchar* key = NULL;
    gchar* val = NULL;

    if ( !run_dlg_add_val_2( dlg, NULL, &grp, &key, &val ) )
        return;

    printf( "on_mitem_ctx_add( %s ): [%s] [%s] [%s]\n",
            conf_ctx_name( rdata_ctx->ctx_ ), grp, key, val );

    GtkTreeIter it_grp;
    GtkTreeIter it_key;
    GtkTreePath* path_grp = NULL;
    GtkTreePath* path_key = NULL;

    path_grp = row_find_child_by_name( dlg, it_ctx, grp );
    if ( path_grp != NULL )
    {
        // XXX: group exists:

        printf( "on_mitem_ctx_add(): GRP [%s] EXISTS\n", grp );

        row_select_by_path_mod( dlg, path_grp );
        gtk_tree_path_free( path_grp );

        row_cur_get_iter( dlg, &it_grp );
        row_data* rdata_grp = row_field_get_data( dlg, &it_grp );

        path_key = row_find_child_by_name( dlg, it_grp, key );
        if ( path_key != NULL )
        {
            // XXX: group exists, key exists

            printf( "on_mitem_ctx_add(): KEY [%s] EXISTS\n", key );

            row_select_by_path_mod( dlg, path_key );
            gtk_tree_path_free( path_key );

            row_cur_get_iter( dlg, &it_key );
            row_data* rdata_key = row_field_get_data( dlg, &it_key );

            if ( rdata_key->inh_ || g_strcmp0( val, rdata_key->val_ ) != 0 )
            {
                // NOTE: conf_chg_val() / conf_save()
                //
                conf_chg_val( rdata_key, val );
                if ( conf_save( rdata_ctx->ctx_, dlg ) )
                {
                    row_field_set_val( dlg, it_key, val );
                    row_key_unset_inh( dlg, it_key );
                    conf_reload_child_ctxs( rdata_ctx->ctx_, dlg );
                }
            }

        }
        else
        {
            // XXX: group exists, key does not

            // NOTE: conf_add_val() / conf_save():
            //
            conf_add_val( rdata_grp, key, val );
            if ( !conf_save( rdata_grp->ctx_, dlg ) )
                return;

            // NOTE: rdata:
            //
            row_data* rdata_new_key = mk_rdata( rdata_grp->ctx_,
                                            rdata_grp->group_, // group
                                            key,               // key
                                            val,               // val
                                            FALSE,             // ro
                                            FALSE,             // inh
                                            RT_KEY             // rtype
                                          );

            GtkTreeIter it_grp_tstrore = row_get_tstore_iter( dlg, it_grp );

            GtkTreeIter it_new_key = row_add( dlg,
                                          key,
                                          val,
                                          rdata_new_key,
                                          &it_grp_tstrore
                                        );

            // NOTE: select newly added key node:
            //
            row_select_by_iter_tstore( dlg, it_new_key );

            // NOTE: unset inherited:
            //
            GtkTreeIter it_cur;
            row_cur_get_iter( dlg, &it_cur );

            row_key_unset_inh( dlg, it_cur );

            // NOTE: conf_reload_child_ctxs():
            //
            conf_reload_child_ctxs( rdata_new_key->ctx_, dlg );

        }

    }
    else
    {
        // XXX: group does not exist:

        // NOTE: rdata: new grp:
        //
        row_data* rdata_new_grp = mk_rdata( rdata_ctx->ctx_,
                                    grp,    // group
                                    NULL,   // key
                                    NULL,   // val
                                    FALSE,  // ro
                                    FALSE,  // inh
                                    RT_GRP  // rtype
                                  );


        // NOTE: conf_add_val() / conf_save():
        //
        conf_add_val( rdata_new_grp, key, val );
        if ( !conf_save( rdata_new_grp->ctx_, dlg ) )
        {
            rm_rdata( rdata_new_grp );
            return;
        }


        GtkTreeIter it_ctx_tstrore = row_get_tstore_iter( dlg, it_ctx );

        gchar* display_name = g_strdup_printf( "[%s]", grp );
        GtkTreeIter it_new_grp = row_add( dlg,
                                          display_name,  // name
                                          "",            // val
                                          rdata_new_grp, // rdata
                                          &it_ctx_tstrore
                                        );
        g_free( display_name );

        // NOTE: select newly added grp node:
        //
        row_select_by_iter_tstore( dlg, it_new_grp );


        // NOTE: rdata: new key:
        //
        row_data* rdata_new_key = mk_rdata( rdata_new_grp->ctx_,
                                            rdata_new_grp->group_, // group
                                            key,                   // key
                                            val,                   // val
                                            FALSE,                 // ro
                                            FALSE,                 // inh
                                            RT_KEY                 // rtype
                                          );

        GtkTreeIter it_new_key = row_add( dlg,
                                          key,
                                          val,
                                          rdata_new_key,
                                          &it_new_grp
                                        );

        // NOTE: select newly added key node:
        //
        row_select_by_iter_tstore( dlg, it_new_key );

        // NOTE: conf_reload_child_ctxs():
        //
        conf_reload_child_ctxs( rdata_new_key->ctx_, dlg );

    }

    g_free( grp );
    g_free( key );
    g_free( val );

} // on_mitem_ctx_add()



// handler for button-press-event signal:
//
static gboolean
on_mouse_click( GtkWidget* w, GdkEvent* e, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    GdkEventButton* ebtn = ( GdkEventButton* ) e;


    // LMB double click:
    //
    if ( ebtn->type == GDK_2BUTTON_PRESS && ebtn->button == 1 )
    {
        xxx_toggle( dlg );
        return FALSE;
    }


    // not an RMB => do not process event (return FALSE):
    //
    if ( ebtn->type != GDK_BUTTON_PRESS || ebtn->button != 3 )
        return FALSE;


    //
    // further down only return TRUE to not allow selecting rows with RMB
    //


    if ( ebtn->window != gtk_tree_view_get_bin_window( dlg->tree_v_ ) )
        return TRUE;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return TRUE;


    GtkTreePath* path_cur = NULL;
    path_cur = gtk_tree_model_get_path( gtk_tree_view_get_model( dlg->tree_v_ ), &it );

    GtkTreePath* path_rmb = NULL;
    gboolean onrow =
        gtk_tree_view_get_path_at_pos( dlg->tree_v_, ebtn->x, ebtn->y, &path_rmb,
                                       NULL, NULL, NULL );

    if ( onrow )
    {
        if ( gtk_tree_path_compare( path_cur, path_rmb ) != 0 )
            onrow = FALSE;
    }

    gtk_tree_path_free( path_cur );
    gtk_tree_path_free( path_rmb );

    if ( !onrow )
        return TRUE;


    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return TRUE;

    GtkMenu* menu = gui_mk_popup_menu( dlg, rdata );

    if ( menu )
    {
        gtk_menu_popup( menu, NULL, NULL, NULL, NULL,
                        ebtn->button, ebtn->time );
                        // 0, gtk_get_current_event_time() );
    }

    return TRUE;

} // on_rmb()



// handler for key-press-event signal:
// [e]: GdkEventKey
//
static gboolean
on_key_press( GtkWidget* w, GdkEvent* e, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    // printf( " >> on_key_press(): [0x%X]\n", e->key.keyval );

    // block Ctrl+F:
    //
    if ( e->key.keyval == GDK_KEY_f )
    {
        // printf( " ** ** on_key_press(): BLCOK 'F' KEY\n" );
        return TRUE;
    }

    // do not close dialog on 'Escape' key:
    //
    if ( e->key.keyval == GDK_KEY_Escape && !g_close_with_esc )
    {
        // printf( " ** ** on_key_press(): ESC KEY\n" );
        return TRUE;
    }

    return FALSE; // propagate event

} // on_key_press()




/* ******************************************************************
*
*  gui:
*
*/

//    static void
//    on_tree_search( GtkTreeView* treeview, gpointer p )
//    {
//        printf( " >>  on_tree_search()\n" );
//
//        cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
//        if ( !dlg )
//            return;
//
//        gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
//    }

// setup signal handlers for dialog:
//
static void
events_setup( cfg_edit_dlg* dlg )
{
    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( dlg->btn_showinh_ ),
                      "toggled",
                      G_CALLBACK( &on_btn_showinh ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-link",             // mouse click
                      G_CALLBACK( &on_lab_fname ),
                      dlg );
    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-current-link",     // press Enter key
                      G_CALLBACK( &on_lab_fname ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_reload_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_reload ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_tst_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_tst ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_add_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_add ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_edit_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_edit ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_toggle_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_toggle ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",         // tree sel changed
//                      "row-activated",
                      G_CALLBACK( &on_row_sel ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "button-press-event",
                      G_CALLBACK( &on_mouse_click ),
                      dlg );

//    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
//                      "start-interactive-search",
//                      G_CALLBACK( &on_tree_search ),
//                      dlg );


    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "key-press-event",
                      G_CALLBACK( &on_key_press ),
                      dlg );

} // events_setup()




/* ******************************************************************
*
*  TODO: refactoring:
*
*/

static void
xxx_chg_val( cfg_edit_dlg* dlg, const row_data* rdata, GtkTreeIter it, const gchar* txt )
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



static void
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
static void
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

