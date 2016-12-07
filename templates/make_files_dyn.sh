#/bin/sh -x
sh template.sh dyn dynp void* &&
sh template.sh dyn dynf float &&
sh template.sh dyn dync char &&
sh template.sh dyn dyni int &&
mv -f dyn*.h ../src &&
ls -la --color ../src/dyn*.h

