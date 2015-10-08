#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest
import tap;
import os

#import pyex1

#tr = tap.TAPTestRunner()
#s = unittest.defaultTestLoader.loadTestsFromTestCase(pyex1.TestStringMethods)
#tr.set_stream(1)
#tr.run(s);


pt = os.path.dirname(os.path.curdir)
loader = unittest.TestLoader();
tests = loader.discover(pt,"*.py")

tr = tap.TAPTestRunner()
tr.set_stream(1)
tr.run(tests)
