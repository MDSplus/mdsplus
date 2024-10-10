#!/usr/bin/env python
#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import sys
import os

import setuptools
from setuptools import setup


def setupkw():
    from runpy import run_path
    try:
        mod_dir = os.path.dirname(os.path.abspath(__file__))
        ver_data = run_path(os.path.join(mod_dir, "_version.py"))
        version = ver_data["version"]
        release = "%d.%d.%d" % version
        release_tag = ver_data["release_tag"]

    except Exception:
        release = "0.0.0"
        release_tag = "Unknown"
    pth_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    return dict(
        name="MDSplus",
        extra_path=("mdsplus", pth_dir),
        version=release,
        description="MDSplus Python Objects - " + release_tag,
        long_description=(
            "This module provides all of the functionality of MDSplus TDI natively in python.\n"
            "All of the MDSplus data types such as signal are represented as python classes.\n"
        ),
        author="MDSplus Development Team",
        author_email="mdsplusadmin@psfc.mit.edu",
        url="http://www.mdsplus.org/",
        license="MIT",
        classifiers=[
            "Programming Language :: Python",
            "Intended Audience :: Science/Research",
            "Environment :: Console",
            "Topic :: Scientific/Engineering",
        ],
        keywords=[
            "physics",
            "mdsplus",
        ],
        packages = ['MDSplus'],
        package_dir = {'MDSplus':''},
    )


if setuptools.__version__ < "60.0.0":
    # assume that setuptools can't directly use pyproject.toml
    # for the [project] section, so need the old setup.py

    print(sys.argv)
    if "develop" in sys.argv:
        # old setuptools can't have both --editable and --user
        # but it defaults to user anyway!
        if "--user" in sys.argv:
            import site

            print("Removing --user")
            argv = sys.argv
            argv.remove("--user")
            argv.append("--install-dir")
            argv.append(site.getusersitepackages())
            sys.argv = argv

    setup(
        test_suite="tests.test_all",
        zip_safe=False,
        packages=["MDSplus",
                  "MDSplus.wsgi",
                  "MDSplus.widgets",
                  "MDSplus.tests"],
        package_dir={"MDSplus": ".",
                     "MDSplus.widgets":"widgets",
                     "MDSplus.wsgi":"wsgi",
                     "MDSplus.tests":"tests"},
        package_data={"MDSplus.wsgi":[
            'html/*',
            'conf/*',
            'js/*',
            '*.tbl']},
        **setupkw()
    )

else:
    # setuptools can use pyproject.toml
    setup()
