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

linux_xml      = "%s/linux.xml"%(os.path.dirname(os.path.dirname(os.path.realpath(__file__))),)
pkg_exclusions = ('repo','gsi','gsi_bin','idl','idl_bin','labview','labview_bin','php','d3d','matlab')

update_abuildrepo_index = "update_abuildrepo_index() {\nreturn 0;\n}\n"
signkeys = os.getenv("signkeys","")
signkeys = (len(signkeys) > 0) and os.path.exists(signkeys)
     
def getPackageFiles(buildroot,includes,excludes):
    files=[]
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
                print("excluding: %s" % exclude)
                hasuid=True
        excludefiles=getPackageFiles(buildroot,excludes,[])
        if hasuid:
            print "Found %d" % len(excludefiles)
            for exclude in excludefiles:
                print("excluding: %s" % exclude)
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
            break
    return ans

def doRequire(info, out, root, require):
    if 'external' in require.attrib:
        pkg=externalPackage(info,root,require.attrib['package'])
        if pkg is not None:
            os.write(out,"Requires: %s\n" % pkg)
    else:
        info['reqpkg']=require.attrib['package']
    os.write(out,"Depends: mdsplus%(rflavor)s-%(reqpkg)s (>= %(major)d.%(minor)d.%(release)d\n" % info)
def getInfo():
    version = os.environ['RELEASE_VERSION'].split('.')
    branch  = os.environ['BRANCH']
    return {
        'major'     : int(version[0]),
        'minor'     : int(version[1]),
        'release'   : int(version[2]),
        'buildroot' : os.environ['BUILDROOT'],
        'dist'      : os.environ['DISTNAME'],
        'flavor'    : branch,
        'rflavor'   : "-%s"%branch if not branch=="stable" else "",
    }

def run_cmd(cmd,quiet=False):
    if not quiet: print(cmd)
    status = subprocess.Popen(cmd,shell=True,cwd="/workspace").wait()
    sys.stdout.flush()
    return status == 0

def clean_ws():
    run_cmd("rm -Rf package_files *-install *-deinstall *-upgrade APKBUILD")

def getFiles(info,package):
    includes=[]
    for inc in package.getiterator('include'):
        for inctype in inc.attrib:
            include=inc.attrib[inctype]
            if inctype != "dironly":
                includes.append(include)
    excludes=[]
    for exc in package.getiterator('exclude'):
        for exctype in exc.attrib:
            excludes.append(exc.attrib[exctype])
    if package.find("exclude_staticlibs") is not None:
       excludes.append("/usr/local/mdsplus/lib/*.a")
    if package.find("include_staticlibs") is not None:
        includes.append("/usr/local/mdsplus/lib/*.a")
    return getPackageFiles(info['buildroot'],includes,excludes)

def collectFiles(info,package):
    files = getFiles(info,package)
    cmd = 'dn=$(dirname %%s)&&mkdir -p "./package_files/${dn}"&&cp -dR %(buildroot)s/%%s "./package_files/${dn}/"'% info
    clean_ws()
    for f in files:
        file = f[len("%(buildroot)s/"%info):].replace(' ','\\ ').replace('$','\\$').replace('(','\\(').replace(')','\\)')
        if not run_cmd(cmd%(file,file),True):
            raise Exception("Error collecting apk: %s"%(f,))

def getDependencies(info, root, package):
        depends=[]
        for require in package.getiterator("requires"):
            if 'external' in require.attrib:
                pkg = externalPackage(info,root,require.attrib['package'])
                if pkg is not None:
                    depends.append(pkg)
            else:
                depends.append("mdsplus%s-%s" % (info['rflavor'],require.attrib['package'].replace('_','-')))
        return ' '.join(depends)

def getScripts(info, package):
    scriptname = "mdsplus%(rflavor)s%(name)s.%%s"%info
    scripts = []
    for s in ("pre-install","post-install","pre-deinstall","post-deinstall","pre-upgrade","post-upgrade"):
        scriptcls = package.find(s)
        if scriptcls is not None and ("type" not in scriptcls.attrib or scriptcls.attrib["type"]!="rpm"):
            script = scriptname%s
            scripts.append(script)
            with open("/workspace/%s" % script,"w+") as f:
                f.write("#!/bin/sh\n")
                f.write(scriptcls.text)
            os.chmod("/workspace/%s" % script,0775)
    return 'install="%s"' % " ".join(scripts)

def buildApks():
    info = getInfo()
    tree = ET.parse(linux_xml)
    root = tree.getroot()
    apks=[]
    for package in root.getiterator('package'):
        pkg = package.attrib['name']
        if pkg in pkg_exclusions: continue
        info['packagename'] = "-%s"%pkg if not pkg=='MDSplus' else ""
        info['description'] = package.attrib['description']
        info['pkg_arch']    = package.attrib['arch']
        if info['pkg_arch']=='bin': info['pkg_arch']=os.environ['ARCH']
        collectFiles(info,package)
        info['depends'] = getDependencies(info, root, package)
        info['name']    = info['packagename'].replace('_','-')
        scripts = getScripts(info, package)
        with open("/workspace/APKBUILD","w+") as f:
            f.write("# Contributor: MDSplus Developer Team\n")
            f.write("#Maintainer: Tom Fredian <twf@mdsplus.org>\n")
            f.write("pkgname=mdsplus%(rflavor)s%(name)s\n"%info)
            f.write("pkgver=%(major)d.%(minor)d.%(release)d\n"%info)
            f.write("pkgrel=0\n")
            f.write('pkgdesc="%(description)s"\n'%info)
            f.write('url="http://www.mdsplus.org/%(dist)s/%(flavor)s"\n'%info)
            f.write('arch="%(pkg_arch)s"\n'%info)
            f.write('license="MIT"\n')
            f.write('depends="%(depends)s"\n'%info)
            f.write('depends_dev=""\n')
            f.write('makedepends=""\n')
            f.write('source=""\n')
            if not signkeys: f.write(update_abuildrepo_index)
            f.write('build() {\nreturn 0;\n}\n')
            f.write('check() {\nreturn 0;\n}\n')
            f.write('git() {\nreturn 0;\n}\n')
            f.write("""package() {
mkdir -p "$pkgdir"
  if [ -r /workspace/package_files/usr/local/mdsplus ]
  then rsync -a /workspace/package_files/usr/local/mdsplus "$pkgdir"
  fi
}
""") #""" for nano
            f.write(scripts)
        if not run_cmd("abuild checksum && abuild -cqdP /release/%(flavor)s" % info):
            raise Exception("Problem building package")
    clean_ws()


buildApks()
