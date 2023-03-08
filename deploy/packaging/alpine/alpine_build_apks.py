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
import subprocess
import os
import sys
import fnmatch


srcdir = os.path.realpath(os.path.dirname(__file__)+'/../../..')
sys.path.insert(0, os.path.join(srcdir, 'deploy', 'packaging'))
import linux_build_packages as common

pkg_exclusions = ('repo', 'gsi', 'gsi_bin', 'idl', 'idl_bin',
                  'labview', 'labview_bin', 'php', 'd3d', 'matlab')
noarch_builder = "x86_64"


def signkeys():
    with open("/workspace/.abuild/abuild.conf", "r") as f:
        line = f.readline()
        while len(line) > 0:
            if line.startswith("PACKAGER_PRIVKEY="):
                line = line.split("=", 2)[1].strip('"\n')
                if os.path.exists(line):
                    print("Found private key: %s" % line)
                    line = "/etc/apk/keys/%s.pub" % os.path.basename(line)
                    if os.path.exists(line):
                        print("Found public key: %s" % line)
                        return True
                break
            line = f.readline()
        return False


signkeys = signkeys()


def getPackageFiles(buildroot, includes, excludes):
    files = []
    for f in includes:
        f = buildroot+f
        if os.path.isdir(f):
            for root, dirs, filenams in os.walk(f):
                for filenam in filenams:
                    files.append(os.path.join(root, filenam))
        elif ('?' in f) or ('*' in f):
            dirnam = f
            while ('?' in dirnam) or ('*' in dirnam):
                dirnam = os.path.dirname(dirnam)
            for root, dirs, filenams in os.walk(dirnam):
                if fnmatch.fnmatch(root, f):
                    files = files + \
                        getPackageFiles(
                            buildroot, [root[len(buildroot):], ], excludes)
                else:
                    for filenam in filenams:
                        filenam = os.path.join(root, filenam)
                        if fnmatch.fnmatch(filenam, f):
                            files.append(filenam)
        else:
            try:
                os.stat(f)
                files.append(f)
            except:
                pass
    if len(excludes) > 0:
        hasuid = False
        for exclude in excludes:
            if '/xuid' in exclude:
                print("excluding: %s" % exclude)
                hasuid = True
        excludefiles = getPackageFiles(buildroot, excludes, [])
        if hasuid:
            print("Found %d" % len(excludefiles))
            for exclude in excludefiles:
                print("excluding: %s" % exclude)
        for exclude in excludefiles:
            if exclude in files:
                files.remove(exclude)
    return files


def doRequire(info, out, root, require):
    if 'external' in require.attrib:
        pkg = common.external_package(info, root, require.attrib['package'])
        if pkg:
            os.write(out, "Depends: %s\n" % pkg)
    else:
        info['reqpkg'] = require.attrib['package']
    os.write(
        out, "Depends: mdsplus%(bname)s-%(reqpkg)s (>= %(major)d.%(minor)d.%(release)d\n" % info)


def run_cmd(cmd, quiet=False):
    if not quiet:
        print(cmd)
    status = subprocess.Popen(cmd, shell=True, cwd="/workspace").wait()
    sys.stdout.flush()
    return status == 0


def clean_ws():
    run_cmd("rm -Rf package_files *-install *-deinstall *-upgrade APKBUILD")


def getFiles(info, package):
    includes = []
    for inc in package.iter('include'):
        for inctype in inc.attrib:
            include = inc.attrib[inctype]
            if inctype != "dironly":
                includes.append(include)
    excludes = []
    for exc in package.iter('exclude'):
        for exctype in exc.attrib:
            excludes.append(exc.attrib[exctype])
    if package.find("exclude_staticlibs") is not None:
        excludes.append("/usr/local/mdsplus/lib/*.a")
    if package.find("include_staticlibs") is not None:
        includes.append("/usr/local/mdsplus/lib/*.a")
    return getPackageFiles(info['buildroot'], includes, excludes)


def collectFiles(info, package):
    files = getFiles(info, package)
    cmd = 'dn=$(dirname %%s)&&mkdir -p "./package_files/${dn}"&&cp -dR %(buildroot)s/%%s "./package_files/${dn}/"' % info
    clean_ws()
    for f in files:
        file = f[len("%(buildroot)s/" % info):].replace(' ',
                                                        '\\ ').replace('$', '\\$').replace('(', '\\(').replace(')', '\\)')
        if not run_cmd(cmd % (file, file), True):
            raise Exception("Error collecting apk: %s" % (f,))


