#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest, tap, os, sys;

#import pyex1

#tr = tap.TAPTestRunner()
#s = unittest.defaultTestLoader.loadTestsFromTestCase(pyex1.TestStringMethods)
#tr.set_stream(1)
#tr.run(s);


loader = unittest.TestLoader();

#pt = os.path.dirname(os.path.curdir)
#tests = loader.discover(pt,"*.py")

tr = tap.TAPTestRunner()
tr.set_stream(1)


tests = loader.discover(sys.argv[1],sys.argv[2])
tr.run(tests)
