import subprocess,sys,tarfile,os

def getTopDir():
  return os.path.dirname(os.path.abspath(__file__+'/../'))

def shell(cwd,cmd,msg):
  print cmd
  sys.stdout.flush()
  p=subprocess.Popen(cmd,shell=True,cwd=cwd)
  stat=p.wait()
  if stat != 0:
    print msg
    sys.exit(stat)

class CvsStatus(object):
    status=None

    def __init__(self):
        if CvsStatus.status is None:
            p=subprocess.Popen('cvs -q status -Rv',stdout=subprocess.PIPE,shell=True,cwd=getTopDir())
            CvsStatus.all=p.stdout.read()
            p.wait()
            CvsStatus.status=CvsStatus.all.split('\n')

def getDist():
    if os.name=="nt":
        return "win"
    elif os.uname()[0]=='SunOS':
        return os.uname()[0]+"-"+os.uname()[3].split('.')[0]
    elif os.uname()[0]=='Linux':
        if 'Ubuntu' in os.uname()[3]:
            return getLsbReleaseDist()+os.uname()[4]
        parts=os.uname()[2].split('.')
        for p in parts:
            if p.startswith('el'):
                return p
            if p.startswith('fc'):
                return p
    elif os.uname()[0]=='Darwin':
        return 'macosx'
    print "Error getting distribution information, uname=%s" % (str(os.uname()),)
    sys.exit(1)

def getLsbReleaseDist():
    p=subprocess.Popen('lsb_release -a -s 2>/dev/null',stdout=subprocess.PIPE,shell=True)
    info=p.stdout.readlines()
    p.wait()
    platform=info[0][0:-1]
    version=info[2][0:-1].split('.')[0]
    return platform+version

def getHardwarePlatform():
    p=subprocess.Popen('uname -p',stdout=subprocess.PIPE,shell=True)
    hp=p.stdout.readline()[0:-1]
    p.wait()
    if hp=='x86_64':
        bits=64
    else:
        bits=32
    return (hp,bits)


def getFlavor():
    p=subprocess.Popen('cvs status configure.in',stdout=subprocess.PIPE,shell=True,cwd=getTopDir())
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
        flavor=getFlavor()
    v="?"
    p=subprocess.Popen('cvs status -v include/release.h',stdout=subprocess.PIPE,shell=True,cwd=getTopDir())
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
    DIST=getDist()
    FLAVOR=getFlavor()
    tag=None
    rel="pkgrel-%s-%s-%s-" % (FLAVOR,package,getVersion().replace(".","-"))
    p=subprocess.Popen('cvs status -v rpm/subpackages/%s 2>%s' % (package,os.devnull),stdout=subprocess.PIPE,shell=True,cwd=getTopDir())
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
    print "Checking release of %s" % (package,)
    FLAVOR=getFlavor()
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
                    F=' '.join(sl[3:])[23:-2]
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
                        ans.append("%s not in latest release" % (F,))
                    elif RELREV != CURREV:
                        RR=RELREV.split(".")
                        CR=CURREV.split(".")
                        if len(RR) >= len(CR) or CURREV[0:len(RELREV)-1] != RELREV:
                            ans.append("%s current %s revision is %s while rpm revision is %s" % (F,FLAVOR,CURREV,RELREV))
                IN_PACKAGE=False
                IN_RELEASE=False
    if len(ans)==0:
	print "Package %s is up to date" % (package,)
    else:
        print "Package %s needs updating: " % (package,)
        for l in ans:
          print l
    return ans
