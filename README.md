# PolyBob

PolyBob is a polynomials manager able to manipulate expressions inputed from its shell.

This project was written as part of a work for our engineering school.

##License and credit

All code is released under the [MIT license](http://opensource.org/licenses/MIT). Nonetheless, we use third-party libraries that remain of course under their respective license (see below).

Furthermore even if Taiki handled commits, the software was designed by [mlpo](https://github.com/mlpo) (mathematics and algorithms in Maths/) and [Taiki](https://github.com/TaikiMMT) (parser and solver in Parser/). Matsuharo and [Gregory Le](https://github.com/gregtaole) were also involved in the creation of the user interface (CLI).

## Supported expressions:

### Allocation
`{variable} = expression`

### Compare
`expression1 == expression2`

### Simple calcul
`expression`

Parenthesis are supported, variable can be created and used by typing their names between braces.
Functions can be called by typing their names and putting their arguments between brackets.

## Details

-	Unknowns can be either called x or z, but they will be interpreted as z
-	Spaces are no-op, including inside variable names, {var name} == {varname}

## Third-party libraries and compilation

PolyBob require [Boost Spirit](www.boost.org/doc/libs/release/libs/spirit/) (under the Boost Software License) and [Linenoise](www.boost.org/doc/libs/release/libs/spirit/) (under the BSD 2-Clause License) to compile, a copy of both of them is available in the "Libraries" directory, under their respective licence.

Makefile and Xcode projects are included, clang is best supported.
