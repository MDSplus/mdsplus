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

from unittest import TestCase, TestSuite
from MDSplus import Tree, Device, Data, setenv
from threading import Lock

class Tests(TestCase):
    _lock      = Lock()
    _instances = 0
    _tmpdir    = None

    @classmethod
    def setUpClass(cls):
        with cls._lock:
            if cls._instances == 0:
                import tempfile
                cls._tmpdir=tempfile.mkdtemp()
            setenv("devtree_path",cls._tmpdir)
            cls._instances+=1
    @classmethod
    def tearDownClass(cls):
        import gc,shutil
        gc.collect()
        with cls._lock:
            cls._instances -= 1
            if not cls._instances>0:
                shutil.rmtree(cls._tmpdir)

    def DevicesTests(self,devices):
        with Tree('devtree',-1,'new') as t:
            for name in sorted(devices.__dict__.keys()):
                cls = devices.__dict__[name]
                if isinstance(cls, type) and issubclass(cls, Device):
                    pathparts = name.split('_')
                    node = t.top
                    if len(name)>9 and len(pathparts)>1:
                        name = '_'.join(pathparts[1:])
                        try:
                            node = node.getNode(pathparts[0]+'_')
                        except:
                            node = node.addNode(pathparts[0]+'_','STRUCTURE')
                    cls.Add(node,name)
                    #t.write()

    def MitDevices(self):
        import MitDevices
        self.DevicesTests(MitDevices)
        names = [s for s in (str(s).strip() for s in Data.execute('MitDevices()')[::2]) if not s in MitDevices.__dict__];names.sort()
        _names = ['CHS_A14', 'DC1394', 'DC1394A', 'DIO2', 'DT196AO', 'DT200', 'DT_ACQ16', 'INCAA_TR10', 'JRG_ADC32A', 'JRG_TR1612', 'L6810', 'MATROX']
        passed = []
        with Tree('devtree',-1,'new') as t:
            for name in names:
               try:
                t.addDevice(name,name)
                passed.append(name)
               except: pass
        self.assertEqual(passed,_names)

    def RfxDevices(self):
        self.DevicesTests(__import__('RfxDevices'))
    def W7xDevices(self):
        self.DevicesTests(__import__('W7xDevices'))

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['MitDevices','RfxDevices','W7xDevices']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(Tests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner(verbosity=2).run(suite())

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
