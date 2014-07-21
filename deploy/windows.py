import subprocess,os,sys

class InstallationPackage(object):
    """Provides exists,build,test and deploy methods"""
    def __init__(self,info):
        self.info=info
        self.info['workspace']=os.environ['WORKSPACE']

    def exists(self):
        """Check to see if install kit for this release already exist."""
        for arch in ('x86_64','x86'):
            self.info['arch']=arch
            kit="/repository/Windows/%(flavor)s/%(arch)s/MDSplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.exe" % self.info
            try:
                os.stat(kit)
            except:
                print("%s not found" % kit)
                sys.stdout.flush()
                return False
        return True

    def build(self):
        """Build MDSplus from the sources and install into a 'flavor' directory"""
        status = subprocess.Popen("""
set -e
rm -Rf %(workspace)s/%(flavor)s
mkdir -p %(workspace)s/%(flavor)s
cd ..
./configure --host=x86_64-w64-mingw32 --build=x86_64-redhat-linux-gnu --target=x86_64-w64-mingw32 \
        --prefix=%(workspace)s/%(flavor)s --exec-prefix=%(workspace)s/%(flavor)s \
        --libdir=%(workspace)s/%(flavor)s/bin_x86_64 \
        --bindir=%(workspace)s/%(flavor)s/bin_x86_64 --enable-mdsip_connections --with-labview=$LABVIEW_DIR \
        --with-jdk=$JDK_DIR --with-idl=$IDL_DIR
make clean
make
make install
./configure --host=i686-w64-mingw32 --build=i686-redhat-linux-gnu --target=i686-w64-mingw32 \
        --prefix=%(workspace)s/%(flavor)s --exec-prefix=%(workspace)s/%(flavor)s \
        --libdir=%(workspace)s/%(flavor)s/bin_x86 \
        --bindir=%(workspace)s/%(flavor)s/bin_x86 --enable-mdsip_connections --with-labview=$LABVIEW_DIR \
        --with-jdk=$JDK_DIR --with-idl=$IDL_DIR
make clean
make
make install
pushd %(workspace)%(flavor)s
makensis -DMAJOR=%(major)s -DMINOR=%(minor)s -DRELEASE=%(release)s -DFLAVOR=%(rflavor)s -NOCD \
        -DOUTDIR=%(workspace)s/%(flavor)s %(workspace)s/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d/deploy/mdsplus.nsi 
""" % self.info,shell=True).wait()
        if status != 0:
            raise Exception("Error building windows kit for package mdsplus%(rflavor).%(major)d.%(minor)d-%(release)d.exe" % self.info)
        print("Done building mdsplus%(rflavor).%(major)d.%(minor)d-%(release)d.exe" % self.info)

    def test(self):
        return

    def deploy(self):
        """Deploy release to repository"""
        return
        if subprocess.Popen("""
rsync -a %(workspace)s/%(flavor)s/RPMS /repository/%(dist)s/%(flavor)s/
""" % self.info,shell=True).wait() != 0:
            raise Exception("Error deploying %(flavor)s release to repository" % self.info)
        if subprocess.Popen("""
if ( which python3 > /dev/null 2>&1 )
then
  python3 setup.py -q bdist_egg -d /repository/EGGS
fi
python setup.py -q bdist_egg -d /repository/EGGS
""" % self.info,shell=True,cwd="%(workspace)s/%(flavor)s/%(flavor)s/usr/local/mdsplus/mdsobjects/python" % self.info).wait() != 0:
            raise Exception("Error deploying python release egg to repository" % self.info)
