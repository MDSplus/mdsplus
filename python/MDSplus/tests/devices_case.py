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

import unittest
from MDSplus import Tree, Device, tdi, TreeNNF


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


_common = _mimport("_common")


class Tests(_common.TreeTests):
    tree = 'devices'
    TESTS = {'mit', 'rfx', 'w7x'}

    @staticmethod
    def getNodeName(t, model):
        """produces a generic path fitting with the 12 char limit of node names"""
        pathparts = model.split('_')
        node = t.top
        if len(model) > 9:
            if len(pathparts) > 1:
                base, name = pathparts[0]+'_', '_'.join(pathparts[1:])
            elif len(model) > 12:
                base, name = model[:10]+'_', '_'+model[10:]
            else:
                return node, model
            try:
                node = node.getNode(base)
            except TreeNNF:
                node = node.addNode(base, 'STRUCTURE')
        else:
            name = model
        return node, name

    def _add_devices(self, package):
        with Tree(self.tree, self.shot, 'new') as t:
            devices = __import__(package).__dict__
            for model in sorted(devices.keys()):
                cls = devices[model]
                if isinstance(cls, type) and issubclass(cls, Device):
                    node, name = self.getNodeName(t, model)
                    cls.Add(node, name)

    def _xyz_devices(self, package, expected=None):
        if expected is None:
            expected = [s for s in (str(s).strip()
                                    for s, p in tdi('%s()' % package))]
        expected.sort()
        passed = []
        with Tree(self.tree, self.shot, 'new') as t:
            for model in expected:
                node, name = self.getNodeName(t, model)
                try:
                    node.addDevice(name, model)
                    passed.append(model)
                    if Device.debug:
                        print('PASSED %s' % model)
                except:
                    if Device.debug:
                        print('FAILED %s' % model)
        self.assertEqual(passed, expected)
        self._add_devices(package)

    def mit(self):
        self._xyz_devices('MitDevices', [
            'CHS_A14', 'DC1394', 'DC1394A', 'DIO2', 'DT196AO',
            'DT200', 'DT_ACQ16', 'INCAA_TR10', 'JRG_ADC32A',
            'JRG_TR1612', 'L6810', 'MATROX'])

    @unittest.skip("not clean yet")
    def rfx(self):
        self._xyz_devices('RfxDevices', )  # check them all

    def w7x(self):
        self._xyz_devices('W7xDevices', [])  # no tdi devices to check


Tests.main()
