from unittest import TestCase,TestSuite

from MDSplus import Tree,Float32,Float32Array,Int16Array,setenv


class segmentsTests(TestCase):

    def setUp(self):
        from tempfile import mkdtemp
        self.tmpdir=mkdtemp()
        setenv("seg_tree_path",self.tmpdir)

    def tearDown(self):
        import shutil, gc
        del(self.ptree)
        gc.collect()
        shutil.rmtree(self.tmpdir)

    def arrayDimensionOrder(self):
        from numpy import int16,zeros
        from random import randint
        self.ptree=Tree('seg_tree',-1,'NEW')
        self.ptree.addNode('IMM')
        self.ptree.write()
        self.ptree=Tree('seg_tree',-1)
        self.ptree.createPulse(1)
        self.ptree=Tree('seg_tree',1)
        node=self.ptree.getNode('IMM')
        WIDTH = 640
        HEIGHT= 480;
        currFrame=zeros(WIDTH*HEIGHT, dtype = int16);
        currTime=float(0);
        for i in range(0,WIDTH):
            for j in range(0,HEIGHT):
                currFrame[i*HEIGHT+j]=randint(0,255)
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

if __name__=='__main__':
    from unittest import TextTestRunner
    TextTestRunner().run(suite())
