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

from unittest import TestCase,TestSuite
import os,sys,time
from threading import RLock,Thread

from MDSplus import Connection
from MDSplus import getenv,Float32

class Tests(TestCase):
    debug = False
    inThread = False
    lock = RLock()
    instances = 0
    index = 0
    host = None
    svr = None
    log = None

    @classmethod
    def setUpClass(cls):
        cls.root = os.path.dirname(os.path.realpath(__file__))
        hosts = '%s/mdsip.hosts'%cls.root
        def setup_mdsip(server_env,port_env,default_port,fix0):
            host = getenv(server_env,'')
            if len(host)>0:
                return host,0
            port = int(getenv(port_env,default_port))
            if port==0:
                if fix0: port = default_port
                else: return None,0
            return 'localhost:%d'%(port,),port
	def start_mdsip(port,logname,env=None):
            if port>0:
                from subprocess import Popen,STDOUT
                log = open('%s.log'%logname,'w')
                try:
                    params = ['mdsip','-s','-p',str(port),'-h',hosts]
                    print(' '.join(params+['&>',logname]))
                    mdsip = Popen(params,env=env,stdout=log,stderr=STDOUT)
                except:
                    log.close()
                    raise
                return mdsip,log
            return None,None
        with cls.lock:
            if cls.instances==0:
                cls.host,port = setup_mdsip('MDSIP_SERVER', 'MDSIP_PORT',8900,True)
                cls.svr,cls.log = start_mdsip(port,'mdsip',env=None)
                print(cls.svr,cls.log)
                time.sleep(1)
            cls.instances += 1

    @classmethod
    def tearDownClass(cls):
        import gc
        gc.collect()
        with cls.lock:
            cls.instances -= 1
            if not cls.instances>0:
                cls.host = None
                if cls.svr and cls.svr.poll() is None:
                    cls.svr.terminate()
                    cls.svr.wait()
                    cls.svr = None
                if cls.log:
                    cls.log.close()
                    cls.log = None

    def ConnectionThreads(self):
        def requests(c,idx):
            args = [Float32(i/10+idx) for i in range(10)]
            for i in range(10):
                self.assertEqual(c.get("[$,$,$,$,$,$,$,$,$,$]",*args).tolist(),args)
        c = Connection(self.host)
        threads = [Thread(name="C%d"%i,target=requests,args=(c,i)) for i in range(10)]
        for thread in threads: thread.start()
        for thread in threads: thread.join()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['ConnectionThreads']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(Tests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner(verbosity=2).run(suite())

if __name__=='__main__':
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])
