#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function
import sys


class testing():
    import sys
    from modulefinder import ModuleFinder    
        
    def check_module(self, module_name ):
        finder = self.ModuleFinder(debug=2)
        finder.run_script(module_name)
        for name, mod in finder.modules.items():
            try:                
                __import__(name, fromlist=mod.globalnames.keys(),level=1)
                if(name == "numpy"):
                    print("+",end='')
                else:
                    print(".",end='')
            except ImportError, e:
                print("ERROR IMPORTING %s: " % name + "  --  "+e.message)
        print()
        
    def print_module_report(self, module_name ):
        finder = self.ModuleFinder()
        finder.run_script(module_name)        
        finder.report()

    def load_method(self, file_name, method_name ):
        import imp
        m = imp.load_source("dataTests", file_name)        
        r = getattr(m, method_name)()

    def check_arch(self, lib):
        import ctypes,sys,os
        from ctypes.util import find_library        
        try:
            ctypes.CDLL(lib)
            print("found: "+lib)
            sys.exit(0)
        except OSError:
            sys.exit(1)
        
if __name__ == '__main__':
    t = testing()    
    t.check_arch(sys.argv[1])


