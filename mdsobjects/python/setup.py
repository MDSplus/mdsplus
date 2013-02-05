#!/usr/bin/env python
import sys

def getRelease():
    try:
      import os
      name='MDSplus'
      release=None
      while (len(sys.argv)) > 2:
          if 'name=' in sys.argv[-1]:
              name=sys.argv[-1].split('=')[1]
              sys.argv=sys.argv[0:-1]
          elif 'version=' in sys.argv[-1]:
              release=sys.argv[2].split('=')[1]
              sys.argv=sys.argv[0:-1]
          else:
              raise Exception('Unknown option: '+sys.argv[-1])
      if release is not None:
          return (release,name)
      try:
        from mdsplus_version import mdsplus_version
        return (mdsplus_version,'MDSplus')
      except:
        pass
      if 'MDSPLUS_PYTHON_VERSION' in os.environ:
	return (os.environ['MDSPLUS_PYTHON_VERSION'],'MDSplus')
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
                      return (release,'MDSplus')
    except Exception,e:
        print e
    return ('1.0','MDSplus')


from setuptools import setup, Extension, find_packages
version,name=getRelease()
print version,name
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
                     name+'._opcodes':'./_opcodes',
                     name+'.tests':'./tests',
                     name+'.widgets':'./widgets',
                     name+'.wsgi':'./wsgi'},
      packages = [name,
                  name+'._opcodes',
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
