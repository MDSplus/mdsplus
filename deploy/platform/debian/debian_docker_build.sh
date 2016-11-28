#!/bin/bash
#
# debian_docker_build is used to build, test, package and add deb's to a
# repository for debian based systems.
# 
# publish:
# /publish/repo   -> repository
# /publish/$branch/DEBS/$arch/*.deb
#
if [ "$ARCH" = "amd64" ]
then
    if [ -z "$host" ]
    then
	host=x86_64-linux
    fi
else
    if [ -z "$host" ]
    then
	host=i686-linux
    fi
fi
test64(){
    testarch 64 x86_64-linux bin lib --with-gsi=/usr:gcc64
}
test32(){
    testarch 32 i686-linux   bin lib --with-gsi=/usr:gcc32
}
runtests() {
  if [ "${ARCH}" = "amd64" ]
  then
      test64
  else
      test32
  fi
  checktests
}
makelist(){
    dpkg -c $1 | \
    grep -v python/dist | \
    grep -v python/build | \
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
    if [ "${ARCH}" = "amd64" ]
    then
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/64;
	pushd /workspace/releasebld/64;
	config 64 x86_64-linux bin lib --with-gsi=/usr:gcc64
	$MAKE
	$MAKE install
	popd;
    else
	MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
	mkdir -p ${MDSPLUS_DIR};
	mkdir -p /workspace/releasebld/32;
	pushd /workspace/releasebld/32;
	config 32 i686-linux bin lib --with-gsi=/usr:gcc64
	$MAKE
	$MAKE install
	popd
    fi
    BUILDROOT=/workspace/releasebld/buildroot \
	     BRANCH=${BRANCH} \
	     RELEASE_VERSION=${RELEASE_VERSION} \
	     ARCH=${ARCH} \
	     DISTNAME=${DISTNAME} \
	     PLATFORM=${PLATFORM} \
	     /source/deploy/platform/debian/debian_build_debs.py
    baddeb=0
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*\.deb")
    do
	pkg=$(debtopkg $(basename $deb) "${BNAME}" ${RELEASE_VERSION} ${ARCH})
	if [ "${#pkg}" = "0" ]
	then
	   continue
	fi
	checkfile=/source/deploy/packaging/${PLATFORM}/$pkg.${ARCH}
	if [ "$UPDATEPKG" = "yes" ]
	then
	    mkdir -p /source/deploy/packaging/${PLATFORM}
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
    arches=$(spacedelim ${ARCHES})
    cat - <<EOF > /release/repo/conf/distributions
Origin: MDSplus Development Team
Label: MDSplus
Codename: MDSplus
Architectures: ${arches}
Components: alpha stable${component}
Description: MDSplus packages
EOF
    if [ -d /sign_keys/.gnupg ]
    then
        ls -l /sign_keys/.gnupg
    	echo "SignWith: MDSplus" >> /release/repo/conf/distributions
    fi
    export HOME=/sign_keys
    pushd /release/repo
    reprepro clearvanished
    for deb in $(find /release/${BRANCH}/DEBS/${ARCH} -name "*${major}\.${minor}\.${release}_*")
    do
        if [ -z "$abort" ] || [ "$abort" = "0" ]
        then
            :&& reprepro -V -C ${BRANCH} includedeb MDSplus $deb
            checkstatus abort "Failure: Problem installing $deb into repository." $?
        fi
    done
    popd
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

