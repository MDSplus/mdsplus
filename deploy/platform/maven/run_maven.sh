#!/bin/bash
#
# runs maven within docker

mkdir -p /workspace/maven
cd /workspace/maven||exit $?
mkdir -p /release/maven||exit $?

# setup project.directory
for f in $(find /maven -name pom.xml)
do
  target_dir=$(dirname /workspace/maven/${f:6})
  mkdir -p $target_dir
  cp -f $f $target_dir
  ln -sfT $(dirname $f)/src $target_dir/src
done

$MVN versions:set -DgenerateBackupPoms=false -DnewVersion=$RELEASE_VERSION -DartifactId=*
$MVN $MVNGOAL
