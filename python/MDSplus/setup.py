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
import sys
import os
if 'pip-egg-info' in sys.argv:
    print("When using pip to install MDSplus use 'pip install -e <dir>'")
    sys.exit(1)

def parseTag(tag):
    release=tag.split('-')[0]
    if release=='alpha_release':
       name='mdsplus-alpha'
    elif release=='stable_release':
       name='mdsplus'
    else:
       name='-'.join(['mdsplus',release])
    return name


try:
    exec(open('_version.py').read())
    release=version
    pname=parseTag(release_tag)
except:
    release='1.0'
    pname='mdsplus'
    release_tag='Unknown'
pth_dir=os.path.abspath(__file__)
pth_dir=os.path.abspath(os.path.dirname(pth_dir)+'/..')
setupkw = {
      'name'         : 'MDSplus',
      'extra_path'    : {'mdsplus',pth_dir},
      'version'      : release,
      'description'  : 'MDSplus Python Objectsi - '+release_tag,
      'long_description': """
      This module provides all of the functionality of MDSplus TDI natively in python.
      All of the MDSplus data types such as signal are represented as python classes.
      """,
      'author'       : 'MDSplus Development Team',
      'author_email' : 'twf@www.mdsplus.org',
      'url'          : 'http://www.mdsplus.org/',
      'license'      : 'MIT',
      'classifiers'  : [
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      'keywords'     : ['physics','mdsplus',],
    }

def remove():
    import shutil
    "Remove installed MDSplus package"
    oldpath = list(sys.path)
    try:
        pypath = os.getenv("PYTHONPATH","")
        for p in pypath.split(";"):
            if len(p)>0 and p in sys.path:
                sys.path.remove(p)
        if "" in sys.path:
            sys.path.remove("")
        try: import MDSplus
        except:
            print("Error removing MDSplus: package not found")
            return False
        _f=MDSplus.__file__.split(os.sep)
        while len(_f) > 1 and _f[-1] != 'MDSplus':
            _f=_f[:-1]
        if len(_f) <= 1 and _f[-2].endswith('.egg'):
            _f=_f[:-1]
        if len(_f) == 0:
            print("Error removing MDSplus: invalid package path\n '%s'"%(MDSplus.__file__,))
            return False
        packagedir = os.sep.join(_f)
        sys.stdout.write("Removing '%s' ..."%packagedir)
        try:
            if os.path.islink(packagedir):
                os.remove(packagedir)
            else:
                shutil.rmtree(packagedir)
            _f[-1] = "mdsplus-*.egg-info"
            import glob
            egginfos = glob.glob(os.sep.join(_f))
            for egginfo in egginfos:
                if os.path.isdir(egginfo):
                    shutil.rmtree(egginfo)
        except Exception as exc:
            print(" error: %s" % (exc,))
            return False
        else:
            print(" ok")
            return True
    finally:
        sys.path = oldpath

if __name__=='__main__':
  try:
    print("use distutils.core")
    from distutils.core import setup
    from distutils.cmd import Command
    class TestCommand(Command):
        user_options = []
        def initialize_options(self):
            pass
        def finalize_options(self):
            pass
        def run(self):
            import sys, subprocess
            raise SystemExit(
                subprocess.call([sys.executable,
                                 '-m',
                                 'tests.__init__']))
    class RemoveCommand(Command):
        user_options = []
        def initialize_options(self):
            pass
        def finalize_options(self):
            pass
        def run(self):
            remove()
    setupkw['cmdclass'] = {'test': TestCommand,'remove': RemoveCommand}
  except:
    from setuptools import setup
    print("use setuptools")
    setupkw['include_package_data'] = True
    setupkw['test_suite'] = 'tests.test_all'
    setupkw['zip_safe']   = False
  setup(**setupkw)

