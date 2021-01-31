#include "attrs_dlg.h"

G_DEFINE_TYPE(attrs_dlg, attrs_dlg, GTK_TYPE_DIALOG);


GtkWidget* attrs_dlg_new()
{
  gpointer obj = g_object_new( ATTRS_DLG_TYPE, NULL );
  return GTK_WIDGET( obj );
}



static void
attrs_dlg_class_init( attrs_dlgClass* cls )
{
}



static void
attrs_dlg_init( attrs_dlg* dlg )
{
    printf( " ++ attrs_dlg_init()\n" );
}

