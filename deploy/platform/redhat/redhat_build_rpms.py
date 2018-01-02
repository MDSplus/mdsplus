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
import subprocess,os,sys,pexpect,xml.etree.ElementTree as ET,fnmatch,tempfile

srcdir=os.path.realpath(os.path.dirname(os.path.realpath(__file__))+'/../../..')

def externalPackage(info, root, package):
    ans = None
    for extpackages in root.getiterator('external_packages'):
        platform=extpackages.attrib['platform']
        if platform == info['platform']:
            pkg = extpackages.find(package)
            if pkg is not None:
                if 'package' in pkg.attrib:
                    ans = pkg.attrib['package']
                else:
                    ans = package
    return ans

def fixFilename(info,filename):
    """For binary packages we need to refer to /libnn, /binnn and /uidnn where nn is 32 or 64. For
    noarch packages we need to use /lib??, /bin?? to get the no arch dependent scripts and files"""
    try:
        if info['arch']=='noarch':
            filename=filename.replace("/lib/","/lib??/")
            filename=filename.replace("/bin/","/bin??/")
            if filename[len(filename)-4:] in ('/lib','/bin'):
                filename=filename+"??"
        else:
            filename=filename.replace("/lib/","/lib%(bits)d/")
            filename=filename.replace("/bin/","/bin%(bits)d/")
            filename=filename.replace("/uid/","/uid%(bits)d/")
            if filename[len(filename)-4:] in ('/lib','/bin','/uid'):
                filename=filename+"%(bits)d"
        ans=filename % info
    except Exception,e:
        raise Exception("Error fixing filename %s: %s" % (filename,e))
    return ans

def doRequire(info, out, root, require):
    if 'external' in require.attrib:
        pkg=externalPackage(info,root,require.attrib['package'])
        if pkg is not None:
            os.write(out,"Requires: %s\n" % pkg)
    else:
        info['reqpkg']=require.attrib['package']
        os.write(out,"Requires: mdsplus%(bname)s-%(reqpkg)s = %(major)d.%(minor)d-%(release)d.%(distname)s\n" % info)

