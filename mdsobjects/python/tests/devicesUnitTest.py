#!/usr/bin/python
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

from unittest import TestCase, TestSuite, TextTestRunner
from MDSplus import Tree, Device, tdi, setenv
from threading import Lock

class Tests(TestCase):
    _lock      = Lock()
    _instances = 0
    _tmpdir    = None
    maxDiff    = None
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
    @staticmethod
    def getNodeName(t, model):
        """produces a generic path fitting with the 12 char limit of node names"""
        pathparts = model.split('_')
        node = t.top
        if len(model)>9:
            if len(pathparts)>1:
                base,name = pathparts[0]+'_','_'.join(pathparts[1:])
            elif len(model)>12:
                base,name = model[:10]+'_','_'+model[10:]
            else:
                return node,model
            try:
                node = node.getNode(base)
            except:
                node = node.addNode(base,'STRUCTURE')
        else:
            name = model
        return node,name

    def DevicesTests(self,package):
        with Tree('devtree',-1,'new') as t:
            devices = __import__(package).__dict__
            for model in sorted(devices.keys()):
                cls = devices[model]
                if isinstance(cls, type) and issubclass(cls, Device):
                    node,name = self.getNodeName(t,model)
                    cls.Add(node,name)

    def XyzDevices(self,package,expected=None):
        if expected is None:
            expected = [s for s in (str(s).strip() for s,p in tdi('%s()'%package))]
        expected.sort()
        passed = []
        with Tree('devtree',-1,'new') as t:
            for model in expected:
                node,name = self.getNodeName(t,model)
                try:
                    node.addDevice(name,model)
                    passed.append(model)
                    if Device.debug: print('PASSED %s'%model)
                except:
                    if Device.debug: print('FAILED %s'%model)
        self.assertEqual(passed,expected)
        self.DevicesTests(package)

    def MitDevices(self):
        self.XyzDevices('MitDevices',['CHS_A14', 'DC1394', 'DC1394A', 'DIO2', 'DT196AO', 'DT200', 'DT_ACQ16', 'INCAA_TR10', 'JRG_ADC32A', 'JRG_TR1612', 'L6810', 'MATROX'])
    def RfxDevices(self):
        self.XyzDevices('RfxDevices')  # check them all
    def W7xDevices(self):
        self.XyzDevices('W7xDevices',[])  # no tdi devices to check

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['MitDevices','RfxDevices','W7xDevices']
    @classmethod
    def getTestCases(cls,tests=None):
        if tests is None: tests = cls.getTests()
        return map(cls,tests)

def suite(tests=None):
    return TestSuite(Tests.getTestCases(tests))

def run(tests=None):
    TextTestRunner(verbosity=2).run(suite(tests))

def objgraph():
    import objgraph,gc
    gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    gc.collect()
    objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

if __name__=='__main__':
    import sys
    if len(sys.argv)==2 and sys.argv[1]=='all':
        run()
    elif len(sys.argv)>1:
        run(sys.argv[1:])
    else: print('Available tests: %s'%(' '.join(Tests.getTests())))
