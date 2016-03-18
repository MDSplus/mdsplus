#!/usr/bin/env python
import sys
import os
import subprocess


####### This is a temporary fix for pypi uploading. It will be replaced when version info
####### is provided by configure
script_status=1
for branch in ['alpha','stable']:
   cmd="%s/deploy/get_latest_release %s" % (os.environ['WORKSPACE'],branch)
   p=subprocess.Popen("%s/deploy/get_latest_release %s" % (os.environ['WORKSPACE'],branch),stdout=subprocess.PIPE,shell=True)
   release_tag=p.stdout.readlines()[0][0:-1]
   status = p.wait()
   if status != 0:
      sys.exit(status)
   branch=release_tag.split('_')[0]
   v=release_tag.split('_')[1].split('-')
   major=v[1]
   minor=v[2]
   release=v[3]
   info={}
   if branch == 'stable':
      info['name']='mdsplus'
      info['provides']='mdsplus'
      info['mdsExceptions1']=''
      info['mdsExceptions2']=''
      info['pypi_name']='MDSplus'
   else:
      info['name']='mdsplus-%s' % branch
      info['provides']='mdsplus_%s' % branch
      info['mdsExceptions1']=",'MDSplus.mdsExceptions':'./mdsExceptions'"
      info['mdsExceptions2']=",'MDSplus.mdsExceptions'"
      info['pypi_name']=info['name']
   info['version']='%s.%s.%s' % (major,minor,release)
   info['release_tag']=release_tag
   info['branch']=branch
   info['workspace']=os.environ['WORKSPACE']
   status = subprocess.Popen("""
set -e   
wget -O vcheck.dat "https://pypi.python.org/pypi?:action=files&name=%(pypi_name)s&version=%(version)s" >/dev/null 2>&1
   if ( ! grep %(name)s-%(version)s vcheck.dat > /dev/null )
then 
   rm -Rf %(workspace)s/%(branch)s
   git clone %(workspace)s -b %(release_tag)s %(branch)s
   status=$?
else
   status=100
fi
rm -f vcheck.dat
exit $status
   """ % info,shell=True).wait()
   if status == 100:
      print("Package: %(name)s-%(version)s has already been uploaded" % info)
      continue
   if status != 0:
      script_status=1
      continue
   f=open('%(branch)s/mdsobjects/python/setup.py' % info,'w')
   f.write("""

import sys
import os
from setuptools import setup, Extension, find_packages

setup(name='%(name)s',
      version='%(version)s',
      provides=['%(provides)s'],
      description='MDSplus Python Objects',
      long_description = \"\"\"
This module provides all of the functionality of MDSplus TDI natively in python.
All of the MDSplus data types such as signal are represented as python classes.
         \"\"\",
      author='Tom Fredian,Josh Stillerman,Gabriele Manduchi',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
      download_url = 'https://github.com/MDSplus/mdsplus/archive/%(release_tag)s.tar.gz',
      package_dir = {'MDSplus':'.',
                     'MDSplus.tdibuiltins':'./tdibuiltins',
                     'MDSplus.tests':'./tests',
                     'MDSplus.widgets':'./widgets',
                     'MDSplus.wsgi':'./wsgi'%(mdsExceptions1)s},
      packages = ['MDSplus',
                  'MDSplus.tdibuiltins',
                  'MDSplus.tests',
                  'MDSplus.widgets',
                  'MDSplus.wsgi'%(mdsExceptions2)s],
      package_data = {'':['doc/*.*','widgets/*.glade','js/*.js','html/*.html','wsgi/*.tbl']},
      include_package_data = True,
      platforms = ['Any',],
      classifiers = [ 
         'Programming Language :: Python',
         'Intended Audience :: Science/Research',
         'Environment :: Console',
         'Topic :: Scientific/Engineering',
      ],
      zip_safe = False,
      keywords = ['physics','mdsplus',],
      install_requires=['numpy'],
      test_suite='tests.test_all'
)
""" % info)
   f.close()
   status=subprocess.Popen("""
python setup.py sdist upload
   """,shell=True,cwd='%(workspace)s/%(branch)s/mdsobjects/python' % info).wait()
   if status != 0:
      script_status=1
exit(script_status)

