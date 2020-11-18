
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
    ## this will move new files into publish and update APKINDEX.tar.gz
    # if NEW.tar.gz exists or if an old APKINDEX.tar.gz does not yet exist,
    # it will create a new repository (takes longer, exspecially on a remote fs)
    R=/release/${BRANCH}
    P=/publish/${BRANCH}
    mkdir -p $P/${ARCH} $P/noarch
    cd $P
    rsync -a $R/${ARCH}/*.apk $P/${ARCH}/ &&:
    checkstatus abort "Failure: Problem copying arch apks to publish area!" $?
    rsync -a $R/noarch/*.apk  $P/noarch/ &&:
    checkstatus abort "Failure: Problem copying noarch apks to publish area!" $?
    if [ -r $P/${ARCH}/APKINDEX.tar.gz -a ! -r $P/${ARCH}/NEW.tar.gz ]
    then apk index -x $P/${ARCH}/APKINDEX.tar.gz -o $P/${ARCH}/NEW.tar.gz $P/${ARCH}/*.apk noarch/*.apk
    else apk index                               -o $P/${ARCH}/NEW.tar.gz $P/${ARCH}/*.apk noarch/*.apk
    fi
    checkstatus abort "Failure: Problem updating apk repository in publish!" $?
    abuild-sign -k /sign_keys/mdsplus@mdsplus.org-589e05b6.rsa $P/${ARCH}/NEW.tar.gz
    checkstatus abort "Failure: Problem signing apk repository in publish!" $?
    mv -f $P/${ARCH}/NEW.tar.gz $P/${ARCH}/APKINDEX.tar.gz
}
