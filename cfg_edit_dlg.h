#ifndef CFG_EDIT_DLG_H_
#define CFG_EDIT_DLG_H_

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



enum CFG_EDIT_DLG_PROP_IDS
{
    CFG_EDIT_DLG_PROPID_STUB, // prop ids must be > 0
    CFG_EDIT_DLG_PROPID_PROP1
};



struct _cfg_edit_dlgClass
{
    GtkDialogClass parent_class;
};

struct _cfg_edit_dlg
{
    GtkDialog parent;

    int prop1_;

    GtkTreeStore* store_;
    GtkTreeModel* model_;   // active model
//    GtkTreeModel* model_f_; // filtered model
//    GtkTreeModel* model_a_; // unfiltered model (i.e. all items)
    GtkWidget*    tree_w_;
    GtkTreeView*  tree_v_;
    GtkCellRenderer* ren_txt_;

    GtkWidget*    ent_;
    GtkWidget*    btn_apply_;
    GtkWidget*    btn_reload_;

    gboolean showinh_;
};

typedef struct _cfg_edit_dlgClass cfg_edit_dlgClass;
typedef struct _cfg_edit_dlg      cfg_edit_dlg;



GType cfg_edit_dlg_get_type(); // implemented by G_DEFINE_TYPE macro (in .c file)


#endif

