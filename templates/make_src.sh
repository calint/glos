#/bin/sh -x
echo "generated source:"
./dyn.apply dynp void* &&
./dyn.apply dynf float &&
./dyn.apply str char &&
./dyn.apply dyni int &&
mv -f *.h ../src/lib/ &&

./dyn.apply objmtls objmtl* &&
mv -f *.h ../src/gfx/ &&
echo
