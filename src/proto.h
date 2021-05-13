#ifndef LEPTON_CONF_PROTO_H_
#define LEPTON_CONF_PROTO_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2021 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include <gtk/gtk.h>
#include <liblepton/liblepton.h>

#include "sett_list.h"




#define CFG_EDIT_DLG_TYPE (cfg_edit_dlg_get_type())

// cast [obj] to cfg_edit_dlg*:
//
#define CFG_EDIT_DLG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlg))

// cast [cls] to cfg_edit_dlgClass*:
//
#define CFG_EDIT_DLG_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST ((cls), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))
#define IS_CFG_EDIT_DLG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CFG_EDIT_DLG_TYPE))
#define CFG_EDIT_DLG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), CFG_EDIT_DLG_TYPE, cfg_edit_dlgClass))




struct _cfg_edit_dlgClass
{
    GtkDialogClass parent_class;
};

struct _cfg_edit_dlg
{
    GtkDialog parent;

    GtkTreeStore*    store_;
    GtkTreeView*     tree_v_;

    GtkWidget* lab_cwd_;
    GtkWidget* lab_ctx_;
    GtkWidget* lab_fname_;
    GtkWidget* lab_grp_;
    GtkWidget* lab_key_;
    GtkWidget* lab_dflt_;
    GtkWidget* lab_val_;

    GtkWidget* btn_open_;
    GtkWidget* btn_bmks_;
    GtkWidget* btn_tools_;
    GtkWidget* btn_showinh_;
    GtkWidget* btn_reload_;
    GtkWidget* btn_add_;
    GtkWidget* btn_edit_;
    GtkWidget* btn_toggle_;
    GtkWidget* btn_del_;
    GtkWidget* btn_tst_;
    GtkWidget* btn_hlp_;

    GtkTextBuffer* txtbuf_desc_;

    gboolean showinh_;
};

typedef struct _cfg_edit_dlgClass cfg_edit_dlgClass;
typedef struct _cfg_edit_dlg      cfg_edit_dlg;




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




struct _row_cgk
{
    EdaConfig* ctx_;
    gchar* grp_;
    gchar* key_;
};

typedef struct _row_cgk row_cgk;




// main.c:
//
extern gboolean g_cfg_legacy_mode;
extern gboolean g_close_with_esc;
extern gboolean g_populate_default_ctx;
extern gchar*   g_ext_editor;
extern gboolean g_warn_cfg_file_not_found;
extern gboolean g_print_default_cfg;
extern gboolean g_tst_btn_visible;
extern gboolean g_restore_last_dir;




// cfg_edit_dlg.c:
//
GType
cfg_edit_dlg_get_type(); // implemented by G_DEFINE_TYPE macro (in .c file)

GtkWidget*
cfg_edit_dlg_new();





// gui.c:
//
extern gboolean g_gui_update_enabled;

gboolean gui_update_enabled();
void     gui_update_on();
void     gui_update_off();

void
gui_update( cfg_edit_dlg* dlg );

void
gui_off( cfg_edit_dlg* dlg );

void
gui_mk_labels_line_separ( GtkWidget* parent_box, gint padding );

void
gui_mk_labels_line( const gchar* left_txt,
                    GtkWidget*   right_label,
                    GtkWidget*   parent_box );

GtkWidget*
gui_mk_toolbar( cfg_edit_dlg* dlg );

GtkWidget*
gui_mk_bottom_box( cfg_edit_dlg* dlg );

GtkWidget*
gui_mk_tree_view( cfg_edit_dlg* dlg, GtkTreeStore* store );

void
gui_mk( cfg_edit_dlg* dlg, const gchar* cwd );

GtkMenu*
gui_mk_popup_menu( cfg_edit_dlg* dlg, row_data* rdata );

GtkMenu*
gui_mk_bookmarks_menu( cfg_edit_dlg* dlg );

GtkMenu*
gui_mk_tools_menu( cfg_edit_dlg* dlg );




// dlg.c:
//
gchar*
run_dlg_edit_val( GtkWindow* parent,
                  const gchar*  txt,
                  const gchar*  title );

