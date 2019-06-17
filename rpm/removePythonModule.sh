#!/bin/sh

getModDir() {
    if ( $2 -s -c 'import site' 2>/dev/null )
    then
	opt='-s'
    fi
    mdir=$(dirname $($2 -E $opt -c 'import '$1';print('$1'.__file__)' 2>/dev/null) 2>/dev/null)
    if ( dirname $mdir 2>/dev/null | grep -i mdsplus > /dev/null )
    then
	mdir=$(dirname $mdir 2>/dev/null)
    fi
    echo $mdir
}

for py in python2 python3
do
  mdir=$(getModDir $1 ${py})
  while [ ! -z "$mdir" ]
  do
    if ( echo $mdir | grep -i $1 >/dev/null )
    then
	if [ -L $mdir ]
	then
	  rm -f $mdir
	else
	  rm -Rf $mdir
	fi
	if [ "$?" = 0 ]
	then
	  mdir=$(getModDir $1 ${py})
          continue # check if there is more
	fi
    fi
    break # out of while loop
  done
done
