#! /bin/sh

aclocal -I acinclude
autoconf
automake -a -c
./configure