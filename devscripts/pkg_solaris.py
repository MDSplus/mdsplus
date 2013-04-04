import subprocess,datetime,os,sys,shutil
from pkg_utils import getDist, getWorkspace, getFlavor, getVersion, getRelease, getReleaseTag, checkRelease, getPackages as getAllPackages, newRelease, getHardwarePlatform, getTopDir
from pkg_rpms import writeRpmInfo

def getPackages():
    pkgs=getAllPackages()
    ans=list()
    for pkg in pkgs:
      if pkg!='gsi':
	ans.append(pkg)
    return ans

def writePkgInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
            '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
            '</body>\n</html>\n')
    f.close()

def createPkg(WORKSPACE,FLAVOR,pkg,VERSION,release,arch,DIST,repo):
    sys.stdout.flush()
    p=subprocess.Popen('%s/x86_64/mdsplus/devscripts/makeSolarisRepoPkg %s %s %s %d %s %s %s' % (WORKSPACE,FLAVOR,pkg,VERSION,release,arch,DIST,repo),shell=True)
    return p.wait()

def getPkgFile(arch,dist,pkgflavor,pkg,VERSION,updates):
    return "mdsplus%s-%s_%s-%d_%s.%s.%s.pkg" % (pkgflavor,pkg,VERSION,updates[pkg]['Release'],dist,os.uname()[4],arch)

