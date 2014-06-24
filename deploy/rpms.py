import subprocess,os,sys,pexpect,xml.etree.ElementTree as ET,fnmatch,tempfile

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
                self.info['package']=""
            else:
                self.info['package']="-%s" % pkg
            if package.attrib['arch']=='noarch':
                arches=('noarch',)
            else:
                arches=('x86_64','i686')
            for arch in arches:
                self.info['arch']=arch
                rpm='/repository/%(dist)s/%(flavor)s/RPMS/%(arch)s/mdsplus%(rflavor)s%(package)s-%(major)d.%(minor)d-%(release)d.%(dist)s.%(arch)s.rpm' % self.info
                try:
                    os.stat(rpm)
                except:
                    print("%s not found" % rpm)
                    sys.stdout.flush()
                    return False
        print("Latest %(flavor) release %(major)d.%(minor)d-%(release)d is already available. Nothing to do for this release." % self.info)
        sys.stdout.flush()
        return True

    def buildInstall(self):
        """Build MDSplus from the sources and install into a BUILDROOT directory"""
        self.info['mdsplus_version']="%(major)d.%(minor)d_%(release)d" % self.info
        if self.info['flavor']!='stable':
            self.info['mdsplus_version']="%(flavor)s-%(mdsplus_version)s" % self.info
        tree=ET.parse('packaging.xml')
        root=tree.getroot()
        install_script=root.find('rpm').find('install_script').text
        install_script=install_script[install_script.find('#'):]
        fd,filename=tempfile.mkstemp()
        os.write(fd,install_script % self.info)
        os.close(fd)
        os.chmod(filename,0700)
        status = subprocess.Popen(filename,shell=True).wait()
        os.remove(filename)
        if status != 0:
            raise Exception("Problem building MDSplus from sources")

    def fixFilename(self,filename):
        """For binary packages we need to refer to /libnn, /binnn and /uidnn where nn is 32 or 64. For
        noarch packages we need to use /lib??, /bin?? to get the no arch dependent scripts and files"""
        try:
            if self.info['arch']=='noarch':
                filename=filename.replace("/lib/","/lib??/") 
                filename=filename.replace("/bin/","/bin??/")
                if filename.endswith(('/lib','/bin')):
                    filename=filename+"??"
            else:
                filename=filename.replace("/lib/","/lib%(bits)d/") 
                filename=filename.replace("/bin/","/bin%(bits)d/")
                filename=filename.replace("/uid/","/uid%(bits)d/")
                if filename.endswith(('/lib','/bin','/uid')):
                    filename=filename+"%(bits)d"
            ans=filename % self.info
        except:
            raise Exception("Error fixing filename "+filename)
        return ans

    def buildRpms(self):
        tree=ET.parse('packaging.xml')
        root=tree.getroot()
        rpmspec=root.find('rpm').find('spec_start').text
        s=rpmspec.split('\n')
        for idx in range(len(s)):
            if len(s[idx].strip()) == 0:
                s[idx]=""
        rpmspec='\n'.join(s)
            
        bin_packages=list()
        noarch_packages=list()
        for package in root.getiterator('package'):
            attr=package.attrib
            if attr["arch"]=="noarch":
                noarch_packages.append(package)
            else:
                bin_packages.append(package)
                
        for arch in ({"target":"x86_64-linux","bits":64,"arch_t":".x86_64"},{"target":"i686-linux","bits":32,"arch_t":".i686"}):
            self.info['target']=arch['target']
            self.info['bits']=arch['bits']
            self.info['arch']="bin"
            self.info['arch_t']=arch['arch_t']
            for package in bin_packages:
                if package.attrib['name'] != "MDSplus":
                    self.info["packagename"]="-%s" % package.attrib["name"]
                else:
                    self.info["packagename"]=""
                self.info['summary']=package.attrib['summary']
                self.info['description']=package.attrib['description']
                out,specfilename=tempfile.mkstemp()
                os.write(out,rpmspec % self.info)
                for require in package.getiterator("requires"):
                    self.info['reqpkg']=require.attrib['package']
                    os.write(out,"Requires: mdsplus%(rflavor)s-%(reqpkg)s >= %(major)d.%(minor)d-%(release)d\n" % self.info)
                os.write(out,"""
%%description
%(description)s
%%clean
%%files
%%defattr(-,root,root)
""" % self.info)
                for inc in package.getiterator('include'):
                    for inctype in inc.attrib:
                        include=self.fixFilename(inc.attrib[inctype])
                        if inctype == "dironly":
                            os.write(out,"%%dir %s\n" % include)
                        else:
                            os.write(out,"%s\n" % include)
                for exc in package.getiterator('exclude'):
                    for exctype in exc.attrib:
                        exclude=self.fixFilename(exc.attrib[exctype])
                        os.write(out,"%%exclude %s\n" % exclude)
                if package.find("exclude_staticlibs") is not None:
                    os.write(out,"%%exclude /usr/local/mdsplus/lib%(bits)d/*.a\n" % self.info)
                if package.find("include_staticlibs") is not None:
                    os.write(out,"/usr/local/mdsplus/lib%(bits)d/*.a\n" % self.info)
                for s in (("preinst","pre"),("postinst","post"),("prerm","preun"),("postrm","postun")):
                    script=package.find(s[0])
                    if script is not None and ("type" not in script.attrib or script.attrib["type"]=="rpm"):
                        os.write(out,"%%%s\n%s\n" % (s[1],script.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
                triggerin=package.find("triggerin")
                if triggerin is not None:
                    os.write(out,"%%triggerin -- %s\n%s\n" % (triggerin.attrib['trigger'],
                                                           triggerin.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
                os.close(out)
                self.info['specfilename']=specfilename
                print("Building rpm for mdsplus%(rflavor)s%(packagename)s%(arch_t)s" % self.info)
                sys.stdout.flush()
                p = subprocess.Popen("""
rpmbuild -bb --define '_topdir %(workspace)s/%(flavor)s' --buildroot=%(workspace)s/%(flavor)s/BUILDROOT --target=%(target)s %(specfilename)s 2>&1
""" % self.info,stdout=subprocess.PIPE,shell=True)
                messages=p.stdout.readlines()
                status=p.wait()
                os.remove(specfilename)
                if status != 0:
                    print(''.join(messages))
                    sys.stdout.flush()
                    raise Exception("Error building rpm for package mdsplus%(rflavor)s%(packagename)s%(arch_t)s" % self.info)
                print("Done building rpm for mdsplus%(rflavor)s%(packagename)s%(arch_t)s" % self.info)
                sys.stdout.flush()
        for package in noarch_packages:
            self.info['arch']="noarch"
            if package.attrib['name'] != "MDSplus":
                self.info["packagename"]="-%s" % package.attrib["name"]
            else:
                self.info["packagename"]=""
            out,specfilename=tempfile.mkstemp()
            os.write(out,rpmspec % self.info)
            for require in package.getiterator("requires"):
                self.info['reqpkg']=require.attrib['package']
                os.write(out,"Requires: mdsplus%(rflavor)s-%(reqpkg)s >= %(major)d.%(minor)d-%(release)d\n" % self.info)
            os.write(out,"""
Buildarch: noarch
%%description
%(description)s
%%clean
%%files
%%defattr(-,root,root)
""" % self.info)
            for inc in package.getiterator('include'):
                for inctype in inc.attrib:
                    include=self.fixFilename(inc.attrib[inctype])
                    if inctype == "dironly":
                        os.write(out,"%%dir %s\n" % include)
                    else:
                        os.write(out,"%s\n" % include)
            for exc in package.getiterator('exclude'):
                for exctype in exc.attrib:
                    exclude=self.fixFilename(exc.attrib[exctype])
                    os.write(out,"%%exclude %s\n" % exclude)
            if package.find("exclude_staticlibs") is not None:
                os.write(out,"%%exclude /usr/local/mdsplus/lib??/*.a\n" % self.info)
            for s in (("preinst","pre"),("postinst","post"),("prerm","preun"),("postrm","postun")):
                script=package.find(s[0])
                if script is not None and ("type" not in script.attrib or script.attrib["type"]=="rpm"):
                    os.write(out,"%%%s\n%s\n" % (s[1],script.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
            triggerin=package.find("triggerin")
            if triggerin is not None:
                os.write(out,"%%triggerin -- %s\n%s\n" % (triggerin.attrib['trigger'],
                                                       triggerin.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
            os.close(out)
            self.info['specfilename']=specfilename
            print("Building rpm for mdsplus%(rflavor)s%(packagename)s.noarch" % self.info)
            sys.stdout.flush()
            p=subprocess.Popen("""
rpmbuild -bb --define '_topdir %(workspace)s/%(flavor)s' --buildroot=%(workspace)s/%(flavor)s/BUILDROOT %(specfilename)s 2>&1
""" % self.info,stdout=subprocess.PIPE,shell=True)
            messages=p.stdout.readlines()
            status = p.wait()
            os.remove(specfilename)
            if status != 0:
                print(''.join(messages))
                raise Exception("Error building rpm for package mdsplus%(rflavor)s%(packagename)s.noarch" % self.info)
            print("Done building rpm for mdsplus%(rflavor)s%(packagename)s.noarch" % self.info)
            sys.stdout.flush()
        try:
            cmd="/bin/sh -c 'rpmsign --addsign --define=\"_signature gpg\" --define=\"_gpg_name MDSplus\" %(workspace)s/%(flavor)s/RPMS/*/*.rpm'" % self.info
            child = pexpect.spawn(cmd,timeout=60)
            child.expect("Enter pass phrase: ")
            child.sendline("")
            child.expect(pexpect.EOF)
            child.close()
            if child.status != 0:
                sys.stdout.flush()
                raise Exception("Error signing rpms. status=%d" % child.status)
        except:
            sys.stdout.flush()
            raise Exception("Error signing rpms: %s" % sys.exc_info()[1])
        if subprocess.Popen("createrepo -q %(workspace)s/%(flavor)s/RPMS" % self.info,shell=True).wait() != 0:
            sys.stdout.flush()
            raise Exception("Problem creating repository from RPMS")
 

    def build(self):
        """Build rpms from sources"""
        self.buildInstall()
        self.buildRpms()

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
set -e
sudo yum-config-manager --add-repo test-mdsplus%(rflavor)s.repo
sudo yum-config-manager --enable test-mdsplus%(rflavor)s >/dev/null
sudo yum clean metadata
sudo yum makecache""" % self.info,shell=True).wait() != 0:
            errors.append("Erro preparing repository")
        else:
            print("Testing package installation")
            sys.stdout.flush()
            tree=ET.parse('packaging.xml')
            root=tree.getroot()
            for package in root.getiterator('package'):
                pkg=package.attrib['name']
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
set -e
sudo yum install -y mdsplus%(rflavor)s-mitdevices
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
        if subprocess.Popen("""
rsync -a %(workspace)s/%(flavor)s/RPMS /repository/%(dist)s/%(flavor)s/
""" % self.info,shell=True).wait() != 0:
            raise Exception("Error deploying %(flavor)s release to repository" % self.info)
        if subprocess.Popen("""
set -e
if ( which python3 > /dev/null 2>&1 )
then
  python3 setup.py -q bdist_egg -d /repository/EGGS
fi
python setup.py -q bdist_egg -d /repository/EGGS
""" % self.info,shell=True,cwd="%(workspace)s/%(flavor)s/BUILDROOT/usr/local/mdsplus/mdsobjects/python" % self.info).wait() != 0:
            raise Exception("Error deploying python release egg to repository" % self.info)
