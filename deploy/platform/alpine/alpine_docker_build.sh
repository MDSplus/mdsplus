
#!/bin/bash
#
# alphine_docker_build is used to build, test, package and add apk's to a
# repository for alpine based systems.
#

testx86_64="64 x86_64-linux bin lib"
testx86="32 i686-linux   bin lib --with-gsi=/usr:gcc32"
testarmhf="arm armv6-alpine-linux-muslgnueabihf bin lib"
srcdir=$(readlink -f $(dirname ${0})/../..)

gethost() {
    case $1 in
	x86) echo i686-linux;;
	x86_64) echo x86_64-linux;;
	armhf) echo armv6-alpine-linux-muslgnueabihf;;
    esac
}

if [ -z "$host" ]
then
    host=$(gethost ${ARCH})
fi

export CFLAGS="-DASSERT_LINE_TYPE=int"
export CPPFLAGS="-DASSERT_LINE_TYPE=int"

runtests() {
    testarch ${ARCH} ${host} bin lib
    checktests
}

makelist(){
    echo
}

buildrelease() {
    ### Build release version of MDSplus and then construct installer debs
    set -e
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR}
    mkdir -p /workspace/releasebld/${ARCH}
    rm -Rf /workspace/releasebld/${ARCH}/*
    pushd /workspace/releasebld/${ARCH}
    config ${ARCH} ${host} bin lib ${ALPHA_DEBUG_INFO}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd;
  if [ -z "$NOMAKE" ]
  then
    rm -Rf /release/${BRANCH}/${ARCH}
    mkdir -p /release/${BRANCH}/${ARCH}
    ${srcdir}/deploy/packaging/alpine/build_apks.sh
  fi
}

publish() {
    ### DO NOT CLEAN /publish as it may contain valid older release apks
    SIGNKEYS=/sign_keys/mdsplus@mdsplus.org-589e05b6.rsa
    REPO_DIR=/release/${BRANCH}/${ARCH}
    :&& rsync -a /release/${BRANCH}/${REPO_DIR}/*.apk ${REPO_DIR}/
    checkstatus abort "Failure: Problem copying release rpms to publish area!" $?
    [ -r /sign_keys/mdsplus@mdsplus.org-589e05b6.rsa ]
    checkstatus abort "Failure: Problem finding signkeys for apk repository!" $?
    if [ -r ${REPO_DIR}/APKINDEX.tar.gz ]
    then mv ${REPO_DIR}/APKINDEX.tar.gz ${REPO_DIR}/APKINDEX.tar.gz.old
    fi
    abuild_failure=0
    (
      apk index -o ${REPO_DIR}/APKINDEX.tar.gz /publish/${BRANCH}/${ARCH}/*.apk
      abuild-sign -k ${SIGNKEYS} ${REPO_DIR}/APKINDEX.tar.gz
    ) || (
      abuild_failure=1
      mv ${REPO_DIR}/APKINDEX.tar.gz.old ${REPO_DIR}/APKINDEX.tar.gz
    )
    checkstatus abort "Failure: Problem updating apk repository in publish!" $abuild_failure
}
