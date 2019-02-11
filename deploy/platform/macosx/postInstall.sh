#!/bin/sh
pushd /usr/local/mdsplus/lib
for i in `ls -1 *.dylib`; do ln -s /usr/local/mdsplus/lib/$i /usr/local/lib/; done
popd
. /usr/local/mdsplus/scripts/postinstall

