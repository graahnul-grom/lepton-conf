#ifndef LEPTON_CONF_CFG_REG_H_
#define LEPTON_CONF_CFG_REG_H_

/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2018 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include <liblepton/liblepton.h>




// struct represents a configuration key:
// NOTE: unique( grp, key )
//
struct _CfgEntry
{
    const gchar* grp_;
    const gchar* key_;
    const gchar* def_val_; // default value
    const gchar* desc_;
};

typedef struct _CfgEntry CfgEntry;


static CfgEntry g_cfg_registry[] =
{
    //
    // lepton-conf:
    //
    {
        "lepton-conf",
        "editor",
        "gvim",
        "External text editor for configuration files used by lepton-conf (this program).\n"
        "Currently, it is stored/read from the USER config context only."
    },

    //
    // lepton-schematic:
    //
    {
        "gschem",
        "default-filename",
        "untitled",
        "Default file name for any new schematic files.\n"
        "It is used to create filenames of the form “untitled_N.sch” where N is a number."
    },
    {
        "schematic.gui",
        "use-docks",
        "true",
        "How to display widgets: as dialogs or inside the dock widgets."
    },
    {
        "schematic.gui",
        "use-tabs",
        "false",
        "Whether to use tabbed GUI: display each schematic in its own tab within GtkNotebook widget."
    },
    {
        "schematic.tabs",
        "show-close-button",
        "true",
        "Whether to show \"close\" button on each tab."
    },
    {
        "schematic.tabs",
        "show-up-button",
        "true",
        "Whether to show \"hierarchy up\" button on each tab."
    },
    {
        "schematic.undo",
        "modify-viewport",
        "false",
        "Allow undo/redo operations to change pan and zoom."
    },
    {
        "schematic.log-window",
        "font",
        "",
        "Custom font for the log window (e.g. \"Monospace 10\")."
    },

    //
    // lepton-netlist:
    //
    {
        "gnetlist",
        "default-net-name",
        "unnamed_net",
        "Default name used for nets for which the user has set\n"
        "no explicit name via the netname= or net= attributes."
    },
    {
        "gnetlist",
        "default-bus-name",
        "unnamed_bus",
        "Default name used for buses for which the user has set\n"
        "no explicit name via the netname= or net= attributes."
    },
    {
        "gnetlist",
        "net-naming-priority",
        "net-attribute",
        "Specify which attribute, net (\"net-attribute\") or netname (\"netname-attribute\"),"
        " has priority if a net is found with two names. Any netname"
        " conflict will be resolved using the chosen attribute."
    },
    {
        "gnetlist.hierarchy",
        "traverse-hierarchy",
        "true",
        "Turn on/off hierarchy processing."
    },
    // attribute: //
    {
        "gnetlist.hierarchy",
        "mangle-refdes-attribute",
        "true",
        "Whether to mangle sub-schematic's 'refdes' attributes."
    },
    {
        "gnetlist.hierarchy",
        "refdes-attribute-order",
        "false",
        "While mangling 'refdes' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones."
    },
    {
        "gnetlist.hierarchy",
        "refdes-attribute-separator",
        "/",
        "Separator string used to form mangled 'refdes' attribute names."
    },
    // netname: //
    {
        "gnetlist.hierarchy",
        "mangle-netname-attribute",
        "true",
        "Whether to mangle sub-schematic's 'netname' attributes."
    },
    {
        "gnetlist.hierarchy",
        "netname-attribute-order",
        "false",
        "While mangling 'netname' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones."
    },
    {
        "gnetlist.hierarchy",
        "netname-attribute-separator",
        "/",
        "Separator string used to form mangled 'netname' attribute names."
    },
    // net: //
    {
        "gnetlist.hierarchy",
        "mangle-net-attribute",
        "true",
        "Whether to mangle sub-schematic's 'net' attributes."
    },
    {
        "gnetlist.hierarchy",
        "net-attribute-order",
        "false",
        "While mangling 'net' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones."
    },
    {
        "gnetlist.hierarchy",
        "net-attribute-separator",
        "/",
        "Separator string used to form mangled 'net' attribute names."
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
cfgreg_lookup( const gchar* grp, const gchar* key );

// public:
// convenience func:
// find CfgEntry in global cfg registry by group name and key name,
//   and if found, return description for that entry
//
const gchar*
cfgreg_lookup_descr( const gchar* grp, const gchar* key );

// public:
//
gboolean
cfgreg_can_toggle( const gchar* val );

// public:
//
void
cfgreg_populate_ctx( EdaConfig* ctx );


#endif /* LEPTON_CONF_CFG_REG_H_ */

