#!/bin/sh

CC='clang -std=c11'
SRC=src/main.c
BIN=glos

CFLAGS=$(sdl2-config --cflags)

WARNINGS='-Wall -Wextra -pedantic -Wfatal-errors -Wno-unused-function -Wno-unused-parameter -Wno-incompatible-pointer-types-discards-qualifiers'

LIBS="$(sdl2-config --libs) -lGL -lm -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer"

OPTIMIZATION=-O3
#OPTIMIZATION="-O0 -g"

#REMARKS=-Rpass=inline
REMARKS=

#PROFILE=-pg
PROFILE=

#VALGRIND='valgrind --leak-check=full --track-origins=yes'
VALGRIND=

$CC $PROFILE $SRC -o $BIN $OPTIMIZATION $CFLAGS $LIBS $WARNINGS &&

ls -la --color $BIN && 

$VALGRIND ./$BIN
