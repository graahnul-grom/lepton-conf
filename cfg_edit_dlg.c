/*
 * lepton-conf - Lepton EDA configuration utility.
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif

#include "cfg_edit_dlg.h"
#include "cfg_registry.h"

#include <liblepton/liblepton.h>




gboolean g_populate_default_ctx = TRUE;
static const gchar g_exted_default[] = "gvim";



static gboolean g_gui_update_enabled = TRUE;
static gboolean gui_update_enabled() { return g_gui_update_enabled;  }
static void     gui_update_on()      { g_gui_update_enabled = TRUE;  }
static void     gui_update_off()     { g_gui_update_enabled = FALSE; }



GtkWidget* cfg_edit_dlg_new()
{
  gpointer obj = g_object_new (CFG_EDIT_DLG_TYPE, NULL);
  return GTK_WIDGET (obj);
}




typedef enum
{
    RT_CTX,
    RT_GRP,
    RT_KEY,
}
RowType;

struct _row_data
{
    EdaConfig* ctx_;
    gchar*     group_;
    gchar*     key_;
    gchar*     val_;
    gboolean   ro_;   // read-only
    gboolean   inh_;  // inherited
    RowType    rtype_;
};

typedef struct _row_data row_data;


enum
{
    COL_NAME,
    COL_VAL,
    COL_DATA, // rdata: hidden
    NUM_COLS
};

static int tree_colid_name()  { return COL_NAME; }
static int tree_colid_val()   { return COL_VAL;  }
static int tree_colid_data()  { return COL_DATA; }
static int tree_cols_cnt()    { return NUM_COLS; }




/* ******************************************************************
*
*  forward declarations:
*
*/

static void
gui_mk_events( cfg_edit_dlg* dlg );

static void
gui_mk_labels_line( const gchar* left_txt,
                    GtkWidget*   right_label,
                    GtkWidget*   parent_box );

static void
gui_mk_labels_line_separ( GtkWidget* parent_box );

static GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg );

static GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg, const gchar* cwd );

static GtkWidget*
gui_mk_tree_view( cfg_edit_dlg* dlg, GtkTreeStore* store );

static void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd );

static void
tree_filter_setup( cfg_edit_dlg* p );

static row_data*
row_field_get_data( cfg_edit_dlg* dlg, GtkTreeIter* it );

static gchar*
row_cur_pos_save( cfg_edit_dlg* dlg );

static void
row_cur_pos_restore( cfg_edit_dlg* dlg, gchar* path_str );

static void
row_select_non_inh( cfg_edit_dlg* dlg, GtkTreeIter it );

static void
conf_add_val( row_data* rdata, const gchar* key, const gchar* val );

static void
conf_chg_val( const row_data* rdata, const gchar* txt );

static gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg );

static gboolean
conf_load_ctx( EdaConfig* ctx );

static GtkTreeIter
conf_mk_ctx_node( EdaConfig* ctx, gboolean wok, const gchar* name, cfg_edit_dlg* dlg );

static void
conf_reload_ctx( EdaConfig* ctx, const gchar* path, cfg_edit_dlg* dlg );

static void
conf_reload_ctx_user( cfg_edit_dlg* dlg );

static void
conf_reload_ctx_path( cfg_edit_dlg* dlg );

static void
conf_reload_child_ctxs( EdaConfig* parent_ctx, cfg_edit_dlg* dlg );

static void
conf_load( cfg_edit_dlg* dlg );

static const gchar*
conf_ctx_name( EdaConfig* ctx );

static const gchar*
conf_ctx_fname( EdaConfig* ctx, gboolean* exist, gboolean* rok, gboolean* wok );

static const gboolean
conf_ctx_file_writable( EdaConfig* ctx );

static GtkMenu*
mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata );

static gchar*
run_dlg_edit_val( cfg_edit_dlg* dlg, const gchar* txt, const gchar* title );

static gboolean
run_dlg_add_val( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** key,
                 gchar** val );

static gboolean
run_dlg_add_val_2( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** grp,
                 gchar** key,
                 gchar** val );

static void
on_mitem_ctx_add( GtkMenuItem* mitem, gpointer p );

