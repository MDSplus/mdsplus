#!/bin/bash
cd $(realpath $(dirname $0)/../../../)

get_latest() {
 URL="https://oss.sonatype.org/content/groups/public/org/mdsplus/mdsplus-parent/maven-metadata.xml"
 while [ -z $LATEST ]
 do
  echo request LATEST >&2
  export LATEST=$(curl -s -m 10 $URL 2>/dev/null | grep -oP '(?<=\<latest\>).*(?=\</latest\>)')
  if [ $? -eq 130 ]; then exit 130; fi
 done
 echo $LATEST
}

get_release_diff() {
 export LATEST=$(get_latest)
 git diff alpha_release-${LATEST//./-} --name-only java/
}

get_modules() {
 echo $(grep -oP '(?<=\<module\>).*(?=\</module\>)' java/pom.xml)
}

project_changed() {
 export LATEST=$(get_latest)
 MODULES=$(get_modules)
 if get_release_diff|grep -P "^java/(${MODULES// /\|})/(pom.xml|src/.*)\$" >/dev/null
 then
  echo yes
  return 0
 else
  echo no
  return 1
 fi
}

if [ -n "$1" ]
then
  test -n "$WORKSPACE"&&cd $WORKSPACE
  for cmd in "$@"
  do eval $cmd
  done
fi
