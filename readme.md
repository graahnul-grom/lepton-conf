Lepton EDA configuration utility
================================

lepton-conf is designed to ease the process of editing [Lepton EDA](https://github.com/lepton-eda/lepton-eda)
(Lepton Electronic Design Automation)
configuration that is stored in ini-like files (`*.conf`) - that is, new-style configuration, as opposed
to settings stored in `Guile` source code files. It is a graphical user interface application
(using GTK 2.x, see [screenshots](https://graahnul-grom.github.io/lepton-conf)) that does
essentially the same as [`lepton-cli(1)`](https://graahnul-grom.github.io/ref-man/lepton-cli.html)
command-line utility invoked with the `config` command.
<br />

Documentation
------------
Information about the configuration system can be found on the [Configuration Settings](https://github.com/lepton-eda/lepton-eda/wiki/Configuration-Settings) lepton-eda wiki page.

Dependencies
------------

- The [Lepton EDA](https://github.com/lepton-eda/lepton-eda) suite, version [1.9.8](https://github.com/lepton-eda/lepton-eda/releases/tag/1.9.8-20190928) or later
(at least git [039c08c](https://github.com/lepton-eda/lepton-eda/commit/039c08c6fd106a8402cc690206eeae3ac4d1a7e7), Dec 19, 2018).

Installation from source
------------------------

* Checkout:

```sh
  $ git clone https://github.com/graahnul-grom/lepton-conf.git
  $ cd lepton-conf
```

* Build:

```sh
  $ make
```

  If Lepton EDA is installed in non-standard location, adjust the `$PKG_CONFIG_PATH`
  environment variable when running `make`, e.g.:

```sh
  $ PKG_CONFIG_PATH=/path/to/lepton-eda/lib/pkgconfig make
```

  If GTK `pkgconfig` package (`gtk+-2.0`) has different name on your system, edit it
  at the top of the `Makefile`.

* Install:

  Copy `lepton-conf` executable file anywhere in your `$PATH`

* Run:

```sh
  $ lepton-conf
```

  If Lepton EDA is installed in non-standard location, you may have to set the
  `$LD_LIBRARY_PATH` environment variable, so that `liblepton.so` can be found, e.g.:

```sh
  $ LD_LIBRARY_PATH=/path/to/lepton-eda/lib lepton-conf
```

  Command line arguments:

```
  $ lepton-conf -h

  Usage: lepton-conf [OPTIONS] [PATH]
  Options:
    -l    Legacy config mode: use geda*.conf configuration files
    -e    Close window with 'Escape' key
    -r    Restore last working directory
    -m    Do not warn about missing configuration files
    -d    Do not populate the DEFAULT configuration context on startup
    -p    Print the DEFAULT context in the *.conf file format and exit
    -h    Show usage information
    -v    Show version information
```

License
-------
The same as Lepton EDA: GNU Public License (GPL) version 2.0 or (at your option) any later version.
<br />
Copyright (C) 2017-2021 [dmn](https://github.com/graahnul-grom)

