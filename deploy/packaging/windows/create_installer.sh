#!/bin/bash

if [ -z "${MDSPLUS_DIR}" ]; then
  MDSPLUS_DIR=${BUILDROOT}/usr/local/mdsplus
fi

abort=0
major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
release=$(echo ${RELEASE_VERSION} | cut -d. -f3)

mkdir -p ${DISTROOT}/${PLATFORM}/${FLAVOR}

pushd ${MDSPLUS_DIR}
makensis -V4 -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DBNAME=${BNAME} -NOCD -DBRANCH=${BRANCH} -DARCH=${ARCH} \
  -DINCLUDE=${srcdir}/deploy/packaging/${PLATFORM} \
  -DOUTDIR=${DISTROOT}/${PLATFORM}/${FLAVOR} -Dsrcdir=${srcdir} ${vs} ${srcdir}/deploy/packaging/${PLATFORM}/mdsplus.nsi
popd
if [ -d /sign_keys ]; then
  echo "Signing installer"
  if (
    osslsigncode sign -certs /sign_keys/mdsplus.spc \
      -key /sign_keys/mdsplus.pvk -pass mdsplus \
      -n "MDSplus" -i http://www.mdsplus.org/ \
      -in /release/${FLAVOR}/MDSplus${BNAME}-${major}.${minor}-${release}.exe \
      -out /release/${FLAVOR}/MDSplus${BNAME}-${major}.${minor}-${release}-signed.exe
  ); then
    mv -v /release/${FLAVOR}/MDSplus${BNAME}-${major}.${minor}-${release}-signed.exe \
      /release/${FLAVOR}/MDSplus${BNAME}-${major}.${minor}-${release}.exe
    echo "Installer successfully signed"
  else
    echo "Failed to sign installer"
  fi
fi
