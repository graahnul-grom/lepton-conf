/*
 * lepton-conf - Lepton EDA configuration utility.
 * https://github.com/graahnul-grom/lepton-conf
 * Copyright (C) 2017-2020 dmn <graahnul.grom@gmail.com>
 * License: GPLv2+ - same as Lepton EDA, see
 * https://github.com/lepton-eda/lepton-eda
 */

#include "proto.h"




static CfgEntryEnum*
find_cee( const gchar* grp, const gchar* key );



static CfgEntry g_cfg_registry[] =
{
    //
    // lepton-conf:
    //
    {
        "lepton-conf",
        "editor",
        "gvim",
        "External text editor for configuration files used by lepton-conf (this program).",
        { NULL, NULL, NULL, NULL }
    },
    {
        "lepton-conf",
        "close-with-escape-key",
        "false",
        "Exit lepton-conf with the Escape key.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "lepton-conf",
        "restore-last-dir",
        "false",
        "Restore last working directory on startup.",
        { NULL, NULL, NULL, NULL }
    },

    //
    // lepton-schematic:
    //
    {
        "schematic",
        "default-filename",
        "untitled",
        "Default file name for any new schematic files.\n"
        "It is used to create filenames of the form “untitled_N.sch” where N is a number.",
        {
            "gschem",
            "default-filename",
            "untitled",
            "Default file name for any new schematic files.\n"
            "It is used to create filenames of the form “untitled_N.sch” where N is a number."
        }
    },
    // library: //
    {
        "schematic.library",
        "component-attributes",
        "*",
        "List of attribute names (semicolon-separated) that are displayed in the component select dialog.\n"
        "An empty list will disable the attribute view.\n"
        "If the first list element is an asterisk \"*\", all attributes will be displayed in the alphabetical order.",
        {
            "gschem.library",
            "component-attributes",
            "*",
            "List of attribute names (semicolon-separated) that are displayed in the component select dialog.\n"
            "An empty list will disable the attribute view.\n"
            "If the first list element is an asterisk \"*\", all attributes will be displayed in the alphabetical order."
        }
    },
    {
        "schematic.library",
        "sort",
        "false",
        "If \"true\", the component libraries are sorted alphabetically.\n"
        "Otherwise they are sorted in the order opposite to what they were added in.",
        {
            "gschem.library",
            "sort",
            "false",
            "If \"true\", the component libraries are sorted alphabetically.\n"
            "Otherwise they are sorted in the order opposite to what they were added in."
        }
    },
    // printing: //
    {
        "schematic.printing",
        "layout",
        "auto",
        "When using a paper size, set the orientation of the output.\n"
        "If \"auto\" layout is used, the orientation that best fits the drawing will be used.\n"
        "Possible values: \"portrait\", \"landscape\", or \"auto\"",
        {
            "gschem.printing",
            "layout",
            "auto",
            "When using a paper size, set the orientation of the output.\n"
            "If \"auto\" layout is used, the orientation that best fits the drawing will be used.\n"
            "Possible values: \"portrait\", \"landscape\", or \"auto\""
        }
    },
    {
        "schematic.printing",
        "monochrome",
        "false",
        "Toggle monochrome (\"true\") or color (\"false\") output.",
        {
            "gschem.printing",
            "monochrome",
            "false",
            "Toggle monochrome (\"true\") or color (\"false\") output."
        }
    },
    {
        "schematic.printing",
        "paper",
        "iso_a4",
        "Size the output for a particular paper size.\n"
        "The default value depends on the current locale.\n"
        "Described in the PWG 5101.1 standard (Printer Working Group\n"
        "\"Media Standardized Names\" (http://www.pwg.org/standards.html#s5101).\n"
        "Examples: \"iso_a4\", \"iso_a5\", \"na_letter\".",
        {
            "gschem.printing",
            "paper",
            "iso_a4",
            "Size the output for a particular paper size.\n"
            "The default value depends on the current locale.\n"
            "Described in the PWG 5101.1 standard (Printer Working Group\n"
            "\"Media Standardized Names\" (http://www.pwg.org/standards.html#s5101).\n"
            "Examples: \"iso_a4\", \"iso_a5\", \"na_letter\"."
        }
    },
    // gui: //
    {
        "schematic.gui",
        "use-docks",
        "false",
        "How to display widgets: as dialogs or inside the dock widgets.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "use-tabs",
        "true",
        "Whether to use tabbed GUI: display each schematic in its own tab within GtkNotebook widget.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "font",
        "",
        "Sets the name of the font to be used to draw text in schematics. For example:\n"
        "font=OpenGost Type B TT Regular",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "text-sizes",
        "",
        "If set, these values will appear in the 'size' combo box of 'add text'"
        " and 'edit text' dialogs instead of the default ones "
        "(set in gschem_toplevel.c: 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26)."
        " For example:\n"
        "text-sizes=2;3;4;5;6;8",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "max-recent-files",
        "10",
        "Maximum number of recent files to show in 'File->Open Recent' menu.\n"
        "The default is 10.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "title-show-path",
        "false",
        "Whether to show full file path in the main window's title.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "restore-window-geometry",
        "true",
        "Whether to restore main window's size and position on startup.",
        { NULL, NULL, NULL, NULL }
    },

    {
        "schematic.gui",
        "draw-grips",
        "true",
        "Controls if the editing grips are drawn when selecting objects.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "toolbars",
        "true",
        "Controls if the toolbars are visible or not.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "scrollbars",
        "true",
        "Controls if the scrollbars are visible or not.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "handleboxes",
        "true",
        "Controls if the handleboxes for the menu and toolbars are visible or not.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "zoom-with-pan",
        "true",
        "Sets the zoom functions to pan the display and then zoom.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "fast-mousepan",
        "false",
        "Controls if text is drawn properly or if a simplified version (a text"
        " string bounding box) is drawn during mouse pan. Drawing a simple"
        " box speeds up mousepan a lot for big schematics.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "continue-component-place",
        "true",
        "Controls the behavior of the \"Select Component...\" dialog.\n"
        "Allows to place multiple instances of a component"
        " without having to press the \"Apply\" button each time.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "file-preview",
        "true",
        "Controls if the preview area in the File Open/Save As"
        " dialog boxes is enabled.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "enforce-hierarchy",
        "true",
        "Controls if the movement between hierarchy levels of the same"
        " underlying schematics is allowed or not.\n"
        "If this is enabled, then the user cannot (without using the page manager)"
        " move between hierarchy levels."
        " Otherwise, the user sees all the hierarchy levels as being flat.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "third-button-cancel",
        "true",
        "Controls if the third mouse button cancels draw actions"
        " such as placing of a component or drawing of a primitive.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "warp-cursor",
        "false",
        "Controls if the cursor is warped (moved) when you zoom in and out.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "force-boundingbox",
        "false",
        "Controls if the entire bounding box of a symbol is used when figuring out"
        " which end of the pin is considered the active port.\n"
        "This option is for backward compatibility with old schematic file format.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "net-direction-mode",
        "true",
        "Guess the best continuation direction of an L-shape net when adding a net.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "embed-components",
        "false",
        "Determines if the newly placed components are embedded in the schematic.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "netconn-rubberband",
        "true",
        "Maintain net connections when you move a connected component or net.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "magnetic-net-mode",
        "true",
        "Whether to mark a possible connection that is close to the current"
        " cursor position when drawing a net.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "zoom-gain",
        "20",
        "The percentage size increase/decrease when zooming in/out with the"
        " mouse wheel. Negative values reverses the direction.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "mousepan-gain",
        "1",
        "Controls how much the display pans when using mouse. A larger value"
        " provides greater pan distance when moving the mouse.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "keyboardpan-gain",
        "20",
        "Controls how much the display pans when using the keyboard keys."
        " A larger value provides greater pan distance.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "select-slack-pixels",
        "10",
        "Controls how many pixels around an object can still be clicked"
        " as part of that object. A larger value gives greater ease in"
        " selecting small, or narrow objects.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "text-size",
        "10",
        "Sets the default text size.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "snap-size",
        "100",
        "Sets the default grid spacing.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "scrollpan-steps",
        "8",
        "Controls the number of scroll pan events required to traverse"
        " the viewed schematic area. Larger numbers mean more scroll steps"
        " are required to pan across the viewed area and giving finer"
        " control over positioning.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "dots-grid-dot-size",
        "1",
        "Controls the size of the grid dots (in pixels) in the dots grid display.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "dots-grid-fixed-threshold",
        "10",
        "Specifies the minimum number of pixels for the grid to be displayed."
        " Controls the density of the displayed grid (smaller numbers will"
        " cause the grid to be drawn denser). This option is only effective"
        " when the dots-grid-mode is set to \"fixed\".",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "mesh-grid-display-threshold",
        "3",
        "Specifies the minimum line pitch for the grid to be displayed."
        " Controls the maximum density of the displayed grid before the"
        " minor, then the major grid lines are switched off.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "action-feedback-mode",
        "outline",
        "Sets the default action feedback mode for copy, move, and component place"
        " operations. Either \"outline\" or \"boundingbox\".\n"
        "When set to \"boundingbox\", just a box surrounding objects"
        " is drawn, which is much faster. It may help with slow hardware.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "text-caps-style",
        "both",
        "Sets the default caps style used for the input of text.\n"
        "Either \"both\", \"lower\" or \"upper\".",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "middle-button",
        "mousepan",
        "Controls what the middle mouse button does:\n"
        "- mousepan: mouse panning\n"
        "- popup: display the popup menu\n"
        "- action: perform an action on a single object\n"
        "- stroke: draw strokes\n"
        "- repeat: repeat the last command",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "third-button",
        "popup",
        "Controls what the third mouse button does:\n"
        "- popup: display the popup menu\n"
        "- mousepan: mouse panning",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "scroll-wheel",
        "classic",
        "Controls the binding of the mouse scroll wheel:\n"
        "- classic: zoom in/out,"
        " with Ctrl - horizontal scroll, with Shift - vertical scroll\n"
        "- gtk: vertical scroll,"
        " with Shift - horizontal scroll, with Ctrl - zoom in/out",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "grid-mode",
        "mesh",
        "Either \"mesh\", \"dots\" or \"none\".",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "dots-grid-mode",
        "variable",
        "Controls the mode of the dotted grid, either \"variable\" or \"fixed\".\n"
        "In the variable mode, the grid spacing changes"
        " depending on the zoom factor. In the fixed mode, the grid always"
        " represents the same number of units as the snap-spacing. You can"
        " control the density of the grid using the dots-grid-fixed-threshold option.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "net-selection-mode",
        "enabled_net",
        "Controls how many net segments are selected when you click at a net.\n"
        "Either \"enabled_all\", \"enabled_net\" or \"disabled\".\n"
        "- enabled_all:\n"
        "  - first click selects the net itself\n"
        "  - second click selects all nets directly connected to the selected one\n"
        "  - third click in addition selects all nets with equal \"netname\" attributes\n"
        "- enabled_net:\n"
        "  - first click selects the net itself\n"
        "  - second click selects all nets directly connected to the selected one\n"
        "- disabled:\n"
        "  - mouse clicks just selects the clicked net\n",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "default-titleblock",
        "title-B.sym",
        "Symbol (usually, a title block) to be automatically added"
        " when a new page is created. If you do not want to use a "
        "title block, set this option to an empty string.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "use-toplevel-windows",
        "false",
        "When docking windows GUI is off, allow the following widgets to act\n"
        "as top-level windows (do not stay on top of the main window):\n"
        "- Object properties\n"
        "- Text properties\n"
        "- Options\n"
        "- Log\n"
        "- Find text results\n"
        "- Page manager\n"
        "- Font selector\n"
        "- Color scheme editor",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.gui",
        "small-placeholders",
        "true",
        "Draw either new (smaller) placeholders for missing schematic symbols"
        " (`true`), or classic ones (giant red triangles with an exclamation"
        " mark and two lines of text) if this option is set to `false`.",
        { NULL, NULL, NULL, NULL }
    },

    // tabs: //
    {
        "schematic.tabs",
        "show-close-button",
        "true",
        "Whether to show \"close\" button on each tab.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.tabs",
        "show-up-button",
        "true",
        "Whether to show \"hierarchy up\" button on each tab.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.tabs",
        "show-tooltips",
        "true",
        "Whether to show tabs tooltips in tabbed GUI.",
        { NULL, NULL, NULL, NULL }
    },
    // status-bar: //
    {
        "schematic.status-bar",
        "show-mouse-buttons",
        "false",
        "Whether to show mouse buttons assignment indicators.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.status-bar",
        "show-rubber-band",
        "true",
        "Whether to show net rubber band mode indicator.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.status-bar",
        "show-magnetic-net",
        "true",
        "Whether to show magnetic net mode indicator.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.status-bar",
        "status-bold-font",
        "false",
        "Whether to display the status line with bolder font weight.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.status-bar",
        "status-active-color",
        "green",
        "Color to use for the status line text when some mode is activated.\n"
        "The string can be either one of a set of standard names "
        "(taken from the X11 rgb.txt file), or a hex value in the form '#rrggbb'.",
        { NULL, NULL, NULL, NULL }
    },
    // undo: //
    {
        "schematic.undo",
        "modify-viewport",
        "false",
        "Allow undo/redo operations to change pan and zoom.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.undo",
        "undo-control",
        "true",
        "Controls if the undo is enabled or not.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.undo",
        "undo-type",
        "disk",
        "Controls what kind of medium is used for storing undo data.\n"
        "Either \"disk\" or \"memory\".\n"
        "The disk mechanism is nice because you get undo-level number of"
        " backups of the schematic written to disk as backups so you"
        " should never lose a schematic due to a crash.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.undo",
        "undo-levels",
        "20",
        "Determines the number of levels of undo.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.undo",
        "undo-panzoom",
        "false",
        "Controls if pan or zoom commands are saved in the undo list.\n"
        "If this is enabled, then a pan or zoom will be considered"
        " a command and can be undone.",
        { NULL, NULL, NULL, NULL }
    },

    // log-window: //
    {
        "schematic.log-window",
        "font",
        "Monospace 11",
        "Custom font for the log window (e.g. \"Monospace 10\").",
        { NULL, NULL, NULL, NULL }
    },
    // macro-widget: //
    {
        "schematic.macro-widget",
        "history-length",
        "10",
        "Maximum number of items to keep in the macro-widget "
        "command history (10 by default).",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.macro-widget",
        "font",
        "Monospace 11",
        "Font to be used to draw text in the macro-widget command entry.\n"
        "For example:\n"
        "font=Monospace 12",
        { NULL, NULL, NULL, NULL }
    },
    // schematic: //
    {
        "schematic",
        "bus-ripper-size",
        "200",
        "Sets the size of the auto bus rippers.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "bus-ripper-type",
        "component",
        "Sets the bus ripper type, either a \"component\" or plain \"net\".",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "bus-ripper-rotation",
        "non-symmetric",
        "Either \"symmetric\" or \"non-symmetric\". This deals with how the"
        " bus ripper symbol is rotated when it is auto added to a schematic.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "bus-ripper-symname",
        "busripper-1.sym",
        "The default bus ripper symbol, used when the schematic::bus-ripper-type"
        " configuration key is set to \"component\". The symbol must exist"
        " in a component library.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "net-consolidate",
        "true",
        "Controls if the net consolidation code is used when schematics are read"
        " in, written to disk, and when nets are being drawn (does not consolidate"
        " when things are being copied or moved yet). Net consolidation is the"
        " connection of nets which can be combined into one.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "logging",
        "true",
        "Determines if the logging mechanism is enabled.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "auto-save-interval",
        "120",
        "Controls how often a backup copy is made, in seconds.\n"
        "The backup copy is made when you make some change to the schematic,"
        " and there were more than \"interval\" seconds from the last autosave.\n"
        "Autosaving will not be allowed if setting it to zero.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic",
        "log-window",
        "later",
        "Controls if the log window is shown when lepton-schematic is started up"
        " (\"startup\"), or not (\"later\").",
        { NULL, NULL, NULL, NULL }
    },
    // schematic.attrib: //
    {
        "schematic.attrib",
        "always-promote",
        "footprint;device;value;model-name",
        "The list of attributes that are always promoted"
        " regardless of their visibility.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.attrib",
        "promote",
        "true",
        "Set if attribute promotion occurs when you instantiate a component.\n"
        "Attribute promotion means that any floating (unattached)"
        " attribute which is inside a symbol gets attached to the newly"
        " inserted component when it is instantiated.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.attrib",
        "promote-invisible",
        "false",
        "Set if invisible floating attributes are promoted when a component"
        " is instantiated.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.attrib",
        "keep-invisible",
        "true",
        "If both attribute-promotion and promote-invisible are enabled, then this"
        " controls if invisible floating attributes are kept around in memory and"
        " NOT deleted.  Having this enabled will keeps component slotting working."
        " If attribute-promotion and promote-invisible are enabled and this"
        " keyword is disabled, then component slotting will NOT work (and maybe"
        " other functions which depend on hidden attributes, since those attributes"
        " are deleted from memory).",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.attrib",
        "pin-attribs",
        "pinnumber;pinseq;pintype;pinlabel",
        "Attributes presented in the \"Add Attribute\" and \"Edit Attribute\""
        " dialogs for pins.\n"
        "The attribute names are case sensitive. The order of the"
        " names in the list determines the order they are displayed.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "schematic.attrib",
        "symbol-attribs",
        "netname;footprint;value;refdes;source;model-name;model;net;device;numslots;slot;slotdef;graphical;description;documentation;symversion;comment;author;dist-license;use-license;file",
        "Attributes presented in the \"Add Attribute\" and \"Edit Attribute\""
        " dialogs for symbols and nets.\n"
        "The attribute names are case sensitive. The order of the"
        " names in the list determines the order they are displayed.",
        { NULL, NULL, NULL, NULL }
    },
    // schematic.backup: //
    {
        "schematic.backup",
        "create-files",
        "true",
        "Enable the creation of backup files (name.sch~) when saving a schematic.",
        { NULL, NULL, NULL, NULL }
    },

    //
    // lepton-netlist:
    //
    {
        "netlist",
        "default-net-name",
        "unnamed_net",
        "Default name used for nets for which the user has set\n"
        "no explicit name via the netname= or net= attributes.",
        {
            "gnetlist",
            "default-net-name",
            "unnamed_net",
            "Default name used for nets for which the user has set\n"
            "no explicit name via the netname= or net= attributes."
        }
    },
    {
        "netlist",
        "default-bus-name",
        "unnamed_bus",
        "Default name used for buses for which the user has set\n"
        "no explicit name via the netname= or net= attributes.",
        {
            "gnetlist",
            "default-bus-name",
            "unnamed_bus",
            "Default name used for buses for which the user has set\n"
            "no explicit name via the netname= or net= attributes."
        }
    },
    {
        "netlist",
        "net-naming-priority",
        "net-attribute",
        "Specify which attribute, net (\"net-attribute\") or netname (\"netname-attribute\"),"
        " has priority if a net is found with two names. Any netname"
        " conflict will be resolved using the chosen attribute.",
        {
            "gnetlist",
            "net-naming-priority",
            "net-attribute",
            "Specify which attribute, net (\"net-attribute\") or netname (\"netname-attribute\"),"
            " has priority if a net is found with two names. Any netname"
            " conflict will be resolved using the chosen attribute."
        }
    },
    {
        "netlist.hierarchy",
        "traverse-hierarchy",
        "true",
        "Turn on/off hierarchy processing.",
        {
            "gnetlist.hierarchy",
            "traverse-hierarchy",
            "true",
            "Turn on/off hierarchy processing."
        }
    },
    // attribute: //
    {
        "netlist.hierarchy",
        "mangle-refdes-attribute",
        "true",
        "Whether to mangle sub-schematic's 'refdes' attributes.",
        {
            "gnetlist.hierarchy",
            "mangle-refdes-attribute",
            "true",
            "Whether to mangle sub-schematic's 'refdes' attributes."
        }
    },
    {
        "netlist.hierarchy",
        "refdes-attribute-order",
        "false",
        "While mangling 'refdes' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones.",
        {
            "gnetlist.hierarchy",
            "refdes-attribute-order",
            "false",
            "While mangling 'refdes' attributes, whether to append (false)\n"
            "or prepend (true) sub-schematic's ones."
        }
    },
    {
        "netlist.hierarchy",
        "refdes-attribute-separator",
        "/",
        "Separator string used to form mangled 'refdes' attribute names.",
        {
            "gnetlist.hierarchy",
            "refdes-attribute-separator",
            "/",
            "Separator string used to form mangled 'refdes' attribute names."
        }
    },
    // netname: //
    {
        "netlist.hierarchy",
        "mangle-netname-attribute",
        "true",
        "Whether to mangle sub-schematic's 'netname' attributes.",
        {
            "gnetlist.hierarchy",
            "mangle-netname-attribute",
            "true",
            "Whether to mangle sub-schematic's 'netname' attributes."
        }
    },
    {
        "netlist.hierarchy",
        "netname-attribute-order",
        "false",
        "While mangling 'netname' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones.",
        {
            "gnetlist.hierarchy",
            "netname-attribute-order",
            "false",
            "While mangling 'netname' attributes, whether to append (false)\n"
            "or prepend (true) sub-schematic's ones."
        }
    },
    {
        "netlist.hierarchy",
        "netname-attribute-separator",
        "/",
        "Separator string used to form mangled 'netname' attribute names.",
        {
            "gnetlist.hierarchy",
            "netname-attribute-separator",
            "/",
            "Separator string used to form mangled 'netname' attribute names."
        }
    },
    // net: //
    {
        "netlist.hierarchy",
        "mangle-net-attribute",
        "true",
        "Whether to mangle sub-schematic's 'net' attributes.",
        {
            "gnetlist.hierarchy",
            "mangle-net-attribute",
            "true",
            "Whether to mangle sub-schematic's 'net' attributes."
        }
    },
    {
        "netlist.hierarchy",
        "net-attribute-order",
        "false",
        "While mangling 'net' attributes, whether to append (false)\n"
        "or prepend (true) sub-schematic's ones.",
        {
            "gnetlist.hierarchy",
            "net-attribute-order",
            "false",
            "While mangling 'net' attributes, whether to append (false)\n"
            "or prepend (true) sub-schematic's ones."
        }
    },
    {
        "netlist.hierarchy",
        "net-attribute-separator",
        "/",
        "Separator string used to form mangled 'net' attribute names.",
        {
            "gnetlist.hierarchy",
            "net-attribute-separator",
            "/",
            "Separator string used to form mangled 'net' attribute names."
        }
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
        " to a value of 0.5;0.5, i.e. centered.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "dpi",
        "96",
        "Set the number of pixels per inch used when generating PNG output.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "font",
        "Sans",
        "Set the font to be used for drawing text.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "layout",
        "auto",
        "Predefined string: \"portrait\", \"landscape\", or \"auto\".\n"
        "When using a paper size, set the orientation of the output. If \"auto\""
        " layout is used, the orientation that best fits the drawing will be used.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "margins",
        "18;18;18;18",
        "List of four integers in the form TOP;LEFT;BOTTOM;RIGHT.\n"
        "Set the widths of the margins to be used (the minimal distances"
        " from the sheet edges; actual margins may be larger if the sizes"
        " of the chosen paper do not meet the sizes of the printed schematic).",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "monochrome",
        "false",
        "Toggle monochrome (true) or color (false) output.",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "paper",
        "iso_a4",
        "Size the output for a particular paper size.\n"
        "The default value depends on the current locale.\n"
        "Described in the PWG 5101.1 standard (Printer Working Group\n"
        "\"Media Standardized Names\" (http://www.pwg.org/standards.html#s5101).\n"
        "Examples: \"iso_a4\", \"iso_a5\", \"na_letter\".",
        { NULL, NULL, NULL, NULL }
    },
    {
        "export",
        "size",
        "",
        "Size the output with specific dimensions. If the size is \"auto\","
        " select the size that best fits the drawing. This overrides the"
        " [export]::paper key.",
        { NULL, NULL, NULL, NULL }
    },

    {
        NULL,
        NULL,
        NULL,
        NULL,
        { NULL, NULL, NULL, NULL }
    }
};




