/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "attrs_dlg.h"
#include "proto.h"

static void
attrs_dlg_create( AttrsDlg* dlg );

static void
attrs_dlg_items_add( AttrsDlg* dlg );



G_DEFINE_TYPE(AttrsDlg, attrs_dlg, GTK_TYPE_DIALOG);



// {ret}: attrs list as a string; NULL if attrs list is empty
//
static gchar*
attrs_dlg_result_to_string( AttrsDlg* dlg )
{
    GList* items = dlg->items_result_;

    if ( items == NULL )
        return NULL;

    if ( g_list_length( items ) == 0 )
        return NULL;


    // to avoid leading ';' in the result string:
    //
    gchar* res = (gchar*) items->data;

    for ( GList* p = items->next; p != NULL; p = p->next )
    {
        const gchar* str = (const gchar*) p->data;
        printf( " .. attrs_dlg_result_to_string(): str [%s]\n", str );

        res = g_strdup_printf( "%s;%s", res, str );
    }

    return res;

} // attrs_dlg_result_to_string()



static gboolean
attrs_dlg_add_to_result( GtkTreeModel* mod,
                         GtkTreePath*  path,
                         GtkTreeIter*  it,
                         gpointer      p )
{
    AttrsDlg* dlg = (AttrsDlg*) p;

    gchar* str = NULL;
    gtk_tree_model_get( mod, it, 0, &str, -1 );
    dlg->items_result_ = g_list_append( dlg->items_result_, str );

    printf( " .. add_to_result(): str: [%s]\n", str );

    return FALSE; // FALSE => continue gtk_tree_model_foreach()

} // attrs_dlg_add_to_result()



static GtkWidget*
attrs_dlg_new()
{
  gpointer obj = g_object_new( ATTRS_DLG_TYPE, NULL );
  return GTK_WIDGET( obj );
}



gchar*
attrs_dlg_run( GList* items )
{
    GtkWidget* dlg = attrs_dlg_new();
    AttrsDlg* adlg = ATTRS_DLG( dlg );

    adlg->items_result_ = NULL;
    adlg->items_ = items;
    attrs_dlg_items_add( adlg );


    // select 1st elem in the tree view:
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( adlg->tree_v_ );
    GtkTreeIter it;
    gboolean is_iter_set = gtk_tree_model_get_iter_first( mod, &it );
    if ( is_iter_set )
    {
        GtkTreeSelection* sel = gtk_tree_view_get_selection( adlg->tree_v_ );
        gtk_tree_selection_select_iter( sel, &it );
    }


    gint resp = gtk_dialog_run( GTK_DIALOG( dlg ) );
    if ( resp != GTK_RESPONSE_ACCEPT )
    {
        return NULL;
    }

    GtkTreeModel* mod_res = gtk_tree_view_get_model( adlg->tree_v_ );
    gtk_tree_model_foreach( mod_res, &attrs_dlg_add_to_result, dlg );

    gchar* ret = attrs_dlg_result_to_string( adlg );

    gtk_widget_destroy( dlg );

    return ret;

} // attrs_dlg_run()



static void
attrs_dlg_class_init( AttrsDlgClass* cls )
{
}



static void
attrs_dlg_init( AttrsDlg* dlg )
{
    printf( " ++ attrs_dlg_init()\n" );
    attrs_dlg_create( dlg );
}



static void
attrs_dlg_on_btn_ok( GtkWidget* btn, gpointer p )
{
    printf( " .. attrs_dlg_on_btn_ok()\n" );

    AttrsDlg* dlg = (AttrsDlg*) p;
    gtk_dialog_response( GTK_DIALOG( dlg ), GTK_RESPONSE_ACCEPT );
}



static void
attrs_dlg_on_btn_cancel( GtkWidget* btn, gpointer p )
{
    printf( " .. attrs_dlg_on_btn_cancel()\n" );

    AttrsDlg* dlg = (AttrsDlg*) p;
    gtk_dialog_response( GTK_DIALOG( dlg ), GTK_RESPONSE_REJECT );
}



