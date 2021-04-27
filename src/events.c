/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"
#include "attrs_dlg.h"
#include <gdk/gdkkeysyms.h>




void
events_setup( cfg_edit_dlg* dlg )
{
    g_signal_connect( G_OBJECT( dlg ),
                      "delete-event",
                      G_CALLBACK( &on_delete_event ),
                      NULL );

    g_signal_connect( G_OBJECT( dlg->btn_showinh_ ),
                      "toggled",
                      G_CALLBACK( &on_btn_showinh ),
                      dlg );

    // NOTE: GtkLabel "activate-link" signal emitted when:
    //       1) mouse clicked
    //       2) Enter pressed when label's text is NOT selected
    //
    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-link",
                      G_CALLBACK( &on_lab_fname ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-current-link",
                      G_CALLBACK( &on_lab_fname ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_reload_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_reload ),
                      dlg );

    if ( g_tst_btn_visible )
    {
        g_signal_connect( G_OBJECT( dlg->btn_tst_ ),
                          "clicked",
                          G_CALLBACK( &on_btn_tst ),
                          dlg );
    }

    g_signal_connect( G_OBJECT( dlg->btn_add_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_add ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_edit_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_edit ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_toggle_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_toggle ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_del_ ),
                      "clicked",
                      G_CALLBACK( &on_mitem_del ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_hlp_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_hlp ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_open_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_open ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_bmks_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_bookmarks ),
                      dlg );





    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",         // tree sel changed
//                      "row-activated",
                      G_CALLBACK( &on_row_sel ),
                      dlg );


    // testing:
    //
    GtkTreeSelection* sel = gtk_tree_view_get_selection( dlg->tree_v_ );
    gtk_tree_selection_set_mode( sel, GTK_SELECTION_BROWSE );

    g_signal_connect( G_OBJECT( sel ),
                      "changed",
                      G_CALLBACK( &on_tree_sel_changed ),
                      dlg );







    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "button-press-event",
                      G_CALLBACK( &on_mouse_click ),
                      dlg );

//    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
//                      "start-interactive-search",
//                      G_CALLBACK( &on_tree_search ),
//                      dlg );


    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "key-press-event",
                      G_CALLBACK( &on_key_press ),
                      dlg );


    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "popup-menu",
                      G_CALLBACK( &on_popup_menu ),
                      dlg );

} // events_setup()




/* ******************************************************************
*
*  TESTING:
*
*/

void
on_btn_tst( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );


    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;







    GtkMenu* menu = gui_mk_bookmarks_menu( dlg );

    gtk_menu_popup( menu,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    0,                              // 0 => not a mouse event
                    gtk_get_current_event_time() );





    return;



    chdir( "/home/dmn/lepton/TST/lepton-conf_open_dir/2" );

    tree_set_focus( dlg );

    a_reload( dlg );

    tree_set_focus( dlg );
    gui_update( dlg );

//    on_btn_hlp( NULL, NULL );

//    on_mitem_edit_attrs( NULL, dlg );




    return;



    /*

    // 1: remember current ctx->grp->key:
    row_cgk* cgk = cgk_mk( rdata );
    printf( " .. cgk: [ %s : %s : %s ]\n", cgk->ctx_, cgk->grp_, cgk->key_ );


    // 2: reload tree:
    a_reload( dlg );


    // TESTING:
    // row_select_by_ctx_grp_key( dlg, ctx, grp, key );
    row_select_by_cgk( dlg, cgk );
    cgk_rm( cgk );

    */




    // print current path:
//    /*
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gchar* str = gtk_tree_model_get_string_from_iter( mod, &it );
    printf( " .. p: [%s]\n", str );
    GtkTreePath* path = gtk_tree_path_new_from_string( str );
    g_free( str );
    gtk_tree_path_free( path );
//    */
    //


    return;


//    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );

//    GdkEvent* ev = gdk_event_new( GDK_KEY_PRESS );
//    ev->key.window = gtk_widget_get_window( GTK_WIDGET( dlg->tree_v_ ) );
//    ev->key.window = gtk_widget_get_window( GTK_WIDGET( dlg->tree_v_ ) );
//    ev->key.keyval = GDK_KEY_space;
//    ev->key.  is_modifier keyval = GDK_KEY_space;
//    ev->key.hardware_keycode = 32;
//    gdk_event_put( ev );

    // XXX: OK!
    //
//    gtk_test_widget_send_key( GTK_WIDGET( dlg->tree_v_ ),
//                              GDK_KEY_Return,
//                              (GdkModifierType) 0 );

