#/bin/sh -x
echo "generated source:"
./template dyn dynp void* &&
./template dyn dynf float &&
./template dyn str char &&
./template dyn dyni int &&
mv -f *.h ../src/lib/ &&

./template dyn objmtls objmtl* &&
mv -f *.h ../src/gfx/ &&
echo
