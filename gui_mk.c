#include "cfg_edit_dlg.h"

int tree_colid_name()  { return COL_NAME; }
int tree_colid_val()   { return COL_VAL;  }
int tree_colid_data()  { return COL_DATA; }
int tree_cols_cnt()    { return NUM_COLS; }




static void
tree_cell_draw( GtkTreeViewColumn* col,
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
*  gui_mk:
*
*/

void
gui_mk_labels_line( const gchar* left_txt,
                    GtkWidget*   right_label,
                    GtkWidget*   parent_box )
{
    GtkWidget* hbox = gtk_hbox_new( FALSE, 0 );

    GtkWidget* left_label = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( left_label ), left_txt );
    gtk_label_set_selectable( GTK_LABEL( right_label ), TRUE );

    gtk_box_pack_start( GTK_BOX( hbox ), left_label, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), right_label, FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( parent_box ), hbox, FALSE, FALSE, 0 );
}



void
gui_mk_labels_line_separ( GtkWidget* parent_box )
{
    gtk_box_pack_start( GTK_BOX( parent_box ),
                        gtk_hseparator_new(), FALSE, FALSE, 0 );
}



GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg )
{
    GtkWidget* box_tbar  = gtk_hbox_new( FALSE, 20 );

    // boxes:
    //
    GtkWidget* box1 = gtk_hbox_new( FALSE, 3 );
    GtkWidget* box2 = gtk_hbox_new( FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( box_tbar ), box2, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_tbar ), box1, FALSE, FALSE, 0 );


    // create controls:
    //
    dlg->btn_showinh_ = gtk_check_button_new_with_mnemonic( "" );
    GtkWidget* lab_showinh = gtk_bin_get_child( GTK_BIN( dlg->btn_showinh_ ) );
    gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab_showinh ),
                                        "<i>sho_w inherited</i>" );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  dlg->showinh_ );

    dlg->btn_reload_ = gtk_button_new_with_mnemonic( "_Reload" );
    dlg->btn_add_    = gtk_button_new_with_mnemonic( "_Add" );
    dlg->btn_edit_   = gtk_button_new_with_mnemonic( "_Edit" );
    dlg->btn_toggle_ = gtk_button_new_with_mnemonic( "_Toggle" );
    dlg->btn_tst_    = gtk_button_new_with_mnemonic( "t_st" );


    // images:
    //
    GtkWidget* img_add    = gtk_image_new_from_stock( GTK_STOCK_ADD,         GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_reload = gtk_image_new_from_stock( GTK_STOCK_REFRESH,     GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_edit   = gtk_image_new_from_stock( GTK_STOCK_EDIT,        GTK_ICON_SIZE_LARGE_TOOLBAR );
    GtkWidget* img_toggle = gtk_image_new_from_stock( GTK_STOCK_MEDIA_PAUSE, GTK_ICON_SIZE_LARGE_TOOLBAR );

    gtk_button_set_image( GTK_BUTTON( dlg->btn_add_ ),    img_add );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_reload_ ), img_reload );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_edit_ ),   img_edit );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_toggle_ ), img_toggle );


    // tooltips:
    //
    gtk_widget_set_tooltip_text( dlg->btn_reload_, "Reload configuration from disk." );
    gtk_widget_set_tooltip_text( dlg->btn_add_, "Add new configuration key." );
    gtk_widget_set_tooltip_text( dlg->btn_edit_, "Edit current value." );
    gtk_widget_set_tooltip_text( dlg->btn_toggle_,
                                 "Toggle values like true/false, enabled/disabled.\n"
                                 "The same can be done with double-click."
    );


    // pack controls:
    //
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_reload_, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_add_,    FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_edit_,   FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_toggle_, FALSE, FALSE, 0 );
    //
    // TESTING:
    //
    // gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_tst_,    FALSE, FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( box2 ), dlg->btn_showinh_, FALSE, FALSE, 0 );

    return box_tbar;

} // gui_mk_toolbar()



GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg, const gchar* cwd )
{
    GtkWidget* box_bot = gtk_vbox_new( FALSE, 5 );

    // cwd label:
    //
    gui_mk_labels_line( "<b>working directory: </b>", gtk_label_new( cwd ), box_bot );

    gui_mk_labels_line_separ( box_bot );

    dlg->lab_ctx_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>config context: </b>", dlg->lab_ctx_, box_bot );

    dlg->lab_fname_ = gtk_label_new( NULL );
    gtk_label_set_track_visited_links( GTK_LABEL( dlg->lab_fname_ ), FALSE );
    gui_mk_labels_line( "<b>config file: </b>", dlg->lab_fname_, box_bot );

    gui_mk_labels_line_separ( box_bot );

    dlg->lab_grp_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>group: </b>", dlg->lab_grp_, box_bot );

    dlg->lab_name_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>key: </b>", dlg->lab_name_, box_bot );

    dlg->lab_val_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>value: </b>", dlg->lab_val_, box_bot );

    gui_mk_labels_line_separ( box_bot );


    // description text view:
    //
    GtkWidget* tv = gtk_text_view_new();

    gtk_text_view_set_editable( GTK_TEXT_VIEW( tv ), FALSE );
    gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( tv ), GTK_WRAP_WORD );

    dlg->txtbuf_desc_ = gtk_text_view_get_buffer( GTK_TEXT_VIEW( tv ) );
    gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );

    // scrolled window for description text view:
    //
    GtkWidget* wscroll_desc = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_desc ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );

    gtk_container_add( GTK_CONTAINER( wscroll_desc ), tv );
    gtk_box_pack_start( GTK_BOX( box_bot ), wscroll_desc, FALSE, FALSE, 0 );

    return box_bot;

} // gui_mk_bottom_box()



// {ret}: scrolled window with tree view
//
GtkWidget*
gui_mk_tree_view( cfg_edit_dlg* dlg, GtkTreeStore* store )
{
    GtkTreeModel* tree_model = GTK_TREE_MODEL( store );
    GtkWidget*    tree_widget = gtk_tree_view_new_with_model( tree_model );
    dlg->tree_v_ = GTK_TREE_VIEW( tree_widget );

    gtk_tree_view_set_show_expanders( dlg->tree_v_, TRUE );

    // disable "search as you type".
    // Ctrl+F search is disabled by blocking that key sequence,
    // see on_key_press().
    // This is done 'cos on deactivation of a search box,
    // the tree widget's focus gets lost.
    //
    gtk_tree_view_set_enable_search( dlg->tree_v_, FALSE );

    // columns:
    //
    dlg->ren_txt_ = gtk_cell_renderer_text_new();

    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_name(), "name" );
    tree_add_col( dlg, dlg->ren_txt_, "text", tree_colid_val(),  "value" );

    // scrolled window for tree view:
    //
    GtkWidget* wscroll_tree = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_tree ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll_tree ), GTK_WIDGET( dlg->tree_v_ ) );

    return wscroll_tree;

} // gui_mk_tree_view()



void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd )
{
//    printf( " ++ gui_mk()\n" );

    // window's title:
    //
    gtk_window_set_title( GTK_WINDOW( dlg ),
                          g_strdup_printf( "%s - lepton-conf", cwd ) );


    // tree store:
    //
    dlg->store_ = gtk_tree_store_new(
        tree_cols_cnt(),
          G_TYPE_STRING   // name
        , G_TYPE_STRING   // val
        , G_TYPE_POINTER  // rdata
    );


    // toolbar:
    //
    GtkWidget* toolbar = gui_mk_toolbar( dlg );

    // tree view:
    //
    GtkWidget* wscroll_tree = gui_mk_tree_view( dlg, dlg->store_ );

    // bottom box:
    //
    GtkWidget* box_bot = gui_mk_bottom_box( dlg, cwd );


    // pack to content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( dlg ) );
    gtk_box_pack_start( GTK_BOX( ca ), toolbar, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), wscroll_tree, TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), box_bot, FALSE, FALSE, 0 );

//    printf( " -- gui_mk()\n" );

} // mk_gui()

