#!/bin/sh

dnf check-update
dnf update -y
dnf install -y epel-release

# For RHEL 8
dnf install -y dnf-plugins-core && dnf -y config-manager --set-enabled powertools

cd /packages

echo "=== Installing all MDSplus packages ==="
dnf install -y "$@" || exit 1

echo "=== Testing ==="
. /usr/local/mdsplus/setup.sh

python3 -c 'import MDSplus; print(MDSplus.__version__)' || exit 42

mdstcl show version
# MDSplus uses weird return codes
[ $? -eq 1 ] || exit 42