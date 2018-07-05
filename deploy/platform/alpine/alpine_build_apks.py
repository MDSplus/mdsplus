#!/usr/bin/env python
# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
import subprocess,os,sys,xml.etree.ElementTree as ET,fnmatch,tempfile,shutil

srcdir=os.path.realpath(os.path.dirname(os.path.realpath(__file__))+'/../../..')
      
def getPackageFiles(buildroot,includes,excludes):
    files=list()
    for f in includes:
        f = buildroot+f
        if os.path.isdir(f):
            for root, dirs, filenams in os.walk(f):
                for filenam in filenams:
                    files.append(os.path.join(root,filenam))
        elif ('?' in f) or ('*' in f):
            dirnam=f
            while ('?' in dirnam) or ('*' in dirnam):
                dirnam=os.path.dirname(dirnam)
            for root, dirs, filenams in os.walk(dirnam):
                if fnmatch.fnmatch(root,f):
                    files=files+getPackageFiles(buildroot,[root[len(buildroot):],],excludes)
                else:
                    for filenam in filenams:
                        filenam=os.path.join(root,filenam)
                        if fnmatch.fnmatch(filenam,f):
                            files.append(filenam)
        else:
            try:
                os.stat(f)
                files.append(f)
            except:
                pass
    if len(excludes) > 0:
        hasuid=False
        for exclude in excludes:
            if '/xuid' in exclude:
                print "excluding: %s" % exclude
                hasuid=True
        excludefiles=getPackageFiles(buildroot,excludes,[])
        if hasuid:
            print "Found %d" % len(excludefiles)
            for exclude in excludefiles:
                print "excluding: %s" % exclude
        for exclude in excludefiles:
            if exclude in files:
                files.remove(exclude)
    return files

def externalPackage(info, root, package):
    ans = None
    for extpackages in root.getiterator('external_packages'):
        platform=extpackages.attrib['platform']
        if platform == "alpine":
            pkg = extpackages.find(package)
            if pkg is not None:
                if 'package' in pkg.attrib:
                    ans = pkg.attrib['package']
                else:
                    ans = package
    return ans

def doRequire(info, out, root, require):
    if 'external' in require.attrib:
        pkg=externalPackage(info,root,require.attrib['package'])
        if pkg is not None:
            os.write(out,"Requires: %s\n" % pkg)
    else:
        info['reqpkg']=require.attrib['package']
    os.write(out,"Depends: mdsplus%(BNAME)s-%(reqpkg)s (>= %(major)d.%(minor)d.%(release)d\n" % info)

