BIN="glos"

VALGRIND=
#VALGRIND="valgrind --leak-check=full --track-origins=yes"
#VALGRIND="valgrind --tool=cachegrind"
./make.sh && $VALGRIND ./$BIN
