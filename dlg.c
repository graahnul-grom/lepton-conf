/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




// {post}: caller must g_free() {ret}
//
gchar*
run_dlg_edit_val( cfg_edit_dlg* dlg,
                  const gchar* txt,
                  const gchar* title )
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
gboolean
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

//    printf( "  run_dlg_add_val(): resp: %d\n", res );

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
gboolean
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

//    printf( "  run_dlg_add_val_2(): resp: %d\n", res );

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



// {post}: caller must g_free() {ret}
//
gchar*
run_dlg_list_sel( cfg_edit_dlg* dlg,
                  GList* items,
                  const gchar* current_item,
                  const gchar* title )
{
    GtkWidget* sdlg = gtk_dialog_new_with_buttons(
        title ? title : "Select:",
        GTK_WINDOW( dlg ),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,     GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
        NULL );

    GtkListStore* store = gtk_list_store_new( 1, G_TYPE_STRING );
    GtkTreeModel* model = GTK_TREE_MODEL( store );
    GtkWidget*    tree_w = gtk_tree_view_new_with_model( model );
    GtkTreeView*  tree_v = GTK_TREE_VIEW( tree_w );

    // column:
    //
    GtkCellRenderer* ren = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, "title" );
    gtk_tree_view_column_pack_start( col, ren, TRUE );

    gtk_tree_view_column_add_attribute( col, ren, "text", 0 );

    gtk_tree_view_append_column( tree_v, col );
    //
    //


    // fill list:
    //
    GtkTreeIter it_current;
    for (GList* p = items; p != NULL; p = p->next)
    {
        const gchar* item = (const gchar*) p->data;

        GtkTreeIter it;
        gtk_list_store_append( store, &it );
        gtk_list_store_set( store, &it, 0, item, -1 );

        if ( strcmp( item, current_item ) == 0 )
        {
            it_current = it;
            GtkTreeSelection* sel = gtk_tree_view_get_selection( tree_v );
            gtk_tree_selection_select_iter( sel, &it );
        }
    }
    //
    //


    GtkWidget* wscroll = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll ),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll ), tree_w );

    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( sdlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll, TRUE, TRUE, 10 );


    gtk_dialog_set_alternative_button_order(GTK_DIALOG(sdlg),
                                            GTK_RESPONSE_ACCEPT,
                                            GTK_RESPONSE_REJECT,
                                            -1);
    gtk_dialog_set_default_response (GTK_DIALOG (sdlg),
                                     GTK_RESPONSE_ACCEPT);
    gtk_widget_show_all( sdlg );
//    gtk_widget_set_size_request( sdlg, 300, -1 );


    // select current value in the list:
    //
    gchar* str = gtk_tree_model_get_string_from_iter( model, &it_current );
    GtkTreePath* path = gtk_tree_path_new_from_string( str );

    gtk_tree_view_scroll_to_cell( tree_v, path, NULL, FALSE, 0, 0 );

    gtk_tree_path_free( path );
    g_free( str );
    //
    //


    gint res = gtk_dialog_run( GTK_DIALOG( sdlg ) );
    gchar* ret = NULL;

    if ( res == GTK_RESPONSE_ACCEPT )
    {
        GtkTreeSelection* sel = gtk_tree_view_get_selection( tree_v );
        GtkTreeIter it_new;
        gtk_tree_selection_get_selected( sel, NULL, &it_new );

        gtk_tree_model_get( model, &it_new, 0, &ret, -1 );
    }

    gtk_widget_destroy( sdlg );

    return ret;

} // run_dlg_list_sel()

