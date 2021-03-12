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



static gchar*
attrs_dlg_result_to_string( AttrsDlg* dlg )
{
    GList* items = dlg->items_result_;

    if ( items == NULL )
        return g_strdup( "" );

    if ( g_list_length( items ) == 0 )
        return g_strdup( "" );


    // to avoid leading ';' in the result string:
    //
    gchar* res = (gchar*) items->data;

    for ( GList* p = items->next; p != NULL; p = p->next )
    {
        const gchar* str = (const gchar*) p->data;
        printf( " .. attrs_dlg_result_to_string(): str [%s]\n", str );

        gchar* res_prev = res;

        res = g_strdup_printf( "%s;%s", res, str );

        if ( p->next != NULL )
            g_free( res_prev );
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



static void
attrs_dlg_update_ui( AttrsDlg* dlg )
{
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeModel* mod = NULL;
    GtkTreeIter it;

    const gboolean selected = gtk_tree_selection_get_selected( sel, &mod, &it );
    gint count = gtk_tree_model_iter_n_children( mod, NULL );

    gtk_widget_set_sensitive( dlg->btn_edit_,      selected );
    gtk_widget_set_sensitive( dlg->btn_remove_,    selected );
    gtk_widget_set_sensitive( dlg->btn_move_down_, selected && count > 1 );
    gtk_widget_set_sensitive( dlg->btn_move_up_,   selected && count > 1 );

} // attrs_dlg_update_ui()



static gboolean
duplicate_attr_check_and_warn( AttrsDlg* dlg, gchar* str )
{
    gchar* stripped = g_strstrip( str );

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreeIter it;
    gboolean next = gtk_tree_model_get_iter_first( mod, &it );

    gboolean found = FALSE;
    for ( ; next; next = gtk_tree_model_iter_next( mod, &it ) )
    {
        gchar* str = NULL;
        gtk_tree_model_get( mod, &it, 0, &str, -1 );
        // printf( " .. duplicate_attr_check_and_warn(): str: [%s]\n", str );

        if ( g_strcmp0( str, stripped ) == 0 )
        {
            found = TRUE;

            GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
            gtk_tree_selection_select_iter( sel, &it );
            GtkTreePath* path = gtk_tree_model_get_path( mod, &it );
            gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, FALSE, 0, 0 );

            break;
        }
    }


    if ( found )
    {
        GtkWidget* msgdlg =
        gtk_message_dialog_new( GTK_WINDOW( dlg ),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_WARNING,
                                GTK_BUTTONS_OK,
                                "Attribute already exists." );

        gtk_window_set_title( GTK_WINDOW( msgdlg ), "lepton-conf" );

        gtk_dialog_run( GTK_DIALOG( msgdlg ) );
        gtk_widget_destroy( msgdlg );
    }

    return !found;

} // duplicate_attr_check_and_warn()



static GtkWidget*
attrs_dlg_new()
{
  gpointer obj = g_object_new( ATTRS_DLG_TYPE, NULL );
  return GTK_WIDGET( obj );
}



gchar*
attrs_dlg_run( GList* items, const gchar* title )
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


    if ( title != NULL )
        gtk_window_set_title( GTK_WINDOW( dlg ), title );

    attrs_dlg_update_ui( adlg );
    gint resp = gtk_dialog_run( GTK_DIALOG( dlg ) );

    if ( resp != GTK_RESPONSE_ACCEPT )
    {
        gtk_widget_destroy( dlg );
        return NULL;
    }

    GtkTreeModel* mod_res = gtk_tree_view_get_model( adlg->tree_v_ );
    gtk_tree_model_foreach( mod_res, &attrs_dlg_add_to_result, dlg );

    gchar* ret = attrs_dlg_result_to_string( adlg );

    g_list_free( adlg->items_result_ );
    adlg->items_result_ = NULL;

    gtk_widget_destroy( dlg );

    return ret;

} // attrs_dlg_run()



static void
attrs_dlg_on_show( GtkWidget* widget )
{
    AttrsDlg* dlg = ATTRS_DLG( widget );

    settings_restore_wnd_geom( GTK_WINDOW( dlg ), "attrs" );

    GTK_WIDGET_CLASS(attrs_dlg_parent_class)->show(widget);
}



