/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




gboolean g_gui_update_enabled = FALSE;




gboolean gui_update_enabled() { return g_gui_update_enabled;  }
void     gui_update_on()      { g_gui_update_enabled = TRUE;  }
void     gui_update_off()     { g_gui_update_enabled = FALSE; }



// update labels, disable/enable controls
// {pre}: tree has focus, row selected
//
void
gui_update( cfg_edit_dlg* dlg )
{
//    printf( " >> xxx_update_gui(): enabled: [%d]\n", gui_update_enabled() );

    if ( !gui_update_enabled() )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gtk_label_set_text( GTK_LABEL( dlg->lab_ctx_ ), conf_ctx_name( rdata->ctx_ ) );

    if ( rdata->rtype_ == RT_GRP || rdata->rtype_ == RT_KEY )
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_grp_ ), rdata->group_ );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_grp_ ), NULL );
    }

    if ( rdata->rtype_ == RT_KEY )
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_key_ ), rdata->key_ );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  rdata->val_ );

        const gchar* dflt = cfgreg_lookup_dflt_val( rdata->group_, rdata->key_ );
        gtk_label_set_text( GTK_LABEL( dlg->lab_dflt_ ), dflt ? dflt : "" );

        const gchar* desc = cfgreg_lookup_descr( rdata->group_, rdata->key_ );
        gtk_text_buffer_set_text( dlg->txtbuf_desc_, desc ? desc : "", -1 );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_key_ ), NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_dflt_ ), NULL );

        gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );
    }

    // set sensitivity for add btn
    //
    if ( (rdata->rtype_ == RT_CTX || rdata->rtype_ == RT_GRP) && !rdata->ro_)
    {
        gtk_widget_set_sensitive( dlg->btn_add_, TRUE );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_add_, FALSE );
    }

    // set sensitivity for edit btn
    //
    if ( (rdata->rtype_ == RT_KEY) && !rdata->ro_)
    {
        gtk_widget_set_sensitive( dlg->btn_edit_, TRUE );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_edit_, FALSE );
    }

    // set sensitivity for toggle btn
    //
    if ( (rdata->rtype_ == RT_KEY) && !rdata->ro_ )
    {
        gboolean can_toggle = cfgreg_can_toggle( rdata->val_ );
        gtk_widget_set_sensitive( dlg->btn_toggle_, can_toggle );
    }
    else
    {
        gtk_widget_set_sensitive( dlg->btn_toggle_, FALSE );
    }

    // set sensitivity for delete btn
    //
    gboolean enable_del = ( rdata->rtype_ == RT_GRP || rdata->rtype_ == RT_KEY )
                          && !rdata->ro_
                          && !rdata->inh_;
    gtk_widget_set_sensitive( dlg->btn_del_, enable_del );


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

} // gui_update()




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
    dlg->btn_del_    = gtk_button_new();
    dlg->btn_tst_    = gtk_button_new_with_mnemonic( "t_st" );


    // images:
    //
    const GtkIconSize size = GTK_ICON_SIZE_LARGE_TOOLBAR;
    GtkWidget* img_add    = gtk_image_new_from_stock( GTK_STOCK_ADD,         size );
    GtkWidget* img_reload = gtk_image_new_from_stock( GTK_STOCK_REFRESH,     size );
    GtkWidget* img_edit   = gtk_image_new_from_stock( GTK_STOCK_EDIT,        size );
    GtkWidget* img_toggle = gtk_image_new_from_stock( GTK_STOCK_MEDIA_PAUSE, size );
    GtkWidget* img_del    = gtk_image_new_from_stock( GTK_STOCK_DELETE,      size );

    gtk_button_set_image( GTK_BUTTON( dlg->btn_add_ ),    img_add );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_reload_ ), img_reload );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_edit_ ),   img_edit );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_toggle_ ), img_toggle );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_del_ ),    img_del );


    // tooltips:
    //
    gtk_widget_set_tooltip_text( dlg->btn_reload_, "Reload configuration from disk" );
    gtk_widget_set_tooltip_text( dlg->btn_add_, "Add new configuration key" );
    gtk_widget_set_tooltip_text( dlg->btn_edit_, "Edit current value" );
    gtk_widget_set_tooltip_text( dlg->btn_toggle_,
                                 "Toggle values like true/false, enabled/disabled"
                                 " (double-click)"
    );
    gtk_widget_set_tooltip_text( dlg->btn_del_, "Delete selected item (Del)" );


    // pack controls:
    //
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_reload_, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_add_,    FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_edit_,   FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_toggle_, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box1 ), dlg->btn_del_,    FALSE, FALSE, 0 );
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

    dlg->lab_key_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>key: </b>", dlg->lab_key_, box_bot );

    dlg->lab_val_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>value: </b>", dlg->lab_val_, box_bot );

    dlg->lab_dflt_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>default: </b>", dlg->lab_dflt_, box_bot );

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
//    GtkTreeModel* sort_model =  gtk_tree_model_sort_new_with_model( tree_model );

    GtkWidget*    tree_widget = gtk_tree_view_new_with_model( tree_model );
//    GtkWidget*    tree_widget = gtk_tree_view_new_with_model( sort_model );

