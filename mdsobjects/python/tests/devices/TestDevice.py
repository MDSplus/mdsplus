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
        {'path': ':TASK_ERROR2',                'type': 'TASK',    'options':('no_write_shot','write_once'), 'valueExpr':'Method(3.,"error",head)'},
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
        time.sleep(3)
