#include "cfg_edit_dlg.h"

#include <liblepton/liblepton.h>




struct _conf_key_data
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* seed_;
    const gchar* desc_;
};

typedef struct _conf_key_data conf_key_data;


static conf_key_data g_conf_key_data[] =
{
    {
        "sys",
        "newKey",
        "sys__newKey",
        "eklmn oprst"
    },
    {
        "gschem",
        "default-filename",
        "gschem__default-filename",
        "Define the default file name for any new schematic files created in gschem.\n"
        "It is used to create filenames of the form “untitled_N.sch” where N is a number."
    },
    {
        "schematic.gui",
        "use-docks",
        "schematic.gui__use-docks",
//        "The type of GUI is controlled by \"use-docks\" boolean"
//        " configuration key in \"gschem.gui\" group."
//        " If widgets will be shown"
//        " in docks (true) or as"
//        " a dialog boxes (false)"

        "The type of GUI: how to display widgets.\n"
        " If true, widgets will be shown in docks.\n"
        " If false, widgets will be shown as a dialog boxes."

//        "The type of GUI is controlled by \"use-docks\" boolean configuration key in \"gschem.gui\" group. If widgets will be shown in docks (true) or as a dialog boxes (false)"
    },
    {
        NULL,
        NULL,
        NULL,
        NULL
    }
};


const conf_key_data*
conf_key_data_lookup( const gchar* grp, const gchar* key )
{
    const conf_key_data* data = g_conf_key_data;

    char* seed = g_strdup_printf( "%s__%s", grp, key );
    const conf_key_data* ret = NULL;

    for ( ; data->seed_ != NULL; ++data )
    {
        if ( g_strcmp0( data->seed_, seed ) == 0 )
        {
            ret = data;
            break;
        }
    }

    g_free( seed );

    return ret;
}


static const gchar*
conf_key_data_lookup_desc( const gchar* grp, const gchar* key )
{
    const conf_key_data* data = conf_key_data_lookup( grp, key );

    if ( data != NULL)
        return data->desc_;

    return NULL;
}




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
mk_gui( cfg_edit_dlg* dlg );

static row_data*
row_field_get_data( cfg_edit_dlg* dlg, GtkTreeIter* it );

static void
conf_add_val( row_data* rdata, const gchar* key, const gchar* val );

static void
conf_chg_val( row_data* rdata, const gchar* txt );

static gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg );

static gboolean
conf_load_ctx( EdaConfig* ctx );

static GtkTreeIter
conf_mk_ctx_node( EdaConfig* ctx, const gchar* name, cfg_edit_dlg* dlg );

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



/* ******************************************************************
*
*  gobject stuff:
*
*/

G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);
// G_DEFINE_TYPE (GschemObjectPropertiesWidget, gschem_object_properties_widget, GSCHEM_TYPE_BIN);



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
    printf( "cfg_edit_dlg::dispose( %p ); refcnt: %d\n",
        obj, obj ? obj->ref_count : 0 );

    cfg_edit_dlgClass* cls = CFG_EDIT_DLG_GET_CLASS( obj );

    GObjectClass* parent_cls = g_type_class_peek_parent( cls );
    parent_cls->dispose( obj );

    printf( "cfg_edit_dlg::dispose(): done.\n" );
}



static void
cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
    // NOTE: glib debug messages:
    // NOTE: g_debug() adds newline:
    //
    g_debug( " >> g_debug(): cfg_edit_dlg::class_init()" );

    GObjectClass* gcls = G_OBJECT_CLASS( cls );

    gcls->dispose      = &cfg_edit_dlg_dispose;
    gcls->get_property = &cfg_edit_dlg_get_property;
    gcls->set_property = &cfg_edit_dlg_set_property;
}



static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_init()\n" );
    mk_gui( dlg );
}




