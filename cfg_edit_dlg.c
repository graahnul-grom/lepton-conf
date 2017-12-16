#include "cfg_edit_dlg.h"
#include <liblepton/liblepton.h>
#include <liblepton/libgedaguile.h>


/*! \brief Create a new cfg edit dlg
*
*  \param [in] w_current The GschemToplevel structure
*/
GtkWidget* cfg_edit_dlg_new()
//cfg_edit_dlg_new (GschemToplevel *w_current)
{
  gpointer obj = g_object_new (CFG_EDIT_DLG_TYPE, NULL);
  return GTK_WIDGET (obj);
}



/* ******************************************************************&
*
*  gobject-specific stuff:
*
*/

G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);
// G_DEFINE_TYPE (GschemObjectPropertiesWidget, gschem_object_properties_widget, GSCHEM_TYPE_BIN);


static void cfg_edit_dlg_get_property( GObject* obj, guint id, GValue* val, GParamSpec* spec )
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


static void cfg_edit_dlg_set_property( GObject* obj, guint id, const GValue* val, GParamSpec* spec )
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


static void cfg_edit_dlg_dispose( GObject* obj )
{
//    printf( "cfg_edit_dlg::dispose( %p ); refcnt: %d\n",
//        obj, obj ? obj->ref_count : 0 );

    cfg_edit_dlgClass* cls = CFG_EDIT_DLG_GET_CLASS( obj );

    GObjectClass* parent_cls = g_type_class_peek_parent( cls );
    parent_cls->dispose( obj );

    printf( "cfg_edit_dlg::dispose(): done.\n" );
}


static void cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
    printf( "cfg_edit_dlg::class_init()\n" );

    GObjectClass* gcls = G_OBJECT_CLASS( cls );

    gcls->dispose      = &cfg_edit_dlg_dispose;
    gcls->get_property = &cfg_edit_dlg_get_property;
    gcls->set_property = &cfg_edit_dlg_set_property;

    GParamSpec* spec = g_param_spec_int( "prop1",
        "",  // nick
        "",  // blurb
        0,   // min
        10,  // max
        7,   // default
        G_PARAM_READABLE | G_PARAM_WRITABLE );

    g_object_class_install_property( gcls, CFG_EDIT_DLG_PROPID_PROP1, spec );
}

/**** ^^ gobject stuff ^^ ****************************************************** */




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



// TODO: row_data: free memory
//
static row_data*
mk_rdata( EdaConfig*  ctx,
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



static gboolean
conf_add_val( row_data* rdata, const gchar* key, const gchar* val );

static gboolean
conf_chg_val( row_data* rdata, const gchar* txt );

static void
conf_load( cfg_edit_dlg* dlg );

static const gchar*
conf_ctx_name( EdaConfig* ctx );

static const gchar*
conf_ctx_fname( EdaConfig* ctx, gboolean* exist, gboolean* rok, gboolean* wok );




static int tree_colid_name()  { return COL_NAME; }
static int tree_colid_val()   { return COL_VAL;  }
static int tree_colid_data()  { return COL_DATA; }
static int tree_cols_cnt()    { return NUM_COLS; }




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
    if ( !res )
        printf( " >> >> row_cur_get_iter(): !sel\n");

    return res;

} // row_cur_get_iter()



// {ret}: parent iterator of currently selected row
//
static gboolean
row_cur_get_parent_iter( cfg_edit_dlg* dlg, GtkTreeIter* it, GtkTreeIter* itParent )
{
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    // TODO: free path:
    //
    GtkTreePath* path = gtk_tree_model_get_path( mod, it );

    gtk_tree_path_up( path );
    return gtk_tree_model_get_iter( mod, itParent, path );

} // row_cur_get_parent_iter()




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




