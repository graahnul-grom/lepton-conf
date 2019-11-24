/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#ifdef DEBUG
    #define G_LOG_DOMAIN "lepton-conf"
#endif

#include "proto.h"




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



// "show" handler for dialog:
// NOTE: called *after* cfg_edit_dlg_init()
//
static void
cfg_edit_dlg_on_show( GtkWidget* widget )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

//    settings_restore_showinh( dlg );
//    settings_restore_path( dlg );
    settings_wnd_geom_restore( GTK_WINDOW( dlg ), NULL );

    GTK_WIDGET_CLASS(cfg_edit_dlg_parent_class)->show(widget);
}



static void
cfg_edit_dlg_on_unmap( GtkWidget* widget )
{
    cfg_edit_dlg* dlg = CFG_EDIT_DLG( widget );

    settings_save( dlg );

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

    a_init( dlg );

//    printf( " -- cfg_edit_dlg_init()\n" );
}

