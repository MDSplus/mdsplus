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
winebottle64=$(mktemp --tmpdir -d winebottle64.XXXXXXXX)
test64="64 x86_64-w64-mingw32 bin_x86_64 bin_x86_64 --with-winebottle=$winebottle64"
winebottle32=$(mktemp --tmpdir -d winebottle32.XXXXXXXX)
test32="32 i686-w64-mingw32   bin_x86    bin_x86 --with-winebottle=$winebottle32"

srcdir=$(readlink -e $(dirname ${0})/../..)

export JNI_INCLUDE_DIR=${srcdir}/3rd-party-apis/windows-jdk
export JNI_MD_INCLUDE_DIR=${srcdir}/3rd-party-apis/windows-jdk/win32


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
    if [ -z "$NOMAKE" ]; then
      HOME=$winebottle64 WINEARCH=win64 wine cmd /C ${srcdir}/mdsobjects/cpp/visual-studio-build.bat
      cp /workspace/releasebld/64/bin_x86_64/MdsObjectsCppShr-VS.dll ${MDSPLUS_DIR}/bin_x86_64/
      cp /workspace/releasebld/64/bin_x86_64/*.lib ${MDSPLUS_DIR}/bin_x86_64/
      cp /workspace/releasebld/32/bin_x86/*.lib ${MDSPLUS_DIR}/bin_x86/
    fi
    popd
    ###
    ### pack installer
    ###
  if [ -z "$NOMAKE" ]; then
    source ${srcdir}/deploy/packaging/windows/create_installer.sh
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
