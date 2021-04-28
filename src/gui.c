/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




static void
gui_set_main_wnd_title( cfg_edit_dlg* dlg );




gboolean g_gui_update_enabled = FALSE;




gboolean gui_update_enabled() { return g_gui_update_enabled;  }
void     gui_update_on()      { g_gui_update_enabled = TRUE;  }
void     gui_update_off()     { g_gui_update_enabled = FALSE; }



// [value]: one;two;three
// {ret}:   one\ntwo\nthree
//
static gchar*
semicol_list_to_newline_list( const gchar* value )
{
    gchar** strs = g_strsplit( value, ";", 0 );
    gchar** pp = strs;

    gchar* res = NULL;

    while ( *strs != NULL )
    {
        gchar* prev = res;

        res = g_strdup_printf( "%s%s%s",
                               res ? res : "",
                               res ? "\n" : "",
                               *strs );

        ++ strs ;

        if ( *strs != NULL )
            g_free( prev );
    }

    g_strfreev( pp );

    return res;

} // mk_list_tooltip()



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


    // eneble showinh chk box, in case it was disabled in gui_off():
    //
    gtk_widget_set_sensitive( dlg->btn_showinh_, TRUE );


    gui_set_main_wnd_title( dlg );


    gchar* cwd = g_get_current_dir();
    gtk_label_set_text( GTK_LABEL( dlg->lab_cwd_ ), cwd );
    gtk_widget_set_tooltip_text( dlg->lab_cwd_, cwd );
    g_free( cwd );

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


        if ( strcmp( rdata->key_, "symbol-attribs" ) == 0 ||
             strcmp( rdata->key_, "pin-attribs" )    == 0 ||
             strcmp( rdata->key_, "always-promote" ) == 0 )
        {
            gchar* strings_val = semicol_list_to_newline_list( rdata->val_ );
            if ( strings_val != NULL )
                gtk_widget_set_tooltip_text( dlg->lab_val_, strings_val );
            g_free( strings_val );

            if ( dflt != NULL )
            {
                gchar* strings_dflt = semicol_list_to_newline_list( dflt );
                if ( strings_dflt != NULL )
                    gtk_widget_set_tooltip_text( dlg->lab_dflt_, strings_dflt );
                g_free( strings_dflt );
            }
        }
        else
        {
            gtk_widget_set_tooltip_text( dlg->lab_val_, rdata->val_ );
            gtk_widget_set_tooltip_text( dlg->lab_dflt_, dflt ? dflt : "");
        }


        const gchar* desc = cfgreg_lookup_descr( rdata->group_, rdata->key_ );
        gtk_text_buffer_set_text( dlg->txtbuf_desc_, desc ? desc : "", -1 );
    }
    else
    {
        gtk_label_set_text( GTK_LABEL( dlg->lab_key_ ), NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  NULL );
        gtk_label_set_text( GTK_LABEL( dlg->lab_dflt_ ), NULL );

        gtk_widget_set_tooltip_text( dlg->lab_val_, "" );
        gtk_widget_set_tooltip_text( dlg->lab_dflt_, "" );

        gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );
    }

    // set sensitivity for add btn
    //
    const gboolean enable_add =
        ( rdata->rtype_ == RT_CTX || rdata->rtype_ == RT_GRP )
        && !rdata->ro_;
    gtk_widget_set_sensitive( dlg->btn_add_, enable_add );

    // set sensitivity for edit btn
    //
    const gboolean enable_edit = rdata->rtype_ == RT_KEY && !rdata->ro_;
    gtk_widget_set_sensitive( dlg->btn_edit_, enable_edit );

    // set sensitivity for toggle btn
    //
    const gboolean enable_toggle =
        rdata->rtype_ == RT_KEY
        && !rdata->ro_
        && cfgreg_can_toggle( rdata );

    gtk_widget_set_sensitive( dlg->btn_toggle_, enable_toggle );

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

    gtk_widget_set_tooltip_text( dlg->lab_fname_, fname ? fname : "" );

    g_free( str_access );
    g_free( str_markup );

//    printf( " >> on_row_sel(): ctx fname: [%s]\n", fname );
//    printf( " >> on_row_sel(): name: [%s], val: [%s]\n", name, val );

} // gui_update()



