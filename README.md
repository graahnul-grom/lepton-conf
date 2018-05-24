lepton-conf - configuration utility for [Lepton EDA](https://github.com/lepton-eda/lepton-eda)
====================================================

lepton-conf is designed to ease the process of editing Lepton EDA configuration that
is stored in ini-like files (`geda.conf`) - that is, new-style configuration, as opposed
to settings stored in `guile` source code files. It is a graphical user interface application
(using GTK 2.x, see [screenshots](https://graahnul-grom.github.io/lepton-conf)) that does
essentially the same as `lepton-cli` (formerly named "[`gaf`](http://wiki.geda-project.org/geda:gaf_utility)")
command-line utility invoked with `config` command.
<br />
The project is a work in progress, so please report any spotted bugs and provide
your suggestions on the [issues](https://github.com/graahnul-grom/lepton-conf/issues) page.
<br />

Documentation
------------
Information about the configuration system can be found on this [wiki page](https://github.com/lepton-eda/lepton-eda/wiki/Configuration-Settings).

Dependencies
------------

- [Lepton Electronic Design Automation](https://github.com/lepton-eda/lepton-eda) suite

Installation from source
------------------------

* Checkout lepton-conf:

  `git clone https://github.com/graahnul-grom/lepton-conf.git`

* Change to source folder:

  `cd lepton-conf`

* Edit `Makefile`:

  - Set `LEPTON_INST_ROOT` variable to point to Lepton EDA installation path, e.g.:

    `LEPTON_INST_ROOT=/usr/local`

  - Set `PK_GTK` variable to match GTK 2.0 `pkg-config` package name on your system, e.g.:

    `PK_GTK=gtk+-2.0`

  - (Optionally) comment out line that starts with `DBG=` to disable debugging.

* Build:

  `make`

* Install:

  Place `lepton-conf` executable file anywhere in your `$PATH`

* Run:

  `> lepton-conf`

  If Lepton EDA is installed in non-standard location, you may have to adjust
  `$LD_LIBRARY_PATH` environment variable, so that `liblepton` can be found, e.g.:

  `> LD_LIBRARY_PATH=/path/to/lepton-eda/lib lepton-conf`

  Command line arguments:

  `> lepton-conf -h`
  ```
  Usage: lepton-conf [-h] [-v] [-e] [-d] [dirname]
  Options:
    -h    Help (this message).
    -v    Show version.
    -e    Close window with 'Escape' key.
    -d    Do not populate DEFAULT config context on startup.
  ```


Limitations
-----------
Currently `liblepton` API does not support deletion of configuration
keys and groups. Hence, those operations have to be performed manually,
i.e. by editing configuration files in text editor.
<br />
**Note**: when you modify configuration files in external editor,
do not forget to press the `Reload` button!


License
-------
The same as Lepton EDA: GNU Public License (GPL) version 2.0 or (at your option) any later version.

