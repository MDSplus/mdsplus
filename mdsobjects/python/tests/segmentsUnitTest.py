#!/usr/bin/env python
# -*- coding: utf-8 -*-

from unittest import TestCase,TestSuite

from tree import Tree
from mdsdata import *
from mdsscalar import *
from mdsarray import *

import numpy as np
import random
import os
import sys



import tempfile
_tmpdir=tempfile.mkdtemp()

def setUpModule():    
    pass

def tearDownModule():
    import shutil
    shutil.rmtree(_tmpdir)
    

class segmentsTests(TestCase):

    def setUp(self):
        os.environ["seg_tree_path"]=_tmpdir
        
    def tearDown(self):
        pass

    def arrayDimensionOrder(self):
        ptree=Tree('seg_tree',-1,'NEW')
        ptree.addNode('IMM')
        ptree.write()
        ptree=Tree('seg_tree',-1)
        ptree.createPulse(1)
        ptree=Tree('seg_tree',1)
        node=ptree.getNode('IMM')
        
        WIDTH = 640
        HEIGHT =480;
        currFrame=np.zeros(WIDTH*HEIGHT, dtype = np.int16);
        currTime=float(0);
        for i in range(0,WIDTH):
            for j in range(0,HEIGHT):
                currFrame[i*HEIGHT+j]=random.randint(0,255)
        currTime = float(0)
        startTime = Float32(currTime)
        endTime = Float32(currTime)
        dim = Float32Array(currTime)
        segment = Int16Array(currFrame)
        segment.resize([1,HEIGHT,WIDTH])
        shape = segment.getShape()
        node.makeSegment(startTime, endTime, dim, segment)
        retShape = node.getShape()
        
        self.assertEqual(shape[0],retShape[0])
        self.assertEqual(shape[1],retShape[1])
        self.assertEqual(shape[2],retShape[2])

    def runTest(self):
        self.arrayDimensionOrder()



def suite():
    tests = ['arrayDimensionOrder']
    return TestSuite(map(segmentsTests,tests))
