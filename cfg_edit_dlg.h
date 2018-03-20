#ifndef CFG_EDIT_DLG_H_
#define CFG_EDIT_DLG_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include <gtk/gtk.h>
#include <liblepton/liblepton.h>
#include "cfgreg.h"




#define CFG_EDIT_DLG_TYPE           (cfg_edit_dlg_get_type())

// cast [obj] to cfg_edit_dlg*:
//
#define CFG_EDIT_DLG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlg))

// cast [cls] to cfg_edit_dlgClass*:
//
#define CFG_EDIT_DLG_CLASS(cls) (G_TYPE_CHECK_CLASS_CAST ((cls), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))
#define IS_CFG_EDIT_DLG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CFG_EDIT_DLG_TYPE))
#define CFG_EDIT_DLG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))




extern gboolean g_close_with_esc;
extern gboolean g_populate_default_ctx;




struct _cfg_edit_dlgClass
{
    GtkDialogClass parent_class;
};


struct _cfg_edit_dlg
{
    GtkDialog parent;

    GtkTreeStore* store_;

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
    GtkWidget* btn_tst_;

    gboolean showinh_;

};


typedef struct _cfg_edit_dlgClass cfg_edit_dlgClass;
typedef struct _cfg_edit_dlg      cfg_edit_dlg;


GType
cfg_edit_dlg_get_type(); // implemented by G_DEFINE_TYPE macro (in .c file)

GtkWidget*
cfg_edit_dlg_new();




// row data:
//
typedef enum
{
    RT_CTX,
    RT_GRP,
    RT_KEY,
}
RowType;

struct _row_data
{
    EdaConfig* ctx_;
    gchar*     group_;
    gchar*     key_;
    gchar*     val_;
    gboolean   ro_;   // read-only
    gboolean   inh_;  // inherited
    RowType    rtype_;
};

typedef struct _row_data row_data;




// tree columns:
//
enum
{
    COL_NAME,
    COL_VAL,
    COL_DATA, // rdata: hidden
    NUM_COLS
};

int tree_colid_name();
int tree_colid_val();
int tree_colid_data();
int tree_cols_cnt();




// cfg_edit_dlg.c:
//
gboolean g_close_with_esc;
gboolean g_populate_default_ctx;
// TODO: const gchar* g_exted_default;
#define g_exted_default "gvim"

row_data*
mk_rdata( EdaConfig*   ctx,
          const gchar* group,
          const gchar* key,
          const gchar* val,
          gboolean     ro,
          gboolean     inh,
          RowType      rtype );

void
rm_rdata( row_data* rdata );

gboolean
rm_rdata_func( GtkTreeModel* mod,
               GtkTreePath*  path,
               GtkTreeIter*  it,
               gpointer      p );

GtkTreePath*
row_find_child_by_name( cfg_edit_dlg* dlg,
                        GtkTreeIter it_parent,
                        const gchar* name );

void
row_field_set_val( cfg_edit_dlg* dlg, GtkTreeIter it, const gchar* val );

row_data*
row_field_get_data( cfg_edit_dlg* dlg, GtkTreeIter* it );

gboolean
row_cur_get_iter( cfg_edit_dlg* dlg, GtkTreeIter* it );

GtkTreeIter
row_get_tstore_iter( cfg_edit_dlg* dlg, GtkTreeIter it );

void
row_key_unset_inh( cfg_edit_dlg* dlg, GtkTreeIter it );

void
row_select_by_iter_tstore( cfg_edit_dlg* dlg, GtkTreeIter it_tstore );

void
row_select_by_path_mod( cfg_edit_dlg* dlg, GtkTreePath* path_mod );

void
row_select_non_inh( cfg_edit_dlg* dlg, GtkTreeIter it );

gchar*
row_cur_pos_save( cfg_edit_dlg* dlg );

void
row_cur_pos_restore( cfg_edit_dlg* dlg, gchar* path_str );

void
tree_set_focus( cfg_edit_dlg* dlg );

void
xxx_reload( cfg_edit_dlg* dlg );

void
xxx_showinh( cfg_edit_dlg* dlg, gboolean show );

