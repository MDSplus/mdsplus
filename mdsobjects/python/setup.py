#!/usr/bin/env python
import sys
import os

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


branch = None
try:
    exec(open('_version.py').read())
except:
    pass
if branch is None:
    version,name=getRelease()
elif branch == "stable":
    name="mdsplus"
else:
    name="mdsplus_%s" % branch

pname='MDSplus'


setupkw = {
      'name'         : name,
      'version'      : version,
      'description'  : 'MDSplus Python Objects',
      'long_description': """
      This module provides all of the functionality of MDSplus TDI natively in python.
      All of the MDSplus data types such as signal are represented as python classes.
      """,
      'author'       : 'Tom Fredian,Josh Stillerman,Gabriele Manduchi',
      'author_email' : 'twf@www.mdsplus.org',
      'url'          : 'http://www.mdsplus.org/',
      'download_url' : 'http://www.mdsplus.org/mdsplus_download/python',
      'package_dir'  : {pname:'.',
                     pname+'.tests':'./tests',
                     pname+'.widgets':'./widgets',
                     pname+'.wsgi':'./wsgi',
                     pname+'.mdsExceptions':'./mdsExceptions'},
      'packages'     : [pname,
                  pname+'.tests',
                  pname+'.widgets',
                  pname+'.wsgi',
                  pname+'.mdsExceptions'],
      'package_data' : {'':['doc/*.*','widgets/*.glade','js/*.js','html/*.html','wsgi/*.tbl']},
      'classifiers'  : [
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      'keywords'     : ('physics','mdsplus',),
#       install_requires=['numpy','ctypes'],
    }

try:
    from setuptools import setup
    setupkw['include_package_data'] = True
    setupkw['test_suite'] = 'tests.test_all'
    setupkw['zip_safe']   = False
except:
    from distutils.core import setup
    from distutils.cmd import Command
    class TestCommand(Command):
        user_options = []

        def initialize_options(self):
            pass

        def finalize_options(self):
            pass

        def run(self):
            import sys, subprocess

            raise SystemExit(
                subprocess.call([sys.executable,
                                 '-m',
                                 'tests.__init__']))
    setupkw['cmdclass'] = {'test': TestCommand}
setup(**setupkw)
