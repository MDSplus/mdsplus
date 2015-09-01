import subprocess,os,sys,datetime

def shell(cmd,msg):
    print(cmd)
    sys.stdout.flush()
    if subprocess.Popen(cmd,shell=True,cwd='..').wait() != 0:
        raise Exception(msg)

class InstallationPackage(object):
    """Provides exists,build,test and deploy methods"""
    def __init__(self,info):
        self.info=info
        if self.info['flavor']=="stable":
            self.info['pyflavor']=""
            self.info['pkgflavor']=""
        else:
            self.info['pyflavor']="%(flavor)s-" % self.info
            self.info['pkgflavor']="-%(flavor)s" % self.info
        if self.info['flavor'] == "stable":
            self.info['flavor_part']=""
        else:
            self.info['flavor_part']="-%(flavor)s" % self.info

    def exists(self):
        """Determine if package is available for this release."""
        pkgfile="/mdsplus/dist/macosx/%(flavor)s/MDSplus%(flavor_part)s-%(major)d-%(minor)d-%(release)d-osx.pkg" % self.info
        try:
            os.stat(pkgfile)
        except:
            print("%s not found, build required" % pkgfile)
            sys.stdout.flush()
            return False
        return True

    def build(self):
        """Build MacOSX package"""
        self.info['workspace']=os.environ['WORKSPACE']
        print("Building new release %(major)d.%(minor)d-%(release)d" % self.info)
        sys.stdout.flush()
        try:
            os.mkdir("../pkg")
        except:
            pass
        print("%s, Starting build" % str(datetime.datetime.now()))
        sys.stdout.flush()
        shell("""

set -e
./configure --prefix=$(pwd)/build/mdsplus \
            --exec-prefix=$(pwd)/build/mdsplus \
            --with-idl=$IDL_DIR --with-jdk=$JDK_DIR --with-labview=$LABVIEW_DIR

        ""","configure failed")
        shell("""

rm -f lib/*.a
set -e
make clean
make

        """,'make failed')
        shell("""

set -e
export MDSPLUS_VERSION="%(pyflavor)s-%(major)d.%(minor)d_%(release)d"
make install
chmod -R o-w,g-w $(pwd)/build/mdsplus

        """ % self.info,'make install failed')
        shell("""

set -e
cd build/mdsplus/mdsobjects/python
python setup.py bdist_egg version=%(pyflavor)s%(major)d.%(minor)d.%(release)d

        """ % self.info,'python bdist_egg failed')
        shell("""

set -e
sudo chown -R root:admin ./build
rm -f MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg
/Developer/usr/bin/packagemaker --title "MDSplus%(pkgflavor)s" --version "%(major)d.%(minor)d.%(release)d" --scripts $(pwd)/build/mdsplus/scripts \
--install-to "/usr/local" --target "10.5" -r $(pwd)/build -v -i "MDSplus%(pkgflavor)s" -o MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg
sudo chown -R $(id -un):$(id -gn) $(pwd)/build

        """ % self.info,'Building package failed')
        print("%s, Build completed" % str(datetime.datetime.now()))
        sys.stdout.flush()

    def test(self):
        print("Test on macosx not yet implemented, continuing without testing")
        sys.stdout.flush()

    def deploy(self):
        print("Deploying new release %(major)d.%(minor)d-%(release)d" % self.info)
        sys.stdout.flush()
        shell("""

set -e
rsync -a  MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg /mdsplus/dist/macosx/%(flavor)s/

        """ % self.info,'Failed to copy to destination')
        print("Completed deployment")
        sys.stdout.flush()