gboolean
run_dlg_add_key_val( cfg_edit_dlg* dlg,
                     const gchar*  title,
                     gchar**       key,
                     gchar**       val );

gboolean
run_dlg_add_grp_key_val( cfg_edit_dlg* dlg,
                         const gchar*  title,
                         gchar**       grp,
                         gchar**       key,
                         gchar**       val );

gchar*
run_dlg_list_sel( cfg_edit_dlg* dlg,
                  GList*        names,
                  const gchar*  txt,
                  const gchar*  title );

void
run_dlg_hlp();




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
                gboolean*     inh_all,
                gboolean      print );

void
conf_load_groups( EdaConfig*    ctx,
                  gboolean      wok,
                  cfg_edit_dlg* dlg,
                  GtkTreeIter   itParent,
                  gboolean      print );

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
conf_del_key( const row_data* rdata );

gboolean
conf_del_grp( const row_data* rdata );

gboolean
conf_save( EdaConfig* ctx, cfg_edit_dlg* dlg );




// sett.c:
//

extern GList* g_bookmarks;
extern GList* g_tools;

void
settings_restore_showinh( cfg_edit_dlg* dlg );

void
settings_restore_path( cfg_edit_dlg* dlg );

void
settings_save( cfg_edit_dlg* dlg );

void
settings_save_wnd_geom( GtkWindow* wnd, const gchar* name );

void
settings_restore_wnd_geom( GtkWindow* wnd, const gchar* name );

void
settings_restore_last_dir();

void
settings_read_close_with_esc();

void
settings_read_editor();

void
settings_read_restore_last_dir();




// events.c:
//
void
events_setup( cfg_edit_dlg* dlg );

void
on_btn_tst( GtkButton* btn, gpointer* p );

void
on_tree_sel_changed( GtkTreeSelection* sel, gpointer p );

void
on_row_sel( GtkTreeView* tree, gpointer* p );

void
on_delete_event( cfg_edit_dlg* dlg, GdkEvent* e, gpointer* p );

void
on_lab_fname( GtkLabel* lab, gchar* uri, gpointer* p );

gboolean
on_mouse_click( GtkWidget* w, GdkEvent* e, gpointer p );

gboolean
on_key_press( GtkWidget* w, GdkEvent* e, gpointer p );

void
on_btn_toggle( GtkButton* btn, gpointer* p );

void
on_btn_reload( GtkButton* btn, gpointer* p );

void
on_btn_showinh( GtkToggleButton* btn, gpointer* p );

void
on_btn_hlp( GtkToggleButton* btn, gpointer* p );

void
on_btn_edit( GtkButton* btn, gpointer* p );

void
on_btn_open( GtkButton* btn, gpointer* p );

void
on_btn_bookmarks( GtkButton* btn, gpointer* p );

void
on_btn_tools( GtkButton* btn, gpointer* p );

void
on_mitem_key_edit( GtkMenuItem* mitem, gpointer p );

void
on_mitem_del( GtkMenuItem* mitem, gpointer p );

void
on_mitem_rest_dflt( GtkMenuItem* mitem, gpointer p );

void
on_mitem_sel_font( GtkMenuItem* mitem, gpointer p );

void
on_mitem_sel_paper_size( GtkMenuItem* mitem, gpointer p );

void
on_mitem_sel_color( GtkMenuItem* mitem, gpointer p );

void
on_mitem_edit_attrs( GtkMenuItem* mitem, gpointer p );

void
on_btn_add( GtkButton* btn, gpointer* p );

void
on_mitem_grp_add( GtkMenuItem* mitem, gpointer p );

void
on_mitem_ctx_add( GtkMenuItem* mitem, gpointer p );

void
on_mitem_toggle( GtkMenuItem* mitem, gpointer p );

void
on_mitem_bookmark_add( GtkMenuItem* mitem, gpointer p );

void
on_mitem_bookmarks_manage( GtkMenuItem* mitem, gpointer p );

void
on_mitem_bookmark_goto( GtkMenuItem* mitem, gpointer p );

void
on_mitem_tools_manage( GtkMenuItem* mitem, gpointer p );

void
on_mitem_tool_execute( GtkMenuItem* mitem, gpointer p );

gboolean
on_popup_menu( GtkWidget* widget, gpointer p );




