import os,subprocess,datetime,sys
from pkg_utils import getWorkspace, getVersion, getRelease, checkRelease, getFlavor, newRelease

def writePkgInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
            '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
            '</body>\n</html>\n')
    f.close()

def shell(cwd,cmd,msg):
  print cmd
  sys.stdout.flush()
  p=subprocess.Popen(cmd,shell=True,cwd=cwd)
  stat=p.wait()
  if stat != 0:
    print msg
    sys.exit(stat)

def makeMacosxPkgCommand(args):
    WORKSPACE=getWorkspace()
    FLAVOR=getFlavor()
    VERSION=getVersion()
    if FLAVOR=="stable":
        pyflavor=""
        pkgflavor=""
    else:
        pyflavor=FLAVOR+"-"
        pkgflavor="-"+FLAVOR
    release=getRelease("macosx")
    release=0
    need_to_build=False
    need_to_tag=False
    if len(checkRelease("macosx")) > 0:
        need_to_build=True
        release=release+1
        need_to_tag=True
    try:
      os.mkdir("%s/pkg" % WORKSPACE)
    except:
      pass
    v=os.uname()[2].split('.')
    pkgfile="%s/pkg/MDSplus%s-%s-%d-osx-%s-%s.pkg" % (WORKSPACE,pkgflavor,VERSION.replace('.','-'),release,v[0],v[1])
    if not need_to_build:
        print "Checking to see if package file, %s, exists" % (pkgfile,)
        try:
            print os.stat(pkgfile)
        except:
            print '%s is missing. Rebuilding.' % (pkgfile,)
            need_to_build=True
    if need_to_build:
       mdsplusdir="%s/mdsplus" % (WORKSPACE,)
       print "%s, Starting build" % (str(datetime.datetime.now()),)
       shell(mdsplusdir,
	     './configure --enable-mdsip_connections --disable-java --exec-prefix=%s/build/mdsplus --with-idl=/Applications/itt/idl/idl' % (WORKSPACE,),
             "configure failed")
       shell(mdsplusdir,'make','make failed')
       shell(mdsplusdir,'rm -Rf ../build/mdsplus; make install','make install failed')
       shell(mdsplusdir,'cd ../build/mdsplus/mdsobjects/python; env MDSPLUS_PYTHON_VERSION=%s%s.%d python setup.py bdist_egg' % (pyflavor,VERSION,release),'python bdist_egg failed')
       shell(mdsplusdir,'rm -f %s; /Developer/usr/bin/packagemaker --title "MDSplus%s" --version "%s.%d" --scripts %s/mdsplus/macosx/scripts --install-to "/usr/local" --target "10.5" -r %s/build -v -i "MDSplus%s" -o %s' % (pkgfile,pkgflavor,VERSION,release,WORKSPACE,WORKSPACE,pkgflavor,pkgfile),'Building package failed')
       print "%s, Setup kit build completed" % (str(datetime.datetime.now()))
       if need_to_tag:
            print "Tag all modules for this release. This can take a while!"
            newRelease('macosx',FLAVOR,VERSION,release,'macosx')
       build_url=os.environ['BUILD_URL']
       writePkgInfo(pkgfile[0:-4])
    sys.stdout.flush()
    shell(WORKSPACE,'rsync -a pkg/* %s' % (args[2],),'Failed to copy to destination')
