#!/bin/sh
# dependencies (ubuntu packages):
# * libglm-dev/mantic,mantic,now 0.9.9.8+ds-7 all: opengl math
# * libtbb-dev/mantic 2021.9.0-2ubuntu1 i386: for parallel unsequenced `for_each`
# * libsdl2-dev/mantic,now 2.28.3+dfsg-2 amd64
# * libsdl2-gfx-dev/mantic,now 1.0.4+dfsg-5 amd64
# * libsdl2-image-dev/mantic,now 2.6.3+dfsg-2 amd64
# * libsdl2-ttf-dev/mantic,now 2.20.2+dfsg-1 amd64
# * opengl related packages

set -e

#CC="g++ -std=c++20 -Wno-changes-meaning -fanalyzer"
#CC="g++ -std=c++20 -Wno-changes-meaning"
CC="clang++ -std=c++20"
SRC="src/main.cpp src/app/application.cpp"
BIN="glos"
CFLAGS="$(sdl2-config --cflags)"
LDFLAGS=
#LDFLAGS="-fsanitize=address -fsanitize-address-use-after-scope"
#LDFLAGS="-fsanitize=thread"
LIBS="-lGL -lSDL2_image -lSDL2_ttf -lSDL2_mixer $(sdl2-config --libs) -ltbb"
# note. -ltbb must be last in list
WARNINGS="-Wall -Wextra -Wpedantic -Wfatal-errors \
    -Wshadow -Wconversion -Wsign-conversion \
    -Wno-unsafe-buffer-usage \
    -Wno-unused-function -Wno-unused-parameter"
OPTIMIZATION="-O3 -g"
REMARKS=
#REMARKS="-Rpass=inline"
PROFILE=
#PROFILE="-pg"

CMD="$CC -o $BIN $SRC $PROFILE $REMARKS $OPTIMIZATION $CFLAGS $LDFLAGS $WARNINGS $LIBS"
echo $CMD
$CMD
echo
ls -la --color $BIN
echo
