import subprocess,os,sys,xml.etree.ElementTree as ET,fnmatch

__doc__="""Deploy installation packages

   Format:
      python deploy.py flavor major minor release

   Where:
      flavor is one of alpha,beta or stable
      major is first number in version (i.e. in release 6.2-42, major is 6)
      minor is second number in version (i.e. in release 6.2.42, minor is 2)
      release is third number in version (i.e. in release 6.2-42, release is 42)

   Deploy does the following:

   * Determine platform and import platform specific packaging class instance.
   * Check if installation packages for this flavor and version already exits in the
     repository and just exit if they do.
   * Build new installation packages.
   * Test new installation packages.
   * Deploy installation packages to repository.

  NOTE: DIST environment variable designating the platform (i.e. el6,fc20,Ubuntu12)
        must be defined!

"""
def flushPrint(text):
  print(text)
  sys.stdout.flush()

def packages(packagingXml):
    """Get package list"""
    pkgs=list()
    tree=ET.parse(packagingXml)
    root=tree.getroot()
    pfiles=dict()
    for package in root.getiterator('package'):
      pkgs.append(package.attrib['name'])
    return pkgs

def getPackageFiles(buildroot,includes,excludes):
    files=list()
    for f in includes:
        f = buildroot+f
        if os.path.isdir(f):
            for root, dirs, filenams in os.walk(f):
                for filenam in filenams:
                    files.append(os.path.join(root,filenam))
        elif ('?' in f) or ('*' in f):
            dirnam=f
            while ('?' in dirnam) or ('*' in dirnam):
                dirnam=os.path.dirname(dirnam)
            for root, dirs, filenams in os.walk(dirnam):
                if fnmatch.fnmatch(root,f):
                    files=files+getPackageFiles(buildroot,[root[len(buildroot):],],excludes)
                else:
                    for filenam in filenams:
                        filenam=os.path.join(root,filenam)
                        if fnmatch.fnmatch(filenam,f):
                            files.append(filenam)
        else:
            try:
                os.stat(f)
                files.append(f)
            except:
                pass
    if len(excludes) > 0:
        hasuid=False
        for exclude in excludes:
            if '/xuid' in exclude:
                print "excluding: %s" % exclude
                hasuid=True
        excludefiles=getPackageFiles(buildroot,excludes,[])
        if hasuid:
            print "Found %d" % len(excludefiles)
            for exclude in excludefiles:
                print "excluding: %s" % exclude
        for exclude in excludefiles:
            if exclude in files:
                files.remove(exclude)
    return files

