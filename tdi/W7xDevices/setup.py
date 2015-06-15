#!/usr/bin/env python

from setuptools import setup, Extension
setup(name='W7xDevices', version=0.1,
      description='W7-X device support',
      author='Brian Standley', author_email='brian@brianstandley.com',
      url='http://www.mdsplus.org/',
      package_dir = {'W7xDevices':'.'},
      packages = ['W7xDevices'],
      platforms = ('Any',),
      classifiers = ['Development Status :: 4 - Beta',
                     'Programming Language :: Python',
                     'Intended Audience :: Science/Research',
                     'Environment :: Console',
                     'Topic :: Scientific/Engineering'],
      keywords = ('physics','mdsplus'),
      zip_safe = False)
