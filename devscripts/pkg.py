from subprocess import Popen,PIPE
import os,sys,tarfile
import datetime

class CvsStatus(object):
    status=None

    def __init__(self):
        if CvsStatus.status is None:
            p=Popen('cvs -q status -Rv',stdout=PIPE,shell=True,cwd=os.getcwd())
            CvsStatus.all=p.stdout.read()
            p.wait()
            CvsStatus.status=CvsStatus.all.split('\n')

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

def getDist():
    if os.name=="nt":
        return "win"
    dist="?"
    try:
        p=Popen('rpmbuild -E "%dist" 2>/dev/null',stdout=PIPE,shell=True)
        dist=p.stdout.read()[1:-1]
        p.wait()
    except:
        pass
    return dist

def getFlavor():
    p=Popen('cvs status configure.in',stdout=PIPE,shell=True,cwd=os.getcwd())
    stat=p.stdout.readline()
    while len(stat) > 0:
        if 'Sticky Tag' in stat:
            if '(none)' in stat:
                flavor='alpha'
            else:
                flavor=stat.split()[2]
        stat=p.stdout.readline()
    p.wait()
    return flavor

def getVersion(flavor=None):
    if flavor is None:
        flavor=FLAVOR
    v="?"
    p=Popen('cvs status -v include/release.h',stdout=PIPE,shell=True,cwd=os.getcwd())
    stat=p.stdout.readline()
    while len(stat) > 0:
        if "pkgver-%s" % (flavor,) in stat:
            v=stat.split()
            v=v[0].split("-")
            v= "%s.%s" % (v[2],v[3])
        stat=p.stdout.readline()
    p.wait()
    return v

def getReleaseTag(package):
    global DIST
    tag=None
    rel="pkgrel-%s-%s-%s-" % (FLAVOR,package,getVersion().replace(".","-"))
    p=Popen('cvs status -v rpm/subpackages/%s 2>%s' % (package,os.devnull),stdout=PIPE,shell=True,cwd=os.getcwd())
    stat=p.stdout.readline()
    major=0
    minor=0
    release=0
    while len(stat) > 0:
        if str(rel) in stat and DIST in stat:
            t=stat.split()[0]
            ts=t.split('-')
            try:
                tmaj=int(ts[3])
                tmin=int(ts[4])
                trel=int(ts[5])
                if (tmaj*1000000 + tmin*10000 + trel) >= (major*1000000 + minor*10000 + release):
                    tag=t
                    major=tmaj
                    minor=tmin
                    release=trel
            except:
                pass
        stat=p.stdout.readline()
    p.wait()
    return tag

def getRelease(package):
    tag=getReleaseTag(package)
    if tag is not None:
      tag=tag.split('-')
      if len(tag) == 7:
        return int(tag[5])
    return 0

def checkRelease(package):
    ans=list()
    tag=getReleaseTag(package)
    if tag is None:
        ans.append("RPM out of date: No release for this package, flavor, and plaform")
    else:
        cs=CvsStatus()
        F=""
        IN_PACKAGE=False
        IN_RELEASE=False
        for line in cs.status:
            if "File:" in line:
                F=""
                IN_PACKAGE=False
            elif "Repository revision" in line:
                if "/Attic/" not in line:
                    sl=line.split()
                    F=sl[3][23:-2]
                    CURREV=sl[2]
            elif "pkg_%s" % package in line:
                if F != "":
                    IN_PACKAGE=True
            elif tag in line:
                IN_RELEASE=True
                sl=line.split()
                RELREV=sl[2][0:-1]
            elif "===" in line:
                if IN_PACKAGE:
                    if not IN_RELEASE:
                        ans.append("%s not in latest rpm" % (F,))
                    elif RELREV != CURREV:
                        RR=RELREV.split(".")
                        CR=CURREV.split(".")
                        if len(RR) >= len(CR) or CURREV[0:len(RELREV)-1] != RELREV:
                            ans.append("%s current %s revision is %s while rpm revision is %s" % (F,FLAVOR,CURREV,RELREV))
                IN_PACKAGE=False
                IN_RELEASE=False
    return ans

def printHelp():
    print "This is going to be help"
    sys.exit(0)
    return

