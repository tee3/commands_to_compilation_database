Commands to Compilation Database
================================

.. contents::

Overview
--------

This is a program to generate a compilation database from the output
of a build tool.

There is a Python version and a C++ version.  Currently, the C++
version does not provide for command-line options and is for testing
only.

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

Building
--------

Run the following command to build the system.

::

   $ make

Run the following command to test the system.

NOTE: Currently, the tests fail for the C++ program.

::

   $ make test
