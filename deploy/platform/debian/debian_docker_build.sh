#!/bin/bash
#
# debian_docker_build is used to build, test, package and add deb's to a
# repository for debian based systems.
#
# release:
# /release/repo   -> repository
# /release/$branch/DEBS/$arch/*.deb
#
# publish:
# /publish/repo   -> repository
# /publish/$branch/DEBS/$arch/*.deb
#

srcdir=$(readlink -e $(dirname ${0})/../..)

# configure based on ARCH
case "${ARCH}" in
  "amd64")
    host=x86_64-linux
    gsi_param="--with-gsi=/usr:gcc64"
    bits=64
    ;;
  "armhf")
    host=arm-linux-gnueabihf
    gsi_param="--with-gsi=/usr:gcc32"
    bits=32
    ;;
  *)
    host=i686-linux
    gsi_param="--with-gsi=/usr:gcc32"
    bits=32
    ;;
esac

if [[ "$OS" == "debian_wheezy" ]]
then
  export JDK_DIR=/usr/lib/jvm/java-7-openjdk-${ARCH}
fi
config_param="${bits} ${host} bin lib ${gsi_param}"
runtests() {
  testarch ${config_param}
  checktests
}
makelist(){
    dpkg -c $1 | \
    grep -v python/dist | \
    grep -v python/build | \
    grep -v python/doc | \
    grep -v egg-info | \
    grep -v '/$' | \
    awk '{for (i=6; i<NF; i++) printf $i " "; print $NF}' | \
    awk '{split($0,a," -> "); print a[1]}' | \
    sort
}
debtopkg() {
    if ( echo $1 | grep mdsplus${2}_${3} >/dev/null )
    then
        echo ""
    else
        echo ${1:8+${#2}:-${#3}-${#4}-6}
    fi
}
buildrelease() {
    ### Build release version of MDSplus and then construct installer debs
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    set -e
    mkdir -p /release/${BRANCH}/DEBS/${ARCH}
    rm -Rf /workspace/releasebld/* /release/${BRANCH}/DEBS/${ARCH}/*
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR};
    mkdir -p /workspace/releasebld/${bits};
    pushd /workspace/releasebld/${bits};
    config ${config_param}
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
	     PLATFORM=${PLATFORM} \
	     ${srcdir}/deploy/platform/debian/debian_build_debs.py
    baddeb=0
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*\.deb")
    do
	pkg=$(debtopkg $(basename $deb) "${BNAME}" ${RELEASE_VERSION} ${ARCH})
	if [ "${#pkg}" = "0" ]
	then
	   continue
	fi
        if [[ x${pkg} == x*_bin ]]
        then
          checkfile=${srcdir}/deploy/packaging/${PLATFORM}/$pkg.$ARCH
        else
          checkfile=${srcdir}/deploy/packaging/${PLATFORM}/$pkg.noarch
        fi
	if [ "$UPDATEPKG" = "yes" ]
	then
	    mkdir -p ${srcdir}/deploy/packaging/${PLATFORM}
	    makelist $deb > ${checkfile}
	else
	    set +e
	    echo "Checking contents of $(basename $deb)"
	    if ( diff <(makelist $deb) <(sort ${checkfile}) )
	    then
		echo "Contents of $(basename $deb) is correct."
	    else
                checkstatus baddeb "Failure: Problem with contents of $(basename $deb)" 1
	    fi
	    set -e
	fi
    done
    checkstatus abort "Failure: Problem with contents of one or more debs. (see above)" $baddeb
  if [ -z "$abort" ] || [ "$abort" = "0" ]
  then
    echo "Building repo";
    mkdir -p /release/repo/conf
    mkdir -p /release/repo/db
    mkdir -p /release/repo/dists
    mkdir -p /release/repo/pool
    if [ "${BRANCH}" = "alpha" -o "${BRANCH}" = "stable" ]
    then
        component=""
    else
        component=" ${BRANCH}"
    fi
    cat - <<EOF > /release/repo/conf/distributions
Origin: MDSplus Development Team
Label: MDSplus
Codename: MDSplus
Architectures: ${ARCHES}
Components: alpha stable${component}
Description: MDSplus packages
EOF
    GPG_HOME=""
    if [ -d /sign_keys/${OS}/.gnupg ]
    then
	GPG_HOME="/sign_keys/${OS}"
    elif [ -d /sign_keys/.gnupg ]
    then
	GPG_HOME="/sign_keys"
    fi
    if [ ! -z "$GPG_HOME" ]
    then
    	echo "SignWith: MDSplus" >> /release/repo/conf/distributions
	rsync -a ${GPG_HOME}/.gnupg /tmp
    fi
    pushd /release/repo
    reprepro clearvanished
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*${major}\.${minor}\.${release}_*")
    do
        if [ -z "$abort" ] || [ "$abort" = "0" ]
        then
            :&& HOME=/tmp reprepro -V -C ${BRANCH} includedeb MDSplus $deb
            checkstatus abort "Failure: Problem installing $deb into repository." $?
        else
          break
        fi
    done
    popd
  fi #abort
  fi #nomake
}
publish() {
    ### DO NOT CLEAN /publish as it may contain valid older release packages
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    mkdir -p /publish/${BRANCH}/DEBS
    rsync -a /release/${BRANCH}/DEBS/${ARCH} /publish/${BRANCH}/DEBS/
    if [ ! -r /publish/repo ]
    then
        :&& rsync -a /release/repo /publish/
	checkstatus abort "Failure: Problem copying repo into publish area." $?
    else
	pushd /publish/repo
	reprepro clearvanished
	:&& env HOME=/sign_keys reprepro -V --keepunused --keepunreferenced -C ${BRANCH} includedeb MDSplus ../${BRANCH}/DEBS/${ARCH}/*${major}\.${minor}\.${release}_*
       	checkstatus abort "Failure: Problem installing debian into publish repository." $?
	popd
    fi
}
