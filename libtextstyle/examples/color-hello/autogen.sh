#!/bin/sh
# This file is in the public domain.
#
# Script for regenerating all autogenerated files.

mkdir -p build-aux
cp -p ../../build-aux/config.rpath build-aux/config.rpath
aclocal -I../../m4 -I../../gnulib-m4
autoconf
automake -a -c
rm -rf autom4te.cache