// disable GUI controls (except Reload btn), clear labels.
// call it when tree selection is lost
//
void
gui_off( cfg_edit_dlg* dlg )
{
    gtk_widget_set_sensitive( dlg->btn_showinh_, FALSE );
    // gtk_widget_set_sensitive( dlg->btn_reload_,  FALSE );
    gtk_widget_set_sensitive( dlg->btn_add_,     FALSE );
    gtk_widget_set_sensitive( dlg->btn_edit_,    FALSE );
    gtk_widget_set_sensitive( dlg->btn_toggle_,  FALSE );
    gtk_widget_set_sensitive( dlg->btn_del_,     FALSE );

    gtk_label_set_text( GTK_LABEL( dlg->lab_ctx_),   NULL );
    gtk_label_set_text( GTK_LABEL( dlg->lab_fname_), NULL );
    gtk_label_set_text( GTK_LABEL( dlg->lab_grp_ ),  NULL );
    gtk_label_set_text( GTK_LABEL( dlg->lab_key_ ),  NULL );
    gtk_label_set_text( GTK_LABEL( dlg->lab_val_ ),  NULL );
    gtk_label_set_text( GTK_LABEL( dlg->lab_dflt_ ), NULL );

    gtk_text_buffer_set_text( dlg->txtbuf_desc_, "", -1 );

} // gui_off()




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
    gtk_label_set_ellipsize( GTK_LABEL( right_label ), PANGO_ELLIPSIZE_END );
    gtk_misc_set_alignment( GTK_MISC( right_label ), 0, 0.5 );

    gtk_box_pack_start( GTK_BOX( hbox ), left_label, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( hbox ), right_label, TRUE, TRUE, 0 );

    gtk_box_pack_start( GTK_BOX( parent_box ), hbox, FALSE, FALSE, 0 );
}



void
gui_mk_labels_line_separ( GtkWidget* parent_box, gint padding )
{
    gtk_box_pack_start( GTK_BOX( parent_box ),
                        gtk_hseparator_new(), FALSE, FALSE, padding );
}



GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg )
{
    GtkWidget* box_tbar  = gtk_hbox_new( FALSE, 20 );

    // boxes:
    //
    GtkWidget* box_left  = gtk_hbox_new( FALSE, 0 );
    GtkWidget* box_right = gtk_hbox_new( FALSE, 3 );
    GtkWidget* box2      = gtk_hbox_new( FALSE, 0 );

    gtk_box_pack_start( GTK_BOX( box_tbar ), box_left,  FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_tbar ), box2,      FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_tbar ), box_right, FALSE, FALSE, 0 );


    // create controls:
    //
    dlg->btn_showinh_ = gtk_check_button_new_with_mnemonic( "" );
    GtkWidget* lab_showinh = gtk_bin_get_child( GTK_BIN( dlg->btn_showinh_ ) );
    gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab_showinh ),
                                        "<i>sho_w inherited</i>" );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dlg->btn_showinh_ ),
                                  dlg->showinh_ );


    dlg->btn_open_   = gtk_button_new();
    dlg->btn_bmks_   = gtk_button_new();
    dlg->btn_reload_ = gtk_button_new();
    dlg->btn_add_    = gtk_button_new();
    dlg->btn_edit_   = gtk_button_new();
    dlg->btn_toggle_ = gtk_button_new();
    dlg->btn_del_    = gtk_button_new();
    dlg->btn_tst_    = gtk_button_new_with_mnemonic( "t_st" );
    dlg->btn_hlp_    = gtk_button_new();

    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_open_ ),   FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_bmks_ ),   FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_reload_ ), FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_add_ ),    FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_edit_ ),   FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_toggle_ ), FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_del_ ),    FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_tst_ ),    FALSE );
    gtk_button_set_focus_on_click( GTK_BUTTON( dlg->btn_hlp_ ),    FALSE );


    // images:
    //
    const GtkIconSize size = GTK_ICON_SIZE_LARGE_TOOLBAR;

    GtkWidget* img_open   = gtk_image_new_from_stock( GTK_STOCK_OPEN,        size );
    GtkWidget* img_bmks   = gtk_image_new_from_stock( GTK_STOCK_DIRECTORY,   size );
    GtkWidget* img_add    = gtk_image_new_from_stock( GTK_STOCK_ADD,         size );
    GtkWidget* img_reload = gtk_image_new_from_stock( GTK_STOCK_REFRESH,     size );
    GtkWidget* img_edit   = gtk_image_new_from_stock( GTK_STOCK_EDIT,        size );
    GtkWidget* img_toggle = gtk_image_new_from_stock( GTK_STOCK_MEDIA_PAUSE, size );
    GtkWidget* img_del    = gtk_image_new_from_stock( GTK_STOCK_DELETE,      size );
    GtkWidget* img_hlp    = gtk_image_new_from_stock( GTK_STOCK_ABOUT,       size );

    gtk_button_set_image( GTK_BUTTON( dlg->btn_open_ ),   img_open );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_bmks_ ),   img_bmks );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_add_ ),    img_add );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_reload_ ), img_reload );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_edit_ ),   img_edit );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_toggle_ ), img_toggle );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_del_ ),    img_del );
    gtk_button_set_image( GTK_BUTTON( dlg->btn_hlp_ ),    img_hlp );


    // tooltips:
    //
    gtk_widget_set_tooltip_text( dlg->btn_open_,
                                 "Open a directory (Ctrl+O)" );
    gtk_widget_set_tooltip_text( dlg->btn_bmks_,
                                 "Bookmarks (Ctrl+B)" );
    gtk_widget_set_tooltip_text( dlg->btn_reload_,
                                 "Reload configuration from disk (F5)" );
    gtk_widget_set_tooltip_text( dlg->btn_add_,
                                 "Add new configuration key (Ctrl+A)" );
    gtk_widget_set_tooltip_text( dlg->btn_edit_,
                                 "Edit current value (F2)" );
    gtk_widget_set_tooltip_text( dlg->btn_toggle_,
                                 "Toggle between possible values (space bar)" );
    gtk_widget_set_tooltip_text( dlg->btn_del_,
                                 "Delete selected item (Del)" );
    gtk_widget_set_tooltip_text( dlg->btn_hlp_,
                                 "Help/About" );


    // pack controls:
    //
    gtk_box_pack_start( GTK_BOX( box_left ),  dlg->btn_open_,       FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_left ),  dlg->btn_bmks_,       FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_reload_,     FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), gtk_hseparator_new(), FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_add_,        FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_edit_,       FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_toggle_,     FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), gtk_hseparator_new(), FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_del_,        FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( box_right ), gtk_hseparator_new(), FALSE, FALSE, 3 );
    gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_hlp_,        FALSE, FALSE, 0 );
    //
    // TESTING:
    //
    if ( g_tst_btn_visible )
    {
        gtk_box_pack_start( GTK_BOX( box_right ), dlg->btn_tst_, FALSE, FALSE, 0 );
    }

    gtk_box_pack_start( GTK_BOX( box2 ), dlg->btn_showinh_, FALSE, FALSE, 0 );

    return box_tbar;

} // gui_mk_toolbar()



GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg )
{
    GtkWidget* box_bot = gtk_vbox_new( FALSE, 5 );

    // cwd label:
    //
    dlg->lab_cwd_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>working directory: </b>", dlg->lab_cwd_, box_bot );

    gui_mk_labels_line_separ( box_bot, 0 );

    dlg->lab_ctx_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>context: </b>", dlg->lab_ctx_, box_bot );

    dlg->lab_fname_ = gtk_label_new( NULL );
    gtk_label_set_track_visited_links( GTK_LABEL( dlg->lab_fname_ ), FALSE );
    gui_mk_labels_line( "<b>config file: </b>", dlg->lab_fname_, box_bot );

    gui_mk_labels_line_separ( box_bot, 0 );

    dlg->lab_grp_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>group: </b>", dlg->lab_grp_, box_bot );

    dlg->lab_key_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>key: </b>", dlg->lab_key_, box_bot );

    dlg->lab_val_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>value: </b>", dlg->lab_val_, box_bot );

    dlg->lab_dflt_ = gtk_label_new( NULL );
    gui_mk_labels_line( "<b>default: </b>", dlg->lab_dflt_, box_bot );

    gui_mk_labels_line_separ( box_bot, 5 );


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
    tree_add_column_1( dlg );
    tree_add_column_2( dlg );

    // scrolled window for tree view:
    //
    GtkWidget* wscroll_tree = gtk_scrolled_window_new( NULL, NULL );
    gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( wscroll_tree ),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC );
    gtk_container_add( GTK_CONTAINER( wscroll_tree ), GTK_WIDGET( dlg->tree_v_ ) );

    return wscroll_tree;

} // gui_mk_tree_view()