//    GtkTreeSortable* sortable = GTK_TREE_SORTABLE( sort_model );
//    gtk_tree_sortable_set_sort_column_id( sortable,
//                                          tree_colid_name(),
//                                          GTK_SORT_ASCENDING );



    // TODO: use sort function:
    //
//    gtk_tree_sortable_set_default_sort_func(
//    gtk_tree_sortable_set_sort_func(
//        sortable,
//        tree_colid_name(),
//        &tree_sort_cmp_fun,
//        dlg,
//        NULL );
//    GtkWidget*    tree_widget = gtk_tree_view_new_with_model( sort_model );
//    gtk_tree_sortable_set_sort_column_id( GTK_TREE_SORTABLE( sort_model ),
//                                          tree_colid_name(),
//                                          GTK_SORT_ASCENDING );



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

    gtk_window_set_default_icon_name( GTK_STOCK_PREFERENCES );


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




/* ******************************************************************
*
*  popup menu:
*
*/

GtkMenu*
gui_mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata )
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
        mitem_key_edit = gtk_menu_item_new_with_mnemonic( "_edit..." );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_key_edit);
        g_signal_connect( G_OBJECT( mitem_key_edit ),
                          "activate",
                          G_CALLBACK( &on_mitem_key_edit ),
                          dlg );
        gtk_widget_show( mitem_key_edit );
        gtk_widget_set_sensitive( mitem_key_edit, !rdata->ro_ );


        GtkWidget* mitem_rest_dflt = NULL;
        mitem_rest_dflt = gtk_menu_item_new_with_mnemonic( "_restore default..." );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_rest_dflt);
        g_signal_connect( G_OBJECT( mitem_rest_dflt ),
                          "activate",
                          G_CALLBACK( &on_mitem_rest_dflt ),
                          dlg );
        gtk_widget_show( mitem_rest_dflt );

        const gchar* dflt = cfgreg_lookup_dflt_val( rdata->group_, rdata->key_ );
        gboolean en = dflt != NULL && strcmp( dflt, rdata->val_) != 0;
        gtk_widget_set_sensitive( mitem_rest_dflt, en && !rdata->ro_ && !rdata->inh_ );


        if ( strcmp( rdata->key_, "font" ) == 0 )
        {
            GtkWidget* mitem_separ = gtk_separator_menu_item_new();
            gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem_separ );
            gtk_widget_show( mitem_separ );

            GtkWidget* mitem_font_edit = NULL;
            mitem_font_edit = gtk_menu_item_new_with_mnemonic( "_select font..." );
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_font_edit);
            g_signal_connect( G_OBJECT( mitem_font_edit ),
                              "activate",
                              G_CALLBACK( &on_mitem_sel_font ),
                              dlg );
            gtk_widget_show( mitem_font_edit );
            gtk_widget_set_sensitive( mitem_font_edit, !rdata->ro_ );
        }
        else
        if ( strcmp( rdata->key_, "paper" ) == 0 )
        {
            GtkWidget* mitem_separ = gtk_separator_menu_item_new();
            gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem_separ );
            gtk_widget_show( mitem_separ );

            GtkWidget* mitem_sel_paper_size = NULL;
            mitem_sel_paper_size = gtk_menu_item_new_with_mnemonic( "_select paper size..." );
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_sel_paper_size);
            g_signal_connect( G_OBJECT( mitem_sel_paper_size ),
                              "activate",
                              G_CALLBACK( &on_mitem_sel_paper_size ),
                              dlg );
            gtk_widget_show( mitem_sel_paper_size );
            gtk_widget_set_sensitive( mitem_sel_paper_size, !rdata->ro_ );
        }
        else
        if ( strcmp( rdata->key_, "status-active-color" ) == 0 )
        {
            GtkWidget* mitem_separ = gtk_separator_menu_item_new();
            gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem_separ );
            gtk_widget_show( mitem_separ );

            GtkWidget* mitem_sel_color = NULL;
            mitem_sel_color = gtk_menu_item_new_with_mnemonic( "_select color..." );
            gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_sel_color);
            g_signal_connect( G_OBJECT( mitem_sel_color ),
                              "activate",
                              G_CALLBACK( &on_mitem_sel_color ),
                              dlg );
            gtk_widget_show( mitem_sel_color );
            gtk_widget_set_sensitive( mitem_sel_color, !rdata->ro_ );
        }
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


    if ( rdata->rtype_ == RT_KEY || rdata->rtype_ == RT_GRP )
    {
        GtkWidget* mitem_separ = gtk_separator_menu_item_new();
        gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem_separ );
        gtk_widget_show( mitem_separ );

        GtkWidget* mitem_del = NULL;
        mitem_del = gtk_menu_item_new_with_mnemonic( "_delete..." );
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem_del);
        g_signal_connect( G_OBJECT( mitem_del ),
                          "activate",
                          G_CALLBACK( &on_mitem_del ),
                          dlg );
        gtk_widget_show( mitem_del );
        gtk_widget_set_sensitive( mitem_del, !rdata->ro_ && !rdata->inh_ );
    }


    return GTK_MENU( menu );

} // gui_mk_popup_menu()

