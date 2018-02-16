#/bin/bash
srcdir=$(dirname $(realpath $0))
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
if diff --help | grep side-by-side && diff --help | grep suppress-common-lines
then
DIFF_SIDE_BY_SIDE=--side-by-side --suppress-common-lines
else
DIFF_SIDE_BY_SIDE=
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

if [ ! -z $1 ]
then

if [ "$2" == "update" ]
then
  $TDITEST $zdrv$1 2>&1 \
   | grep -v 'Data inserted:' \
   | grep -v 'Length:' \
   > ${srcdir}/$test.ans
else
  if [ "$test" != "test-devices" ]
  then
    if ( ! $0 $srcdir/test-devices.tdi )
    then
      if [ -r $srcdir/${test}-nodevices.tdi ]
      then
        test=${test}-nodevices
      fi
    fi
  fi
  $TDITEST $zdrv$srcdir/$test.tdi 2>&1 \
   | grep -v 'Data inserted:' \
   | grep -v 'Length:' \
   | diff $DIFF_Z $DIFF_SIDE_BY_SIDE /dev/stdin $srcdir/$test.ans > $test-diff.log
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
