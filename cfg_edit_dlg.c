#include "cfg_edit_dlg.h"

enum
{
    COL_1_NAME,
    COL_2_VAL,
    NUM_COLS
};



static int colid_name()  { return COL_1_NAME; }
static int colid_val()   { return COL_2_VAL; }
static int cols_cnt()    { return NUM_COLS; }

static void col_add( GtkTreeView* tree, GtkCellRenderer* ren,
              const gchar* prop, gint col_id, const gchar* title )
{
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, title );
    gtk_tree_view_column_pack_start( col, ren, TRUE );
    gtk_tree_view_column_add_attribute( col, ren, prop, col_id );
    gtk_tree_view_append_column( tree, col );
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
cfg_edit_dlg_on_btn_apply( GtkButton* dlg, gpointer* p )
{
    printf( "cfg_edit_dlg::cfg_edit_dlg_on_btn_apply()\n" );
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
        G_TYPE_STRING,
        G_TYPE_STRING
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

    col_add( dlg->tree_v_, ren_text, "text",   colid_name(), "cfg name" );


    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG(dlg) );

    GtkWidget* lab = gtk_label_new( "eklmn!" );
    gtk_box_pack_start( GTK_BOX( ca ), lab, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), dlg->tree_w_, FALSE, FALSE, 0 );


    // action area:
    //
    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG(dlg) );

    GtkWidget* btn = gtk_button_new_with_mnemonic( "_apply" );
    gtk_box_pack_start( GTK_BOX( aa ), btn, FALSE, FALSE, 0 );




    gtk_widget_show_all( GTK_WIDGET(dlg) );

    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &cfg_edit_dlg_on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( btn ),
                      "clicked",
                      G_CALLBACK( &cfg_edit_dlg_on_btn_apply ),
                      NULL );
}

