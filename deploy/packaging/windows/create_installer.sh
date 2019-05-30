#!/bin/bash

if [ -z ${MDSPLUS_DIR} ]
then MDSPLUS_DIR=/workspace/releasebld/buildroot
fi

abort=0
major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
release=$(echo ${RELEASE_VERSION} | cut -d. -f3)

if [ "${BRANCH}" = "stable" ]
then bname=""
else bname="-${BRANCH}"
fi

pushd ${MDSPLUS_DIR}
makensis -V4 -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DFLAVOR=${bname} -NOCD -DBRANCH=${BRANCH} \
     -DOUTDIR=/release/${BRANCH} -Dsrcdir=${srcdir} ${vs} ${srcdir}/deploy/packaging/${PLATFORM}/mdsplus.nsi
popd
if [ -d /sign_keys ]
then
    echo "Signing installer"
    if ( osslsigncode sign -certs /sign_keys/mdsplus.spc \
        -key /sign_keys/mdsplus.pvk -pass mdsplus \
        -n "MDSplus" -i http://www.mdsplus.org/ \
        -in /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}.exe \
	-out /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}-signed.exe
       )
    then
	mv -v /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}-signed.exe \
	   /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}.exe
	echo "Installer successfully signed"
    else
	echo "Failed to sign installer"
    fi
fi
