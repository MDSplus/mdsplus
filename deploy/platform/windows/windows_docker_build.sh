#!/bin/bash
#
# windows_docker_build.sh - build windows installer
#
# release:
# /release/$flavor/MDSplus-*.exe
#
# publish:
# /publish/$flavor/MDSplus-*.exe
#

srcdir=$(readlink -e $(dirname ${0})/../..)

export JNI_INCLUDE_DIR=${srcdir}/3rd-party-apis/windows-jdk
export JNI_MD_INCLUDE_DIR=${srcdir}/3rd-party-apis/windows-jdk/win32
mkdir -p /workspace/winebottle64
test64="64 x86_64-w64-mingw32 bin_x86_64 bin_x86_64 --with-winebottle=/workspace/winebottle64"
mkdir -p /workspace/winebottle32
test32="32 i686-w64-mingw32   bin_x86    bin_x86    --with-winebottle=/workspace/winebottle32"

runtests() {
    # run tests with the platform specific params read from test32 and test64
    testarch ${test64}
    testarch ${test32};
    checktests;
}

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
    config ${test64} ${CONFIGURE_EXTRA}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd;
    mkdir -p /workspace/releasebld/32;
    pushd /workspace/releasebld/32;
    config ${test32} ${CONFIGURE_EXTRA}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd
    if [ -z "$NOMAKE" ]; then
      pushd /workspace/releasebld/32/mdsobjects/cpp
      $MAKE generate-libs-from-dlls
      popd
      pushd /workspace/releasebld/64/mdsobjects/cpp
      $MAKE generate-libs-from-dlls
      HOME=/workspace/winebottle64 WINEARCH=win64\
	wine cmd /C ${srcdir}/deploy/platform/windows/visual-studio-build.bat
      cp /workspace/releasebld/64/bin_x86_64/MdsObjectsCppShr-VS.dll ${MDSPLUS_DIR}/bin_x86_64/
      cp /workspace/releasebld/64/bin_x86_64/*.lib ${MDSPLUS_DIR}/bin_x86_64/
      cp /workspace/releasebld/32/bin_x86/*.lib ${MDSPLUS_DIR}/bin_x86/
      cp -r ${srcdir}/icons ${MDSPLUS_DIR}/
      popd
    fi
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
    rsync -a /release/${FLAVOR}/MDSplus${BNAME}-${major}.${minor}-${release}.exe /publish/${FLAVOR}
}
