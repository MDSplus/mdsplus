# MDSplus Jenkins Server
## Audience
MDSPlus developers working on mit hosted jenkins server
## Nodes
- jenkins2.mdsplus.org - web front end for jenkins
- mdsplus-jenkins2.psfc.mit.edu - current jenkins 'master' node runs trigger jobs
- mdsplus-builder.psfc.mit.edu - host that runs most of the docker builds
- mac-mdsplus-2.psfc.mit.edu - new(ish) intel based mac for  mac builds
- mac-mdsplus.psfc.mit.edu - older intel based mac for mac builds (being replaced)
- jaspi.psfc.mit.edu - raspberry pi for arm builds - should not be needed !
## Jobs
### Triggers
- 1-alpha-release
- 1-stable-release
- 1-pull-request-test
### Cron jobs
- 2-docker-cleanup
- 2-dockerhub-builders
- 2-dockerhub-services
- 2-Issues Reminders
- 2-restart-slaves-docker-service (not used)
- 2-test-installer
### Conditional
- 3-build_docs
- 3-rsync-dist - This job is run after 1-alpha-release and 1-stable-release. It rsyncs from the localstorage on mdsplus-builder (/mdsplus) to the dist directories on the server (/mdsplus-dist == /mnt/scratch/mdsplus/dist)
- 3-updatepkg
