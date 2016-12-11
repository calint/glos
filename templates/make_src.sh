#/bin/sh -x
sh template.sh dyn dynp void* &&
sh template.sh dyn dynf float &&
sh template.sh dyn str char &&
sh template.sh dyn dyni int &&
mv -f *.h ../src/lib/ &&
ls -la --color ../src/lib/*.h