//    gboolean res = gtk_widget_activate( GTK_WIDGET( dlg->tree_v_ ) );
//    gtk_widget_grab_focus( GTK_WIDGET( dlg->tree_v_ ) );
//    printf( " >>  res(): [%d]\n", res ); // => false


    // EdaConfig* ctx_parent = eda_config_get_user_context();
    // EdaConfig* ctx        = eda_config_get_context_for_path( "." );
    // conf_reload_child_ctxs( ctx_parent, dlg );

//    gchar* path = row_cur_pos_save( dlg );

//    gui_update_off();

//    GtkAdjustment* adj = gtk_tree_view_get_vadjustment( dlg->tree_v_ );
//    gtk_tree_view_set_vadjustment( dlg->tree_v_, NULL );

//    a_reload( dlg );
    // conf_reload_ctx( ctx, "3", dlg );

//    gtk_tree_view_set_vadjustment( dlg->tree_v_, adj );

//    tree_set_focus( dlg );

//    gui_update_on();

//    row_cur_pos_restore( dlg, path );
//    g_free( path );

    // char* path_str = row_cur_pos_save( dlg );
    // conf_reload_ctx_path( dlg );
    // row_cur_pos_restore( dlg, path_str );
    // row_cur_pos_restore( dlg, "0:1:0" );
    // GtkTreePath* path = gtk_tree_path_new_from_string( "0:1:0" );
    // gtk_tree_path_up( path );

} // on_btn_tst()




/* ******************************************************************
*
*  event handlers:
*
*/

// testing:
//
// GtkTreeSelection "changed" signal handler
//
void
on_tree_sel_changed( GtkTreeSelection* sel, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    g_return_if_fail( dlg != NULL );


    GtkTreeModel* mod = NULL;
    GtkTreeIter it;
    gboolean res = gtk_tree_selection_get_selected( sel, &mod, &it );

#ifdef DEBUG
    printf( " .. on_tree_sel_changed(): res: [%d]\n", res );
#endif

    if ( !res ) // NOTE: tree selection is lost
    {
        gui_off( dlg );
        return;
    }


    // TESTING: GtkTreeView::cursor-changed => => GtkTreeSelection::changed
    //
    gui_update( dlg );
    //
    //
}



// GtkTreeView "cursor-changed" signal handler
//
void
on_row_sel( GtkTreeView* tree, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    // TESTING: GtkTreeView::cursor-changed => => GtkTreeSelection::changed
    //
    // gui_update( dlg );
    //
    //
}



void
on_delete_event( cfg_edit_dlg* dlg, GdkEvent* e, gpointer* p )
{
    // printf( "cfg_edit_dlg::on_delete_event()\n" );

    // NOTE: free rdata
    //
    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    gtk_tree_model_foreach( mod, &rdata_rm_func, dlg );

    gtk_widget_destroy( GTK_WIDGET( dlg ) );
}



// GtkLabel "activate-link" signal handler
//
void
on_lab_fname( GtkLabel* lab, gchar* uri, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    const gchar* fname = gtk_label_get_text( lab );
    a_run_editor( dlg, fname );

} // on_lab_fname()




/* ******************************************************************
*
*  mouse / keyboard:
*
*/

// handler for button-press-event signal:
//
gboolean
on_mouse_click( GtkWidget* w, GdkEvent* e, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;


    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return FALSE;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return FALSE;


    GdkEventButton* ebtn = ( GdkEventButton* ) e;


    // LMB double click:
    //
    if ( !rdata->ro_ && ebtn->type == GDK_2BUTTON_PRESS && ebtn->button == 1 )
    {
        if ( cfgreg_can_toggle( rdata ) )
        {
            a_toggle( dlg );
        }
        else
        if ( rdata->rtype_ == RT_KEY )
        {
            gchar* txt = run_dlg_edit_val( GTK_WINDOW( dlg ), rdata->val_, NULL );
            a_chg_val( dlg, rdata, it, txt );
            g_free( txt );
        }

        return TRUE;
    }


    // not an RMB => do not process event (return FALSE):
    //
    if ( ebtn->type != GDK_BUTTON_PRESS || ebtn->button != 3 )
        return FALSE;


    //
    // further down only return TRUE to not allow selecting rows with RMB
    //


    if ( ebtn->window != gtk_tree_view_get_bin_window( dlg->tree_v_ ) )
        return TRUE;


    GtkTreePath* path_cur = NULL;
    path_cur = gtk_tree_model_get_path( gtk_tree_view_get_model( dlg->tree_v_ ), &it );

    GtkTreePath* path_rmb = NULL;
    gboolean onrow =
        gtk_tree_view_get_path_at_pos( dlg->tree_v_, ebtn->x, ebtn->y, &path_rmb,
                                       NULL, NULL, NULL );

    if ( onrow )
    {
        if ( gtk_tree_path_compare( path_cur, path_rmb ) != 0 )
            onrow = FALSE;
    }

    gtk_tree_path_free( path_cur );
    gtk_tree_path_free( path_rmb );

    if ( !onrow )
        return TRUE;

    GtkMenu* menu = gui_mk_popup_menu( dlg, rdata );
    if ( menu )
    {
        gtk_menu_popup( menu, NULL, NULL, NULL, NULL,
                        ebtn->button, ebtn->time );
                        // 0, gtk_get_current_event_time() );
    }

    return TRUE;

} // on_mouse_click()



