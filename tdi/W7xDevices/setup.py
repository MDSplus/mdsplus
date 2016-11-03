#!/usr/bin/env python

try:
    exec(open('_version.py').read())
    if branch == "stable":
        name="mdsplus_w7xdevices"
    else:
        name="mdsplus_%s_w7xdevices" % branch
except:
    import os
    if "BRANCH" in os.environ and os.environ["BRANCH"] != "stable":
        branch="_"+os.environ["BRANCH"]
    else:
        branch=""
    name="mdsplus%s_w7xdevices" % branch
    version='0.1'

try:
    from setuptools import setup
except:
    from distutils.core import setup

setup(name=name, version=version,
      description='W7-X device support',
      author='Brian Standley', author_email='brian@brianstandley.com',
      url='http://www.mdsplus.org/',
      package_dir = {'W7xDevices':'.'},
      packages = ['W7xDevices'],
      platforms = ('Any',),
      classifiers = [
                     'Programming Language :: Python',
                     'Intended Audience :: Science/Research',
                     'Environment :: Console',
                     'Topic :: Scientific/Engineering'],
      keywords = ('physics','mdsplus'),
      zip_safe = False)