def buildApks():
    info=dict()
    info['buildroot']=os.environ['BUILDROOT']
    info['BRANCH']=os.environ['BRANCH']
    version=os.environ['RELEASE_VERSION'].split('.')
    info['dist']=os.environ['DISTNAME']
    info['flavor']=info['BRANCH']
    info['major']=int(version[0])
    info['minor']=int(version[1])
    info['release']=int(version[2])
    if info['BRANCH']=="stable":
        info['BNAME']=""
    else:
        info['BNAME']="-%s" % info['BRANCH']
    info['rflavor']=info['BNAME']
    tree=ET.parse(srcdir+'/deploy/packaging/linux.xml')
    root=tree.getroot()
    apks=list()
    pkg_exclusions=('repo','gsi','gsi_bin','idl','idl_bin',
                    'labview','labview_bin','php','d3d','matlab')
    for package in root.getiterator('package'):
        subprocess.Popen("rm -Rf package_files *.*-install *.*-deinstall *.*-upgrade APKBUILD",
                         shell=True,cwd="/workspace").wait()
        pkg = package.attrib['name']
        if pkg in pkg_exclusions:
            continue
        if pkg=='MDSplus':
            info['packagename']=""
        else:
            info['packagename']="-%s" % pkg
        info['description']=package.attrib['description']
        info['pkg_arch']=package.attrib['arch']
        if info['pkg_arch']=='bin':
            info['pkg_arch']=os.environ['ARCH']
        includes=list()
        for inc in package.getiterator('include'):
            for inctype in inc.attrib:
                include=inc.attrib[inctype]
                if inctype != "dironly":
                    includes.append(include)
        excludes=list()
        for exc in package.getiterator('exclude'):
            for exctype in exc.attrib:
                excludes.append(exc.attrib[exctype])
        if package.find("exclude_staticlibs") is not None:
            excludes.append("/usr/local/mdsplus/lib/*.a")
        if package.find("include_staticlibs") is not None:
            includes.append("/usr/local/mdsplus/lib/*.a")
        files=getPackageFiles(info['buildroot'],includes,excludes)
        for f in files:
            info['file']=f[len("%(buildroot)s/"%info)-1:].replace(' ','\\ ').replace('$','\\$')\
                                        .replace('(','\\(').replace(')','\\)')
            if subprocess.Popen("""
set -o verbose
set -e
dn=$(dirname %(file)s)
mkdir -p "./package_files/${dn}"
cp -dR %(buildroot)s/%(file)s "./package_files${dn}/"
                
""" % info,shell=True,cwd="/workspace").wait() != 0:
                raise Exception("Error building apk")
            sys.stdout.flush()
        depends=list()
        for require in package.getiterator("requires"):
            if 'external' in require.attrib:
                pkg=externalPackage(info,root,require.attrib['package'])
                if pkg is not None:
                    depends.append(pkg)
            else:
                depends.append("mdsplus%s-%s" % (info['rflavor'],require.attrib['package'].replace('_','-')))
        if len(depends)==0:
            info['depends']=''
        else:
            info['depends']=' '.join(depends)
        info['name']=info['packagename'].replace('_','-')
        info['scripts']=""
        scripts=[]
        for s in ("pre-install","post-install","pre-deinstall","post-deinstall",
                  "pre-upgrade","post-upgrade"):
            script=package.find(s)
            if script is not None and ("type" not in script.attrib or script.attrib["type"]!="rpm"):
                info['script-type']=s
                info['script']="mdsplus%(BNAME)s%(name)s.%(script-type)s" % info
                scripts.append(info['script'])
                f=open("/workspace/%(script)s" % info,"w")
                f.write("#!/bin/sh\n")
                f.write(script.text)
                f.close()
                os.chmod("/workspace/%(script)s" % info,0775)
        if len(scripts) > 0:
            info['scripts']='install="%s"' % " ".join(scripts)
        f=open("/workspace/APKBUILD","w")
        f.write("""
# Contributor: MDSplus Developer Team
#Maintainer: Tom Fredian <twf@mdsplus.org> 
pkgname=mdsplus%(BNAME)s%(name)s
pkgver=%(major)d.%(minor)d.%(release)d
pkgrel=0
pkgdesc="%(description)s"
        url="http://www.mdsplus.org/%(dist)s/%(BRANCH)s"
arch="%(pkg_arch)s"
license="MIT"
depends="%(depends)s"
depends_dev=""
makedepends=""
source=""


build() {
  return 0;
}

package() {
        mkdir -p "$pkgdir"
        if [ -r /workspace/package_files/usr/local/mdsplus ]
        then
          rsync -a /workspace/package_files/usr/local/mdsplus "$pkgdir"
        fi
}

%(scripts)s

""" % info)
        f.close()
        if subprocess.Popen("""
abuild checksum >/dev/null 2>&1
        abuild -q -d -P /release/%(BRANCH)s  2>&1 | grep -vi git | grep -v whoami
status=$?
rm -Rf package_files *.*-install *.*-deinstall *.*-upgrade APKBUILD
exit $status
        """ % info,shell=True,cwd="/workspace").wait() != 0:
            raise Exception("Problem building package")
        sys.stdout.flush()


buildApks()
