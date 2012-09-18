import subprocess,datetime,os,sys
from pkg_utils import getDist, getWorkspace, getFlavor, getVersion, getRelease, getReleaseTag, checkRelease, getPackages, newRelease, getTopDir
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

def beginRpmSpec(specfile,version,release,rpmflavor,pythonflavor):
    f_in=open('%s/rpm/mdsplus-part1.spec' % (getTopDir(),),'r')
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
    f_in=open('%s/rpm/mdsplus-part2-hudson.spec' % (getTopDir(),),"r")
    line=f_in.readline()
    while len(line) > 0:
        line=line.replace("--VERSION--",version)
        line=line.replace("--RELEASE--",str(release))
        line=line.replace("--PYTHONFLAVOR--",pythonflavor)
        f_out.write(line)
        line=f_in.readline()
    f_in.close()
    f_out.close()

def addPkgToRpmSpec(specfile,pkg,release,DIST,rpmflavor):
    f_in=open('%s/rpm/subpackages/%s' % (getTopDir(),pkg,),'r')
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
    if FLAVOR == "stable":
      rpmflavor=""
    else:
      rpmflavor="-"+FLAVOR
    sys.stdout.flush()
    p=subprocess.Popen('rpmbuild -ba' +\
                ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                ' --define="_topdir %s"' % (WORKSPACE,)+\
                ' --define="_builddir %s"' % (WORKSPACE,)+\
                ' --define="flavor %s"' % (FLAVOR,)+\
                ' --define="rpmflavor %s"' % (rpmflavor,)+\
                ' --define="s_dist %s"' % (DIST,)+\
                ' %s/x86_64/mdsplus/rpm/repos.spec >/dev/null' % (WORKSPACE,),shell=True,cwd=getTopDir())
    rpmbuild_status=p.wait()
    if rpmbuild_status != 0:
        print "Error building repository rpm for x86_64 %s %s. rpmbuild returned status=%d." % (DIST,FLAVOR,rpmbuild_status)
        status="error"
    else:
        sys.stdout.flush()
        p=subprocess.Popen('rpmbuild -ba'+\
                    ' --target=i686-linux'+\
                    ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                    ' --define="_topdir %s"' % (WORKSPACE,)+\
                    ' --define="_builddir %s"' % (WORKSPACE,)+\
                    ' --define="flavor %s"' % (FLAVOR,)+\
                    ' --define="rpmflavor %s"' % (rpmflavor,)+\
                    ' --define="s_dist %s"' % (DIST,)+\
                    ' %s/x86_64/mdsplus/rpm/repos.spec >/dev/null' % (WORKSPACE,),shell=True,cwd=getTopDir())
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
    """Internal"""
    print makeRepoRpms()

