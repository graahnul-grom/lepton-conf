Lepton EDA configuration utility
================================

lepton-conf is designed to ease the process of editing
[Lepton EDA](https://github.com/lepton-eda/lepton-eda)
(Lepton Electronic Design Automation)
configuration that is stored in ini-like files (`*.conf`) - that is,
new-style configuration, as opposed to settings stored in `Guile`
source code files.
<br />
It is a graphical user interface application (using GTK 2.x, see
[screenshots](https://graahnul-grom.github.io/lepton-conf))
that does essentially the same as
[lepton-cli(1)](https://lepton-eda.github.io/man/lepton-cli.html)
command-line utility invoked with the `config` command.
<br />

<p align="center">
<a href="https://graahnul-grom.github.io/lepton-conf/lepton-conf-main-window.png">
  <img src="https://graahnul-grom.github.io/lepton-conf/lepton-conf-main-window.png" />
</a>
</p>

Documentation
------------
Information about the configuration system can be found on the
[Configuration Settings](https://github.com/lepton-eda/lepton-eda/wiki/Configuration-Settings)
wiki page and in the
[Lepton EDA Reference Manual](https://lepton-eda.github.io/lepton-manual.html/index.html).

Dependencies
------------

- The [Lepton EDA](https://github.com/lepton-eda/lepton-eda) suite, version
[1.9.8](https://github.com/lepton-eda/lepton-eda/releases/tag/1.9.8-20190928)
or later (at least git
[039c08c](https://github.com/lepton-eda/lepton-eda/commit/039c08c6fd106a8402cc690206eeae3ac4d1a7e7),
Dec 19, 2018).

- A C compiler.

- [pkg-config](http://pkgconfig.freedesktop.org).

- [GTK+](http://www.gtk.org) and its development files, version 2.24.0 or later.

- [Autoconf](http://www.gnu.org/software/autoconf)

- [Automake](http://www.gnu.org/software/automake)

Installation from source
------------------------

* Checkout source code:

```sh
  $ git clone https://github.com/graahnul-grom/lepton-conf.git
  $ cd lepton-conf
```

* Run `autogen.sh` to generate the configuration script:

```sh
  $ ./autogen.sh
```

* Run the configuration script. You can pass it the `--prefix` option
if you want to install `lepton-conf` to custom location, e.g.
`./configure --prefix=$HOME/lepton-conf`:

```sh
  $ ./configure
```

  If `Lepton EDA` is installed in non-standard location, adjust the `$PKG_CONFIG_PATH`
  environment variable when running the `configure` script:

```sh
  $ PKG_CONFIG_PATH=/path/to/lepton-eda/lib/pkgconfig ./configure
```

* Build:

```sh
  $ make
```

* Install (you may need to run this as root):

```sh
  $ make install
```

* Run the application:

```sh
  $ lepton-conf
```

  If `Lepton EDA` is installed in non-standard location, set the
  `$LD_LIBRARY_PATH` environment variable when running `lepton-conf`:

```sh
  $ LD_LIBRARY_PATH=/path/to/lepton-eda/lib lepton-conf
```

License
-------
The same as Lepton EDA: GNU Public License (GPL) version 2.0
or (at your option) any later version.
<br />
Copyright (C) 2017-2021 [dmn](https://github.com/graahnul-grom)