void
xxx_chg_val( cfg_edit_dlg*   dlg,
             const row_data* rdata,
             GtkTreeIter     it,
             const gchar*    txt );

void
xxx_toggle( cfg_edit_dlg* dlg );

void
xxx_update_gui( cfg_edit_dlg* dlg );




// gui_mk.c:
//
gboolean g_gui_update_enabled;
gboolean gui_update_enabled();
void     gui_update_on();
void     gui_update_off();

GtkTreeIter
tree_add_row( cfg_edit_dlg* dlg,
              const gchar*  name,
              const gchar*  val,
              gpointer      rdata,
              GtkTreeIter*  itParent );

void
gui_mk_labels_line_separ( GtkWidget* parent_box );

void
gui_mk_labels_line( const gchar* left_txt,
                    GtkWidget*   right_label,
                    GtkWidget*   parent_box );

GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg );

GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg, const gchar* cwd );

GtkWidget*
gui_mk_tree_view( cfg_edit_dlg* dlg, GtkTreeStore* store );

void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd );

GtkMenu*
gui_mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata );




// dlg.c:
//
gchar*
run_dlg_edit_val( cfg_edit_dlg* dlg,
                  const gchar* txt,
                  const gchar* title );

gboolean
run_dlg_add_val( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** key,
                 gchar** val );

gboolean
run_dlg_add_val_2( cfg_edit_dlg* dlg,
                 const gchar* title,
                 gchar** grp,
                 gchar** key,
                 gchar** val );




// conf.c:
//
gboolean
conf_is_hidden_key( const gchar* name );

gboolean
conf_is_hidden_group( const gchar* name );

void
conf_load_keys( EdaConfig*    ctx,
                const gchar*  group,
                cfg_edit_dlg* dlg,
                GtkTreeIter   itParent,
                gboolean      wok,
                gboolean*     inh_all );

void
conf_load_groups( EdaConfig*    ctx,
                  gboolean      wok,
                  cfg_edit_dlg* dlg,
                  GtkTreeIter   itParent );

gboolean
conf_load_ctx( EdaConfig* ctx );

GtkTreeIter
conf_mk_ctx_node( EdaConfig*    ctx,
                  gboolean      wok,
                  const gchar*  name,
                  cfg_edit_dlg* dlg );

void
conf_reload_child_ctxs( EdaConfig* parent_ctx, cfg_edit_dlg* dlg );

void
conf_load( cfg_edit_dlg* dlg );

const gchar*
conf_ctx_name( EdaConfig* ctx );

const gchar*
conf_ctx_fname( EdaConfig* ctx, gboolean* exist, gboolean* rok, gboolean* wok );

const gboolean
conf_ctx_file_writable( EdaConfig* ctx );

void
conf_add_val( row_data* rdata, const gchar* key, const gchar* val );

void
conf_chg_val( const row_data* rdata, const gchar* txt );

gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg );




// sett.c:
//
void
settings_restore( GtkWidget* widget );

void
settings_save( GtkWidget* widget );




// events.c:
//
void
events_setup( cfg_edit_dlg* dlg );

void
on_delete_event( cfg_edit_dlg* dlg, GdkEvent* e, gpointer* p );

void
on_row_sel( GtkTreeView* tree, gpointer* p );

void
on_btn_reload( GtkButton* btn, gpointer* p );

void
on_btn_tst( GtkButton* btn, gpointer* p );

void
on_btn_add( GtkButton* btn, gpointer* p );

void
on_btn_edit( GtkButton* btn, gpointer* p );

void
on_btn_toggle( GtkButton* btn, gpointer* p );

void
on_lab_fname( GtkLabel* lab, gpointer* p );

void
on_btn_showinh( GtkToggleButton* btn, gpointer* p );

void
on_mitem_key_edit( GtkMenuItem* mitem, gpointer p );

void
on_mitem_grp_add( GtkMenuItem* mitem, gpointer p );

void
on_mitem_ctx_add( GtkMenuItem* mitem, gpointer p );

gboolean
on_mouse_click( GtkWidget* w, GdkEvent* e, gpointer p );

gboolean
on_key_press( GtkWidget* w, GdkEvent* e, gpointer p );




#endif

