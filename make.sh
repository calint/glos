#!/bin/sh
set -e

#CC="g++ -std=c++20 -Wno-changes-meaning"
CC="clang++ -std=c++20"
SRC="src/main.cpp"
BIN="glos"
CFLAGS="-fno-rtti $(sdl2-config --cflags)"
LDFLAGS=
#LDFLAGS="-fsanitize=address -fsanitize-address-use-after-scope"
LIBS="-lGL -lSDL2_image -lSDL2_ttf -lSDL2_mixer $(sdl2-config --libs)"
WARNINGS="-Wall -Wextra -Wpedantic -Wfatal-errors \
    -Wconversion -Wsign-conversion \
    -Wno-unsafe-buffer-usage \
    -Wno-unused-function -Wno-unused-parameter"
OPTIMIZATION="-O3 -g"
REMARKS=
#REMARKS="-Rpass=inline"
PROFILE=
PROFILE="-pg"

CMD="$CC -o $BIN $SRC $PROFILE $REMARKS $OPTIMIZATION $CFLAGS $LDFLAGS $LIBS $WARNINGS -ltbb"
echo $CMD
$CMD
echo
ls -la --color $BIN
echo
