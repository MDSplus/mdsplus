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
