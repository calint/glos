#/bin/sh -x
echo "generated source:"
./tmpl_dyn dynp void* &&
./tmpl_dyn dynf float &&
./tmpl_dyn str char &&
./tmpl_dyn dyni int &&
mv -f *.h ../src/lib/ &&

./tmpl_dyn objmtls objmtl* &&
mv -f *.h ../src/gfx/ &&
echo
