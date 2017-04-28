from unittest import TestCase,TestSuite
from MDSplus import Tree,Float32,Float32Array,Int16Array,setenv
from threading import Lock

class segmentsTests(TestCase):
    _lock      = Lock()
    _instances = 0
    _tmpdir    = None

    @classmethod
    def setUpClass(cls):
        with cls._lock:
            if cls._instances == 0:
                import tempfile
                cls._tmpdir=tempfile.mkdtemp()
            setenv("seg_tree_path",cls._tmpdir)
            cls._instances+=1
    @classmethod
    def tearDownClass(cls):
        import gc,shutil
        gc.collect()
        with cls._lock:
            cls._instances -= 1
            if not cls._instances>0:
                shutil.rmtree(cls._tmpdir)

    def arrayDimensionOrder(self):
        from numpy import int16,zeros
        from random import randint
        ptree=Tree('seg_tree',-1,'NEW')
        ptree.addNode('IMM')
        ptree.write()
        ptree=Tree('seg_tree',-1)
        ptree.createPulse(1)
        ptree=Tree('seg_tree',1)
        node=ptree.getNode('IMM')
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
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['arrayDimensionOrder']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(segmentsTests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])
