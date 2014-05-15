import subprocess,datetime,os,sys,shutil

def writeDebInfo(outfile):
    """Create an info file which contains a link to the hudson build which created the releases"""
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write(('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%(url)s" />\n'+ 
            '</head>\n<body>\n<p>For more info please follow <a href="%(url)s">this link</a>.</p>\n'+
            '</body>\n</html>\n') % {'url':url})
    f.close()

def getDebfile(self,debflavor,pkg):
    """Get the name of the debian install file"""
    if os.uname()[-1]=='x86_64':
        arch='amd64'
    else:
        arch='i386'
    return "/DEBS/%(arch)s/mdsplus%(flav)s-%(pkg)s_%(major)d.%(minor)d.%(release)d_%(arch)s.deb"\
            % {'arch':arch,'flav':debflavor,'pkg':pkg,
               'major':self.major,'minor':self.minor,'release':self.release}

def createDeb(self,pkg):
    """Create a debian packages"""
    sys.stdout.flush()
    return subprocess.Popen('%(workspace)s/devscripts/makeDebian %(flavor)s %(pkg)s %(major)d.%(minor)d %(release)s' \
                           % {'workspace':self.workspace,'flavor':self.flavor,'pkg':pkg,'major':self.major,
                              'minor':self.minor,'release':self.release},shell=True,cwd=self.topdir).wait()

def prepareRepo(repodir,clean):
    try:
      os.mkdir(repodir)
    except OSError:
      pass
    for f in ('conf','pool','dists','db'):
      if clean:
        try:
            shutil.rmtree(repodir+'/'+f)
        except:
            pass
      try:
        os.mkdir(repodir+'/'+f)
      except OSError,e:
        if e.errno == os.errno.EEXIST:
           pass
        else:
           raise e
    f=open(repodir+'/conf/distributions','w')
    f.write(
"""Origin: MDSplus Development Team
Label: MDSplus
Codename: MDSplus
Version: 1.0
Architectures: i386 amd64 source
Components: alpha beta stable
Description: MDSplus packages
SignWith: MDSplus
""")
    f.close()

def exists(self):
    """Determine if package is available for this release. This becomes a method of the Build class in buildRelease.py ."""
    if self.flavor == "stable":
        flavor_part=""
    else:
        flavor_part="-%s" % self.flavor
    for pkg in ['all']+self.packages:
        debfile='/repository/%s/%s%s' % (self.dist,self.flavor,getDebfile(self,flavor_part,pkg))
        try:
            os.stat(debfile)
        except:
            self.log("%s not found, build required" % debfile)
            return False
    return True

def build(self):
    """Build Ubuntu Debian packages for the complete package and all the individual subpackages"""
    self.log("Building new release %d.%d.%d" % (self.major,self.minor,self.release))
    for d in ['debian','DEBS','BUILDROOT','EGGS','REPO']:
        try:
            os.mkdir("%s/%s" % (self.workspace,d))
        except:
            pass
    prepareRepo(self.workspace+"/REPO",True)
    if self.flavor=="stable":
        debflavor=""
        pythonflavor=""
    else:
        debflavor="-"+self.flavor
        pythonflavor=self.flavor+"-"
    self.log("%s, Starting build" % str(datetime.datetime.now()))
    cmd="""
rm -Rf DEBS/* SOURCES
./configure --enable-mdsip_connections --enable-nodebug \
--prefix=%(workspace)s/BUILDROOT/usr/local/mdsplus \
--exec_prefix=%(workspace)s/BUILDROOT/usr/local/mdsplus --with-gsi=/usr:gcc%(bits)d \
--with-labview=$LABVIEW_DIR --with-jdk=$JDK_DIR --with-idl=$IDL_DIR
if (! make clean )
then 
  exit 1
fi
if (! make )
then
  exit 1
fi
if (! make install )
then
  exit 1
fi
cd mdsobjects/python
export MDSPLUS_PYTHON_VERSION="%(pythonflavor)s%(major)d.%(minor)d-%(release)d"
rm -Rf dist
python setup.py bdist_egg
rsync -a dist %(workspace)s/BUILDROOT/usr/local/mdsplus/mdsobjects/python/
"""  % {'workspace':self.workspace,'bits':self.bits,'pythonflavor':pythonflavor,
                  'major':self.major,'minor':self.minor,'release':self.release}
    self.log(cmd)
    status=subprocess.Popen(cmd,shell=True,cwd=self.topdir).wait()
    if status != 0:
        raise Exception('Build failed with status=%d' % status)
    self.log("%s, Done with build" % str(datetime.datetime.now()))
    status=createDeb(self,'all')
    if status != 0:
        raise Exception("Error build catch all package, status=%d" % status)
    for pkg in self.packages:
        debfile=self.workspace+getDebfile(self,debflavor,pkg)
        status=createDeb(self,pkg)
        if status != 0:
            raise Exception("Error building debian package %s, status=%d" % (debfile,status))
        writeDebInfo(debfile[0:-3])
    self.log("%s, Completed build." % str(datetime.datetime.now()))

def test(self):
    self.log("Testing new release %d.%d-%d" % (self.major,self.minor,self.release))
    sys.path.insert(0,self.workspace+'/tests')
    from distribution_tests import test_debian as test
    test(self.workspace,self.flavor)
    sys.stdout.flush()


def deploy(self):
    self.log("Deploying new release %d.%d-%d" % (self.major,self.minor,self.release))
    prepareRepo("/repository/%s/repo" % self.dist,False)
    subprocess.Popen('find DEBS -name "*.deb" -exec reprepro -V --waitforlock 20 -b /repository/%s/repo -C %s includedeb MDSplus {} \;' \
                                % (self.dist,self.flavor),shell=True,cwd=self.workspace).wait()
    status=subprocess.Popen('rsync -av DEBS /repository/%s/%s/' % (self.dist,self.flavor),shell=True,cwd=self.workspace).wait()
    if status != 0:
        raise Exception("Error copying files to destination")
    for d in ('EGGS','REPO','DEBS'):
        try:
            shutil.rmtree(self.workspace+'/'+d)
        except:
            pass
    self.log("Completed deployment")
