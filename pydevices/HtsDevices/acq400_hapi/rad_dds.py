#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
raddds.py specializes Acq400 for RADCELF triple DDS device

- enumerates all site services, available as uut.sX.knob
- simply property interface allows natural "script-like" usage

 - eg
  - uut1.s0.set_arm = 1
 - compared to 
  - set.site1 set_arm=1

- monitors transient status on uut, provides blocking events
- read_channels() - reads all data from channel data service.
Created on Sun Jan  8 12:36:38 2017

@author: pgm
"""

import acq400
import netclient

class RAD3DDS(acq400.Acq400):
    
    def __init__(self, _uut, monitor=True):
            acq400.Acq400.__init__(self, _uut, monitor)
            site = 4
            for sm in [ 'ddsA', 'ddsB', 'ddsC']:                
                self.svc[sm] = netclient.Siteclient(self.uut, acq400.AcqPorts.SITE0+site)
                self.mod_count += 1
                site += 1
            site = 7
            for sm in [ 'clkdA', 'clkdB']:
                self.svc[sm] = netclient.Siteclient(self.uut, acq400.AcqPorts.SITE0+site)
                self.mod_count += 1
                site += 1                