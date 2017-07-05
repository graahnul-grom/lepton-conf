#include "cfg_edit_dlg.h"
#include <liblepton/liblepton.h>
#include <liblepton/libgedaguile.h>


static GtkTreeModel*
dlg_model( cfg_edit_dlg* dlg )
{
//    return gtk_tree_view_get_model( dlg->tree_v_ );
    return dlg->model_;
}

static void
dlg_model_set( cfg_edit_dlg* dlg, GtkTreeModel* model )
{
    dlg->model_ = model;
}

static void
dlg_model_upd( cfg_edit_dlg* dlg )
{
    dlg_model_set( dlg, gtk_tree_view_get_model( dlg->tree_v_ ) );
}

// {ret}: tree store iter corresponding to model's iter [it]
//
GtkTreeIter dlg_tstore_iter( cfg_edit_dlg* dlg, GtkTreeIter it )
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
    GtkTreeModel* model = dlg_model( dlg );
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
}



// {post}: {ret} owned by geda cfg api
//
static const gchar*
ctx_get_fname( EdaConfig* ctx, gboolean* exist, gboolean* rok, gboolean* wok )
{
    const gchar* fname = eda_config_get_filename( ctx );

    if ( !fname )
        return NULL;

    *exist = access( fname, F_OK ) == 0;
    *rok =   access( fname, R_OK ) == 0;
    *wok =   access( fname, W_OK ) == 0;

    return fname;
}



//typedef enum
//{
//    RT_CTX,
//    RT_GRP,
//    RT_KEY
//} row_type;

struct _row_data
{
    EdaConfig*   ctx_;
    const gchar* group_;
    const gchar* key_;
    const gchar* val_;
    gboolean     ro_;  // read-only
    gboolean     inh_; // inherited
};

typedef struct _row_data row_data;

enum
{
    COL_NAME,
    COL_INH,
    COL_VAL,
    COL_DATA,     // hidden
    NUM_COLS
};



// TODO: row_data: free memory
//
static row_data*
mk_rdata( EdaConfig*   ctx,
         const gchar* group,
         const gchar* key,
         const gchar* val,
         gboolean     ro,
         gboolean     inh )
{
    row_data* rdata = g_malloc( sizeof( row_data ) );

    rdata->ctx_   = ctx;
    rdata->group_ = group ? g_strdup( group ) : NULL;
    rdata->key_   = key   ? g_strdup( key )   : NULL;
    rdata->val_   = val   ? g_strdup( val )   : NULL;
    rdata->ro_    = ro;
    rdata->inh_   = inh;

    return rdata;
}



static void load_cfg( cfg_edit_dlg* dlg );

static int colid_name()     { return COL_NAME; }
static int colid_inh()      { return COL_INH; }
static int colid_val()      { return COL_VAL; }
static int colid_data()     { return COL_DATA; }
static int cols_cnt()       { return NUM_COLS; }

// {ret}: what is to be displayed in "name" tree column
//
//static const gchar*
//data_name_field( const row_data* data )
//{
//    row_type type = data->type_;
//    if ( type == RT_CTX )
//    {
//    }
//}



// {post}: caller must free [name], [val]
//
//static gboolean
//cur_row_get_fields( cfg_edit_dlg* dlg,
//                    gchar**       name,
//                    gchar**       val,
//                    gboolean*     editable,
//                    row_data**    rdata )
//{
//    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
//    GtkTreeIter it;
//    gboolean res = gtk_tree_selection_get_selected( sel, NULL, &it );
//    if ( !res )
//    {
//        printf( " >> >> cur_row_get_fields(): !sel\n");
//        return FALSE;
//    }
////    GtkTreeModel* model = gtk_tree_view_get_model( tree );
//
//    gchar* n = NULL;
//    gtk_tree_model_get( dlg->model_, &it, colid_name(), &n, -1 );
//    *name = n;
//
//    gchar* v = NULL;
//    gtk_tree_model_get( dlg->model_, &it, colid_val(), &v, -1 );
//    *val = v;
//
//    row_data* ptr = NULL;
//    gtk_tree_model_get( dlg->model_, &it, colid_data(), &ptr, -1 );
//
//    *editable = ptr ? !ptr->ro_ : FALSE;
//
//    *rdata = ptr;
//
//    return TRUE;
//
//} // cur_row_get_fields()


// {ret}: iterator of currently selected row
//
static gboolean
cur_row_get_iter( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );


    GtkTreeModel* model = NULL;
    gboolean res = gtk_tree_selection_get_selected( sel, &model, it );
    dlg_model_set( dlg, model );


