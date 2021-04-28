#ifndef LEPTON_CONF_DLG_ATTRS_H_
#define LEPTON_CONF_DLG_ATTRS_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include <gtk/gtk.h>


#define ATTRS_DLG_TYPE           (attrs_dlg_get_type())
#define ATTRS_DLG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATTRS_DLG_TYPE, AttrsDlg))
#define ATTRS_DLG_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST ((cls), ATTRS_DLG_TYPE, AttrsDlgClass))
#define IS_ATTRS_DLG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATTRS_DLG_TYPE))
#define ATTRS_DLG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ATTRS_DLG_TYPE, AttrsDlgClass))


struct _AttrsDlgClass
{
    GtkDialogClass parent_class;
};

struct _AttrsDlg
{
    GtkDialog parent;

    GtkListStore* store_;
    GtkTreeView*  tree_v_;
    GList*        items_;
    GList*        items_result_;

    GtkWidget* btn_edit_;
    GtkWidget* btn_add_;
    GtkWidget* btn_remove_;
    GtkWidget* btn_move_up_;
    GtkWidget* btn_move_down_;

    gchar* dlg_name_;
};

typedef struct _AttrsDlgClass AttrsDlgClass;
typedef struct _AttrsDlg      AttrsDlg;


GType
attrs_dlg_get_type();

gchar*
attrs_dlg_run( const gchar* value, const gchar* title, const gchar* dlg_name );

#ifdef DEBUG
void
attrs_dlg_dbg_print_items( GList* items );
#endif

#endif

