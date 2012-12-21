import os,subprocess,datetime,sys
from pkg_utils import getWorkspace, getVersion, getRelease, checkRelease, getFlavor, newRelease, shell

def writePkgInfo(outfile):
    f=open(outfile+'-info.html','w')
    url="http://hudson.mdsplus.org/job/%s/%s" % (os.environ['JOB_NAME'],os.environ['BUILD_NUMBER'])
    f.write('<html>\n<head>\n'+
            '<meta http-equiv="Refresh" content="0; url=%s" />\n' % (url,) + 
            '</head>\n<body>\n<p>For more info please follow <a href="%s">this link</a>.</p>\n' % (url,)+
            '</body>\n</html>\n')
    f.close()

def makeMacosxPkgCommand(args):
    "Make distribution package for macosx."""
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
    pkgfile="%s/pkg/MDSplus%s-%s-%d-osx.pkg" % (WORKSPACE,pkgflavor,VERSION.replace('.','-'),release)
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
	     './configure --enable-mdsip_connections --prefix=%s/build/mdsplus --exec-prefix=%s/build/mdsplus --with-idl=/Applications/itt/idl/idl' % (WORKSPACE,WORKSPACE,),
             "configure failed")
       shell(mdsplusdir,'rm -f lib/*.a; make','make failed')
       shell(mdsplusdir,'sudo /usr/bin/mdsplus_sudo remove ../build','remove build failed')
       shell(mdsplusdir,'env MDSPLUS_VERSION="%s%s.%d" make install' % (pyflavor,VERSION,release),'make install failed')
       shell(mdsplusdir,'cd ../build/mdsplus/mdsobjects/python; python setup.py bdist_egg version=%s%s.%d' % (pyflavor,VERSION,release),'python bdist_egg failed')
       shell(mdsplusdir,'rsync -a ../build/mdsplus/mdsobjects/python/dist/*.egg %s/%s/EGGS/' % (args[2],FLAVOR),'Failed to copy eggs to destination')
       shell(mdsplusdir,'sudo /usr/bin/mdsplus_sudo chown ../build','chown failed')
       shell(mdsplusdir,'rm -f %s; /Developer/usr/bin/packagemaker --title "MDSplus%s" --version "%s.%d" --scripts %s/build/mdsplus/scripts --install-to "/usr/local" --target "10.5" -r %s/build -v -i "MDSplus%s" -o %s' % (pkgfile,pkgflavor,VERSION,release,WORKSPACE,WORKSPACE,pkgflavor,pkgfile),'Building package failed')
       shell(mdsplusdir,'sudo /usr/bin/mdsplus_sudo remove ../build','remove build failed')
       print "%s, Setup kit build completed" % (str(datetime.datetime.now()))
       if need_to_tag:
            print "Tag all modules for this release. This can take a while!"
            newRelease('macosx',FLAVOR,VERSION,release,'macosx')
       build_url=os.environ['BUILD_URL']
       writePkgInfo(pkgfile[0:-4])
    sys.stdout.flush()
    shell(WORKSPACE,'rsync -a pkg/* %s/%s' % (args[2],FLAVOR),'Failed to copy to destination')
