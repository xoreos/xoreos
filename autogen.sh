#!/bin/sh

aclocal -I m4 && autoheader && automake && autoconf