// handler for key-press-event signal:
// [e]: GdkEventKey
//
gboolean
on_key_press( GtkWidget* w, GdkEvent* e, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;

    // printf( " >> on_key_press(): [0x%X]\n", e->key.keyval );

    // block Ctrl+F:
    //
    if ( e->key.keyval == GDK_KEY_f )
    {
        // printf( " ** ** on_key_press(): BLCOK 'F' KEY\n" );
        return TRUE;
    }

    // do not close dialog on 'Escape' key:
    //
    if ( e->key.keyval == GDK_KEY_Escape && !g_close_with_esc )
    {
        // printf( " ** ** on_key_press(): ESC KEY\n" );
        return TRUE;
    }

    // Delete:
    //
    if ( e->key.keyval == GDK_KEY_Delete )
    {
        on_mitem_del( NULL, (gpointer) dlg );
        return TRUE;
    }

    // F5:
    //
    if ( e->key.keyval == GDK_KEY_F5 )
    {
        on_btn_reload( NULL, (gpointer) dlg );
        return TRUE;
    }

    // F2:
    //
    if ( e->key.keyval == GDK_KEY_F2 )
    {
        on_btn_edit( NULL, (gpointer) dlg );
        return TRUE;
    }

    // space bar:
    //
    if ( e->key.keyval == GDK_KEY_space )
    {
        on_btn_toggle( NULL, (gpointer) dlg );
        return TRUE;
    }

    // Ctrl + A:
    //
    // NOTE: 4 => control is pressed:
    //
    if ( e->key.keyval == GDK_KEY_a && (e->key.state & 4) )
    {
        on_btn_add( NULL, (gpointer) dlg );
        return TRUE;
    }

    // Ctrl + O:
    //
    // NOTE: 4 => control is pressed:
    //
    if ( e->key.keyval == GDK_KEY_o && (e->key.state & 4) )
    {
        on_btn_open( NULL, (gpointer) dlg );
        return TRUE;
    }


    return FALSE; // propagate event

} // on_key_press()




/* ******************************************************************
*
*  TOGGLE / RELOAD / SHOWINH:
*
*/

void
on_btn_toggle( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    a_toggle( dlg );

}



void
on_mitem_toggle( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    a_toggle( dlg );

}



void
on_btn_reload( GtkButton* btn, gpointer* p )
{
    // NOTE: do not use [btn], since this function is called
    //       not only from the button event handler
    //

    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    a_reload( dlg );

    tree_set_focus( dlg );
    gui_update( dlg );

} // on_btn_reload()



void
on_btn_showinh( GtkToggleButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gboolean showinh = gtk_toggle_button_get_active( btn );

    // - about to hide all inherited rows
    // - current row is inherited
    // => change to nearest non-inherited row upwards in hierarchy:
    //
    if ( !showinh && rdata->inh_ )
    {
        row_select_non_inh( dlg, it );
    }

    a_showinh( dlg, showinh );


    // ensure that current node is visible after refiltering:
    //
    row_cur_get_iter( dlg, &it );

    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    if ( path != NULL )
    {
        gtk_tree_view_scroll_to_cell( dlg->tree_v_, path, NULL, TRUE, 0.5, 0 );
        gtk_tree_path_free( path );
    }

} // on_btn_showinh()