static void
on_mitem_grp_add( GtkMenuItem* mitem, gpointer p );

static void
on_mitem_key_edit( GtkMenuItem* mitem, gpointer p );

static void
on_btn_showinh( GtkToggleButton* btn, gpointer* p );

static void
xxx_chg_val( cfg_edit_dlg* dlg, const row_data* rdata, GtkTreeIter it, const gchar* txt );

static void
xxx_toggle( cfg_edit_dlg* dlg );

static void
xxx_update_gui( cfg_edit_dlg* dlg );

static void
xxx_showinh( cfg_edit_dlg* dlg, gboolean show );

static void
xxx_reload( cfg_edit_dlg* dlg );




/* ******************************************************************
*
*  gobject stuff: define class:
*
*/
G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);




/* ******************************************************************
*
*  save/restore app settings:
*
*/

// "show" handler for dialog:
// NOTE: called *after* cfg_edit_dlg_init()
//
static void
settings_restore( GtkWidget* widget )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

    EdaConfig* ctx = eda_config_get_user_context();

    // show inherited:
    //
    gboolean showinh = eda_config_get_boolean(
        ctx,
        "lepton-conf",
        "lepton-conf-hidden-showinh", NULL );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  showinh );

    xxx_showinh( dlg, showinh );
    // was: on_btn_showinh( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ), (gpointer*) dlg );


    // geometry:
    //
    gint x = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-x", NULL );
    gint y = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-y", NULL );
    gtk_window_move( GTK_WINDOW( dlg ), x, y );

    gint width = eda_config_get_int(  ctx, "lepton-conf", "lepton-conf-hidden-width", NULL );
    gint height = eda_config_get_int( ctx, "lepton-conf", "lepton-conf-hidden-height", NULL );
    if ( width != 0 && height != 0 )
        gtk_window_resize( GTK_WINDOW( dlg ), width, height );


    // tree path:
    //
    GError* err = NULL;
    gchar* path = eda_config_get_string( eda_config_get_user_context(),
                                          "lepton-conf",
                                          "lepton-conf-hidden-path",
                                          &err );
    g_clear_error( &err );

    if ( path != NULL )
        row_cur_pos_restore( dlg, path );

    g_free( path );


    // NOTE: call parent gobject:
    //
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->show(widget);

} // settings_restore()



// "unmap" handler for dialog:
// called when dialog box is closed
//
static void
settings_save( GtkWidget* widget )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

    gint x = 0;
    gint y = 0;
    gtk_window_get_position( GTK_WINDOW( dlg ), &x, &y );

    gint width  = 0;
    gint height = 0;
    gtk_window_get_size( GTK_WINDOW( dlg ), &width, &height );


    EdaConfig* ctx = eda_config_get_user_context();

    // show inh bn state:
    //
    eda_config_set_boolean( ctx, "lepton-conf", "lepton-conf-hidden-showinh", dlg->showinh_ );


    // geometry:
    //
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-x", x );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-y", y );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-width",  width );
    eda_config_set_int( ctx, "lepton-conf", "lepton-conf-hidden-height", height );


    // tree path:
    //
    gchar* path = row_cur_pos_save( dlg );
    if ( path != NULL )
        eda_config_set_string( ctx, "lepton-conf", "lepton-conf-hidden-path", path );
    g_free( path );


    // save config:
    //
    eda_config_save( ctx, NULL );


    // NOTE: call parent gobject:
    //
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->unmap(widget);

} // settings_save()




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
cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
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
    wcls->show =  &settings_restore;
    wcls->unmap = &settings_save;
}



static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
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
    gui_mk_events( dlg );
    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

    xxx_update_gui( dlg );
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



static row_data*
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
static gboolean
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
static gchar*
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



static void
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
    gtk_tree_view_expand_to_path( dlg->tree_v_, p0 );
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



static row_data*
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



static GtkTreeIter
row_add( cfg_edit_dlg* dlg,
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

} // row_add()




/* ******************************************************************
*
*  tree:
*
*/

static void tree_cell_draw( GtkTreeViewColumn* col,
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



static void
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

    xxx_reload( dlg );

} // on_btn_reload()



