lepton-conf - configuration utility for [Lepton EDA](https://github.com/lepton-eda/lepton-eda)
====================================================

Installation
============

Dependencies
------------

- [Lepton EDA](https://github.com/lepton-eda/lepton-eda) suite

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