static GtkWidget*
gui_mk_legacy_mode_box()
{
    GtkWidget* ebox = gtk_event_box_new();
    GtkWidget* lab = gtk_label_new(
        "Legacy mode: using geda*.conf configuration files" );

    gtk_container_add( GTK_CONTAINER( ebox ), lab );

    GdkColor color_bg;
    gdk_color_parse( "steelblue", &color_bg );
    gtk_widget_modify_bg( ebox, GTK_STATE_NORMAL, &color_bg );

    GdkColor color_fg;
    gdk_color_parse( "whitesmoke",  &color_fg );
    gtk_widget_modify_fg( lab, GTK_STATE_NORMAL, &color_fg );

    return ebox;

} // gui_mk_legacy_mode_box()



static void
gui_set_main_wnd_title( cfg_edit_dlg* dlg )
{
    gchar* cwd = g_get_current_dir();

    gchar* title =
        g_strdup_printf(
            "%s - lepton-conf%s",
            cwd,
            g_cfg_legacy_mode ? ": legacy mode" : "" );

    g_free( cwd );

    gtk_window_set_title( GTK_WINDOW( dlg ), title );

    g_free( title );
}



void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd )
{
    gui_set_main_wnd_title( dlg );

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
    GtkWidget* box_bot = gui_mk_bottom_box( dlg );


    // pack to content area:
    //
    GtkWidget* ca = gtk_dialog_get_content_area( GTK_DIALOG( dlg ) );

    gtk_box_pack_start( GTK_BOX( ca ), toolbar, FALSE, FALSE, 0 );

    if ( g_cfg_legacy_mode )
    {
        gtk_box_pack_start( GTK_BOX( ca ), gui_mk_legacy_mode_box(),
                            FALSE, FALSE, 0 );
    }

    gtk_box_pack_start( GTK_BOX( ca ), wscroll_tree, TRUE, TRUE, 0 );


    GtkWidget* expander = gtk_expander_new( NULL );
    gtk_container_add( GTK_CONTAINER( expander ), box_bot );
    gtk_expander_set_expanded( GTK_EXPANDER( expander ), TRUE );

    gtk_box_pack_start( GTK_BOX( ca ), expander, FALSE, FALSE, 0 );

//    printf( " -- gui_mk()\n" );

} // mk_gui()




/* ******************************************************************
*
*  popup menu:
*
*/

static void
mk_mitem_separ( GtkWidget* menu )
{
    GtkWidget* mitem = gtk_separator_menu_item_new();
    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem );
    gtk_widget_show( mitem );
}



static void
mk_mitem( GtkWidget* menu,
          const gchar* text,
          void (*callback) (GtkMenuItem*, gpointer),
          gpointer data,
          gboolean enabled,
          const gchar* stockid,
          gboolean bold )
{
    GtkWidget* mitem = gtk_image_menu_item_new_with_mnemonic( text );

    if ( bold )
    {
        GtkWidget* lab = gtk_bin_get_child( GTK_BIN( mitem ) );
        gchar* str = g_strdup_printf( "<b>%s</b>", text );
        gtk_label_set_markup_with_mnemonic( GTK_LABEL( lab ), str );
        g_free( str );
    }

    if ( stockid != NULL )
    {
        GtkWidget* img = gtk_image_new_from_stock( stockid, GTK_ICON_SIZE_MENU );
        gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM( mitem ), img );
    }

    gtk_menu_shell_append (GTK_MENU_SHELL (menu), mitem);
    g_signal_connect( G_OBJECT( mitem ),
                      "activate",
                      G_CALLBACK( callback ),
                      data );
    gtk_widget_show( mitem );
    gtk_widget_set_sensitive( mitem, enabled );

} // mk_mitem()



