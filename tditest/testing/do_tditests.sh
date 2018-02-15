#/bin/bash
tdir=$(realpath $(dirname ${0}))
tmpdir=$(mktemp -d)
export main_path="${tmpdir};$(realpath ${tdir}/../../trees)"
export subtree_path="${tmpdir};$(realpath ${tdir}/../../trees/subtree)"
export MDS_PATH="${tmpdir};$(realpath ${tdir}/../../tdi)"
export MDS_PYDEVICE_PATH="${tmpdir};$(realpath ${tdir}/../../pydevices)"
if [ "$OS" == "windows" ]
then
  zdrv="Z:"
  PYTHON="wine python"
  TDITEST="wine tditest"
else
  zdrv=""
  PYTHON=python
  TDITEST=tditest
fi


if [ -z "$PyLib" ]
then
  pyver="$($PYTHON -V 2>&1)"
  if [ $? = 0 -a "$pyver" != "" ]
  then
    PyLib=$(echo $pyver | awk '{print $2}' 2>/dev/null | awk -F. '{print "python"$1"."$2}' 2>/dev/null)
    if [ $? = 0 ]
    then
      export PyLib
    fi
  fi
fi
status=0

test=$(basename "$1")
test=${test%.tdi}

if [ "$2" == "update" ]
then
  tmpdir=$zdrv$tmpdir $TDITEST $zdrv$1 2>&1 | \
   grep -v 'Data inserted:' | \
   grep -v 'Length:' |
   sed $'s/\r$//' > ${tdir}/$test.ans
else
  if [ "$test" != "test-devices" ]
  then
    if ( ! $0 $tdir/test-devices.tdi )
    then
      if [ -r $tdir/${test}-nodevices.tdi ]
      then
        test=${test}-nodevices
      fi
    fi
  fi
  tmpdir=$zdrv$tmpdir $TDITEST $zdrv$tdir/$test.tdi 2>&1 | \
    grep -v 'Data inserted:' | \
    grep -v 'Length:' | \
    sed $'s/\r$//' | \
    diff /dev/stdin $tdir/$test.ans > $test-diff.log
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
if [ ! -z "${tmpdir}" ]
then
  rm -Rf ${tmpdir}
fi
exit $status
