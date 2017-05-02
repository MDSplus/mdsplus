#!/bin/bash
#
# Promote a branch (i.e. stable) to the current head of another branch (i.e. alpha)
#
# Format: promote.sh git-source-dir 
BRANCH=$1
PROMOTE_TO=$2
set -v
set -e
git checkout ${PROMOTE_TO}
git pull
git checkout ${BRANCH}
git rebase -q -s ours ${PROMOTE_TO}
set +e
git pull -q -s recursive -X theirs 2>/dev/null
set -e
git status | grep "modified:" | awk '{system("git checkout '${PROMOTE_TO}' -- "$2)}'
git status | grep "deleted by us:" | awk '{system("git rm "$4)}'
git commit -m "Feature: Promote ${BRANCH} to ${PROMOTE_TO}"
git push
