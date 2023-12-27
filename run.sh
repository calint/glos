BIN="glos"

VALGRIND=
#VALGRIND="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes"
#VALGRIND="valgrind --leak-check=full --track-origins=yes"
#VALGRIND="valgrind --leak-check=full"
#VALGRIND="valgrind --tool=cachegrind"
./make.sh && $VALGRIND ./$BIN
