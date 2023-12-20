#/bin/sh -x
echo "generated source:"
./dyn.apply dynp void* &&
./dyn.apply str char &&
./dyn.apply dyni int &&
mv -f *.h ../src/lib/ &&

./alloc.apply alloc object objects 32768 &&
mv -f *.h ../src/obj/ &&

echo