def pkgaddCommand(args):
    pkg=args[2]
    path=args[3]
    try:
        os.stat("%s%srpm/subpackages/%s" % (os.getcwd(),os.sep,pkg,))
    except:
        print "Package %s does not exist!" % (pkg,)
        sys.exit(1)
    p=Popen('cvs -Q tag -F pkg_%s %s >%s 2>&1' % (pkg,path,os.devnull),shell=True,cwd=os.getcwd())
    if p.wait() == 0:
        print "%s added to package %s" % (path,pkg)

def pkgremoveCommand(args):
    pkg=args[2]
    path=args[3]
    try:
        os.stat("%s%srpm/subpackages/%s" % (os.getcwd(),os.sep,pkg,))
    except:
        print "Package %s does not exist!" % (pkg,)
        sys.exit(1)
    p=Popen('cvs -Q tag -d pkg_%s %s >%s 2>&1' % (pkg,path,os.devnull),shell=True,cwd=os.getcwd())
    if p.wait() == 0:
        print "%s added to package %s" % (path,pkg)

def listCommand(args):
    cs=CvsStatus()
    if len(args) >= 3:
        item=args[2]
    else:
        item=""
    if item == "none":
        F=None
        IN_PACKAGE=False
        for line in cs.status:
            if "Repository revision" in line:
                if "/Attic/" not in line:
                    sl=line.split()
                    F=sl[3][23:-2]
                    IN_PACKAGE=False
            if 'pkg_' in line:
                IN_PACKAGE=True
            if "====" in line:
                if F is not None and not IN_PACKAGE:
                    print F
                F=None
        if F is not None and not IN_PACKAGE:
            print F
    elif item == "all":
        F=None
        out=list()
        for line in cs.status:
            if "Repository revision" in line:
                if "/Attic/" not in line:
                    sl=line.split()
                    F=sl[3][23:-2]
                    PKG=None
            if 'pkg_' in line:
                ls=line.split()
                PKG=ls[0][4:]
                if F is not None:
                    out.append("%-15s %s" % (PKG,F))
            if "====" in line:
                if F is not None and PKG is None:
                    out.append("%-15s %s" % ("none",F))
                F=None
        if F is not None and PKG is None:
            out.append("%-15s %s" % ("none",F))
        out.sort()
        for o in out:
            print o
    elif item == "":
        pass
    else:
        pkg=item
        try:
            os.stat("%s%srpm/subpackages/%s" % (os.getcwd(),os.sep,pkg,))
        except:
            print "Package %s does not exist!" % (pkg,)
            sys.exit(1)
        F=None
        out=list()
        for line in cs.status:
            if "Repository revision" in line:
                if "/Attic/" not in line:
                    sl=line.split()
                    F=sl[3][23:-2]
                    PKG="None"
            if 'pkg_' in line:
                ls=line.split()
                PKG=ls[0][4:]
                if PKG==pkg and F is not None:
                    out.append(F)
        out.sort()
        for o in out:
            print o

def getVersionCommand(args):
    if len(args) >= 3:
        print getVersion(args[2])
    else:
        print getVersion()

def getReleaseTagCommand(args):
    print getReleaseTag(args[2])

def getReleaseCommand(args):
    print getRelease(args[2])

def checkReleaseCommand(args):
    ans=checkRelease(args[2])
    if len(ans)==0:
        print "ok"
    else:
        for line in ans:
            print line

def getPackages(includeEmpty=False):
    pkgs=list()
    w=os.walk("%s%srpm%ssubpackages"%(os.getcwd(),os.sep,os.sep))
    path,dirs,files = w.next()
    for pkg in files:
        if includeEmpty:
            pkgs.append(pkg)
        else:
            s=os.stat("%s%srpm%ssubpackages%s%s"%(os.getcwd(),os.sep,os.sep,os.sep,pkg))
            if s.st_size > 0:
                pkgs.append(pkg)
    return pkgs

def getPackagesCommand(args):
    includeEmpty = len(args) >= 3 and args[2]=="includeEmpty"
    for pkg in getPackages(includeEmpty):
        print pkg

