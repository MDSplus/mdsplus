#!/bin/sh
mdsplus_dir=$(readlink -f $(dirname ${0})/..)
module=${1}

if test "${module}" = "MDSplus"; then
  setup_install=${MDSPLUS_SETUP_PY}
  srcdir=${mdsplus_dir}/python
  if test "$(${mdsplus_dir}/rpm/python_module_remove.sh ${module} y)" = "y"; then
    setup_install=y
  elif test -z "${MDSPLUS_SETUP_PY}"; then
    echo "Set MDSPLUS_SETUP_PY=y if you wish to run setup.py install."
  fi
else
  setup_install=
  srcdir=${mdsplus_dir}/pydevices
fi

for python in python2 python3; do
  if ( ${python} -c pass >/dev/null 2>&1 ); then
    ${python} -m compileall -q -f ${srcdir}/${module} >/dev/null 2>&1
    if test "${setup_install}" = "y"; then
      if ! ${python} ${srcdir}/${module}/setup.py -q install >/dev/null; then
        echo "Could not 'setup.py install' MDSplus for ${python}"
      fi
    fi
  fi
done
