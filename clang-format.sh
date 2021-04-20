#!/bin/sh
dir=$(dirname $0)
exclude_path="-path .git"
exclude_file=
for o in build mitdevices opcbuiltins.h 3rd-party-apis mdstcpip/udt4 mdstcpip/zlib mdsobjects/cpp/rapidjson
do
 if test -d $o
 then exclude_path="$exclude_path -o -path $dir/$o"
 else exclude_file="$exclude_file ! -name $o"
 fi
done

find $dir/$1 \( $exclude_path \) -prune -false\
 -o \( -type f \( -regex '.*\.\(c\|h\)\(pp\|\)$' $exclude_file \) \)\
 -exec clang-format -i -style=file \{\} \;\
 -exec sed -i -E '{N;s/(\/\/[^\n]*)\\\n\s*\/\//\1/g;P;D}' \{\} \;
