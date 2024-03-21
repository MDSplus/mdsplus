#!/bin/env python
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
import traceback
import subprocess
import os
import sys
import pexpect
import tempfile

srcdir = os.path.realpath(os.path.dirname(__file__)+'/../../..')
sys.path.insert(0, os.path.join(srcdir, 'deploy', 'packaging'))
import linux_build_packages as common

rpmspec = """
%%define debug_package %%{nil}
%%global _missing_build_ids_terminate_build 0
%%global _unpackaged_files_terminate_build 0
Name: mdsplus%(bname)s%(packagename)s
Version: %(major)d.%(minor)d
Release: %(release)s.%(dist)s
License: BSD Open Source - Copyright (c) 2010, Massachusetts Institute of Technology All rights reserved.
Source: https://github.com/MDSplus/mdsplus/archive/%(branch)s_release-%(major)d-%(minor)d-%(release)d.tar.gz
URL: http://www.mdsplus.org
Vendor: Massachusetts Institute of Technology
Packager: Plasma Science and Fusion Center <mdsplus@www.mdsplus.org>
Summary: %(summary)s
Group: Applications/Archiving
Prefix: /usr/local
AutoReqProv: yes
"""

pckspec = """
%%description
%(description)s
%%clean
%%files
%%defattr(-,root,root)
"""

def fixFilename(info, filename):
    """For binary packages we need to refer to /libnn, /binnn and /uidnn where nn is 32 or 64. For
    noarch packages we need to use /lib??, /bin?? to get the no arch dependent scripts and files"""
    try:
        if info['arch'] == 'noarch':
            filename = filename.replace("/lib/", "/lib??/")
            filename = filename.replace("/bin/", "/bin??/")
            if filename[len(filename)-4:] in ('/lib', '/bin'):
                filename = filename+"??"
        else:
            filename = filename.replace("/lib/", "/lib%(bits)d/")
            filename = filename.replace("/bin/", "/bin%(bits)d/")
            filename = filename.replace("/uid/", "/uid%(bits)d/")
            if filename[len(filename)-4:] in ('/lib', '/bin', '/uid'):
                filename = filename+"%(bits)d"
        ans = filename % info
    except Exception:
        traceback.print_exc()
        raise Exception("Error fixing filename %s:" % (filename,))
    return ans


def doRequire(info, out, root, require):
    if 'external' in require.attrib:
        pkg = common.external_package(info, root, require.attrib['package'])
        if pkg:
            common.writeb(out, "Requires: %s\n" % pkg)
    else:
        info['reqpkg'] = require.attrib['package']
        common.writeb(out, "Requires: mdsplus%(bname)s-%(reqpkg)s = %(major)d.%(minor)d-%(release)d.%(dist)s\n" % info)


