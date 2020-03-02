#!/bin/bash
BRANCH=$1
PROMOTED_RELEASE_TAG=$2
if [[ "${PROMOTED_RELEASE_TAG}" =~ ^alpha_release-[0-9]+-[0-9]+-[0-9]+$ ]]
then
 set +x
 git reset --hard
 git merge ${PROMOTED_RELEASE_TAG} --strategy ours -m"Build: promoted ${PROMOTED_RELEASE_TAG}" --log
 git rm -rfq --cached .
 git checkout ${PROMOTED_RELEASE_TAG} .
 git commit --amend --no-edit
 git clean -fxdq
 if ! git diff --name-only --exit-code ${BRANCH} ${PROMOTED_RELEASE_TAG}
 then exit 1
 fi
 TAG=${PROMOTED_RELEASE_TAG/alpha_/${BRANCH}_}
 git push --set-upstream origin ${BRANCH}
else
 echo "Can only promote to alpha_release-* tags"
 echo "Got \"$PROMOTED_RELEASE_TAG\" instead"
 exit 1
fi
