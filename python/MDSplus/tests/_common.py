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
from MDSplus import Tree, getenv, setenv, tcl, Connection
from MDSplus import checkStatus, TreeWRITEFIRST, TreeNOT_OPEN
import traceback
import threading
import re
import gc
import os
import sys
import time

from MDSplus.mdsExceptions import MDSplusException


iswin = sys.platform.startswith('win')
MDSIP_PROTOCOL = getenv('MDSIP_PROTOCOL','TCP')


class logger(object):
    """wrapper class to force flush on each write"""

    def __init__(self, filename):
        self.__f = open(filename, 'w+')

    def write(self, *a, **kv):
        self.__f.write(*a, **kv)
        self.__f.flush()

    def __enter__(self, *a, **kv): return self

    def __exit__(self, *a, **kv): return self.close()

    def __getattr__(self, name): return self.__f.__getattribute__(name)


class TestThread(threading.Thread):
    """ Run Tests in parralel and evaluate """

    @staticmethod
    def assertRun(timeout, *threads):
        for thread in threads:
            thread.start()
        last = time.time()
        for thread in threads:
            if timeout > 0:
                thread.join(timeout)
                now = time.time()
                timeout -= now - last
                last = now
            else:
                thread.join(0)
        for thread in threads:
            thread.check()

    def __init__(self, name, test, *args, **kwargs):
        super(TestThread, self).__init__(name=name)
        self.exc = threading.ThreadError(name + " still running")
        self.test = test
        self.args = args
        self.kwargs = kwargs

    def run(self):
        try:
            self.test(*self.args, **self.kwargs)
        except Exception as exc:
            if not hasattr(exc, "__traceback__"):
                _, _, exc.__traceback__ = sys.exc_info()
            self.exc = exc
        else:
            self.exc = None

    def check(self):
        if self.exc:
            traceback.print_tb(self.exc.__traceback__)
            raise self.exc


class Tests(TestCase):
    debug = False
    is_win = sys.platform.startswith('win')
    in_valgrind = 'VALGRIND' in os.environ
    inThread = False
    index = 0
    NOT_IN_THREADS = set()
    TESTS = set()

    @classmethod
    def getTests(cls):
        if cls.__module__.endswith("_test"):
            _, test = cls.__module__[:-5].split('_', 1)
            return [test]
        if not cls.inThread:
            return list(cls.TESTS)
        return list(cls.TESTS.difference(cls.NOT_IN_THREADS))

    @property
    def module(self):
        return self.__module__.split('.')[-1]

    def assertEqual(self, fst, snd, *a, **kv):
        if isinstance(fst, str):
            sys.stderr.write('%s\n' % str(fst))
        super(Tests, self).assertEqual(fst, snd, *a, **kv)

    def runTest(self):
        import traceback
        stdout, stderr = sys.stdout, sys.stderr
        try:
            with logger("%s-out.log" % self.module) as sys.stdout:
                sys.stderr = sys.stdout
                for test in self.getTests():
                    sys.stdout.write("### %s ###\n" % test)
                    sys.stdout.flush()
                    self.__getattribute__(test)()
        finally:
            sys.stdout, sys.stderr = stdout, stderr

    def _doTCLTest(self, expr, out=None, err=None, regex=False, verify=False, quiet=False, tree=None):
        def checkre(pattern, string):
            if pattern is None:
                self.assertEqual(string is None, True)
            else:
                self.assertEqual(string is None, False)
                self.assertEqual(re.match(pattern, str(string)) is None,
                                 False, '"%s"\nnot matched by\n"%s"' % (string, pattern))
        if not quiet:
            sys.stderr.write("TCL> %s\n" % (expr,))
        outo, erro = (tree.tcl if tree else tcl)(
            expr, True, True, True)
        if verify:
            ver, erro = erro.split('\n', 2)
            self.assertEqual(ver.endswith("%s" % expr), True)
            if len(erro) == 0:
                erro = None
        if not regex:
            self.assertEqual(outo, out)
            self.assertEqual(erro, err)
        else:
            checkre(out, outo)
            checkre(err, erro)

    def _doExceptionTest(self, expr, exc, tree=None):
        if Tests.debug:
            sys.stderr.write("TCL(%s) # expected exception: %s\n" %
                             (expr, exc.__name__))
        try:
            (tree.tcl if tree else tcl)(
                expr, True, True, True)
        except Exception as e:
            self.assertEqual(e.__class__, exc)
            return
        self.fail("TCL: '%s' should have signaled an exception" % expr)

    @classmethod
    def getTestCases(cls, tests=None):
        if tests is None:
            tests = cls.getTests()
        return map(cls, tests)

    @classmethod
    def getTestSuite(cls, tests=None):
        return TestSuite(cls.getTestCases(tests))

    @classmethod
    def runTests(cls, tests=None):
        TextTestRunner(verbosity=2).run(cls.getTestSuite(tests))

    @classmethod
    def runWithObjgraph(cls, tests=None):
        import objgraph
        gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
        cls.runTests(tests)
        gc.collect()
        objgraph.show_backrefs([a for a in gc.garbage if hasattr(
            a, '__del__')], filename='%s.png' % __file__[:-3])

    @classmethod
    def main(cls):
        cls.maxDiff = None  # show full diffs
        if cls.__module__ == '__main__':
            cls.__module__ = os.path.basename(sys.argv[0]).split('.', 1)[0]
            if len(sys.argv) == 2 and sys.argv[1] == 'all':
                cls.runTests()
            elif len(sys.argv) > 1:
                cls.runTests(sys.argv[1:])
            elif cls.__module__.endswith("_test"):
                cls.runTests()
            else:
                print('Available tests: %s' % (' '.join(cls.getTests())))
    envx = {}
    env = {}

    @classmethod
    def _setenv(cls, name, value):
        value = str(value)
        cls.env[name] = value
        cls.envx[name] = value
        setenv(name, value)