// [it_result]: will be set to found row
// {ret}: TRUE if cur row is grp and it has child row with [key]
//
//static gboolean
static GtkTreePath*
row_cur_find_child_key( cfg_edit_dlg* dlg,
                        const gchar* key )
{
    GtkTreeIter it_parent;
    if ( !row_cur_get_iter( dlg, &it_parent ) )
        return FALSE;

    row_data* rdata = row_field_get_data( dlg, &it_parent );
    if ( !rdata )
        return FALSE;

    if ( rdata->rtype_ != RT_GRP )
        return FALSE;


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );


    if ( !gtk_tree_model_iter_has_child( mod, &it_parent ) ) // // //
        return FALSE;

    GtkTreeIter it_child;
    gboolean res = gtk_tree_model_iter_children( mod,
                                                 &it_child,
                                                 &it_parent );
    const gchar* kname = NULL;

    while ( res )
    {
        kname = row_field_get_name( dlg, &it_child );

        if ( g_strcmp0( kname, key ) == 0 )
        {
            gchar* str = gtk_tree_model_get_string_from_iter( mod, &it_child );
            return gtk_tree_path_new_from_string( str );
        }

        res = gtk_tree_model_iter_next( mod, &it_child );
    }

    return NULL;

} // row_cur_find_child_key()




static void
row_set_inh( cfg_edit_dlg* dlg, GtkTreeIter itCPY, gboolean val )
{
    GtkTreeIter it = itCPY;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    rdata->inh_ = val;

} // row_set_inh()




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
}



static GtkTreeIter
row_add( cfg_edit_dlg* dlg,
         const gchar*  name,
         const gchar*  val,
         gpointer      rdata,
         GtkTreeIter*  itParent )
{
    GtkTreeIter it;
    gtk_tree_store_append( dlg->store_, &it, itParent );
    //
    // NOTE: gtk_tree_store_set() makes copies of strings:
    //
    gtk_tree_store_set( dlg->store_,
                        &it,
                        tree_colid_name(),     name,
                        tree_colid_val(),      val,
                        tree_colid_data(),     rdata,
                        -1 );

    return it;
}









static void
on_delete_event( GtkWidget* dlg, GdkEvent* e, gpointer* p )
{
    printf( "cfg_edit_dlg::on_delete_event()\n" );
    gtk_widget_destroy( dlg );
}



static void
on_row_sel( GtkTreeView* tree, gpointer* p )
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


    gtk_label_set_text( GTK_LABEL( dlg->lab_ctx_ ), conf_ctx_name( rdata->ctx_ ) );

    if ( rdata->rtype_ == RT_KEY )
    {
        gchar* name = row_field_get_name( dlg, &it );
        gchar* val = row_field_get_val( dlg, &it );
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), name );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ), val );
        g_free( name );
        g_free( val );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ), NULL );
    }

    const gchar* fname = conf_ctx_fname( rdata->ctx_, NULL, NULL, NULL );
    gchar* str = NULL;
    if ( fname != NULL )
        str = g_strdup_printf( "<a href='%s'>%s</a>", fname, fname );
    gtk_label_set_markup( GTK_LABEL( dlg->lab_fname_ ), str ? str : "" );
    g_free( str );

//    printf( " >> on_row_sel(): ctx fname: [%s]\n", fname );
//    printf( " >> on_row_sel(): name: [%s], val: [%s]\n", name, val );

} // on_row_sel()



static void
on_btn_reload( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;


    // remember current tree node:
    //
    GtkTreePath* path = NULL;
    GtkTreeIter it;
    if ( row_cur_get_iter( dlg, &it ) )
    {
        //
        // TODO: free path:
        //
        GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
        path = gtk_tree_model_get_path( mod, &it );
    }


    tree_filter_remove( dlg );

    gtk_tree_store_clear( dlg->store_ );

    conf_load( dlg );

    tree_filter_setup( dlg );


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

} // on_btn_reload()



static void
on_lab_fname( GtkLabel* lab, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    const gchar* fname = gtk_label_get_text( lab );

    printf( " >> on_lab_fname(): [%s]\n", fname);

    const gchar exted[] = "gvim";
    GError* err = NULL;
    GAppInfo* ai =
    g_app_info_create_from_commandline( exted,
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
}



static void
on_btn_showinh( GtkToggleButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gboolean show = gtk_toggle_button_get_active( btn );
//    printf( " >> cfg_edit_dlg_on_btn_showinh(): %d\n", show );

    dlg->showinh_ = show;
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( mod ) );

    gtk_tree_view_expand_all( dlg->tree_v_ );

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
}




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