/* ******************************************************************
*
*  rdata:
*
*/

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
        printf( " >> >> row_cur_pos_restore(): !path_str\n");
        return;
    }

    GtkTreePath* path = gtk_tree_path_new_from_string( path_str );


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeIter it;
    if ( !gtk_tree_model_get_iter( mod, &it, path ) )
        gtk_tree_path_up( path );
    // gtk_tree_model_sort_iter_is_valid()



    gtk_tree_view_expand_to_path( dlg->tree_v_, path );
    gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path, NULL, NULL, FALSE );

    gtk_tree_path_free( path );
    g_free( path_str );
}



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
    printf( "cfg_edit_dlg::on_delete_event()\n" );

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

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gtk_label_set_text( GTK_LABEL( dlg->lab_ctx_ ), conf_ctx_name( rdata->ctx_ ) );

    if ( rdata->rtype_ == RT_KEY )
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), rdata->key_ );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  rdata->val_ );

        const gchar* desc = conf_key_data_lookup_desc( rdata->group_, rdata->key_ );
        gtk_text_buffer_set_text( dlg->txtbuf_desc_, desc ? desc : "", -1 );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_name_ ), NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  NULL );

        gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );
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

} // on_row_sel()



static void
on_btn_reload( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;


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

} // on_btn_reload()



static void
on_btn_tst( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;


    // char* path_str = row_cur_pos_save( dlg );
    // conf_reload_ctx_path( dlg );
    // row_cur_pos_restore( dlg, path_str );
}



static void
on_lab_fname( GtkLabel* lab, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    const gchar* fname = gtk_label_get_current_uri( lab );

    const gchar exted[] = "gvim";
//    const gchar exted[] = "leaf pad";

    GError* err = NULL;
    GAppInfo* ai =
    g_app_info_create_from_commandline( exted,
                                        NULL,
                                        G_APP_INFO_CREATE_NONE,
                                        &err );
    if ( err )
    {
        printf( " >> on_lab_fname(): err: [%s]\n", err->message );
        g_clear_error( &err );
    }

    if ( ai )
    {
        GFile* gfile = g_file_new_for_path( fname );
        GList* args = g_list_append( NULL, gfile );

        if ( !g_app_info_launch( ai, args, NULL, &err ) )
        {
            printf( " >> on_lab_fname(): !g_app_info_launch():\n" );
            printf( "    err: [%d], msg: [%s]\n", err->code, err->message );
        }

        g_clear_error( &err );
        g_list_free( args );
    }

} // on_lab_fname()



static void
on_btn_showinh( GtkToggleButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    dlg->showinh_ = gtk_toggle_button_get_active( btn );

//    char* path = row_cur_pos_save( dlg );


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );

    gtk_tree_model_filter_refilter( GTK_TREE_MODEL_FILTER( mod ) );
//    gtk_tree_view_expand_all( dlg->tree_v_ );
    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );


//    row_cur_pos_restore( dlg, path );

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

//    printf( "cfg_edit_dlg_on_mitem_edit(): k: [%s], v: [%s]\n", rdata->key_, rdata->val_ );

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

    if ( txt == NULL )
        return;

    if ( !rdata->inh_ && g_strcmp0( rdata->val_, txt ) == 0 )
    {
        g_free( txt );
        return;
    }


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

    GtkTreePath* path1 = row_cur_find_child_key( dlg, key );

    if ( path1 != NULL )
    {
        printf( "on_mitem_add(): [%s] EXISTS\n", key );

        gtk_tree_view_expand_to_path( dlg->tree_v_, path1 );
        gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path1, NULL, NULL, FALSE );

        GtkTreeIter it_child;
        GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
        gtk_tree_model_get_iter( mod, &it_child, path1 );

        row_data* rdata_child = row_field_get_data( dlg, &it_child );


        if ( rdata_child->inh_ || g_strcmp0( val, rdata_child->val_ ) != 0 )
        {
            // NOTE: conf_chg_val() / conf_save()
            //
            conf_chg_val( rdata_child, val );

            if ( conf_save( rdata->ctx_, dlg ) )
            {
                row_field_set_val( dlg, it_child, val );

                // unset inherited:
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

    GtkTreePath* path2 = NULL;

    // expand parent:
    //
    path2 = gtk_tree_model_get_path( GTK_TREE_MODEL( dlg->store_ ),
                                    &it_grp_tstrore );
    gtk_tree_view_expand_row( dlg->tree_v_, path2, FALSE );
    gtk_tree_path_free( path2 );

    // select child:
    //
    path2 = gtk_tree_model_get_path( GTK_TREE_MODEL( dlg->store_ ),
                                    &it_new );
    // gtk_tree_view_set_cursor( dlg->tree_v_, path2, NULL, FALSE );
    gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path2, NULL, NULL, FALSE );
    gtk_tree_path_free( path2 );

    // unset inherited:
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

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;
}



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

