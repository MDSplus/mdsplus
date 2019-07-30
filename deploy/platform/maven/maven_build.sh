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

MVN="mvn -Dmaven.repo.local=/workspace/.m2/repository -DoutputDirectory=\$PWD -DsourceDirectory=/mdsplus-api/src"
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
   $(volume "${SRCDIR}/java/mdsplus-api" /mdsplus-api) \
   $(volume "${WORKSPACE}" /workspace) \
   $(volume "${KEYS}" /sign_keys) \
   ${DOCKERIMAGE} /bin/sh -c "
	mkdir -p /workspace/mdsplus-api
	cd /workspace/mdsplus-api
	cat /mdsplus-api/pom.xml|sed -e 's/0.0.0-SNAPSHOT/${RELEASE_VERSION}/'>./pom.xml&&
	${MVN} $MVNGOAL
"
