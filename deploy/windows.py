import subprocess,os,sys

class InstallationPackage(object):
    """Provides exists,build,test and deploy methods"""
    def __init__(self,info):
        self.info=info

    def exists(self):
        """Check to see if install kit for this release already exist."""
        kit="/mdsplus/dist/Windows/%(flavor)s/MDSplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.exe" % self.info
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
rm -Rf /tmp/%(flavor)s
mkdir -p /tmp/%(flavor)s
cd ..
./configure --host=x86_64-w64-mingw32 --build=x86_64-redhat-linux-gnu --target=x86_64-w64-mingw32 \
        --prefix=/tmp/%(flavor)s --exec-prefix=/tmp/%(flavor)s \
        --libdir=/tmp/%(flavor)s/bin_x86_64 \
        --bindir=/tmp/%(flavor)s/bin_x86_64 --with-labview=$LABVIEW64_DIR \
        --with-jdk=$JDK_DIR --with-idl=$IDL64_DIR --with-java_target=6 --with-java_bootclasspath=$(pwd)/rt.jar \
        --with-visual_studio=/mdsplus/visual-studio-2008
make clean
make
make install
./configure --host=i686-w64-mingw32 --build=i686-redhat-linux-gnu --target=i686-w64-mingw32 \
        --prefix=/tmp/%(flavor)s --exec-prefix=/tmp/%(flavor)s \
        --libdir=/tmp/%(flavor)s/bin_x86 \
        --bindir=/tmp/%(flavor)s/bin_x86 --with-labview=$LABVIEW_DIR \
        --with-jdk=$JDK_DIR --with-idl=$IDL_DIR --with-java_target=6 --with-java_bootclasspath=$(pwd)/rt.jar \
        --with-visual_studio=/mdsplus/visual-studio-2008
make clean
make
make install
makedir=$(pwd)
pushd /tmp/%(flavor)s
makensis -DMAJOR=%(major)d -DMINOR=%(minor)d -DRELEASE=%(release)d -DFLAVOR=%(rflavor)s -NOCD \
        -DOUTDIR=/tmp/%(flavor)s -DVisualStudio ${makedir}/deploy/mdsplus.nsi
echo mdsplus | signcode -spc /mdsplus/certs/mdsplus.spc \
         -v /mdsplus/certs/mdsplus.pvk \
         -a sha1 \
         -$ individual \
         -n MDSplus  \
         -i http://www.mdsplus.org/ \
         -t http://timestamp.verisign.com/scripts/timestamp.dll \
         -tr 10 /tmp/%(flavor)s/MDSplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.exe
popd
""" % self.info,shell=True).wait()
        if status != 0:
            raise Exception("Error building windows kit for package mdsplus%(rflavor)s.%(major)d.%(minor)d-%(release)d.exe" % self.info)
        print("Done building mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.exe" % self.info)

    def test(self):
        return

    def deploy(self):
        """Deploy release to /mdsplus/dist"""
        self.info['job_name']=os.environ['JOB_NAME']
        self.info['build_number']=os.environ['BUILD_NUMBER']
        if subprocess.Popen("""
rsync -a /tmp/%(flavor)s/MDSplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.exe /mdsplus/dist/Windows/%(flavor)s/
cat - > /mdsplus/dist/Windows/%(flavor)s/hudson_build%(rflavor)s-%(major)d.%(minor)d-%(release)d.html <<EOF
<html>
<head>
<meta http-equiv="Refresh" content="0; url=http://hudson.mdsplus.org/job/%(job_name)s/%(build_number)s" />
</head>
<body>
<p>For build information please follow <a href="http://hudson.mdsplus.org/job/%(job_name)s/%(build_number)s">this link</a></p>
</body>
</html>
EOF
""" % self.info,shell=True).wait() != 0:
            raise Exception("Error deploying %(flavor)s release to /mdsplus/dist" % self.info)
