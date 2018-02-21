#!/usr/bin/env python2
# -*- coding: utf-8 -*-
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
import sys,os
if "LD_PRELOAD" in os.environ:
    os.environ.pop("LD_PRELOAD")

MDSplus_path=os.path.dirname(os.path.abspath(__file__))
if sys.path[0] != MDSplus_path:
    sys.path.insert(0,MDSplus_path)


class testing(object):
    import re

    TEST_FORMAT  = 'TEST_FORMAT'
    TEST_TAPFILE = 'TEST_TAPFILE'
    TEST_XMLFILE = 'TEST_XMLFILE'

    # list of test formats form TEST_FORMAT env
    test_format  = re.findall(r"[\w']+", os.getenv(TEST_FORMAT,'log,tap'))

    tap_file = os.getenv(TEST_TAPFILE, os.path.splitext(os.path.basename(sys.argv[1]))[0]+'.tap')
    xml_file = os.getenv(TEST_XMLFILE, os.path.splitext(os.path.basename(sys.argv[1]))[0]+'.xml')

    def check_unittest_version(self, module_name ):
        if module_name.startswith('thread'):
            import unittest
            if '__version__' in unittest.__dict__:
                return float(unittest.__version__)>=2.7
        return True

    def check_module(self, module_name ):
        from modulefinder import ModuleFinder
        finder = ModuleFinder(debug=2)
        finder.run_script(module_name)
        for name, mod in finder.modules.items():
            try:
                __import__(name, fromlist=mod.globalnames.keys(),level=1)
                sys.stdout.write('.')
            except ImportError as e:
                print("ERROR IMPORTING %s: " % name + "  --  "+e.message)

    def check_loadmethod(self, file_name, class_name, method_name ):
        import imp
        m = imp.load_source(class_name, file_name)
        getattr(m, method_name)()

    def check_loadlib(self, lib):
        import ctypes
        ctypes.CDLL(lib)

    def skip_test(self, module_name, message):
        # TODO: fix this
        if 'tap' in self.test_format:
            f = open(self.tap_file,'w')
            f.write("ok 1 - " + module_name + " # SKIP " + message + "\n")
            f.write("1..1")
            f.close
        if 'log' in self.test_format:
            print(message)
            sys.exit(77)

    def run_tap(self, module):
        import tap,unittest
        tr = tap.TAPTestRunner()
        tr.set_stream(1)
        loader = unittest.TestLoader()
        tests = loader.loadTestsFromModule(module)
        tr.run(tests)

    def run_nose(self, module_name):
        import nose,shutil
        f = self.test_format
        nose_aux_args = ['-d','-s','-v']
        res = 0

        if 'tap' in f:
            try:
                from tap.plugins._nose import TAP #analysis:ignore
                nose.run(argv=[ sys.argv[1],'', '--with-tap'] + nose_aux_args)
            except:
                f.remove('tap')
        if 'xml' in f:
            try:
                from nose.plugins.xunit import Xunit #analysis:ignore
            except ImportError:
                f.remove('xml')
        if len(f) > 1:
            if 'log' in f and 'tap' in f and 'xml' in f:
                res = nose.run(argv=[ sys.argv[1], module_name,
                '--with-tap','--tap-combined','--tap-out=.'+os.path.basename(sys.argv[1]),
                '--tap-format="{method_name} {short_description}"',
                '--with-xunit','--xunit-file='+self.xml_file] + nose_aux_args)
                shutil.copyfile('.'+os.path.basename(sys.argv[1])+'/testresults.tap',self.tap_file)
                shutil.rmtree('.'+os.path.basename(sys.argv[1]))

            elif 'log' in f and 'tap' in f:
                res = nose.run(argv=[ sys.argv[1], module_name,
                '--with-tap','--tap-combined','--tap-out=.'+os.path.basename(sys.argv[1]),
                '--tap-format="{method_name} {short_description}"'] + nose_aux_args)
                shutil.copyfile('.'+os.path.basename(sys.argv[1])+'/testresults.tap',self.tap_file)
                shutil.rmtree('.'+os.path.basename(sys.argv[1]))

            elif 'log' in f and 'xml' in f:
                res = nose.run(argv=[ sys.argv[1], module_name,
                '--with-xunit','--xunit-file='+self.xml_file] + nose_aux_args)
        elif len(f) > 0:
            if 'tap' in f:
                res = nose.run(argv=[ sys.argv[1], module_name,
                '--with-tap','--tap-stream',
                '--tap-format="{method_name} {short_description}"'] + nose_aux_args)

            elif 'xml' in f:
                print("WARNING: xml output can not be streamed to stdout")
                res = nose.run(argv=[ sys.argv[1], module_name,
                '--with-xunit','--xunit-file='+self.xml_file] + nose_aux_args)
            else:
                res = nose.run(argv=[ sys.argv[1], module_name] + nose_aux_args)
        else:
            raise IndexError
        return res

ts = testing()
def check_arch(file_name):
    module_name = os.path.basename(file_name)
    if not ts.check_unittest_version(module_name):
        ts.skip_test(module_name,'Unfit unittest version < 2.7')
    try:
        from MDSplus import getenv
    except Exception as e:
        ts.skip_test(module_name,'Unable to import MDSplus: "%s"'%(e,))
    if module_name.startswith('dcl'):
      try:
        pylib = getenv('PyLib')
        print('PyLib="%s"'%pylib)
        if not pylib:
            ts.skip_test(module_name,'Invalid/unset PyLib env.')
        ts.check_loadlib(pylib)
      except OSError:
        ts.skip_test(module_name,'Unable to load python lib "%s"'%(pylib,))

if __name__ == '__main__':
    if '--skip' in sys.argv:
        ts.skip_test(sys.argv[1],'Skipped tests')
    sys.argv[0] = sys.argv[1]
    check_arch(sys.argv[1])
    try:
        res = ts.run_nose(sys.argv[1])
        sys.exit( not res )
    except SystemExit:
        raise
    except:
        import traceback
        traceback.print_exc()
    ts.skip_test(sys.argv[1],"unrecoverable error from nose "+str(sys.exc_info()[0]))

