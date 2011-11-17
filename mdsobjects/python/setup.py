#!/usr/bin/env python


def getRelease():
    try:
      import os
      if 'MDSPLUS_PYTHON_VERSION' in os.environ:
	return os.environ['MDSPLUS_PYTHON_VERSION']
      for flavor in ['','-beta','-alpha']:
          f=os.popen("/bin/rpm -q mdsplus%s-python;echo $?" % (flavor,))
          l=f.readlines()
          f.close()
          if l[1]=='0\n':
              p=l[0].split('-')
              for i in range(len(p)):
                  if p[i]=='python':
                      if p[i-1] != 'mdsplus':
                          release=p[i-1]+'-'
                      else:
                          release=""
                      release=release+p[i+1]+'-'+p[i+2][0:-1]
                      return release
    except Exception,e:
        return '1.0'


from setuptools import setup, Extension, find_packages
version=getRelease()
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
