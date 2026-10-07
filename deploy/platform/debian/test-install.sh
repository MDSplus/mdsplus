#!/bin/bash

apt update
apt upgrade -y

cd /packages

echo "=== Installing all MDSplus packages ==="
# all files must be relative paths
DEBIAN_FRONTEND=noninteractive \
    apt -f install -yf "${@/#/./}" \
    --option=Dpkg::Options::=--force-confdef

echo "=== Testing ==="
. /usr/local/mdsplus/setup.sh

python3 -c 'import MDSplus; print(MDSplus.__version__)' || exit 42

mdstcl show version
# MDSplus uses weird return codes
[ $? -eq 1 ] || exit 42
