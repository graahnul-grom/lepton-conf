#include "sett_list.h"
#include "proto.h"

gchar*
settings_list_to_string( GList* lst )
{
    GString* gstr = g_string_new( NULL );

    for ( GList* p = lst; p != NULL; p = p->next )
    {
        gchar* item = (gchar*) p->data;

        gstr = g_string_append( gstr, item );
        gstr = g_string_append( gstr, ";" );
    }

    gchar* str = g_string_free( gstr, FALSE );

    return str;

} // settings_list_to_string()



void
settings_list_save( GList* lst, const gchar* name )
{
    gchar* str = settings_list_to_string( lst );

    EdaConfig* ctx = eda_config_get_cache_context();
    eda_config_set_string( ctx, "lepton-conf", name, str );
    eda_config_save( ctx, NULL );

    g_free( str );
}



gboolean
settings_list_add( GList* lst, gchar* item )
{
    GList* found = g_list_find_custom( lst,
                                       item,
                                       (GCompareFunc) &g_strcmp0 );

    if ( found == NULL )
    {
        lst = g_list_append( lst, item );
        return TRUE;
    }

    return FALSE;

} // settings_list_add()



void
settings_list_load( GList* lst, const gchar* name )
{
    EdaConfig* ctx = eda_config_get_cache_context();

    gsize len = 0;
    GError* err = NULL;
    gchar** strs = eda_config_get_string_list( ctx,
                                               "lepton-conf",
                                               name,
                                               &len,
                                               &err );
    if ( len > 0 && err == NULL )
    {
        for ( int i = 0; i < len; ++i )
        {
            gchar* item = strs[ i ];
            settings_list_add( lst, g_strdup( item ) );
        }
    }

    g_strfreev( strs );

} // settings_list_load()



void
settings_list_clear( GList** lst )
{
    g_list_free_full( *lst, &g_free );

    *lst = NULL;
}

