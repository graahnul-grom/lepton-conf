#include "cfgreg.h"




// private:
// find CfgEntry in global cfg registry by group name and key name
//
const CfgEntry*
cfgreg_lookup( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = g_cfg_registry;
    const CfgEntry* ret   = NULL;

    for ( ; entry->grp_ != NULL; ++entry )
    {
        gboolean cond1 = g_strcmp0( entry->grp_, grp ) == 0;
        gboolean cond2 = g_strcmp0( entry->key_, key ) == 0;

        if ( cond1 && cond2 )
        {
            ret = entry;
            break;
        }
    }

    return ret;
}



// public:
// convenience func:
// find CfgEntry in global cfg registry by group name and key name,
//   and if found, return description for that entry
//
const gchar*
cfgreg_lookup_descr( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = cfgreg_lookup( grp, key );

    if ( entry != NULL)
        return entry->desc_;

    return NULL;
}



// public:
//
gboolean
cfgreg_can_toggle( const gchar* val )
{
    gboolean can_toggle = FALSE;

    can_toggle |= g_strcmp0( val, "true"     ) == 0;
    can_toggle |= g_strcmp0( val, "false"    ) == 0;
    can_toggle |= g_strcmp0( val, "enabled"  ) == 0;
    can_toggle |= g_strcmp0( val, "disabled" ) == 0;

    return can_toggle;
}



// public:
//
void
cfgreg_populate_ctx( EdaConfig* ctx )
{
    const CfgEntry* entry = g_cfg_registry;

    for ( ; entry->grp_ != NULL; ++entry )
    {
        const gchar* grp     = entry->grp_;
        const gchar* key     = entry->key_;
        const gchar* def_val = entry->def_val_;

        eda_config_set_string( ctx, grp, key, def_val );
    }
}

