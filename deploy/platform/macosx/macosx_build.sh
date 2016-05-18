#!/bin/bash
#
# platform/macosx/macosx_build.sh
#
# Invoked by mdsplus/deploy/build.sh for macosx platforms.
#
#

set -e

if [ -r ${SRCDIR}/deploy/os/${OS}.env ]
then
    source ${SRCIDR}/deploy/os/${OS}.env
fi

PUBLISHDIR=${PUBLISHDIR}/${BRANCH}
MAKE=${MAKE:="env LANG=en_US.UTF-8 make"}

if [ "${RELEASE}" = "yes" ]
then
    RELEASEDIR=${RELEASEDIR}/${BRANCH}
    mkdir -p ${RELEASEDIR}
else
    RELEASEDIR=""
fi

if [ "${PUBLISH}" = "yes" ]
then
    PUBLISHDIR=${PUBLISHDIR}
    mkdir -p ${PUBLISHDIR}
fi

if [ "$RELEASE" = "yes" ]
then
    ###
    ### Clean up workspace
    ###
    set +e
    rm -Rf ${WORKSPACE}/releasebld
    ###
    ### Build release version of MDSplus and then construct installer rpms
    ###
    set -e
    MDSPLUS_DIR=${WORKSPACE}/releasebld/buildroot/usr/local/mdsplus
    mkdir -p ${MDSPLUS_DIR}
    pushd ${WORKSPACE}/releasebld/
    ${SRCDIR}/configure \
	    --prefix=${MDSPLUS_DIR} \
	    --exec_prefix=${MDSPLUS_DIR} \
	    --with-java_target=6 \
	    --with-java_bootclasspath=${SRCDIR}/rt.jar
    $MAKE
    $MAKE install
    popd
    if [ "$BRANCH" == "stable" ]
    then
	BNAME=""
    else
	BNAME="-${BRANCH}"
    fi
    IFS='.' read -ra VERS <<< "${VERSION}"
    sudo chown -R root:admin ${WORKSPACE}/releasebld/buildroot
    /Developer/usr/bin/packagemaker --title "MDSplus%(pkgflavor)s" \
				    --version "%(major)d.%(minor)d.%(release)d" \
				    --scripts ${SRCDIR}/macosx/scripts \
				    --install-to "/usr/local" \
				    --target "10.5" \
				    -r $(pwd)/build -v -i "MDSplus${BNAME}" \
				    -o ${RELEASEDIR}/MDSplus${BNAME}-${VERS[0]}-${VERS[1]}-${VERS[2]}-osx.pkg
    sudo chown -R $(id -un):$(id -gn) $(pwd)/build
fi

if [ "$PUBLISH" = "yes" ]
then
    set -e
    if [ "$BRANCH" == "stable" ]
    then
	BNAME=""
    else
	BNAME="-${BRANCH}"
    fi
    IFS='.' read -ra VERS <<< "${VERSION}"
    rsync -a ${RELEASEDIR}/MDSplus${BNAME}-${VERS[0]}-${VERS[1]}-${VERS[2]}-osx.pkg ${PUBLISHDIR}/
fi
