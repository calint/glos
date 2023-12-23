#!/bin/sh
set -e

#CC="gcc -std=gnu11"
CC="clang++ -std=c++20"
SRC="src/main.cpp"
BIN="glos"
CFLAGS="$(sdl2-config --cflags) -fno-exceptions -fno-rtti"
WARNINGS="-Wall -Wextra -Wpedantic -Wfatal-errors \
    -Wconversion -Wsign-conversion \
    -Wno-unsafe-buffer-usage -Wno-unused-function -Wno-unused-parameter"
LIBS="$(sdl2-config --libs) -lGL -lm -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lSDL2_mixer"
#OPTIMIZATION=-O3
OPTIMIZATION="-O3 -g"
REMARKS=
#REMARKS=-Rpass=inline
PROFILE=
#PROFILE=-pg

$CC $PROFILE $SRC -o $BIN $OPTIMIZATION $CFLAGS $LIBS $WARNINGS

ls -la --color $BIN