static void
attrs_dlg_on_btn_remove( GtkWidget* btn, gpointer p )
{
    printf( " .. attrs_dlg_on_btn_remove()\n" );
    AttrsDlg* dlg = (AttrsDlg*) p;

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeModel* mod = NULL;
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, &mod, &it );

    if ( !res )
    {
        printf( " .. attrs_dlg_on_btn_remove(): !sel\n" );
        return;
    }

    gchar* str = NULL;
    gtk_tree_model_get( mod, &it, 0, &str, -1 );
    printf( " .. attrs_dlg_on_btn_remove(): str: [%s]\n", str );

    gboolean removed = gtk_list_store_remove( GTK_LIST_STORE( mod ), &it );
    printf( " .. .. attrs_dlg_on_btn_remove(): removed: [%d]\n", removed );

} // attrs_dlg_on_btn_remove()



static void
attrs_dlg_on_btn_add( GtkWidget* btn, gpointer p )
{
    printf( " .. on_btn_add()\n" );
    AttrsDlg* dlg = (AttrsDlg*) p;

    gchar* str = run_dlg_edit_val( NULL, "txt", "title" );
    if ( str != NULL )
    {
        printf( " .. attrs_dlg_on_btn_add(): str: [%s]\n", str );

        GtkTreeIter it;
        gtk_list_store_append( dlg->store_, &it );
        gtk_list_store_set( dlg->store_, &it, 0, str, -1 );
    }

} // attrs_dlg_on_btn_add()



// TODO: attrs_dlg_on_btn_move_down(): rewrite using GtkTreePath
//
static void
attrs_dlg_on_btn_move_down( GtkWidget* btn, gpointer p )
{
    printf( " .. attrs_dlg_on_btn_move_down()\n" );
    AttrsDlg* dlg = (AttrsDlg*) p;

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeModel* mod = NULL;
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, &mod, &it );

    if ( !res )
    {
        printf( " .. attrs_dlg_on_btn_move_down(): !sel\n" );
        return;
    }

    gchar* str = NULL;
    gtk_tree_model_get( mod, &it, 0, &str, -1 );
    printf( " .. attrs_dlg_on_btn_move_down(): str: [%s]\n", str );

    GtkTreeIter* it_old = gtk_tree_iter_copy( &it );
    gboolean has_next = gtk_tree_model_iter_next( mod, &it );

    if ( has_next )
    {
        gtk_list_store_move_after( dlg->store_, it_old, &it );
    }
    else
    {
        gtk_list_store_move_after( dlg->store_, it_old, NULL );
    }

} // attrs_dlg_on_btn_move_down()



static void
attrs_dlg_on_btn_move_up( GtkWidget* btn, gpointer p )
{
    printf( " .. attrs_dlg_on_btn_move_up()\n" );
    AttrsDlg* dlg = (AttrsDlg*) p;

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeModel* mod = NULL;
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, &mod, &it );

    if ( !res )
    {
        printf( " .. attrs_dlg_on_btn_move_up(): !sel\n" );
        return;
    }

    gchar* str = NULL;
    gtk_tree_model_get( mod, &it, 0, &str, -1 );
    printf( " .. attrs_dlg_on_btn_move_up(): str: [%s]\n", str );

    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );
    gboolean move_ok = gtk_tree_path_prev( path );
    printf( " .. .. attrs_dlg_on_btn_move_up(): moved_ok: [%d]\n", move_ok );

    if ( move_ok )
    {
        GtkTreeIter it_prev;
        gboolean iter_ok = gtk_tree_model_get_iter( mod, &it_prev, path );
        printf( " .. .. attrs_dlg_on_btn_move_up(): iter_ok: [%d]\n", iter_ok );

        if ( iter_ok )
        {
            gtk_list_store_move_before( dlg->store_, &it, &it_prev );
        }
    }
    else
    {
        gtk_list_store_move_before( dlg->store_, &it, NULL );
    }

} // attrs_dlg_on_btn_move_up()



