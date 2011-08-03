#!/usr/bin/env python

from setuptools import setup, Extension
version='0.2'
setup(name='MitDevices',
      version=version,
      description='MIT Python Device support',
      long_description = """
      This module provides the MIT device support classes
      """,
      author='Tom Fredian,Josh Stillerman',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
#      download_url = 'http://www.mdsplus.org/binaries/python/',
      package_dir = {'MitDevices':'.',},
      package_data = {'':['*.glade',]},
      include_package_data = True,
      packages = ['MitDevices',],
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
     install_requires=["pexpect","PyXML"],
      zip_safe = False,
     )
