import subprocess,datetime,os,sys

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

def shell(self,cwd,cmd,msg):
  self.log(cmd)
  status=subprocess.Popen(cmd,shell=True,cwd=cwd).wait()
  stat=p.wait()
  if stat != 0:
      raise Exception(msg)

def exists(self):
    """Determine if package is available for this release. This becomes a method of the Package class in pkg_utils."""
    if self.flavor == "stable":
        flavor_part=""
    else:
        flavor_part="-%s" % self.flavor
    pkgfile="/repository/macosx/%s/MDSplus%s-%d-%d-%d-osx.pkg" % (self.flavor,flavor_part,self.major,
                                                                  self.minor,self.release)
    try:
        os.stat(pkgfile)
    except:
        self.log("%s not found, build required" % pkgfile)
        return False
    return True

def build(self):
    """Build MacOSX package"""
    self.log("Building new release %d.%d-%d" % (self.major,self.minor,self.release))
    try:
      os.mkdir("%s/pkg" % self.workspace)
    except:
      pass
    if self.flavor=="stable":
        pyflavor=""
        pkgflavor=""
    else:
        pyflavor=self.flavor+"-"
        pkgflavor="-"+self.flavor
    self.log("%s, Starting build" % str(datetime.datetime.now()))
    shell(self,self.topdir,
	     ('./configure --enable-mdsip_connections --prefix=%(workspace)s/build/mdsplus '+\
                  '--exec-prefix=%(workspace)s/build/mdsplus '+\
                  '--with-idl=$IDL_DIR --with-jdk=$JDK_DIR --with-labview=$LABVIEW_DIR') % {'workspace':self.workspace},"configure failed")
    shell(self,self.topdir,'rm -f lib/*.a; make clean; make','make failed')
    shell(self,self.topdir,'env MDSPLUS_VERSION="%s%d.%d_%d" make install' % (pyflavor,self.major,self.minor,self.release),'make install failed')
    shell(self,self.workspace+'/build/mdsplus/mdsobjects/python','python setup.py bdist_egg version=%s%d.%d_%d' % (pyflavor,self.major,self.minor,self.release),'python bdist_egg failed')
    shell(self,self.workspace,"""
sudo chown -R root:admin ./build
rm -f MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg
/Developer/usr/bin/packagemaker --title "MDSplus%(pkgflavor)s" --version "%(major)d.$(minor)d.%(release)d" --scripts ./build/mdsplus/scripts \
--install-to "/usr/local" --target "10.5" -r ./build -v -i "MDSplus%(pkgflavor)s" -o MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg
status=$?
sudo chown -R $(id -un):$(id -gn) ./build
exit $status
""" % {'pkgflavor':pkgflavor,'major':self.major,'minor':self.minor,'release':self.release},'Building package failed')
    self.log("%s, Build completed" % str(datetime.datetime.now()))


def test(self):
    self.log("Test on macosx not yet implemented, continuing without testing")

def deploy(self):
    print("Deploying new release %d.%d-%d" % (self.major,self.minor,self.release))
    if self.flavor=="stable":
        pkgflavor=""
    else:
        pkgflavor="-"+self.flavor
    shell(self,self.workspace,'rsync -a ./build/mdsplus/mdsobjects/python/dist/*.egg /repository/EGGS/','Failed to copy eggs to destination')
    try:
        f=open('/repository/macosx/%(flavor)s/MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx-info.html' % {'pkgflavor':pkgflavor,
                                                                                                                     'major':self.major,
                                                                                                                     'minor':self.minor,
                                                                                                                     'release':self.release,
                                                                                                                     'flavor':self.flavor},'w')
        url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
        f.write('<html>\n<head>\n'+
                '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
                '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
                '</body>\n</html>\n')
        f.close
    except:
        raise Exception('Error creating info file')
    shell(self,self.workspace,
          'rsync -a  MDSplus%(pkgflavor)s-%(major)d-%(minor)d-%(release)d-osx.pkg /repository/macosx/%(flavor)s/' % {'pkgflavor':pkgflavor,
                                                                                                                     'major':self.major,
                                                                                                                     'minor':self.minor,
                                                                                                                     'release':self.release,
                                                                                                                     'flavor':self.flavor},
          'Failed to copy to destination')
    self.log("Completed deployment")
