#!/bin/sh -x
# cross platform opengl framework
#	linux
#	android
#	ios	
#	osx

CC='clang -std=c11 -I/usr/include/SDL2'
SRC=src/main.c
BIN=glos
CFLAGS=$(sdl2-config --cflags)
WARNINGS=-Wall -Wextra -pedantic -Wfatal-errors -Wno-unused-function -Wno-unused-parameter
LIBS=$(sdl2-config --libs)
OPTIMIZATIONS=-O3
VALGRIND='valgrind --leak-check=full --track-origins=yes'

$CC $SRC -o $BIN $OPTIMIZATIONS $CFLAGS $LIBS $WARNINGS &&
ls -la --color $BIN && 
 $VALGRIND ./$BIN
#./$BIN