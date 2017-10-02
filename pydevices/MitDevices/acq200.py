#!/usr/bin/python
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
import re

#       12345678901234567890123456789012
IN32 = '-' * 32
Z32  = '0' * 32
IN6  = '-' * 6
Z6   = '0' * 6

GNSR = re.compile('getNumSamples=([0-9]*) pre=([0-9]*) post=([0-9]*) elapsed=([0-9]*)')

GNS_TOTAL = 0
GNS_PRE   = 1
GNS_POST  = 2
GNS_ELAPSED = 3

class ACQ200:
	'models an ACQ200 class intelligent digitizer'
	def set_dio32(self, value = IN32):
		return self.uut.acq2sh("set.dio32 " + value)

	def set_dio32_bit(self, bit) :
		bits = Z32[:bit] + "1" + Z32[bit+1:]
		self.set_dio32(bits)
		return bits

	def get_dio32(self):
		return self.uut.acq2sh("get.dio32")

	def set_dio6(self, value = IN6):
		return self.uut.acqcmd("setDIO " + value)

	def set_dio6_bit(self, bit, value = 0) :
		self.uut.acq2sh("set.dtacq dio_bit %d %s" % (bit, value))

	def get_dio6(self):
		return self.uut.acqcmd("getDIO")

	def set_route(self, dx, route = "in fpga"):
		return self.uut.acq2sh("set.route "+dx+" "+route)

	def clear_routes(self):
		for dx in ['d0', 'd1', 'd2', 'd3', 'd4', 'd5' ] :
			self.uut.acq2sh("set.route " + dx + " in fpga")


	def get_state(self):
		return self.uut.acqcmd("getState")

	def get_numSamples(self):
		reply = self.uut.acqcmd("getNumSamples")
		amatch = GNSR.match(reply)
		return amatch.groups()

	def set_arm(self):
		return self.uut.acqcmd("setArm")

	def set_abort(self):
		return self.uut.acqcmd("setAbort")

	def get_host(self):
		return self.uut.host

	def setChannelCount(self, nchan):
		self.uut.acqcmd("setChannelMask " + '1' * nchan)

	def setPrePostMode(self, pre=100000, post=100000, trig_src='DI3', trig_edge='rising'):
		self.uut.acq2sh("set.pre_post_mode %d %d %s %s" % (pre, post, trig_src, trig_edge,))

	def trigger(self):
		return self.uut.acq2sh("set.dtacq dio_bit 3 P")

	def waitState(self, state):

		return self.uut.waitState(state)


	def __init__(self, uut):
		self.uut = uut

