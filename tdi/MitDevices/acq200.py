#!/usr/bin/python

import dt100
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

class Acq200:
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

	def setPrePostMode(self, pre=100000, post=100000, trig_src='di3', trig_edge='rising'):
#		self.uut.acqcmd("setInternalClock 20000")
		self.uut.acq2sh("set.pre_post_mode %d %d %s %s" % (pre, post, trig_src, trig_edge,))

	def trigger(self):
		return self.uut.acq2sh("set.dtacq dio_bit 3 P")

	def waitState(self, state):

		return self.uut.waitState(state)


	def __init__(self, uut):
		self.uut = uut

