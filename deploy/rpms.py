import subprocess,os,sys,pexpect

def _packages():
    """Get package list from rpm spec file"""
    pkgs=list()
    f=open('rpm.spec','r')
    lines=f.readlines()
    f.close()
    for line in lines:
        if line.startswith('%package'):
            pkgs.append(line.split()[1])
    return pkgs

class InstallationPackage(object):
    """Provides exists,build,test and deploy methods"""
    def __init__(self,info):
        self.info=info
        self.info['workspace']=os.environ['WORKSPACE']

    def exists(self):
        """Check to see if rpms for this release do not already exist."""
        for pkg in _packages():
            self.info['package']=pkg
            found=list()
            for arch in ('i686','noarch','x86_64'):
                self.info['arch']=arch
                rpm='/repository/%(dist)s/%(flavor)s/RPMS/%(arch)s/mdsplus%(rflavor)s-%(package)s-%(major)d.%(minor)d-%(release)d.%(dist)s.%(arch)s.rpm' % self.info
                try:
                    os.stat(rpm)
                    found.append(arch)
                except:
                    pass
            if not ((len(found) == 1 and found[0]=='noarch') or (len(found) == 2 and found[0]=='i686' and found[1]=='x86_64')):
                print("/repository/%(dist)s/%(flavor)s/RPMS/%(arch)s/mdsplus%(rflavor)s-%(package)s-%(major)d.%(minor)d-%(release)d.%(dist)s rpm's not found" % self.info)
                sys.stdout.flush()
                return False
        return True


    def build(self):
        """Build rpms using rpmbuild"""
        if len(self.info['rflavor'])==0:
            self.info['D_RFLAVOR']=""
        else:
            self.info['D_RFLAVOR']="--define='rflavor %(rflavor)s'" % self.info
        if "el5" in self.info['DIST']:
            cmds="""
mkdir -p %(workspace)s/%(flavor)s/{BUILD,RPMS,SPECS,SRPMS} && \
ln -sf /repository/SOURCES %(workspace)s/%(flavor)s/ && \
rpmbuild -bb  \
  --define='DIST %(DIST)s' \
  --define='BITS 64' \
  --define='_topdir %(workspace)s/%(flavor)s' \
  --define='mdsplus_version %(major)d.%(minor)d' \
  --define='mdsplus_release %(release)d' \
  %(D_RFLAVOR)s \
  --define='flavor %(flavor)s' \
  --buildroot=%(workspace)s/%(flavor)s/BUILDROOT \
  --target=x86_64-linux rpm_el5_bin.spec && \
rpmbuild -bb  \
  --define='DIST %(DIST)s' \
  --define='BITS 32' \
  --define '_topdir %(workspace)s/%(flavor)s' \
  --define='mdsplus_version %(major)d.%(minor)d' \
  --define='mdsplus_release %(release)d' \
  %(D_RFLAVOR)s \
  --define='flavor %(flavor)s' \
  --buildroot=%(workspace)s/%(flavor)s/BUILDROOT \
  --target=i686-linux rpm_el5_bin.spec && \
rpmbuild -bb  \
  --define='DIST %(DIST)s' \
  --define='BITS 32' \
  --define '_topdir %(workspace)s/%(flavor)s' \
  --define='mdsplus_version %(major)d.%(minor)d' \
  --define='mdsplus_release %(release)d' \
  %(D_RFLAVOR)s \
  --define='flavor %(flavor)s' \
  --buildroot=%(workspace)s/%(flavor)s/BUILDROOT \
  --target=i686-linux rpm_el5_noarch.spec
""" % self.info
        else:
            cmds="""
mkdir -p %(workspace)s/%(flavor)s/{BUILD,RPMS,SPECS,SRPMS} && \
ln -sf /repository/SOURCES %(workspace)s/%(flavor)s/ && \
rpmbuild -bb  \
  --define='DIST %(DIST)s' \
  --define='BITS 64' \
  --define='_topdir %(workspace)s/%(flavor)s' \
  --define='mdsplus_version %(major)d.%(minor)d' \
  --define='mdsplus_release %(release)d' \
  %(D_RFLAVOR)s \
  --define='flavor %(flavor)s' \
  --buildroot=%(workspace)s/%(flavor)s/BUILDROOT \
  --target=x86_64-linux rpm.spec && \
rpmbuild -bb  \
  --define='DIST %(DIST)s' \
  --define='BITS 32' \
  --define '_topdir %(workspace)s/%(flavor)s' \
  --define='mdsplus_version %(major)d.%(minor)d' \
  --define='mdsplus_release %(release)d' \
  %(D_RFLAVOR)s \
  --define='flavor %(flavor)s' \
  --buildroot=%(workspace)s/%(flavor)s/BUILDROOT \
  --target=i686-linux rpm.spec
""" % self.info
        if subprocess.Popen(cmds,shell=True).wait() != 0:
            sys.stdout.flush()
            raise Exception("Problem building %s rpms" % self.info['flavor'])
        try:
            cmd="/bin/sh -c 'rpmsign --addsign --define=\"_signature gpg\" --define=\"_gpg_name MDSplus\" %(flavor)s/RPMS/*/*.rpm'" % self.info
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
        if subprocess.Popen("createrepo -q %(flavor)s/RPMS" % self.info,shell=True).wait() != 0:
            sys.stdout.flush()
            raise Exception("Problem creating repository from RPMS")

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
            pkgs=_packages()
            pkgs.append('mdsplus')
            for pkg in pkgs:
                if pkg != 'repo':
                    if pkg=='mdsplus':
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
        if subprocess.Popen("""
rsync -a %(flavor)s/RPMS /repository/%(dist)s/%(flavor)s/
""" % self.info,shell=True).wait() != 0:
            raise Exception("Error deploying %(flavor)s release to repository" % self.info)
        if subprocess.Popen("""
python setup.py -q bdist_egg -d /repository/EGGS
""" % self.info,shell=True,cwd="%(flavor)s/BUILDROOT/usr/local/mdsplus/mdsobjects/python" % self.info).wait() != 0:
            raise Exception("Error deploying python release egg to repository" % self.info)
