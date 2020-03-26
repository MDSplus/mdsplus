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

import time
import threading
from MDSplus import Device, DevUNKOWN_STATE, Int32Array
from MDSplus import with_mdsrecords, mdsrecord, cached_property


@with_mdsrecords
class TestDevice(Device):
    parts = [
        dict(
            path=':ACTIONSERVER',
            type='TEXT',
            options=('no_write_shot', 'write_once'),
        ),
        dict(
            path=':ACTIONSERVER:INIT1',
            type='ACTION',
            options=('no_write_shot', 'write_once'),
            valueExpr=(
             'Action('
             'Dispatch(head.ACTIONSERVER, "INIT", 10),'
             'Method(None, "init1", head))'),
        ),
        dict(
            path=':ACTIONSERVER:INIT2',
            type='ACTION',
            options=('no_write_shot', 'write_once'),
            valueExpr=(
             'Action('
             'Dispatch(head.ACTIONSERVER, "INIT", head.ACTIONSERVER.INIT1),'
             'Method(None, "init2", head))'),
        ),
        dict(
            path=':ACTIONSERVER:PULSE',
            type='ACTION',
            options=('no_write_shot', 'write_once'),
            valueExpr='Action(node.DISPATCH, node.TASK)'
        ),
        dict(
            path=':ACTIONSERVER:PULSE:DISPATCH',
            type='DISPATCH',
            options=('no_write_shot', 'write_once'),
            valueExpr='Dispatch(head.ACTIONSERVER, "PULSE", 10)'
        ),
        dict(
            path=':ACTIONSERVER:PULSE:TASK',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(None, "pulse", head)'
        ),
        dict(
            path=':ACTIONSERVER:STORE',
            type='ACTION',
            options=('no_write_shot', 'write_once'),
            valueExpr='Action(node.DISPATCH, node.TASK)',
        ),
        dict(
            path=':ACTIONSERVER:STORE:DISPATCH',
            type='DISPATCH',
            options=('no_write_shot', 'write_once'),
            valueExpr='Dispatch(head.ACTIONSERVER, "STORE", 10)',
        ),
        dict(
            path=':ACTIONSERVER:STORE:TASK',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(None,"store",head)',
        ),
        dict(
            path=':ACTIONSERVER:MANUAL',
            type='ACTION',
            options=('no_write_shot', 'write_once'),
            valueExpr='Action(node.DISPATCH,node.TASK)',
        ),
        dict(
            path=':ACTIONSERVER:MANUAL:DISPATCH',
            type='DISPATCH',
            options=('no_write_shot', 'write_once'),
            valueExpr='Dispatch(head.ACTIONSERVER, "MANUAL", 10)',
        ),
        dict(
            path=':ACTIONSERVER:MANUAL:TASK',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(None,"manual",head)',
        ),
        dict(
            path=':TASK_TEST',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(None, "test", head)',
        ),
        dict(
            path=':TASK_ERROR1',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(None, "error", head)'
        ),
        dict(
            path=':TASK_TIMEOUT',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(1., "timeout", head)',
        ),
        dict(
            path=':TASK_ERROR2',
            type='TASK',
            options=('no_write_shot', 'write_once'),
            valueExpr='Method(10., "error", head)',
        ),
        dict(
            path=':INIT1_DONE',
            type='NUMERIC',
            options=('no_write_model', 'write_once'),
            filter=float,
            cached=0,  # cache_on_set = False
        ),
        dict(
            path=':INIT2_DONE',
            type='NUMERIC',
            options=('no_write_model', 'write_once'),
            filter=float,
            cached=1,  # cache_on_set = True
        ),
        dict(
            path=':PULSE_DONE',
            type='NUMERIC',
            options=('no_write_model', 'write_once'),
            filter=float,
        ),
        dict(
            path=':STORE_DONE',
            type='NUMERIC',
            options=('no_write_model', 'write_once'),
            filter=float,
        ),
        dict(
            path=':MANUAL_DONE',
            type='NUMERIC',
            options=('no_write_model', 'write_once'),
            filter=float,
            default=-1,
        ),
        dict(
            path=':DATA',
            type='SIGNAL',
            options=('no_write_model', 'write_once'),
        ),
    ]

    class Worker(threading.Thread):
        """An async worker should be a proper class
           This ensures that the methods remian in memory
           It should at least take one argument: teh device node
        """
        def __init__(self, dev):
            super(TestDevice.Worker, self).__init__(name=dev.path)
            # make thread safe copy of device node with a non-global context
            self.dev = dev.copy()

        def run(self):
            self.dev.tree.normal()
            self.dev.DATA.beginSegment(0, 9,
                                       Int32Array(range(10)),
                                       Int32Array([0]*10))
            for i in range(10):
                time.sleep(.1)
                self.dev.DATA.putSegment(Int32Array([i]), i)

    def init1(self):
        self._init1_done = time.time()

    def init2(self):
        """start async worker"""
        thread = self.Worker(self)
        thread.start()
        # store thread reference in persistent field
        self.persistent = dict(thread=thread)
        self._init2_done = time.time()

    def pulse(self):
        self._pulse_done = time.time()

    def store(self):
        """joins async worker"""
        self.persistent['thread'].join()
        # cleanup thread reference
        self.persistent = None
        self._store_done = time.time()

    def manual(self):
        self._manual_done = time.time()

    def test(self):
        return 'TEST'

    def error(self):
        raise DevUNKOWN_STATE

    def timeout(self):
        time.sleep(10)

    @mdsrecord(filter=int, default=0)
    def done(self): return self.manual_done

    _reg = 0  # used to simulate volatile readout
    @cached_property
    def cache(self):
        self._reg += 1
        return self._reg
