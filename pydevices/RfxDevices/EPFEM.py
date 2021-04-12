#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import time
import socket
import traceback
import logging
import errno
import os
import re
import inspect
import copy
import subprocess


from MDSplus import *


class EPFEM(Device):

    parts = [
        {'path': ':COMMENT', 'type': 'text',
            'value': 'Electrostatic probes front end master'},
        {'path': ':EVT', 'type': 'text', 'value': 'EP_EVT'},
        {'path': ':T_ON', 'type': 'numeric', 'value': 1},
        {'path': ':T_OFF', 'type': 'numeric', 'value': 1},
        {'path': ':N_CYCL', 'type': 'numeric', 'value': 1},
        {'path': ':WD_TMT', 'type': 'numeric', 'value': 1}
    ]

    parts.append({'path': ':TRIG_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('ELBP_PROBE_SERVER', 'PULSE_ON', 50, None), Method(None, 'trig', head))", 'options': ('no_write_shot',)})

    def trig(self):

        try:
            evtName = self.getNode('EVT').data()

            print (self.getPath() + " - EPFEM - trig() - Sent event " + evtName)

            Event.setevent(evtName, "start")
        except:
            Data.execute('DevLogErr($1, $2)', self.getNid(),
                         'Invalid event (:EVT)')

            raise DevBAD_PARAMETER

        return 1
