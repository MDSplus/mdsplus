#!/bin/sh
#
getModDir() {
    if ( python -s -c 'import site' 2>/dev/null )
    then
	opt='-s'
    fi
    mdir=$(dirname $(python -E $opt -c 'import '$1';print('$1'.__file__)' 2>/dev/null) 2>/dev/null)
    if ( dirname $mdir 2>/dev/null | grep -i mdsplus > /dev/null )
    then
	mdir=$(dirname $mdir 2>/dev/null)
    fi
    echo $mdir
}

mdir=$(getModDir $1)
while [ ! -z "$mdir" ]
do
    if ( echo $mdir | grep -i $1 >/dev/null )
    then
	if ( rm -Rf $mdir )
	then
	    mdir=$(getModDir $1)
	else
	    mdir=""
	fi
    fi
done
