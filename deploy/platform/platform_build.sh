#!/bin/bash
#
# platform/platform_build.sh
#
# Invoked by mdsplus/deploy/build.sh.
#
# Run docker image to build mdsplus
#
set -e
volume() {
    if [ ! -z "$1" ]
    then
	echo "-v $(realpath ${1}):${2}"
    fi
}

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
spacedelim() {
    if [ ! -z "$1" ]
    then
        if [ "$1" = "skip" ]
        then
            ans=""
        else
            IFS=',' read -ra ARR <<< "$1"
            ans="${ARR[*]}"
        fi
    fi
    echo $ans
}
rundocker(){
    images=(${DOCKERIMAGE})
    ARCH="$(spacedelim ${ARCH})"
    arches=(${ARCH})
    SANITIZE="$(spacedelim $SANITIZE)"
    VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"
    idx=0
    if [ -z "$INTERACTIVE" ]
    then
        stdio="-a stdout -a stderr"
        program="${DOCKER_SRCDIR}/deploy/platform/platform_docker_build.sh"
    else
        stdio="-i"
        program="/bin/bash"
    fi
    while [[ $idx -lt ${#images[*]} ]]
    do
        image=${images[$idx]}
        arch=${arches[$idx]}
        echo "Building installers for ${arch} using ${image}"
        if [ ! -z "$INTERACTIVE" ]
        then
            echo "run ${SRCDIR}/deploy/platform/platform_docker_build.sh"
            echo "or  NOMAKE=1 ${SRCDIR}/deploy/platform/platform_docker_build.sh"
        fi
        #
        # If there are both 32-bit and 64-bit packages for the platform
        # only build the deb's after both 32-bit and 64-bit builds are
        # complete. Likewise only publish the release once.
        #
        if [ -z $JARS_DIR ]
        then jars_dir=
        else jars_dir=/jars_dir
        fi
        if [ -z $FORWARD_PORT ]
        then port_forwarding=
        else port_forwarding="-p ${FORWARD_PORT}:${FORWARD_PORT}"
             echo $port_forwarding
        fi
	status=127
	loop_count=0
	while [ $status = 127 -a $loop_count -lt 5 ]
	do
	    let loop_count=$loop_count+1
            docker run --cap-add=SYS_PTRACE -t $stdio --cidfile=${WORKSPACE}/${OS}_docker-cid \
		   -u $(id -u):$(id -g) --privileged \
		   -h $DISTNAME \
		   -e "ARCH=${arch}" \
		   -e "ARCHES=${ARCH}" \
		   -e "BRANCH" \
		   -e "COLOR" \
		   -e "DISPLAY" \
		   -e "DISTNAME" \
		   -e "GIT_COMMIT" \
		   -e "OS" \
		   -e "PLATFORM" \
		   -e "PUBLISH" \
		   -e "RELEASE" \
		   -e "RELEASE_VERSION"  \
		   -e "SANITIZE" \
		   -e "TEST" \
		   -e "TESTFORMAT" \
		   -e "UPDATEPKG" \
		   -e "VALGRIND_TOOLS" \
		   -e "MAKE_JARS" \
		   -e "CONFIGURE_PARAMS" \
		   -e "mdsevent_port=$EVENT_PORT" \
		   -e "HOME=/workspace" \
		   -e "JARS_DIR=$jars_dir" \
		   -e "TEST_TIMEUNIT" \
		   -v ${SRCDIR}:${DOCKER_SRCDIR} \
		   -v ${WORKSPACE}:/workspace \
		   $port_forwarding \
		   $(volume "${JARS_DIR}" /jars_dir) \
		   $(volume "${RELEASEDIR}" /release) \
		   $(volume "${PUBLISHDIR}" /publish) \
		   $(volume "${KEYS}" /sign_keys) \
		   ${image} $program
            status=$?
            if [ -r ${WORKSPACE}/${OS}_docker-cid ]
            then
		sleep 3
		docker rm $(cat ${WORKSPACE}/${OS}_docker-cid)
		rm -f ${WORKSPACE}/${OS}_docker-cid
            fi
	done
        if [ ! "$status" = "0" ]
        then
    	RED $COLOR
    	cat <<EOF >&2
======================================================

Docker ${PLATFORM}_docker_build.sh returned failure
status when exiting from ${image}

======================================================
EOF
    	NORMAL $COLOR
    	exit $status
        fi
        let idx=idx+1
    done
    exit $status
}
default_build(){
    if [ "${RELEASE}" = "yes" -o "${PUBLISH}" = "yes" ]
    then
        mkdir -p ${RELEASEDIR}/${BRANCH}
    else
        RELEASEDIR=""
    fi
    if [ "${PUBLISH}" = "yes" ]
    then
        mkdir -p ${PUBLISHDIR}/${BRANCH}
    fi
}
set +e
platform_build="${SRCDIR}/deploy/platform/${PLATFORM}/${PLATFORM}_build.sh"
if [ -f "${platform_build}" ]
then
    echo "running ${platform_build}."
    source ${platform_build} "$@"
else
    echo "Using default script."
    default_build
    rundocker
fi
