BIN=glos

#VALGRIND='valgrind --leak-check=full --track-origins=yes'
VALGRIND=

./make.sh && $VALGRIND ./$BIN
