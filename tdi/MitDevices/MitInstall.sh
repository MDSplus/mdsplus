#!/bin/sh
rm -f /bigffs/*
rm -f /ffs/user/*
cd /bigffs/
tar -xzf /tmp/bigffs.tgz 
cd /ffs/user
tar -xzf /bigffs/ffs.user.tgz 
