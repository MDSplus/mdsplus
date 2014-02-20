import os,subprocess,datetime,sys
from pkg_utils import getWorkspace, getVersion, getRelease, checkRelease, getFlavor, newRelease

#def mkdirhier(outfile):
#  parts=outfile.split('\\')[0:-1]
#  dirname=''
#  for part in parts:
#    dirname=dirname+'\\'+part
#    try:
#      os.mkdir(dirname)
#    except Exception,e:
#      print e

def msiUpdateSetup(FLAVOR,WORKSPACE,VERSION,release,bits,outfile,msiflavor):
    try:
        os.unlink('Setup/Setup%d.vdproj-orig' % (bits,))
    except:
        pass
    os.rename('Setup/Setup%d.vdproj' % (bits,),'Setup/Setup%d.vdproj-orig' % (bits,))
    try:
        os.stat(outfile+".uuid")
    except:
#        mkdirhier(outfile)
	sys.stdout.flush()
        p=subprocess.Popen("uuidgen > %s.uuid" % (outfile,),shell=True)
        stat=p.wait()
        if stat != 0:
            raise Exception("Error generating uuid: %s.uuid" % (outfile,))
    u_in=open("%s.uuid" % (outfile,))
    uuid=u_in.readline()[0:-1]
    u_in.close()
    if bits == 32:
        setupdir="x86"
    else:
        setupdir="x86_64"
    f_in=open('Setup/Setup%d.vdproj-orig' %(bits,),'r')
    f_out=open('Setup/Setup%d.vdproj' %(bits,),'w')
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
            line='        "ProductVersion" = "8:%s.%d"' % (VERSION,release)
        elif '"OutputFilename"' in line:
            line='        "OutputFilename" = "8:%s.msi"' % (outfile.replace('\\','\\\\'),)
        elif '"PostBuildEvent"' in line:
            line='        "PostBuildEvent" = "8:\\"$(ProjectDir)..\\\\devscripts\\\\sign_kit.bat\\" \\"%s\\\\..\\\\%s\\\\%s\\\\%s.%d\\\\Setup.exe\\" \\"$(BuiltOuputPath)\\""' % (WORKSPACE.replace('\\','\\\\'),FLAVOR,setupdir,VERSION,release)
        elif '"Url"' in line:
            line='        "Url" = "8:http://www.mdsplus.org/dist/Windows/%s/%s/%s.%d"' % (FLAVOR,setupdir,VERSION,release)
        print >>f_out,line
        line=f_in.readline()
    f_in.close()
    f_out.close()
    try:
        os.unlink('Setup/Setup%d.vdproj-orig' % (bits,))
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

