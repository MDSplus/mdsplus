#!/bin/bash
#
# windows_docker_build.sh - build windows installer
#
# release:
# /release/$branch/MDSplus-*.exe
#
# publish:
# /publish/$branch/MDSplus-*.exe
#
test64="64 x86_64-w64-mingw32 bin_x86_64 bin_x86_64 --with-winebottle=/tmp/winebottle-64"
test32="32 i686-w64-mingw32   bin_x86    bin_x86 --with-winebottle=/tmp/winebottle-32"
export JNI_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk
export JNI_MD_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk/win32
buildrelease() {
    abort=0
    ### Clean up workspace
    rm -Rf /workspace/releasebld
    ### Build release version of MDSplus and then construct installer rpms
    set -e
    MDSPLUS_DIR=/workspace/releasebld/buildroot
    mkdir -p ${MDSPLUS_DIR};
    mkdir -p /workspace/releasebld/64;
    pushd /workspace/releasebld/64;
    config ${test64}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd;
    mkdir -p /workspace/releasebld/32;
    pushd /workspace/releasebld/32;
    config ${test32}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd
    ###
    ### pack installer
    ###
  if [ -z "$NOMAKE" ]; then
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
        bname=""
    else
        bname="-${BRANCH}"
    fi
    if [ ! -z "$WINHOST" -a -r /winbld -a ! -z "$WINREMBLD" ]
    then
        windows_cleanup() {
            rm -Rf /winbld/${tmpdir}
        }
        pushd /winbld
        tmpdir=$(mktemp -d mdsplus-XXXXXXXXXX)
        trap windows_cleanup EXIT
        topsrcdir=${WINREMBLD}/${tmpdir}
        cd ${tmpdir}
        rsync -am --include="*/" --include="*.h*" --include="*.def" --exclude="*" /source/ ./
        rsync -am /workspace/releasebld/64/include/config.h ./include/
        rsync -a /source/mdsobjects/cpp /source/mdsobjects/MdsObjects* /source/mdsobjects/VS-* ./mdsobjects/
        rsync -a /source/deploy/platform/windows/winbld.bat ./deploy/
        rsync -a ${MDSPLUS_DIR}/bin_* ./
        curl http://${WINHOST}:8080${topsrcdir}/deploy/winbld.bat
        # see if files are there
        :&& ls /winbld/${tmpdir}/bin_x86*/*.lib /winbld/${tmpdir}/bin_x86*/MdsObjectsCppShr-VS.* > /dev/null
        checkstatus abort "Failure: Problem building Visual Studio dll." $?
        rsync -av --include="*/" --include="*.lib" --include="MdsObjectsCppShr-VS.dll" --exclude="*" /winbld/${tmpdir}/bin_x86* ${MDSPLUS_DIR}/
        vs="-DVisualStudio"
        popd
    fi
    pushd $MDSPLUS_DIR
    makensis -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DFLAVOR=${bname} -NOCD \
         -DOUTDIR=/release/${BRANCH} ${vs} /source/deploy/packaging/${PLATFORM}/mdsplus.nsi
    popd
    if [ -d /sign_keys ]
    then
        for timestamp_server in http://timestamp.comodoca.com/authenticode http://timestamp.verisign.com/scripts/timestamp.dll http://timestamp.globalsign.com/scripts/timestamp.dll http://tsa.starfieldtech.com
        do
            if ( signcode -spc /sign_keys/mdsplus.spc \
                  -v /sign_keys/mdsplus.pvk \
                  -a sha1 \
                  -$ individual \
                  -n MDSplus  \
                  -i http://www.mdsplus.org/ \
                  -t http://timestamp.verisign.com/scripts/timestamp.dll \
                  -tr 10 /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}.exe <<EOF
mdsplus
EOF
               )
           then
               break
           fi
        done
    fi
  fi # NOMAKE
}
publish() {
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
        bname=""
    else
        bname="-${BRANCH}"
    fi
    rsync -a /release/${BRANCH}/MDSplus${bname}-${major}.${minor}-${release}.exe /publish/${BRANCH}
}
