BIN="glos"

VALGRIND=
#VALGRIND="valgrind --leak-check=full --track-origins=yes"

./make.sh && $VALGRIND ./$BIN