static void
on_btn_tst( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    // EdaConfig* ctx_parent = eda_config_get_user_context();
    EdaConfig* ctx        = eda_config_get_context_for_path( "." );
    // conf_reload_child_ctxs( ctx_parent, dlg );

    gchar* path = row_cur_pos_save( dlg );

    gui_update_off();

//    GtkAdjustment* adj = gtk_tree_view_get_vadjustment( dlg->tree_v_ );
//    gtk_tree_view_set_vadjustment( dlg->tree_v_, NULL );

    xxx_reload( dlg );
    // conf_reload_ctx( ctx, "3", dlg );

//    gtk_tree_view_set_vadjustment( dlg->tree_v_, adj );

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

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
static void
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
static void
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
static void
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

    GtkMenu* menu = mk_popup_menu( dlg, rdata );

    if ( menu )
    {
        gtk_menu_popup( menu, NULL, NULL, NULL, NULL,
                        ebtn->button, ebtn->time );
                        // 0, gtk_get_current_event_time() );
    }

    return TRUE;

} // on_rmb()




/* ******************************************************************
*
*  popup menu:
*
*/

static GtkMenu*
mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata )
{
//    if ( rdata->rtype_ == RT_CTX )
//        return NULL;

    GtkWidget* menu = gtk_menu_new();


    if ( rdata->rtype_ == RT_CTX )
    {
        GtkWidget* mitem_ctx_add = NULL;
        mitem_ctx_add = gtk_menu_item_new_with_mnemonic( "_add" );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_ctx_add);
        g_signal_connect( G_OBJECT( mitem_ctx_add ),
                          "activate",
                          G_CALLBACK( &on_mitem_ctx_add ),
                          dlg );
        gtk_widget_show( mitem_ctx_add );
        gtk_widget_set_sensitive( mitem_ctx_add, !rdata->ro_ );
    }
    else
    if ( rdata->rtype_ == RT_KEY )
    {
        GtkWidget* mitem_key_edit = NULL;
        mitem_key_edit = gtk_menu_item_new_with_mnemonic( "_edit" );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_key_edit);
        g_signal_connect( G_OBJECT( mitem_key_edit ),
                          "activate",
                          G_CALLBACK( &on_mitem_key_edit ),
                          dlg );
        gtk_widget_show( mitem_key_edit );
        gtk_widget_set_sensitive( mitem_key_edit, !rdata->ro_ );
    }
    else
    if ( rdata->rtype_ == RT_GRP )
    {
        GtkWidget* mitem_grp_add = NULL;
        mitem_grp_add = gtk_menu_item_new_with_mnemonic( "_add" );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_grp_add);
        g_signal_connect( G_OBJECT( mitem_grp_add ),
                          "activate",
                          G_CALLBACK( &on_mitem_grp_add ),
                          dlg );
        gtk_widget_show( mitem_grp_add );
        gtk_widget_set_sensitive( mitem_grp_add, !rdata->ro_ );
    }


    return GTK_MENU( menu );

} // mk_popup_menu()




/* ******************************************************************
*
*  aux dialogs:
*
*/

// {post}: caller must g_free() {ret}
//
static gchar*
run_dlg_edit_val( cfg_edit_dlg* dlg, const gchar* txt, const gchar* title )
{
    GtkWidget* vdlg = gtk_dialog_new_with_buttons(
        title ? title : "Edit value:",
        GTK_WINDOW( dlg ),
        GTK_DIALOG_MODAL, // | GTK_DIALOG_DESTROY_WITH_PARENT,
//        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL );

    if ( title )
        gtk_window_set_title( GTK_WINDOW( vdlg ), title );

    GtkWidget* ent = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent ), txt );

    GtkWidget* vbox = gtk_vbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent, TRUE, TRUE, 5 );

    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( vdlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), vbox, TRUE, TRUE, 10 );


    gtk_dialog_set_alternative_button_order(GTK_DIALOG(vdlg),
                                            GTK_RESPONSE_ACCEPT,
                                            GTK_RESPONSE_REJECT,
                                            -1);

    gtk_dialog_set_default_response (GTK_DIALOG (vdlg),
                                     GTK_RESPONSE_ACCEPT);

    gtk_entry_set_activates_default( GTK_ENTRY( ent ), TRUE );


    gtk_widget_show_all( vdlg );

