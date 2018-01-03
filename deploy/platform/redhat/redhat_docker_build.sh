#!/bin/bash
#
# redhat_docker_build.sh is used to build, test, package and add rpms's to a
# repository for redhat based systems.
#
# release:
# /release/$branch/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
# /release/$branch/RPMS/$arch/*.rpm
#
# publish:
# /publish/$branch/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
# /publish/$branch/RPMS/$arch/*.rpm
# /publish/$branch/cache/$arch/*.rpm-*
#

srcdir=$(readlink -e $(dirname ${0})/../..)

test64="64 x86_64-linux bin64 lib64 --with-gsi=/usr:gcc64"
test32="32 i686-linux   bin32 lib32 --with-gsi=/usr:gcc32 --with-valgrind-lib=/usr/lib64/valgrind"
makelist(){
    rpm2cpio $1 | \
        cpio --list --quiet | \
        grep -v python/dist | \
        grep -v python/build | \
        grep -v egg-info | \
	grep -v '\.build\-id' | \
        sort
}
buildrelease(){
    ###
    ### Clean up workspace
    ###
    rm -Rf /workspace/releasebld
    ###
    ### Build release version of MDSplus and then construct installer rpms
    ###
    set -e
    MDSPLUS_DIR=/workspace/releasebld/buildroot/usr/local/mdsplus
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
  if [ -z "$NOMAKE" ]; then
    BUILDROOT=/workspace/releasebld/buildroot
    echo "Building rpms";
    ###
    ### Setup repository rpm info
    ###
    mkdir -p ${BUILDROOT}/etc/yum.repos.d;
    mkdir -p ${BUILDROOT}/etc/pki/rpm-gpg/;
    cp ${srcdir}/deploy/platform/redhat/RPM-GPG-KEY-MDSplus ${BUILDROOT}/etc/pki/rpm-gpg/;
    if [ -d /sign_keys/.gnupg ]
    then
        GPGCHECK="1"
    else
        echo "WARNING: Signing Keys Unavailable. Building unsigned RPMS"
        GPGCHECK="0"
    fi
    if [ -r /sign_keys/RPM-GPG-KEY-MDSplus ]
    then
	cp /sign_keys/RPM-GPG-KEY-MDSplus ${BUILDROOT}/etc/pki/rpm-gpg/;
    fi
    cat - > ${BUILDROOT}/etc/yum.repos.d/mdsplus${BNAME}.repo <<EOF
[MDSplus${BNAME}]
name=MDSplus${BNAME}
baseurl=http://www.mdsplus.org/dist/${OS}/${BRANCH}/RPMS
enabled=1
gpgcheck=${GPGCHECK}
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
metadata_expire=300
EOF
    ###
    ### Build RPMS
    ###
    ### Clean up release stage area
    ###
    rm   -Rf /release/${BRANCH}/*
    BRANCH=${BRANCH} \
          RELEASE_VERSION=${RELEASE_VERSION} \
          BNAME=${BNAME} \
          DISTNAME=${DISTNAME} \
          BUILDROOT=${BUILDROOT} \
          PLATFORM=${PLATFORM} \
          ${srcdir}/deploy/platform/${PLATFORM}/${PLATFORM}_build_rpms.py;
    createrepo -q /release/${BRANCH}/RPMS
    badrpm=0
    for rpm in $(find /release/${BRANCH}/RPMS -name '*\.rpm')
    do
        pkg=$(echo $(basename $rpm) | cut -f3 -d-)
        #
        # Skip main installer which only has package dependencies and no files
        # Skip the repo rpm which will contain the branch name
        #
        if ( echo ${pkg} | grep '\.' >/dev/null ) || ( echo ${pkg} | grep repo >/dev/null )
        then
            continue
        fi
        pkg=${pkg}.$(echo $(basename $rpm) | cut -f5 -d- | cut -f3 -d.)
        checkfile=${srcdir}/deploy/packaging/${PLATFORM}/$pkg
        if [ "$UPDATEPKG" = "yes" ]
        then
            mkdir -p ${srcdir}/deploy/packaging/${PLATFORM}/
            makelist $rpm > ${checkfile}
        else
            echo "Checking contents of $(basename $rpm)"
            if ( diff <(makelist $rpm) <(sort ${checkfile}) )
            then
                echo "Contents of $(basename $rpm) is correct."
            else
                checkstatus badrpm "Failure: Problem with contents of $(basename $rpm)" 1
            fi
        fi
    done
    checkstatus abort "Failure: Problem with contents of one or more rpms. (see above)" $badrpm
  fi #nomake
}

publish(){
    ### DO NOT CLEAN /publish as it may contain valid older release rpms
    :&& rsync -a --exclude=repodata /release/${BRANCH}/* /publish/${BRANCH}
    checkstatus abort "Failure: Problem copying release rpms to publish area!" $?
    if ( createrepo -h | grep '\-\-deltas' > /dev/null )
    then
        use_deltas="--deltas"
    fi
    :&& createrepo -q --update --cachedir /publish/${BRANCH}/cache ${use_deltas} /publish/${BRANCH}/RPMS
    checkstatus abort "Failure: Problem creating rpm repository in publish area!" $?
}
