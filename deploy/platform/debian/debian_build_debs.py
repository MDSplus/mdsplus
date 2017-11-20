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
        if info['platform']==platform:
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

def buildDebs():
    info=dict()
    info['buildroot']=os.environ['BUILDROOT']
    info['BRANCH']=os.environ['BRANCH']
    version=os.environ['RELEASE_VERSION'].split('.')
    info['dist']=os.environ['DISTNAME']
    info['platform']=os.environ['PLATFORM']
    info['arch']=os.environ['ARCH']
    info['flavor']=info['BRANCH']
    info['major']=int(version[0])
    info['minor']=int(version[1])
    info['release']=int(version[2])
    info['BNAME']=os.environ['BNAME']
    info['rflavor']=info['BNAME']
    tree=ET.parse(srcdir+'/deploy/packaging/linux.xml')
    root=tree.getroot()
    debs=list()
    for package in root.getiterator('package'):
        pkg = package.attrib['name']
        if pkg=='MDSplus':
            info['packagename']=""
        else:
            info['packagename']="-%s" % pkg
        info['description']=package.attrib['description']
        info['tmpdir']=tempfile.mkdtemp()
        try:
            os.mkdir("%(tmpdir)s/DEBIAN" % info)
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
mkdir -p %(tmpdir)s/DEBIAN
mkdir -p "%(tmpdir)s/${dn}"
cp -a %(buildroot)s/%(file)s "%(tmpdir)s/${dn}/"
""" % info,shell=True).wait() != 0:
                    raise Exception("Error building deb")
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
                info['depends']="\nDepends: %s" % ','.join(depends)
            info['name']=info['packagename'].replace('_','-')
            f=open("%(tmpdir)s/DEBIAN/control" % info,"w")
            f.write("""Package: mdsplus%(rflavor)s%(name)s
Version: %(major)d.%(minor)d.%(release)d
Section: admin
Priority: optional
Architecture: %(arch)s%(depends)s
Maintainer: Tom Fredian <twf@www.mdsplus.org>
Description: %(description)s
""" % info)
            f.close()
            for s in ("preinst","postinst","prerm","postrm"):
                script=package.find(s)
                if script is not None and ("type" not in script.attrib or script.attrib["type"]!="rpm"):
                    info['script']=s
                    f=open("%(tmpdir)s/DEBIAN/%(script)s" % info,"w")
                    f.write("#!/bin/bash\n")
                    f.write("%s" % (script.text.replace("__INSTALL_PREFIX__","/usr/local")))
                    f.close()
                    os.chmod("%(tmpdir)s/DEBIAN/%(script)s" % info,0775)
            info['debfile']="/release/%(flavor)s/DEBS/%(arch)s/mdsplus%(rflavor)s%(packagename)s_%(major)d.%(minor)d.%(release)d_%(arch)s.deb" % info
            if subprocess.Popen("""
set -o verbose
set -e
dpkg-deb --build %(tmpdir)s %(debfile)s
""" % info,shell=True).wait() != 0:
                raise Exception("Problem building package")
            sys.stdout.flush()
            debs.append({"deb":info["debfile"],"arch":info["arch"]})
        finally:
            shutil.rmtree("%(tmpdir)s" % info)

buildDebs()