def build():
    info = common.get_info()
    root = common.get_root()
    bin_packages = list()
    noarch_packages = list()
    for package in root.iter('package'):
        attr = package.attrib
        if attr["arch"] == "noarch":
            noarch_packages.append(package)
        else:
            bin_packages.append(package)
    architectures = [{"target": "x86_64-linux",
                      "bits": 64, "arch_t": ".x86_64"}]
    if info['dist'] != 'el8' and info['dist'] != 'el9':
        architectures.append(
            {"target": "i686-linux", "bits": 32, "arch_t": ".i686"})

    for arch in architectures:
        info['target'] = arch['target']
        info['bits'] = arch['bits']
        info['arch'] = "bin"
        info['arch_t'] = arch['arch_t']
        for package in bin_packages:
            if package.attrib['name'] != "MDSplus":
                info["packagename"] = "-%s" % package.attrib["name"]
            else:
                info["packagename"] = ""
            info['summary'] = package.attrib['summary']
            info['description'] = package.attrib['description']
            out, specfilename = tempfile.mkstemp()
            common.writeb(out, rpmspec % info)
            for require in package.iter("requires"):
                doRequire(info, out, root, require)
            common.writeb(out, pckspec % info)
            for inc in package.iter('include'):
                for inctype in inc.attrib:
                    include = fixFilename(info, inc.attrib[inctype])
                    if inctype == "dironly":
                        common.writeb(out, "%%dir %s\n" % include)
                    else:
                        common.writeb(out, "%s\n" % include)
            for exc in package.iter('exclude'):
                for exctype in exc.attrib:
                    exclude = fixFilename(info, exc.attrib[exctype])
                    common.writeb(out, "%%exclude %s\n" % exclude)
            if package.find("exclude_staticlibs") is not None:
                common.writeb(out, "%%exclude /usr/local/mdsplus/lib%(bits)d/*.a\n" % info)
            if package.find("include_staticlibs") is not None:
                common.writeb(out, "/usr/local/mdsplus/lib%(bits)d/*.a\n" % info)
            for phase in ("pre", "post", "preun", "postun"):
                script = package.find(phase)
                if script is not None:
                    common.writeb(out, "%%%s\n%s\n" % (phase, script.text))
            os.close(out)
            info['specfilename'] = specfilename
            print("Building rpm for mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)

            sys.stdout.flush()
            subprocess.Popen("/bin/cat %(specfilename)s" %
                             info, shell=True).wait()
            for dir in ('BUILD', 'RPMS', 'SOURCES', 'SPECS', 'SRPMS'):
                try:
                    os.mkdir(('/release/%(flavor)s/' % info) + dir)
                except OSError:
                    pass  # if exists
            p = subprocess.Popen("rpmbuild -bb --define '_topdir /release/%(flavor)s' --buildroot=%(buildroot)s --target=%(target)s %(specfilename)s 2>&1" %
                                 info, stdout=subprocess.PIPE, shell=True)
            message = p.stdout.read()
            status = p.wait()
            os.remove(specfilename)
            if status != 0:
                sys.stdout.write(common.s(message))
                sys.stdout.flush()
                raise Exception(
                    "Error building rpm for package mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)
            print("Done building rpm for mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)
            sys.stdout.flush()
    for package in noarch_packages:
        info['arch'] = "noarch"
        if package.attrib['name'] != "MDSplus":
            info["packagename"] = "-%s" % package.attrib["name"]
        else:
            info["packagename"] = ""
        info['summary'] = package.attrib['summary']
        info['description'] = package.attrib['description']
        out, specfilename = tempfile.mkstemp()
        common.writeb(out, rpmspec % info)
        for require in package.iter("requires"):
            doRequire(info, out, root, require)
        common.writeb(out, "Buildarch: noarch\n")
        common.writeb(out, pckspec % info)
        for inc in package.iter('include'):
            for inctype in inc.attrib:
                include = fixFilename(info, inc.attrib[inctype])
                if inctype == "dironly":
                    common.writeb(out, "%%dir %s\n" % include)
                else:
                    common.writeb(out, "%s\n" % include)
        for exc in package.iter('exclude'):
            for exctype in exc.attrib:
                exclude = fixFilename(info, exc.attrib[exctype])
                common.writeb(out, "%%exclude %s\n" % exclude)
        if package.find("exclude_staticlibs") is not None:
            common.writeb(out, "%%exclude /usr/local/mdsplus/lib??/*.a\n" % info)
        for s in ("pre", "post", "preun", "postun"):
            script = package.find(s)
            if script is not None:
                common.writeb(out, "%%%s\n%s\n" % (s, script.text))
        os.close(out)
        info['specfilename'] = specfilename
        print("Building rpm for mdsplus%(bname)s%(packagename)s.noarch" % info)
        sys.stdout.flush()
        subprocess.Popen("/bin/cat %(specfilename)s" % info, shell=True).wait()
        p = subprocess.Popen("rpmbuild -bb --define '_topdir /release/%(flavor)s' --buildroot=%(buildroot)s %(specfilename)s 2>&1" %
                             info, stdout=subprocess.PIPE, shell=True)
        message = p.stdout.read()
        status = p.wait()
        os.remove(specfilename)
        if status != 0:
            print(common.s(message))
            raise Exception(
                "Error building rpm for package mdsplus%(bname)s%(packagename)s.noarch" % info)
        print("Done building rpm for mdsplus%(bname)s%(packagename)s.noarch" % info)
        sys.stdout.flush()
    try:
        os.stat('/sign_keys/.gnupg')
        try:
            cmd = "/bin/sh -c 'rsync -a /sign_keys /tmp/; HOME=/tmp/sign_keys rpmsign --addsign /release/%(flavor)s/RPMS/*/*%(major)d.%(minor)d-%(release)d*.rpm'" % info
            try:
                if sys.version_info < (3,):
                    bout = sys.stdout
                else:
                    bout = sys.stdout.buffer
            except:
                child = pexpect.spawn(cmd, timeout=60)
            else:
                child = pexpect.spawn(cmd, timeout=60, logfile=bout)
            index = child.expect(["Enter pass phrase: ", pexpect.EOF])
            if index == 0:
                child.sendline("")
                child.expect(pexpect.EOF)
            child.close()
            if child.status != 0:
                sys.stdout.flush()
                raise Exception("Error signing rpms. status=%d" % child.status)
        except:
            print("Got exception in rpm signing:")
            traceback.print_exc()
    except:
        print("Sign keys unavailable. Not signing packages.")

if __name__ == "__main__":
    build()
