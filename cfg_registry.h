#ifndef CFG_REGISTRY_H_
#define CFG_REGISTRY_H_

/*
 * Lepton EDA configuration utility
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL 2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */



// struct represents a configuration key:
// NOTE: unique( grp, key )
//
struct _CfgEntry
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* desc_;
};

typedef struct _CfgEntry CfgEntry;


static CfgEntry g_cfg_registry[] =
{
    {
        "sys",
        "newKey",
        "eklmn oprst"
    },
    {
        "gschem",
        "default-filename",
        "Define the default file name for any new schematic files created in gschem.\n"
        "It is used to create filenames of the form “untitled_N.sch” where N is a number."
    },
    {
        "schematic.gui",
        "use-docks",
        "The type of GUI: how to display widgets.\n"
        " If true, widgets will be shown in docks.\n"
        " If false, widgets will be shown as a dialog boxes."
    },
    {
        NULL,
        NULL,
        NULL
    }
};


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
static const gchar*
cfgreg_lookup_descr( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = cfgreg_lookup( grp, key );

    if ( entry != NULL)
        return entry->desc_;

    return NULL;
}


#endif /* CFG_REGISTRY_H_ */

