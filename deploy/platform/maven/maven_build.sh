#!/bin/bash
#
# runs maven in a controlled maner
mkdir -p $WORKSPACE
cleanup() {
  docker rm $(cat $WORKSPACE/docker-cid)
  rm -f $WORKSPACE/docker-cid
}
trap cleanup EXIT
volume() {
  if [ -n "$1" ]
  then echo "-v $(realpath $1):$2"
  fi
}

MVN="mvn -B -Dmaven.repo.local=/release/maven/repository -DsourceDirectory=/maven"
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
 if [ -n "$PUBLISHDIR" ]
 then mkdir -p "$PUBLISHDIR"
 fi
 if [ -z $KEYS ]
 then MVNGOAL="$MVNGOAL install"
 else MVNGOAL="$MVNGOAL deploy"
 fi
elif [ "$RELEASE" = "yes" ]
then
 echo "RELEASEDIR=$RELEASEDIR"
 if [ -n "$RELEASEDIR" ]
 then mkdir -p "$RELEASEDIR"
 fi
 MVNGOAL="$MVNGOAL package"
else
 RELEASEDIR=$WORKSPACE/tests
 mkdir -p $RELEASEDIR
 MVNGOAL="$MVNGOAL test"
fi

if [ -n "$INTERACTIVE" ]
then
 PROGRAM=/bin/bash
 echo run /scripts/run_maven.sh
 INTERACTIVE="-i"
else PROGRAM=/scripts/run_maven.sh
fi

if [ "$BRANCH" != "alpha" ]
then RELEASE_VERSION=0.0.0-SNAPSHOT
fi

docker run -t $INTERACTIVE -u $(id -u) --network=host -a stdout -a stderr --cidfile="$WORKSPACE/docker-cid" \
   -e MVN="$MVN" \
   -e MVNGOAL="$MVNGOAL" \
   -e BRANCH="$BRANCH" \
   -e RELEASE_VERSION="$RELEASE_VERSION" \
   -e GNUPGHOME=/sign_keys/.gnupg \
   -e HOME=/release/maven \
   $(volume "$REPOSITORY" /repository) \
   $(volume "$WORKSPACE" /root) \
   $(volume "$SRCDIR/deploy/platform/maven" /scripts) \
   $(volume "$SRCDIR/java" /maven) \
   $(volume "$RELEASEDIR" /release) \
   $(volume "$WORKSPACE" /workspace) \
   $(volume "$KEYS" /sign_keys) \
   $DOCKERIMAGE $PROGRAM
