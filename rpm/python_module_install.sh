#!/bin/sh
mdsplus_dir=$(readlink -f $(dirname ${0})/..)
module=${1}

if test "${module}" = "MDSplus"; then
  setup_install=${MDSPLUS_SETUP_PY}
  srcdir=${mdsplus_dir}/python
  if test "$(${mdsplus_dir}/rpm/python_module_remove.sh ${module} y)" = "y"; then
    setup_install=y
  fi
else
  setup_install=
  srcdir=${mdsplus_dir}/pydevices
fi

if test "${setup_install}" = "y"; then
  echo "MDSPLUS_SETUP_PY has been removed, and ${mdsplus_dir}/python has been placed in the PYTHONPATH in ${mdsplus_dir}/etc/envsyms"
fi