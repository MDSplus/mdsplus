#!/usr/bin/env python
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

import os
import shutil
import subprocess
import tempfile
import time

from MDSplus import Connection, GetMany
from MDSplus import Int32, Float32, ADD, Range, setenv, Tree, TreeNNF


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


_common = _mimport("_common")


class Tests(_common.TreeTests, _common.MdsIp):
    index = 0
    trees = ["consub"]
    tree = "con"
    treesub = "consub"
    TESTS = {'io', 'thick', 'thread', 'tunnel', 'tcp', 'write'}

    def thick(self):
        def testnci(thick, local, con, nci):
            l = local.S.__getattribute__(nci)
            t = thick.S.__getattribute__(nci)
            if nci.endswith("_nids"):
                l, t = str(l), str(t)
                try:
                    c = str(con.get("getnci(getnci(S,$),'nid_number')", nci))
                except TreeNNF:
                    c = '[]'
            else:
                c = con.get("getnci(S,$)", nci)
            try:
                self.assertEqual(t, c)
                self.assertEqual(t, l)
            except:
                print(nci, t, l, c)
                raise
        server, server_port = self._setup_mdsip(
            'ACTION_SERVER', 'ACTION_PORT', 7500+self.index, True)

        svr, svr_log = self._start_mdsip(server, server_port, 'thick')
        try:
            con = Connection(server)
            self.assertEqual(
                con.get("zero([1,1,1,1,1,1,1,1],1)").tolist(),
                [[[[[[[[0]]]]]]]])
            with Tree(self.tree, -1, "new") as local:
                local.addNode(self.treesub, "SUBTREE")
                s = local.addNode("S", "SIGNAL")
                s.addTag("tagS")
                s.record = ADD(Float32(1), Float32(2))
                t = local.addNode("T", "TEXT")
                t.addNode("TT", "TEXT").addTag("tagTT")
                t.record = t.TT
                t.TT = "recTT"
                local.write()
            with Tree(self.treesub, -1, "new") as sub:
                sub.addNode("OK")
                sub.write()
            local.normal()
            Tree.setCurrent(self.tree, 7)
            setenv("%s_path" % self.tree, "%s::" % server)
            print(con.get("getenv($//'_path')", self.tree))
            con.get("TreeShr->TreeOpen(ref($),val($),val(1))", self.tree, -1)
            thick = Tree(self.tree, -1)
            thick.createPulse(1)
            thick1 = Tree(self.tree, 1)
            self.assertEqual(
                getattr(local, self.treesub.upper()).OK.nid,
                getattr(thick1, self.treesub.upper()).OK.nid)
            local_filename = local.getFileName()
            thick_filename = thick.getFileName()
            self.assertTrue("::" in thick_filename, thick_filename)
            self.assertTrue(
                local_filename, thick_filename.split("::", 1)[1])
            """ TreeTurnOff / TreeTurnOn """
            thick.S.on = False
            self.assertEqual(local.S.on, False)
            thick.S.on = True
            self.assertEqual(local.S.on, True)
            """ TreeSetCurrentShotId / TreeGetCurrentShotId """
            Tree.setCurrent(self.tree, 1)
            self.assertEqual(Tree.getCurrent(self.tree), 1)
            """ TreeGetRecord / TreeSetRecord """
            self.assertEqual(str(local.S.record), "1. + 2.")
            self.assertEqual(str(thick.S.record), "1. + 2.")
            thick.S.record = ADD(Float32(2), Float32(4))
            self.assertEqual(str(local.S.record), "2. + 4.")
            self.assertEqual(str(thick.S.record), "2. + 4.")
            self.assertEqual(str(local.T.record), str(thick.T.record))
            """ GetDefaultNid / SetDefaultNid """
            self.assertEqual(thick.getDefault(), thick.top)
            thick.setDefault(thick.S)
            self.assertEqual(thick.getDefault(), thick.top.S)
            thick.setDefault(thick.top)
            """ FindNodeWildRemote """
            self.assertEqual(str(thick.getNodeWild("T*")),
                             str(local.getNodeWild("T*")))
            """ FindTagWildRemote """
            self.assertEqual(thick.findTags("*"), local.findTags("*"))
            """ nci """
            thick.S.write_once = True
            self.assertEqual(thick.S.write_once, True)
            for nci in (
                'on', 'depth', 'usage_str', 'dtype', 'length',
                'rlength', 'fullpath', 'minpath', 'member_nids',
                'children_nids', 'rfa', 'write_once',
            ):
                testnci(thick, local, con, nci)
            """ new stuff """
            self.assertEqual(local.getFileName(), con.get(
                "treefilename($,-1)", self.tree))
        finally:
            if svr_log:
                svr_log.close()
            self._stop_mdsip((svr, server))

    def io(self):
        connection = Connection("thread://io")
        """ mdsconnect """
        self.assertEqual(connection.get('_a=1').tolist(), 1)
        self.assertEqual(connection.get('_a').tolist(), 1)
        self.assertEqual(connection.getObject('1:3:1').__class__, Range)
        g = GetMany(connection)
        g.append('a', '1')
        g.append('b', '$', 2)
        g.append('c', '$+$', 1, 2)
        g.execute()
        self.assertEqual(g.get('a'), 1)
        self.assertEqual(g.get('b'), 2)
        self.assertEqual(g.get('c'), 3)

    def _thread_test(self, server):
        def requests(self, c, idx):
            args = [Int32(i+idx+10) for i in range(10)]
            for _ in range(20):
                self.assertEqual(
                    c.get("[$,$,$,$,$,$,$,$,$,$]", *args).tolist(), args)
        connection = Connection(server)
        _common.TestThread.assertRun(100, *(
            _common.TestThread("T%d" % idx, requests, self, connection, idx)
            for idx in range(5)
        ))

    def tcp(self):
        server, server_port = self._setup_mdsip(
            'ACTION_SERVER', 'ACTION_PORT', 7510+self.index, True)
        svr, svr_log = self._start_mdsip(server, server_port, 'tcp')
        try:
            self._thread_test(server)
        finally:
            if svr_log:
                svr_log.close()
            self._stop_mdsip((svr, server))

    def tunnel(self):
        self._thread_test('local://threads')

    def thread(self):
        self._thread_test('thread://threads')

    def write(self):
        count = 100

        def thread(test, name, node, count):
            i = -1
            max_period = 0
            last = start = time.time()
            for i in range(count):
                data = Float32([i*10+1])
                dim = Float32([last])
                node.makeSegment(dim[0], dim[0], data, dim)
                end = time.time()
                max_period = max(end-last, max_period)
                last = end
            i += 1
            test.assertEqual(i, count)
            print("%s: rate=%f, max_period=%f" %
                  (name, i / (end-start), max_period))

        server, server_port = self._setup_mdsip(
            'ACTION_SERVER', 'ACTION_PORT', 7520+self.index, True)
        tempdir = tempfile.mkdtemp()
        try:
            svr, svr_log = self._start_mdsip(server, server_port, 'tcp')
            try:
                con = Connection(server)

                def check(line, *args):
                    sts = con.get(line, *args)
                    self.assertTrue(
                        sts & 1, "error %d in '%s'" % (sts, line))

                check("setenv('test_path='//$)", tempdir)
                for line in (
                    "TreeOpenNew('test', 1)",
                    "TreeAddNode('EV1', _, 6)",
                    "TreeAddNode('EV2', _, 6)",
                    "TreeWrite()",
                    "TreeClose()",
                ):
                    check(line)
                setenv("test_path", "%s::%s" % (server, tempdir))
                tree = Tree("test", 1)
                _common.TestThread.assertRun(
                    100,
                    _common.TestThread('EV1', thread, self,
                                       'EV1', tree.EV1.copy(), count),
                    _common.TestThread('EV2', thread, self,
                                       'EV2', tree.EV2.copy(), count),
                )
            finally:
                if svr_log:
                    svr_log.close()
                self._stop_mdsip((svr, server))
        finally:
            shutil.rmtree(tempdir, ignore_errors=False, onerror=None)


Tests.main()