//    g_signal_connect_swapped (vdlg,
//                                "response",
//                                G_CALLBACK (gtk_widget_destroy),
//                                vdlg);


    gtk_widget_show_all( vdlg );
    gtk_widget_set_size_request( vdlg, 300, -1 );





    gint res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  edit_val_dlg(): resp: %d\n", res );

    gchar* ret = NULL;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        ret = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent ) ) );
    }

    gtk_widget_destroy( vdlg );

    return ret;

} // run_dlg_edit_val()



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
    gtk_entry_set_text( GTK_ENTRY( ent_val ), "newVal" );

    GtkWidget* vbox = gtk_vbox_new( TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_key, TRUE, TRUE, 5 );
    gtk_box_pack_start( GTK_BOX( vbox ), ent_val, TRUE, TRUE, 5 );

    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( vdlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), vbox, TRUE, TRUE, 10 );

    gtk_widget_show_all( vdlg );
    gtk_widget_set_size_request( vdlg, 300, -1 );
    gint res = gtk_dialog_run( GTK_DIALOG( vdlg ) );

    printf( "  edit_val_dlg(): resp: %d\n", res );

    gboolean ret = FALSE;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        *key = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_key ) ) );
        *val = g_strdup( gtk_entry_get_text( GTK_ENTRY( ent_val ) ) );
        ret = TRUE;
    }

    gtk_widget_destroy( vdlg );

    return ret;

} // dlg_add_val_run()



static void
on_mitem_edit( GtkMenuItem* mitem, gpointer p )
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

//    printf( "cfg_edit_dlg_on_mitem_edit(): k: [%s], v: [%s]\n", rdata->key_, rdata->val_ );

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

    if ( txt && conf_chg_val( rdata, txt ) )
    {
//        printf( "cfg_edit_dlg_on_mitem_edit(): [%s] => [%s]\n", rdata->val_, txt );

        row_field_set_val( dlg, it, txt );

        g_free( txt );


        // unset inherited:
        //
        row_set_inh( dlg, it, FALSE );
        GtkTreeIter it_parent;
        if ( row_cur_get_parent_iter( dlg, &it, &it_parent ) )
            row_set_inh( dlg, it_parent, FALSE );

    }

} // on_mitem_edit()



static void
on_mitem_add( GtkMenuItem* mitem, gpointer p )
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

    if ( run_dlg_add_val( dlg, NULL, &key, &val ) )
    {
        GtkTreePath* path = row_cur_find_child_key( dlg, key );

        if ( path != NULL )
        {
            printf( "on_mitem_add(): [%s] EXISTS\n", key );

            gtk_tree_view_expand_to_path( dlg->tree_v_, path );
            gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path, NULL, NULL, FALSE );

            GtkTreeIter it_child;
            GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
            gtk_tree_model_get_iter( mod, &it_child, path );

            row_data* rdata_child = row_field_get_data( dlg, &it_child );

            if ( conf_chg_val( rdata_child, val ) )
            {
                row_field_set_val( dlg, it_child, val );

                // unset inherited:
                //
                row_set_inh( dlg, it_child, FALSE );
                row_set_inh( dlg, it, FALSE );
            }

            gtk_tree_path_free( path );
            return;
        }


        if ( conf_add_val( rdata, key, val ) )
        {
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

            // TODO: dlg_model_upd( dlg ): need it?
            // dlg_model_upd( dlg );

            // GtkTreePath* path = gtk_tree_model_get_path( dlg_model( dlg ), // ERR
            // printf( " >> on_mitem_add(): [%s]\n", gtk_tree_path_to_string( path ) );

            GtkTreePath* path = NULL;

            // expand parent:
            //
            path = gtk_tree_model_get_path( GTK_TREE_MODEL( dlg->store_ ),
                                            &it_grp_tstrore );
            gtk_tree_view_expand_row( dlg->tree_v_, path, FALSE );
            gtk_tree_path_free( path );

            // select child:
            //
            path = gtk_tree_model_get_path( GTK_TREE_MODEL( dlg->store_ ),
                                            &it_new );
            gtk_tree_view_set_cursor( dlg->tree_v_, path, NULL, FALSE );
            gtk_tree_path_free( path );

            // unset inherited:
            //
            GtkTreeIter it_cur;
            if ( row_cur_get_iter( dlg, &it_cur ) )
                row_set_inh( dlg, it_cur, FALSE );
            GtkTreeIter it_parent;
            if ( row_cur_get_parent_iter( dlg, &it_cur, &it_parent ) )
                row_set_inh( dlg, it_parent, FALSE );

        } // if conf_add_val()

        g_free( key );
        g_free( val );

    } // if run_dlg_add_val()

} // on_mitem_add()



