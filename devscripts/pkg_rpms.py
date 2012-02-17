import subprocess,datetime,os,sys
from pkg_utils import getDist, getWorkspace, getFlavor, getVersion, getRelease, getReleaseTag, checkRelease, getPackages, makeSrcTar, newRelease
def signrpmsCommand(args):
    print signrpms(args[2])

def signrpms(arch):
    try:
        import pexpect
        WORKSPACE=getWorkspace()
        cmd="/bin/sh -c 'rpmsign --addsign --define=\"_signature gpg\" --define=\"_gpg_name MDSplus\" *.rpm'"
    	child = pexpect.spawn(cmd,timeout=60,cwd=WORKSPACE+'/RPMS/'+arch)
#    	child.logfile=sys.stdout
    	child.expect("Enter pass phrase: ")
    	child.sendline("")
        child.expect(pexpect.EOF)
    	child.close()
    	return child.status
    except Exception,e:
        print "Error with signrpms - %s" % (e,)
        return 1

def beginRpmSpec(specfile,version,release,rpmflavor):
    f_in=open('rpm/mdsplus-part1.spec','r')
    f_out=open(specfile,'w')
    line=f_in.readline()
    while len(line) > 0:
        line=line.replace("--VERSION--",version)
        line=line.replace("--RELEASE--",str(release))
        line=line.replace("--RPMFLAVOR--",rpmflavor)
        f_out.write(line)
        line=f_in.readline()
    f_in.close()
    for pkg in getPackages():
        f_out.write("requires: mdsplus%s-%s\n" % (rpmflavor,pkg))
    f_in=open('rpm/mdsplus-part2-hudson.spec',"r")
    line=f_in.readline()
    while len(line) > 0:
        f_out.write(line)
        line=f_in.readline()
    f_in.close()
    f_out.close()

def addPkgToRpmSpec(specfile,pkg,release,DIST,rpmflavor):
    f_in=open('rpm/subpackages/%s' % (pkg,),'r')
    f_out=open(specfile,'a')
    line=f_in.readline()
    while len(line) > 0:
        line=line.replace("--RELEASE--","%d.%s" % (release,DIST))
        line=line.replace("--RPMFLAVOR--",rpmflavor)
        f_out.write(line)
        line=f_in.readline()
    f_in.close()
    f_out.close()

def makeRepoRpms():
    WORKSPACE=getWorkspace()
    DIST=getDist()
    FLAVOR=getFlavor()
    p=subprocess.Popen('rpmbuild -ba' +\
                ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                ' --define="_topdir %s"' % (WORKSPACE,)+\
                ' --define="_builddir %s"' % (WORKSPACE,)+\
                ' --define="flavor %s"' % (FLAVOR,)+\
                ' --define="s_dist %s"' % (DIST,)+\
                ' %s/x86_64/mdsplus/rpm/repos.spec >/dev/null' % (WORKSPACE,),shell=True,cwd=os.getcwd())
    rpmbuild_status=p.wait()
    if rpmbuild_status != 0:
        print "Error building repository rpm for x86_64 %s %s. rpmbuild returned status=%d." % (DIST,FLAVOR,rpmbuild_status)
        status="error"
    else:
        p=subprocess.Popen('rpmbuild -ba'+\
                    ' --target=i686-linux'+\
                    ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                    ' --define="_topdir %s"' % (WORKSPACE,)+\
                    ' --define="_builddir %s"' % (WORKSPACE,)+\
                    ' --define="flavor %s"' % (FLAVOR,)+\
                    ' --define="s_dist %s"' % (DIST,)+\
                    ' %s/x86_64/mdsplus/rpm/repos.spec >/dev/null' % (WORKSPACE,),shell=True,cwd=os.getcwd())
        rpmbuild_status=p.wait()
        if rpmbuild_status != 0:
            print "Error building repository rpm for i686 %s %s. rpmbuild returned status=%d." % (DIST,FLAVOR,rpmbuild_status)
            status="error"
        else:
            status="ok"
    return status
def writeRpmInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
            '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
            '</body>\n</html>\n')
    f.close()

def makeRepoRpmsCommand(args):
    print makeRepoRpms()

def makeRpmsCommand(args):
    DIST=getDist()
    WORKSPACE=getWorkspace()
    FLAVOR=getFlavor()
    DISTPATH=args[2]+"/"+DIST+"/"+FLAVOR+"/"
    for d in ['RPMS','SOURCES','SPECS','SRPMS','EGGS']:
        try:
            os.mkdir("%s%s%s" % (WORKSPACE,os.sep,d))
        except:
            pass
    VERSION=getVersion()
    if FLAVOR=="stable":
        rpmflavor=""
        pythonflavor=""
    else:
        rpmflavor="-"+FLAVOR
        pythonflavor=FLAVOR+"-"
    release=getRelease("kernel")
    if len(checkRelease("kernel")) > 0:
        release=release+1
    specfile="%s/SPECS/mdsplus-%s-%s-%s.spec" % (WORKSPACE,FLAVOR,VERSION,DIST)
    beginRpmSpec(specfile,VERSION,release,rpmflavor)
    need_to_build=False
    updates=dict()
    for pkg in getPackages():
        updates[pkg]=dict()
        updates[pkg]['Update']=False
        RELEASE_TAG=getReleaseTag(pkg)
        updates[pkg]['Release']=getRelease(pkg)
        if RELEASE_TAG is None:
            print "No releases yet for %s mdsplus-%s. Building." % (FLAVOR,pkg)
            updates[pkg]['Update']=True
        else:
            c=checkRelease(pkg)
            if len(c) > 0:
                updates[pkg]['Update']=True
                updates[pkg]['Release']=updates[pkg]['Release']+1
                print "New %s release for mdsplus-%s. Building.\n==========================" % (FLAVOR,pkg)
                for line in c:
                    print line
                print "================================="
