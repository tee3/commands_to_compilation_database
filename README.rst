Commands to Compilation Database
================================

.. contents::

Overview
--------

This is a program to generate a compilation database from the output
of a build tool.

There is a Python version and a C++ version.

Motivation
----------

Many build tools provide an option to print the commands taken and
this program will take this as input and create a compilation database
from it.

Usage
-----

To show the options, run the following commands.

::

   $ commands_to_compilation_database_py --help

::

   $ commands_to_compilation_database_cpp --help

The simplest usage is to pipe the output from the compilation to the
program.  The following command shows the usage with Boost.Build by
forcing the tool to generate all targets and dry-running the build
(which prints the commands).

::

   $ b2 -a -n | commands_to_compilation_database_py --build-tool=Boost.Build

If the build tool provides a mechanism to output the compilation
commands while running, the compilation database can be updated
incrementally on each build as shown below for Boost.Build.

::

   $ b2 -d+2 | tee | commands_to_compilation_database_py --build-tool=Boost.Build --incremental

Requirements
------------

Python Implementation
~~~~~~~~~~~~~~~~~~~~~

- Python 2.7

C++ Implementation
~~~~~~~~~~~~~~~~~~

- Standard C++11 Compiler

   - auto
   - std::begin, std::end
   - etc.

- Standard C++11 Standard Library

   - <regex>

- Boost C++ Libraries 1.55.0

   - Boost.Program Options
   - Boost.Filesytem
   - Boost.Algorithm (String)

- Boost.Build from Boost C++ Libraries 1.55.0

Building
--------

Run the following command to build and test the system.

::

   $ b2