//    gboolean res = gtk_tree_selection_get_selected( sel, NULL, it );
    if ( !res )
        printf( " >> >> cur_row_get_iter(): !sel\n");

    return res;

} // cur_row_get_iter()



// {ret}: iterator of currently selected row
//
static gboolean
cur_row_get_parent_iter( cfg_edit_dlg* dlg, GtkTreeIter* it, GtkTreeIter* itParent )
{
    GtkTreeModel* model = dlg_model( dlg );
    GtkTreePath* path = gtk_tree_model_get_path( model, it );
    gtk_tree_path_up( path );
    return gtk_tree_model_get_iter( model, itParent, path );

} // cur_row_get_parent_iter()



static row_data*
row_get_field_data( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    row_data* rdata = NULL;
    gtk_tree_model_get( dlg_model( dlg ), it, colid_data(), &rdata, -1 );

    return rdata;

} // cur_row_get_field_data()



// {post}: caller must free {ret}
//
static gchar*
row_get_field_val( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    gchar* val = NULL;
    gtk_tree_model_get( dlg_model( dlg ), it, colid_val(), &val, -1 );

    return val;

} // cur_row_get_field_val()



static void
row_set_field_val( cfg_edit_dlg* dlg, GtkTreeIter itCPY, const gchar* val )
{
    GtkTreeIter it = itCPY;

    row_data* rdata = row_get_field_data( dlg, &it );
    if ( !rdata )
        return;

    // TODO: free val_
    //
    rdata->val_ = g_strdup( val );


    GtkTreeIter itStore = dlg_tstore_iter( dlg, it );

    gtk_tree_store_set( dlg->store_,
                        &itStore,
                        colid_val(), val,
                        -1 );

    dlg_model_upd( dlg );

} // row_set_field_val()



static gboolean
row_get_field_inh( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    row_data* rdata = row_get_field_data( dlg, it );
    if ( !rdata )
        return FALSE;

    return rdata->inh_;

} // row_get_field_inh()



static void
row_set_field_inh( cfg_edit_dlg* dlg, GtkTreeIter itCPY, gboolean val )
{
    GtkTreeIter it = itCPY;

    row_data* rdata = row_get_field_data( dlg, &it );
    if ( !rdata )
        return;

    rdata->inh_ = val;


    GtkTreeIter itStore = dlg_tstore_iter( dlg, it );

    gtk_tree_store_set( dlg->store_,
                        &itStore,
                        colid_inh(), val,
                        -1 );

    dlg_model_upd( dlg );

} // row_set_field_inh()



static gboolean
row_is_editable( cfg_edit_dlg* dlg, GtkTreeIter* it )
{
    row_data* rdata = row_get_field_data( dlg, it );

    return rdata ? !rdata->ro_ : FALSE;

} // cur_row_is_editable()




static void cell_draw( GtkTreeViewColumn* col,
                       GtkCellRenderer*   ren,
                       GtkTreeModel*      model,
                       GtkTreeIter*       it,
                       gpointer           p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    dlg_model_set( dlg, model );

    if ( ren != dlg->ren_txt_ )
        return;

    gboolean inh = row_get_field_inh( dlg, it );

    if ( inh )
        g_object_set( ren, "foreground", "gray", NULL );
    else
        g_object_set( ren, "foreground", "black", NULL );
}





static gboolean
filter( GtkTreeModel* model, GtkTreeIter* it, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    dlg_model_set( dlg, model );

//    row_data* rdata = NULL;
    // OK: gtk_tree_model_get( model, it, colid_data(), &rdata, -1 );
//    gtk_tree_model_get( dlg_model( dlg ), it, colid_data(), &rdata, -1 );

    const row_data* rdata = row_get_field_data( dlg, it );
    if ( !rdata )
        return FALSE;

    if ( !dlg->showinh_ )
        return !rdata->inh_;
//    return FALSE;
    return TRUE;

} // filter()



static void
filter_setup( cfg_edit_dlg* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeModel* modf = gtk_tree_model_filter_new( dlg_model( dlg ), NULL );

    gtk_tree_model_filter_set_visible_func(
        GTK_TREE_MODEL_FILTER( modf ),
        &filter,
        dlg,
        NULL);

    gtk_tree_view_set_model( dlg->tree_v_, modf );

    dlg_model_upd( dlg );

} // filter_setup()



