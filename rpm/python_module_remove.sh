#!/bin/sh
mdsplus_dir=$(readlink -f $(dirname ${0})/..)
module=${1}

get_mod_dir() {
  python=$1
  if ( ${python} -s -c 'import site' >/dev/null 2>&1 ); then
    opt='-s'
  fi
  mdir=$(realpath $(${python} -E $opt -c 'import pkgutil;print(pkgutil.get_loader("'${module}'").filename)')) 2>/dev/null
  if (echo $mdir | grep "${mdsplus_dir}" >/dev/null); then
    echo $mdir >&2
    return  # imported form MDSPLUS_DIR
  fi
  echo $mdir
}

removed=n
for python in python2 python3; do
  mdir=$(get_mod_dir ${python})
  while [ ! -z "$mdir" ]; do
    if ( echo $mdir | grep -i ${module}} >/dev/null ); then
      if [ -L $mdir ]; then
        rm -f $mdir
      else
        rm -Rf $mdir
      fi
      echo $mdir >&2
      if [ $? -eq 0 ]; then
        removed=y
        mdir=$(get_mod_dir ${python})
        continue # check if there is more
      fi
    fi
    break # out of while loop
  done
done

rm -Rf ${mdsplus_dir}/python/${module}*egg-info
if test -n "$2"; then
  echo $removed
else
  rm -Rf ${mdsplus_dir}/python/${module}
  if test "${module}" != "MDSplus"; then
    ${mdsplus_dir}/pydevices/${module}
  fi
  rmdir ${mdsplus_dir}/python 2>/dev/null || :
  rmdir ${mdsplus_dir}/pydevices 2>/dev/null || :
fi