//    g_signal_connect( vdlg, "response", G_CALLBACK( &resp ), dlg );

    gtk_widget_set_size_request( vdlg, 300, -1 );



    gint res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  run_dlg_edit_val(): resp: %d\n", res );

    gchar* ret = NULL;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        ret = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent ) ) );
    }

    gtk_widget_destroy( vdlg );

    return ret;

} // run_dlg_edit_val()



// {post}: caller must g_free() [key], [val]
//
static gboolean
run_dlg_add_val( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** key,
                 gchar** val )
{
    GtkWidget* vdlg = gtk_dialog_new_with_buttons(
        title ? title : "Add key/value:",
        GTK_WINDOW( dlg ),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL );

    if ( title )
        gtk_window_set_title( GTK_WINDOW( vdlg ), title );

    GtkWidget* ent_key = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent_key ), "newKey" );

    GtkWidget* ent_val = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent_val ), "newValue" );

    GtkWidget* vbox = gtk_vbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_key, TRUE, TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_val, TRUE, TRUE, 5 );


    gtk_dialog_set_alternative_button_order(GTK_DIALOG(vdlg),
                                            GTK_RESPONSE_ACCEPT,
                                            GTK_RESPONSE_REJECT,
                                            -1);

    gtk_dialog_set_default_response (GTK_DIALOG (vdlg),
                                     GTK_RESPONSE_ACCEPT);

    gtk_entry_set_activates_default( GTK_ENTRY( ent_key ), TRUE );
    gtk_entry_set_activates_default( GTK_ENTRY( ent_val ), TRUE );


    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( vdlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), vbox, TRUE, TRUE, 10 );

    gtk_widget_show_all( vdlg );
    gtk_widget_set_size_request( vdlg, 300, -1 );
    gint res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  run_dlg_add_val(): resp: %d\n", res );

    gboolean ret = FALSE;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        *key = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_key ) ) );
        *val = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_val ) ) );
        ret = TRUE;
    }

    gtk_widget_destroy( vdlg );

    return ret;

} // run_dlg_add_val()



// {post}: caller must g_free() [grp], [key], [val]
//
static gboolean
run_dlg_add_val_2( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** grp,
                 gchar** key,
                 gchar** val )
{
    GtkWidget* vdlg = gtk_dialog_new_with_buttons(
        title ? title : "Add group/key/value:",
        GTK_WINDOW( dlg ),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL );

    GtkWidget* cb_grp = gtk_combo_box_text_new_with_entry();
//    GtkWidget* cb_grp = gtk_combo_box_new_with_entry();
    GtkWidget* ent_grp = gtk_bin_get_child( GTK_BIN( cb_grp ) );
    gtk_entry_set_text( GTK_ENTRY( ent_grp ), "newGroup" );


    // combo: add list of child groups:
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    GtkTreeIter it_parent;
    if ( !row_cur_get_iter( dlg, &it_parent ) )
        return FALSE;

    GtkTreeIter it_child;
    gboolean next = gtk_tree_model_iter_children( mod,
                                                  &it_child,
                                                  &it_parent );

    for ( ; next ; next = gtk_tree_model_iter_next( mod, &it_child ) )
    {
        row_data* rd = row_field_get_data( dlg, &it_child );
        if ( rd != NULL )
        {
            gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT( cb_grp ),
                                            rd->group_ );
        }
    }



    GtkWidget* ent_key = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent_key ), "newKey" );

    GtkWidget* ent_val = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY( ent_val ), "newValue" );

    GtkWidget* vbox = gtk_vbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), cb_grp,  TRUE, TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_key, TRUE, TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_val, TRUE, TRUE, 5 );


    gtk_dialog_set_alternative_button_order(GTK_DIALOG(vdlg),
                                            GTK_RESPONSE_ACCEPT,
                                            GTK_RESPONSE_REJECT,
                                            -1);

    gtk_dialog_set_default_response (GTK_DIALOG (vdlg),
                                     GTK_RESPONSE_ACCEPT);

    gtk_entry_set_activates_default( GTK_ENTRY( ent_grp ), TRUE );
    gtk_entry_set_activates_default( GTK_ENTRY( ent_key ), TRUE );
    gtk_entry_set_activates_default( GTK_ENTRY( ent_val ), TRUE );


    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( vdlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), vbox, TRUE, TRUE, 10 );

    gtk_widget_show_all( vdlg );
    gtk_widget_set_size_request( vdlg, 350, -1 );
    gint res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  run_dlg_add_val_2(): resp: %d\n", res );

    gboolean ret = FALSE;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        *grp = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_grp ) ) );
        *key = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_key ) ) );
        *val = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_val ) ) );
        ret = TRUE;
    }

    gtk_widget_destroy( vdlg );

    return ret;

} // run_dlg_add_val_2()




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