// CfgEntry fields accessor functions:
//
static const gchar*
cfg_entry_get_grp( const CfgEntry* entry )
{
    if ( g_cfg_legacy_mode && entry->legacy_.grp_ )
        return entry->legacy_.grp_;
    return entry->grp_;
}

static const gchar*
cfg_entry_get_key( const CfgEntry* entry )
{
    if ( g_cfg_legacy_mode && entry->legacy_.key_ )
        return entry->legacy_.key_;
    return entry->key_;
}

static const gchar*
cfg_entry_get_dflt_val( const CfgEntry* entry )
{
    if ( g_cfg_legacy_mode && entry->legacy_.def_val_ )
        return entry->legacy_.def_val_;
    return entry->def_val_;
}

static const gchar*
cfg_entry_get_descr( const CfgEntry* entry )
{
    if ( g_cfg_legacy_mode && entry->legacy_.desc_ )
        return entry->legacy_.desc_;
    return entry->desc_;
}




// private:
// find CfgEntry in global cfg registry by group name and key name
//
const CfgEntry*
cfgreg_lookup( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = g_cfg_registry;
    const CfgEntry* ret   = NULL;

    for ( ; cfg_entry_get_grp( entry ) != NULL; ++entry )
    {
        gboolean cond1 = g_strcmp0( cfg_entry_get_grp( entry ), grp ) == 0;
        gboolean cond2 = g_strcmp0( cfg_entry_get_key( entry ), key ) == 0;

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
        return cfg_entry_get_descr( entry );

    return NULL;
}



const gchar*
cfgreg_lookup_dflt_val( const gchar* grp, const gchar* key )
{
    const CfgEntry* entry = cfgreg_lookup( grp, key );

    if ( entry != NULL )
        return cfg_entry_get_dflt_val( entry );

    return NULL;
}



// public:
//
gboolean
cfgreg_can_toggle( const row_data* rdata )
{
    if ( !rdata )
        return FALSE;

    if ( rdata->rtype_ != RT_KEY )
        return FALSE;

    const gchar* val = rdata->val_;

    const gboolean can_toggle =
        strcmp( val, "true" )  == 0 ||
        strcmp( val, "false" ) == 0 ||
        find_cee( rdata->group_, rdata->key_ );

    return can_toggle;
}



// public:
//
void
cfgreg_populate_ctx( EdaConfig* ctx )
{
    const CfgEntry* entry = g_cfg_registry;

    for ( ; cfg_entry_get_grp( entry ) != NULL; ++entry )
    {
        const gchar* grp     = cfg_entry_get_grp( entry );
        const gchar* key     = cfg_entry_get_key( entry );
        const gchar* def_val = cfg_entry_get_dflt_val( entry );

        eda_config_set_string( ctx, grp, key, def_val );
    }
}




static GList* g_cees = NULL;



static CfgEntryEnum*
mk_cee( const gchar* grp, const gchar* key, GList* vals )
{
    CfgEntryEnum* e = g_malloc( sizeof( CfgEntryEnum ) );
    e->grp_  = g_strdup( grp );
    e->key_  = g_strdup( key );
    e->vals_ = vals;
    return e;
}



static void
rm_cee( CfgEntryEnum* e )
{
    g_free( e->grp_ );
    g_free( e->key_ );
    if ( e->vals_ != NULL )
        g_list_free( e->vals_ );
    g_free( e );
}



static int
cee_cmp_fun( gconstpointer a, gconstpointer b )
{
    CfgEntryEnum* e1 = (CfgEntryEnum*) a;
    CfgEntryEnum* e2 = (CfgEntryEnum*) b;
    gboolean gmatch = strcmp( e1->grp_, e2->grp_) == 0;
    gboolean kmatch = strcmp( e1->key_, e2->key_) == 0;

    return gmatch && kmatch ? 0 : 1;
}



static void
add_cee( const gchar* grp, const gchar* key, const gchar** vals )
{
    GList* gl = NULL;
    for ( const gchar** p = vals; *p != NULL; ++p )
        gl = g_list_append( gl, (gpointer) *p );

    CfgEntryEnum* e = mk_cee( grp, key, gl );
    g_cees = g_list_append( g_cees, (gpointer) e );
}



CfgEntryEnum*
find_cee( const gchar* grp, const gchar* key )
{
    CfgEntryEnum* data = mk_cee( grp, key, NULL );
    GList* found = g_list_find_custom( g_cees, (gconstpointer) data, &cee_cmp_fun );
    rm_cee( data );

    return found ? (CfgEntryEnum*) found->data : NULL;
}



const gchar*
cee_next_val( const gchar* grp, const gchar* key, const gchar* current )
{
    CfgEntryEnum* e = find_cee( grp, key );
    if ( !e )
        return NULL;

    for ( GList* gl = e->vals_; gl != NULL; gl = g_list_next (gl) )
    {
        const gchar* val = (const gchar*) gl->data;
        if ( strcmp( val, current ) != 0 )
            continue;

        GList* next  = g_list_next( gl );
        GList* first = g_list_first( gl );

        if ( next != NULL )
            return (const gchar*) next->data;

        if ( first != NULL )
            return (const gchar*) first->data;
    }

    return NULL;

} // cee_next_val()



void
cfgreg_init()
{
    const gchar* e_l[] = { "auto", "landscape", "portrait", NULL };
    add_cee( "export", "layout", e_l );

    const gchar* gp_l[] = { "auto", "landscape", "portrait", NULL };
    add_cee( "gschem.printing",    "layout", gp_l );
    add_cee( "schematic.printing", "layout", gp_l );

    const gchar* s_brr[] = { "non-symmetric", "symmetric", NULL };
    add_cee( "schematic", "bus-ripper-rotation", s_brr );

    const gchar* s_brt[] = { "component", "net", NULL };
    add_cee( "schematic", "bus-ripper-type", s_brt );

    const gchar* s_lw[] = { "later", "startup", NULL };
    add_cee( "schematic", "log-window", s_lw );

    const gchar* sg_afm[] = { "outline", "boundingbox", NULL };
    add_cee( "schematic.gui", "action-feedback-mode", sg_afm );

    const gchar* sg_dgm[] = { "variable", "fixed", NULL };
    add_cee( "schematic.gui", "dots-grid-mode", sg_dgm );

    const gchar* sg_gm[] = { "mesh", "dots", "none", NULL };
    add_cee( "schematic.gui", "grid-mode", sg_gm );

    const gchar* sg_mb[] = { "mousepan", "popup", "action", "stroke", "repeat", NULL };
    add_cee( "schematic.gui", "middle-button", sg_mb );

    const gchar* sg_nsm[] = { "enabled_net", "enabled_all", "disabled", NULL };
    add_cee( "schematic.gui", "net-selection-mode", sg_nsm );

    const gchar* sg_sw[] = { "classic", "gtk", NULL };
    add_cee( "schematic.gui", "scroll-wheel", sg_sw );

    const gchar* sg_tcs[] = { "both", "lower", "upper", NULL };
    add_cee( "schematic.gui", "text-caps-style", sg_tcs );

    const gchar* sg_tb[] = { "mousepan", "popup", NULL };
    add_cee( "schematic.gui", "third-button", sg_tb );

    const gchar* nl_nnp[] = { "net-attribute", "netname-attribute", NULL };
    add_cee( "gnetlist", "net-naming-priority", nl_nnp );
    add_cee( "netlist",  "net-naming-priority", nl_nnp );

#ifdef DEBUG
//    const gchar* v = NULL;
//    v = cee_next_val( "schematic.gui", "text-caps-style", "both" );
//    printf( " .. .. [%s]\n", v );
//    v = cee_next_val( "schematic.gui", "text-caps-style", "lower" );
//    printf( " .. .. [%s]\n", v );
//    v = cee_next_val( "schematic.gui", "text-caps-style", "upper" );
//    printf( " .. .. [%s]\n", v );
//    v = cee_next_val( "schematic.gui", "text-caps-style", "XXX" );
//    printf( " .. .. [%s]\n", v );
//    v = cee_next_val( "XXX", "XXX", "XXX" );
//    printf( " .. .. [%s]\n", v );
//    dbg_print_cees();
#endif

} // cfgreg_init()



//#ifdef DEBUG
//static void
//dbg_print_cees()
//{
//    for ( GList* g = g_cees; g != NULL; g = g_list_next( g ) )
//    {
//        CfgEntryEnum* v = (CfgEntryEnum*) g->data;
//        printf( " >> >> d: [%s]::%s\n", v->grp_, v->key_ );
//
//        for ( GList* gg = v->vals_; gg != NULL; gg = g_list_next (gg) )
//        {
//            printf( "  <%s>\n", (const gchar*) gg->data );
//        }
//    }
//}
//#endif

