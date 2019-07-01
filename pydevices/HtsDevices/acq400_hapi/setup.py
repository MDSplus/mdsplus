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

name='acq400_hapi'
pname = name
setupkw = {
      'name'         : name,
      'version'      : '0.1',
      'description'  : "Python API for D-TACQ acq4xx devices",
      'long_description': """
      This module provides callable api for ACQ4XX devices that are reachable on the
      network.
      """,
      'author'       : 'Peter Milne',
      'author_email' : 'peter.milne@d-tacq.com',
      'url'          : 'http://www.d-tacq.com/',
      'package_dir'  : {pname:'.',
                     },
      'packages'     : [pname,
                  ],
      'classifiers'  : [
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      'keywords'     : ['data acquisition','hardware','digital to analog'],
#       install_requires=['numpy','ctypes'],
    }

try:
    from setuptools import setup
    setupkw['include_package_data'] = False
    setupkw['zip_safe']   = False
except:
    from distutils.core import setup

setup(**setupkw)
