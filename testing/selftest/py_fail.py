#!/usr/bin/env python
# -*- coding: utf-8 -*-

import unittest

class TestFail(unittest.TestCase):

  def test_fail(self):
      s = 'expect to fail'
      self.assertEqual(s.split(), ['expect', 'to', 'succeed'])

if __name__ == '__main__':
    unittest.main()