static gboolean
conf_is_hidden_key( const gchar* name )
{
    gboolean hidden = FALSE;
    hidden = strstr( name, "lepton-conf-hidden" ) != NULL;
    return hidden;
}



static gboolean
conf_is_hidden_group( const gchar* name )
{
    gboolean hidden = FALSE;
    hidden = strstr( name, "dialog-geometry" ) != NULL;
    return hidden;
}



static void
conf_load_keys( EdaConfig*    ctx,
                const gchar*  group,
                cfg_edit_dlg* dlg,
                GtkTreeIter   itParent,
                gboolean      wok,
                gboolean*     inh_all )
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
        row_data* rdata = mk_rdata( ctx,
                                    group,          // group
                                    name,           // key
                                    val,            // val
                                    !wok, // ro
                                    inh,            // inh
                                    RT_KEY          // rtype
                                  );

        row_add( dlg,
                 name,
                 val,
                 rdata,
                 &itParent
               );

        g_free( val );

    } // for keys

    g_strfreev( pp );

} // conf_load_keys()



static void
conf_load_groups( EdaConfig*    ctx,
                  gboolean      wok,
                  cfg_edit_dlg* dlg,
                  GtkTreeIter   itParent )
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
        row_data* rdata = mk_rdata( ctx,
                                    name,   // group
                                    NULL,   // key
                                    NULL,   // val
                                    !wok,   // ro
                                    FALSE,  // inh
                                    RT_GRP  // rtype
                                  );

        gchar* display_name = g_strdup_printf( "[%s]", name );

        GtkTreeIter it = row_add( dlg,
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

        conf_load_keys( ctx, name, dlg, it, wok, &inh_all );

        // mark group itself as inh if all children are inh:
        //
        rdata->inh_ = inh_all;

    } // for groups

    g_strfreev( pp );

} // conf_load_groups()



static gboolean
conf_load_ctx( EdaConfig* ctx )
{
    gboolean res = TRUE;

    const gchar* fname = conf_ctx_fname( ctx, NULL, NULL, NULL );

    if ( fname != NULL )
    {
        GError* err = NULL;

        res = eda_config_load( ctx, &err );

        if ( !res )
        {
            printf( "conf_load_ctx(): !eda_config_load( \"%s\" )\n", fname );

            if ( err != NULL )
            {
                printf( "    err msg: [%s]\n", err->message );
            }
        }

        g_clear_error( &err );
    }

    return res;

} // conf_load_ctx()



static GtkTreeIter
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
    row_data* rdata = mk_rdata( ctx,
                                NULL,  // group
                                NULL,  // key
                                NULL,  // val
                                !wok,  // ro
                                FALSE, // inh
                                RT_CTX // rtype
                              );

    GtkTreeIter
    it = row_add( dlg,
                   name,  // name
                   "",    // val
                   rdata, // rdata
                   NULL   // itParent
                 );

    return it;

} // conf_mk_ctx_node()



// reload groups and keys in [ctx]
//
// [path]: string:
//   "0" - ctx: default
//   "1" - ctx: system
//   "2" - ctx: user
//   "3" - ctx: path(.)
//
static void
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



    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );


    tree_filter_setup( dlg ); // NOTE: !!!

} // conf_reload_ctx()



