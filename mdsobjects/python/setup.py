#!/usr/bin/env python
import sys
import os
try:
    from setuptools import setup
except:
    from distutils.core import setup

def getRelease():
    name='mdsplus'
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
    if '-' in release:
        parts=release.split('-')
        name=name+'_'+parts[0]
        release=parts[1]
    return (release,name.lower())


try:
    exec(open('_version.py').read())
    if branch == "stable":
        name="mdsplus"
    else:
        name="mdsplus_%s" % branch
except:
    version,name=getRelease()
    if "BRANCH" in os.environ and os.environ["BRANCH"] != "stable":
        branch=" (%s)" % os.environ["BRANCH"]
    else:
        branch=""
pname='MDSplus'
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
      package_dir = {pname:'.',
                     pname+'.tdibuiltins':'./tdibuiltins',
                     pname+'.tests':'./tests',
                     pname+'.widgets':'./widgets',
                     pname+'.wsgi':'./wsgi',
                     pname+'.mdsExceptions':'./mdsExceptions'},
      packages = [pname,
                  pname+'.tdibuiltins',
                  pname+'.tests',
                  pname+'.widgets',
                  pname+'.wsgi',
                  pname+'.mdsExceptions'],
      package_data = {'':['doc/*.*','widgets/*.glade','js/*.js','html/*.html','wsgi/*.tbl']},
      include_package_data = True,
      classifiers = [
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      keywords = ('physics','mdsplus',),
#       install_requires=['numpy','ctypes'],
      test_suite='tests.test_all',
      zip_safe = False,
    )
