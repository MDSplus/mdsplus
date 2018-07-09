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

from MDSplus import Device,DevUNKOWN_STATE

class TestDevice(Device):
    parts=[
        {'path': ':ACTIONSERVER',               'type': 'TEXT',    'options':('no_write_shot','write_once')},
        {'path': ':ACTIONSERVER:INIT1',         'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(Dispatch(head.ACTIONSERVER,"INIT",10),Method(None,"init1",head))'},
        {'path': ':ACTIONSERVER:INIT2',         'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(Dispatch(head.ACTIONSERVER,"INIT",head.ACTIONSERVER.INIT1),Method(None,"init2",head))'},
        {'path': ':ACTIONSERVER:PULSE',         'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
        {'path': ':ACTIONSERVER:PULSE:DISPATCH','type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.ACTIONSERVER,"PULSE",10)'},
        {'path': ':ACTIONSERVER:PULSE:TASK',    'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"pulse",head)'},
        {'path': ':ACTIONSERVER:STORE',         'type': 'ACTION',  'options':('no_write_shot','write_once'), 'valueExpr':'Action(node.DISPATCH,node.TASK)'},
        {'path': ':ACTIONSERVER:STORE:DISPATCH','type': 'DISPATCH','options':('no_write_shot','write_once'), 'valueExpr':'Dispatch(head.ACTIONSERVER,"STORE",10)'},
        {'path': ':ACTIONSERVER:STORE:TASK',    'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"store",head)'},
        {'path': ':TASK_TEST',                  'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"test",head)'},
        {'path': ':TASK_ERROR1',                'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(None,"error",head)'},
        {'path': ':TASK_TIMEOUT',               'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(1.,"timeout",head)'},
        {'path': ':TASK_ERROR2',                'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(10.,"error",head)'},
        {'path': ':INIT1_DONE',                 'type': 'NUMERIC', 'options':('no_write_model','write_once')},
        {'path': ':INIT2_DONE',                 'type': 'NUMERIC', 'options':('no_write_model','write_once')},
        {'path': ':PULSE_DONE',                 'type': 'NUMERIC', 'options':('no_write_model','write_once')},
        {'path': ':STORE_DONE',                 'type': 'NUMERIC', 'options':('no_write_model','write_once')},
    ]
    def init1(self):
        self.init1_done.record = time.time()
    def init2(self):
        self.init2_done.record = time.time()
    def pulse(self):
        self.pulse_done.record = time.time()
    def store(self):
        self.store_done.record = time.time()
    def test(self):
        return 'TEST'
    def error(self):
        raise DevUNKOWN_STATE
    def timeout(self):
        time.sleep(10)