static void
filter_remove( cfg_edit_dlg* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gtk_tree_view_set_model( dlg->tree_v_, GTK_TREE_MODEL( dlg->store_ ) );

    dlg_model_upd( dlg );

} // filter_remove()



static void
add_col( cfg_edit_dlg*    dlg,
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
                                             &cell_draw,
                                             dlg,
                                             NULL );

    gtk_tree_view_append_column( dlg->tree_v_, col );
}



static GtkTreeIter
add_row( cfg_edit_dlg* dlg,
         const gchar*  name,
         gboolean      inh,
         const gchar*  val,
         gpointer      rdata,
         GtkTreeIter*  itParent )
{
    GtkTreeIter it;
    gtk_tree_store_append( dlg->store_, &it, itParent );
    gtk_tree_store_set( dlg->store_,
                        &it,
                        colid_name(),     name,
                        colid_inh(),      inh,
                        colid_val(),      val,
                        colid_data(),     rdata,
                        -1 );

    dlg_model_upd( dlg );

    return it;
}






static void
get_property( GObject* obj, guint id, GValue* val, GParamSpec* spec );

static void
set_property( GObject* obj, guint id, const GValue* val, GParamSpec* spec );

static void
dispose( GObject* obj );



G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);
// G_DEFINE_TYPE (GschemObjectPropertiesWidget, gschem_object_properties_widget, GSCHEM_TYPE_BIN);






static void
cfg_edit_dlg_on_delete_event( GtkWidget* dlg, GdkEvent* e, gpointer* p )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_on_delete_event()\n" );
    gtk_widget_destroy( dlg );
}



static void
cfg_edit_dlg_on_btn_apply( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkEntry* ent = GTK_ENTRY( dlg->ent_ );
    if ( !ent )
        return;


    GtkTreeIter it;
    if ( !cur_row_get_iter( dlg, &it ) )
        return;


    row_data* rdata = row_get_field_data( dlg, &it );
    if ( !rdata || rdata->ro_ )
        return;


    const gchar* txt = gtk_entry_get_text( ent );

    // noop:
    //
    if ( g_strcmp0( rdata->val_, txt ) == 0 )
    {
        printf( " >> on_btn_apply(): NOOP\n" );
        return;
    }

    printf( " >> on_btn_apply(): [%s::%s]: [%s] => [%s]\n",
            rdata->group_, rdata->key_, rdata->val_, txt );

    // set:
    //
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           rdata->key_,
                           txt );

    // save:
    //
    GError* err = NULL;
    gboolean res = eda_config_save( rdata->ctx_, &err );
    if ( !res )
    {
        printf( " >> on_btn_apply(): !eda_config_save()\n" );
        if ( err != NULL )
            printf( "    err: %s\n", err->message );
        g_clear_error( &err );
        return;
    }

    // get:
    //
    gchar* new_val = eda_config_get_string( rdata->ctx_,
                                            rdata->group_,
                                            rdata->key_,
                                            &err );
    if ( new_val == NULL )
    {
        printf( " >> on_btn_apply(): !eda_config_get_string()\n" );
        if ( err != NULL )
            printf( "    err: %s\n", err->message );
        g_clear_error( &err );
        return;
    }



    row_set_field_val( dlg, it, new_val );


    // mark current key as not inherited:
    //
    row_set_field_inh( dlg, it, FALSE );


    // mark parent group as not inherited:
    //
    GtkTreeIter itParent;
    if ( cur_row_get_parent_iter( dlg, &it, &itParent ) )
        row_set_field_inh( dlg, itParent, FALSE );

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

    // NOTE: reload all:
    //
    g_signal_emit_by_name( dlg->btn_reload_, "clicked", dlg );

} // cfg_edit_dlg_on_btn_apply()



static void
cfg_edit_dlg_on_row_sel( GtkTreeView* tree,
                         gpointer*    p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkEntry* ent = GTK_ENTRY( dlg->ent_ );
    if ( !ent )
        return;


    GtkTreeIter it;
    cur_row_get_iter( dlg, &it );


    const row_data* rdata = row_get_field_data( dlg, &it );
    if ( !rdata || rdata->ro_ )
    {
        gtk_entry_set_text( ent, "" );
        return;
    }

    gchar* val = row_get_field_val( dlg, &it );
    gtk_entry_set_text( ent, val );
    g_free( val );

    gboolean editable = row_is_editable( dlg, &it );
    gtk_editable_set_editable( GTK_EDITABLE( ent ), editable );
    gtk_widget_set_sensitive( dlg->btn_apply_, editable );

} // cfg_edit_dlg_on_row_sel()



