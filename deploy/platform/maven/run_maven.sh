#!/bin/bash
#
# runs maven within docker
for f in $(find /maven -name pom.xml)
do
  target_dir=$(dirname /workspace/maven/${f:6})
  mkdir -p $target_dir
  cp $f    $target_dir
done
cd /workspace/maven||exit $?
mkdir -p /release/maven

$MVN versions:set -DgenerateBackupPoms=false -DnewVersion=$RELEASE_VERSION -DartifactId=*
$MVN $MVNGOAL
