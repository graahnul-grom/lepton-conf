#include "cfg_edit_dlg.h"
#include <liblepton/liblepton.h>
#include <liblepton/libgedaguile.h>

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
mk_data( EdaConfig*   ctx,
         const gchar* group,
         const gchar* key,
         const gchar* val,
         gboolean     ro )
{
    row_data* data = g_malloc( sizeof( row_data ) );

    data->ctx_   = ctx;
    data->group_ = group ? g_strdup( group ) : NULL;
    data->key_   = key   ? g_strdup( key )   : NULL;
    data->val_   = val   ? g_strdup( val )   : NULL;
    data->ro_    = ro;

    return data;
}



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



static gboolean
cur_row_get_fields( cfg_edit_dlg* dlg, // GtkTreeView* tree,
                    gchar**      name,
                    gchar**      val,
                    gboolean*    editable,
                    row_data**   data );
static void
cur_row_set_fields_val( cfg_edit_dlg* dlg,
                        const gchar*  val );



static void load_cfg( cfg_edit_dlg* dlg );



static int colid_name()     { return COL_NAME; }
static int colid_inh()      { return COL_INH; }
static int colid_val()      { return COL_VAL; }
static int colid_data()     { return COL_DATA; }
static int cols_cnt()       { return NUM_COLS; }


static void
add_col( GtkTreeView*     tree,
         GtkCellRenderer* ren,
         const gchar*     prop,
         gint             col_id,
         const gchar*     title )
{
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, title );
    gtk_tree_view_column_pack_start( col, ren, TRUE );
    gtk_tree_view_column_add_attribute( col, ren, prop, col_id );
    gtk_tree_view_append_column( tree, col );
}



static GtkTreeIter
add_row( cfg_edit_dlg* dlg,
         const gchar*  name,
         gboolean      inh,
         const gchar*  val,
         gpointer      data,
         GtkTreeIter*  it_parent )
{
    GtkTreeIter it;
    gtk_tree_store_append( dlg->store_, &it, it_parent );
    gtk_tree_store_set( dlg->store_,
                        &it,
                        colid_name(),     name,
                        colid_inh(),      inh,
                        colid_val(),      val,
                        colid_data(),     data,
                        -1 );

//    gtk_tree_view_expand_all( dlg->tree_v_ );

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
cfg_edit_dlg_on_delete_event( GtkWidget* dlg, GdkEvent* e, gpointer* data )
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

    if ( !gtk_editable_get_editable( GTK_EDITABLE( ent ) ) )
        return;

    const gchar* txt = gtk_entry_get_text( ent );


    gchar*    name     = NULL;  // uu
    gchar*    val      = NULL;  // uu
    gboolean  editable = FALSE; // uu
    row_data* rdata    = NULL;

    gboolean res = cur_row_get_fields( dlg, &name, &val, &editable, &rdata );
//    gboolean res = cur_row_get_fields( dlg->tree_v_, &name, &val, &editable, &rdata );
    if ( !res )
        return;

    g_free( name );
    g_free( val );


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
    res = eda_config_save( rdata->ctx_, &err );
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


    cur_row_set_fields_val( dlg, new_val );

    // NOTE: update data
    //
//    g_free( rdata->val_ );
    rdata->val_ = g_strdup( new_val );


} // cfg_edit_dlg_on_btn_apply()



static void
cur_row_set_fields_val( cfg_edit_dlg* dlg,
//                    const gchar*  name,
                    const gchar*  val )
                    // const row_data* data )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, NULL, &it );
    if ( !res )
    {
        printf( " >> >> cur_row_set_fields(): !sel\n");
        return;
    }
//    GtkTreeModel* model = gtk_tree_view_get_model( tree );


    gtk_tree_store_set( dlg->store_,
                        &it,
//                        colid_name(),     name,
//                        colid_inh(),      inh,
                        colid_val(),      val,
//                        colid_data(),     data,
                        -1 );
}



// {post}: caller must free [name], [val]
//
static gboolean
cur_row_get_fields( cfg_edit_dlg* dlg, // GtkTreeView* tree,
                    gchar**      name,
                    gchar**      val,
                    gboolean*    editable,
                    row_data**   data )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, NULL, &it );
    if ( !res )
    {
        printf( " >> >> cur_row_get_fields(): !sel\n");
        return FALSE;
    }
//    GtkTreeModel* model = gtk_tree_view_get_model( tree );

    gchar* n = NULL;
    gtk_tree_model_get( dlg->model_, &it, colid_name(), &n, -1 );
    *name = n;

    gchar* v = NULL;
    gtk_tree_model_get( dlg->model_, &it, colid_val(), &v, -1 );
    *val = v;

    row_data* ptr = NULL;
    gtk_tree_model_get( dlg->model_, &it, colid_data(), &ptr, -1 );

    *editable = ptr ? !ptr->ro_ : FALSE;

    *data = ptr;

    return TRUE;
}



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

    gchar*    name     = NULL;
    gchar*    val      = NULL;
    gboolean  editable = FALSE;
    row_data* rdata    = NULL;

    gboolean res = cur_row_get_fields( dlg, &name, &val, &editable, &rdata );
