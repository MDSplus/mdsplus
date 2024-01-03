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
  if [ -n "$1" ]; then
    echo "-v $(realpath ${1}):${2}"
  fi
}
RED() {
  if [ "$COLOR" = "yes" ]; then
    echo -e "\033[31m"
  fi
}
GREEN() {
  if [ "$COLOR" = "yes" ]; then
    echo -e "\033[32m"
  fi
}
NORMAL() {
  if [ "$COLOR" = "yes" ]; then
    echo -e "\033[0m"
  fi
}
spacedelim() {
  if [ -n "$1" ]; then
    if [ "$1" = "skip" ]; then
      ans=""
    else
      IFS=',' read -ra ARR <<<"$1"
      ans="${ARR[*]}"
    fi
  fi
  echo $ans
}
rundocker() {
  images=(${DOCKERIMAGE})
  ARCH="$(spacedelim ${ARCH})"
  arches=(${ARCH})
  SANITIZE="$(spacedelim $SANITIZE)"
  VALGRIND_TOOLS="$(spacedelim $VALGRIND_TOOLS)"
  idx=0
  if [ -z "$INTERACTIVE" ]; then
    # stdio="-a stdout -a stderr"
    stdio="--detach"
    program="${DOCKER_SRCDIR}/deploy/platform/platform_docker_build.sh"
  else
    stdio="-i"
    program="/bin/bash"
  fi
  while [[ $idx -lt ${#images[*]} ]]; do
    image=${images[$idx]}
    arch=${arches[$idx]}
    echo "Building installers for ${arch} using ${image}"
    if [ -n "$INTERACTIVE" ]; then
      echo "env: source ${DOCKER_SRCDIR}/deploy/os/${OS}.env"
      echo "run: ${DOCKER_SRCDIR}/deploy/platform/platform_docker_build.sh"
      echo "or:  NOMAKE=1 ${DOCKER_SRCDIR}/deploy/platform/platform_docker_build.sh"
    fi
    #
    # If there are both 32-bit and 64-bit packages for the platform
    # only build the deb's after both 32-bit and 64-bit builds are
    # complete. Likewise only publish the release once.
    #
    if [ -z $JARS_DIR ]; then
      jars_dir=
    else
      jars_dir=/jars_dir
    fi
    if [ -z $FORWARD_PORT ]; then
      port_forwarding=
    else
      port_forwarding="-p ${FORWARD_PORT}:${FORWARD_PORT}"
      echo $port_forwarding
    fi
    if docker network >/dev/null 2>&1; then
      #docker supports --network
      if [ -z ${DOCKER_NETWORK} ]; then
        docker network rm ${OS} || :
        docker network create ${OS}
        NETWORK=--network=${OS}
      else
        NETWORK=--network=${DOCKER_NETWORK}
      fi
    else
      DOCKER_NETWORK=bridge
      NETWORK=
    fi

    function kill_docker() {
      if [ -r ${WORKSPACE}/${OS}_docker-cid ]; then
        docker kill $(cat ${WORKSPACE}/${OS}_docker-cid) || true
        docker rm $(cat ${WORKSPACE}/${OS}_docker-cid) || true
        rm -f ${WORKSPACE}/${OS}_docker-cid
      fi
    }

    function abort() {
      kill_docker
      status=1
    }

    trap abort SIGINT

    status=127
    loop_count=0
    while [ $status = 127 -a $loop_count -lt 5 ]; do
      let loop_count=$loop_count+1

      kill_docker

      docker run --cap-add=SYS_PTRACE -t $stdio \
        --rm \
        --cidfile=${WORKSPACE}/${OS}_docker-cid \
        ${NETWORK} \
        -u $(id -u):$(id -g) --privileged -h $DISTNAME -e "srcdir=${DOCKER_SRCDIR}" \
        -e "ARCH=${arch}" \
        -e "ARCHES=${ARCH}" \
        -e "BRANCH" \
        -e "BNAME" \
        -e "FLAVOR" \
        -e "COLOR" \
        -e "DISPLAY" \
        -e "DISTNAME" \
        -e "GIT_COMMIT" \
        -e "OS" \
        -e "PLATFORM" \
        -e "PUBLISH" \
        -e "RELEASE" \
        -e "RELEASE_VERSION" \
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
        -e "CONFIGURE_EXTRA" \
        -v ${SRCDIR}:${DOCKER_SRCDIR} \
        -v ${WORKSPACE}:/workspace $port_forwarding $(volume "${JARS_DIR}" /jars_dir) \
        $(volume "${RELEASEDIR}" /release) \
        $(volume "${PUBLISHDIR}" /publish) \
        $(volume "${KEYS}" /sign_keys) \
        ${image} $program
      status=$?

      if [ -z "$INTERACTIVE" ]; then
        docker logs -f $(cat ${WORKSPACE}/${OS}_docker-cid)
      fi
    done
    if [ -z ${DOCKER_NETWORK} ]; then
      docker network rm ${OS}
    fi
    if [ ! "$status" = "0" ]; then
      RED
      cat <<EOF >&2
======================================================

Docker ${PLATFORM}_docker_build.sh returned failure
status when exiting from ${image}

======================================================
EOF
      NORMAL
      exit $status
    fi
    let idx=idx+1
  done
  exit $status
}
default_build() {
  if [ "${RELEASE}" = "yes" ]; then
    rm -Rf ${RELEASEDIR}/${FLAVOR}
    mkdir -p ${RELEASEDIR}/${FLAVOR}
  fi
  if [ "${PUBLISH}" = "yes" ]; then
    mkdir -p ${PUBLISHDIR}/${FLAVOR}
  fi
}

case "$BRANCH" in
  stable)
    export CONFIGURE_EXTRA=
    export FLAVOR="stable"
    export BNAME=""
    ;;
  alpha)
    export CONFIGURE_EXTRA=--enable-debug=info
    export FLAVOR="alpha"
    export BNAME="-alpha"
    ;;
  *)
    export CONFIGURE_EXTRA=--enable-debug=info
    export FLAVOR="other"
    export BNAME="-other"
    ;;
esac

set +e
platform_build="${SRCDIR}/deploy/platform/${PLATFORM}/${PLATFORM}_build.sh"
if [ -f "${platform_build}" ]; then
  echo "running ${platform_build}."
  source ${platform_build} "$@"
else
  echo "Using default script."
  default_build
  rundocker
fi
