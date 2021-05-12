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

do_createrepo() {
  repodir=$1
  tmpdir=$(mktemp -d)
  trap 'rm -Rf ${tmpdir}' EXIT
  if [ -d ${repodir}/${FLAVOR}/RPMS/repodata ]; then
    rsync -a ${repodir}/${FLAVOR}/RPMS/repodata ${tmpdir}
    update_args="--update --cachedir ${repodir}/${FLAVOR}/cache ${use_deltas}"
  fi
  : && createrepo -q $update_args -o ${tmpdir} ${repodir}/${FLAVOR}/RPMS
  checkstatus abort "Failure: Problem creating rpm repository in ${repodir}!" $?
  : && rsync -a ${tmpdir}/repodata ${repodir}/${FLAVOR}/RPMS/
}

srcdir=$(readlink -e $(dirname ${0})/../..)

test64="64 x86_64-linux bin64 lib64 --with-gsi=/usr:gcc64"
test32="32 i686-linux   bin32 lib32 --with-gsi=/usr:gcc32 --with-valgrind-lib=/usr/lib32/valgrind"

runtests() {
  # run tests with the platform specific params read from test32 and test64
  testarch ${test64}
  if [ "${ARCHES}" != "amd64" ]; then
    if [ -f /usr/bin/python-i686 ]; then
      PYTHON=/usr/bin/python-i686 testarch ${test32}
    else
      testarch ${test32}
    fi
  fi
  checktests
}
makelist() {
  rpm2cpio $1 |
    cpio --list --quiet |
    grep -v MDSplus/dist |
    grep -v MDSplus/doc |
    grep -v MDSplus/build |
    grep -v egg-info |
    grep -v '\.build\-id' |
    sort
}
buildrelease() {
  set -e
  RELEASEBLD=/workspace/releasebld
  BUILDROOT=${RELEASEBLD}/buildroot
  MDSPLUS_DIR=${BUILDROOT}/usr/local/mdsplus
  rm -Rf ${RELEASEBLD} /release/${FLAVOR}
  mkdir -p ${RELEASEBLD}/64 ${BUILDROOT} ${MDSPLUS_DIR}
  pushd ${RELEASEBLD}/64
  config ${test64} ${CONFIGURE_EXTRA}
  if [ -z "$NOMAKE" ]; then
    $MAKE
    $MAKE install
  fi
  popd
  if [ "${ARCHES}" != "amd64" ]; then
    mkdir -p ${RELEASEBLD}/32
    pushd ${RELEASEBLD}/32
    config ${test32} ${CONFIGURE_EXTRA}
    if [ -z "$NOMAKE" ]; then
      $MAKE
      $MAKE install
    fi
    popd
  fi
  if [ -z "$NOMAKE" ]; then
    echo "Building rpms"
    ###
    ### Setup repository rpm info
    ###
    mkdir -p ${BUILDROOT}/etc/yum.repos.d
    mkdir -p ${BUILDROOT}/etc/pki/rpm-gpg/
    cp ${srcdir}/deploy/platform/redhat/RPM-GPG-KEY-MDSplus ${BUILDROOT}/etc/pki/rpm-gpg/
    if [ -d /sign_keys/.gnupg ]; then
      GPGCHECK="1"
    else
      echo "WARNING: Signing Keys Unavailable. Building unsigned RPMS"
      GPGCHECK="0"
    fi
    if [ -r /sign_keys/RPM-GPG-KEY-MDSplus ]; then
      cp /sign_keys/RPM-GPG-KEY-MDSplus ${BUILDROOT}/etc/pki/rpm-gpg/
    fi
    cat - >${BUILDROOT}/etc/yum.repos.d/mdsplus${BNAME}.repo <<EOF
[MDSplus${BNAME}]
name=MDSplus${BNAME}
baseurl=http://www.mdsplus.org/dist/${OS}/${FLAVOR}/RPMS
enabled=1
gpgcheck=${GPGCHECK}
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-MDSplus
metadata_expire=300
EOF
    mkdir -p /release/${FLAVOR}
    BUILDROOT=${BUILDROOT}\
      ${srcdir}/deploy/packaging/${PLATFORM}/${PLATFORM}_build_rpms.py
    do_createrepo /release
    badrpm=0
    for rpm in $(find /release/${FLAVOR}/RPMS -name '*\.rpm'); do
      pkg=$(echo $(basename $rpm) | cut -f3 -d-)
      #
      # Skip main installer which only has package dependencies and no files
      # Skip the repo rpm which will contain the branch name
      #
      if (echo ${pkg} | grep '\.' >/dev/null) || (echo ${pkg} | grep repo >/dev/null); then
        continue
      fi
      pkg=${pkg}.$(echo $(basename $rpm) | cut -f5 -d- | cut -f3 -d.)
      checkfile=${srcdir}/deploy/packaging/${PLATFORM}/$pkg
      if [ "$UPDATEPKG" = "yes" ]; then
        mkdir -p ${srcdir}/deploy/packaging/${PLATFORM}/
        makelist $rpm >${checkfile}
      else
        echo "Checking contents of $(basename $rpm)"
        if (diff <(makelist $rpm) <(sort ${checkfile})); then
          echo "Contents of $(basename $rpm) is correct."
        else
          checkstatus badrpm "Failure: Problem with contents of $(basename $rpm)" 1
        fi
      fi
    done
    checkstatus abort "Failure: Problem with contents of one or more rpms. (see above)" $badrpm
  fi #nomake
}

publish() {
  ### DO NOT CLEAN /publish as it may contain valid older release rpms
  mkdir -p /publish/${FLAVOR}
  : && rsync -a --exclude=repodata /release/${FLAVOR}/* /publish/${FLAVOR}
  checkstatus abort "Failure: Problem copying release rpms to publish area!" $?
  if (createrepo -h | grep '\-\-deltas' >/dev/null); then
    use_deltas="--deltas"
  fi
  do_createrepo /publish
  checkstatus abort "Failure: Problem updating rpm repository in publish area!" $?
}