static void
attrs_dlg_on_unmap( GtkWidget* widget )
{
    AttrsDlg* dlg = ATTRS_DLG( widget );

    settings_save_wnd_geom( GTK_WINDOW( dlg ), "attrs" );

    GTK_WIDGET_CLASS(attrs_dlg_parent_class)->unmap(widget);
}



static void
attrs_dlg_class_init( AttrsDlgClass* cls )
{
    GtkWidgetClass* wcls = GTK_WIDGET_CLASS( cls );

    wcls->show =  &attrs_dlg_on_show;
    wcls->unmap = &attrs_dlg_on_unmap;
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


    GtkTreeIter* it_next = gtk_tree_iter_copy( &it );
    gboolean has_next = gtk_tree_model_iter_next( mod, it_next );

    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );
    gboolean has_prev = gtk_tree_path_prev( path );


    // remove:
    //
    gboolean removed = gtk_list_store_remove( GTK_LIST_STORE( mod ), &it );
    printf( " .. .. >> attrs_dlg_on_btn_remove(): removed: [ %d ]\n", removed );
    //
    //


    if ( has_next )
    {
        gtk_tree_selection_select_iter( sel, it_next );
    }
    else
    if ( has_prev )
    {
        GtkTreeIter it_prev;
        gboolean prev_ok = gtk_tree_model_get_iter( mod, &it_prev, path );

        if ( prev_ok )
            gtk_tree_selection_select_iter( sel, &it_prev );
        else
            printf( " .. .. attrs_dlg_on_btn_remove(): ! prev_ok !\n" );
    }
    else
    {
        printf( " .. .. attrs_dlg_on_btn_remove(): LIST E M P T Y\n" );
    }

    attrs_dlg_update_ui( dlg );

} // attrs_dlg_on_btn_remove()



static gboolean
empty_string_check_and_warn( AttrsDlg* dlg, gchar* str )
{
    gchar* stripped = g_strstrip( str );

    if ( strlen( stripped ) > 0 )
    {
        return TRUE;
    }


    GtkWidget* msgdlg =
    gtk_message_dialog_new( GTK_WINDOW( dlg ),
                            GTK_DIALOG_MODAL,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "Please enter a non-empty string." );

    gtk_window_set_title( GTK_WINDOW( msgdlg ), "lepton-conf" );

    gtk_dialog_run( GTK_DIALOG( msgdlg ) );
    gtk_widget_destroy( msgdlg );

    return FALSE;

} // empty_string_check_and_warn()





static void
attrs_dlg_on_btn_add( GtkWidget* btn, gpointer p )
{
    AttrsDlg* dlg = (AttrsDlg*) p;

    gchar* str_new = run_dlg_edit_val( GTK_WINDOW( dlg ),
                                       "my-attr",
                                       "Add attribute" );

    if ( str_new != NULL )
    {
        printf( " .. attrs_dlg_on_btn_add(): str_new: [%s]\n", str_new );

        if ( empty_string_check_and_warn( dlg, str_new ) &&
             duplicate_attr_check_and_warn( dlg, str_new ) )
        {
            GtkTreeIter it;
            gtk_list_store_append( dlg->store_, &it );
            gtk_list_store_set( dlg->store_, &it, 0, str_new, -1 );


            // select added and scroll to it:
            //
            GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
            gtk_tree_selection_select_iter( sel, &it );

            GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
            GtkTreePath* path = gtk_tree_model_get_path( mod, &it );
            gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, FALSE, 0, 0 );
        }

        g_free( str_new );
    }

    attrs_dlg_update_ui( dlg );

} // attrs_dlg_on_btn_add()



