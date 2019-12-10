/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2019 dmn <graahnul.grom@gmail.com>
 * License: GPL2 - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "cfgreg.h"




static CfgEntry g_cfg_registry[] =
{
    //
    // lepton-conf:
    //
    {
        "lepton-conf",
        "editor",
        "gvim",
        "External text editor for configuration files used by lepton-conf (this program)."
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
    // library: //
    {
        "gschem.library",
        "component-attributes",
        "*",
        "List of attribute names (semicolon-separated) that are displayed in the component select dialog.\n"
        "An empty list will disable the attribute view.\n"
        "If the first list element is an asterisk \"*\", all attributes will be displayed in the alphabetical order."
    },
    {
        "gschem.library",
        "sort",
        "false",
        "If \"true\", the component libraries are sorted alphabetically.\n"
        "Otherwise they are sorted in the order opposite to what they were added in."
    },
    // printing: //
    {
        "gschem.printing",
        "layout",
        "auto",
        "When using a paper size, set the orientation of the output.\n"
        "If \"auto\" layout is used, the orientation that best fits the drawing will be used.\n"
        "Possible values: \"portrait\", \"landscape\", or \"auto\""
    },
    {
        "gschem.printing",
        "monochrome",
        "false",
        "Toggle monochrome (\"true\") or color (\"false\") output."
    },
    {
        "gschem.printing",
        "paper",
        "iso_a4",
        "Size the output for a particular paper size.\n"
        "The default value depends on the current locale.\n"
        "Described in the PWG 5101.1 standard (Printer Working Group\n"
        "\"Media Standardized Names\" (http://www.pwg.org/standards.html#s5101).\n"
        "Examples: \"iso_a4\", \"iso_a5\", \"na_letter\"."
    },
    // gui: //
    {
        "schematic.gui",
        "use-docks",
        "false",
        "How to display widgets: as dialogs or inside the dock widgets."
    },
    {
        "schematic.gui",
        "use-tabs",
        "true",
        "Whether to use tabbed GUI: display each schematic in its own tab within GtkNotebook widget."
    },
    {
        "schematic.gui",
        "font",
        "",
        "Sets the name of the font to be used to draw text in schematics. For example:\n"
        "font=OpenGost Type B TT Regular"
    },
    {
        "schematic.gui",
        "text-sizes",
        "",
        "If set, these values will appear in the 'size' combo box of 'add text'"
        " and 'edit text' dialogs instead of the default ones "
        "(set in gschem_toplevel.c: 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26)."
        " For example:\n"
        "text-sizes=2;3;4;5;6;8"
    },
    {
        "schematic.gui",
        "max-recent-files",
        "10",
        "Maximum number of recent files to show in 'File->Open Recent' menu.\n"
        "The default is 10."
    },
    {
        "schematic.gui",
        "title-show-path",
        "false",
        "Whether to show full file path in the main window's title."
    },
    {
        "schematic.gui",
        "restore-window-geometry",
        "true",
        "Whether to restore main window's size and position on startup."
    },


    {
        "schematic.gui",
        "draw-grips",
        "true",
        "Controls if the editing grips are drawn when selecting objects."
    },
    {
        "schematic.gui",
        "toolbars",
        "true",
        "Controls if the toolbars are visible or not."
    },
    {
        "schematic.gui",
        "scrollbars",
        "true",
        "Controls if the scrollbars are visible or not."
    },
    {
        "schematic.gui",
        "handleboxes",
        "true",
        "Controls if the handleboxes for the menu and toolbars are visible or not."
    },
    {
        "schematic.gui",
        "zoom-with-pan",
        "true",
        "Sets the zoom functions to pan the display and then zoom."
    },
    {
        "schematic.gui",
        "fast-mousepan",
        "false",
        "Controls if text is drawn properly or if a simplified version (a line which"
        " represents the text string) is drawn during mouse pan.  Drawing a simple"
        " line speeds up mousepan a lot for big schematics."
    },
    {
        "schematic.gui",
        "continue-component-place",
        "true",
        "Controls the behavior of the \"Select Component...\" dialog.\n"
        "Allow to place multiple instances of a component"
        " without having to press the \"Apply\" button each time."
    },
    {
        "schematic.gui",
        "file-preview",
        "true",
        "Controls if the preview area in the File Open/Save As and"
        " Select Component dialog boxes is enabled."
    },
    {
        "schematic.gui",
        "enforce-hierarchy",
        "true",
        "Controls if the movement between hierarchy levels of the same"
        " underlying schematics is allowed or not.\n"
        "If this is enabled, then the user cannot (without using the page manager)"
        " move between hierarchy levels."
        " Otherwise, the user sees all the hierarchy levels as being flat."
    },
    {
        "schematic.gui",
        "third-button-cancel",
        "true",
        "Controls if the third mouse button cancels draw actions"
        " such as placing of a component or drawing of a primitive."
    },
    {
        "schematic.gui",
        "warp-cursor",
        "false",
        "Controls if the cursor is warped (moved) when you zoom in and out."
    },
    {
        "schematic.gui",
        "force-boundingbox",
        "false",
        "Controls if the entire bounding box of a symbol is used when figuring out"
        " which end of the pin is considered the active port.\n"
        "This option is for backward compatibility with old schematic file format."
    },
    {
        "schematic.gui",
        "net-direction-mode",
        "true",
        "Guess the best continuation direction of an L-shape net when adding a net."
    },
    {
        "schematic.gui",
        "embed-components",
        "false",
        "Determines if the newly placed components are embedded in the schematic."
    },


    // tabs: //
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
    // status-bar: //
    {
        "schematic.status-bar",
        "show-mouse-buttons",
        "false",
        "Whether to show mouse buttons assignment indicators."
    },
    {
        "schematic.status-bar",
        "show-rubber-band",
        "true",
        "Whether to show net rubber band mode indicator."
    },
    {
        "schematic.status-bar",
        "show-magnetic-net",
        "true",
        "Whether to show magnetic net mode indicator."
    },
    {
        "schematic.status-bar",
        "status-bold-font",
        "false",
        "Whether to display the status line with bolder font weight."
    },
    {
        "schematic.status-bar",
        "status-active-color",
        "green",
        "Color to use for the status line text when some mode is activated.\n"
        "The string can be either one of a set of standard names "
        "(taken from the X11 rgb.txt file), or a hex value in the form '#rrggbb'."
    },
    // undo: //
    {
        "schematic.undo",
        "modify-viewport",
        "false",
        "Allow undo/redo operations to change pan and zoom."
    },
    // log-window: //
    {
        "schematic.log-window",
        "font",
        "Monospace 11",
        "Custom font for the log window (e.g. \"Monospace 10\")."
    },
    // macro-widget: //
    {
        "schematic.macro-widget",
        "history-length",
        "10",
        "Maximum number of items to keep in the macro-widget "
        "command history (10 by default)."
    },
    {
        "schematic.macro-widget",
        "font",
        "Monospace 11",
        "Font to be used to draw text in the macro-widget command entry.\n"
        "For example:\n"
        "font=Monospace 12"
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

    //
    // lepton-cli:
    //

    // [export] group:
    {
        "export",
        "align",
        "auto",
        "List of two doubles in the form HALIGN;VALIGN or string \"auto\".\n"
        "Set how the drawing is aligned within the page. HALIGN controls the"
        " horizontal alignment, and VALIGN the vertical. Each alignment value"
        " should be in the range 0.0 to 1.0. The \"auto\" alignment is equivalent"
        " to a value of 0.5;0.5, i.e. centered."
    },
    {
        "export",
        "dpi",
        "96",
        "Set the number of pixels per inch used when generating PNG output."
    },
    {
        "export",
        "font",
        "Sans",
        "Set the font to be used for drawing text."
    },
    {
        "export",
        "layout",
        "auto",
        "Predefined string: \"portrait\", \"landscape\", or \"auto\".\n"
        "When using a paper size, set the orientation of the output. If \"auto\""
        " layout is used, the orientation that best fits the drawing will be used."
    },
    {
        "export",
        "margins",
        "18;18;18;18",
        "List of four integers in the form TOP;LEFT;BOTTOM;RIGHT.\n"
        "Set the widths of the margins to be used (the minimal distances"
        " from the sheet edges; actual margins may be larger if the sizes"
        " of the chosen paper do not meet the sizes of the printed schematic)."
    },
    {
        "export",
        "monochrome",
        "false",
        "Toggle monochrome (true) or color (false) output."
    },
    {
        "export",
        "paper",
        "iso_a4",
        "Size the output for a particular paper size.\n"
        "The default value depends on the current locale.\n"
        "Described in the PWG 5101.1 standard (Printer Working Group\n"
        "\"Media Standardized Names\" (http://www.pwg.org/standards.html#s5101).\n"
        "Examples: \"iso_a4\", \"iso_a5\", \"na_letter\"."
    },
    {
        "export",
        "size",
        "",
        "Size the output with specific dimensions. If the size is \"auto\","
        " select the size that best fits the drawing. This overrides the"
        " [export]::paper key."
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

    if ( entry != NULL )
        return entry->desc_;

    return NULL;
}



const gchar*
cfgreg_lookup_dflt_val( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = cfgreg_lookup( grp, key );

    if ( entry != NULL )
        return entry->def_val_;

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

    can_toggle |= g_strcmp0( val, "net-attribute" )     == 0;
    can_toggle |= g_strcmp0( val, "netname-attribute" ) == 0;

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

