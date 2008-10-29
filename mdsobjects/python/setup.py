#!/usr/bin/env python

from setuptools import setup, Extension

setup(name='MDSobjects',
      version='0.1',
      description='MDSplus Python Objects',
      long_description = """
      This module provides all of the functionality of MDSplus TDI natively in python.
      All of the MDSplus data types such as signal are represented as python classes.
      """,
      author='Tom Fredian,Josh Stillerman,Gabriele Manduchi',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
      download_url = 'http://www.psfc.mit.edu/~twf/MDSobjects-1.1-py2.4.egg',
      package_dir = {'MDSobjects':'.','MDSobjects._opcodes':'./_opcodes'},
      packages = ['MDSobjects','MDSobjects._opcodes'],
#      package_data = {'MDSobjects':'*'},
      platforms = ('Any',),
      classifiers = [ 'Development Status :: 4 - Beta',
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      keywords = ('physics','mdsplus',),
#      install_requires=['numpy','ctypes'],
#      include_package_data = True,
      zip_safe = False,
     )
