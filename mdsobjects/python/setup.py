#!/usr/bin/env python
import sys
import os

def getRelease():
    name='MDSplus'
    remove_args=list()
    release='1.0'

    for arg in sys.argv:
        if arg.startswith('name='):
            name=arg.split('=')[1]
            remove_args.append(arg)
        if arg.startswith('version='):
            release=arg.split('=')[1]
            remove_args.append(arg)
    for arg in remove_args:
        sys.argv.remove(arg)

    if 'MDSPLUS_PYTHON_VERSION' in os.environ:
        release=os.environ['MDSPLUS_PYTHON_VERSION']
    try:
        from mdsplus_version import mdsplus_version
        release=mdsplus_version
    except:
        pass
    return (release,name)


from setuptools import setup, Extension, find_packages
version,name=getRelease()
print (version,name)
setup(name=name,
      version=version,
      description='MDSplus Python Objects',
      long_description = """
      This module provides all of the functionality of MDSplus TDI natively in python.
      All of the MDSplus data types such as signal are represented as python classes.
      """,
      author='Tom Fredian,Josh Stillerman,Gabriele Manduchi',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
      download_url = 'http://www.mdsplus.org/mdsplus_download/python',
      package_dir = {name:'.',
                     name+'.tdibuiltins':'./tdibuiltins',
                     name+'.tests':'./tests',
                     name+'.widgets':'./widgets',
                     name+'.wsgi':'./wsgi'},
      packages = [name,
                  name+'.tdibuiltins',
                  name+'.tests',
                  name+'.widgets',
                  name+'.wsgi'],
      package_data = {'':['doc/*.*','widgets/*.glade','js/*.js','html/*.html']},
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