def checkPackageFiles(buildroot,packagingXml,mode):
    tree=ET.parse(packagingXml)
    root=tree.getroot()
    pfiles=dict()
    for package in root.getiterator('package'):
        includes=[]
        for inc in package.getiterator('include'):
            for inctype in inc.attrib:
                if inctype != "dironly":
                    include=inc.attrib[inctype]
                    if include.endswith("/lib"):
                        include=include+"*"
                    elif "/lib/" in include:
                        include=include.replace("/lib/","/lib*/")
                    elif include.endswith("/bin"):
                        include=include+"*"
                    elif "/bin/" in include:
                        include=include.replace("/bin/","/bin*/")
                    elif include.endswith("/uid"):
                        include=include+"*"
                    elif "/uid/" in include:
                        include=include.replace("/uid/","/uid*/")
                    includes.append(include)
        excludes=[]
        for exc in package.getiterator('exclude'):
            for exctype in exc.attrib:
                exclude=exc.attrib[exctype]
                if exclude.endswith("/lib"):
                    exclude=exclude+"*"
                elif "/lib/" in exclude:
                    exclude=exclude.replace("/lib/","/lib*/")
                elif exclude.endswith("/bin"):
                    exclude=exclude+"*"
                elif "/bin/" in exclude:
                    exclude=exclude.replace("/bin/","/bin*/")
                elif exclude.endswith("/uid"):
                    exclude=exclude+"*"
                elif "/uid/" in exclude:
                    exclude=exclude.replace("/uid/","/uid*/")
                excludes.append(exclude)
        if package.find("exclude_staticlibs") is not None:
            excludes.append("/usr/local/mdsplus/lib??/*.a")
        if package.find("include_staticlibs") is not None:
            includes.append("/usr/local/mdsplus/lib??/*a")
        pfiles[package.attrib['name']]=getPackageFiles(buildroot,includes,excludes)
        if mode == "contents":
            print("Packge: %s (Arch: %s) contains:" % (package.attrib['name'],package.attrib['arch']))
            for f in pfiles[package.attrib['name']]:
                f=f.replace('/bin32/','/bin/')
                f=f.replace('/bin64/','/bin64/')
                f=f.replace('/lib32/','/lib/')
                f=f.replace('/lib64/','/lib/')
                f=f.replace('/uid32/','/uid/')
                f=f.replace('/uid64/','/uid/')
                print('      '+f[len(buildroot):])
    if mode == "check":
        for pkg in pfiles:
            for package in root.getiterator('package'):
                if package.attrib['name']==pkg:
                    arch = package.attrib['arch']
                    for f in pfiles[pkg]:
                        p=subprocess.Popen('file "%s"' % f,stdout=subprocess.PIPE,shell=True)
                        ftype=p.stdout.readlines()
                        status=p.wait()
                        if status == 0:
                            ok=False
                            ftype=ftype[0][:-1]
                            if arch == "noarch":
                                accepted_types=("text","compressed","Zip","vi:","mnu:",".DS_Store:",
                                                "/mdsplus/trees/","JPEG","PNG",
                                                "dll:","symbolic link",".sav:","python","Java","PGP")
                                for a_t in accepted_types:
                                    if a_t in ftype:
                                        ok=True
                                        break
                            else:
                                accepted_types=("uid:","ELF","ar archive","symbolic link")
                                for a_t in accepted_types:
                                    if a_t in ftype:
                                        ok=True
                                        break
                            if not ok:
                                print "%s but: %s" % (arch,ftype)
        files=pfiles.values()
        for i in range(len(files)):
            for j in range(i+1,len(files)):
                for f in files[i]:
                    if f in files[j]:
                        print("%s is included in both %s and %s" % (f,pfiles.keys()[i],pfiles.keys()[j]))
        allfiles=list()
        for f in files:
            allfiles+=f
        for root, dirs, filenams in os.walk(buildroot):
            for filenam in filenams:
                f=os.path.join(root,filenam)
                if f not in allfiles:
                    print("%s is not in any package" % f)
                else:
                    allfiles.remove(f)
        if len(allfiles) > 0:
            for f in allfiles:
                print("%s in a package but not in buildroot" % f)

if __name__ == "__main__":

  if len(sys.argv) > 1 and sys.argv[1]=='check':
    buildroot=sys.argv[2]
    checkPackageFiles(buildroot,'packaging.xml','check')
    sys.exit(0)
  elif len(sys.argv) > 1 and sys.argv[1]=='contents':
    buildroot=sys.argv[2]
    checkPackageFiles(buildroot,'packaging.xml','contents')
    sys.exit(0)

  if "DIST" not in os.environ:
    flushPrint("DIST not defined!")
    flushPrint(__doc__)
    sys.exit(1)

  if len(sys.argv) not in (5,6):
    flushPrint("Invalid argument list")
    flushPrint(__doc__)
    sys.exit(1)

  info={'flavor':sys.argv[1],'major':int(sys.argv[2]),'minor':int(sys.argv[3]),'release':int(sys.argv[4]),'dist':os.environ['DIST'],'workspace':os.environ['WORKSPACE']}
  if info['flavor']=='stable':
    info['rflavor']=""
  else:
    info['rflavor']="-"+info['flavor']
  info['DIST']=os.environ['DIST']
  if os.environ['DIST'].startswith('el') or os.environ['DIST'].startswith('fc'):
    module=__import__('rpms',globals())
  elif os.environ['DIST'].startswith('Ubuntu'):
    module=__import__('ubuntu',globals())
  elif os.environ['DIST'].startswith('solaris'):
    module=__import__('solaris',globals())
  elif os.environ['DIST'].startswith('win'):
    module=__import__('windows',globals())
  InstallationPackage=module.InstallationPackage(info)
  if len(sys.argv)==4:
    if not InstallationPackage.exists():
      if subprocess.Popen("tar zxf /repository/SOURCES/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.tgz" % info,shell=True,cwd=info['workspace']).wait() != 0:
        raise Exception("Error unpacking sources for this release")
      InstallationPackage.build()
      InstallationPackage.test()
      InstallationPackage.deploy()
  else:
    if sys.argv[5]=='exists':
      print(InstallationPackage.exists())
    elif sys.argv[5]=='build':
      if subprocess.Popen("tar zxf /repository/SOURCES/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.tgz" % info,shell=True,cwd=info['workspace']).wait() != 0:
        raise Exception("Error unpacking sources for this release")
      InstallationPackage.build()
    elif sys.argv[5]=='test':
      InstallationPackage.test()
    elif sys.argv[5]=='deploy':
      InstallationPackage.deploy()
       