GtkMenu*
gui_mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata )
{
    GtkWidget* menu = gtk_menu_new();

    if ( rdata->rtype_ == RT_CTX )
    {
        mk_mitem( menu,
                  "_Add..",
                  &on_mitem_ctx_add,
                  dlg,
                  !rdata->ro_,
                  GTK_STOCK_ADD,
                  FALSE );
    }
    else
    if ( rdata->rtype_ == RT_GRP )
    {
        mk_mitem( menu,
                  "_Add...",
                  &on_mitem_grp_add,
                  dlg,
                  !rdata->ro_,
                  GTK_STOCK_ADD,
                  FALSE );
    }
    else
    if ( rdata->rtype_ == RT_KEY )
    {
        const gboolean can_toggle = cfgreg_can_toggle( rdata );

        mk_mitem( menu,
                  "_Edit...",
                  &on_mitem_key_edit,
                  dlg,
                  !rdata->ro_,
                  GTK_STOCK_EDIT,
                  !can_toggle );

        if ( can_toggle )
        {
//                    gtk_bin_get_child( GTK_BIN());
            mk_mitem( menu,
                      "_Toggle",
                      &on_mitem_toggle,
                      dlg,
                      !rdata->ro_,
                      GTK_STOCK_MEDIA_PAUSE,
                      TRUE );
        }

        const gchar* dflt = cfgreg_lookup_dflt_val( rdata->group_, rdata->key_ );
        gboolean en = dflt != NULL && strcmp( dflt, rdata->val_) != 0;
        en = en && !rdata->ro_ && !rdata->inh_;
        mk_mitem( menu,
                  "_Restore Default...",
                  &on_mitem_rest_dflt,
                  dlg,
                  en,
                  GTK_STOCK_CLEAR,
                  FALSE );


        if ( strcmp( rdata->key_, "font" ) == 0 )
        {
            mk_mitem_separ( menu );
            mk_mitem( menu,
                      "_Select Font...",
                      &on_mitem_sel_font,
                      dlg,
                      !rdata->ro_,
                      GTK_STOCK_SELECT_FONT,
                      FALSE );
        }
        else
        if ( strcmp( rdata->key_, "paper" ) == 0 )
        {
            mk_mitem_separ( menu );
            mk_mitem( menu,
                      "_Select Paper Size...",
                      &on_mitem_sel_paper_size,
                      dlg,
                      !rdata->ro_,
                      GTK_STOCK_PAGE_SETUP,
                      FALSE );
        }
        else
        if ( strcmp( rdata->key_, "status-active-color" ) == 0 )
        {
            mk_mitem_separ( menu );
            mk_mitem( menu,
                      "_Select Color...",
                      &on_mitem_sel_color,
                      dlg,
                      !rdata->ro_,
                      GTK_STOCK_SELECT_COLOR,
                      FALSE );
        }
        else
        if ( strcmp( rdata->key_, "symbol-attribs" ) == 0 ||
             strcmp( rdata->key_, "pin-attribs" )    == 0 ||
             strcmp( rdata->key_, "always-promote" ) == 0 )
        {
            mk_mitem_separ( menu );
            mk_mitem( menu,
                      "Edit _Attributes...",
                      &on_mitem_edit_attrs,
                      dlg,
                      !rdata->ro_,
                      GTK_STOCK_PROPERTIES,
                      FALSE );
        }

    } // RT_KEY


    if ( rdata->rtype_ == RT_KEY || rdata->rtype_ == RT_GRP )
    {
        mk_mitem_separ( menu );
        mk_mitem( menu,
                  "_Delete...",
                  &on_mitem_del,
                  dlg,
                  !rdata->ro_ && !rdata->inh_,
                  GTK_STOCK_DELETE,
                  FALSE );
    }


    return GTK_MENU( menu );

} // gui_mk_popup_menu()



GtkMenu*
gui_mk_bookmarks_menu( cfg_edit_dlg* dlg )
{
    GtkWidget* menu = gtk_menu_new();

    GtkWidget* mitem = gtk_menu_item_new_with_label( "Bookmark current directory" );
    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem );
    gtk_widget_show( mitem );

    g_signal_connect( G_OBJECT( mitem ),
                          "activate",
                          G_CALLBACK( &on_mitem_bookmark_add ),
                          NULL );

    mitem = gtk_menu_item_new_with_label( "Manage bookmarks..." );
    gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem );
    gtk_widget_show( mitem );

    g_signal_connect( G_OBJECT( mitem ),
                          "activate",
                          G_CALLBACK( &on_mitem_bookmarks_manage ),
                          NULL );

    mk_mitem_separ( menu );


    for ( GList* p = g_bookmarks; p != NULL; p = p->next )
    {
        gchar* path = (gchar*) p->data;

        mitem = gtk_menu_item_new_with_label( path );
        gtk_menu_shell_append( GTK_MENU_SHELL( menu ), mitem );
        gtk_widget_show( mitem );

        g_signal_connect( G_OBJECT( mitem ),
                          "activate",
                          G_CALLBACK( &on_mitem_bookmark_goto ),
                          dlg );
    }

    return GTK_MENU( menu );

} // gui_mk_bookmarks_menu()

