#!/usr/bin/env python
#
# Copyright (c) 2021, Massachusetts Institute of Technology All rights reserved.
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


import MDSplus
import importlib


acq2106_435st = importlib.import_module('acq2106_435st')

class _ACQ2106_435SC(acq2106_435st._ACQ2106_435ST):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """
    
    sc_parts = [
        {
            # Resampling factor. This is used during streaming by makeSegmentResampled()
            'path': ':RES_FACTOR',
            'type': 'numeric',
            'value': 100,
            'options': ('write_shot',)
        },
    ]

    def init(self):
        
        self.slots = super(_ACQ2106_435SC, self).getSlots()
        freq = int(self.freq.data())

        # Available Clock Plans for the 2106 Signal Conditioning devices (from D-Tacq): 
        # 10 kSPS (5M12  /512)
        # 20 kSPS (10M24 /512)
        # 40 kSPS (20M48 /512)
        # 80 kSPS (20M48 /256)
        # 128kSPS (32M768/256)
        allow_freqs = [10000, 20000, 40000, 80000, 128000]

        # Frequency innput validation
        if freq not in allow_freqs:
            raise MDSplus.DevBAD_PARAMETER(
                "FREQ must be 10000, 20000, 40000, 80000 or 128000; not %d" % (freq,))

        for site in self.slots:
            self.setGainsOffsets(site)
            if self.debug:
                print("GAINs Committed for site %s" % (site,))
                
        # Here, the argument to the init of the superclass:
        # - init(True) => use resampling function:
        # makeSegmentResampled(begin, end, dim, b, resampled, res_factor)
        super(_ACQ2106_435SC, self).init(resampling=True)

    INIT=init

    
    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.Acq2106(self.node.data(), monitor=False, has_wr=True)
        return uut

    def computeGains(self, g):
        g1opts = [ 1000, 100, 10, 1 ]
        g2opts = [ 1, 2, 5, 10 ]
        
        for g1opt in g1opts:
            if g >= g1opt:
                g1 = g1opt
                g2 = g // g1opt
                if g2 not in g2opts:
                    raise MDSplus.DevBAD_PARAMETER(
                            "SC_GAIN must be computable from (one of: 1000, 100, 10, 1) * (one of: 1, 2, 5, 10) ; not %d" % (g,))
                return (g1, g2)

    def setGainsOffsets(self, site):
        import epics
        import socket

        if site not in [1, 3, 5]:
            # (slw) TODO: Improve
            print('site is not 1, 3, or 5')
            return

        # TODO: we should probably replace this with a EPICS_NAME node instead of asking for the hostname
        domainName = socket.gethostbyaddr(str(self.node.data()))[0]
        splitDomainName = domainName.split(".")

        #For EPICS PV definitions hardcoded in D-Tacq's "/tmp/records.dbl", 
        # the ACQs DNS hostnames/domain names should be of the format <chassis name> _ <three digits serial number>
        epicsDomainName = splitDomainName[0].replace("-", "_")

        # Choosing the input offset (0, 32, 64) depending of the selected site:
        input_offset = { 1: 0, 3: 32, 5: 64 }[site]

        for i in range(32):
            gain = str(getattr(self, 'INPUT_%3.3d:SC_GAIN' % (i + input_offset + 1,)).data())

            parts = gain.split(",")

            if len(parts) == 2:
                gain1 = int(float(parts[0]))
                gain2 = int(float(parts[1]))
            else:
                gain1, gain2 = self.computeGains(int(float(gain)))

            offset = getattr(self, 'INPUT_%3.3d:SC_OFFSET' % (i + input_offset + 1,)).data()

            pvg1 = "{}:{}:SC32:G1:{:02d}".format(epicsDomainName, site, i + 1)
            pv = epics.PV(pvg1)
            valueg1 = str(gain1)
            pv.put(valueg1, wait=True)

            pvg2 = "{}:{}:SC32:G2:{:02d}".format(epicsDomainName, site, i + 1)
            pv = epics.PV(pvg2)
            valueg2 = str(gain2)
            pv.put(valueg2, wait=True)

            pvg3 = "{}:{}:SC32:OFFSET:{:02d}".format(epicsDomainName, site, i + 1)
            pv = epics.PV(pvg3)
            valueg3 = str(offset)
            pv.put(valueg3, wait=True)

        pv = epics.PV('{}:{}:SC32:GAIN:COMMIT'.format(epicsDomainName, site))
        pv.put('1')

    # target_path = path to the node to call setSegmentScale on.
    # input_path = path to parent node for the input channel, typically INPUT_xxx, default to target_path
    def setChanScale(self, target_path, input_path=None):
        target_node = self.__getattr__(target_path)
        input_node = target_node

        if input_path:
            input_node = self.__getattr__(input_path)

        # Attention: For versions of the firmware v498 or greater in all the ACQ SC32 the calibration coefficients and offsets 
        # already take into account the gains and offsets of the signal conditioning stages.
        
        # = (coefficient * value) + offset
        target_node.setSegmentScale(
            MDSplus.ADD(
                MDSplus.MULTIPLY(
                    input_node.COEFFICIENT, 
                    MDSplus.dVALUE()
                ), 
                input_node.OFFSET
            )
        )


def assemble(cls):
    cls.parts = list(_ACQ2106_435SC.carrier_parts + _ACQ2106_435SC.sc_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i+1,),
                'type': 'SIGNAL', 
                'valueExpr': 'head.setChanScale("INPUT_%3.3d")' % (i+1,),
                'options': ('no_write_model', 'write_once',)
            },
            {
                'path': ':INPUT_%3.3d:DECIMATE' % (i+1,),
                'type':'NUMERIC', 
                'valueExpr':'head.def_dcim',
                'options':('no_write_shot',)
            },           
            {
                'path': ':INPUT_%3.3d:COEFFICIENT' % (i+1,), 
                'type':'NUMERIC',
                'options':('no_write_model', 
                'write_once',)
            },
            {
                'path': ':INPUT_%3.3d:OFFSET' % (i+1,),
                'type':'NUMERIC',
                'options':('no_write_model', 'write_once',)
            },
            {
                # Local (per channel) SC gains
                'path': ':INPUT_%3.3d:SC_GAIN' % (i+1,),
                'type':'TEXT', 
                'value':1,
                'options':('no_write_shot',)
            },
            {
                # Local (per channel) SC offsets
                'path': ':INPUT_%3.3d:SC_OFFSET' % (i+1,),
                'type':'NUMERIC', 
                'value':0,
                'options':('no_write_shot',)
            },   
            {
                # Re-sampling streaming data goes here:
                'path': ':INPUT_%3.3d:RESAMPLED' % (i+1,),
                'type': 'SIGNAL', 
                'valueExpr': 'head.setChanScale("INPUT_%3.3d:RESAMPLED", "INPUT_%3.3d")' % (i+1, i+1),
                'options': ('no_write_model', 'write_once',)
            },
        ]

class ACQ2106_435SC_1ST(_ACQ2106_435SC): 
    sites=1

assemble(ACQ2106_435SC_1ST)

class ACQ2106_435SC_2ST(_ACQ2106_435SC): 
    sites=2

assemble(ACQ2106_435SC_2ST)

class ACQ2106_435SC_3ST(_ACQ2106_435SC): 
    sites=3

assemble(ACQ2106_435SC_3ST)

del(assemble)
