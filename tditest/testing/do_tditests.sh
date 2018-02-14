#/bin/bash
#
tdir=$(realpath $(dirname ${0}))
tmpdir=$(mktemp -d)
export main_path="${tmpdir};$(realpath ${tdir}/../../trees)"
export subtree_path="${tmpdir};$(realpath ${tdir}/../../trees/subtree)"
export MDS_PATH="${tmpdir};$(realpath ${tdir}/../../tdi)"
export MDS_PYDEVICE_PATH="${tmpdir};$(realpath ${tdir}/../../pydevices)"
if [ -z "$PyLib" ]
then
  pyver="$(python -V 2>&1)"
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
if ( echo "if_error(py(1),0)" | tditest 2>/dev/null | grep -v if_error | grep 0 >/dev/null )
then
    testlist="$(cat ${tdir}/test-nopy.list)"
else
    testlist="$(cat ${tdir}/test.list)"
fi
											    
for t in ${testlist}
do
    if [ "$1" == "update" ]
    then
	tmpdir=$tmpdir tditest ${tdir}/${t}.tdi 2>&1 | \
	    grep -v 'Data inserted:' | \
	    grep -v 'Length:' > ${tdir}/${t}.ans
    else
	tmpdir=$tmpdir tditest ${tdir}/${t}.tdi 2>&1 | \
	    grep -v 'Data inserted:' | \
	    grep -v 'Length:' | \
	    diff /dev/stdin ${tdir}/${t}.ans > ${t}-diff.log
	tstat=$?
	if [ "$tstat" != "0" ]
	then
  	    echo "FAIL: ${t}"
	    status=$tstat
	else
	    echo "PASS: ${t}"
	    rm -f ${t}-diff.log
	fi
    fi
done
if [ ! -z "${tmpdir}" ]
then
  rm -Rf ${tmpdir}
fi
exit $status
