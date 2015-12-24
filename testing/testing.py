#!/usr/bin/env python2
# -*- coding: utf-8 -*-
from __future__ import print_function
import sys,os



class testing():    
    from unittest import TestCase,TestSuite,SkipTest
    test_format = os.getenv('TEST_FORMAT','tap')

    def check_module(self, module_name ):
        from modulefinder import ModuleFinder
        finder = self.ModuleFinder(debug=2)
        finder.run_script(module_name)
        for name, mod in finder.modules.items():
            try:                
                __import__(name, fromlist=mod.globalnames.keys(),level=1)
                print(".",end='')
            except ImportError, e:
                print("ERROR IMPORTING %s: " % name + "  --  "+e.message)
        
    def check_loadmethod(self, file_name, class_name, method_name ):
        import imp
        m = imp.load_source(class_name, file_name)
        getattr(m, method_name)()

    def check_loadlib(self, lib):
        import ctypes
        ctypes.CDLL(lib)
        
    def skip_test(self, module_name, message):
        if(self.test_format == 'tap'):
            print("ok 1 - " + module_name + " # SKIP " + message)
            print("1..1")
        sys.exit(77)
        
    def run(self, module):                
        import tap,unittest
        tr = tap.TAPTestRunner()
        tr.set_stream(1)        
        loader = unittest.TestLoader()
        tests = loader.loadTestsFromModule(module)
        tr.run(tests)
        
    def run_nose(self, module_name):
        import nose
        if self.test_format == 'tap':
            nose.run(argv=[ sys.argv[0], module_name,'--with-tap','--tap-stream'])
        elif self.test_format == 'xml':
            nose.run(argv=[sys.argv[0], module_name,'--with-xunit'])
        else:
            nose.run(argv=[sys.argv[0], module_name])
        


ts = testing()

def check_arch(file_name):
    if sys.platform.startswith('win'):
        lib='MdsShr.dll'
    elif sys.platform.startswith('darwin'):
        lib='libMdsShr.dylib'
    else:
        lib='libMdsShr.so'
    try:
        ts.check_loadlib(lib)
    except OSError:
        ts.skip_test(os.path.basename(file_name),
                     'Unable to load MDSplus core libs')

def run():
    import inspect
    frame = inspect.stack()[1]
    check_arch(frame[1])
    try:
        ts.run_nose(frame[1])
    except:
        try:
            module = inspect.getmodule(frame[0])
            ts.run(module)
        except:
            ts.skip_test(module,'Unable to run tests')


if __name__ == '__main__':
    import inspect
    sys.argv[0] = sys.argv[1]
    check_arch(sys.argv[1])
    try:
        ts.run_nose(sys.argv[1])
    except:
        ts.skip_test(module,'Unable to run tests')