static GtkMenu*
mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata )
{
    if ( rdata->rtype_ == RT_CTX )
        return NULL;

    GtkWidget* menu = gtk_menu_new();

    GtkWidget* mitem_edit = NULL;
    GtkWidget* mitem_add  = NULL;

    if ( rdata->rtype_ == RT_KEY )
    {
        mitem_edit = gtk_menu_item_new_with_mnemonic( "_edit" );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_edit);
        g_signal_connect( G_OBJECT( mitem_edit ),
                          "activate",
                          G_CALLBACK( &on_mitem_edit ),
                          dlg );
        gtk_widget_show( mitem_edit );
        gtk_widget_set_sensitive( mitem_edit, !rdata->ro_ );
    }

    if ( rdata->rtype_ == RT_GRP )
    {
        mitem_add = gtk_menu_item_new_with_mnemonic( "_add" );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_add);
        g_signal_connect( G_OBJECT( mitem_add ),
                          "activate",
                          G_CALLBACK( &on_mitem_add ),
                          dlg );
        gtk_widget_show( mitem_add );
        gtk_widget_set_sensitive( mitem_add, !rdata->ro_ );
    }


//    gtk_widget_set_sensitive( mitem2, rdata->inh_ );

    return GTK_MENU( menu );

} // mk_popup_menu()



static gboolean
on_rmb( GtkWidget* w, GdkEvent* e, gpointer p )
{
    GdkEventButton* ebtn = ( GdkEventButton* ) e;

    // not an RMB => do not process event (return FALSE):
    //
    if ( ebtn->type != GDK_BUTTON_PRESS || ebtn->button != 3 )
        return FALSE;


    //
    // further down only return TRUE to not allow selecting rows with RMB
    //


    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return TRUE;

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




/* ******************************************************************&
*
*  config manipulation routines:
*
*/

static void
conf_load_keys( EdaConfig*    ctx,
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
                                    !file_writable, // ro
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
            printf( " >> conf_load_groups(): !eda_config_load( \"%s\" )\n", fname );

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
        printf( " >> conf_load_groups(): !eda_config_get_groups()\n" );
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
                                    name,           // group
                                    NULL,            // key
                                    NULL,            // val
                                    !file_writable,  // ro
                                    // TRUE,         // ro
                                    inh,             // inh
                                    RT_GRP           // rtype
                                  );

        gchar* display_name = g_strdup_printf( "[%s]", name );

        GtkTreeIter it = row_add( dlg,
                                  display_name,
                                  // name, // name
                                  "",      // val
                                  rdata,   // rdata
                                  &itParent
                                );

        g_free( display_name );

        // make sure empty groups will not be marked as inherited:
        //   see load_keys()
        //
        gboolean inh_all = FALSE;

        conf_load_keys( ctx, name, dlg, it, file_writable, &inh_all );

        // mark group itself as inh if all children are inh:
        //
        rdata->inh_ = inh_all;

    } // for groups

    g_strfreev( pp );

} // conf_load_groups()



