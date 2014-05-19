import os,subprocess,datetime

def exists(self):
    """See if installation kit exists for this flavor and version"""
    status = subprocess.Popen("""
NET USE Z: /DELETE
NET USE Z: \\alchome\mdsplus-dist
""",shell=True).wait()
    if status != 0:
        raise Exception("Error mounting repository")
    try:
        os.stat("Z:\%s\%d.%d.%d" % (self.flavor,self.major,self.minor,self.release))
        return True
    except:
        return False

def msiUpdateSetup(self,bits,msiflavor,outfile):
    try:
        os.unlink('Setup/Setup%d.vdproj-orig' % bits)
    except:
        pass
    os.rename('Setup/Setup%d.vdproj' % bits, 'Setup/Setup%d.vdproj-orig' % bits )
    try:
        os.stat(outfile+".uuid")
    except:
        status=subprocess.Popen("uuidgen > %s.uuid" % (outfile,),shell=True).wait()
        if status != 0:
            raise Exception("Error generating uuid: %s.uuid" % (outfile,))
    u_in=open("%s.uuid" % (outfile,))
    uuid=u_in.readline()[0:-1]
    u_in.close()
    if bits == 32:
        setupdir="x86"
    else:
        setupdir="x86_64"
    f_in=open('Setup/Setup%d.vdproj-orig' % bits ,'r')
    f_out=open('Setup/Setup%d.vdproj' % bits ,'w')
    line=f_in.readline()
    mdsplus_product=False
    while len(line) > 0:
        line=line[0:-2]
        if '"ProductName"' in line:
            if "MDSplus" in line:
                mdsplus_product=True
                line='        "ProductName" = "8:MDSplus%s"' % (msiflavor,)
            else:
                mdsplus_product=False
        elif '"ProductCode"' in line and mdsplus_product:
            line='        "ProductCode" = "8:{%s}"' % (uuid.upper(),)
        elif '"ProductVersion"' in line:
            line='        "ProductVersion" = "8:%d.%d.%d"' % (self.major,self.minor,self.release)
        elif '"OutputFilename"' in line:
            line='        "OutputFilename" = "8:%s.msi"' % (outfile.replace('\\','\\\\'),)
        elif '"PostBuildEvent"' in line:
            line='        "PostBuildEvent" = "8:\\"$(ProjectDir)..\\\\devscripts\\\\sign_kit.bat\\" \\"%s\\\\..\\\\%s\\\\%s\\\\%d.%d.%d\\\\Setup.exe\\" \\"$(BuiltOuputPath)\\""' % (self.workspace.replace('\\','\\\\'),self.flavor,setupdir,self.major,self.minor,self.release)
        elif '"Url"' in line:
            line='        "Url" = "8:http://www.mdsplus.org/dist/Windows/%s/%s/%d.%d.%d"' % (self.flavor,setupdir,self.major,self.minor,self.release)
        print >>f_out,line
        line=f_in.readline()
    f_in.close()
    f_out.close()
    try:
        os.unlink('Setup/Setup%d.vdproj-orig' % bits )
    except:
        pass

def writeMsiInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
            '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
            '</body>\n</html>\n')
    f.close()

def build(self):
    """Make Windows distribution kits."""
    self.workspace=self.workspace.replace('/','\\')
    if self.flavor=="stable":
        msiflavor=""
        pythonflavor=""
    else:
        msiflavor="-"+self.flavor
        pythonflavor=self.flavor+"-"
    try:
      os.mkdir("%s\\..\\%s" % (self.workspace,self.flavor))
    except Exception:
      pass
    for p in ('x86','x86_64'):
        try:
            os.mkdir("%s\\..\\%s\\%s" % (self.workspace,self.flavor,p))
        except:
            pass
        try:
            os.mkdir("%s\\..\\%s\\%s\\%d.%d.%d" % (self.workspace,self.flavor,p,self.major,self.minor,self.release))
        except Exception,e:
            pass
    msi32="%(workspace)s\\..\\%(flavor)s\\x86\\%(major)d.%(minor)d.%(release)d\\MDSplus%(msiflavor)s-%(major)d.%(minor)d.%(release)d.x86" % \
        {'workspace':self.workspace,'flavor':self.flavor,'major':self.major,'minor':self.minor,'release':self.release,'msiflavor':msiflavor}
    msi64="%(workspace)s\\..\\%(flavor)s\\x86_64\\%(major)d.%(minor)d.%(release)d\\MDSplus%(msiflavor)s-%(major)d.%(minor)d.%(release).x86_64" % \
        {'workspace':self.workspace,'flavor':self.flavor,'major':self.major,'minor':self.minor,'release':self.release,'msiflavor':msiflavor}
    self.log("%s, Starting build of java apps" % str(datetime.datetime.now()))
    status=subprocess.Popen('devenv /build "Release|Java" mdsplus.sln',shell=True,cwd=self.workspace).wait()
    if status != 0:
        raise Exception('Build failed')
    self.log("%s, Java build completed" % str(datetime.datetime.now()))
    self.log("%s, Starting to build 32-bit apps" % str(datetime.datetime.now()))
    status=subprocess.Popen('devenv /build "Release|Win32" mdsplus.sln',shell=True,cwd=self.workspace).wait()
    if statis != 0:
        raise Exception('Build falied')
    self.log("%s, 32-bit apps build completed" % str(datetime.datetime.now()))
    self.log("%s, Starting to build 64-bit apps" % str(datetime.datetime.now()))
    status=subprocess.Popen('devenv /build "Release|x64" mdsplus.sln',shell=True,cwd=self.workspace).wait()
    if status != 0:
        raise Exception('Build failed')
    self.log("%s, 64-bit apps build completed" % str(datetime.datetime.now()))
    msiUpdateSetup(self,32,msiflavor,msi32)
    self.log("%s, Starting to build 32-bit setup kit" % str(datetime.datetime.now()))
    status=subprocess.Popen('devenv /build "Release|Setup32" mdsplus.sln',shell=True,cwd=self.workspace).wait()
    if status != 0:
        raise Exception('Build failed')
    self.log("%s, 32-bit setup kit build completed" % str(datetime.datetime.now()))
    msiUpdateSetup(self,64,msiflavor,msi64)
    self.log("%s, Starting to build 64-bit setup kit" % str(datetime.datetime.now()))
    status=subprocess.Popen('devenv /build "Release|Setup64" mdsplus.sln',shell=True,cwd=self.workspace).wait()
    if status != 0:
        raise Exception('Build failed')
    self.log("%s, 64-bit setup kit build completed" % str(datetime.datetime.now()))
    writeMsiInfo(msi32)
    writeMsiInfo(msi64)

def test(self):
    self.log("No automated testing for windows. Skipping")

def deploy(self):
    status=subprocess.Popen('xcopy /Y/I/E %s\\..\\%s Z:\\' % (self.workspace,self.flavor),shell=True).wait()
    if status != 0:
        raise Exception('Error during deployment to repository')