static void
cfg_edit_dlg_on_btn_reload( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;


    // remember current tree node:
    //
    GtkTreePath* path = NULL;
    GtkTreeIter it;
    if ( cur_row_get_iter( dlg, &it ) )
        path = gtk_tree_model_get_path( dlg_model( dlg ), &it );


    filter_remove( dlg );

    gtk_tree_store_clear( dlg->store_ );

    load_cfg( dlg );

    filter_setup( dlg );


    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
//    gtk_widget_activate( GTK_WIDGET( dlg->tree_v_ ) );


    // restore current tree node:
    //
    if ( path )
    {
        gtk_tree_view_expand_to_path( dlg->tree_v_, path );
        gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path, NULL, NULL, FALSE );
    }


    gtk_tree_view_expand_all( dlg->tree_v_ ); // // //

}



static void
cfg_edit_dlg_on_btn_exted( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;


    GtkTreeIter it;
    if ( !cur_row_get_iter( dlg, &it ) )
        return;


    row_data* rdata = row_get_field_data( dlg, &it );
    if ( !rdata )
        return;

    gboolean exist = FALSE;
    gboolean rok   = FALSE;
    gboolean wok   = FALSE;

    const gchar* fname = ctx_get_fname( rdata->ctx_, &exist, &rok, &wok );
    if ( !fname )
        return;

    GError* err = NULL;
    GAppInfo* ai =
    g_app_info_create_from_commandline( "gvim",
                                        NULL,
                                        G_APP_INFO_CREATE_NONE,
                                        &err );
    if ( ai )
    {
        GFile* gfile = g_file_new_for_path( fname );
        GList* args = g_list_append( NULL, gfile );

        g_app_info_launch( ai, args, NULL, &err );

        g_list_free( args );
    }

} // cfg_edit_dlg_on_btn_exted()



static void
cfg_edit_dlg_on_btn_showinh( GtkToggleButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gboolean show = gtk_toggle_button_get_active( btn );
    printf( " >> cfg_edit_dlg_on_btn_showinh(): %d\n", show );

    if ( !show )
    {
//        gtk_tree_view_set_model( dlg->tree_v_, dlg->model_f_ );
//        gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( dlg->model_f_ ) );

    }
    else
    {
//        gtk_tree_view_set_model( dlg->tree_v_, dlg_model( dlg ) );
    }

    dlg->showinh_ = show;
    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( dlg_model( dlg ) ) );

    gtk_tree_view_expand_all( dlg->tree_v_ );

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
}





/*
 * *****************************************************************
 *  gobject-specific stuff:
 * *****************************************************************
 */

static void
get_property( GObject* obj, guint id, GValue* val, GParamSpec* spec )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );

    if ( id == CFG_EDIT_DLG_PROPID_PROP1 )
    {
        g_value_set_int( val, dlg->prop1_ );
    }
    else
    {
        G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
    }
}



static void
set_property( GObject* obj, guint id, const GValue* val, GParamSpec* spec )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );

    if ( id == CFG_EDIT_DLG_PROPID_PROP1 )
    {
        dlg->prop1_ = g_value_get_int( val );
    }
    else
    {
        G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
    }
}



static void
dispose( GObject* obj )
{
//    printf( "cfg_edit_dlg::dispose( %p ); refcnt: %d\n",
//        obj, obj ? obj->ref_count : 0 );

    cfg_edit_dlgClass* cls = CFG_EDIT_DLG_GET_CLASS( obj );

    GObjectClass* parent_cls = g_type_class_peek_parent( cls );
    parent_cls->dispose( obj );

    printf( "cfg_edit_dlg::dispose(): done.\n" );
}



static void
cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_class_init()\n" );

    GObjectClass* gcls = G_OBJECT_CLASS( cls );

    gcls->dispose      = &dispose;
    gcls->get_property = &get_property;
    gcls->set_property = &set_property;

    GParamSpec* spec = g_param_spec_int( "prop1",
        "",  // nick
        "",  // blurb
        0,   // min
        10,  // max
        7,   // default
        G_PARAM_READABLE | G_PARAM_WRITABLE );

    g_object_class_install_property( gcls, CFG_EDIT_DLG_PROPID_PROP1, spec );
}



static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_init()\n" );

    dlg->prop1_ = 5;



    // store:
    //
    dlg->store_ = gtk_tree_store_new(
        cols_cnt(),
          G_TYPE_STRING     // name
        , G_TYPE_BOOLEAN  // inherited
        , G_TYPE_STRING   // val
        , G_TYPE_POINTER  // rdata
    );

    dlg_model_set( dlg, GTK_TREE_MODEL(dlg->store_) );
