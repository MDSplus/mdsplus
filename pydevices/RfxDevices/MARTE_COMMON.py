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

from MDSplus import Device, Event
from os import environ
from time import sleep


class MARTE_COMMON(Device):
    """ MDSplus device superclass for MARTe applications """
    parts = [
        {'path': ':COMMENT', 'type': 'text'},
        {'path': ':ID', 'type': 'numeric', 'value': 0},
        {'path': ':CONTROL', 'type': 'text', 'value': 'CONTROL'},
        {'path': '.SIGNALS', 'type': 'structure'},
        {'path': '.SIGNALS:NAMES', 'type': 'text'},
    ]
    # Maximim number of stored signals: 256
    for i in range(256):
        parts.extend([
            {'path': '.SIGNALS:SIGNAL_%03d' % (i+1), 'type': 'structure'},
            {'path': '.SIGNALS:SIGNAL_%03d:NAME' % (i+1), 'type': 'text'},
            {'path': '.SIGNALS:SIGNAL_%03d:DESCRIPTION' % (
                i+1), 'type': 'text'},
            {'path': '.SIGNALS:SIGNAL_%03d:DATA' % (i+1), 'type': 'signal'},
        ])
    del(i)
    parts.extend([
        {'path': ':INIT_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('MARTE_SERVER','SEQ_INIT',50,None),Method(None,'init',head))",
         'options': ('no_write_shot',)},
        {'path': ':STORE_ACTION', 'type': 'action',
         'valueExpr': "Action(Dispatch('MARTE_SERVER','SEQ_STORE',50,None),Method(None,'store',head))",
         'options': ('no_write_shot',)},
    ])

    def getEventName(self):
        if environ.get("MARTE_EVENT") is None:
            return "MARTE"
        else:
            return environ["MARTE_EVENT"]

    def init(self):
        """ init method
        will send a SETUP event with the required information to allow
        MDSInterface service retrieving parameter and signal information
        """
        eventStr = "SETUP " + self.tree.name + " " + self.control.data() + " " + \
            str(self.tree.shot) + " " + str(self.id.data()) + " "

        eventStr = eventStr + " " + str(self.params.nid)
        eventStr = eventStr + " " + str(self.wave_params.nid)
        eventStr = eventStr + " " + str(self.signals.nid)
        print(eventStr)
        Event.setevent(self.getEventName(), eventStr)
        sleep(1)
        return 1

    def load(self):
        """load method
        will send a LOAD event forcing reporting in MARTe confirguration
        the actual value of MDSplus parameters.
        GAM field MdsId will specify the target device
        for every GAM taking MDSplus parameters
        """
        eventStr = "LOAD"
        Event.setevent(self.getEventName(), eventStr)
        return 1

    # Event transition requests
    def pre_req(self):
        eventStr = "PRE_REQ " + str(self.id.data())
        Event.setevent(self.getEventName(), eventStr)
        return 1

    def pulse_req(self):
        eventStr = "PULSE_REQ"
        Event.setevent(self.getEventName(), eventStr)
        return 1

    def post_req(self):
        eventStr = "POST_REQ"
        Event.setevent(self.getEventName(), eventStr)
        return 1

    def collection_complete(self):
        eventStr = "COLLECTION_COMPLETE"
        Event.setevent(self.getEventName(), eventStr)
        return 1

    def abort(self):
        eventStr = "ABORT"
        Event.setevent(self.getEventName(), eventStr)
        return 1

    def store(self):
        """store method
        will force flush buffered data.
        Typially called after COLLECTION_COMPLETE event
        """
        eventStr = "STORE " + str(self.id.data())
        print(eventStr)
        Event.setevent(self.getEventName(), eventStr)
        sleep(2)
        return 1
