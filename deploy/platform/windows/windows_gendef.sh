#!/bin/bash
dir=$(dirname $(dirname $(dirname $(dirname $0))))
if [ -z $1 ]
then build=tests
else build=releasebld
fi
for def in $(find $dir -type f -name *.def 2>/dev/null)
do
  defname=$(basename $def)
  dllname=${defname%.*}.dll
  if gendef - $dir/build/windows/$build/64/bin_x86_64/$dllname 2>&1 >$def~ | grep 'Found PE+ image'
  then diff $def~ $def
  fi
done
