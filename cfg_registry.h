#ifndef CFG_REGISTRY_H_
#define CFG_REGISTRY_H_

/*
 * Lepton EDA configuration utility
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL 2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */



// struct represents a configuration key:
//
struct _CfgEntry
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* seed_;
    const gchar* desc_;
};

typedef struct _CfgEntry CfgEntry;


static CfgEntry g_cfg_registry[] =
{
    {
        "sys",
        "newKey",
        "sys__newKey",
        "eklmn oprst"
    },
    {
        "gschem",
        "default-filename",
        "gschem__default-filename",
        "Define the default file name for any new schematic files created in gschem.\n"
        "It is used to create filenames of the form “untitled_N.sch” where N is a number."
    },
    {
        "schematic.gui",
        "use-docks",
        "schematic.gui__use-docks",
//        "The type of GUI is controlled by \"use-docks\" boolean"
//        " configuration key in \"gschem.gui\" group."
//        " If widgets will be shown"
//        " in docks (true) or as"
//        " a dialog boxes (false)"

        "The type of GUI: how to display widgets.\n"
        " If true, widgets will be shown in docks.\n"
        " If false, widgets will be shown as a dialog boxes."

//        "The type of GUI is controlled by \"use-docks\" boolean configuration key in \"gschem.gui\" group. If widgets will be shown in docks (true) or as a dialog boxes (false)"
    },
    {
        NULL,
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

    char* seed = g_strdup_printf( "%s__%s", grp, key );
    const CfgEntry* ret = NULL;

    for ( ; entry->seed_ != NULL; ++entry )
    {
        if ( g_strcmp0( entry->seed_, seed ) == 0 )
        {
            ret = entry;
            break;
        }
    }

    g_free( seed );

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