def makeMsiCommand(args):
    """Make Windows distribution kits."""
    WORKSPACE=getWorkspace()
    print "Workspace begins as: %s" % (WORKSPACE,)
    WORKSPACE=WORKSPACE.replace('/','\\')
    print "Workspace is now: %s" % (WORKSPACE,)
    FLAVOR=getFlavor()
    print "WORKSPACE is %s" % (WORKSPACE,)
    VERSION=getVersion()
    DISTPATH=args[2]+'/Windows/'
    if FLAVOR=="stable":
        msiflavor=""
        pythonflavor=""
    else:
        msiflavor="-"+FLAVOR
        pythonflavor=FLAVOR+"-"
    release=getRelease("windows")
    need_to_build=False
    need_to_tag=False
    if len(checkRelease("windows")) > 0:
        need_to_build=True
        release=release+1
        need_to_tag=True
    try:
      os.mkdir("%s\\..\\%s" % (WORKSPACE,FLAVOR))
    except Exception,e:
      print "Did not create flavor directory, %s" % (e,)
      pass
    for p in ('x86','x86_64'):
        try:
            os.mkdir("%s\\..\\%s\\%s" % (WORKSPACE,FLAVOR,p))
        except:
            pass
        try:
            os.mkdir("%s\\..\\%s\\%s\\%s.%d" % (WORKSPACE,FLAVOR,p,VERSION,release))
        except Exception,e:
            print "Did not create release directory, %s" % (e,)
            pass
    msi32="%s\\..\\%s\\x86\\%s.%s\\MDSplus%s-%s.%d.x86" % (WORKSPACE,FLAVOR,VERSION,release,msiflavor,VERSION,release)
    msi64="%s\\..\\%s\\x86_64\\%s.%s\\MDSplus%s-%s.%d.x86_64" % (WORKSPACE,FLAVOR,VERSION,release,msiflavor,VERSION,release)
    if not need_to_build:
        try:
            os.stat(msi32+".msi")
            try:
                os.stat(msi64+".msi")
            except:
                print '%s missing. Rebuilding.' % (msi64,)
                need_to_build=True
        except:
            print '%s missing. Rebuilding.' % (msi32,)
            need_to_build=True
    status="ok"
    if need_to_build:
        print "%s, Starting build java" % (str(datetime.datetime.now()),)
	sys.stdout.flush()
        p=subprocess.Popen('pushd %s & devenv /build "Release|Java" mdsplus.sln' % (WORKSPACE,),shell=True)
        stat=p.wait()
        print "%s, Java build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (stat != 0):
            print "Build failed!"
            sys.exit(stat)
        print "%s, Starting to build 32-bit apps" % (str(datetime.datetime.now()),)
        sys.stdout.flush()
        p=subprocess.Popen('pushd %s & devenv /build "Release|Win32" mdsplus.sln' % (WORKSPACE,),shell=True)
        stat=p.wait()
        print "%s, 32-bit apps build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (stat != 0):
            print "Build failed!"
            sys.exit(stat)
        print "%s, Starting to build 64-bit apps" % (str(datetime.datetime.now()),)
        sys.stdout.flush()
        p=subprocess.Popen('pushd %s & devenv /build "Release|x64" mdsplus.sln' % (WORKSPACE,),shell=True)
        stat=p.wait()
        print "%s, 64-bit apps build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (stat != 0):
            print "Build failed!"
            sys.exit(stat)
        msiUpdateSetup(FLAVOR,WORKSPACE,VERSION,release,32,msi32,msiflavor)
        print "%s, Starting to build 32-bit setup kit" % (str(datetime.datetime.now()),)
        sys.stdout.flush()
        p=subprocess.Popen('pushd %s & devenv /build "Release|Setup32" mdsplus.sln' % (WORKSPACE,),shell=True)
        stat=p.wait()
        print "%s, 32-bit setup kit build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (stat != 0):
            print "Build failed!"
            sys.exit(stat)
        msiUpdateSetup(FLAVOR,WORKSPACE,VERSION,release,64,msi64,msiflavor)
        print "%s, Starting to build 64-bit setup kit" % (str(datetime.datetime.now()),)
        sys.stdout.flush()
        p=subprocess.Popen('pushd %s & devenv /build "Release|Setup64" mdsplus.sln' % (WORKSPACE,),shell=True)
        stat=p.wait()
        print "%s, 64-bit setup kit build completed with status=%d" % (str(datetime.datetime.now()),stat)
        if (stat != 0):
            print "Build failed!"
            sys.exit(stat)
        if need_to_tag:
            print "Tag all modules for this release. This can take a while!"
            newRelease('windows',FLAVOR,VERSION,release,'win')
        build_url=os.environ['BUILD_URL']
        writeMsiInfo(msi32)
        writeMsiInfo(msi64)    
    sys.stdout.flush()
    print "Done tagging modules. Now copying files to distribution location"
    sys.stdout.flush()
    p=subprocess.Popen('pscp -r -i c:\\Users\\twf\\id_dsa_nop.ppk %s/../%s %s' % (WORKSPACE,FLAVOR,DISTPATH),shell=True)
    sys.exit(p.wait())
