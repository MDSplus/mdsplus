import subprocess,os,sys,pexpect,xml.etree.ElementTree as ET,fnmatch,tempfile
from deploy import getPackageFiles

#################################### old stuff ###############################################

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

############################### end old stuff ###########################################################

class InstallationPackage(object):
    """Provides exists,build,test and deploy methods"""
    def __init__(self,info):
        self.info=info
        self.info['workspace']=os.environ['WORKSPACE']

    def exists(self):
        """Check to see if rpms for this release already exist."""
        tree=ET.parse('packaging.xml')
        root=tree.getroot()
        for package in root.getiterator('package'):
            pkg=package.attrib['name']
            if pkg == "MDSplus":
                pkg = ""
            else:
                pkg = "-%s" % pkg
            self.info['package']=pkg
            self.info['arch']={"noarch-x86_64":"noarch","noarch-i686":"noarch","bin-x86_64":"amd64","bin-i686":"i386"}["%s-%s" % (package.attrib['arch'],os.uname()[-1])]
            rpm='/repository/%(dist)s/%(flavor)s/DEBS/%(arch)s/mdsplus%(rflavor)s%(package)s-%(major)d.%(minor)d.%(release)d_%(arch)s.deb' % self.info
            try:
                os.stat(rpm)
            except:
                print("%s not found" % rpm)
                sys.stdout.flush()
                return False
        print("Latest %(flavor) release %(major)d.%(minor)d.%(release)d is already available. Nothing to do for this release." % self.info)
        sys.stdout.flush()
        return True

    def buildInstall(self):
        """Build MDSplus from the sources and install into a BUILDROOT directory"""
        self.info['mdsplus_version']="%(major)d.%(minor)d_%(release)d" % self.info
        if self.info['flavor']!='stable':
            self.info['mdsplus_version']="%(flavor)s-%(mdsplus_version)s" % self.info
        tree=ET.parse('packaging.xml')
        root=tree.getroot()
        install_script=root.find('ubuntu').find('install_script').text
        install_script=install_script[install_script.find('#'):]
        fd,filename=tempfile.mkstemp()
        os.write(fd,install_script % self.info)
        os.close(fd)
        os.chmod(filename,0700)
        status = subprocess.Popen(filename,shell=True).wait()
        os.remove(filename)
        if status != 0:
            raise Exception("Problem building MDSplus from sources")

    def buildDebs(self):
        tree=ET.parse('packaging.xml')
        bin_arch={'x86_64':'amd64','i686':'i386'}[os.uname()[-1])
        root=tree.getroot()
        for package in root.getiterator('package'):
            pkg = package.attrib['name']
            if pkg=='MDSplus':
                self.info['packagename']=""
            else:
                self.info['packagename']="-%s" % pkg
            self.info['arch']={'bin':bin_arch,'noarch':'noarch'}[package.attrib['arch']]
            self.info['description']=package.attrib['description']
            self.info['tmpdir']=tempfile.mkdtemp()
            os.mkdir("%(tmpdir)s/DEBIAN" % self.info)
            includes=list()
            for inc in package.getiterator('include'):
                for inctype in inc.attrib:
                    include=self.fixFilename(inc.attrib[inctype])
                    if inctype != "dironly":
                        includes.append(include)
            for exc in package.getiterator('exclude'):
                for exctype in exc.attrib:
                    excludes.append(self.fixFilename(exc.attrib[exctype]))
            if package.find("exclude_staticlibs") is not None:
                excludes.append("/usr/local/mdsplus/lib/*.a")
            if package.find("include_staticlibs") is not None:
                includes.append("/usr/local/mdsplus/lib/*.a")
            files=getPackageFiles("%(workspace)s/%(flavor)s/BUILDROOT" % self.info,includes,excludes)
            for f in files:
                self.info['file']=f
                if subprocess.Popen("""
set -e
dn=$(dirname %(file)s)
mkdir -p %(tmpdir)s/${dn}
cp %(workspace)s/%(flavor)s/BUILDROOT/%(file)s ${dn}/
""",shell=True).wait() != 0:
                    raise Exception("Error building deb")
            depends=list()
            for require in package.getiterator("requires"):
                depends.append("mdsplus%s-%s" % (self.info['rflavor'],require.attrib['package']))
            self.info['depends']=','.join(depends)
            f=open("%(tmpdir)/DEBIAN/control","w")
            f.write("""Package: mdsplus%(rflavor)s%(packagename)s
Version: %(major)d.%(minor)d.%(release)d
Section: admin
Priority: optional
Architecture: %(arch)s
Depends: %(depends)s
Maintainer: Tom Fredian <twf@www.mdsplus.org>
Description: %(description)s
""") % self.info
            f.close()
            for s in ("preinst","postinst","prerm","postrm"):
                script=package.find(s)
                if script is not None and ("type" not in script.attrib or script.attrib["type"]!="rpm"):
                    self.info['script']=s
                    f.open("%(tmpdir)s/DEBIAN/%(script)s" % self.info,"w")
                    f.write(out,"%s" % (script.text.replace("__INSTALL_PREFIX__","/usr/local/mdsplus")))
                    f.close()
            self.info['debfile']="%(workspace)s/%(flavor)s/DEBS/%(arch)s/mdsplus%(rflavor)s%(packagename)s_%(major)d.%(minor)d.%(release)d_%(arch)s.deb" % self.info
            if subprocess.Popen("""
set -e
mkdir -p %{workspace}s/%{flavor}s/DEBS/%(arch)s
dpkg-deb -- build %(tmpdir)s %(debfile)s
reprepro -V -b %{workspace}s/%(flavor)s/REPO -C %(flavor)s includedeb MDSplus %(debfile)s
""" % self.info,shell=True).wait() != 0:
                raise Exception("Problem building package or repository")
 

    def build(self):
        """Build rpms from sources"""
        self.buildInstall()
        self.buildDebs()

    def test(self):
        print("Preparing test repository")
        sys.stdout.flush()
        self.info['workspace']=os.environ['WORKSPACE']
        errors=list()
        f=open("test-mdsplus%(rflavor)s.repo" % self.info,"w")
        f.write("""
[test-mdsplus%(rflavor)s]
name=test-mdsplus%(rflavor)s
baseurl=file://%(workspace)s/%(flavor)s/RPMS
enabled=1
skip_if_unavailable=1
""" % self.info)
        f.close()
        if subprocess.Popen("""
sudo yum remove -y 'mdsplus*' 
sudo yum-config-manager --add-repo test-mdsplus%(rflavor)s.repo &&
sudo yum-config-manager --enable test-mdsplus%(rflavor)s >/dev/null
sudo yum clean metadata &&
sudo yum makecache""" % self.info,shell=True).wait() != 0:
            errors.append("Erro preparing repository")
        else:
            print("Testing package installation")
            sys.stdout.flush()
            pkgs=packages('packaging.xml')
            for pkg in pkgs:
                if pkg != 'repo':
                    if pkg=='MDSplus':
                        pkg=""
                    else:
                        pkg="-"+pkg
                    self.info['package']=pkg
                    if subprocess.Popen("sudo yum install -y mdsplus%(rflavor)s%(package)s && sudo yum remove -y 'mdsplus*'" % self.info,shell=True).wait() != 0:
                        errors.append("Error installing package mdsplus%(rflavor)s%(package)s" % self.info)
        if len(errors) == 0:
            if subprocess.Popen("""
sudo yum install -y mdsplus%(rflavor)s-mitdevices &&
. /etc/profile.d/mdsplus.sh
python <<EOF
import sys,os
import MDSplus
sys.path.append(os.path.dirname(MDSplus.__file__))
from tests import test_all
from unittest import TextTestRunner
result=TextTestRunner().run(test_all())
if not result.wasSuccessful():
  sys.exit(1)
EOF""" % self.info,shell=True).wait() != 0:
                errors.append("Error running regression tests")
        subprocess.Popen("""
sudo yum-config-manager --disable test-mdsplus%(rflavor)s >/dev/null
sudo yum remove -y 'mdsplus*'""" % self.info,shell=True).wait()
        if len(errors) > 0:
            errors.insert(0,"Testing failed")
            raise Exception('\n'.join(errors))

    def deploy(self):
        """Deploy release to repository"""
        pass
