#!/bin/sh

set -xe

CC="${CXX:-cc}"
CFLAGS="-Wall -Wextra -std=c11 -pedantic -g"
LIBS="-L../../build -lserver "

$CC $CFLAGS $LIBS main.c -o main