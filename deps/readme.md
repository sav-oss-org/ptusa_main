# Libraries used

## Lua

Lua is a scripting programming language. A characteristic feature of Lua is
implementation of a large number of program entities with a minimum of
syntactic means.

Thus, all composite user data types (arrays, structures, sets, queues, lists)
are implemented through the mechanism of tables, and the mechanisms of
object-oriented programming, including multiple inheritance, are implemented
using metatables, which are also responsible for overloading operations and a
number of other features.
Lua is a procedural dynamically typed modular language with automatic memory
management. Includes basic elements to support functional and object
programming styles. Thus, Lua can be called a multi-paradigm language.
Built-in parallel programming tools allow you to write multithreaded programs
only by means of the language, without accessing the operating system API or
external libraries.

Since the main purpose of Lua is embedding, it has effective means of
interlanguage interaction, focused mainly on calling C libraries and working
in a C environment. The language supports a small number of built-in data
types: boolean values, numbers, strings, functions, streams. There are no
typical combined data structures, such as arrays, sets, lists, and records;
instead of all of them, one basic Lua structure is used a table.

A separate type of *userdata* is designed specifically for low-level
programming and data exchange with external code in other languages.
Functions in Lua are objects of the first class, can be assigned and passed
in parameters. Closures are supported, it is possible to create higher-order
functions. The object system is prototypical, there is no explicit support for
inheritance, but it is easily implemented using metatables.

Detailed information is available in the
[Lua documentation](https://www.lua.org).

## tolua++

Tolua is a tool that greatly simplifies the integration of C/C++ code with Lua.
Based on a cleaned header file, tolua automatically generates the binding code
to access C/C++ features from Lua. Using Lua API and tag method facilities,
tolua maps C/C++ constants, external variables, functions, classes, and methods
to Lua.
To use tolua, create a package file, a C/C++ cleaned header file, listing the
constants, variables, functions, classes, and methods to want to export to the
Lua environment. Then tolua parses this file and creates a C/C++ file that
automatically binds the C/C++ code to Lua. If link the created file with our
application, the specified C/C++ code can be accessed from Lua.

Detailed information is available in the
[tolua documentation](
https://web.tecgraf.puc-rio.br/~celes/tolua/tolua-3.2.html).

## zlib

Zlib is a software library used for data compression and is an abstraction of
the DEFLATE compression algorithm used in their gzip file compression program.

Detailed information is available at the
[zlib website](https://www.zlib.net).

## subhook

SubHook is a simple hooking library for C and C++ that works on Windows, Linux
and macOS. It supports x86 only (32-bit and 64-bit).

Detailed information is available in the
[subhook documentation](
https://github.com/Zeex/subhook).

## cxxopts

Cxxopts is a lightweight C++ option parser library, supporting the standard
GNU style syntax for options.

Detailed information is available in the
[cxxopts documentation](
https://github.com/jarro2783/cxxopts).

## googletest

Google Test is a library for unit testing in C++. Google Test is built on the
xUnit testing methodology, that is, when individual parts of the program
(classes, functions, modules) are tested separately from each other, in
isolation. The library itself is developed with the active use of testing,
when adding any parts to the official version, in addition to the code of the
changes themselves, it is necessary to write a set of tests to confirm their
correctness.

Detailed information is available in the
[Google Test documentation](
https://github.com/google/googletest).

## google benchmark

Google benchmark is a microbenchmark support library. The library can be used
with C++03. However, it requires C++11 to build, including compiler and
standard library support.

Detailed information is available in the
[Google Benchmark documentation](
https://github.com/google/benchmark).
