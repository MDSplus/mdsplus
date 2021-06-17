#!/bin/bash
[ -z "${BUILDROOT}" ] && exit 1
pckdir=$(dirname $(realpath $0))
export HOME=/workspace
cd /workspace
signkeys="/sign_keys/mdsplus@mdsplus.org-589e05b6.rsa"
mkdir -p /workspace/.abuild
if [ -f $signkeys ]; then
  echo -e "PACKAGER_PRIVKEY=\"$signkeys\"\n" >/workspace/.abuild/abuild.conf
elif [ ! -f /workspace/.abuild/abuild.conf ]; then
  touch /workspace/.abuild/abuild.conf
  abuild-keygen -aqn # generate new keys
fi
# we have to unset srcdir as it is a reserved name in abuild
# in the process of building the apks it will wipe $srcdir
srcdir= \
  BUILDROOT=${BUILDROOT}\
  ${pckdir}/alpine_build_apks.py
