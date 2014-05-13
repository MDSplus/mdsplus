import subprocess,os,sys

class MDSplusVersion(object):
  """Instantiation of this class will check to see if anything changed since the last release of this "flavor".
  If something changed it will increment the release field of the version (i.e. major.minor.release) and tag the latest
  versions/commit with a tag called 'flavor'_release-'major'-'minor'-'release'.
    """

  def __init__(self):
    self.topdir=os.path.dirname(os.path.abspath(__file__+'/../'))
    #
    #   Get the flavor from the environment variable "CVS_BRANCH" set by hudson.
    #
    if 'CVS_BRANCH' in os.environ:
      self.flavor=os.environ['CVS_BRANCH']
    else:
      self.flavor='alpha'
    #
    #   Find the cvs tags on module configure.in
    #
    p=subprocess.Popen('cvs log -h configure.in',stdout=subprocess.PIPE,shell=True,cwd=self.topdir)
    out=p.stdout.readlines()
    p.wait()
    version_num=0
    tag=self.flavor+'_release-'
    #
    # Find the latest release tag for this flavor
    #
    for line in out:
      if tag in line:
        v=line.split(':')[0].split('-')
        this_version=(int(v[1])<<24)+(int(v[2])<<16)+(int(v[3]))
        if this_version > version_num:
          version_num=this_version
          self.major=int(v[1])
          self.minor=int(v[2])
          self.release=int(v[3])
    #
    # Check to see if anything changed. cvs diff will output lines with Index: module-name
    # a module has changed.
    #
    p=subprocess.Popen('cvs -Q diff --brief -r %s 2>&1 | grep -c Index:' % self.rtag(),stdout=subprocess.PIPE,
                       shell=True,cwd=self.topdir)
    out=p.stdout.readlines()
    p.wait()
    num_changes=int(out[0][:-1])
    print "%d modules have changed since the last release (%d.%d.%d)" % (num_changes,self.major,self.minor,self.release)
    #
    # If something has changed, increment the release part of the tag and create a new tag of the current revisions
    #
    if num_changes > 0:
      self.release=self.release+1
      if self.flavor == 'stable':
        rflavor=""
      else:
        rflavor="-"+self.flavor
      status=subprocess.Popen(('if ( cvs -Q tag %(tag)s 2>&1);then'+
                               'mkdir /tmp/%(flavor)s; ln -sf $(pwd) /tmp/%(flavor)s/mdsplus;pushd /tmp/%(flavor)s;'
                               'tar zcf /repository/SOURCES/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d --exclude CVS mdsplus;'+
                               'popd; rm -Rf /tmp/%(flavor)s;fi') % {'tag':self.rtag(),'rflavor':rflavor,'major':self.major,
                                                                     'minor':self.minor,'release':self.release,'flavor':self.flavor},
                              shell=True,cwd=self.topdir).wait()
      if status != 0:
        raise Exception("Error tagging new release - %s %d.%d.%d" % (self.flavor,self.major,self.minor,self.release))
      print "New MDSplus %s release: %d.%d.%d" % (self.flavor,self.major,self.minor,self.release)

  def rtag(self):
        return "%s_release-%d-%d-%d" % (self.flavor,self.major,self.minor,self.release)

class MDSplusBuild(object):
  """This class will provide several methods:
      check()  --- See if kits are available for the latest release on the repository server
      build()  --- Build kit(s)
      test()   --- Install kits and run regression tests
      deploy() --- Deploy kit to repository server

      Builds for all platforms can be accomplished by doing:

      b=MDSplusBuild()
      if not b.exists():
        b.build()
        b.test()
        b.deploy()
  """

  
  def __init__(self):
    """Configure the check,build,test and deploy methods based on the system architecture."""
    v=MDSplusVersion()
    self.flavor=v.flavor
    self.major=v.major
    self.minor=v.minor
    self.release=v.release
    self.topdir=v.topdir
    self.packages=self.getPackages()
    self.dist=self.getDist()
    self.machine=os.uname()[-1]
    if self.machine=='x86_64':
      self.bits=64
    else:
      self.bits=32
    if self.dist.startswith('fc') or self.dist.startswith('el'):
      from buildRedhat import exists,build,test,deploy
    elif self.dist == 'macosx':
      from buildMacosx import exists,build,test,deploy
    elif self.dist == 'win':
      from buildWindows import exists,build,test,deploy
    elif self.dist.startswith('Ubuntu'):
      from buildUbuntu import exists,build,test,deploy
    self.exists_rtn = exists
    self.build_rtn = build
    self.test_rtn = test
    self.deploy_rtn = deploy
    self.workspace = os.environ['WORKSPACE']
  
  def exists(self):
    """See if package for this release already exists"""
    return self.exists_rtn(self)

  def build(self):
    """Build a new release"""
    return self.build_rtn(self)

  def test(self):
    """Test the new release"""
    return self.test_rtn(self)

  def deploy(self):
    """Deploy the new release to the download repository"""
    return self.deploy_rtn(self)

  def getDist(self):
    """Determine the type of platform this is running on."""

    def getLsbReleaseDist():
      p=subprocess.Popen('lsb_release -a -s 2>/dev/null',stdout=subprocess.PIPE,shell=True)
      info=p.stdout.readlines()
      p.wait()
      platform=info[0][0:-1]
      version=info[2][0:-1].split('.')[0]
      return platform+version

    dist=None
    if os.name=="nt":
      dist="win"
    elif os.uname()[0]=='SunOS':
      dist=os.uname()[0]+"-"+os.uname()[3].split('.')[0]
    elif os.uname()[0]=='Linux':
      if 'Ubuntu' in os.uname()[3]:
        dist=getLsbReleaseDist()+os.uname()[4]
      else:
        parts=os.uname()[2].split('.')
        for p in parts:
          if p.startswith('el') or p.startswith('fc'):
            dist=p
            break
          elif os.uname()[0]=='Darwin':
            dist='macosx'
            break
    if dist is None:
      raise Exception("Error getting distribution information, uname=%s" % (str(os.uname()),))
    return dist

  def getPackages(self):
    """Get the list of subpackages."""
    pkgs=list()
    w=os.walk("%s%srpm%ssubpackages"%(self.topdir,os.sep,os.sep))
    path,dirs,files = w.next()
    for pkg in files:
      s=os.stat("%s%srpm%ssubpackages%s%s"%(self.topdir,os.sep,os.sep,os.sep,pkg))
      if s.st_size > 0:
        pkgs.append(pkg)
    return pkgs

  def log(self,text):
    print(text)
    sys.stdout.flush()

if __name__ == "__main__":
  b=MDSplusBuild()
  if not b.exists():
    b.build()
    b.test()
    b.deploy()
