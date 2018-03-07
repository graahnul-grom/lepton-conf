#ifndef CFG_EDIT_DLG_H_
#define CFG_EDIT_DLG_H_

/*
 * Lepton EDA configuration utility
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL 2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */


#include <gtk/gtk.h>



// compare:
//
//#define GSCHEM_TYPE_OPTIONS_WIDGET           (gschem_options_widget_get_type())
//#define GSCHEM_OPTIONS_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_OPTIONS_WIDGET, GschemOptionsWidget))
//#define GSCHEM_OPTIONS_WIDGET_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_OPTIONS_WIDGET, GschemOptionsWidgetClass))
//#define IS_GSCHEM_OPTIONS_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_OPTIONS_WIDGET))
//#define GSCHEM_OPTIONS_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),  GSCHEM_TYPE_OPTIONS_WIDGET, GschemOptionsWidgetClass))


#define CFG_EDIT_DLG_TYPE           (cfg_edit_dlg_get_type())
// cast [obj] to cfg_edit_dlg*:
//
#define CFG_EDIT_DLG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlg))

// cast [cls] to cfg_edit_dlgClass*:
//
#define CFG_EDIT_DLG_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST ((cls), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))
#define IS_CFG_EDIT_DLG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CFG_EDIT_DLG_TYPE))
#define CFG_EDIT_DLG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))




struct _cfg_edit_dlgClass
{
    GtkDialogClass parent_class;
};


struct _cfg_edit_dlg
{
    GtkDialog parent;

    GtkTreeStore* store_;

    GtkWidget*   tree_w_;
    GtkTreeView* tree_v_;

    GtkCellRenderer* ren_txt_;

    GtkWidget* lab_ctx_;
    GtkWidget* lab_fname_;
    GtkWidget* lab_grp_;
    GtkWidget* lab_name_;
    GtkWidget* lab_val_;

    GtkTextBuffer* txtbuf_desc_;

    GtkWidget* btn_showinh_;
    GtkWidget* btn_reload_;
    GtkWidget* btn_add_;
    GtkWidget* btn_edit_;
    GtkWidget* btn_toggle_;

    gboolean showinh_;

};


typedef struct _cfg_edit_dlgClass cfg_edit_dlgClass;
typedef struct _cfg_edit_dlg      cfg_edit_dlg;



GType cfg_edit_dlg_get_type(); // implemented by G_DEFINE_TYPE macro (in .c file)

GtkWidget* cfg_edit_dlg_new();

#endif

