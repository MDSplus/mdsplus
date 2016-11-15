#!/bin/bash
#
# windows_docker_build.sh - build windows installer
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

export PYTHONPATH=/workspace/python
mkdir -p ${PYTHONPATH}
ln -sf /source/mdsobjects/python ${PYTHONPATH}/MDSplus
export PyLib=python27
export MDS_PATH=/source/tdi
set -e
MAKE=${MAKE:="env LANG=en_US.UTF-8 make"}

if [ "$TEST" = "yes" -o "$RELEASE" = "yes" ]
then
    
    export JNI_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk
    export JNI_MD_INCLUDE_DIR=/source/3rd-party-apis/windows-jdk/win32
    MDSPLUS_DIR=/workspace/buildroot;
    MDS_PATH=${MDSPLUS_DIR}/tdi;
    mkdir -p /workspace/64;
    mkdir -p ${MDSPLUS_DIR};
    pushd /workspace/64;
    /source/configure  \
	     --prefix=${MDSPLUS_DIR} \
	     --exec_prefix=${MDSPLUS_DIR} \
	     --bindir=${MDSPLUS_DIR}/bin_x86_64 \
	     --libdir=${MDSPLUS_DIR}/bin_x86_64 \
	     --host=x86_64-w64-mingw32 \
	     --with-java_target=6 \
	     --with-java_bootclasspath=/source/rt.jar
    $MAKE
    $MAKE install
    popd
    mkdir -p /workspace/32;
    pushd /workspace/32;
    /source/configure  \
	--prefix=${MDSPLUS_DIR} \
	--exec_prefix=${MDSPLUS_DIR} \
	--bindir=${MDSPLUS_DIR}/bin_x86 \
	--libdir=${MDSPLUS_DIR}/bin_x86 \
	--host=i686-w64-mingw32 \
	--with-java_target=6 \
	--with-java_bootclasspath=/source/rt.jar
    $MAKE
    $MAKE install
    popd
fi
if [ "${TESTS}" = "yes" ]
then
    pushd /workspace/64
    set +e
    echo " --- MAKING TESTS 64bit --- "
    if ( ! $MAKE -k tests )
    then
	RED $COLOR
	cat <<EOF >&2
==================================================

Failure: problem performing 64-bit tests

==================================================
EOF
	NORMAL $COLOR
	tests_failed=yes
    fi
    popd
    pushd /workspace/32
    echo " --- MAKING TESTS 32bit --- "
    if ( ! $MAKE -k tests )
    then
	RED $COLOR
	cat <<EOF >&2
==================================================

Failure: problem performing 64-bit tests

==================================================
EOF
	NORMAL $COLOR
	tests_failed=yes
    fi
    popd
    if [ "$test_failed" = "yes" ]
    then
	exit 1
    fi
fi

set -e

if [ "$RELEASE" = "yes" ]
then
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
	bname=""
    else
	bname="-${BRANCH}"
    fi
    if [ ! -z "$WINHOST" -a -r /winbld -a ! -z "$WINREMBLD" ]
    then
	windows_cleanup() {
	    rm -Rf /winbld/${tmpdir}
	}
	pushd /winbld
	tmpdir=$(mktemp -d mdsplus-XXXXXXXXXX)
	trap windows_cleanup EXIT
	topsrcdir=${WINREMBLD}/${tmpdir}
	cd ${tmpdir}
	rsync -am --include="*/" --include="*.h*" --include="*.def" --exclude="*" /source/ ./
	rsync -am /workspace/64/include/config.h ./include/
	rsync -a /source/mdsobjects/cpp /source/mdsobjects/MdsObjects* /source/mdsobjects/VS-* ./mdsobjects/
	rsync -a /source/deploy/platform/windows/winbld.bat ./deploy/
	rsync -a ${MDSPLUS_DIR}/bin_* ./
	curl http://${WINHOST}:8080${topsrcdir}/deploy/winbld.bat
	# see if files are there
	if ( ls /winbld/${tmpdir}/bin_x86*/*.lib /winbld/${tmpdir}/bin_x86*/MdsObjectsCppShr-VS.* > /dev/null )
	then
	    rsync -av --include="*/" --include="*.lib" --include="MdsObjectsCppShr-VS.dll" --exclude="*" /winbld/${tmpdir}/bin_x86* ${MDSPLUS_DIR}/
	else
	    RED $COLOR
	    cat <<EOF >&2
============================================

Failure: Problem building Visual Studio dll

============================================
EOF
	    NORMAL $COLOR
	    exit 1
	fi
	vs="-DVisualStudio"
	popd
    fi
    pushd $MDSPLUS_DIR
    makensis -DMAJOR=${major} -DMINOR=${minor} -DRELEASE=${release} -DFLAVOR=${bname} -NOCD \
             -DOUTDIR=/release ${vs} /source/deploy/packaging/${PLATFORM}/mdsplus.nsi
    popd
    if [ -d /sign_keys ]
    then
	set +e
	for timestamp_server in http://timestamp.comodoca.com/authenticode http://timestamp.verisign.com/scripts/timestamp.dll http://timestamp.globalsign.com/scripts/timestamp.dll http://tsa.starfieldtech.com
	do
	    if ( signcode -spc /sign_keys/mdsplus.spc \
				-v /sign_keys/mdsplus.pvk \
				-a sha1 \
				-$ individual \
				-n MDSplus  \
				-i http://www.mdsplus.org/ \
				-t http://timestamp.verisign.com/scripts/timestamp.dll \
				-tr 10 /release/MDSplus${bname}-${major}.${minor}-${release}.exe <<EOF
mdsplus
EOF
	       )
	    then
		break
	    fi
	done
	set -e
	    
    fi
fi

if [ "$PUBLISH" = "yes" ]
then
    major=$(echo ${RELEASE_VERSION} | cut -d. -f1)
    minor=$(echo ${RELEASE_VERSION} | cut -d. -f2)
    release=$(echo ${RELEASE_VERSION} | cut -d. -f3)
    if [ "${BRANCH}" = "stable" ]
    then
	bname=""
    else
	bname="-${BRANCH}"
    fi
    rsync -a /release/MDSplus${bname}-${major}.${minor}-${release}.exe /publish/${BRANCH}/
fi