//    gboolean res = cur_row_get_fields( tree, &name, &val, &editable, &rdata );
    if ( !res )
        return;

    gtk_entry_set_text( ent, val );
    gtk_editable_set_editable( GTK_EDITABLE( ent ), editable );
    gtk_widget_set_sensitive( dlg->btn_apply_, editable );

    g_free( name );
    g_free( val );

} // cfg_edit_dlg_on_row_sel()



static void
cfg_edit_dlg_on_btn_reload( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gtk_tree_store_clear( dlg->store_ );
    load_cfg( dlg );
}



static void
cfg_edit_dlg_on_btn_exted( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

//    gtk_tree_store_clear( dlg->store_ );
//    load_cfg( dlg );
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
        , G_TYPE_POINTER  // data
    );

    dlg->model_ = GTK_TREE_MODEL( dlg->store_ );


    // view:
    //
    dlg->tree_w_ = gtk_tree_view_new_with_model( dlg->model_ );
    dlg->tree_v_ = GTK_TREE_VIEW( dlg->tree_w_ );
    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );


    // tree view columns:
    //
    GtkCellRenderer* ren_text = gtk_cell_renderer_text_new();
    GtkCellRenderer* ren_bool = gtk_cell_renderer_toggle_new();

    add_col( dlg->tree_v_, ren_text, "text",   colid_name(), "name" );
    add_col( dlg->tree_v_, ren_bool, "active", colid_inh(),  "inherited" );
    add_col( dlg->tree_v_, ren_text, "text",   colid_val(),  "value" );


    load_cfg( dlg );


//    gtk_tree_view_collapse_all( dlg->tree_v_ );
    GtkTreePath* path0 = gtk_tree_path_new_from_string( "2" );
    if ( path0 )
        gtk_tree_view_expand_row( dlg->tree_v_, path0, FALSE );

    GtkTreePath* path1 = gtk_tree_path_new_from_string( "3" );
    if ( path1 )
        gtk_tree_view_expand_row( dlg->tree_v_, path1, FALSE );


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
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll ), dlg->tree_w_ );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll, TRUE, TRUE, 0 );


    // box:
    //
    GtkWidget* box = gtk_hbox_new( FALSE, 0 );

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
    GtkWidget* btn_reload = gtk_button_new_with_mnemonic( "_reload" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_reload, FALSE, FALSE, 0 );

    // ext ed btn:
    //
    GtkWidget* btn_exted = gtk_button_new_with_mnemonic( "_ext ed" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_exted, FALSE, FALSE, 0 );


    gtk_widget_show_all( GTK_WIDGET(dlg) );


    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &cfg_edit_dlg_on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( dlg->btn_apply_ ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_apply ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_exted ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_exted ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_reload ),
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
           GtkTreeIter*  itParent,
           gboolean      file_writable )
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

        // NOTE: data:
        //
        row_data* data = mk_data( ctx,
                                  group,         // group
                                  name,          // key
                                  val,           // val
                                  !file_writable // ro
                                );

        add_row( dlg,
                 name,
                 inh,
                 val,
                 data,
                 itParent );

        g_free( val );

        // if key is inherited, also mark group as inherited:
        //
        if ( inh )
        {
            gtk_tree_store_set( dlg->store_, itParent, colid_inh(), TRUE, -1 );
        }

    } // for keys

    g_strfreev( pp );

} // load_keys()



static void
load_groups( EdaConfig*    ctx,
             const gchar*  fname,
             cfg_edit_dlg* dlg,
             GtkTreeIter*  itParent,
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

        GtkTreeIter it = add_row( dlg,
                                  name,    // name
                                  FALSE,   // inh
                                  "",      // val
                                  NULL,    // data
                                  itParent
                                );

        load_keys( ctx, name, dlg, &it, file_writable );

    } // for groups

    g_strfreev( pp );

} // load_groups()



static void
load_ctx( EdaConfig* ctx, const gchar* name, cfg_edit_dlg* dlg )
{
    const gchar* fname = eda_config_get_filename( ctx );

    gboolean wok = FALSE;
    gchar str[ PATH_MAX ] = "";

    if ( fname != NULL )
    {
        gboolean exist = access( fname, F_OK ) == 0;
        gboolean rok = access( fname, R_OK ) == 0;
        wok = access( fname, W_OK ) == 0;
        sprintf( str, "[%s%s%s] %s",
                 exist ? "f" : "-",
                 rok ? "r" : "-",
                 wok ? "w" : "-",
                 fname );
    }

    gboolean inh = eda_config_get_parent( ctx ) != NULL;

    GtkTreeIter it = add_row( dlg,
                              name,  // name
                              inh,   // inh
                              str,   // val
                              NULL,  // data
                              NULL
                            );

    load_groups( ctx, fname, dlg, &it, wok );

} // load_ctx()



static void
load_cfg( cfg_edit_dlg* dlg )
{
    load_ctx( eda_config_get_default_context(),       "DEFAULT",  dlg );
    load_ctx( eda_config_get_system_context(),        "SYSTEM",   dlg );
    load_ctx( eda_config_get_user_context(),          "USER",     dlg );
    load_ctx( eda_config_get_context_for_path( "." ), "PATH [.]", dlg );
}

