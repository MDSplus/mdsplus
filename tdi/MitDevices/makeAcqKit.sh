#!/bin/bash
#
# file MakeAcqKit.sh
#
# create a tgz containing the files that need to be on the d-tacq boards, mostly 
# in /usr/local/bin.
#
# also includes load.acq  which is a script that unpacks the tar file and puts its
# contents in the right places.  It should be placed in /ffs/user and invoked by 
# /ffs/user/rc.user
#
tar -czf mdsacqkit.tgz xmlfunctions.sh doInit.sh get.State sendFile acq_inetd.conf acq.ao.zero load.acq acq.ao.setvoltage
