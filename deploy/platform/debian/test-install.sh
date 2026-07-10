#!/bin/bash

apt update
apt upgrade -y

cd /packages

echo "=== Installing all MDSplus packages ==="
apt -f install -y "${@/#/./}" # all files must be relative paths

echo "=== Testing ==="
. /usr/local/mdsplus/setup.sh
mdstcl show version

# MDSplus uses weird return codes
[ $? -eq 1 ] || exit 42
