#!/bin/sh
if [ $# -ne 1 ]
then
  echo >&2 "usage: $0 board-ip-address"
  exit 0
fi
board=$1

scp $MDSPLUS_DIR/lib/acq_root_filesystem.tgz root@$board:/bigffs
scp $MDSPLUS_DIR/lib/acq_root_filesystem.tgz_ffs root@$board:/bigffs
ssh root@$board '(cd /; tar -xzf /bigffs/acq_root_filesystem.tgz_ffs)'
echo "***********************************************************************"
echo 
echo "       do not forget to set options in /ffs/user/rc.user.options       "
echo 
echo "   do not forget to turn on ftp and mdsshell in /ffs/rc.local.options  "
echo 
echo "***********************************************************************"