static void
attrs_dlg_on_btn_edit( GtkWidget* btn, gpointer p )
{
    AttrsDlg* dlg = (AttrsDlg*) p;

    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    GtkTreeModel* mod = NULL;
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, &mod, &it );

    if ( !res )
    {
        printf( " .. attrs_dlg_on_btn_edit(): !sel\n" );
        return;
    }

    gchar* str = NULL;
    gtk_tree_model_get( mod, &it, 0, &str, -1 );


    gchar* str_new = run_dlg_edit_val( GTK_WINDOW( dlg ), str, NULL );

    if ( str_new != NULL )
    {
        printf( " .. attrs_dlg_on_btn_edit(): str_new: [%s]\n", str_new );

        // NOTE: empty_string_check_and_warn() strips leading and trailing spaces:
        //
        if ( empty_string_check_and_warn( dlg, str_new ) )
        {
            printf( " .. attrs_dlg_on_btn_edit(): str_new: [%s]\n", str_new );

            gtk_list_store_set( dlg->store_, &it, 0, str_new, -1 );
        }

        g_free( str_new );
    }

} // attrs_dlg_on_btn_edit()



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
        // move to the beginning of the list:
        //
        gtk_list_store_move_after( dlg->store_, it_old, NULL );
    }


    // select moved node and scroll to it:
    //
    gtk_tree_selection_select_iter( sel, it_old );
    GtkTreePath* path = gtk_tree_model_get_path( mod, it_old );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, FALSE, 0, 0 );

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
        // move to the end of the list:
        //
        gtk_list_store_move_before( dlg->store_, &it, NULL );
    }


    // select moved node and scroll to it:
    //
    gtk_tree_selection_select_iter( sel, &it );
    path = gtk_tree_model_get_path( mod, &it );
    gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, FALSE, 0, 0 );

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


    // content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( dlg ) );
    GtkWidget* hbox = gtk_hbox_new (FALSE, 0);

    gtk_box_pack_start( GTK_BOX( ca ), hbox, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), wscroll, TRUE, TRUE, 0 );


    // action area: OK, Cancel buttons:
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
    gtk_box_pack_start( GTK_BOX( hbox ), vbox, FALSE, FALSE, 0 );

    dlg->btn_edit_      = gtk_button_new();
    dlg->btn_add_       = gtk_button_new();
    dlg->btn_remove_    = gtk_button_new();
    dlg->btn_move_up_   = gtk_button_new();
    dlg->btn_move_down_ = gtk_button_new();

    GtkWidget* img_edit = gtk_image_new_from_stock(
        GTK_STOCK_EDIT, GTK_ICON_SIZE_LARGE_TOOLBAR );
    gtk_container_add( GTK_CONTAINER( dlg->btn_edit_ ), img_edit );
    gtk_widget_set_tooltip_text( dlg->btn_edit_, "Edit" );

    GtkWidget* img_add = gtk_image_new_from_stock(
        GTK_STOCK_ADD, GTK_ICON_SIZE_LARGE_TOOLBAR );
    gtk_container_add( GTK_CONTAINER( dlg->btn_add_ ), img_add );
    gtk_widget_set_tooltip_text( dlg->btn_add_, "Add" );

    GtkWidget* img_remove = gtk_image_new_from_stock(
        GTK_STOCK_REMOVE, GTK_ICON_SIZE_LARGE_TOOLBAR );
    gtk_container_add( GTK_CONTAINER( dlg->btn_remove_ ), img_remove );
    gtk_widget_set_tooltip_text( dlg->btn_remove_, "Remove" );

    GtkWidget* img_move_up = gtk_image_new_from_stock(
        GTK_STOCK_GO_UP, GTK_ICON_SIZE_LARGE_TOOLBAR );
    gtk_container_add( GTK_CONTAINER( dlg->btn_move_up_ ), img_move_up );
    gtk_widget_set_tooltip_text( dlg->btn_move_up_, "Move up" );

    GtkWidget* img_move_down = gtk_image_new_from_stock(
        GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_LARGE_TOOLBAR );
    gtk_container_add( GTK_CONTAINER( dlg->btn_move_down_ ), img_move_down );
    gtk_widget_set_tooltip_text( dlg->btn_move_down_, "Move down" );

    gtk_box_pack_start( GTK_BOX( vbox ), dlg->btn_edit_,      FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), dlg->btn_add_,       FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), dlg->btn_remove_,    FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), dlg->btn_move_up_,   FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( vbox ), dlg->btn_move_down_, FALSE, FALSE, 3 );

    g_signal_connect( G_OBJECT( dlg->btn_remove_ ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_remove ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_add_ ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_add ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_edit_ ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_edit ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_move_down_ ),
                      "clicked",
                      G_CALLBACK( &attrs_dlg_on_btn_move_down ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_move_up_ ),
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



#ifdef DEBUG
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
#endif

