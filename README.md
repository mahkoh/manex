manex
=====

manex is a harmless tool to create and view example man-pages.

Usage
-----

![manex basic](http://i.imgur.com/Lf3hNq8.png)

Let's try `manex utils`:

![manex utils](http://i.imgur.com/Cjig1jr.png)

`manex utils ssh`: 

![manex utils ssh](http://i.imgur.com/BUT6yfr.png)

Manex opens a man-page in your man-pager (vim in my case).

Source code
-----------

Manex uses a simple, custom language.
Here is the source code of the ssh page:

![manex code](http://i.imgur.com/2YN06Wq.png)

These files are converted to man-pages at compile time:

![manex compile](http://i.imgur.com/h41AHqI.png)

Adding new manex pages
----------------------

We want to add a stdio example for `rust`.
To do this, we go to the `pre` directory and create a new folder `rust`.
In there we create a new file `io.mx` and add the example:

![manex rust](http://i.imgur.com/Bj2OWWG.png)

`make` generates the man-page:

![manex rust compile](http://i.imgur.com/eSjEyjY.png)

`manex rust io`:

![manex rust done](http://i.imgur.com/tHKiNv0.png)

zsh
---

There is a zsh completion file in `contrib`:

![manex completion](http://i.imgur.com/h7AVAzh.png)

Note that this file is generated by `configure`.

Installation
------------

    ./configure && make && sudo make install

The default prefix is `/usr/local`.
You can set another prefix with the `$PREFIX` variable.

    PREFIX=/home/username ./configure && make && make install

Highlighting
------------

Per-language highlighting is possible if you use vim as your man-pager but hasn't been implemented yet.

Contributing
-----------

Pull requests are welcome.
Some syntax details:

* Empty lines don't matter but please add them between examples and keep them out of examples.
* All lines except the first one have to follow one of these rules:
  - Be empty
  - Be "example", "code", or "text"
  - Start with a space

License
-------

The files in `pre/utils` come from the [cheat](https://github.com/chrisallenlane/cheat) project and are *GPL3*.

The files in `pre/golang` come from [Go by Example](https://github.com/mmcgrana/gobyexample) and are *Creative Commons Attribution 3.0 Unported*.

All other files in `pre` are *CC0 Public Domain*.

All other files are *MIT*.
