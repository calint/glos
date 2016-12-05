template=$1
name=$2
type=$3

cp $1 $name.h &&
sed -i "s/\${name}/$name/g" $name.h &&
sed -i "s/\${type}/$type/g" $name.h &&
ls -la --color $name.h