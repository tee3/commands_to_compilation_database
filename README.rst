Commands to Compilation Database
================================

.. contents::

Overview
--------

This is a program to generate a compilation database from the output
of a build tool.  It has built in support for the output of the
``clang`` toolset for Boost.Build, simple ``make``, and provides a
mechanism to specify the regular expression to match a compiler
command.

There is a Python version and a C++ version.

There is a program to generate a compilation database from a list of
files on the standard input.  Compiler flags can be given on the
comamnd line.

Status
------

This is a work in progress and has not been widely tested.

It has been tested with Boost.Build on Linux and OS X (not tested on
Windows yet).

The following is a minimum that needs to be tested.

- Test Python version on Windows.
- Test C++ version on Windows.
- Test ``--build-tool=make`` output for simple Makefiles.
- Test ``--compile-command-regex`` option.
- Consider root directory option.
- Test adding additional compiler strings.
- Test adding additional source file extensions.
- Test Objective-C and Objective-C++ support.
- Expand automated testing.
- Test ``--incremental`` option operation.

Motivation
----------

Many build tools provide an option to print the commands taken and
this program will take this as input and create a compilation database
from it.  This is useful with build systems that do not have native
support for generating compilation databases.

Usage
-----

commands_to_compilation_database
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To show the options, run the following commands.

::

   commands_to_compilation_database_py --help

::

   commands_to_compilation_database_cpp --help

The simplest usage is to pipe the output from the compilation to the
program.  The following command shows the usage with Boost.Build by
forcing the tool to generate all targets and dry-running the build
(which prints the commands).

::

   b2 -a -n | commands_to_compilation_database_py --build-tool=Boost.Build

If the build tool provides a mechanism to output the compilation
commands while running, the compilation database can be updated
incrementally on each build as shown below for Boost.Build.

::

   b2 -d+2 | tee | commands_to_compilation_database_py --build-tool=Boost.Build --incremental

files_to_compilation_database
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To show the options, run the following commands.

::

   files_to_compilation_database_py --help

::

   files_to_compilation_database_cpp --help

The simplest usage is to pipe a list of filenames to the program.  The
following command shows the usage with Git to generate the list of C
and C++ source and header filenames.

::

   git ls-files *.[ch] *.[ch]pp | \
       files_to_compilation_database_py \
       --cflags="-std=c89" \
       --cxxflags="-std=c++11" \
       --include=include/dir1 \
       --include=include/dir2

Requirements
------------

Python Implementation
~~~~~~~~~~~~~~~~~~~~~

- Python 2.7

C++ Implementation
~~~~~~~~~~~~~~~~~~

- Standard C++11 Compiler

   - ``auto``
   - ``std::begin``, ``std::end``
   - etc.

- Standard C++11 Standard Library

   - ``<regex>``

- Boost C++ Libraries 1.55.0

   - Boost.Program Options
   - Boost.Filesytem
   - Boost.Algorithm (String)

- Boost.Build from Boost C++ Libraries 1.55.0

Building
--------

Run the following command to build and test the system.

::

   b2

Installation
------------

Run the following command to install the system to the ``/usr/local``
prefix.

::

   b2 --prefix=/usr/local install