def getDependencies(info, root, package):
    depends = []
    for require in package.iter("requires"):
        if 'external' in require.attrib:
            pkg = common.external_package(info, root, require.attrib['package'])
            if pkg:
                depends.append(pkg)
        else:
            depends.append(
                "mdsplus%s-%s" % (info['bname'], require.attrib['package'].replace('_', '-')))
    return ' '.join(depends)


def getScripts(info, package):
    scriptname = "mdsplus%(bname)s%(name)s.%%s" % info
    scripts = []
    for s in ("pre-install", "post-install", "pre-deinstall", "post-deinstall", "pre-upgrade", "post-upgrade"):
        scriptcls = package.find(s)
        if scriptcls and ("type" not in scriptcls.attrib or scriptcls.attrib["type"] != "rpm"):
            script = scriptname % s
            scripts.append(script)
            with open("/workspace/%s" % script, "w+") as f:
                f.write("#!/bin/sh\n")
                f.write(scriptcls.text)
            os.chmod("/workspace/%s" % script, 0775)
    return 'install="%s"' % " ".join(scripts)


def build():
    info = common.get_info()
    root = common.get_root()
    noarch = "noarch"
    archdir = "/release/%(flavor)s/%(arch)s" % info
    noarchdir = "/release/%(flavor)s/noarch" % info
    os.system("rm -Rf %s/*" % archdir)
    if info['arch'] == noarch_builder:
        os.system("rm -Rf %s" % noarchdir)
        os.system("mkdir -p %s" % noarchdir)
    for package in root.iter('package'):
        pkg = package.attrib['name']
        if pkg in pkg_exclusions:
            continue
        info['packagename'] = "-%s" % pkg if not pkg == 'MDSplus' else ""
        info['description'] = package.attrib['description']
        info['depends'] = getDependencies(info, root, package)
        info['name'] = info['packagename'].replace('_', '-')
        # decide whether to build noarch package or skip
        info['pkg_arch'] = package.attrib['arch']
        if info['pkg_arch'] == noarch:
            if info['arch'] != noarch_builder:
                continue
        elif info['pkg_arch'] == "bin":
            info['pkg_arch'] = info['arch']
        print(info['pkg_arch'])
        collectFiles(info, package)
        scripts = getScripts(info, package)
        with open("/workspace/APKBUILD", "w+") as f:
            f.write("# Contributor: MDSplus Developer Team\n")
            f.write("#Maintainer: Tom Fredian <twf@mdsplus.org>\n")
            f.write("pkgname=mdsplus%(bname)s%(name)s\n" % info)
            f.write("pkgver=%(major)d.%(minor)d\n" % info)
            f.write("pkgrel=%(release)d\n" % info)
            f.write('pkgdesc="%(description)s"\n' % info)
            f.write('url="http://www.mdsplus.org/%(dist)s/%(flavor)s/"\n' % info)
            f.write('arch="%(pkg_arch)s"\n' % info)
            f.write('license="MIT"\n')
            f.write('depends="%(depends)s"\n' % info)
            f.write('depends_dev=""\n')
            f.write('makedepends=""\n')
            f.write('source=""\n')
            if not signkeys:  # if keys not installed, override indexing method to avoid ERROR
                f.write(
                    "update_abuildrepo_index() {\necho 'Skipping update_abuildrepo_index()'\nreturn 0;\n}\n")
            f.write('build() {\nreturn 0;\n}\n')
            f.write('check() {\nreturn 0;\n}\n')
            f.write('git() {\nreturn 0;\n}\n')
            f.write("""package() {
mkdir -p "$pkgdir"
  if [ -r /workspace/package_files/usr/local/mdsplus ]
  then rsync -a /workspace/package_files/usr/local/mdsplus "$pkgdir"
  fi
}
""")  # """ for nano
            f.write(scripts)
        if not run_cmd("abuild checksum && abuild -cqdP /release/%(flavor)s" % info):
            raise Exception("Problem building package")
        if info['pkg_arch'] == noarch:
            fnm = "mdsplus%(bname)s%(name)s-%(major)d.%(minor)d-r%(release)d.apk" % info
            os.rename("%s/%s" % (archdir, fnm), "%s/%s" % (noarchdir, fnm))
    clean_ws()

if __name__ == "__main__":
    build()