//    dlg->model_ = GTK_TREE_MODEL( dlg->store_ );


    // view:
    //
    dlg->tree_w_ = gtk_tree_view_new_with_model( dlg_model( dlg ) );
    dlg->tree_v_ = GTK_TREE_VIEW( dlg->tree_w_ );
    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );


    // tree view columns:
    //
//    GtkCellRenderer* ren_text = gtk_cell_renderer_text_new();
    dlg->ren_txt_ = gtk_cell_renderer_text_new();
    GtkCellRenderer* ren_bool = gtk_cell_renderer_toggle_new();

//    g_object_set( dlg->ren_txt_, "foreground", "gray", NULL );

    add_col( dlg, dlg->ren_txt_, "text",   colid_name(), "name" );
    add_col( dlg, ren_bool,      "active", colid_inh(),  "inherited" );
    add_col( dlg, dlg->ren_txt_, "text",   colid_val(),  "value" );


    dlg->showinh_ = TRUE;

//    filter_setup( dlg );
    load_cfg( dlg );
    filter_setup( dlg );


//    gtk_tree_view_collapse_all( dlg->tree_v_ );
//    GtkTreePath* path0 = gtk_tree_path_new_from_string( "2" );
//    if ( path0 )
//        gtk_tree_view_expand_row( dlg->tree_v_, path0, FALSE );
//    GtkTreePath* path1 = gtk_tree_path_new_from_string( "3" );
//    if ( path1 )
//        gtk_tree_view_expand_row( dlg->tree_v_, path1, FALSE );

    gtk_tree_view_expand_all( dlg->tree_v_ );


    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG(dlg) );

    // label:
    //
    gchar* cwd = g_get_current_dir();
    gchar str[ PATH_MAX ] = "";
    sprintf( str, "cwd: %s", cwd );
    GtkWidget* lab = gtk_label_new( str );
    g_free( cwd );
    gtk_box_pack_start( GTK_BOX( ca ), lab, FALSE, TRUE, 0 );

    // scrolled win:
    //
    GtkWidget* wscroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll ), dlg->tree_w_ );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll, TRUE, TRUE, 0 );


    // box:
    //
    GtkWidget* box = gtk_hbox_new( FALSE, 0 );


    // show inh chkeck box:
    //
    GtkWidget* btn_showinh = gtk_check_button_new_with_mnemonic( "sho_w inh" );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( btn_showinh ),
                                  dlg->showinh_ );
    gtk_box_pack_start( GTK_BOX( box ), btn_showinh, TRUE, TRUE, 10 );


    // edit val field:
    //
    dlg->ent_ = gtk_entry_new();
    gtk_box_pack_start( GTK_BOX( box ), dlg->ent_, TRUE, TRUE, 10 );


    // apply btn:
    //
    dlg->btn_apply_ = gtk_button_new_with_mnemonic( "_apply" );
    gtk_box_pack_start( GTK_BOX( box ), dlg->btn_apply_, FALSE, FALSE, 10 );

    gtk_box_pack_start( GTK_BOX( ca ),  box, FALSE, FALSE, 0 );



    // action area:
    //
    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG(dlg) );

    // reload btn:
    //
    dlg->btn_reload_ = gtk_button_new_with_mnemonic( "_reload" );
    gtk_box_pack_start( GTK_BOX( aa ), dlg->btn_reload_, FALSE, FALSE, 0 );

    // ext ed btn:
    //
    GtkWidget* btn_exted = gtk_button_new_with_mnemonic( "_ext ed" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_exted, FALSE, FALSE, 0 );


    gtk_widget_show_all( GTK_WIDGET(dlg) );


    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &cfg_edit_dlg_on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( btn_showinh ),
                      "toggled",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_showinh ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_apply_ ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_apply ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_exted ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_exted ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_reload_ ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_reload ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",
//                      "row-activated",
                      G_CALLBACK( &cfg_edit_dlg_on_row_sel ),
                      dlg );

    g_signal_emit_by_name( dlg->tree_v_, "cursor-changed", dlg );
}








