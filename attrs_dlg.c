/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "attrs_dlg.h"

static void
attrs_dlg_create( attrs_dlg* dlg );


G_DEFINE_TYPE(attrs_dlg, attrs_dlg, GTK_TYPE_DIALOG);


GtkWidget* attrs_dlg_new()
{
  gpointer obj = g_object_new( ATTRS_DLG_TYPE, NULL );
  return GTK_WIDGET( obj );
}



static void
attrs_dlg_class_init( attrs_dlgClass* cls )
{
}



static void
attrs_dlg_init( attrs_dlg* dlg )
{
    printf( " ++ attrs_dlg_init()\n" );
    attrs_dlg_create( dlg );
}




static void
attrs_dlg_create( attrs_dlg* dlg )
{
    // list store:
    //
    dlg->store_ = gtk_list_store_new( 1, G_TYPE_STRING );
    GtkTreeModel* model = GTK_TREE_MODEL( dlg->store_ );


    // tree view:
    //
    GtkWidget* tree_w = gtk_tree_view_new_with_model( model );
    dlg->tree_v_ = GTK_TREE_VIEW( tree_w );


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


    // show:
    //
    gtk_widget_show_all( GTK_WIDGET( dlg ) );


} // attrs_dlg_create()