def promoteCommand(args):
    """Promote the alpha release to beta or beta release to stable.

Format: promote alpha|beta

"""
    if len(args) < 3:
      print "Specify either alpha or beta for first argument to the promote commannd.\nFor more information use help promote."
      sys.exit(1)
    flavor=args[2]
    if flavor == "alpha":
        p=subprocess.Popen('cvs -Q rtag -dB beta mdsplus',shell=True,cwd=getTopDir())
        if p.wait()!=0:
            print "Error promoting alpha to beta release"
            return
        p=subprocess.Popen('cvs -Q rtag -bR -r HEAD beta mdsplus',shell=True,cwd=getTopDir())
        if p.wait()!=0:
            print "Error promoting alpha to beta release"
            return
        flavor="beta"
    elif flavor == "beta":
        p=subprocess.Popen('cvs -Q rtag -dB stable mdsplus',shell=True,cwd=getTopDir())
        if p.wait()!=0:
            print "Error promoting beta to stable release"
            return
        p=subprocess.Popen('cvs -Q rtag -bR -r beta stable mdsplus',shell=True,cwd=getTopDir())
        if p.wait()!=0:
            print "Error promoting beta to stable release"
            return
        flavor="stable"
    else:
        print "You can only promote alpha or beta releases"
        return

def newVersionCommand(args):
    """Change base version number of a release.

Format: newVersion alpha|beta|stable major|minor|version-number

Examples:

    python pkg.py newVersion alpha minor

      If the current alpha version was 3.1 the alpha version would be changed to 3.2

    python pkg.py newVersion beta major

      If the current beta version was 2.5 then the new beta version would be changed to 3.0

    python pkg.py newVersion stable 3.6

      The new stable version would be set to 3.6
"""
    flavor=args[2]
    if len(args) <= 3 or (len(args) > 3 and args[3] == "skip"):
	sys.exit(0)
    if flavor in ('alpha','beta','stable'):
      major = len(args) > 3 and args[3] == "major"
      version=getVersion(flavor)
      v=version.split('.')
      majv=int(v[0])
      minv=int(v[1])
      p=subprocess.Popen('cvs -Q tag -d pkgver-%s-%d-%d include/release.h' % (flavor,majv,minv),shell=True,cwd=getTopDir())
      if p.wait()!=0:
        print "Error deleting old version tags"
        return
      if major:
        majv=majv+1
        minv=0
      elif args[3]=="minor":
        minv=minv+1
      else:
	nversion=args[3]
        try:
          nv=float(nversion)
          nv=nversion.split('.')
          majv=int(nv[0])
          minv=int(nv[1])
        except:
	  print "version must be one of 'major', 'minor' or nnn.n"
          sys.exit(1)
      p=subprocess.Popen('cvs -Q tag pkgver-%s-%d-%d include/release.h' % (flavor,majv,minv),shell=True,cwd=getTopDir())
      if p.wait()!=0:
        print "Error changing version"
        return
      for pkg in getPackages():
        p=subprocess.Popen('cvs status -v rpm/subpackages/%s' % (pkg,),shell=True,stdout=subprocess.PIPE,cwd=getTopDir())
        line=p.stdout.readline()
        while len(line) > 0:
            if "pkgrel-%s" % (flavor,) in line:
                rel=line.split()[0]
                p2=subprocess.Popen('cvs -Q tag -d %s rpm/subpackages/%s' % (rel,pkg),shell=True,cwd=getTopDir())
                p2.wait()
            line=p.stdout.readline()
        p.wait()
    else:
	print "Invalid flavor /%s/. Specify alpha, beta or stable." % (flavor,)

def getWorkspace():
    try:
        ans = os.environ['WORKSPACE']
    except:
        print "Environment variable, WORKSPACE, is not defined. This command must only be run by hudson."
        sys.exit(1)
    return ans

def newRelease(pkg,flavor,version,release,dist):
    cs=CvsStatus()
    tagversion=version.replace(".","-")
    newtag="pkgrel-%s-%s-%s-%s-%s" % (flavor,pkg,version.replace(".","-"),release,dist)
    p=subprocess.Popen('cvs -Q tag -F "%s" rpm/subpackages/%s' % (newtag,pkg),shell=True,cwd=getTopDir())
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
                if not sys.platform.startswith('win'):
                    F=F.replace('$','\\$')
                p=subprocess.Popen('cvs -Q tag -F "%s" "%s"' % (newtag,F),shell=True,cwd=getTopDir())
                stat=p.wait()
		if stat != 0:
		    print "Error tagging with command: %s" % ('cvs -Q tag -F "%s" "%s"' % (newtag,F),)

