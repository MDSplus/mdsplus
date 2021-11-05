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

from MDSplus import Tree, Device, mdsExceptions as Exc


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


_common = _mimport("_common")


class Tests(_common.TreeTests):
    shotinc = 1
    tree = 'task'
    TESTS = {'do'}

    def do(self):
        with Tree(self.tree, self.shot, 'new') as tree:
            Device.PyDevice('TestDevice').Add(tree, 'TESTDEVICE')
            tree.write()
        tree.normal()
        for _ in range(2):
            self._doTCLTest(
                'do TESTDEVICE:TASK_TEST', tree=tree)
            self._doExceptionTest(
                'do TESTDEVICE:TASK_ERROR1', Exc.DevUNKNOWN_STATE, tree)
            self._doExceptionTest(
                'do TESTDEVICE:TASK_ERROR2', Exc.DevUNKNOWN_STATE, tree)
            self._doExceptionTest(
                'do TESTDEVICE:TASK_TIMEOUT', Exc.TdiTIMEOUT, tree)


Tests.main()
