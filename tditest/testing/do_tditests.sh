#/bin/bash
srcdir=$(readlink -f $(dirname ${0}))
if [ "$OS" == "windows" ]
then
  zdrv="Z:"
  PYTHON="wine python"
  TDITEST="wine tditest"
  DIFF_Z=-Z
else
  zdrv=""
  PYTHON=python
  TDITEST=tditest
  DIFF_Z=
fi

#if diff --help | grep side-by-side &>/dev/null
#then
#DIFF_FLAGS="--side-by-side -W128"
#else
#DIFF_FLAGS=
#fi

status=0

test=$(basename "$1")
test=${test%.tdi}

hasmitdevices() {
 for path in ${LD_LIBRARY_PATH//:/ }; do
  if [ -e $path/libMitDevices.so ]
  then return 0
  fi
 done
 return 1
}

haspython() {
 if $TDITEST <<< 'py("1")' > /dev/null
 then return 0
 else return 1
 fi
}

if [ ! -z $1 ]
then

if [ "$2" == "update" ]
then
  tmpdir=$(mktemp -d)
  trap 'if [ ! -z "${tmpdir}" ]; then rm -Rf ${tmpdir}; fi' EXIT
  export main_path="${tmpdir};$(readlink -f ${srcdir}/../../trees)"
  export subtree_path="${tmpdir};$(readlink -f ${srcdir}/../../trees/subtree)"
  export MDS_PATH="${tmpdir};$(readlink -f ${srcdir}/../../tdi)"
  export MDS_PYDEVICE_PATH="${tmpdir};$(readlink -f ${srcdir}/../../pydevices)"
  $TDITEST $zdrv$srcdir/$test.tdi 2>&1 \
   | grep -v 'Data inserted:' \
   | grep -v 'Length:' \
   > ${srcdir}/$test.ans
   if [ -e ./tditst.tmp ] ;then rm -f ./tditst.tmp; fi
else
  if [[ $test = *"py"* ]] && ! $TDITEST <<< 'py("1")' > /dev/null
  then echo no python;exit 77
  fi
  if [[ $test = *"dev"* ]]
  then
    found=0
    for path in ${LD_LIBRARY_PATH//:/ }; do
      if [ -e $path/libMitDevices.so ]
      then found=1
      fi
    done
    if [ $found == 0 ]
    then echo no libMitDevices.so;exit 77
    fi
  fi
  if [ -e ./shotid.sys ]; then rm -f ./shotid.sys; fi
  LSAN_OPTIONS="$LSAN_OPTIONS,print_suppressions=0" \
  $TDITEST $zdrv$srcdir/$test.tdi 2>&1 \
   | grep -v 'Data inserted:' \
   | grep -v 'Length:' \
   | diff $DIFF_Z $DIFF_FLAGS /dev/stdin $srcdir/$test.ans
  tstat=$?
  if [ "$tstat" != "0" ]
  then
    echo "FAIL: $test"
    status=$tstat
  else
    echo "PASS: $test"
    rm -f $test-diff.log
  fi
fi
exit $status

fi
