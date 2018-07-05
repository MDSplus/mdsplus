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

getjava() {
    if [ "$ARCH" = "armhfxxxxx" ]; then echo "--disable-java"; fi
}

if [ -z "$host" ]
then
    host=$(gethost ${ARCH})
fi

export CFLAGS="-DASSERT_LINE_TYPE=int"
export CPPFLAGS="-DASSERT_LINE_TYPE=int"

runtests() {
    testarch ${ARCH} ${host} bin lib $(getjava ${ARCH})
    checktests
}

makelist(){
    echo
}

buildrelease() {
    ### Build release version of MDSplus and then construct installer debs
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    set -e
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR}
    mkdir -p /workspace/releasebld/${ARCH}
#    rm -Rf /workspace/releasebld/${ARCH}/*
    pushd /workspace/releasebld/${ARCH}
    config ${ARCH} ${host} bin lib $(getjava ${ARCH})
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd;
  if [ -z "$NOMAKE" ]; then
    BUILDROOT=/workspace/releasebld/buildroot \
    BRANCH=${BRANCH} \
    RELEASE_VERSION=${RELEASE_VERSION} \
    ARCH=${ARCH} \
    DISTNAME=${DISTNAME} \
    HOME=/ ${srcdir}/deploy/platform/alpine/alpine_build_apks.py
  fi
}

publish() {
    rsync -a /release/${BRANCH} /publish/
}


