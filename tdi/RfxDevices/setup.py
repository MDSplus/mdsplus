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

#!/usr/bin/env python

try:
    exec(open('_version.py').read())
    if branch == "stable":
        name="mdsplus_rfxdevices"
    else:
        name="mdsplus_%s_rfxdevices" % branch
except:
    import os
    if "BRANCH" in os.environ and os.environ["BRANCH"] != "stable":
        branch="_"+os.environ["BRANCH"]
    else:
        branch=""
    name="mdsplus%s_rfxdevices" % branch
    version='0.2'

try:
    from setuptools import setup
except:
    from distutils.core import setup

setup(name=name,
      version=version,
      description='RFX Python Device support',
      long_description = """
      This module provides the RFX device support classes
      """,
      author='Gabriele Manduchi',
      author_email='gabriele.manduchi@igi.cnr.it',
      url='http://www.mdsplus.org/',
#      download_url = 'http://www.mdsplus.org/binaries/python/',
      package_dir = {'RfxDevices':'.',},
      packages = ['RfxDevices',],
#      package_data = {'MDSplus':'*'},
      platforms = ('Any',),
      classifiers = [ 'Development Status :: 4 - Beta',
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      keywords = ('physics','mdsplus',),
#       install_requires=['numpy','ctypes'],
#      include_package_data = True,
#      test_suite='tests.test_all',
      zip_safe = False,
     )