static void
conf_load_ctx( EdaConfig* ctx, const gchar* name, cfg_edit_dlg* dlg )
{
    gboolean exist = FALSE;
    gboolean rok   = FALSE;
    gboolean wok   = FALSE;

    const gchar* fname = conf_ctx_fname( ctx, &exist, &rok, &wok );

    gchar str[ PATH_MAX ] = "";

    if ( fname != NULL )
    {
        sprintf( str, "config file: (%s%s%s) %s",
                 exist ? "f" : "-",
                 rok   ? "r" : "-",
                 wok   ? "w" : "-",
                 fname
               );
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
                                inh,   // inh
                                RT_CTX // rtype
                              );

    GtkTreeIter it = row_add( dlg,
                              name,  // name
                              str,   // val
                              rdata, // rdata
                              NULL
                            );

//    load_groups( ctx, dlg, it, wok );
    conf_load_groups( ctx, fname, dlg, it, wok );

} // conf_load_ctx()



static void
conf_load( cfg_edit_dlg* dlg )
{
//    EdaConfig* cfg = eda_config_get_default_context();
//    GError* err = NULL;
//    eda_config_load( cfg, &err );
//    load_ctx( cfg, "context: DEFAULT",  dlg );
//    g_clear_error( &err );

    conf_load_ctx( eda_config_get_default_context(),       "context: DEFAULT",  dlg );
    conf_load_ctx( eda_config_get_system_context(),        "context: SYSTEM",   dlg );
    conf_load_ctx( eda_config_get_user_context(),          "context: USER",     dlg );
    conf_load_ctx( eda_config_get_context_for_path( "." ), "context: PATH (.)", dlg );
}



// {post}: {ret} owned by geda cfg api
//
static const gchar*
conf_ctx_name( EdaConfig* ctx )
{
    if ( ctx == eda_config_get_default_context() )
        return "DEFAULT";
    if ( ctx == eda_config_get_system_context() )
        return "SYSTEM";
    if ( ctx == eda_config_get_user_context() )
        return "USER";
    if ( ctx == eda_config_get_context_for_path( "." ) )
        return "PATH (.)";

    return NULL;
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



/*
static gboolean
conf_has_key( EdaConfig* ctx, const gchar* grp, const gchar* key )
{
    GError* err = NULL;
    gchar* val = eda_config_get_string( ctx, grp, key, &err );

    if ( err != NULL )
        printf( " >> conf_has_key(): err: %s\n", err->message );

    g_clear_error( &err );
    g_free( val );

    return val != NULL;
}
*/


static gboolean
conf_add_val( row_data* rdata, const gchar* key, const gchar* val )
{
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           key,
                           val );

    GError* err = NULL;
    gboolean res = eda_config_save( rdata->ctx_, &err );

    return res;
}



static gboolean
conf_chg_val( row_data* rdata, const gchar* txt )
{
    // set:
    //
    eda_config_set_string( rdata->ctx_,
                           rdata->group_,
                           rdata->key_,
                           txt );

    // save cfg:
    //
    GError* err = NULL;
    gboolean res = eda_config_save( rdata->ctx_, &err );
    if ( !res )
    {
        printf( " >> conf_chg_val( %s ): !eda_config_save()\n",
                txt );
        if ( err != NULL )
            printf( "    err: %s\n", err->message );
        g_clear_error( &err );
        return FALSE;
    }

    // get:
    //
    gchar* new_val = eda_config_get_string( rdata->ctx_,
                                            rdata->group_,
                                            rdata->key_,
                                            &err );
    if ( new_val == NULL )
    {
        printf( " >> conf_chg_val(): !eda_config_get_string( %s )\n",
                rdata->key_ );
        if ( err != NULL )
            printf( "    err: %s\n", err->message );
        g_clear_error( &err );
        return FALSE;
    }

    g_free( new_val );

    return TRUE;

} // conf_chg_val()




