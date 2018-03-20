#include "cfg_edit_dlg.h"
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

    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-link",             // mouse click
                      G_CALLBACK( &on_lab_fname ),
                      dlg );
    g_signal_connect( G_OBJECT( dlg->lab_fname_ ),
                      "activate-current-link",     // press Enter key
                      G_CALLBACK( &on_lab_fname ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_reload_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_reload ),
                      dlg );

    g_signal_connect( G_OBJECT( dlg->btn_tst_ ),
                      "clicked",
                      G_CALLBACK( &on_btn_tst ),
                      dlg );

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

    g_signal_connect( G_OBJECT( dlg->tree_v_ ),
                      "cursor-changed",         // tree sel changed
//                      "row-activated",
                      G_CALLBACK( &on_row_sel ),
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

    gchar* path = row_cur_pos_save( dlg );

    gui_update_off();

//    GtkAdjustment* adj = gtk_tree_view_get_vadjustment( dlg->tree_v_ );
//    gtk_tree_view_set_vadjustment( dlg->tree_v_, NULL );

    a_reload( dlg );
    // conf_reload_ctx( ctx, "3", dlg );

//    gtk_tree_view_set_vadjustment( dlg->tree_v_, adj );

    tree_set_focus( dlg );

    gui_update_on();

    row_cur_pos_restore( dlg, path );
    g_free( path );

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

void
on_row_sel( GtkTreeView* tree, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    gui_update( dlg );
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



void
on_lab_fname( GtkLabel* lab, gpointer* p )
{
    cfg_edit_dlg* dlg = (cfg_edit_dlg*) p;
    if ( !dlg )
        return;

    const gchar* fname = gtk_label_get_current_uri( lab );

    // external editor [read config]:
    //
    GError* err = NULL;
    gchar* exted = eda_config_get_string( eda_config_get_user_context(),
                                          "lepton-conf",
                                          "editor",
                                          &err );
    g_clear_error( &err );

    if (!exted)
        exted = g_strdup( g_exted_default );


    GAppInfo* ai =
    g_app_info_create_from_commandline( exted,
                                        NULL,
                                        G_APP_INFO_CREATE_NONE,
                                        &err );
    if ( err )
    {
        printf( " >> on_lab_fname(): GAppInfo err: [%d], msg: [%s]\n",
                err->code, err->message );
        g_clear_error( &err );
    }

    if ( ai )
    {
        GFile* gfile = g_file_new_for_path( fname );
        GList* args = g_list_append( NULL, gfile );

        if ( !g_app_info_launch( ai, args, NULL, &err ) )
        {
            printf( " >> on_lab_fname(): LAUNCH err: [%d], msg: [%s]\n",
                    err->code, err->message );

            GtkWidget* mdlg = gtk_message_dialog_new(
                NULL,
                // GTK_WINDOW( dlg ), // TODO: => SIGBUS
                (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Could not launch external editor [%s].\n"
                "Please set it via [lepton-conf]::editor config key."
                , exted );

            gtk_dialog_run( GTK_DIALOG( mdlg ) );
            gtk_widget_destroy( mdlg );
        }

        g_clear_error( &err );
        g_list_free( args );
    }


    // external editor [write config]: TODO: reload config here?
    //
    eda_config_set_string( eda_config_get_user_context(),
                           "lepton-conf",
                           "editor",
                           exted );

    eda_config_save( eda_config_get_user_context(), &err );
    g_clear_error( &err );

    g_free( exted );

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

    GdkEventButton* ebtn = ( GdkEventButton* ) e;


    // LMB double click:
    //
    if ( ebtn->type == GDK_2BUTTON_PRESS && ebtn->button == 1 )
    {
        a_toggle( dlg );
        return FALSE;
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

    GtkTreeIter it;
    if ( !row_cur_get_iter( dlg, &it ) )
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


    row_data* rdata = row_field_get_data( dlg, &it );
    if ( !rdata )
        return TRUE;

    GtkMenu* menu = gui_mk_popup_menu( dlg, rdata );

    if ( menu )
    {
        gtk_menu_popup( menu, NULL, NULL, NULL, NULL,
                        ebtn->button, ebtn->time );
                        // 0, gtk_get_current_event_time() );
    }

    return TRUE;

} // on_rmb()



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
on_btn_reload( GtkButton* btn, gpointer* p )
{
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

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

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

    gchar* txt = run_dlg_edit_val( dlg, rdata->val_, NULL );

    a_chg_val( dlg, rdata, it, txt );

    g_free( txt );

} // on_mitem_key_edit()




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

    if ( !run_dlg_add_val( dlg, NULL, &key, &val ) )
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

    // NOTE: unset inherited:
    //
    GtkTreeIter it_cur;
    if ( row_cur_get_iter( dlg, &it_cur ) )
        row_key_unset_inh( dlg, it_cur );

    // NOTE: conf_reload_child_ctxs()
    //
    conf_reload_child_ctxs( rdata->ctx_, dlg );


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

    if ( !run_dlg_add_val_2( dlg, NULL, &grp, &key, &val ) )
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