#            else:
#                try:
#                    rpmfile="%s/RPMS/x86_64/mdsplus%s-%s-%s-%s.%s.x86_64.rpm" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST)
#                    os.stat(rpmfile)
#                except Exception,e:
#                    print "%s missing. Rebuilding." % (rpmfile,)
#                    updates[pkg]['Update']=True
#                try:
#                    rpmfile="%s/RPMS/i686/mdsplus%s-%s-%s-%s.%s.i686.rpm" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST)
#                    os.stat(rpmfile)
#                except Exception,e:
#                    print "%s missing. Rebuilding." % (rpmfile,)
#                    updates[pkg]['Update']=True
                
        if updates[pkg]['Update']:
            need_to_build=True
        addPkgToRpmSpec(specfile,pkg,updates[pkg]['Release'],DIST,rpmflavor)
    status="ok"
    if need_to_build:
        if False:
            print "%s, Starting to make source tar file" % (str(datetime.datetime.now()),)
            makeSrcTar("%s/SOURCES/mdsplus%s-%s.tar.gz" % (WORKSPACE,rpmflavor,VERSION))
            print "%s, Done making source tar file" % (str(datetime.datetime.now()),)
        print "%s, Starting to build 32-bit rpms" % (str(datetime.datetime.now()),)
        p=subprocess.Popen('tar zcf %s/SOURCES/mdsplus%s-%s.tar.gz --exclude CVS ../mdsplus;' % (WORKSPACE,rpmflavor,VERSION) +\
                    'rpmbuild --target i686-linux' +\
                    ' --buildroot %s/BUILDROOT/i686 -ba' % (WORKSPACE,)+\
                    ' --define="_topdir %s"' % (WORKSPACE,)+\
                    ' --define="_builddir %s/i686/mdsplus"' % (WORKSPACE,)+\
                    ' %s' %(specfile,),shell=True,cwd=os.getcwd())
        rpmbuild_status=p.wait()
        print "%s, Done building 32-bit rpms - status=%d" % (str(datetime.datetime.now()),rpmbuild_status)
        if rpmbuild_status != 0:
            print "Error build i686 %s.%s rpms. rpmbuild returned with status=%d" % (FLAVOR,DIST,rpmbuild_status)
            status="error"
        else:
            print "%s, Starting to sign 32-bit rpms" % (str(datetime.datetime.now()),)
            sstatus=signrpms('i686')
            print "%s, Done signing 32-bit rpms - status=%d" % (str(datetime.datetime.now()),sstatus)
            for pkg in getPackages():
                if updates[pkg]['Update']:
                    writeRpmInfo("%s/RPMS/i686/mdsplus%s-%s-%s-%s.%s.i686" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST))
            print "%s, Starting to build 64-bit rpms" % (str(datetime.datetime.now()),)
            p=subprocess.Popen('rpmbuild --target x86_64-linux'+\
                        ' --buildroot %s/BUILDROOT/x86_64 -ba' % (WORKSPACE,)+\
                        ' --define="_topdir %s"' % (WORKSPACE,)+
                    ' --define="_builddir %s/x86_64/mdsplus"' % (WORKSPACE,)+
                    ' %s' %(specfile,),shell=True,cwd=os.getcwd())
            rpmbuild_status=p.wait()
            print "%s, Done building 64-bit rpms - status=%d" % (str(datetime.datetime.now()),rpmbuild_status)
            if rpmbuild_status != 0:
                status="error"
            else:
                print "%s, Starting to sign 64-bit rpms" % (str(datetime.datetime.now()),)
                sstatus=signrpms('x86_64')
                print "%s, Done signing 64-bit rpms - status=%d" % (str(datetime.datetime.now()),sstatus)
                for pkg in getPackages():
                    if updates[pkg]['Update']:
                        writeRpmInfo("%s/RPMS/x86_64/mdsplus%s-%s-%s-%s.%s.x86_64" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST))

        if updates['python']['Update']:
            p=subprocess.Popen('env MDSPLUS_PYTHON_VERSION="%s%s-%s" python setup.py bdist_egg' % (pythonflavor,VERSION,updates['python']['Release']),shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
            python_status=p.wait()
            if python_status != 0:
                print "Error building MDSplus-%s%s-%s" % (pythonflavor,VERSION,updates['python']['Release'])
            else:
                p=subprocess.Popen('mv dist/* ${WORKSPACE}/../EGGS',shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
                p.wait()
    else:
        print 'All RPMS are up to date'
        status="skip"
    if status=="ok":
        print "Build completed successfully. Checking for new releaseas and tagging the modules"
        for pkg in getPackages():
            print "Checking %s for new release" % (pkg,)
            if updates[pkg]['Update']:
                print "      New release. Tag modules with %s %s %s %s" % (FLAVOR,VERSION,updates[pkg]['Release'],DIST)
                newRelease(pkg,FLAVOR,VERSION,updates[pkg]['Release'],DIST)
            else:
                print "      No changes, skipping"
        print "Updating repository rpms"
        status=makeRepoRpms()
    if status=="error":
        sys.exit(1)
    elif status=="ok":
        try:
            p=subprocess.Popen('createrepo . >/dev/null',shell=True,cwd=WORKSPACE+"/RPMS")
            stat=p.wait()
	    if stat != 0:
              raise Exception("Repository creation return status=%d" % (stat,))
        except Exception,e:
            print "Error creating repo: %s" (e,)
            sys.exit(p.wait())
    if status=='ok':
        p=subprocess.Popen('rsync -a RPMS %s;rsync -a SOURCES %s;rsync -a EGGS %s' % (DISTPATH,DISTPATH,DISTPATH,DISTPATH),shell=True,cwd=WORKSPACE)
        pstat=p.wait()
        if pstat != 0:
	  print "Error copying files to final destination. Does the directory %s exist and is it writable by the account used by this hudson node?" % (DISTPATH,)
        else:
          p=subprocess.Popen('rm -Rf RPMS SOURCES EGGS',shell=True,cwd=WORKSPACE)
          pstat=p.wait()
        sys.exit(pstat)
    sys.exit(0)
