import subprocess,datetime,os,sys,pexpect

def signrpms(self,arch):
    """Sign the arpms"""
    try:
        cmd="/bin/sh -c 'rpmsign --addsign --define=\"_signature gpg\" --define=\"_gpg_name MDSplus\" *.rpm'"
    	child = pexpect.spawn(cmd,timeout=60,cwd=self.workspace+'/RPMS/'+arch)
    	child.expect("Enter pass phrase: ")
    	child.sendline("")
        child.expect(pexpect.EOF)
    	child.close()
        if child.status != 0:
            raise Exception("Error signing rpms. status=%d" % child.status)
    	return child.status
    except:
        raise Exception("Error signing rpms: %s" % sys.exc_info()[1])

def makeRepoRpms(self):
    if self.flavor == "stable":
      rpmflavor=""
    else:
      rpmflavor="-"+self.flavor
    p=subprocess.Popen('rpmbuild -ba -vv' +\
                ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                ' --define="_topdir %s"' % (self.workspace,)+\
                ' --define="_builddir %s"' % (self.workspace,)+\
                ' --define="flavor %s"' % (self.flavor,)+\
                ' --define="rpmflavor %s"' % (rpmflavor,)+\
                ' --define="s_dist %s"' % (self.dist,)+\
                ' %s/x86_64/mdsplus/rpm/repos.spec' % (self.workspace,),shell=True,cwd=self.topdir)
    rpmbuild_status=p.wait()
    if rpmbuild_status != 0:
        raise Exception("Error building repository rpm for x86_64 %s %s. rpmbuild returned status=%d." % (self.dist,self.flavor,rpmbuild_status))
    else:
        p=subprocess.Popen('rpmbuild -ba -vv'+\
                    ' --target=i686-linux'+\
                    ' --buildroot=$(mktemp -t -d mdsplus-repo-build.XXXXXXXXXX)'+\
                    ' --define="_topdir %s"' % (self.workspace,)+\
                    ' --define="_builddir %s"' % (self.workspace,)+\
                    ' --define="flavor %s"' % (self.flavor,)+\
                    ' --define="rpmflavor %s"' % (rpmflavor,)+\
                    ' --define="s_dist %s"' % (self.dist,)+\
                    ' %s/x86_64/mdsplus/rpm/repos.spec' % (self.workspace,),shell=True,cwd=self.topdir)
        rpmbuild_status=p.wait()
        if rpmbuild_status != 0:
            raise Exception("Error building repository rpm for i686 %s %s. rpmbuild returned status=%d." % (self.dist,self.flavor,rpmbuild_status))

def writeRpmInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write(('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%(url)s" />\n'+ 
            '</head>\n<body>\n<p>For more info please follow <a href="%(url)s">this link</a>.</p>\n'+
            '</body>\n</html>\n') % {'url':url})
    f.close()

def exists(self):
    """Determine if package is available for this release. This becomes a method of the Package class in pkg_utils."""
    if self.flavor == "stable":
        flavor_part=""
    else:
        flavor_part="-%s" % self.flavor
    for pkg in [None]+self.packages:
        if pkg is not None:
            pkg_part="-%s" % pkg
        else:
            pkg_part=""
        rpms=('/repository/%s/%s/RPMS/i686/mdsplus%s%s-%d.%d-%d.%s.i686.rpm' % (self.dist,self.flavor,flavor_part,pkg_part,self.major,self.minor,self.release,self.dist),
              '/repository/%s/%s/RPMS/x86_54/mdsplus%s-%s-%d.%d-%d.%s.x86_64.rpm' % (self.dist,self.flavor,flavor_part,pkg_part,self.major,self.minor,self.release,self.dist))
        for rpm in rpms:
            try:
                os.stat(rpm)
            except:
                self.log("%s not found, build required" % rpm)
                return False
    return True

