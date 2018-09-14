#!/bin/bash
#
# Promote a branch (i.e. stable) to the current head of another branch (i.e. alpha)
#
# Format: promote.sh git-source-dir 
BRANCH=$1
PROMOTE_TO=$2
if ( echo $0 | grep /tmp > /dev/null )
then
  set -v
  set -e
  git checkout ${PROMOTE_TO}
  git clean -x -f -d
  git pull
  git checkout ${BRANCH}
  git pull
  git branch -D $(whoami)-promote-${BRANCH}-${PROMOTE_TO} 2>/dev/null || :
  git checkout ${BRANCH} -b $(whoami)-promote-${BRANCH}-${PROMOTE_TO}
  git rebase -q -s ours ${PROMOTE_TO}
  set +e
  git merge stable -q -s recursive -X theirs 2>/dev/null
  set -e
  git status | grep "modified:" | awk '{system("git checkout '${PROMOTE_TO}' -- "$2)}'
  git status | grep "deleted by us:" | awk '{system("git rm "$4)}'
  git commit -a -m "Feature: Incorporate new features and fixes from ${PROMOTE_TO} branch into ${BRANCH}" 
  git tag ${BRANCH}_release-$(git tag | grep ${PROMOTE_TO}_release\- | sort -Vr | awk -F \- '{print $2"-"$3"-"$4; exit}')
  rm -f $0
else
    cp $0 /tmp/
    /tmp/promote.sh ${BRANCH} ${PROMOTE_TO}
fi


