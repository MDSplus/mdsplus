#!/bin/bash

status=0

srcdir=$(readlink -f $(dirname ${0}))

event_name=test_event
timeout=20
sleepdur=5

if [ "$OS" == "windows" ]; then
  WFEVENT="wine wfevent"
  SETEVENT="wine setevent"
  MDSIP="wine mdsip"
  OPTTIMEOUT="/t:$timeout"
  OPTDATA="/d"
  DIFF_Z="-Z"
else
  WFEVENT="wfevent"
  SETEVENT="setevent"
  MDSIP="mdsip"
  OPTTIMEOUT="--timeout=$timeout"
  OPTDATA="--data"
  DIFF_Z=""
fi

# See testing/ports.csv
port=8020
if [ ! -z $TEST_PORT_OFFET ]; then
  port=$((port + TEST_PORT_OFFSET))
fi

# Start event server

$MDSIP --multi --protocol=tcp --port=$port --hostfile=$MDSPLUS_DIR/testing/mdsip.hosts > mdsip.log 2> mdsip.err &
pidmdsip=$!

sleep $sleepdur

### TEST 1: remote event no data

name=wfeventRemote

mds_event_server=localhost:$port $WFEVENT $event_name $OPTTIMEOUT > $name.log 2> $name.err &
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

### TEST 2: remote event with data

name=wfeventRemoteData

mds_event_server=localhost:$port $WFEVENT $event_name $OPTDATA $OPTTIMEOUT > $name.log 2> $name.err &
pid=$!
sleep $sleepdur
$SETEVENT $event_name "hello1world2" # Data from remote event is always 12 byte long

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

# Cleanup
kill -SIGKILL $pidmdsip

# Return
exit $status