static void
conf_reload_ctx_user( cfg_edit_dlg* dlg )
{
    printf( "-- -- RELOAD CTX: [user] -- --\n" );
    conf_reload_ctx( eda_config_get_user_context(), "2", dlg );
}



static void
conf_reload_ctx_path( cfg_edit_dlg* dlg )
{
    printf( "-- -- RELOAD CTX: [path] -- --\n" );
    conf_reload_ctx( eda_config_get_context_for_path( "." ), "3", dlg );
}



// reload all child contexts of [parent_ctx]
//
static void
conf_reload_child_ctxs( EdaConfig* parent_ctx, cfg_edit_dlg* dlg )
{
    gchar* path = row_cur_pos_save( dlg );


    gui_update_off();

    if ( parent_ctx == eda_config_get_system_context() )
    {
        conf_reload_ctx_user( dlg );
        conf_reload_ctx_path( dlg );
    }
    else
    if ( parent_ctx == eda_config_get_user_context() )
    {
        conf_reload_ctx_path( dlg );
    }

    gui_update_on();


    row_cur_pos_restore( dlg, path );
    g_free( path );

} // conf_reload_child_ctxs()



static void
conf_load( cfg_edit_dlg* dlg )
{
    EdaConfig* ctx_dflt = eda_config_get_default_context();
    EdaConfig* ctx_sys  = eda_config_get_system_context();
    EdaConfig* ctx_user = eda_config_get_user_context();
    EdaConfig* ctx_path = eda_config_get_context_for_path( "." );

    gchar* name_dflt = g_strdup_printf( "context: %s", conf_ctx_name( ctx_dflt ) );
    gchar* name_sys  = g_strdup_printf( "context: %s", conf_ctx_name( ctx_sys  ) );
    gchar* name_user = g_strdup_printf( "context: %s", conf_ctx_name( ctx_user ) );
    gchar* name_path = g_strdup_printf( "context: %s", conf_ctx_name( ctx_path ) );


    // load:
    //
    GtkTreeIter it;
    gboolean wok = FALSE;

    wok = conf_ctx_file_writable( ctx_dflt );
    it = conf_mk_ctx_node( ctx_dflt, wok, name_dflt, dlg );
    // conf_load_ctx( ctx_dflt );
    if ( g_populate_default_ctx )
    {
        cfgreg_populate_ctx( ctx_dflt );
    }
    conf_load_groups( ctx_dflt, wok, dlg, it );

    wok = conf_ctx_file_writable( ctx_sys );
    it = conf_mk_ctx_node( ctx_sys, wok, name_sys, dlg );
    conf_load_ctx( ctx_sys );
    conf_load_groups( ctx_sys, wok, dlg, it );

    wok = conf_ctx_file_writable( ctx_user );
    it = conf_mk_ctx_node( ctx_user, wok, name_user, dlg );
    conf_load_ctx( ctx_user );
    conf_load_groups( ctx_user, wok, dlg, it );

    wok = conf_ctx_file_writable( ctx_path );
    it = conf_mk_ctx_node( ctx_path, wok, name_path, dlg );
    conf_load_ctx( ctx_path );
    conf_load_groups( ctx_path, wok, dlg, it );


    g_free( name_dflt );
    g_free( name_sys );
    g_free( name_user );
    g_free( name_path );


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



static const gchar*
conf_ctx_name( EdaConfig* ctx )
{
    if ( ctx == eda_config_get_default_context() )
        return g_strdup( "DEFAULT" );
    if ( ctx == eda_config_get_system_context() )
        return g_strdup( "SYSTEM" );
    if ( ctx == eda_config_get_user_context() )
        return g_strdup( "USER" );
    if ( ctx == eda_config_get_context_for_path( "." ) )
        return g_strdup( "PATH (.)" );

    return g_strdup( "unknown" );
}



// {post}: {ret} owned by geda cfg api
//
static const gchar*
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



static const gboolean
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



static void
conf_add_val( row_data* rdata, const gchar* key, const gchar* val )
{
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           key,
                           val );
}








