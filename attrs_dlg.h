#ifndef LEPTON_CONF_DLG_ATTRS_H_
#define LEPTON_CONF_DLG_ATTRS_H_

#include <gtk/gtk.h>


#define ATTRS_DLG_TYPE           (attrs_dlg_get_type())
#define ATTRS_DLG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATTRS_DLG_TYPE, attrs_dlg))
#define ATTRS_DLG_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST ((cls), ATTRS_DLG_TYPE, attrs_dlgClass))
#define IS_ATTRS_DLG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATTRS_DLG_TYPE))
#define ATTRS_DLG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ATTRS_DLG_TYPE, attrs_dlgClass))


struct _attrs_dlgClass
{
    GtkDialogClass parent_class;
};

struct _attrs_dlg
{
    GtkDialog parent;

    GtkTreeStore* store_;
    GtkTreeView*  tree_v_;
};

typedef struct _attrs_dlgClass attrs_dlgClass;
typedef struct _attrs_dlg      attrs_dlg;


GType
attrs_dlg_get_type();

GtkWidget*
attrs_dlg_new();

#endif