static void
attrs_dlg_create( AttrsDlg* dlg )
{
    // list store:
    //
    dlg->store_ = gtk_list_store_new( 1, G_TYPE_STRING );
    GtkTreeModel* model = GTK_TREE_MODEL( dlg->store_ );


    // tree view:
    //
    GtkWidget* tree_w = gtk_tree_view_new_with_model( model );
    dlg->tree_v_ = GTK_TREE_VIEW( tree_w );

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    gtk_tree_selection_set_mode( sel, GTK_SELECTION_BROWSE );


    // column:
    //
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, "Attributes" );

    GtkCellRenderer* ren = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start( col, ren, TRUE );
    gtk_tree_view_column_add_attribute( col, ren, "text", 0 );

    gtk_tree_view_append_column( dlg->tree_v_, col );
    gtk_tree_view_set_headers_visible( dlg->tree_v_, FALSE );


    // scrolled:
    //
    GtkWidget* wscroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll ), tree_w );


    // pack:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( dlg ) );
    GtkWidget* hbox = gtk_hbox_new (FALSE, 0);

    gtk_box_pack_start( GTK_BOX( ca ), hbox, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), wscroll, TRUE, TRUE, 0 );


    // action area, OK, Cancel buttons:
    //
    GtkWidget* btn_ok = gtk_button_new_with_mnemonic( "_OK" );
    GtkWidget* btn_cancel = gtk_button_new_with_mnemonic( "_Cancel" );

    GtkWidget* aa = gtk_dialog_get_action_area( GTK_DIALOG( dlg ) );
    gtk_box_pack_start( GTK_BOX( aa ), btn_ok, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( aa ), btn_cancel, TRUE, TRUE, 0 );

    g_signal_connect( G_OBJECT( btn_ok ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_ok ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_cancel ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_cancel ),
                      dlg );


    // buttons:
    //
    GtkWidget* vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start( GTK_BOX( hbox ), vbox, TRUE, TRUE, 0 );

    GtkWidget* btn_edit      = gtk_button_new_with_mnemonic( "_Edit" );
    GtkWidget* btn_add       = gtk_button_new_with_mnemonic( "_Add" );
    GtkWidget* btn_remove    = gtk_button_new_with_mnemonic( "_Remove" );
    GtkWidget* btn_move_up   = gtk_button_new_with_mnemonic( "Move _Up" );
    GtkWidget* btn_move_down = gtk_button_new_with_mnemonic( "Move _Down" );

    gtk_box_pack_start( GTK_BOX( vbox ), btn_edit,      FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), btn_add,       FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), btn_remove,    FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), btn_move_up,   FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), btn_move_down, FALSE, FALSE, 3 );

    g_signal_connect( G_OBJECT( btn_remove ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_remove ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_add ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_add ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_move_down ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_move_down ),
                      dlg );

    g_signal_connect( G_OBJECT( btn_move_up ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_move_up ),
                      dlg );

    gtk_widget_show_all( GTK_WIDGET( dlg ) );

} // attrs_dlg_create()



static void
attrs_dlg_items_add( AttrsDlg* dlg )
{
    if ( dlg->items_ == NULL )
    {
        printf( " >> attrs_dlg_items_add(): NOP 1\n" );
        return;
    }

    if ( g_list_length( dlg->items_ ) == 0 )
    {
        printf( " >> attrs_dlg_items_add(): NOP 2\n" );
        return;
    }

    for ( GList* p = dlg->items_; p != NULL; p = p->next )
    {
        const gchar* str = (const gchar*) p->data;
        printf( " >> attrs_dlg_items_add(): str: [%s]\n", str );

        GtkTreeIter it;
        gtk_list_store_append( dlg->store_, &it );
        gtk_list_store_set( dlg->store_, &it, 0, str, -1 );
    }

} // attrs_dlg_items_add()



void
attrs_dlg_dbg_print_items( GList* items )
{
    printf( " .. attrs_dlg_dbg_print_items()\n" );

    if ( items == NULL )
    {
        printf( " .. items: NULL\n" );
        return;
    }

    if ( g_list_length( items ) == 0 )
    {
        printf( " .. items: 0\n" );
        return;
    }

    printf( " .. items:\n" );

    for ( GList* p = items; p != NULL; p = p->next )
    {
        const gchar* str = (const gchar*) p->data;
        printf( " .. .. [ %s ]\n", str );
    }

} // attrs_dlg_dbg_print_items()

