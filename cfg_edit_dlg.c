#include "cfg_edit_dlg.h"
#include <liblepton/liblepton.h>
#include <liblepton/libgedaguile.h>

enum
{
    COL_NAME,
    COL_INH,
    COL_VAL,
    COL_EDITABLE, // hidden
    NUM_COLS
};



static void load_cfg( cfg_edit_dlg* dlg );



static int colid_name()     { return COL_NAME; }
static int colid_inh()      { return COL_INH; }
static int colid_val()      { return COL_VAL; }
static int colid_editable() { return COL_EDITABLE; }
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
         gboolean      editable,
         GtkTreeIter*  it_parent )
{
    GtkTreeIter it;
    gtk_tree_store_append( dlg->store_, &it, it_parent );
    gtk_tree_store_set( dlg->store_,
                        &it,
                        colid_name(),     name,
                        colid_inh(),      inh,
                        colid_val(),      val,
                        colid_editable(), editable,
                        -1 );

    gtk_tree_view_expand_all( dlg->tree_v_ );

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
    printf( "cfg_edit_dlg::cfg_edit_dlg_on_btn_apply()\n" );
}



static void
cfg_edit_dlg_on_btn_edit( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );

    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, NULL, &it );

    if ( res )
    {
        gchar* name = NULL;
        gtk_tree_model_get( dlg->model_, &it, colid_name(), &name, -1 );

        gboolean editable = FALSE;
        gtk_tree_model_get( dlg->model_, &it, colid_editable(), &editable, -1 );

        printf( "cfg_edit_dlg::cfg_edit_dlg_on_btn_edit(): %s [%d]\n",
                name,
                editable );

        g_free( name );


        if ( editable )
        {
            gchar* val = NULL;
            gtk_tree_model_get( dlg->model_, &it, colid_val(), &val, -1 );
            printf( "    == [%s]\n", val );
        }

    }

}



/*
 * *****************************************************************
 *
 *  gobject-specific stuff:
 *
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
        G_TYPE_STRING
        , G_TYPE_BOOLEAN
        , G_TYPE_STRING
        , G_TYPE_BOOLEAN
        , G_TYPE_BOOLEAN
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

//    GtkTreeIter it = add_row( dlg, "[name]", "[val]", NULL );
//    it = add_row( dlg, "123", "456", &it );


    load_cfg( dlg );


    // collapse 0 level, expand 1st level:
    //
    gtk_tree_view_collapse_all( dlg->tree_v_ );

    GtkTreePath* path0 = gtk_tree_path_new_from_string( "0" );
    if ( path0 )
        gtk_tree_view_expand_row( dlg->tree_v_, path0, FALSE );

    GtkTreePath* path1 = gtk_tree_path_new_from_string( "1" );
    if ( path1 )
        gtk_tree_view_expand_row( dlg->tree_v_, path1, FALSE );


    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG(dlg) );

    GtkWidget* lab = gtk_label_new( "eklmn!" );
    gtk_box_pack_start( GTK_BOX( ca ), lab, FALSE, FALSE, 0 );

    GtkWidget* wscroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll ),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll ), dlg->tree_w_ );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll, TRUE, TRUE, 0 );


    // action area:
    //
    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG(dlg) );

    GtkWidget* btn_apply = gtk_button_new_with_mnemonic( "_apply" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_apply, FALSE, FALSE, 0 );

    GtkWidget* btn_edit = gtk_button_new_with_mnemonic( "_edit" );
    gtk_box_pack_start( GTK_BOX( aa ), btn_edit, FALSE, FALSE, 0 );


    gtk_widget_show_all( GTK_WIDGET(dlg) );

    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &cfg_edit_dlg_on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( btn_apply ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_apply ),
                      NULL );

    g_signal_connect( G_OBJECT( btn_edit ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_edit ),
                      dlg );
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
//        printf( "    [%s]\n", name );

        gchar* val = eda_config_get_string( ctx, group, name, &err );
        if ( val == NULL )
        {
            printf( " >> load_keys( %s ): !eda_config_get_string( %s )\n", group, name );
            if ( err != NULL )
            {
                printf( "    err: %s\n", err->message );
            }
//            val = g_strdup( "[err]" );
            continue;
        }


        gboolean inh = eda_config_is_inherited( ctx, group, name, &err );
        if ( err != NULL )
        {
            printf( " >> load_keys(): !eda_config_is_inherited()\n" );
            printf( "    err: %s\n", err->message );
        }
        g_clear_error( &err );

        gboolean editable = file_writable;

        add_row( dlg, name, inh, val, editable, itParent );

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

    printf( " >> num of grps: <%lu>\n", len );

    for ( gsize ndx = 0; ndx < len; ++ndx )
    {
        const gchar* name = pp[ ndx ];
//        printf( "  <%s>\n", name );

        if ( strstr( name, "dialog-geometry" ) == NULL )
        {
            GtkTreeIter it = add_row( dlg, name, FALSE, "", FALSE, itParent );
            load_keys( ctx, name, dlg, &it, file_writable );
        }
    }

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

    GtkTreeIter it = add_row( dlg, name, inh, str, FALSE, NULL );

    load_groups( ctx, fname, dlg, &it, wok );
}



static void
load_cfg( cfg_edit_dlg* dlg )
{
    load_ctx( eda_config_get_default_context(),       "DEFAULT",  dlg );
    load_ctx( eda_config_get_system_context(),        "SYSTEM",   dlg );
    load_ctx( eda_config_get_user_context(),          "USER",     dlg );
    load_ctx( eda_config_get_context_for_path( "." ), "PATH [.]", dlg );
}