class MdsIp(object):
    root = os.path.dirname(os.path.realpath(__file__))

    @staticmethod
    def _setup_mdsip(server_env, port_env, default_port, fix0):
        host = getenv(server_env, '')
        if host:
            return host, 0
        port = int(getenv(port_env, default_port))
        if port == 0:
            if fix0:
                port = default_port
            else:
                return None, 0
        return 'localhost:%d' % (port,), port

    def _testDispatchCommand(self, mdsip, command, stdout=None, stderr=None):
        self._doTCLTest('dispatch/command/wait/server=%s %s' %
                        (mdsip, command))

    def _testDispatchCommandNoWait(self, mdsip, command, stdout=None, stderr=None):
        self._doTCLTest('dispatch/command/nowait/server=%s %s' %
                        (mdsip, command))

    def _checkIdle(self, server, **opt):
        show_server = "Checking server: %s\n[^,]+, [^,]+, logging (.|\n)*Inactive\n" % server
        self._doTCLTest('show server %s' %
                        server, out=show_server, regex=True, **opt)

    def _waitIdle(self, server, timeout):
        timeout = time.time()+timeout
        while 1:
            time.sleep(.3)
            try:
                self._checkIdle(server, quiet=True)
            except:
                if time.time() < timeout:
                    continue
                raise
            else:
                break

    def _wait(self, svr, to):
        if to <= 0:
            return svr.poll()
        if sys.version_info < (3, 3):
            rtn = svr.poll()
            for _ in range(int(10*to)):
                if rtn is not None:
                    break
                time.sleep(.1)
                rtn = svr.poll()
            return rtn
        try:
            svr.wait(to)
        except:
            return None
        else:
            return svr.poll()

    def _stop_mdsip(self, *procs_in):
        # filter unused mdsip
        procs = [(svr, server) for svr, server in procs_in if server]
        for svr, server in procs:
            if not svr:  # close trees on externals
                try:
                    self._doTCLTest(
                        'dispatch/command/wait/server=%s close/all' % server)
                except MDSplusException:
                    pass
        # filter external mdsip
        procs = [(svr, server) for svr, server in procs if svr]
        # filter terminated
        procs = [(svr, server) for svr, server in procs if svr.poll() is None]
        if len(procs) == 0:
            return
        """
        # stop server
        for svr, server in procs:
            try:
                self._doTCLTest('stop server %s' % server)
            except MDSplusException:
                pass
        t = time.time()+6
        procs = [(svr, server)
                 for svr, server in procs if self._wait(svr, t-time.time()) is None]
        if len(procs) == 0:
            return
        """
        # terminate server
        for svr, server in procs:
            sys.stderr.write("sending SIGTERM to %s" % server)
            svr.terminate()
        t = time.time()+3
        procs = [(svr, server)
                 for svr, server in procs if self._wait(svr, t-time.time()) is None]
        if len(procs) == 0:
            return
        # kill server
        for svr, server in procs:
            sys.stderr.write("sending SIGKILL to %s" % server)
            svr.kill()
        t = time.time()+3
        procs = [server for svr, server in procs if self._wait(
            svr, t-time.time()) is None]
        if len(procs) == 0:
            return
        raise Exception("FAILED cleaning up mdsips: %s" % (", ".join(procs),))

    def _start_mdsip(self, server, port, logname, protocol=MDSIP_PROTOCOL):
        if port > 0:
            from subprocess import Popen, STDOUT
            logfile = '%s-%s%d.log' % (self.module, logname, self.index)
            log = open(logfile, 'w') if iswin else None
            try:
                hosts = '%s/mdsip.hosts' % self.root
                params = ['mdsip', '-s', '-p',
                          str(port), '-P', protocol, '-h', hosts]

                print(' '.join(params + ['&>', logfile]))
                if not log:
                    params.extend(['2>&1', '|', 'tee', logfile])
                mdsip = Popen(params)
            except:
                if log:
                    log.close()
                raise
            try:
                self._waitIdle(server, 10)  # allow mdsip to launch
            except Exception:
                mdsip.kill()
                raise
        else:
            mdsip, log = None, None
        if server:
            c = Connection(server)
            for envpair in self.envx.items():
                checkStatus(c.get('setenv($//"="//$)', *envpair))
            c.get('tcl($)', 'set verify')
        return mdsip, log