def build(self):
    """Build Redhat RPMS for the complete package and all the individual subpackages"""
    self.log("Building new release %d.%d-%d" % (self.major,self.minor,self.release))
    rpm_spec_start="""Name: mdsplus%s
Version: %d.%d
Release: %d.%s
License: BSD Open Source
Summary: The MDSplus Data System
Group: Applications/Acquisition
Prefix: /usr/local
Summary: MDSplus Data Acquisition System
AutoReqProv: yes
"""
    rpm_spec_middle="""
%%description
Main libraries and programs to get MDSplus operational

%%build
if [ "%%_target" != "i686-linux" ]
then
  cd ${WORKSPACE}/x86_64/mdsplus
  ./configure --prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin64 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib64 --enable-nodebug --enable-mdsip_connections --with-gsi=/usr:gcc64 --with-labview=$LABVIEW_DIR --with-jdk=$JDK_DIR --with-idl=$IDL_DIR
  make clean
  env LANG=en_US.UTF-8 make
else
  cd ${WORKSPACE}/i686/mdsplus
  ./configure --prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --exec_prefix=$RPM_BUILD_ROOT/usr/local/mdsplus --bindir=$RPM_BUILD_ROOT/usr/local/mdsplus/bin32 --libdir=$RPM_BUILD_ROOT/usr/local/mdsplus/lib32 --enable-nodebug --host=i686-linux --enable-mdsip_connections --with-gsi=/usr:gcc32 --with-labview=$LABVIEW_DIR --with-jdk=$JDK_DIR --with-idl=$IDL_DIR
  make clean
  env LANG=en_US.UTF-8 make
fi

%%install

if [ "%%_target" != "i686-linux" ]
then
  cd ${WORKSPACE}/x86_64/mdsplus
  env MDSPLUS_VERSION="%(pythonflavor)s-%(major)d.%(minor)d.%(release)d" make install
  rsync -a ${WORKSPACE}/i686/mdsplus/bin32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/lib32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/uid32 $RPM_BUILD_ROOT/usr/local/mdsplus/
  rsync -a ${WORKSPACE}/i686/mdsplus/mdsobjects/python/dist $RPM_BUILD_ROOT/usr/local/mdsplus/mdsobjects/python/
else
  cd ${WORKSPACE}/i686/mdsplus
  env MDSPLUS_VERSION="%(pythonflavor)s-%(major)d.%(minor)d.%(release)d" make install
  pushd mdsobjects/python
  python setup.py bdist_egg
  rsync -a dist $RPM_BUILD_ROOT/usr/local/mdsplus/mdsobjects/python/
  popd
fi

%%clean
#rm -rf $RPM_BUILD_ROOT

%%files

"""
    for d in ['RPMS','SPECS','SRPMS','EGGS']:
        try:
            os.mkdir("%s/%s" % (self.workspace,d))
        except:
            pass
    if self.flavor=="stable":
        rpmflavor=""
        pythonflavor=""
    else:
        rpmflavor="-"+self.flavor
        pythonflavor=self.flavor+"-"
    specfile="%s/SPECS/mdsplus%s-%d.%d-%d.%s.spec" % (self.workspace,rpmflavor,self.major,self.minor,self.release,self.dist)
    f=open(specfile,'w')
    f.write(rpm_spec_start % (rpmflavor,self.major,self.minor,self.release,self.dist));
    for pkg in self.packages:
        f.write("requires: mdsplus%s-%s >= %d.%d-%d\n" % (rpmflavor,pkg,self.major,self.minor,self.release))
    f.write(rpm_spec_middle % {'pythonflavor':pythonflavor,'major':self.major,'minor':self.minor,'release':self.release})
    for pkg in self.packages:
        f_in=open('%s/rpm/subpackages/%s' % (self.topdir,pkg),'r')
        for line in f_in:
            line=line.replace("--RELEASE--","%d.%s" % (self.release,self.dist))
            line=line.replace("--RPMFLAVOR--",rpmflavor)
            f.write(line)
    f.close()
    if 'UPDATE_CHANGELOG' in os.environ:
        self.log("Updating ChangeLog")
        p=subprocess.Popen('$(pwd)/devscripts/UpdateChangeLog %s' % (self.flavor,),shell=True,cwd=getTopDir())
        p.wait()
    self.log("%s, Starting to build 32-bit rpms" % (str(datetime.datetime.now()),))
    build_cmds="""
export MDSPLUS_PYTHON_VERSION="%(pythonflavor)s%(major)d.%(minor)d-%(release)d";
scp alchome.psfc.mit.edu:/mnt/scratch/mdsplus/rpm-signing-keys.tgz ~/;tar xfC ~/rpm-signing-keys.tgz ~;
rm -Rf %(workspace)s/RPMS/*;
rpmbuild --target i686-linux --buildroot %(workspace)s/BUILDROOT/i686 -ba \
         --define="_topdir %(workspace)s" --define="_builddir %(workspace)s/i686/mdsplus" %(specfile)s
""" % {'pythonflavor':pythonflavor,'major':self.major,'minor':self.minor,'release':self.release,'workspace':self.workspace,'specfile':specfile}
    p=subprocess.Popen(build_cmds,shell=True,cwd=self.topdir)
    status=p.wait()
    if status != 0:
        raise Exception("Error building i686 %s.%s rpms. rpmbuild returned with status=%d" % (self.flavor,self.dist,status))
    self.log("%s, Done building 32-bit rpms" % str(datetime.datetime.now()))
    self.log("%s, Starting to sign 32-bit rpms" % str(datetime.datetime.now()))
    signrpms(self,'i686')
    self.log("%s, Done signing 32-bit rpms" % str(datetime.datetime.now()))
    for pkg in self.packages:
        writeRpmInfo("%s/RPMS/i686/mdsplus%s-%s-%d.%d-%d.%s.i686" % (self.workspace,rpmflavor,pkg,self.major,self.minor,self.release,self.dist))
    self.log("%s, Starting to build 64-bit rpms" % str(datetime.datetime.now()))
    status=subprocess.Popen(('rpmbuild --target x86_64-linux'+
                            ' --buildroot %(workspace)s/BUILDROOT/x86_64 -ba'+
                           ' --define="_topdir %(workspace)s"'+
                       ' --define="_builddir %(workspace)s/x86_64/mdsplus"'+
                       ' %(specfile)s') % {'workspace':self.workspace,'specfile':specfile},shell=True,cwd=self.topdir).wait()
    if status != 0:
        raise Exception("Error buildin x86_64 %s.%s rpms. rpmbuild returned with status=%d" % (self.flavor,self.dist,status))
    self.log("%s, Done building 64-bit rpms" % str(datetime.datetime.now()))
    self.log("%s, Starting to sign 64-bit rpms" % str(datetime.datetime.now()))
    signrpms(self,'x86_64')
    self.log("%s, Done signing 64-bit rpms" % str(datetime.datetime.now()))
    for pkg in self.packages:
        writeRpmInfo("%s/RPMS/x86_64/mdsplus%s-%s-%d.%d-%d.%s.x86_64" % (self.workspace,rpmflavor,pkg,self.major,self.minor,self.release,self.dist))
    self.log("Updating repository rpms")
    makeRepoRpms(self)
    status=subprocess.Popen('createrepo . >/dev/null',shell=True,cwd=self.workspace+"/RPMS").wait()
    if status != 0:
        raise Exception("Repository creation return status=%d" % (status,))
    self.log("%s, Completed build." % str(datetime.datetime.now()))

def test(self):
    print("Testing new release %d.%d-%d" % (self.major,self.minor,self.release))
    sys.path.insert(0,self.workspace+'/x86_64/mdsplus/tests')
    from distribution_tests import test_rpms as test
    test(self.workspace,self.flavor)
    sys.stdout.flush()


def deploy(self):
    print("Deploying new release %d.%d-%d" % (self.major,self.minor,self.release))
    outpath='/repository/%s/%s' % (self.dist,self.flavor)
    status=subprocess.Popen(('rsync -av %(workspace)s/RPMS %(outpath)s/;'+
                             'rsync -av %(workspace)s/i686/mdsplus/mdsobjects/python/dist/*.egg /repository/EGGS/;'+
                             'rm -Rf %(workspace)s/EGGS') % {'outpath':outpath,'workspace':self.workspace},
                            shell=True).wait()
    if status != 0:
        raise Exception("Error copying files to final destination. Does the directory %s exist and is it writable by the account used by this hudson node?" % (DISTPATH,))
    self.log("Completed deployment")
