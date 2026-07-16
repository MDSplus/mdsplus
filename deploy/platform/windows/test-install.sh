#!/bin/sh

apt update
apt upgrade -y
apt install -y wine

cd /packages

wine "$1" /S /AllUsers
wine mdstcl.bat show version

# MDSplus uses weird return codes
[ $? -eq 1 ] || exit 42