// tree.c:
//
row_data*
rdata_mk( EdaConfig*   ctx,
          const gchar* group,
          const gchar* key,
          const gchar* val,
          gboolean     ro,
          gboolean     inh,
          RowType      rtype );

void
rdata_rm( row_data* rdata );

gboolean
rdata_rm_func( GtkTreeModel* mod,
               GtkTreePath*  path,
               GtkTreeIter*  it,
               gpointer      p );

const gchar*
rdata_get_name( const row_data* rdata );

row_cgk*
cgk_mk( const row_data* rdata );

void
cgk_rm( row_cgk* rdata );

int tree_colid_name();
int tree_colid_val();
int tree_colid_data();
int tree_cols_cnt();

void
tree_set_focus( cfg_edit_dlg* dlg );

void tree_add_column_1( cfg_edit_dlg* dlg );
void tree_add_column_2( cfg_edit_dlg* dlg );

GtkTreeIter
tree_add_row( cfg_edit_dlg* dlg,
              const gchar*  name,
              const gchar*  val,
              gpointer      rdata,
              GtkTreeIter*  itParent );

void
tree_filter_setup( cfg_edit_dlg* p );

void
tree_filter_remove( cfg_edit_dlg* p );

gint
tree_sort_cmp_fun( GtkTreeModel* model,
                   GtkTreeIter*  a,
                   GtkTreeIter*  b,
                   gpointer      data );




// row.c:
//
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

void
row_select_parent( cfg_edit_dlg* dlg, GtkTreeIter it );

void
row_select_by_ctx_grp_key( cfg_edit_dlg* dlg,
                           EdaConfig*   ctx,
                           const gchar* grp_name,
                           const gchar* key_name );

void
row_select_by_cgk( cfg_edit_dlg* dlg, const row_cgk* cgk );

gchar*
row_cur_pos_save( cfg_edit_dlg* dlg );

void
row_cur_pos_restore( cfg_edit_dlg* dlg, gchar* path_str );




// actions.c:
//
void
a_init( cfg_edit_dlg* dlg );

void
a_reload( cfg_edit_dlg* dlg );

void
a_showinh( cfg_edit_dlg* dlg, gboolean show );

void
a_chg_val( cfg_edit_dlg*   dlg,
           const row_data* rdata,
           GtkTreeIter     it,
           const gchar*    txt );

void
a_toggle( cfg_edit_dlg* dlg );

void
a_delete( cfg_edit_dlg* dlg );

void
a_run_editor( cfg_edit_dlg* dlg, const gchar* fname_to_edit );

gboolean
a_open_dir( cfg_edit_dlg* dlg, const char* path );

gboolean
try_chdir( const gchar* path );




// cfgreg.c:
//

// struct represents a legacy gEDA configuration key:
//
struct _CfgEntryLegacy
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* def_val_; // default value
    const gchar* desc_;
};

typedef struct _CfgEntryLegacy CfgEntryLegacy;

// struct represents a configuration key:
// NOTE: unique( grp, key )
//
struct _CfgEntry
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* def_val_; // default value
    const gchar* desc_;
    const CfgEntryLegacy legacy_;
};

typedef struct _CfgEntry CfgEntry;

// cfg key with predefined set of values
//
struct _CfgEntryEnum
{
    gchar* grp_;
    gchar* key_;
    GList* vals_; // possible values of grp::key (list of strings)
};

typedef struct _CfgEntryEnum CfgEntryEnum;


const CfgEntry*
cfgreg_lookup( const gchar* grp, const gchar* key );

// convenience func:
// find CfgEntry in global cfg registry by group name and key name,
//   and if found, return description for that entry
//
const gchar*
cfgreg_lookup_descr( const gchar* grp, const gchar* key );

const gchar*
cfgreg_lookup_dflt_val( const gchar* grp, const gchar* key );

gboolean
cfgreg_can_toggle( const row_data* rdata );

void
cfgreg_populate_ctx( EdaConfig* ctx );

void
cfgreg_init();

const gchar*
cee_next_val( const gchar* grp, const gchar* key, const gchar* current );



#endif /* LEPTON_CONF_PROTO_H_ */

