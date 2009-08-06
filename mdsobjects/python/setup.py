#!/usr/bin/env python

from setuptools import setup, Extension, find_packages
version='0.3'
setup(name='MDSplus',
      version=version,
      description='MDSplus Python Objects',
      long_description = """
      This module provides all of the functionality of MDSplus TDI natively in python.
      All of the MDSplus data types such as signal are represented as python classes.
      """,
      author='Tom Fredian,Josh Stillerman,Gabriele Manduchi',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
      download_url = 'http://www.mdsplus.org/binaries/python/',
      package_dir = {'MDSplus':'.','MDSplus._opcodes':'./_opcodes','MDSplus.tests':'./tests','MDSplus.doc':'./doc','MDSplus.widgets':'./widgets'},
      packages = ['MDSplus','MDSplus._opcodes','MDSplus.tests','MDSplus.widgets'],
      package_data = {'':['doc/*.*','widgets/*.glade']},
      include_package_data = True,
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
      test_suite='tests.test_all',
      zip_safe = False,
     )
