#/bin/sh -x
./template dyn dynp void* &&
./template dyn dynf float &&
./template dyn str char &&
./template dyn dyni int &&
mv -f *.h ../src/lib/ &&
#ls -la --color ../src/lib/*.h

./template dyn objmtls objmtl* &&
mv -f *.h ../src/gfx/ &&

echo done

