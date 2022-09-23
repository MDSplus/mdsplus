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

acq2106_435sm = importlib.import_module('acq2106_435sm')

class _ACQ2106_435SCSM(acq2106_435sm._ACQ2106_435SM):
    """
    D-Tacq ACQ2106 Signal Conditioning support.
    """
    
    sc_parts = [
        {
            'path': ':EPICS_NAME',
            'type': 'string',
            'value': 'acq2106_xxx',
            'options': ('write_model',)
        }
    ]

    def getSlots(self):
        uut = self.getUUT()
        
        # Ask UUT what are the sites that are actually being populatee with a 435ELF
        slot_list = {}
        for (site, _) in sorted(uut.modules.items()):
            site_number = int(site)
            if site_number == 1:
                slot_list[site_number] = uut.s1
            elif site_number == 2:
                slot_list[site_number] = uut.s2
            elif site_number == 3:
                slot_list[site_number] = uut.s3
            elif site_number == 4:
                slot_list[site_number] = uut.s4
            elif site_number == 5:
                slot_list[site_number] = uut.s5
            elif site_number == 6:
                slot_list[site_number] = uut.s6
        return slot_list

    def init(self):
        uut = self.getUUT()
        self.slots = self.getSlots()
        freq = str(self.freq.data())

        # Available Clock Plans for the 2106 Signal Conditioning devices (from D-Tacq): 
        # 10 kSPS (5M12  /512)
        # 20 kSPS (10M24 /512)
        # 40 kSPS (20M48 /512)
        # 80 kSPS (20M48 /256)
        # 128kSPS (32M768/256)

        to_nano = 1E9
        # Dictionay of freqs:WR Nanosec per Ticks
        allowed_freqs_tickns = {'10000':(1./5120000)*to_nano, '20000':(1./10240000)*to_nano, '40000':(1./20480000)*to_nano, 
                        '80000':(1./20480000)*to_nano, '128000':1./32768000*to_nano}

        if freq in allowed_freqs_tickns:
            uut.cC.WRTD_TICKNS = allowed_freqs_tickns.get(freq)
        else:
            raise MDSplus.DevBAD_PARAMETER(
                "FREQ must be 10000, 20000, 40000, 80000 or 128000; not %s" % (freq,))

        for site in self.slots:
            self.setGainsOffsets(site)
            if self.debug:
                print("GAINs Committed for site %s" % (site,))
                
        super(_ACQ2106_435SCSM, self).init()

    INIT=init
    
    def getUUT(self):
        import acq400_hapi
        uut = acq400_hapi.factory(self.node.data())
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

        if site not in [1, 3, 5]:
            # (slw) TODO: Improve
            print('site is not 1, 3, or 5')
            return

        epicsDomainName = self.epics_name.data()

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

        # target_path = path to the node to call setSegmentScale on.
    # input_path = path to parent node for the input channel, typically INPUT_xxx, default to target_path
    def setTransientChanScale(self, target_path, input_path=None):
        target_node = self.__getattr__(target_path)
        input_node = target_node

        if input_path:
            input_node = self.__getattr__(input_path)

        # Attention: For versions of the firmware v498 or greater in all the ACQ SC32 the calibration coefficients and offsets 
        # already take into account the gains and offsets of the signal conditioning stages.
        
        # = (coefficient * value) + offset
        target_node.TRANSIENT.setSegmentScale(
            MDSplus.ADD(
                MDSplus.MULTIPLY(
                    input_node.COEFFICIENT, 
                    MDSplus.dVALUE()
                ), 
                input_node.OFFSET
            )
        )

def assemble(cls):
    cls.parts = list(_ACQ2106_435SCSM.carrier_parts + _ACQ2106_435SCSM.sc_parts)
    for i in range(cls.sites*32):
        cls.parts += [
            {
                'path': ':INPUT_%3.3d' % (i+1,),
                'type': 'SIGNAL', 
                'valueExpr': 'head.setChanScale("INPUT_%3.3d")' % (i + 1,),
                'options': ('no_write_model', 'write_once',)
            },        
            {
                'path': ':INPUT_%3.3d:COEFFICIENT' % (i+1,), 
                'type':'NUMERIC',
                'options':('no_write_model', 'write_once',)
            },
            {
                'path': ':INPUT_%3.3d:OFFSET' % (i+1,),
                'type':'NUMERIC',
                'options':('no_write_model', 'write_once',)
            },
            {
                 # Capture transient signal goes here:
                'path': ':INPUT_%3.3d:TRANSIENT' % (i + 1,),
                'type': 'SIGNAL',
                'valueExpr': 'head.setTransientChanScale("INPUT_%3.3d")' % (i + 1,),
                #'options': ('no_write_model',)
            },
            {
                # Local (per channel) SC gains
                'path': ':INPUT_%3.3d:SC_GAIN' % (i+1,),
                'type':'NUMERIC', 
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
        ]

class ACQ2106_435SCSM_1ST(_ACQ2106_435SCSM): 
    sites=1

assemble(ACQ2106_435SCSM_1ST)

class ACQ2106_435SCSM_2ST(_ACQ2106_435SCSM): 
    sites=2

assemble(ACQ2106_435SCSM_2ST)

class ACQ2106_435SCSM_3ST(_ACQ2106_435SCSM): 
    sites=3

assemble(ACQ2106_435SCSM_3ST)

del(assemble)
