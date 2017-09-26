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
try:
    exec(open('_version.py').read())
    if branch == "stable":
        name="mdsplus_w7xdevices"
    else:
        name="mdsplus_%s_w7xdevices" % branch
except:
    import os
    if "BRANCH" in os.environ and os.environ["BRANCH"] != "stable":
        branch="_"+os.environ["BRANCH"]
    else:
        branch=""
    name="mdsplus%s_w7xdevices" % branch
    version='0.1'

try:
    from setuptools import setup
except:
    from distutils.core import setup

setup(name=name, version=version,
      description='W7-X device support',
      author='Brian Standley', author_email='brian@brianstandley.com',
      url='http://www.mdsplus.org/',
      package_dir = {'W7xDevices':'.'},
      packages = ['W7xDevices'],
      platforms = ('Any',),
      classifiers = [
                     'Programming Language :: Python',
                     'Intended Audience :: Science/Research',
                     'Environment :: Console',
                     'Topic :: Scientific/Engineering'],
      keywords = ('physics','mdsplus'),
      zip_safe = False)