def getPackages(includeEmpty=False):
    pkgs=list()
    w=os.walk("%s%srpm%ssubpackages"%(getTopDir(),os.sep,os.sep))
    path,dirs,files = w.next()
    for pkg in files:
        if includeEmpty:
            pkgs.append(pkg)
        else:
            s=os.stat("%s%srpm%ssubpackages%s%s"%(getTopDir(),os.sep,os.sep,os.sep,pkg))
            if s.st_size > 0:
                pkgs.append(pkg)
    return pkgs

def newReleaseCommand(args):
    """Internal"""
    newRelease(args[2],args[3],args[4],args[5],args[6])

def getPackagesCommand(args):
    """List all package names.

Format:

    getPackages [includeEmpty]
"""
    includeEmpty = len(args) >= 3 and args[2]=="includeEmpty"
    for pkg in getPackages(includeEmpty):
        print pkg

def checkReleasesCommand(args):
    """Check to see if any changes where made to any package sources since the last published release of that package
Format:

    pkg.py checkRelease
"""
    for pkg in getPackages():
        ans=checkRelease(pkg)
        if len(ans) == 0:
            print "Package %s is up-to-date" % (pkg)
        else:
            print "Package %s has the following updated modules:" % (pkg,)
            for m in ans:
                print "   %s" % (m,)
def getVersionCommand(args):
    """Get current version number of alpha,beta or stable releases

Format:

    pkg.py getVersion alpha|beta|stable
"""
    if len(args) >= 3:
        print getVersion(args[2])
    else:
        print getVersion()

def getReleaseTagCommand(args):
    """Internal"""
    print getReleaseTag(args[2])

def getReleaseCommand(args):
    """Get release of a package.

Format:
    pkg.py getRelease package-name
"""
    print getRelease(args[2])

def checkReleaseCommand(args):
    """Check to see if any package source was changed since last release

Format:

    pkg.py checkRelease pkgname (i.e. camac)
"""
    ans=checkRelease(args[2])
    if len(ans)==0:
        print "ok"
    else:
        for line in ans:
            print line

def listCommand(args):
    """List modules in packages.

Format:

    pkg.py list package-name|none

    Use none to list modules not yet included in any package. Use pkgadd command to add new/orphaned
    modules to packages.
"""
    if len(args) >= 3:
        item=args[2]
    else:
        print listCommand.__doc__
        sys.exit(1)
    cs=CvsStatus()
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
            os.stat("%s%srpm/subpackages/%s" % (getTopDir(),os.sep,pkg,))
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

def pkgaddCommand(args):
    """Add module to package.

Format:

    pkg.py pkgadd package-name module-file
"""
    pkg=args[2]
    path=args[3]
    try:
        os.stat("%s%srpm/subpackages/%s" % (getTopDir(),os.sep,pkg,))
    except:
        print "Package %s does not exist!" % (pkg,)
        sys.exit(1)
    if not sys.platform.startswith('win'):
        path=path.replace('$','\\$')
    p=subprocess.Popen('cvs -Q tag -F pkg_%s %s >%s 2>&1' % (pkg,path,os.devnull),shell=True)
    if p.wait() == 0:
        print "%s added to package %s" % (path,pkg)

def pkgremoveCommand(args):
    """Remove module from a package.

Format:

   pkg.py remove package-name module-file
"""
    pkg=args[2]
    path=args[3]
    try:
        os.stat("%s%srpm/subpackages/%s" % (getTopDir(),os.sep,pkg,))
    except:
        print "Package %s does not exist!" % (pkg,)
        sys.exit(1)
    if not sys.platform.startswith('win'):
       path=path.replace('$','\\$')
    p=subprocess.Popen('cvs -Q tag -d pkg_%s %s >%s 2>&1' % (pkg,path,os.devnull),shell=True)
    if p.wait() == 0:
        print "%s removed from package %s" % (path,pkg)

