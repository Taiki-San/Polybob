Polybob is a polynom manager able to manipulate expressions inputed from its shell.
This project was written as a project for our engineering school

# Supported expressions:

## Allocation:
`{variable} = expression`

## Compare
`expression1 == expression2`

## Simple calcul
`expression`

Parenthesis are supported, variable can be created and used by typing their names between braces.
Functions can be called by typing their names and putting their arguments between brackets.

# Details

-	Unknowns can be either called x or z, but they will be interpreted as z
-	Spaces are no-op, including inside variable names, {var name} == {varname}

# Compilation

Polybob require Boost and linenoise to compile, a copy of both of them is available in the 'Libraries' directory, under their respective licence.
Makefile and Xcode projects are included, clang is best supported