void
on_btn_hlp( GtkToggleButton* btn, gpointer* p )
{
//    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
//    if ( !dlg )
//        return;
//    printf( " .. on_btn_hlp()\n" );

    GtkWidget* dlg = gtk_about_dialog_new();
    GtkAboutDialog* adlg = GTK_ABOUT_DIALOG( dlg );

    gtk_about_dialog_set_program_name( adlg, "lepton-conf" );
    gtk_about_dialog_set_version( adlg, "1.0 epsilon" );
    gtk_about_dialog_set_comments( adlg, "Lepton EDA Configuration Utility" );

    gtk_about_dialog_set_copyright( adlg,
        "Copyright © 2017-2021 dmn <graahnul.grom@gmail.com>" );

    gtk_about_dialog_set_license( adlg,
        "lepton-conf is freely distributable under\n"
        "the GNU Public License (GPL) version 2.0\n"
        "or (at your option) any later version.\n"
        "See the COPYING file for the full text of the license." );

    GtkWidget* ca = gtk_dialog_get_content_area (GTK_DIALOG (dlg));

    GtkWidget* label_leda = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( label_leda ),
        "<a href='http://github.com/lepton-eda/lepton-eda'>Lepton Electronic Design Automation</a>" );

    GtkWidget* label_lconf = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( label_lconf ),
        "<a href='https://github.com/graahnul-grom/lepton-conf'>lepton-conf</a>" );

    GtkWidget* label_docs = gtk_label_new( NULL );
    gtk_label_set_markup( GTK_LABEL( label_docs ),
        "Documentation: <a href='https://github.com/lepton-eda/lepton-eda/wiki/Configuration-Settings'>configuration settings</a> wiki page" );

    gtk_box_pack_start( GTK_BOX( ca ), gtk_hseparator_new(), FALSE, FALSE, 5 );
    gtk_box_pack_start( GTK_BOX( ca ), label_docs, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), gtk_hseparator_new(), FALSE, FALSE, 5 );
    gtk_box_pack_start( GTK_BOX( ca ), label_lconf, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), label_leda, FALSE, FALSE, 0 );
    gtk_box_pack_start( GTK_BOX( ca ), gtk_hseparator_new(), FALSE, FALSE, 5 );

    gtk_widget_show_all( dlg );
    gtk_dialog_run( GTK_DIALOG( dlg ) );
    gtk_widget_destroy( dlg );

} // on_btn_hlp()




/* ******************************************************************
*
*  EDIT:
*
*/

void
on_btn_edit( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    if ( rdata->rtype_ != RT_KEY || rdata->ro_ )
        return;

    gchar* txt = run_dlg_edit_val( GTK_WINDOW( dlg ), rdata->val_, NULL );

    a_chg_val( dlg, rdata, it, txt );

    g_free( txt );

} // on_btn_edit()



void
on_mitem_key_edit( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    gchar* txt = run_dlg_edit_val( GTK_WINDOW( dlg ), rdata->val_, NULL );

    a_chg_val( dlg, rdata, it, txt );

    g_free( txt );

} // on_mitem_key_edit()



