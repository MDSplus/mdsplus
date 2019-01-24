#!/bin/bash
dir=$(dirname $0)
for def in $(find $dir -type f -name *.def 2>/dev/null)
do
  defname=$(basename $def)
  dllname=${defname%.*}.dll
  gendef - $dir/build/windows/tests/64/bin_x86_64/$dllname > $def
done
