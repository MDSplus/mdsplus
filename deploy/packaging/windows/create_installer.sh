#!/bin/bash
abort=0
major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
release=$(echo ${RELEASE_VERSION} | cut -d. -f3)

if [ "${BRANCH}" = "stable" ]
then bname=""
else bname="-${BRANCH}"
fi

#if [ ! -z "$WINHOST" -a -r /winbld -a ! -z "$WINREMBLD" ]
#then
#  windows_cleanup() {
#      rm -Rf /winbld/${tmpdir}
#  }
#  pushd /winbld
#  tmpdir=$(mktemp -d mdsplus-XXXXXXXXXX)
#  trap windows_cleanup EXIT
#  topsrcdir=${WINREMBLD}/${tmpdir}
#  cd ${tmpdir}
#  rsync -am --include="*/" --include="*.h" --include="*.hpp" --include="*.def" --exclude="*" ${srcdir}/ ./
#  rsync -am /workspace/releasebld/64/include/mdsplus/mdsconfig.h ./include/
#  rsync -a ${srcdir}/mdsobjects/cpp ${srcdir}/mdsobjects/MdsObjects* ${srcdir}/mdsobjects/VS-* ./mdsobjects/
#  rsync -a ${srcdir}/deploy/platform/windows/winbld.bat ./deploy/
#  rsync -a ${MDSPLUS_DIR}/bin_* ./
#  curl http://${WINHOST}:8080${topsrcdir}/deploy/winbld.bat
  # see if files are there
#  :&& ls /winbld/${tmpdir}/bin_x86*/*.lib /winbld/${tmpdir}/bin_x86*/MdsObjectsCppShr-VS.* > /dev/null
#  checkstatus abort "Failure: Problem building Visual Studio dll." $?
#  rsync -av --include="*/" --include="*.lib" --include="MdsObjectsCppShr-VS.dll" --exclude="*" /winbld/${tmpdir}/bin_x86* ${MDSPLUS_DIR}/
#  vs="-DVisualStudio"
#  popd
#fi

pushd ${MDSPLUS_DIR}
makensis -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DFLAVOR=${bname} -NOCD -DBRANCH=${BRANCH} \
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