def checkReleasesCommand(args):
    for pkg in getPackages():
        ans=checkRelease(pkg)
        if len(ans) == 0:
            print "Package %s is up-to-date" % (pkg)
        else:
            print "Package %s has the following updated modules:" % (pkg,)
            for m in ans:
                print "   %s" % (m,)

def promoteCommand(args):
    flavor=args[2]
    major = len(args) > 3 and args[3] == "major"
    if flavor == "alpha":
        p=Popen('cvs -Q rtag -dB beta mdsplus',shell=True,cwd=os.getcwd())
        if p.wait()!=0:
            print "Error promoting alpha to beta release"
            return
        p=Popen('cvs -Q rtag -bR -r HEAD beta mdsplus',shell=True,cwd=os.getcwd())
        if p.wait()!=0:
            print "Error promoting alpha to beta release"
            return
        flavor="beta"
    elif flavor == "beta":
        p=Popen('cvs -Q rtag -dB stable mdsplus',shell=True,cwd=os.getcwd())
        if p.wait()!=0:
            print "Error promoting beta to stable release"
            return
        p=Popen('cvs -Q rtag -bR -r beta stable mdsplus',shell=True,cwd=os.getcwd())
        if p.wait()!=0:
            print "Error promoting beta to stable release"
            return
        flavor="stable"
    else:
        print "You can only promote alpha or beta releases"
        return
    v=getVersion(flavor).split(".")
    majv=int(v[0])
    minv=int(v[1])
    p=Popen('cvs -Q tag -d pkgver-%s-%d-%d include/release.h' % (flavor,majv,minv),shell=True,cwd=os.getcwd())
    if p.wait()!=0:
        print "Error promoting"
        return
    if major:
        majv=majv+1
        minv=0
    else:
        minv=minv+1
    p=Popen('cvs -Q tag pkgver-%s-%d-%d include/release.h' % (flavor,majv,minv),shell=True,cwd=os.getcwd())
    if p.wait()!=0:
        print "Error promoting"
        return
    for pkg in getPackages():
        p=Popen('cvs status -v rpm/subpackages/%s' % (pkg,),shell=True,stdout=PIPE,cwd=os.getcwd())
        line=p.stdout.readline()
        while len(line) > 0:
            if "pkgrel-%s" % (flavor,) in line:
                rel=line.split()[0]
                p2=Popen('cvs -Q tag -d %s rpm/subpackages/%s' % (rel,pkg),shell=True,cwd=os.getcwd())
                p2.wait()
            line=p.stdout.readline()

def getWorkspace():
    try:
        ans = os.environ['WORKSPACE']
        if os.name == 'nt':
            ans=os.getcwd()
    except:
        print "Environment variable, WORKSPACE, is not defined. This command must only be run by hudson."
        sys.exit(1)
    return ans

def makeSrcTar(file):
    cs=CvsStatus()
    def excludeCVS(file):
        if "/CVS" in file:
            return True
        if len(file) == len("../mdsplus"):
            return False
        if file[len("../mdsplus"):] in cs.all:
            return False
        return True
    tgz=tarfile.open(file,mode="w:gz")
    try:
        tgz.add('../mdsplus',arcname='mdsplus',exclude=excludeCVS)
    except TypeError,e:
        walk=os.walk('../mdsplus')
        for path, dir, files in walk:
            for name in files:
                fname=os.path.join(path,name)
                if not excludeCVS(fname):
                    tgz.add(fname,recursive=False,arcname=fname[3:])
    tgz.close()

def newRelease(pkg,flavor,version,release,dist):
    cs=CvsStatus()
    tagversion=version.replace(".","-")
    newtag="pkgrel-%s-%s-%s-%s-%s" % (flavor,pkg,version.replace(".","-"),release,dist)
    p=Popen('cvs -Q tag -F "%s" rpm/subpackages/%s' % (newtag,pkg),shell=True,cwd=os.getcwd())
    p.wait()
    for line in cs.status:
        if "Repository revision" in line:
            if "/Attic/" not in line:
                sl=line.split()
                F=sl[3][23:-2]
                PKG="None"
        if 'pkg_' in line:
            ls=line.split()
            PKG=ls[0][4:]
            if PKG==pkg and F is not None:
                p=Popen('cvs -Q tag -F "%s" %s' % (newtag,F),shell=True,cwd=os.getcwd())
                p.wait()

