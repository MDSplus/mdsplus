#!/bin/sh
mdsplus_dir=$(readlink -f $(dirname ${0})/..)
module=${1}

if test "${module}" = "MDSplus"; then
  srcdir=${mdsplus_dir}/python
  if test -n "$(${mdsplus_dir}/rpm/python_module_remove.sh ${module} y)"; then
    setup_install=y
  elif ( echo ${PYTHONPATH} | grep ${mdsplus_dir}/python >/dev/null ); then
    setup_install=n
  else
    read -t 10 -p "Do you wish to run setup.py install? (y/N) " setup_install
  fi
else
  srcdir=${mdsplus_dir}/pydevices
  setup_install=n
fi

for python in python2 python3; do
  if ( ${python} -c pass >/dev/null 2>&1 ); then
    ${python} -m compileall -q -f ${srcdir}/${module} >/dev/null 2>&1
    if test "${setup_install}" = "y"; then
      if ! ${python} ${mdsplus_dir}/python/MDSplus/setup.py -q install >/dev/null; then
        echo "Could not 'setup.py install' MDSplus for ${python}"
      fi
    fi
  fi
done
