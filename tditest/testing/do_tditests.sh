#!/bin/bash
TMP_LD_PRELOAD="$LD_PRELOAD"
unset LD_PRELOAD
test=$(basename "$1")
test=${test%.tdi}
if [[ "$TMP_LD_PRELOAD" == *"libtsan.so"* ]] && [[ "$test" == "test-dev-py" ]]; then
  echo "test-dev-py hangs in Tcl('add node a12/model=a12') @ tsan"
  exit 77
fi
srcdir=$(readlink -f $(dirname ${0}))
if [ "$OS" == "windows" ]; then
  zdrv="Z:"
  PYTHON="wine python"
  TDITEST="wine tditest"
  DIFF_Z=-Z
else
  zdrv=""
  PYTHON=${PYTHON:-python}
  TDITEST=tditest
  DIFF_Z=
fi

status=0

run() {
  eval $1 2>&1 | tee ${2-/dev/null} |
    grep -v -e '^[DIWE],'\
      -e '^\s*Data inserted:'\
      -e '^Connected:'\
      -e 'Length:'
}

if [ ! -z $1 ]; then
  if [[ "$test" == *"tab"* ]]; then
    cmd="$TDITEST <    $srcdir/$test.tdi"
    # fixes [?1034h for old readline verisons, rhel5/6/7, fc17/18/20
    export TERM=vt100
  else
    cmd="$TDITEST $zdrv$srcdir/$test.tdi $zdrv$(pwd) 2 3"
  fi

  if [ -z ${MDSPLUS_DIR} ]; then
    MDSPLUS_DIR=$(readlink -f ${srcdir}/../..)
  fi
  # use tmpdir tobisolate shotdb.sys
  tmpdir=$(mktemp -d)
  trap 'rm -Rf ${tmpdir}' EXIT
  MDS_PATH=".;${MDSPLUS_DIR}/tdi;."
  MDS_PYDEVICE_PATH="${MDSPLUS_DIR}/pydevices;${MDSPLUS_DIR}/python/MDSplus/tests/devices"
  subtree_path="${tmpdir};${MDSPLUS_DIR}/trees/subtree"
  main_path="${tmpdir};${MDSPLUS_DIR}/trees"

  if [[ $test == *"py"* ]]; then
    LD_PRELOAD="$TMP_LD_PRELOAD"
    if [ -z $PyLib ]; then
      echo no python lib
      exit 77
    fi
  fi
  if [[ $test == *"dev"* ]]; then
    found=0
    for path in ${LD_LIBRARY_PATH//:/ }; do
      if [ -e $path/libMitDevices.so ] || [ -e $path/libMitDevices.dylib ]; then
        found=1
      fi
    done
    if [ $found == 0 ]; then
      echo "no libMitDevices.so or no libMitDevices.dylib (macOS)"
      exit 77
    fi
  fi
  if [ -e ./shotid.sys ]; then rm -f ./shotid.sys; fi
  if [ -e ./tditst.tmp ]; then rm -f ./tditst.tmp; fi
  LSAN_OPTIONS="$LSAN_OPTIONS,print_suppressions=0"

  if [ "$2" == "update" ]; then
    run "${cmd}" >"${srcdir}/$test.ans"
  else
    unset ok
    if diff --help | grep side-by-side &>/dev/null; then
      run "${cmd}" "${test}-out.log" |
        diff $DIFF_Z --side-by-side -W128 /dev/stdin ${srcdir}/$test.ans |
        expand | grep -E -C3 '^.{61} ([|>]\s|<$)' || ok=1
    else
      run "${cmd}" "${test}-out.log" |
        diff $DIFF_Z /dev/stdin ${srcdir}/$test.ans && ok=1
    fi
    echo ok=$ok
    if [ -z $ok ]; then
      echo "FAIL: $test"
      exit 1
    else
      echo "PASS: $test"
      rm -f $test-diff.log
      exit 0
    fi
  fi
fi