static void
load_keys( EdaConfig*    ctx,
           const gchar*  group,
           cfg_edit_dlg* dlg,
           GtkTreeIter   itParent,
           gboolean      file_writable,
           gboolean*     inh_all )
{
    gsize len = 0;
    GError* err = NULL;

    gchar** pp = eda_config_get_keys( ctx, group, &len, &err );
    if ( pp == NULL )
    {
        printf( " >> load_keys(): !eda_config_get_keys()\n" );
        if ( err != NULL )
        {
            printf( "    err: %s\n", err->message );
        }
        g_clear_error( &err );
        return;
    }


    for ( gsize ndx = 0; ndx < len; ++ndx )
    {
        const gchar* name = pp[ ndx ];

        gchar* val = eda_config_get_string( ctx, group, name, &err );
        if ( val == NULL )
        {
            printf( " >> load_keys( %s ): !eda_config_get_string( %s )\n", group, name );
            if ( err != NULL )
            {
                printf( "    err: %s\n", err->message );
            }
            continue;
        }


        gboolean inh = eda_config_is_inherited( ctx, group, name, &err );
        if ( err != NULL )
        {
            printf( " >> load_keys(): !eda_config_is_inherited()\n" );
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
                                   !file_writable, // ro
                                   inh             // inh
                                 );

        add_row( dlg,
                 name,
                 inh,
                 val,
                 rdata,
                 &itParent );

        g_free( val );

    } // for keys

    g_strfreev( pp );

} // load_keys()



static void
load_groups( EdaConfig*    ctx,
             const gchar*  fname,
             cfg_edit_dlg* dlg,
             GtkTreeIter   itParent,
             gboolean      file_writable )
{
    if ( fname != NULL )
    {
        GError* err = NULL;
        gboolean res = eda_config_load( ctx, &err );
        if ( !res )
        {
            printf( " >> load_groups(): !eda_config_load()\n" );

            if ( err != NULL )
            {
                printf( "    err: %s\n", err->message );
            }

            g_clear_error( &err );
            return;
        }
    }

    gsize len = 0;
    gchar** pp = eda_config_get_groups( ctx, &len );
    if ( pp == NULL )
    {
        printf( " >> load_groups(): !eda_config_get_groups()\n" );
        return;
    }

    for ( gsize ndx = 0; ndx < len; ++ndx )
    {
        const gchar* name = pp[ ndx ];

        if ( strstr( name, "dialog-geometry" ) != NULL )
            continue;

        gboolean inh = FALSE;

        // NOTE: rdata:
        //
        row_data* rdata = mk_rdata( ctx,
                                    NULL,  // group
                                    NULL,  // key
                                    NULL,  // val
                                    TRUE,  // ro
                                    inh    // inh
                                  );

        GtkTreeIter it = add_row( dlg,
                                  name,    // name
                                  inh,     // inh
                                  "",      // val
                                  rdata,   // rdata
                                  &itParent
                                );

        gboolean inh_all = TRUE;
        load_keys( ctx, name, dlg, it, file_writable, &inh_all );

        row_set_field_inh( dlg, it, inh_all );

    } // for groups

    g_strfreev( pp );

} // load_groups()



static void
load_ctx( EdaConfig* ctx, const gchar* name, cfg_edit_dlg* dlg )
{
    gboolean exist = FALSE;
    gboolean rok   = FALSE;
    gboolean wok   = FALSE;

    const gchar* fname = ctx_get_fname( ctx, &exist, &rok, &wok );

    gchar str[ PATH_MAX ] = "";

    if ( fname != NULL )
    {
        sprintf( str, "[%s%s%s] %s",
                 exist ? "f" : "-",
                 rok   ? "r" : "-",
                 wok   ? "w" : "-",
                 fname );
    }

//    gboolean inh = eda_config_get_parent( ctx ) != NULL;
    gboolean inh = FALSE;

    // NOTE: rdata:
    //
    row_data* rdata = mk_rdata( ctx,
                               NULL,  // group
                               NULL,  // key
                               NULL,  // val
                               TRUE,  // ro
                               inh    // inh
                             );

    GtkTreeIter it = add_row( dlg,
                              name,  // name
                              inh,   // inh
                              str,   // val
                              rdata, // rdata
                              NULL
                            );

    load_groups( ctx, fname, dlg, it, wok );

} // load_ctx()



static void
load_cfg( cfg_edit_dlg* dlg )
{
    load_ctx( eda_config_get_default_context(),       "DEFAULT",  dlg );
    load_ctx( eda_config_get_system_context(),        "SYSTEM",   dlg );
    load_ctx( eda_config_get_user_context(),          "USER",     dlg );
    load_ctx( eda_config_get_context_for_path( "." ), "PATH [.]", dlg );
}

