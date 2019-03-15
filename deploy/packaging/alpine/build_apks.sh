#!/bin/bash
pckdir=$(dirname $(realpath $0))
cd /workspace
signkeys="/sign_keys/mdsplus@mdsplus.org-589e05b6.rsa"
mkdir -p /workspace/.abuild
if [ -f $signkeys ]
then
  echo -e "PACKAGER_PRIVKEY=\"$signkeys\"\n" > /workspace/.abuild/abuild.conf
else
  if [ ! -f /workspace/.abuild/abuild.conf ]
  then
    touch /workspace/.abuild/abuild.conf
    abuild-keygen -aqn # generate new keys
  fi
fi
# we have to unset srcdir as it is a reserved name in abuild
# in the process of building the apks it will wipe $srcdir
srcdir= \
BUILDROOT=/workspace/releasebld/buildroot \
BRANCH=${BRANCH} \
RELEASE_VERSION=${RELEASE_VERSION} \
ARCH=${ARCH} \
DISTNAME=${DISTNAME} \
${pckdir}/alpine_build_apks.py
