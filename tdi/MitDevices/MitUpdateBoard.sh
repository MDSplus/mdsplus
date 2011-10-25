#!/bin/bash
scp bigffs.tgz root@$1:/tmp
scp MitInstall.sh root@$1:/tmp
ssh root@$1 /tmp/MitInstall.sh
ssh root@$1 ls -l /bigffs /ffs/user
