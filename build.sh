#!/bin/sh
set -e

CC="gcc -std=gnu11"
#CC="clang++ -std=c++20"
SRC=src/main.c
BIN=glos
CFLAGS="$(sdl2-config --cflags)"
WARNINGS="-Wall -Wextra -Wfatal-errors -Wno-unused-function -Wno-unused-parameter"
LIBS="$(sdl2-config --libs) -lGL -lm -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer"
#OPTIMIZATION=-O3
OPTIMIZATION="-O0 -g"
#REMARKS=-Rpass=inline
REMARKS=
#PROFILE=-pg
PROFILE=

$CC $PROFILE $SRC -o $BIN $OPTIMIZATION $CFLAGS $LIBS $WARNINGS

ls -la --color $BIN
