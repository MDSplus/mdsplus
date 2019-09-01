#!/bin/bash
#
# runs $srcdir/bootstrap in a controlled maner
mkdir -p ${WORKSPACE}
cleanup() {
  docker rm $(cat ${WORKSPACE}/docker-cid)
  rm -f ${WORKSPACE}/docker-cid
}
trap cleanup EXIT
volume() {
  if [ ! -z "$1" ]
  then echo "-v $(realpath ${1}):${2}"
  fi
}

if [ ! -z $PUBLISHDIR ]
then mkdir -p "$PUBLISHDIR"
fi

MVN="mvn -Dmaven.repo.local=/workspace/.m2/repository -DsourceDirectory=/maven"
if [ ! -z $KEYS ]
then MVN="$MVN -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml"
fi

# setup goal
if [ "$TEST" = "yes" ]
then MVNGOAL=
else MVNGOAL="-DskipTests"
fi
if [ -z $KEYS ]
then MVNGOAL="$MVNGOAL -Dgpg.skip"
fi
if   [ "$PUBLISH" = "yes" ]
then
 if [ -z $KEYS ]
 then MVNGOAL="$MVNGOAL install"
 else MVNGOAL="$MVNGOAL deploy"
 fi
elif [ "$RELEASE" = "yes" ]
then MVNGOAL="$MVNGOAL package"
else MVNGOAL="$MVNGOAL test"
fi

docker run -t -a stdout -a stderr --cidfile="${WORKSPACE}/docker-cid" \
   -e MVN \
   -e "HOME=/workspace" \
   $(volume "${SRCDIR}/java" /maven) \
   $(volume "${WORKSPACE}" /workspace) \
   $(volume "${KEYS}" /sign_keys) \
   ${DOCKERIMAGE} /bin/sh -c "
	rm -rf /workspace/maven;mkdir -p /workspace/maven;cd /workspace/maven
	find /maven -name pom.xml -exec /bin/sh -c 'mkdir -p "\$"(dirname /workspace"\$"0);cp "\$"0 /workspace"\$"0' '{}' ';'
        ${MVN} versions:set -DgenerateBackupPoms=false -DnewVersion=${RELEASE_VERSION} -DartifactId=* &&\
	${MVN} $MVNGOAL
"
