#!/usr/bin/env python
try:
    exec(open('_version.py').read())
    if branch == "stable":
        name="mdsplus_mitdevices"
    else:
        name="mdsplus_%s_mitdevices" % branch
except:
    import os
    if "BRANCH" in os.environ and os.environ["BRANCH"] != "stable":
        branch="_"+os.environ["BRANCH"]
    else:
        branch=""
    name="mdsplus%s_mitdevices" % branch
    version='0.3'

try:
    from setuptools import setup
except:
    from distutils.core import setup

setup(name=name,
      version=version,
      description='MIT Python Device support',
      long_description = """
      This module provides the MIT device support classes
      """,
      author='Tom Fredian,Josh Stillerman',
      author_email='twf@www.mdsplus.org',
      url='http://www.mdsplus.org/',
      package_dir = {'MitDevices':'.',},
      package_data = {'':['*.glade',]},
      include_package_data = True,
      packages = ['MitDevices',],
      platforms = ('Any',),
      classifiers = [
      'Programming Language :: Python',
      'Intended Audience :: Science/Research',
      'Environment :: Console',
      'Topic :: Scientific/Engineering',
      ],
      keywords = ('physics','mdsplus',),
      install_requires=["pexpect"],
      zip_safe = False,
     )
