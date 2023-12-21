#/bin/sh -x
echo "generated source:"
./dyn.apply str char &&
mv -f *.h ../src/lib/ &&

#./alloc.apply alloc object objects 32768 &&
#mv -f *.h ../src/obj/ &&

echo
