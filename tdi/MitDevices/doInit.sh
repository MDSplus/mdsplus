#!/bin/sh
read initialize
. /etc/profile
killall initialize
killall postshot.sh
killall set.Arm
while (killall acqcmd 2>/dev/null); do : ;done
if [ -x /tmp/AOInitialize ]; then
  /tmp/AOInitialize > /tmp/AOinitlog.log 2>&1
  rm -f /tmp/AOInitialize
fi
chmod a+x /tmp/initialize
/tmp/initialize > /tmp/initlog.log 2>&1 
sleep 1
set.Arm