def makeRpmsCommand(args):
    """Make Redhat distributions rpms."""
    DIST=getDist()
    WORKSPACE=getWorkspace()
    FLAVOR=getFlavor()
    DISTPATH=args[2]+"/"+DIST+"/"+FLAVOR+"/"
    for d in ['RPMS','SOURCES','SPECS','SRPMS','EGGS']:
        try:
            os.mkdir("%s/%s" % (WORKSPACE,d))
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
    beginRpmSpec(specfile,VERSION,release,rpmflavor,pythonflavor)
    need_to_build=False
    need_changelog=False
    updates=dict()
    for pkg in getPackages():
        updates[pkg]=dict()
        updates[pkg]['Update']=False
        updates[pkg]['Tag']=False
        RELEASE_TAG=getReleaseTag(pkg)
        updates[pkg]['Release']=getRelease(pkg)
        if RELEASE_TAG is None:
            print "No releases yet for %s mdsplus-%s. Building." % (FLAVOR,pkg)
            updates[pkg]['Update']=True
            updates[pkg]['Tag']=True
            need_changelog=True
        else:
            c=checkRelease(pkg)
            if len(c) > 0:
                updates[pkg]['Tag']=True
                updates[pkg]['Update']=True
                updates[pkg]['Release']=updates[pkg]['Release']+1
                print "New %s release for mdsplus-%s. Building.\n==========================" % (FLAVOR,pkg)
                for line in c:
                    print line
                print "================================="
                need_changelog=True
            else:
                for p in ('x86_64','i686'):
                  try:
                    rpm="/mnt/dist/%s/RPMS/%s/mdsplus%s-%s-%s-%s.%s.%s.rpm" % (FLAVOR,p,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST,p)
		    os.stat(rpm)
                  except:
                    print "%s missing, rebuilding" % (rpm)
                    updates[pkg]['Update']=True
        if updates[pkg]['Update']:
            need_to_build=True
        addPkgToRpmSpec(specfile,pkg,updates[pkg]['Release'],DIST,rpmflavor)
    status="ok"
    if need_to_build:
        if 'UPDATE_CHANGELOG' in os.environ and need_changelog:
            print "Updating ChangeLog"
            sys.stdout.flush()
            p=subprocess.Popen('$(pwd)/devscripts/UpdateChangeLog %s' % (FLAVOR,),shell=True,cwd=getTopDir())
            p.wait()
        print "%s, Starting to build 32-bit rpms" % (str(datetime.datetime.now()),)
        sys.stdout.flush()
        p=subprocess.Popen('export MDSPLUS_PYTHON_VERSION="%s%s-%s";' % (pythonflavor,VERSION,updates['python']['Release']) +\
                    'scp alchome.psfc.mit.edu:/mnt/scratch/mdsplus/rpm-signing-keys.tgz ~/;tar xfC ~/rpm-signing-keys.tgz ~;' +\
                    'rm -Rf %s/RPMS/*;' % (WORKSPACE,) +\
                    'rpmbuild --target i686-linux' +\
                    ' --buildroot %s/BUILDROOT/i686 -ba' % (WORKSPACE,)+\
                    ' --define="_topdir %s"' % (WORKSPACE,)+\
                    ' --define="_builddir %s/i686/mdsplus"' % (WORKSPACE,)+\
                    ' %s' %(specfile,),shell=True,cwd=getTopDir())
        rpmbuild_status=p.wait()
        print "%s, Done building 32-bit rpms - status=%d" % (str(datetime.datetime.now()),rpmbuild_status)
        if rpmbuild_status != 0:
            print "Error build i686 %s.%s rpms. rpmbuild returned with status=%d" % (FLAVOR,DIST,rpmbuild_status)
            status="error"
        else:
            print "%s, Starting to sign 32-bit rpms" % (str(datetime.datetime.now()),)
            sstatus=signrpms('i686')
            print "%s, Done signing 32-bit rpms - status=%d" % (str(datetime.datetime.now()),sstatus)
            if sstatus != 0:
		print "Error signing package"
                sys.exit(1)
            for pkg in getPackages():
                if updates[pkg]['Update']:
                    writeRpmInfo("%s/RPMS/i686/mdsplus%s-%s-%s-%s.%s.i686" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST))
            print "%s, Starting to build 64-bit rpms" % (str(datetime.datetime.now()),)
            sys.stdout.flush()
            p=subprocess.Popen('rpmbuild --target x86_64-linux'+\
                        ' --buildroot %s/BUILDROOT/x86_64 -ba' % (WORKSPACE,)+\
                        ' --define="_topdir %s"' % (WORKSPACE,)+
                    ' --define="_builddir %s/x86_64/mdsplus"' % (WORKSPACE,)+
                    ' %s' %(specfile,),shell=True,cwd=getTopDir())
            rpmbuild_status=p.wait()
            print "%s, Done building 64-bit rpms - status=%d" % (str(datetime.datetime.now()),rpmbuild_status)
            if rpmbuild_status != 0:
                status="error"
            else:
                print "%s, Starting to sign 64-bit rpms" % (str(datetime.datetime.now()),)
                sstatus=signrpms('x86_64')
                if sstatus != 0:
                  print "Error signing rpm"
                  sys.exit(sstatus)
                print "%s, Done signing 64-bit rpms - status=%d" % (str(datetime.datetime.now()),sstatus)
                for pkg in getPackages():
                    if updates[pkg]['Update']:
                        writeRpmInfo("%s/RPMS/x86_64/mdsplus%s-%s-%s-%s.%s.x86_64" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST))
    else:
        print 'All RPMS are up to date'
        status="skip"
    if status=="ok":
        print "Updating repository rpms"
        status=makeRepoRpms()
    if status=="ok":
        try:
            sys.stdout.flush()
            p=subprocess.Popen('createrepo . >/dev/null',shell=True,cwd=WORKSPACE+"/RPMS")
            stat=p.wait()
	    if stat != 0:
              raise Exception("Repository creation return status=%d" % (stat,))
        except Exception,e:
            print "Error creating repo: %s" (e,)
            sys.exit(p.wait())
        sys.path.insert(0,WORKSPACE+'/x86_64/mdsplus/tests')
        from distribution_tests import test_rpms as test
        test(WORKSPACE,FLAVOR)
        sys.stdout.flush()
        try:
	  os.stat("%s/source" % (WORKSPACE,))
          dotar=True
        except:
          dotar=False
        if dotar:
          p=subprocess.Popen('tar zcf ../SOURCES/mdsplus%s-%s.tar.gz --exclude CVS mdsplus' % (rpmflavor,VERSION),shell=True,cwd="%s/source" % (WORKSPACE,))
          pstat=p.wait()
          if pstat != 0:
            print "Error creating source tarball"
            sys.exit(1)

        p=subprocess.Popen('rsync -av RPMS %s;rsync -av SOURCES %s;rsync -av ./i686/mdsplus/mdsobjects/python/dist/*.egg %s/EGGS/' % (DISTPATH,DISTPATH,DISTPATH),shell=True,cwd=WORKSPACE)
        pstat=p.wait()
        if pstat != 0:
            print "Error copying files to final destination. Does the directory %s exist and is it writable by the account used by this hudson node?" % (DISTPATH,)
            sys.exit(pstat)
        else:
            sys.stdout.flush()
            p=subprocess.Popen('rm -Rf SOURCES EGGS',shell=True,cwd=WORKSPACE)
            pstat=p.wait()
        print "Build completed successfully. Checking for new releaseas and tagging the modules"
        for pkg in getPackages():
            print "Checking %s for new release" % (pkg,)
            if updates[pkg]['Tag']:
                print "      New release. Tag modules with %s %s %s %s" % (FLAVOR,VERSION,updates[pkg]['Release'],DIST)
                newRelease(pkg,FLAVOR,VERSION,updates[pkg]['Release'],DIST)
            else:
                print "      No changes, skipping"
    if status=="error":
        sys.exit(1)
    sys.exit(0)
