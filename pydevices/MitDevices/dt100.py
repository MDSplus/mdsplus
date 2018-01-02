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
import pexpect
import re
import transport
import array
import os

debug = 0

if os.getenv("LOGCMD", "NO") == "YES":
	logcmd = 1
else:
	logcmd = 0

def lprint(s):
	if logcmd:
		print s

def dprint(s):
	if debug:
		print s

class Connection:
	def __init__(self, _p):
		self.p = _p


class DT100(transport.Transport):
	'connects to remote dt100 server, holds open connections and handles transactions'

	def logtx(self, s):
		global logcmd
		if logcmd > 0:
			print "%s => \"%s\"" % (self.host, s)

	def logrx(self, s):
		global logcmd
		if logcmd > 0:
			print "%s <= \"%s\"" % (self.host, s)

	def _connect(self):
		hp = self.host.split(":")
		if len(hp)==2:
# it's a tunnel ...
			target = hp[0] + ' ' + hp[1]
		else:
			target = self.host + ' ' + '53504'

		return Connection(pexpect.spawn('nc ' + target))

	def connectMaster(self):
		dprint("connectMaster( " + self.host + " )")
		self.acq = self._connect()
		self.acq.p.expect('MasterInterpreter')
		self.acq.p.sendline('dt100 open master 1')
		i = self.acq.p.expect("DT100:\r", timeout=60);
		if i==0:
			dprint("OK")
		else:
			print "Timeout"

	def connectShell(self):
		dprint("connectShell( " + self.host + " )")
		self.sh = self._connect()
		self.sh.p.expect('MasterInterpreter')
		self.sh.p.sendline('dt100 open shell 1')
		i = self.sh.p.expect("DT100:\r", timeout=60);
		if i==0:
			dprint("OK")
		else:
			print "Timeout"

	def connectStatemon(self):
		hp = self.host.split(":")
		if len(hp)==2:
			# it's a tunnel ...
			port = int(hp[1]) + 1
			target = hp[0] + ' ' + str(port)
		else:
			target = self.host + ' ' + '53535'

		dprint("connectStatemon(" + target)

		self.statemon = pexpect.spawn('nc ' + target)
		self.statemon.first_time = 1
		self.statemon.arm_time = self.statemon.stop_time = 0
		#self.statemon = pexpect.spawn('nc ' + self.host + ' ' + '53535')

	def connectChannel(self, channel):
		channel_dev = "/dev/acq32/acq32.1.%02d" % channel
		dprint("connectChannel( " + self.host + " " + channel_dev + " )")
		ch = self._connect()
		ch.p.expect('MasterInterpreter')
		dprint("sendline dt100 open data1 " + channel_dev)
		ch.p.sendline('dt100 open data1 ' + channel_dev)
		dprint("expect:");
		i = ch.p.expect("DT100:\r", timeout=60);
		if i==0:
			dprint("OK")
		else:
			print "Timeout"
		return ch

	def acqcmd(self, command):
		tx = "acqcmd " + command
		self.logtx(tx)
		self.acq.p.sendline(tx)
		self.acq.m = re.compile('ACQ32:(.*)\r')
		i = self.acq.p.expect(self.acq.m, timeout=60);
		if i==0:
			self.logrx(self.acq.p.match.group(0))
			return self.acq.p.match.group(1)
		else:
			print "Timeout"
			return 0

	def acq2sh(self, command):
		self.logtx(command)
		self.sh.p.sendline(command)
		self.sh.m = re.compile('(.*)\r\nEOF(.*)\r\n')
		i = self.sh.p.expect(self.sh.m, timeout=60);
		if i==0:
			dprint("OK")
			return self.sh.p.match.group(1)
		else:
			print "Timeout"
			return 0

	def waitState(self, state):
		regstr = '([0-9\.]*) [0-9] ST_(.*)\r\n'
		dprint ("waitState %s" % regstr)
		wantex = re.compile(regstr)

		while self.statemon.expect([wantex, pexpect.TIMEOUT], timeout=60) == 0:
			dprint(self.statemon.after)
			if self.statemon.match.group(2) == "ARM":
				self.statemon.arm_time = (float)(self.statemon.match.group(1))
				self.statemon.stop_time = 0
			elif self.statemon.match.group(2) == "STOP":
				self.statemon.stop_time = (float)(self.statemon.match.group(1))
			if self.statemon.match.group(2) == state:
				if state == "STOP" and self.statemon.first_time == 1:
					dprint("pass first time")
					pass
				else:
					break
			self.statemon.first_time = 0

#		print self.statemon.match.group(0)
#	print "match %s" % (self.statemon.match.group(1))

		active_time = 0

		if self.statemon.arm_time != 0 and self.statemon.stop_time != 0:
			active_time = self.statemon.stop_time - self.statemon.arm_time
			self.statemon.arm_time = self.statemon.stop_time = 0
			dprint("Active time: %f" % active_time)

		return [self.statemon.match.group(2), active_time]

	def readChannel(self, channel, nsamples, start=0, stride=1, format='h'):
		ch = self.connectChannel(channel)
		read_spec = "dt100 read %d %d %d" % (start, start+nsamples, stride)
		self.logtx("sendline:" + read_spec)
		ch.p.sendline(read_spec)
		read_rep = re.compile('DT100:([0-9]*) bytes\r\n')
		rc = ch.p.expect(read_rep, timeout=60)
		if rc == 0:
			nbytes = int(ch.p.match.group(1))
			data = array.array(format)
			data.fromstring(ch.p.read(nbytes))
			dprint(data)
			return data
		else:
			dprint("ERROR")
			return "ERROR"



	def __init__(self, _host):
		'create a transport host is a DNS name, or \'.D\' in A.B.C.D where $SUBNET=A.B.C'
		dprint("Dt100__init__ host:" + _host)
		if _host.startswith('.'):
			self.host = str(os.getenv("SUBNET")) + _host
		else:
			self.host = _host

		if os.getenv("CONNECT", "YES") == "YES":
		    self.connectMaster()
		    self.connectShell()
		    self.connectStatemon()
