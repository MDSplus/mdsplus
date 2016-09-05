#!/usr/bin/env python

from setuptools import setup
version='0.2'
setup(name='TutorialDevices',
      version=version,
      description='RFX Python Device support',
      long_description = """
      This module provides the RFX device support classes
      """,
      author='Gabriele Manduchi',
      author_email='gabriele.manduchi@igi.cnr.it',
      url='http://www.mdsplus.org/',
#      download_url = 'http://www.mdsplus.org/binaries/python/',
      package_dir = {'TutorialDevices':'.',},
      packages = ['TutorialDevices',],
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