static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_init()\n" );

    dlg->prop1_ = 5;


    // tree store:
    //
    dlg->store_ = gtk_tree_store_new(
        tree_cols_cnt(),
          G_TYPE_STRING   // name
        , G_TYPE_STRING   // val
        , G_TYPE_POINTER  // rdata
    );

    // dlg_model_set( dlg, GTK_TREE_MODEL(dlg->store_) );


    // tree view:
    //
    // dlg->tree_w_ = gtk_tree_view_new_with_model( dlg_model( dlg ) );
    dlg->tree_w_ = gtk_tree_view_new_with_model( GTK_TREE_MODEL(dlg->store_) );
    dlg->tree_v_ = GTK_TREE_VIEW( dlg->tree_w_ );
    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );


    // tree view columns:
    //
    dlg->ren_txt_ = gtk_cell_renderer_text_new();

    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_name(), "name" );
    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_val(),  "value" );


    dlg->showinh_ = TRUE;


    conf_load( dlg );
    tree_filter_setup( dlg );


    gtk_tree_view_expand_all( dlg->tree_v_ );


    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG(dlg) );




    gchar* cwd = g_get_current_dir();




    // -------------------------- box (top):
    //
    GtkWidget* hbox_top = gtk_hbox_new( FALSE, 0 );


    // cwd labels:
    //
    GtkWidget* lbox000 = gtk_hbox_new( FALSE, 0 );

    GtkWidget* lab_cwd0 = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( lab_cwd0 ), "<b>cwd: </b>" );
    gtk_box_pack_start( GTK_BOX( lbox000 ), lab_cwd0, FALSE, FALSE, 0 );

    GtkWidget* lab_cwd1 = gtk_label_new( cwd );
    gtk_label_set_selectable( GTK_LABEL( lab_cwd1 ), TRUE );
    gtk_box_pack_start( GTK_BOX( lbox000 ), lab_cwd1, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( hbox_top ), lbox000, FALSE, FALSE, 0 );


    // "Find:" label:
    //
//    GtkWidget* lab_find = gtk_label_new_with_mnemonic( "_Find: " );
//    gtk_box_pack_start( GTK_BOX( hbox_top ), lab_find, FALSE, FALSE, 0 );


    // find edit field:
    //
//    GtkWidget* ent_find = gtk_entry_new();
//    gtk_box_pack_start( GTK_BOX( hbox_top ), ent_find, TRUE, TRUE, 0 );


    // attach label to edit field:
    //
//    gtk_label_set_mnemonic_widget( GTK_LABEL( lab_find ), ent_find );


    // add hbox_top to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ),  hbox_top, FALSE, FALSE, 0 );







    // -------------------------- box2 (top):
    //
    GtkWidget* hbox2_top = gtk_hbox_new( FALSE, 0 );



        // TODO: set window title elsewhere:
        //
        gtk_window_set_title( GTK_WINDOW( dlg ),
                              g_strdup_printf( "gedacfged - %s", cwd ) );
        // g_free( cwd );



    // add hbox2_top to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ),  hbox2_top, FALSE, FALSE, 0 );



    // scrolled win:
    //
    GtkWidget* wscroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );

    // add tree to wscroll:
    //
    gtk_container_add( GTK_CONTAINER( wscroll ), dlg->tree_w_ );


    // add wscroll to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ), wscroll, TRUE, TRUE, 0 );







    // -------------------------- box (bottom):
    //
//    GtkWidget* vbox_bot = gtk_vbutton_box_new();
//    GtkWidget* vbox_bot = gtk_vbox_new( FALSE, 0 );
    GtkWidget* vbox_bot = gtk_vbox_new( TRUE, 0 );




    // ctx labels:
    //
    GtkWidget* lbox0 = gtk_hbox_new( FALSE, 0 );

    GtkWidget* lab_ctx0 = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( lab_ctx0 ), "<b>ctx: </b>" );
    gtk_box_pack_start( GTK_BOX( lbox0 ), lab_ctx0, FALSE, FALSE, 0 );

    dlg->lab_ctx_ = gtk_label_new( NULL );
    gtk_label_set_selectable( GTK_LABEL( dlg->lab_ctx_ ), TRUE );
    gtk_box_pack_start( GTK_BOX( lbox0 ), dlg->lab_ctx_, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( vbox_bot ), lbox0, FALSE, FALSE, 0 );




    // ctx fname labels:
    //
    GtkWidget* lbox00 = gtk_hbox_new( FALSE, 0 );

    GtkWidget* lab_fname0 = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( lab_fname0 ), "<b>fname: </b>" );
    gtk_box_pack_start( GTK_BOX( lbox00 ), lab_fname0, FALSE, FALSE, 0 );

    dlg->lab_fname_ = gtk_label_new( NULL );