def makeSolarisPkgsCommand(args):
    """Make solaris distribution packages."""
    DIST=getDist()
    WORKSPACE=getWorkspace()
    FLAVOR=getFlavor()
    need_to_build=len(args) > 3
    for arch in ('x86_64','i686'):
        try:
            shutil.rmtree('%s/%s/BUILDROOT' % (WORKSPACE,arch))
        except:
            pass
    for d in ['x86_64/BUILDROOT','i686/BUILDROOT','EGGS','bin','x86_64/PKG','i686/PKG']:
        try:
            os.mkdir("%s%s%s" % (WORKSPACE,os.sep,d))
        except:
            pass
    for app in ('tar','make','find'):
        try:
            os.symlink('/usr/bin/g%s' % (app,),'%s/bin/%s' % (WORKSPACE,app))
        except Exception,e:
            print "Error making app symlink: %s" % (e,)
            pass
    os.environ['PATH']='%s/bin:' % (WORKSPACE,)+os.environ['PATH']
    VERSION=getVersion()
    if FLAVOR=="stable":
        pkgflavor=""
        pythonflavor=""
    else:
        pkgflavor="-"+FLAVOR
        pythonflavor=FLAVOR+"-"
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
        else:
            c=checkRelease(pkg)
            if len(c) > 0:
                updates[pkg]['Update']=True
		updates[pkg]['Tag']=True
                updates[pkg]['Release']=updates[pkg]['Release']+1
                print "New %s release for mdsplus-%s. Building.\n==========================" % (FLAVOR,pkg)
                for line in c:
                    print line
                print "================================="
            else:
                for arch in ('i686','x86_64'):
                  pkgfile='%s/%s/PKG/%s' % (WORKSPACE,arch,getPkgFile(arch,DIST,pkgflavor,pkg,VERSION,updates))
                  try:
                      os.stat(pkgfile)
                  except Exception,e:
                      print "%s missing. Rebuilding." % (pkgfile,)
                      updates[pkg]['Update']=True
        if updates[pkg]['Update']:
            need_to_build=True
    status="ok"
    if need_to_build:
        for pkg in getPackages():
          for arch in ('i686','x86_64'):
            try:
              os.unlink('%s/%s/PKG/%s' % (WORKSPACE,arch,getPkgFile(arch,DIST,pkgflavor,pkg,VERSION,updates)))
            except:
              pass
        prefix32="%s/i686/BUILDROOT/usr/local/mdsplus" %(WORKSPACE,)
        cmd='echo $PATH; cd ${WORKSPACE}/i686/mdsplus;' +\
             'export MDSPLUS_PYTHON_VERSION="%s%s-%s";' % (pythonflavor,VERSION,updates['python']['Release']) +\
            './configure --enable-mdsip_connections --enable-nodebug ' +\
            '--with-jdk=$JDK_DIR --with-idl=$IDL_DIR --exec-prefix=%s --prefix=%s;' % (prefix32,prefix32) +\
             'if (! make clean ); then exit 1; fi; if ( ! make ); then exit 1; fi; if ( ! make install ); then exit 1; fi;' +\
             'olddir=$(pwd);' +\
             'cd mdsobjects/python;' +\
             'rm -Rf dist;' +\
             'if ( ! python setup.py bdist_egg); then exit 1; fi;' +\
             'mkdir -p %s/mdsobjects/python;' % (prefix32,) +\
             'rsync -a dist %s/mdsobjects/python/;' % (prefix32,) +\
             'cd $olddir'
        sys.stdout.flush()
        p=subprocess.Popen(cmd,shell=True,cwd=getTopDir())
        build_status=p.wait()
        print "%s, Done building i686 parts - status=%d" % (str(datetime.datetime.now()),build_status)
        if build_status != 0:
            print "Error building mdsplus. Status=%d" % (build_status,)
            status="error"
            sys.exit(1)
        prefix64="%s/x86_64/BUILDROOT/usr/local/mdsplus" % (WORKSPACE,)
        cmd='cd ${WORKSPACE}/x86_64/mdsplus;'
        cmd=cmd+'./configure --enable-mdsip_connections --disable-java --with-idl=$IDL_DIR'
        cmd=cmd+'--exec-prefix=%s --prefix=%s CFLAGS="-m64" FFLAGS="-m64";' % (prefix64,prefix64)
        cmd=cmd+'if (! make ); then exit 1; fi; if (! make install); then exit 1; fi;'
        cmd=cmd+'mv %s/lib %s/lib64; mv %s/bin %s/bin64;' % (prefix64,prefix64,prefix64,prefix64)
        cmd=cmd+'rsync -a %s/lib/* %s/lib32;' % (prefix32,prefix64)
        cmd=cmd+'rsync -a %s/bin/* %s/bin32;' % (prefix32,prefix64)
        cmd=cmd+'rsync -a %s/java %s/;' % (prefix32,prefix64)
        cmd=cmd+'rsync -a %s/mdsobjects %s/' % (prefix32,prefix64)
        sys.stdout.flush()
        p=subprocess.Popen(cmd,shell=True,cwd=getTopDir())
        build_status=p.wait()
        print "%s, Done building x86_64 parts - status=%d" % (str(datetime.datetime.now()),build_status)
        if build_status != 0:
            print "Error building mdsplus. Status=%d" % (build_status,)
            status="error"
            sys.exit(1)
        for pkg in getPackages():
            for arch in ('x86_64','i686'):
                build_status=createPkg(WORKSPACE,FLAVOR,pkg,VERSION,updates[pkg]['Release'],arch,DIST,'~/repo-for-testing-%s' % (arch))
                if build_status != 0:
                    print "Error building %s package for %s" % (arch,pkg)
                    sys.exit(1)

        if updates['python']['Update']:
            sys.stdout.flush()     
            p=subprocess.Popen('env MDSPLUS_PYTHON_VERSION="%s%s-%s" python setup.py bdist_egg' % 
                               (pythonflavor,VERSION,updates['python']['Release']),
                               shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
            python_status=p.wait()
            if python_status != 0:
                print "Error building MDSplus-%s%s-%s" % (pythonflavor,VERSION,updates['python']['Release'])
            else:
                sys.stdout.flush()
                p=subprocess.Popen('mv dist/* %s/EGGS/;rm -Rf dist'%(WORKSPACE,),shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
                p.wait()
    else:
        print 'All packages are up to date'
        status="skip"
    if status=="ok":
        sys.path.insert(0,WORKSPACE+'/x86_64/mdsplus/tests')
        from distribution_tests import test_solaris as test
        test(WORKSPACE,FLAVOR)
        print "Build and testing completed successfully. Updating packages in public repositories."
        sys.stdout.flush()
        for pkg in getPackages():
            for arch in ('x86_64','i686'):
                build_status=createPkg(WORKSPACE,FLAVOR,pkg,VERSION,updates[pkg]['Release'],arch,DIST,'~/mdsplus-repo-%s' % (arch))
                if build_status != 0:
                    print "Error building %s package for %s" % (arch,pkg)
                    sys.exit(1)
                else:
                  os.close(os.open('%s/%s/PKG/%s' % (WORKSPACE,arch,getPkgFile(arch,DIST,pkgflavor,pkg,VERSION,updates)),os.O_CREAT))
        p=subprocess.Popen('devscripts/solarisServeRestart',shell=True,cwd="%s/x86_64/mdsplus"%(WORKSPACE))
	p.wait()
        print "Tagging modules with new releases."
        sys.stdout.flush()
        for pkg in getPackages():
            if updates[pkg]['Tag']:
                print "New release. Tag %s modules with %s %s %s %s" % (pkg,FLAVOR,VERSION,updates[pkg]['Release'],DIST)
                sys.stdout.flush()
                newRelease(pkg,FLAVOR,VERSION,updates[pkg]['Release'],DIST)
    if status=="error":
        sys.exit(1)