def newReleaseCommand(args):
    newRelease(args[2],args[3],args[4],args[5],args[6])

def makeSrcTarCommand(args):
    makeSrcTar(args[2])

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
    p=Popen('rpmbuild -ba' +\
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
        p=Popen('rpmbuild -ba'+\
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

def makeRepoRpmsCommand(args):
    print makeRepoRpms()

def makeRpmsCommand(args):
    global DIST
    WORKSPACE=getWorkspace()
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
            else:
                try:
                    rpmfile="%s/RPMS/x86_64/mdsplus%s-%s-%s-%s.%s.x86_64.rpm" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST)
                    os.stat(rpmfile)
                except:
                    print "%s missing. Rebuilding." % (rpmfile)
                    updates[pkg]['Update']=True
                try:
                    rpmfile="%s/RPMS/i686/mdsplus%s-%s-%s-%s.%s.i686.rpm" % (WORKSPACE,rpmflavor,pkg,VERSION,updates[pkg]['Release'],DIST)
                    os.stat(rpmfile)
                except:
                    print "%s missing. Rebuilding." % (rpmfile)
                    updates[pkg]['Update']=True
                
        if updates[pkg]['Update']:
            need_to_build=True
        addPkgToRpmSpec(specfile,pkg,updates[pkg]['Release'],DIST,rpmflavor)
    status="ok"
    if need_to_build:
        print "%s, Starting to make source tar file" % (str(datetime.datetime.now()),)
        makeSrcTar("%s/SOURCES/mdsplus%s-%s.tar.gz" % (WORKSPACE,rpmflavor,VERSION))
        print "%s, Done making source tar file" % (str(datetime.datetime.now()),)
        print "%s, Starting to build 32-bit rpms" % (str(datetime.datetime.now()),)
        p=Popen('rpmbuild --target i686-linux' +\
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
            print "%s, Starting to build 64-bit rpms" % (str(datetime.datetime.now()),)
            p=Popen('rpmbuild --target x86_64-linux'+\
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

        if updates['python']['Update']:
            p=Popen('env MDSPLUS_PYTHON_VERSION="%s%s-%s" python setup.py bdist_egg' % (pythonflavor,VERSION,updates['python']['Release']),shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
            python_status=p.wait()
            if python_status != 0:
                print "Error building MDSplus-%s%s-%s" % (pythonflavor,VERSION,updates['python']['Release'])
            else:
                p=Popen('mv dist/* ${WORKSPACE}/../EGGS',shell=True,cwd="%s/x86_64/mdsplus/mdsobjects/python"%(WORKSPACE))
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
    else:
        try:
            p=Popen('createrepo . >/dev/null',shell=True,cwd=WORKSPACE+"/RPMS")
        except Exception,e:
            print "Error creating repo: %s" (e,)
        sys.exit(p.wait())

def msiUpdateSetup(VERSION,release,bits,outfile):
    os.rename('Setup/Setup%d.vdproj' % (bits,),'Setup/Setup%d.vdproj-orig' % (bits,))
    try:
        os.stat(outfile+".uuid")
    except:
        p=Popen("uuidgen > %s.uuid" % (outfile,),shell=True)
        p.wait()
    u_in=open("%s.uuid" % (outfile,))
    uuid=u_in.readline()
    u_in.close()
    if bits == 32:
        setupdir="x86"
    else:
        setupdir="x86_64"
    f_in=open('Setup/Setup%d.vdproj-orig' %(bits,),'r')
    f_out=open('Setup/Setup%d.vdproj' %(bits,),'w')
    line=f_in.readline()
    while len(line) > 0:
        if "Product Name" in line:
            s=line.split(':')
            line=s[0]+':'+"8:MDSplus - "+FLAVOR
        if "ProductCode" in line:
            line=s[0]+':'+ "{" + uuid + "}"
        if "ProductVersion" in line:
            s=line.split(':')
            line=s[0]+':'+VERSION+'-'+str(release)
        if "OutputFilename" in line:
            s=line.split(':')
            line=s[0]+':'+outfile+".msi"
        if "PostBuildEvent" in line:
            setup="%s/msi/%s/Setup.exe" % (WORKSPACE,setupdir)
            s=line.split(':')
            line=s[0]+'::\"$(ProjectDir)..\\devscripts\\sign_kit.bat\" \"%s\" \"$(BuiltOuputPath)\"\r\n\r\n"' % (setup,)
        if '"Url"' in line:
            s=line.split(':')
            line=s[0]+':http://www.mdsplus.org/msi/%s"' % (setupdir,)
        f_out.write(line)
        line=f_in.readline()
    f_in.close()
    f_out.close()

def makeMsiCommand(args):
    WORKSPACE=getWorkspace()
    VERSION=getVersion()
    if FLAVOR=="stable":
        msiflavor=""
        pythonflavor=""
    else:
        msiflavor="-"+FLAVOR
        pythonflavor=FLAVOR+"-"
    release=getRelease("windows")
    need_to_build=False
    if len(checkRelease("windows")) > 0:
        need_to_build=True
        release=release+1
    msi32="%s/x86/MDSplus%s-%s.%s" % (WORKSPACE,msiflavor,VERSION,release)
    msi64="%s/x86_64/MDSplus%s-%s.%s" % (WORKSPACE,msiflavor,VERSION,release)
    if not need_to_build:
        try:
            os.stat(msi32+".msi")
            try:
                os.stat(msi64+".msi")
            except:
                print '%s missing. Rebuilding.' % (msi64,)
                need_to_build=True
        except:
            print '%s missing. Rebuilding.' % (msi32,)
            need_to_build=True
    status="ok"
    if need_to_build:
        print "%s, Starting build java" % (str(datetime.datetime.now()),)
        p=Popen('devenv /build "Release|Java" mdsplus.sln',shell=True,cwd=WORKSPACE+"/mdsplus")
        stat=p.wait()
        print "%s, Java build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (status != 0):
            print "Build failed!"
            sys.exit(stat)
        print "%s, Starting to build 32-bit apps" % (str(datetime.datetime.now()),)
        p=Popen('devenv /build "Release|Win32" mdsplus.sln',shell=True,cwd=WORKSPACE+"/mdsplus")
        stat=p.wait()
        print "%s, 32-bit apps build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (status != 0):
            print "Build failed!"
            sys.exit(stat)
        print "%s, Starting to build 64-bit apps" % (str(datetime.datetime.now()),)
        p=Popen('devenv /build "Release|x64" mdsplus.sln',shell=True,cwd=WORKSPACE+"/mdsplus")
        stat=p.wait()
        print "%s, 64-bit apps build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (status != 0):
            print "Build failed!"
            sys.exit(stat)
        msiUpdateSetup(WORKSPACE,msiflavor,VERSION,release,changed,32,msi32)
        print "%s, Starting to build 32-bit setup kit" % (str(datetime.datetime.now()),)
        p=Popen('devenv /build "Release|Setup32" mdsplus.sln',shell=True,cwd=WORKSPACE+"/mdsplus")
        stat=p.wait()
        print "%s, 32-bit setup kit build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (status != 0):
            print "Build failed!"
            sys.exit(stat)
        msiUpdateSetup(WORKSPACE,msiflavor,VERSION,release,changed,64,msi64)
        print "%s, Starting to build 64-bit setup kit" % (str(datetime.datetime.now()),)
        p=Popen('devenv /build "Release|Setup64" mdsplus.sln',shell=True,cwd=WORKSPACE+"/mdsplus")
        stat=p.wait()
        print "%s, 64-bit setup kit build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (status != 0):
            print "Build failed!"
            sys.exit(stat)



if __name__ == "__main__":
    if os.path.dirname(sys.argv[0]) != '':
      os.chdir(os.path.dirname(sys.argv[0]))
    os.chdir("..")
    DIST=getDist()
    FLAVOR=getFlavor()
    if len(sys.argv) < 2:
        printHelp()
    else:
        try:
            rtn=eval("%sCommand" %(sys.argv[1],),globals())
        except NameError,e:
            print "Invalid command"
            printHelp()
        rtn(sys.argv)

