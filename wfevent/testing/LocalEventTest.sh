#!/bin/bash

status=0

srcdir=$(readlink -f $(dirname ${0}))

event_name=test_event
timeout=20
sleepdur=5

if [ "$OS" == "windows" ]; then
  WFEVENT="wine wfevent"
  SETEVENT="wine setevent"
  OPTTIMEOUT="/t:$timeout"
  OPTDATA="/d"
  DIFF_Z="-Z"
else
  WFEVENT="wfevent"
  SETEVENT="setevent"
  OPTTIMEOUT="--timeout=$timeout"
  OPTDATA="--data"
  DIFF_Z=""
fi

### TEST 1: local event no data

name=wfevent

$WFEVENT $event_name $OPTTIMEOUT > $name.log 2> $name.err &
pid=$!
sleep $sleepdur
$SETEVENT $event_name

# Make sure wfevent is terminated
if [ ! kill -0 $pid 2> /dev/null ]; then
  sleep $timeout
  if [ ! kill -0 $pid 2> /dev/null ]; then
    kill -SIGKILL $pid
  fi
fi

# Check output of actlog against reference
if diff $DIFF_Z $name.log ${srcdir}/$name.ans; then
  echo "PASS: $name"
else
  echo "FAIL: $name"
  status=1
fi

### TEST 2: local event with data

name=wfeventData

$WFEVENT $event_name $OPTDATA $OPTTIMEOUT > $name.log 2> $name.err &
pid=$!
sleep $sleepdur
$SETEVENT $event_name "hello"

# Make sure wfevent is terminated
if [ ! kill -0 $pid 2> /dev/null ]; then
  sleep $timeout
  if [ ! kill -0 $pid 2> /dev/null ]; then
    kill -SIGKILL $pid
  fi
fi

# Check output of actlog against reference
if diff $DIFF_Z $name.log ${srcdir}/$name.ans; then
  echo "PASS: $name"
else
  echo "FAIL: $name"
  status=1
fi

# Return
exit $status
