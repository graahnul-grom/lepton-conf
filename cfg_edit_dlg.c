/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif

#include "cfg_edit_dlg.h"




/* ******************************************************************
*
*  gobject stuff:
*
*/

G_DEFINE_TYPE(cfg_edit_dlg, cfg_edit_dlg, GTK_TYPE_DIALOG);



GtkWidget* cfg_edit_dlg_new()
{
  gpointer obj = g_object_new (CFG_EDIT_DLG_TYPE, NULL);
  return GTK_WIDGET (obj);
}



static void
cfg_edit_dlg_get_property( GObject* obj, guint id, GValue* val, GParamSpec* spec )
{
    // cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );
    G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
}



static void
cfg_edit_dlg_set_property( GObject* obj, guint id, const GValue* val, GParamSpec* spec )
{
    // cfg_edit_dlg* dlg = CFG_EDIT_DLG( obj );
    G_OBJECT_WARN_INVALID_PROPERTY_ID( obj, id, spec );
}



static void
cfg_edit_dlg_dispose( GObject* obj )
{
    // g_debug( "cfg_edit_dlg::dispose( %p ); refcnt: %d\n",
        // obj, obj ? obj->ref_count : 0 );

    cfg_edit_dlgClass* cls = CFG_EDIT_DLG_GET_CLASS( obj );

    GObjectClass* parent_cls = g_type_class_peek_parent( cls );
    parent_cls->dispose( obj );

    // g_debug( "cfg_edit_dlg::dispose(): done.\n" );
}



static void
cfg_edit_dlg_on_show( GtkWidget* widget )
{
    settings_restore( widget );
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->show(widget);
}



static void
cfg_edit_dlg_on_unmap( GtkWidget* widget )
{
    settings_save( widget );
    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->unmap(widget);
}



static void
cfg_edit_dlg_class_init( cfg_edit_dlgClass* cls )
{
//    printf( " >> cfg_edit_dlg_class_init()\n" );

    // NOTE: glib debug messages:
    // NOTE: g_debug() adds newline:
    //
    // g_debug( "cfg_edit_dlg::class_init()" );

    GObjectClass* gcls = G_OBJECT_CLASS( cls );

    gcls->dispose      = &cfg_edit_dlg_dispose;
    gcls->get_property = &cfg_edit_dlg_get_property;
    gcls->set_property = &cfg_edit_dlg_set_property;


    GtkWidgetClass* wcls = GTK_WIDGET_CLASS( cls );

    // setup onShow/onClose handlers for dialog:
    //
    wcls->show =  &cfg_edit_dlg_on_show;
    wcls->unmap = &cfg_edit_dlg_on_unmap;
}



static void
cfg_edit_dlg_init( cfg_edit_dlg* dlg )
{
//    printf( " ++ cfg_edit_dlg_init()\n" );

    // g_debug( "cfg_edit_dlg::cfg_edit_dlg_init()\n" );

    // by default, do not show inherited:
    //
    dlg->showinh_ = FALSE;


    gchar* cwd = g_get_current_dir();
    gui_mk( dlg, cwd );
    g_free( cwd );

    // load data:
    //
    conf_load( dlg );

    tree_filter_setup( dlg );
    gtk_widget_show_all( GTK_WIDGET( dlg ) );
    events_setup( dlg );

    tree_set_focus( dlg );
    gui_update_on();
    gui_update( dlg );

//    printf( " -- cfg_edit_dlg_init()\n" );
}