class TreeTests(Tests):
    lock = threading.RLock()
    shotinc = 1
    instances = 0
    trees = []
    tree = None
    tmpdir = None

    @property
    def shot(self):
        return self.index * self.__class__.shotinc + 1

    @classmethod
    def setUpClass(cls):
        with cls.lock:
            if cls.instances == 0:
                gc.collect()
                from tempfile import mkdtemp
                if getenv("TEST_DISTRIBUTED_TREES") is not None:
                    treepath = "thread://tree::%s"
                else:
                    treepath = "%s"
                cls.tmpdir = mkdtemp()
                cls.root = os.path.dirname(os.path.realpath(__file__))
                cls.topsrc = os.path.realpath(
                    cls.root+"%s..%s..%s.." % tuple([os.sep]*3))
                cls.env = dict((k, str(v)) for k, v in os.environ.items())
                cls.envx = {}
                cls._setenv('PyLib', getenv('PyLib'))
                cls._setenv("MDS_PYDEVICE_PATH",
                            '%s/pydevices;%s/devices' % (cls.topsrc, cls.root))
                trees = cls.trees if cls.tree is None else set(
                    cls.trees).union([cls.tree])
                for treename in trees:
                    cls._setenv("%s_path" % treename, treepath % cls.tmpdir)
                if getenv("testing_path") is None:
                    cls._setenv("testing_path", "%s/trees" % cls.root)
            cls.instances += 1

    @classmethod
    def tearDownClass(cls):
        import shutil
        gc.collect()
        with cls.lock:
            cls.instances -= 1
            if not cls.instances > 0:
                shutil.rmtree(cls.tmpdir)

    def tearDown(self):
        gc.collect()
        if not self.is_win or self.inThread:
            return

        def is_tree(o):
            try:
                return isinstance(o, Tree)
            except Exception as e:
                print(e)
                return False
        trees = [o for o in gc.get_objects() if is_tree(o)]
        for t in trees:
            try:
                t.close()
            except TreeWRITEFIRST:
                t.quit()
            except TreeNOT_OPEN:
                pass
            except Exception:
                traceback.print_exc()