//    gtk_label_set_selectable( GTK_LABEL( dlg->lab_fname_ ), TRUE );
    gtk_label_set_track_visited_links( GTK_LABEL( dlg->lab_fname_ ), FALSE );
    gtk_box_pack_start( GTK_BOX( lbox00 ), dlg->lab_fname_, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( vbox_bot ), lbox00, FALSE, FALSE, 0 );




    gtk_box_pack_start( GTK_BOX( vbox_bot ),
                        gtk_hseparator_new(), FALSE, FALSE, 0 );

    // name labels:
    //
    GtkWidget* lbox111 = gtk_hbox_new( FALSE, 0 );

    GtkWidget* lab_name0 = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( lab_name0 ), "<b>name: </b>" );
    gtk_box_pack_start( GTK_BOX( lbox111 ), lab_name0, FALSE, FALSE, 0 );

    dlg->lab_name_ = gtk_label_new( NULL );
    gtk_label_set_selectable( GTK_LABEL( dlg->lab_name_ ), TRUE );
    gtk_box_pack_start( GTK_BOX( lbox111 ), dlg->lab_name_, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( vbox_bot ), lbox111, FALSE, FALSE, 0 );



    // val labels:
    //
    GtkWidget* lbox222 = gtk_hbox_new( FALSE, 0 );

    GtkWidget* lab_val0 = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( lab_val0 ), "<b>value: </b>" );
    gtk_box_pack_start( GTK_BOX( lbox222 ), lab_val0, FALSE, FALSE, 0 );

    dlg->lab_val_ = gtk_label_new( NULL );
    gtk_label_set_selectable( GTK_LABEL( dlg->lab_val_ ), TRUE );
    gtk_box_pack_start( GTK_BOX( lbox222 ), dlg->lab_val_, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( vbox_bot ), lbox222, FALSE, FALSE, 0 );



    gtk_box_pack_start( GTK_BOX( vbox_bot ),
                        gtk_hseparator_new(), FALSE, FALSE, 0 );



    // show inh check box:
    //
    GtkWidget* btn_showinh = gtk_check_button_new_with_mnemonic( "" );
    GtkWidget* lab_showinh = gtk_bin_get_child( GTK_BIN( btn_showinh ) );
    gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab_showinh ),
                                        "<i>sho_w inherited</i>" );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( btn_showinh ), dlg->showinh_ );

//    gtk_box_pack_start_defaults( GTK_BOX( vbox_bot ), btn_showinh );
    gtk_box_pack_start( GTK_BOX( vbox_bot ), btn_showinh, FALSE, FALSE, 0 );
//    gtk_box_pack_start( GTK_BOX( vbox_bot ), btn_showinh, TRUE, FALSE, 0 );



    // add box_bot to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ),  vbox_bot, FALSE, FALSE, 0 );







    // -------------------------- action area:
    //
    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG(dlg) );

    // reload btn:
    //
    dlg->btn_reload_ = gtk_button_new_with_mnemonic( "_reload" );
    gtk_box_pack_start( GTK_BOX( aa ), dlg->btn_reload_, FALSE, FALSE, 0 );

    // ext ed btn:
    //
//    GtkWidget* btn_exted = gtk_button_new_with_mnemonic( "_ext ed" );
//    gtk_box_pack_start( GTK_BOX( aa ), btn_exted, FALSE, FALSE, 0 );





    // show all:
    //
    gtk_widget_show_all( GTK_WIDGET(dlg) );




    // event handlers:
    //
    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( btn_showinh ),
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

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",
//                      "row-activated",
                      G_CALLBACK( &on_row_sel ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "button-press-event",
                      G_CALLBACK( &on_rmb ),
                      dlg );


    // NOTE: dont't do it:
    //  if tree not focused on startup => SIGSEGV
    //
    // g_signal_emit_by_name( dlg->tree_v_, "cursor-changed", dlg );

    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );


    // select row:
    //
    GtkTreePath* path = gtk_tree_path_new_from_string( "0" );
    gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path, NULL, NULL, FALSE );
    gtk_tree_path_free( path );


    g_free( cwd );

} // cfg_edit_dlg_init()

