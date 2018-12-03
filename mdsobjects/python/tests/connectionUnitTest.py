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

from threading import Thread
from MDSplus import Connection,GetMany,Float32,Range,setenv,Tree,TreeNNF,TreeNodeArray
import time

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.TreeTests,_UnitTest.MdsIp):
    index = 0
    tree = "pytree"
    def thick(self):
        def testnci(thick,local,con,nci):
            l = local.S.__getattribute__(nci)
            t = thick.S.__getattribute__(nci)
            if nci.endswith("_nids"):
                l,t = str(l),str(t)
                try:   c = str(con.get("getnci(getnci(S,$),'nid_number')",nci))
                except TreeNNF: c = '[]'
            else:
                c = con.get("getnci(S,$)",nci)
            try:
                self.assertEqual(t,c)
                self.assertEqual(t,l)
            except:
                print(nci,t,l,c)
                raise
        server,server_port  = self._setup_mdsip('ACTION_SERVER', 'ACTION_PORT',7100+self.index,True)
        svr = svr_log = None
        try:
            svr,svr_log = self._start_mdsip(server ,server_port ,'connectionTCP')
            try:
                con = Connection(server)
                with Tree(self.tree,-1,"new") as local:
                    s=local.addNode("S","SIGNAL")
                    s.addNode("T","TEXT").addTag("tagT")
                    s.addTag("tagS")
                    s.record = Float32([0,1,2,3])
                    local.write()
                local.normal()
                setenv("pytree_path","%s::"%server)
                thick = Tree(self.tree,-1)
                con.get("treeopen($,$)",self.tree,-1)
                # self.assertEqual(local.getFileName(),thick.getFileName());# remote Segmentation fault
                self.assertEqual(local.getFileName(),con.get("treefilename($,-1)",self.tree))
                self.assertEqual(str(thick.S.record.evaluate()), "[0.,1.,2.,3.]")
                self.assertEqual(str(local.S.record.evaluate()), "[0.,1.,2.,3.]")
                self.assertEqual(thick.findTags("*")[0],local.findTags("*")[0]) # thick client only finds first tag
                for nci in ('depth','usage_str','dtype','length','rlength','fullpath','minpath','member_nids','children_nids','rfa'):
                    testnci(thick,local,con,nci)
            finally:
                if svr and svr.poll() is None:
                    svr.terminate()
                    svr.wait()
        finally:
            if svr_log: svr_log.close()

    def threadsTcp(self):
        server,server_port  = self._setup_mdsip('ACTION_SERVER', 'ACTION_PORT',7100+self.index,True)
        svr = svr_log = None
        try:
            svr,svr_log = self._start_mdsip(server ,server_port ,'connectionTCP')
            try:
                if svr is not None: time.sleep(1)
                def requests(c,idx):
                    args = [Float32(i/10+idx) for i in range(10)]
                    for i in range(10):
                        self.assertEqual(c.get("[$,$,$,$,$,$,$,$,$,$]",*args).tolist(),args)
                c = Connection(server)
                """ mdsconnect """
                self.assertEqual(c.get('_a=1').tolist(),1)
                self.assertEqual(c.get('_a').tolist(),1)
                self.assertEqual(c.getObject('1:3:1').__class__,Range)
                g = GetMany(c);
                g.append('a','1')
                g.append('b','$',2)
                g.append('c','$+$',1,2)
                g.execute()
                self.assertEqual(g.get('a'),1)
                self.assertEqual(g.get('b'),2)
                self.assertEqual(g.get('c'),3)
                threads = [Thread(name="C%d"%i,target=requests,args=(c,i)) for i in range(10)]
                for thread in threads: thread.start()
                for thread in threads: thread.join()
            finally:
                if svr and svr.poll() is None:
                    svr.terminate()
                    svr.wait()
        finally:
            if svr_log: svr_log.close()

    def threadsLocal(self):
        c=Connection('local://gub')
        class ConnectionThread(Thread):
            def run(self):
                for i in range(1000):
                    self.test.assertEqual(int(c.get('%d' % i)),i)
        t1=ConnectionThread()
        t1.test=self
        t2=ConnectionThread()
        t2.test=self
        t1.start()
        t2.start()
        t1.join()
        t2.join()

    @staticmethod
    def getTests():
        return ['threadsTcp','threadsLocal','thick']

Tests.main(__name__)
