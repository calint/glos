#!/bin/bash
# dependencies (in ubuntu installation):
# * libglm-dev/mantic,mantic,now 0.9.9.8+ds-7 all: opengl math
# * libtbb-dev/mantic 2021.9.0-2ubuntu1 i386: for parallel unsequenced `for_each`
# * libsdl2-dev/mantic,now 2.28.3+dfsg-2 amd64
# * libsdl2-gfx-dev/mantic,now 1.0.4+dfsg-5 amd64
# * libsdl2-image-dev/mantic,now 2.6.3+dfsg-2 amd64
# * libsdl2-ttf-dev/mantic,now 2.20.2+dfsg-1 amd64
# * opengl related packages

set -e

#CC="g++ -std=c++20 -Wno-changes-meaning -fanalyzer"
CC="clang++ -std=c++20"
SRC="src/main.cpp src/application/application.cpp"
BIN="glos"
CFLAGS="-Wfatal-errors $(sdl2-config --cflags)"
LIBS="-ltbb -lGL -lSDL2_image -lSDL2_ttf $(sdl2-config --libs)"
WARNINGS="-Wall -Wextra -Wpedantic \
    -Wshadow -Wconversion -Wsign-conversion \
    -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter"
OPTIMIZATION="-O3"
DEBUG="-g"
if [[ "$1" == "release" ]]; then
    DEBUG=""
fi
PROFILE=""
if [[ "$1" == "profile" ]]; then
    PROFILE="-pg"
fi
LDFLAGS=""
if [[ "$1" == "sanitize" ]]; then
#    LDFLAGS="-fsanitize=address -fsanitize-address-use-after-scope"
    LDFLAGS="-fsanitize=thread"
fi

CMD="$CC -o $BIN $SRC $DEBUG $PROFILE $OPTIMIZATION $CFLAGS $LDFLAGS $WARNINGS $LIBS"
echo $CMD
$CMD
echo
ls -la --color $BIN
echo
