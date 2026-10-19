#!/bin/bash

srcdir=$(readlink -f $(dirname ${0}))

# See testing/ports.csv
mon_port=8021
act_port=8022
dis_port=8023
if [ ! -z $TEST_PORT_OFFET ]; then
  mon_port=$((mon_port + TEST_PORT_OFFSET))
  act_port=$((act_port + TEST_PORT_OFFSET))
  dis_port=$((dis_port + TEST_PORT_OFFSET))
fi

export MONITOR_SERVER=localhost:$mon_port
export ACTION_SERVER=localhost:$act_port
export DISPATCH_SERVER=localhost:$dis_port


if [ "$OS" == "windows" ]; then
  MDSIP="wine mdsip"
  MDSTCL="wine mdstcl"
  ACTLOG="wine actlog"
  DIFF_Z="-Z"
else
  MDSIP="mdsip"
  MDSTCL="mdstcl"
  ACTLOG="actlog"
  DIFF_Z=""
fi

export distest_path=$PWD

mkdir -p $distest_path

$MDSIP -p ${mon_port} -s -h $MDSPLUS_DIR/testing/mdsip.hosts > server_mon.log 2> server_mon.err & # Monitor server
mon_pid=$!
$MDSIP -p ${act_port} -s -h $MDSPLUS_DIR/testing/mdsip.hosts > server_act.log 2> server_act.err & # Action server
act_pid=$!
$MDSIP -p ${dis_port} -s -h $MDSPLUS_DIR/testing/mdsip.hosts > server_dis.log 2> server_dis.err & # Dispatch server
dis_pid=$!
$ACTLOG -monitor ${MONITOR_SERVER} > actlog.log 2> actlog.err &
log_pid=$!

$MDSTCL @${srcdir}/build_tree.tcl

$MDSTCL @${srcdir}/test_action.tcl

kill -SIGKILL $mon_pid $act_pid $dis_pid $log_pid

# Cut time info
cat actlog.log | cut -d' ' -f2- > actlog_cut.log

# Check output of actlog against reference
if diff ${DIFF_Z} actlog_cut.log ${srcdir}/actlog.ans; then
  echo "PASS: actlog"
  exit 0
else
  echo "FAIL: actlog"
  exit 1
fi
