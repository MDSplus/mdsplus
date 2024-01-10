#!/bin/bash
#
# Check ROAM for access to a resource
#
# example: ./roam_check_access "CMOD Data" "Write" "/DC=org/DC=doegrids/OU=People/CN=Thomas W. Fredian 790562"
#
# returns $?=0 if successful and $?=1 if not
# if successful any resource permision aux info will be written to stdout
#
ans=(`echo check_access\(\"${1}\",\"${2}\",\"${3}\",_aux\) \? \(write\(*,_aux\),0\) \: -1 | tditest | grep -v check_access`)
if [ ${#ans[*]} == 1 ]
then
  if [ ${ans[0]} == 0 ]
  then 
    exit 0;
  else
    exit 1;
  fi
else
  let i=0;
  let j=${#ans[*]}-2
  while [ $i -lt $j ]; do echo -n ${ans[$i]}" " ; let i=i+1; done
  echo ${ans[$j]}
  exit 0;
fi
