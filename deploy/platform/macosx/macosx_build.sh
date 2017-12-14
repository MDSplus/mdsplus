#!/bin/bash
#
# platform/macosx/macosx_build.sh
#
# Invoked by mdsplus/deploy/platform/platform_build.sh for windows platform.
#
#

RED() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[31;47m"
    fi
}
GREEN() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[32;47m"
    fi
}
NORMAL() {
    if [ "$1" = "yes" ]
    then
	echo -e "\033[m"
    fi
}

set -e

if [ -r ${SRCDIR}/deploy/os/${OS}.env ]
then
    source ${SRCDIR}/deploy/os/${OS}.env
fi

MAKE=${MAKE:="make"}
if [ -z "$JARS_DIR" ]
then
    JAVA_OPTS="--with-java_target=6 --with-java_bootclasspath=${SRCDIR}/rt.jar"
else
    JAVA_OPTS="--with-jars=${JARS_DIR}"
fi

export PYTHONDONTWRITEBYTECODE=no
if [ "$TEST" = "yes" ]
then
    rm -Rf ${WORKSPACE}/test
    mkdir -p ${WORKSPACE}/test
    MDSPLUS_DIR=${WORKSPACE}/test/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR}
    pushd ${WORKSPACE}/test/
    ${SRCDIR}/configure \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    ${JAVA_OPTS} \
	    --enable-debug
    $MAKE
    $MAKE install
    if ( ! $MAKE -k tests 2>&1 )
    then
	RED $COLOR
	cat <<EOF >&2
======================================================

Failure doing normal tests.

======================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
    popd
fi

if [ "$RELEASE" = "yes" ]
then
    mkdir -p ${RELEASEDIR}/${BRANCH}
    rm -Rf ${RELEASEDIR}/${BRANCH}/*
    ###
    ### Clean up workspace
    ###
    rm -Rf ${WORKSPACE}/releasebld
    ###
    ### Build release version of MDSplus and then construct installer rpms
    ###
    MDSPLUS_DIR=${WORKSPACE}/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR}
    cp ${SRCDIR}/deploy/platform/macosx/MDSplus.pkgproj ${WORKSPACE}/releasebld/
    pushd ${WORKSPACE}/releasebld/
    ${SRCDIR}/configure \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    ${JAVA_OPTS}
    $MAKE
    $MAKE install
    popd
    if [ "$BRANCH" == "stable" ]
    then
	BNAME=""
    else
	BNAME="-${BRANCH}"
    fi
    IFS='.' read -ra VERS <<< "${RELEASE_VERSION}"
    /usr/local/bin/packagesbuild  -v -F ${SRCDIR} ${WORKSPACE}/releasebld/MDSplus.pkgproj
    if [ "$?" == "0" ]
    then
        mv ${WORKSPACE}/releasebld/buildroot/MDSplus.pkg ${RELEASEDIR}/${BRANCH}/MDSplus${BNAME}-${VERS[0]}-${VERS[1]}-${VERS[2]}-osx.pkg 
    fi
#    /Developer/usr/bin/packagemaker \
#	--title "MDSplus%(pkgflavor)s" \
#	--version "%(major)d.%(minor)d.%(release)d" \
#	--scripts ${SRCDIR}/macosx/scripts \
#	--install-to "/" \
#	--target "10.5" \
#	-r ${WORKSPACE}/releasebld/buildroot -v -i "MDSplus${BNAME}" \
#	-o ${RELEASEDIR}/${BRANCH}/MDSplus${BNAME}-${VERS[0]}-${VERS[1]}-${VERS[2]}-osx.pkg
    if [ "$?" != "0" ]
    then
	RED $COLOR
	cat <<EOF >&2
======================================================

Failure: Error building installer

======================================================
EOF
	NORMAL $COLOR
	exit 1
    fi
fi

if [ "$PUBLISH" = "yes" ]
then
    set -e
    mkdir -p ${PUBLISHDIR}/${BRANCH}
    if [ "$BRANCH" == "stable" ]
    then
	BNAME=""
    else
	BNAME="-${BRANCH}"
    fi
    IFS='.' read -ra VERS <<< "${RELEASE_VERSION}"
    rsync -a ${RELEASEDIR}/${BRANCH}/MDSplus${BNAME}-${VERS[0]}-${VERS[1]}-${VERS[2]}-osx.pkg ${PUBLISHDIR}/${BRANCH}/
fi
