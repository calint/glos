#!/bin/sh

CC='gcc -std=gnu11'
SRC=src/main.c
BIN=glos

CFLAGS="$(sdl2-config --cflags)"

WARNINGS='-Wall -Wextra -pedantic -Wfatal-errors -Wno-unused-function -Wno-unused-parameter -Wno-incompatible-pointer-types-discards-qualifiers'
WARNINGS='-Wall -Wextra -Wfatal-errors -Wno-unused-function 
   -Wno-unused-parameter -Wno-incompatible-pointer-types-discards-qualifiers -Wno-discarded-qualifiers'

LIBS="$(sdl2-config --libs) -lGL -lm -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer"

OPTIMIZATION=-O3
#OPTIMIZATION="-O0 -g"

#REMARKS=-Rpass=inline
REMARKS=

#PROFILE=-pg
PROFILE=

$CC $PROFILE $SRC -o $BIN $OPTIMIZATION $CFLAGS $LIBS $WARNINGS &&

ls -la --color $BIN
