lepton-conf - configuration utility for [Lepton EDA](https://github.com/lepton-eda/lepton-eda)
====================================================

lepton-conf is designed to ease the process of editing Lepton EDA configuration that
is stored in ini-like files (`geda.conf`) - that is, new-style configuration, as opposed
to settings stored in `guile` source code files. It is a graphical user interface application
(GTK 2.x) that does essentially the same as `lepton-cli` ("[`gaf`](http://wiki.geda-project.org/geda:gaf_utility)")
command-line utility invoked with `config` command.<br />
Some [screenshots](https://graahnul-grom.github.io/lepton-conf).

Installation
============

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


Limitations
===========
Currently `liblepton` API does not support deletion of configuration
keys and groups. Hence, those operations have to be performed manually,
i.e. by editing configuration files in text editor.