void
on_mitem_del( GtkMenuItem* mitem, gpointer p )
{
    // NOTE: do not use [mitem], since this function is called
    //       not only from the context menu event handler
    //
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    if ( rdata->ro_ || rdata->inh_ )
        return;

    const gboolean iskey = rdata->rtype_ == RT_KEY;
    const gboolean isgrp = rdata->rtype_ == RT_GRP;

    if ( ! ( iskey || isgrp ) )
        return;

    const gchar* msg_key =
        "The following key will be deleted:\n"
        "\n"
        "<b>[%s]::%s</b>\n"
        "\n"
        "Are you sure?";

    const gchar* msg_grp =
        "The following group and all its keys will be deleted:\n"
        "\n"
        "<b>[%s]</b>\n"
        "\n"
        "Are you sure?%s";


    GtkWidget* ddlg = gtk_message_dialog_new_with_markup(
        NULL,
        (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_YES_NO,
        iskey ? msg_key : msg_grp,
        rdata->group_, iskey ? rdata->key_ : "" );

    gtk_window_set_title( GTK_WINDOW( ddlg ), "Confirm delete" );
    gtk_dialog_set_alternative_button_order( GTK_DIALOG( ddlg ),
                                             GTK_RESPONSE_YES,
                                             GTK_RESPONSE_NO,
                                             -1 );
    gtk_dialog_set_default_response( GTK_DIALOG( ddlg ), GTK_RESPONSE_NO );

    if ( gtk_dialog_run( GTK_DIALOG( ddlg ) ) == GTK_RESPONSE_YES )
    {
        a_delete( dlg );
    }

    gtk_widget_destroy( ddlg );

} // on_mitem_del()



void
on_mitem_rest_dflt( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    const gchar* dflt = cfgreg_lookup_dflt_val( rdata->group_, rdata->key_ );
    if (dflt != NULL)
    {
#ifdef DEBUG
        printf( " >> dflt: [%s]\n", dflt );
#endif


        GtkWidget* mdlg = gtk_message_dialog_new_with_markup(
            NULL,
            (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_YES_NO,
            "<b>key</b>: [%s]::%s\n"
            "<b>current value</b>: %s\n"
            "<b>default value</b>: %s"
            , rdata->group_, rdata->key_, rdata->val_, dflt );

        gtk_window_set_title( GTK_WINDOW( mdlg ),
                              "Restore default value?" );

        gtk_dialog_set_alternative_button_order( GTK_DIALOG( mdlg ),
                                                 GTK_RESPONSE_YES,
                                                 GTK_RESPONSE_NO,
                                                 -1 );

        gtk_dialog_set_default_response( GTK_DIALOG( mdlg ),
                                         GTK_RESPONSE_NO );

        if ( gtk_dialog_run( GTK_DIALOG( mdlg ) ) == GTK_RESPONSE_YES )
        {
            a_chg_val( dlg, rdata, it, dflt );
        }

        gtk_widget_destroy( mdlg );
    }

} // on_mitem_rest_dflt()



void
on_mitem_sel_font( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    // the following keys uses font size:
    // - [schematic.log-window]::font
    // - [schematic.macro-widget]::font
    //
    const gboolean grp_log_wnd =
        strcmp( rdata->group_, "schematic.log-window" ) == 0;
    const gboolean grp_macro_wdgt =
        strcmp( rdata->group_, "schematic.macro-widget" ) == 0;

    const gboolean use_size = grp_log_wnd || grp_macro_wdgt;


    GtkWidget* fdlg = gtk_font_selection_dialog_new( "Select Font" );
    GtkWidget* fsdfs = gtk_font_selection_dialog_get_font_selection(
        GTK_FONT_SELECTION_DIALOG( fdlg ) );
    GtkFontSelection* sel =
        GTK_FONT_SELECTION( fsdfs );


    gint size = 18; // default preview size if font size isn't used

    PangoFontDescription* desc =
        pango_font_description_from_string( rdata->val_ );

    if ( use_size )
    {
        gint sz = pango_font_description_get_size( desc ) / PANGO_SCALE;
        if ( sz > 0 )
        {
            size = sz;
        }
    }

    pango_font_description_set_size( desc, size * PANGO_SCALE );

    gchar* fname = pango_font_description_to_string( desc );
    // printf( " == fname: [%s]\n", fname );
    pango_font_description_free( desc );


    gtk_font_selection_set_font_name( sel, fname );
    g_free (fname);

    const char preview[] = "refdes=R1 Q23 U45 footprint=TQFN20_4_EP.fp";
    gtk_font_selection_set_preview_text( sel, preview );


    settings_restore_wnd_geom( GTK_WINDOW( fdlg ), "selfont" );

    if ( gtk_dialog_run( GTK_DIALOG( fdlg ) ) == GTK_RESPONSE_OK )
    {
        PangoFontFamily* family = gtk_font_selection_get_family( sel );
        const char* family_name = pango_font_family_get_name( family );

        PangoFontFace* face = gtk_font_selection_get_face( sel );
        const char* face_name = pango_font_face_get_face_name( face );

        gchar* txt = g_strdup_printf( "%s %s", family_name, face_name );


        if ( use_size )
        {
            size = gtk_font_selection_get_size( sel ) / PANGO_SCALE;
            // printf( " == size: [%d]\n", size );
            if ( size > 0 )
            {
                gchar* prev = txt;
                txt = g_strdup_printf( "%s %d", txt, size );
                g_free( prev );
            }
        }


        a_chg_val( dlg, rdata, it, txt );
        g_free( txt );
    }

    settings_save_wnd_geom( GTK_WINDOW( fdlg ), "selfont" );
    gtk_widget_destroy( fdlg );

} // on_mitem_font_edit()



void
on_mitem_sel_paper_size( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    GList* lst = gtk_paper_size_get_paper_sizes (TRUE); // TRUE: include custom
    GList* names = NULL;

    for ( GList* p = lst; p != NULL; p = p->next )
    {
        const gchar* name = gtk_paper_size_get_name ((GtkPaperSize*) p->data);
        names = g_list_append( names, (gpointer) name );
    }


    gchar* txt = run_dlg_list_sel( dlg, names, rdata->val_, "Select Paper Size" );

    if ( txt != NULL )
    {
        if ( strcmp( txt, rdata->val_ ) != 0 )
        {
            a_chg_val( dlg, rdata, it, txt );
        }

        g_free( txt );
    }

    g_list_free( names );

    for ( GList* p = lst; p != NULL; p = p->next )
    {
        gtk_paper_size_free( (GtkPaperSize*) p->data );
    }

    g_list_free( lst );

} // on_mitem_sel_paper_size()



void
on_mitem_sel_color( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    GtkWidget* cdlg = gtk_color_selection_dialog_new( "Select Color" );
    GtkColorSelectionDialog* csd = GTK_COLOR_SELECTION_DIALOG( cdlg );
    GtkWidget* wsel =
            gtk_color_selection_dialog_get_color_selection( csd );
    GtkColorSelection* sel = GTK_COLOR_SELECTION( wsel );


    GdkColormap* cmap = gdk_colormap_get_system();
    GdkColor color;
    gdk_colormap_alloc_color( cmap, &color,
                              TRUE, TRUE );  // writeable, best_match

    if ( gdk_color_parse( rdata->val_, &color ) )
    {
        gtk_color_selection_set_current_color( sel, &color );
    }


    settings_restore_wnd_geom( GTK_WINDOW( cdlg ), "selcolor" );

    if ( gtk_dialog_run( GTK_DIALOG( cdlg ) ) == GTK_RESPONSE_OK )
    {
        GdkColor c;
        gtk_color_selection_get_current_color( sel, &c );
        char txt[ 64 ] = "";
        sprintf( txt, "#%.2X%.2X%.2X", c.red >> 8, c.green >> 8, c.blue >> 8 );
#ifdef DEBUG
        printf( " >> c: [%s]\n", txt );
#endif

        a_chg_val( dlg, rdata, it, txt );
    }

    settings_save_wnd_geom( GTK_WINDOW( cdlg ), "selcolor" );
    gtk_widget_destroy( cdlg );

    gdk_colormap_free_colors( cmap, &color, 1 );

} // on_mitem_sel_color()



void
on_mitem_edit_attrs( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;


    gchar* attrs_new_as_string = attrs_dlg_run( rdata->val_, rdata->key_ );

    printf( " >> attrs_new_as_string: [%s]\n", attrs_new_as_string );

    if ( attrs_new_as_string != NULL &&
         g_strcmp0( attrs_new_as_string, rdata->val_ ) != 0 )
    {
        a_chg_val( dlg, rdata, it, attrs_new_as_string );
    }

    g_free( attrs_new_as_string );

} // on_mitem_edit_attrs()




/* ******************************************************************
*
*  ADD:
*
*/

void
on_btn_add( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    tree_set_focus( dlg );

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    const row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    if ( rdata->rtype_ == RT_CTX )
    {
        on_mitem_ctx_add( NULL, dlg );
    }
    else
    if ( rdata->rtype_ == RT_GRP )
    {
        on_mitem_grp_add( NULL, dlg );
    }

} // on_btn_add()



// group node: "add" mitem
//
void
on_mitem_grp_add( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return;

    gchar* key = NULL;
    gchar* val = NULL;

    if ( !run_dlg_add_key_val( dlg, NULL, &key, &val ) )
        return;

    GtkTreePath* path1 = row_find_child_by_name( dlg, it, key );

    if ( path1 != NULL )
    {
        printf( "on_mitem_add(): [%s] EXISTS\n", key );

        row_select_by_path_mod( dlg, path1 );

        GtkTreeIter it_child;
        row_cur_get_iter( dlg, &it_child );

//        GtkTreeIter it_child;
//        GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
//        gtk_tree_model_get_iter( mod, &it_child, path1 );

        row_data* rdata_child = row_field_get_data( dlg, &it_child );


        if ( rdata_child->inh_ || g_strcmp0( val, rdata_child->val_ ) != 0 )
        {
            // NOTE: conf_chg_val() / conf_save()
            //
            conf_chg_val( rdata_child, val );

            if ( conf_save( rdata->ctx_, dlg ) )
            {
                row_field_set_val( dlg, it_child, val );




                // XXX:  fixes crash in subsequent calls to row_field_get_data()
                // NOTE: crashes only if sort model is used
                // TODO: does gtk_tree_store_set() invalidate iterators??
                //
                row_cur_get_iter( dlg, &it_child );
                //
                //
                //
                //




                // NOTE: unset inherited:
                //
                row_key_unset_inh( dlg, it_child );

                // NOTE: conf_reload_child_ctxs()
                //
                conf_reload_child_ctxs( rdata->ctx_, dlg );
            }
        }


        gtk_tree_path_free( path1 );
        g_free( key );
        g_free( val );

        return;

    } // if key already exists


    // NOTE: conf_add_val() / conf_save()
    //
    conf_add_val( rdata, key, val );
    if ( !conf_save( rdata->ctx_, dlg ) )
        return;

    printf( "on_mitem_add(): [%s] = [%s]\n", key, val );

    // NOTE: rdata:
    //
    row_data* rdata_new = rdata_mk( rdata->ctx_,
                                    rdata->group_,  // group
                                    key,            // key
                                    val,            // val
                                    FALSE,          // ro
                                    FALSE,          // inh
                                    RT_KEY          // rtype
                                  );

    GtkTreeIter it_grp_tstrore = row_get_tstore_iter( dlg, it );

    GtkTreeIter it_new = tree_add_row( dlg,
                                       key,
                                       val,
                                       rdata_new,
                                       &it_grp_tstrore
                                     );


    // NOTE: select newly added key node:
    //
    row_select_by_iter_tstore( dlg, it_new );

    // NOTE: unset inherited: TODO: do we need it?
    //
    GtkTreeIter it_cur;
    if ( row_cur_get_iter( dlg, &it_cur ) )
        row_key_unset_inh( dlg, it_cur );


    // >>>> remember selected item:
    //
    const row_data* rd_cur = row_field_get_data( dlg, &it_cur );
    row_cgk* cgk = cgk_mk( rd_cur );


    // NOTE: conf_reload_child_ctxs()
    //
    conf_reload_child_ctxs( rdata->ctx_, dlg );


    // >>>> restore selected item:
    //
    row_select_by_cgk( dlg, cgk );
    cgk_rm( cgk );


    g_free( key );
    g_free( val );

} // on_mitem_grp_add()



// context node: "add" mitem
//
void
on_mitem_ctx_add( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it_ctx;
    if ( !row_cur_get_iter( dlg, &it_ctx ) )
        return;

    row_data* rdata_ctx = row_field_get_data( dlg, &it_ctx );
    if ( !rdata_ctx )
        return;


    gchar* grp = NULL;
    gchar* key = NULL;
    gchar* val = NULL;

    if ( !run_dlg_add_grp_key_val( dlg, NULL, &grp, &key, &val ) )
        return;

    printf( "on_mitem_ctx_add( %s ): [%s] [%s] [%s]\n",
            conf_ctx_name( rdata_ctx->ctx_ ), grp, key, val );

    GtkTreeIter it_grp;
    GtkTreeIter it_key;
    GtkTreePath* path_grp = NULL;
    GtkTreePath* path_key = NULL;

    path_grp = row_find_child_by_name( dlg, it_ctx, grp );
    if ( path_grp != NULL )
    {
        // XXX: group exists:

        printf( "on_mitem_ctx_add(): GRP [%s] EXISTS\n", grp );

        row_select_by_path_mod( dlg, path_grp );
        gtk_tree_path_free( path_grp );

        row_cur_get_iter( dlg, &it_grp );
        row_data* rdata_grp = row_field_get_data( dlg, &it_grp );

        path_key = row_find_child_by_name( dlg, it_grp, key );
        if ( path_key != NULL )
        {
            // XXX: group exists, key exists

            printf( "on_mitem_ctx_add(): KEY [%s] EXISTS\n", key );

            row_select_by_path_mod( dlg, path_key );
            gtk_tree_path_free( path_key );

            row_cur_get_iter( dlg, &it_key );
            row_data* rdata_key = row_field_get_data( dlg, &it_key );

            if ( rdata_key->inh_ || g_strcmp0( val, rdata_key->val_ ) != 0 )
            {
                // NOTE: conf_chg_val() / conf_save()
                //
                conf_chg_val( rdata_key, val );
                if ( conf_save( rdata_ctx->ctx_, dlg ) )
                {
                    row_field_set_val( dlg, it_key, val );
                    row_key_unset_inh( dlg, it_key );
                    conf_reload_child_ctxs( rdata_ctx->ctx_, dlg );
                }
            }

        }
        else
        {
            // XXX: group exists, key does not

            // NOTE: conf_add_val() / conf_save():
            //
            conf_add_val( rdata_grp, key, val );
            if ( !conf_save( rdata_grp->ctx_, dlg ) )
                return;

            // NOTE: rdata:
            //
            row_data* rdata_new_key = rdata_mk( rdata_grp->ctx_,
                                            rdata_grp->group_, // group
                                            key,               // key
                                            val,               // val
                                            FALSE,             // ro
                                            FALSE,             // inh
                                            RT_KEY             // rtype
                                          );

            GtkTreeIter it_grp_tstrore = row_get_tstore_iter( dlg, it_grp );

            GtkTreeIter it_new_key = tree_add_row( dlg,
                                                   key,
                                                   val,
                                                   rdata_new_key,
                                                   &it_grp_tstrore
                                                 );

            // NOTE: select newly added key node:
            //
            row_select_by_iter_tstore( dlg, it_new_key );

            // NOTE: unset inherited:
            //
            GtkTreeIter it_cur;
            row_cur_get_iter( dlg, &it_cur );

            row_key_unset_inh( dlg, it_cur );

            // NOTE: conf_reload_child_ctxs():
            //
            conf_reload_child_ctxs( rdata_new_key->ctx_, dlg );

        }

    }
    else
    {
        // XXX: group does not exist:

        // NOTE: rdata: new grp:
        //
        row_data* rdata_new_grp = rdata_mk( rdata_ctx->ctx_,
                                    grp,    // group
                                    NULL,   // key
                                    NULL,   // val
                                    FALSE,  // ro
                                    FALSE,  // inh
                                    RT_GRP  // rtype
                                  );


        // NOTE: conf_add_val() / conf_save():
        //
        conf_add_val( rdata_new_grp, key, val );
        if ( !conf_save( rdata_new_grp->ctx_, dlg ) )
        {
            rdata_rm( rdata_new_grp );
            return;
        }


        GtkTreeIter it_ctx_tstrore = row_get_tstore_iter( dlg, it_ctx );

        gchar* display_name = g_strdup_printf( "[%s]", grp );
        GtkTreeIter it_new_grp = tree_add_row( dlg,
                                               display_name,  // name
                                               "",            // val
                                               rdata_new_grp, // rdata
                                               &it_ctx_tstrore
                                             );
        g_free( display_name );

        // NOTE: select newly added grp node:
        //
        row_select_by_iter_tstore( dlg, it_new_grp );


        // NOTE: rdata: new key:
        //
        row_data* rdata_new_key = rdata_mk( rdata_new_grp->ctx_,
                                            rdata_new_grp->group_, // group
                                            key,                   // key
                                            val,                   // val
                                            FALSE,                 // ro
                                            FALSE,                 // inh
                                            RT_KEY                 // rtype
                                          );

        GtkTreeIter it_new_key = tree_add_row( dlg,
                                               key,
                                               val,
                                               rdata_new_key,
                                               &it_new_grp
                                             );

        // NOTE: select newly added key node:
        //
        row_select_by_iter_tstore( dlg, it_new_key );

        // NOTE: conf_reload_child_ctxs():
        //
        conf_reload_child_ctxs( rdata_new_key->ctx_, dlg );

    }

    g_free( grp );
    g_free( key );
    g_free( val );

} // on_mitem_ctx_add()



// TODO: popup_position_func(): convert coords
//
void
popup_position_func( GtkMenu*  menu,
                     gint*     x,
                     gint*     y,
                     gboolean* push_in, // ?
                     gpointer  p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return;


    GtkTreeModel* mod = gtk_tree_view_get_model( dlg->tree_v_ );
    GtkTreePath* path = gtk_tree_model_get_path( mod, &it );

    GdkRectangle rect;

    gtk_tree_view_get_cell_area(
        dlg->tree_v_,
        path,
        gtk_tree_view_get_column( dlg->tree_v_, 0 ),
        &rect );

    gtk_tree_path_free( path );


    gint tx = rect.x;
    gint ty = rect.y;

    gint wx = 0;
    gint wy = 0;

    gtk_window_get_position( GTK_WINDOW( dlg ), &wx, &wy );

    *x = tx + wx;
    *y = ty + wy;

} // popup_position_func()



gboolean
on_popup_menu( GtkWidget* widget, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return FALSE;


    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
        return FALSE;

    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return FALSE;


    GtkMenu* menu = gui_mk_popup_menu( dlg, rdata );
    gtk_menu_popup( menu,
                    NULL,
                    NULL,
                    &popup_position_func,
                    dlg,
                    0,
                    gdk_event_get_time( NULL ) );

    return TRUE; // TRUE => menu was activated

} // on_popup_menu()



void
on_btn_open( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    GtkWidget* odlg = gtk_file_chooser_dialog_new(
        "Choose a folder",
        GTK_WINDOW( dlg ),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
        GTK_STOCK_OPEN,   GTK_RESPONSE_ACCEPT,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
        NULL );

    GtkFileChooser* chooser = GTK_FILE_CHOOSER( odlg );

    gtk_file_chooser_set_local_only( chooser, TRUE );
    gtk_file_chooser_set_create_folders( chooser, TRUE );

    gint response = gtk_dialog_run( GTK_DIALOG( odlg ) );
    if ( response != GTK_RESPONSE_ACCEPT )
    {
        gtk_widget_destroy( odlg );
        return;
    }

    gchar* path = gtk_file_chooser_get_filename( chooser );
    gtk_widget_destroy( odlg );

    if ( !a_open_dir( dlg, path ) )
    {
        GtkWidget* msgdlg =
        gtk_message_dialog_new( GTK_WINDOW( dlg ),
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "Unable to open directory." );

        gtk_window_set_title( GTK_WINDOW( msgdlg ), "lepton-conf" );

        gtk_dialog_run( GTK_DIALOG( msgdlg ) );
        gtk_widget_destroy( msgdlg );
    }

    g_free( path );

} // on_btn_open()



void
on_mitem_bookmark_add( GtkMenuItem* mitem, gpointer p )
{
    gchar* cwd = g_get_current_dir();
    settings_bookmark_add( cwd );
}



void
on_mitem_bookmark_goto( GtkMenuItem* mitem, gpointer p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;

    const gchar* path = gtk_menu_item_get_label( mitem );
    a_open_dir( dlg, path );
}



void
on_btn_bookmarks( GtkButton* btn, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;

    GtkMenu* menu = gui_mk_bookmarks_menu( dlg );
    gtk_menu_popup( menu,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    0,                              // 0 => not a mouse event
                    gtk_get_current_event_time() );
}