def buildRpms():
    info=dict()
    info['branch']=os.environ['BRANCH']
    version=os.environ['RELEASE_VERSION'].split('.')
    info['major']=int(version[0])
    info['minor']=int(version[1])
    info['release']=int(version[2])
    info['distname']=os.environ['DISTNAME']
    info['buildroot']=os.environ['BUILDROOT']
    info['bname']=os.environ['BNAME']
    info['platform']=os.environ['PLATFORM']
    tree=ET.parse(srcdir+'/deploy/packaging/linux.xml')
    root=tree.getroot()
    rpmspec=root.find('rpm').find('spec_start').text
    s=rpmspec.split('\n')
    for idx in range(len(s)):
        if len(s[idx].strip()) == 0:
            s[idx]=""
    rpmspec='\n'.join(s)

    bin_packages=list()
    noarch_packages=list()
    for package in root.getiterator('package'):
        attr=package.attrib
        if attr["arch"]=="noarch":
            noarch_packages.append(package)
        else:
            bin_packages.append(package)

    for arch in ({"target":"x86_64-linux","bits":64,"arch_t":".x86_64"},{"target":"i686-linux","bits":32,"arch_t":".i686"}):
        info['target']=arch['target']
        info['bits']=arch['bits']
        info['arch']="bin"
        info['arch_t']=arch['arch_t']
        for package in bin_packages:
            if package.attrib['name'] != "MDSplus":
                info["packagename"]="-%s" % package.attrib["name"]
            else:
                info["packagename"]=""
            info['summary']=package.attrib['summary']
            info['description']=package.attrib['description']
            out,specfilename=tempfile.mkstemp()
	    print info,rpmspec
            os.write(out,rpmspec % info)
            for require in package.getiterator("requires"):
                doRequire(info,out,root,require)
            os.write(out,"""
%%description
%(description)s
%%clean
%%files
%%defattr(-,root,root)
""" % info)
            for inc in package.getiterator('include'):
                for inctype in inc.attrib:
                    include=fixFilename(info, inc.attrib[inctype])
                    if inctype == "dironly":
                        os.write(out,"%%dir %s\n" % include)
                    else:
                        os.write(out,"%s\n" % include)
            for exc in package.getiterator('exclude'):
                for exctype in exc.attrib:
                    exclude=fixFilename(info, exc.attrib[exctype])
                    os.write(out,"%%exclude %s\n" % exclude)
            if package.find("exclude_staticlibs") is not None:
                os.write(out,"%%exclude /usr/local/mdsplus/lib%(bits)d/*.a\n" % info)
            if package.find("include_staticlibs") is not None:
                os.write(out,"/usr/local/mdsplus/lib%(bits)d/*.a\n" % info)
            for s in (("preinst","pre"),("postinst","post"),("prerm","preun"),("postrm","postun")):
                script=package.find(s[0])
                if script is not None and ("type" not in script.attrib or script.attrib["type"]=="rpm"):
                    os.write(out,"%%%s\n%s\n" % (s[1],script.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
            triggerin=package.find("triggerin")
            if triggerin is not None:
                os.write(out,"%%triggerin -- %s\n%s\n" % (triggerin.attrib['trigger'],
                                                           triggerin.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
            os.close(out)
            info['specfilename']=specfilename
            print("Building rpm for mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)

            sys.stdout.flush()
            subprocess.Popen("/bin/cat %(specfilename)s" % info,shell=True).wait();
            for s in ('BUILD','RPMS','SOURCES','SPECS','SRPMS'):
                try:
                    os.mkdir('/release/%(branch)s/'%info +s)
                except OSError: pass  # if exists
            p = subprocess.Popen("rpmbuild -bb --define '_topdir /release/%(branch)s' --buildroot=%(buildroot)s --target=%(target)s %(specfilename)s 2>&1" % info,stdout=subprocess.PIPE,shell=True)
            messages=p.stdout.readlines()
            status=p.wait()
            os.remove(specfilename)
            if status != 0:
                print(''.join(messages))
                sys.stdout.flush()
                raise Exception("Error building rpm for package mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)
            print("Done building rpm for mdsplus%(bname)s%(packagename)s%(arch_t)s" % info)
            sys.stdout.flush()
    for package in noarch_packages:
        info['arch']="noarch"
        if package.attrib['name'] != "MDSplus":
            info["packagename"]="-%s" % package.attrib["name"]
        else:
            info["packagename"]=""
        info['summary']=package.attrib['summary']
        info['description']=package.attrib['description']
        out,specfilename=tempfile.mkstemp()
        os.write(out,rpmspec % info)
        for require in package.getiterator("requires"):
            doRequire(info, out, root, require)
        os.write(out,"""
Buildarch: noarch
%%description
%(description)s
%%clean
%%files
%%defattr(-,root,root)
""" % info)
        for inc in package.getiterator('include'):
            for inctype in inc.attrib:
                include=fixFilename(info, inc.attrib[inctype])
                if inctype == "dironly":
                    os.write(out,"%%dir %s\n" % include)
                else:
                    os.write(out,"%s\n" % include)
        for exc in package.getiterator('exclude'):
            for exctype in exc.attrib:
                exclude=fixFilename(info, exc.attrib[exctype])
                os.write(out,"%%exclude %s\n" % exclude)
        if package.find("exclude_staticlibs") is not None:
            os.write(out,"%%exclude /usr/local/mdsplus/lib??/*.a\n" % info)
        for s in (("preinst","pre"),("postinst","post"),("prerm","preun"),("postrm","postun")):
            script=package.find(s[0])
            if script is not None and ("type" not in script.attrib or script.attrib["type"]=="rpm"):
                os.write(out,"%%%s\n%s\n" % (s[1],script.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
        triggerin=package.find("triggerin")
        if triggerin is not None:
            os.write(out,"%%triggerin -- %s\n%s\n" % (triggerin.attrib['trigger'],
                                                       triggerin.text.replace("__INSTALL_PREFIX__","$RPM_INSTALL_PREFIX")))
        os.close(out)
        info['specfilename']=specfilename
        print("Building rpm for mdsplus%(bname)s%(packagename)s.noarch" % info)
        sys.stdout.flush()
        subprocess.Popen("/bin/cat %(specfilename)s" % info,shell=True).wait();
        p=subprocess.Popen("rpmbuild -bb --define '_topdir /release/%(branch)s' --buildroot=%(buildroot)s %(specfilename)s 2>&1" % info,stdout=subprocess.PIPE,shell=True)
        messages=p.stdout.readlines()
        status = p.wait()
        os.remove(specfilename)
        if status != 0:
            print(''.join(messages))
            raise Exception("Error building rpm for package mdsplus%(bname)s%(packagename)s.noarch" % info)
        print("Done building rpm for mdsplus%(bname)s%(packagename)s.noarch" % info)
        sys.stdout.flush()
    try:
        os.stat('/sign_keys/.gnupg')
        try:
            cmd="/bin/sh -c 'HOME=/sign_keys rpmsign --addsign /release/%(branch)s/RPMS/*/*%(major)d.%(minor)d-%(release)d*.rpm'" % info
            child = pexpect.spawn(cmd,timeout=60,logfile=sys.stdout)
            child.expect("Enter pass phrase: ")
            child.sendline("")
            child.expect(pexpect.EOF)
            child.close()
            if child.status != 0:
                sys.stdout.flush()
                raise Exception("Error signing rpms. status=%d" % child.status)
        except Exception,e:
            print("Got exception in rpm signing: %s" % str(e))
    except:
        print("Sign keys unavailable. Not signing packages.")

buildRpms()