//    printf( "  run_dlg_edit_val(): resp: %d\n", res );

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
    gtk_entry_set_text( GTK_ENTRY( ent_val ), "newVal" );

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

} // run_dlg_add_val()




/* ******************************************************************
*
*  "config-changed" event handlers:
*  NOTE: not used
*
*/

static void
on_conf_chg_ctx_dflt( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
    printf( " >> >> on_conf_chg_ctx_dflt(): [%d], [%s] [%s]\n",
        ctx == eda_config_get_default_context(), g, k );
}


static void
on_conf_chg_ctx_sys( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
    printf( " >> >> on_conf_chg_ctx_sys(): [%d], [%s] [%s]\n",
        ctx == eda_config_get_system_context(), g, k );
}


static void
on_conf_chg_ctx_user( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
    printf( " >> >> on_conf_chg_ctx_user(): [%d], [%s] [%s]\n",
        ctx == eda_config_get_user_context(), g, k );
}


static void
on_conf_chg_ctx_path( EdaConfig* ctx, const gchar* g, const gchar* k, void* p )
{
    printf( " >> >> on_conf_chg_ctx_path(): [%d], [%s] [%s]\n",
        ctx == eda_config_get_context_for_path( "." ), g, k );
}




/* ******************************************************************&
*
*  config:
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

        if ( strstr( name, "dialog-geometry" ) != NULL )
            continue;


        gboolean wok = conf_ctx_file_writable( ctx );
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
            printf( " >> conf_load_ctx(): !eda_config_load( \"%s\" )\n", fname );

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
                  const gchar*  name,
                  cfg_edit_dlg* dlg )
{
    gboolean wok = conf_ctx_file_writable( ctx );

    printf( "conf_mk_ctx_node( %s ): wok: [%d]\n",
            conf_ctx_name( ctx ),
            wok );

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



    conf_load_groups( ctx, dlg, it_ctx );



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

    row_cur_pos_restore( dlg, path );

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

    it = conf_mk_ctx_node( ctx_dflt, name_dflt, dlg );
    conf_load_ctx( ctx_dflt );
    conf_load_groups( ctx_dflt, dlg, it );

    it = conf_mk_ctx_node( ctx_sys, name_sys, dlg );
    conf_load_ctx( ctx_sys );
    conf_load_groups( ctx_sys, dlg, it );


    it = conf_mk_ctx_node( ctx_user, name_user, dlg );
    conf_load_ctx( ctx_user );
    conf_load_groups( ctx_user, dlg, it );

    it = conf_mk_ctx_node( ctx_path, name_path, dlg );
    conf_load_ctx( ctx_path );
    conf_load_groups( ctx_path, dlg, it );


    g_free( name_dflt );
    g_free( name_sys );
    g_free( name_user );
    g_free( name_path );


    // setup "config-changed" handlers:
    // NOTE: not used
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

        printf( " .. conf_ctx_file_writable( %s ): fname: [%s], dir: [%s]: wok: [%d]\n",
                    conf_ctx_name( ctx ),
                    fname,
                    dir,
                    ret );

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
conf_chg_val( row_data* rdata, const gchar* txt )
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

static void
mk_labels_line( const gchar* left_txt,
                GtkWidget* right_label,
                GtkWidget* parent_box )
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
mk_labels_line_separ( GtkWidget* parent_box )
{
    gtk_box_pack_start( GTK_BOX( parent_box ),
                        gtk_hseparator_new(), FALSE, FALSE, 0 );
}



static void
mk_gui( cfg_edit_dlg* dlg )
{
    // tree store:
    //
    dlg->store_ = gtk_tree_store_new(
        tree_cols_cnt(),
          G_TYPE_STRING   // name
        , G_TYPE_STRING   // val
        , G_TYPE_POINTER  // rdata
    );

    // tree view:
    //
    dlg->tree_w_ = gtk_tree_view_new_with_model( GTK_TREE_MODEL(dlg->store_) );
    dlg->tree_v_ = GTK_TREE_VIEW( dlg->tree_w_ );
    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );

    // tree view columns:
    //
    dlg->ren_txt_ = gtk_cell_renderer_text_new();

    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_name(), "name" );
    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_val(),  "value" );



    // set show inherited:
    //
    dlg->showinh_ = TRUE;

    // load conf:
    //
    conf_load( dlg );

    // setup filter:
    //
    tree_filter_setup( dlg );




    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG(dlg) );



    gchar* cwd = g_get_current_dir();

    // TODO: set window's title elsewhere:
    //
    gtk_window_set_title( GTK_WINDOW( dlg ),
                          g_strdup_printf( "gedacfged - %s", cwd ) );



    // box (top):
    //
    GtkWidget* box_top = gtk_vbox_new( FALSE, 0 );


    // cwd label:
    //
    mk_labels_line( "<b>cwd: </b>", gtk_label_new( cwd ), box_top );


    // add box_top to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ),  box_top, FALSE, FALSE, 0 );




    // scrolled window for the tree:
    //
    GtkWidget* wscroll_tree = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_tree ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );

    gtk_container_add( GTK_CONTAINER( wscroll_tree ), dlg->tree_w_ );

    gtk_box_pack_start( GTK_BOX( ca ), wscroll_tree, TRUE, TRUE, 0 );




    // box (bottom):
    //
    GtkWidget* box_bot = gtk_vbox_new( FALSE, 5 );
//    GtkWidget* box_bot = gtk_vbox_new( TRUE, 0 );


    dlg->lab_ctx_ = gtk_label_new( NULL );
    mk_labels_line( "<b>ctx: </b>", dlg->lab_ctx_, box_bot );

    dlg->lab_fname_ = gtk_label_new( NULL );
    gtk_label_set_track_visited_links( GTK_LABEL( dlg->lab_fname_ ), FALSE );
    mk_labels_line( "<b>fname: </b>", dlg->lab_fname_, box_bot );

    mk_labels_line_separ( box_bot );

    dlg->lab_name_ = gtk_label_new( NULL );
    mk_labels_line( "<b>name: </b>", dlg->lab_name_, box_bot );

    dlg->lab_val_ = gtk_label_new( NULL );
    mk_labels_line( "<b>value: </b>", dlg->lab_val_, box_bot );

    mk_labels_line_separ( box_bot );


    // description text view:
    //
    GtkWidget* tv = gtk_text_view_new();

    gtk_text_view_set_editable( GTK_TEXT_VIEW( tv ), FALSE );
    gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( tv ), GTK_WRAP_WORD );

    dlg->txtbuf_desc_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( tv ) );
    gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );

    GtkWidget* wscroll_desc = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_desc ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );

    gtk_container_add( GTK_CONTAINER( wscroll_desc ), tv );
    gtk_box_pack_start( GTK_BOX( box_bot ), wscroll_desc, FALSE, FALSE, 0 );



    mk_labels_line_separ( box_bot );


    // 'show inh' check box:
    //
    GtkWidget* btn_showinh = gtk_check_button_new_with_mnemonic( "" );
    GtkWidget* lab_showinh = gtk_bin_get_child( GTK_BIN( btn_showinh ) );
    gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab_showinh ),
                                        "<i>sho_w inherited</i>" );

    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( btn_showinh ), dlg->showinh_ );

    gtk_box_pack_start( GTK_BOX( box_bot ), btn_showinh, FALSE, FALSE, 0 );


    // add box_bot to ca:
    //
    gtk_box_pack_start( GTK_BOX( ca ), box_bot, FALSE, FALSE, 0 );




    // action area:
    //
    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG(dlg) );

    // reload btn:
    //
    dlg->btn_reload_ = gtk_button_new_with_mnemonic( "_reload" );
    gtk_box_pack_start( GTK_BOX( aa ), dlg->btn_reload_, FALSE, FALSE, 0 );

    // tst btn:
    //
    GtkWidget* btn_tst = gtk_button_new_with_mnemonic( "_tst" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_tst, FALSE, FALSE, 0 );




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

    g_signal_connect( G_OBJECT( btn_tst ),
                      "clicked",
                      G_CALLBACK( &on_btn_tst ),
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
    GtkTreePath* path = gtk_tree_path_new_from_string( "1" );
    gtk_tree_view_expand_to_path( dlg->tree_v_, path );
    gtk_tree_view_set_cursor_on_cell( dlg->tree_v_, path, NULL, NULL, FALSE );
    gtk_tree_path_free( path );


    g_free( cwd );

} // mk_gui()

