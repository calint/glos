sh template.sh dyn dynp void* &&
sh template.sh dyn dynf float &&
sh template.sh dyn dync char &&
sh template.sh dyn dyni int &&
cp -ar dyn*.h ../src