static void
conf_chg_val( const row_data* rdata, const gchar* txt )
{
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           rdata->key_,
                           txt );
}



static gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg )
{
    GError* err = NULL;

    gboolean res = eda_config_save( ctx, &err );

    if ( !res )
    {
        printf( " >> conf_save(): !eda_config_save(): [%s]\n",
                err ? err->message : "" );

        GtkWidget* msgdlg =
        gtk_message_dialog_new( GTK_WINDOW( dlg ),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "!eda_config_save()\nerrmsg: [%s]",
                                err ? err->message : "" );

        gtk_dialog_run( GTK_DIALOG( msgdlg ) );
        gtk_widget_destroy( msgdlg );

        g_clear_error( &err );
    }

    return res;

} // conf_save()




/* ******************************************************************
*
*  gui:
*
*/

// setup signal handlers for dialog:
//
static void
gui_mk_events( cfg_edit_dlg* dlg )
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

} // gui_mk_events()



static void
gui_mk_labels_line( const gchar* left_txt,
                    GtkWidget*   right_label,
                    GtkWidget*   parent_box )
{
    GtkWidget* hbox = gtk_hbox_new( FALSE, 0 );

    GtkWidget* left_label = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( left_label ), left_txt );
    gtk_label_set_selectable( GTK_LABEL( right_label ), TRUE );

    gtk_box_pack_start( GTK_BOX( hbox ), left_label, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), right_label, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( parent_box ), hbox, FALSE, FALSE, 0 );
}



static void
gui_mk_labels_line_separ( GtkWidget* parent_box )
{
    gtk_box_pack_start( GTK_BOX( parent_box ),
                        gtk_hseparator_new(), FALSE, FALSE, 0 );
}



static GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg )
{
    GtkWidget* box_tbar  = gtk_hbox_new( FALSE, 20 );

    // boxes:
    //
    GtkWidget* box1 = gtk_hbox_new( FALSE, 3 );
    GtkWidget* box2 = gtk_hbox_new( FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( box_tbar ), box2, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_tbar ), box1, FALSE, FALSE, 0 );


    // create controls:
    //
    dlg->btn_showinh_ = gtk_check_button_new_with_mnemonic( "" );
    GtkWidget* lab_showinh = gtk_bin_get_child( GTK_BIN( dlg->btn_showinh_ ) );
    gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab_showinh ),
                                        "<i>sho_w inherited</i>" );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  dlg->showinh_ );

    dlg->btn_reload_ = gtk_button_new_with_mnemonic( "_Reload" );
    dlg->btn_add_    = gtk_button_new_with_mnemonic( "_Add" );
    dlg->btn_edit_   = gtk_button_new_with_mnemonic( "_Edit" );
    dlg->btn_toggle_ = gtk_button_new_with_mnemonic( "_Toggle" );
    dlg->btn_tst_    = gtk_button_new_with_mnemonic( "t_st" );


    // images:
    //
    GtkWidget* img_add    = gtk_image_new_from_stock( GTK_STOCK_ADD,         GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_reload = gtk_image_new_from_stock( GTK_STOCK_REFRESH,     GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_edit   = gtk_image_new_from_stock( GTK_STOCK_EDIT,        GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_toggle = gtk_image_new_from_stock( GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_LARGE_TOOLBAR );

    gtk_button_set_image( GTK_BUTTON( dlg->btn_add_ ),    img_add );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_reload_ ), img_reload );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_edit_ ),   img_edit );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_toggle_ ), img_toggle );


    // tooltips:
    //
    gtk_widget_set_tooltip_text( dlg->btn_reload_, "Reload configuration from disk." );
    gtk_widget_set_tooltip_text( dlg->btn_add_, "Add new configuration key." );
    gtk_widget_set_tooltip_text( dlg->btn_edit_, "Edit current value." );
    gtk_widget_set_tooltip_text( dlg->btn_toggle_,
                                 "Toggle values like true/false, enabled/disabled.\n"
                                 "The same can be done with double-click."
    );


    // pack controls:
    //
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_reload_, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_add_,    FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_edit_,   FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_toggle_, FALSE, FALSE, 0 );
    //
    // TESTING:
    //
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_tst_,    FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( box2 ), dlg->btn_showinh_, FALSE, FALSE, 0 );

    return box_tbar;

} // gui_mk_toolbar()



static GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg, const gchar* cwd )
{
    GtkWidget* box_bot = gtk_vbox_new( FALSE, 5 );

    // cwd label:
    //
    gui_mk_labels_line( "<b>working directory: </b>", gtk_label_new( cwd ), box_bot );

    gui_mk_labels_line_separ( box_bot );

    dlg->lab_ctx_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>config context: </b>", dlg->lab_ctx_, box_bot );

    dlg->lab_fname_ = gtk_label_new( NULL );
    gtk_label_set_track_visited_links( GTK_LABEL( dlg->lab_fname_ ), FALSE );
    gui_mk_labels_line( "<b>config file: </b>", dlg->lab_fname_, box_bot );

    gui_mk_labels_line_separ( box_bot );

    dlg->lab_grp_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>group: </b>", dlg->lab_grp_, box_bot );

    dlg->lab_name_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>key: </b>", dlg->lab_name_, box_bot );

    dlg->lab_val_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>value: </b>", dlg->lab_val_, box_bot );

    gui_mk_labels_line_separ( box_bot );


    // description text view:
    //
    GtkWidget* tv = gtk_text_view_new();

    gtk_text_view_set_editable( GTK_TEXT_VIEW( tv ), FALSE );
    gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( tv ), GTK_WRAP_WORD );

    dlg->txtbuf_desc_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( tv ) );
    gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );

    // scrolled window for description text view:
    //
    GtkWidget* wscroll_desc = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_desc ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );

    gtk_container_add( GTK_CONTAINER( wscroll_desc ), tv );
    gtk_box_pack_start( GTK_BOX( box_bot ), wscroll_desc, FALSE, FALSE, 0 );

    return box_bot;

} // gui_mk_bottom_box()



// {ret}: scrolled window with tree view
//
static GtkWidget*
gui_mk_tree_view( cfg_edit_dlg* dlg, GtkTreeStore* store )
{
    GtkTreeModel* tree_model = GTK_TREE_MODEL( store );
    GtkWidget*    tree_widget = gtk_tree_view_new_with_model( tree_model );
    dlg->tree_v_ = GTK_TREE_VIEW( tree_widget );

    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );

    // columns:
    //
    dlg->ren_txt_ = gtk_cell_renderer_text_new();

    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_name(), "name" );
    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_val(),  "value" );

    // scrolled window for tree view:
    //
    GtkWidget* wscroll_tree = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_tree ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll_tree ), GTK_WIDGET( dlg->tree_v_ ) );

    return wscroll_tree;

} // gui_mk_tree_view()



static void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd )
{
    // window's title:
    //
    gtk_window_set_title( GTK_WINDOW( dlg ),
                          g_strdup_printf( "%s - lepton-conf", cwd ) );


    // tree store:
    //
    dlg->store_ = gtk_tree_store_new(
        tree_cols_cnt(),
          G_TYPE_STRING   // name
        , G_TYPE_STRING   // val
        , G_TYPE_POINTER  // rdata
    );


    // toolbar:
    //
    GtkWidget* toolbar = gui_mk_toolbar( dlg );

    // tree view:
    //
    GtkWidget* wscroll_tree = gui_mk_tree_view( dlg, dlg->store_ );

    // bottom box:
    //
    GtkWidget* box_bot = gui_mk_bottom_box( dlg, cwd );


    // pack to content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( dlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), toolbar, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll_tree, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), box_bot, FALSE, FALSE, 0 );

} // mk_gui()







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
//
static void
xxx_update_gui( cfg_edit_dlg* dlg )
{
    printf( " >> xxx_update_gui(): enabled: [%d]\n", gui_update_enabled() );
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



static void
xxx_showinh( cfg_edit_dlg* dlg, gboolean show )
{
    dlg->showinh_ = show;

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( mod ) );
    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

}



static void
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

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

    row_cur_pos_restore( dlg, path );
    g_free( path );

    gui_update_on();

} // xxx_reload()

