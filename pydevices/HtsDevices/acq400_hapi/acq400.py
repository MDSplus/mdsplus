#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
acq400.py interface to one acq400 appliance instance
- enumerates all site services, available as uut.sX.knob
- simple property interface allows natural "script-like" usage
 - eg
  - uut1.s0.set_arm = 1
 - equivalent to running this on a logged in shell session on the UUT:
  - set.site1 set_arm=1
- monitors transient status on uut, provides blocking events
- read_channels() - reads all data from channel data service.
Created on Sun Jan  8 12:36:38 2017

@author: pgm
"""

import threading
import re

import os
import errno
import signal
import sys
import netclient
import numpy
import socket
import timeit
import time

class AcqPorts:
    """server port constants"""
    TSTAT = 2235
    STREAM = 4210
    SITE0 = 4220
    SEGSW = 4250
    SEGSR = 4251
    GPGSTL= 4541
    GPGDUMP = 4543

    BOLO8_CAL = 45072
    DATA0 = 53000
    LIVETOP = 53998
    ONESHOT = 53999
    AWG_ONCE = 54201
    AWG_AUTOREARM = 54202
    MGTDRAM = 53990


class SF:
    """state constants"""
    STATE = 0
    PRE = 1
    POST = 2
    ELAPSED = 3
    DEMUX = 5

class STATE:
    """transient states"""
    IDLE = 0
    ARM = 1
    RUNPRE = 2
    RUNPOST = 3
    POPROCESS = 4
    CLEANUP = 5
    @staticmethod
    def str(st):
        if st==STATE.IDLE:
            return "IDLE"
        if st==STATE.ARM:
            return "ARM"
        if st==STATE.RUNPRE:
            return "RUNPRE"
        if st==STATE.RUNPOST:
            return "RUNPOST"
        if st==STATE.POPROCESS:
            return "POPROCESS"
        if st==STATE.CLEANUP:
            return "CLEANUP"
        return "UNDEF"

class StreamClient(netclient.Netclient):
    """handles live streaming data"""
    def __init__(self, addr):
        print("worktodo")

class Channelclient(netclient.Netclient):
    """handles post shot data for one channel.

    Args:
        addr (str) : ip address or dns name
        ch (int) : channel number 1..N

    """
    def __init__(self, addr, ch):
        netclient.Netclient.__init__(self, addr, AcqPorts.DATA0+ch)

    def read(self, ndata, data_size=2, maxbuf=4096):
        """read ndata from channel data server, return as numpy array.
        Args:
            ndata (int): number of elements
            data_size : 2|4 short or int
            maxbuf=4096 : max bytes to read per packet

        Returns:
            :numpy: data array

        @@todo buffer +=
        # this is probably horribly inefficient
        # probably better:
        - retbuf = numpy.array(dtype, ndata)
        - retbuf[cursor].
        """
        buffer = self.sock.recv(maxbuf)
        while len(buffer) < ndata*data_size:
            buffer += self.sock.recv(maxbuf)

        _dtype = numpy.dtype('i4' if data_size == 4 else 'i2')

        return numpy.frombuffer(buffer, dtype=_dtype, count=ndata)


class ExitCommand(Exception):
    pass


def signal_handler(signal, frame):
    raise ExitCommand()

class Statusmonitor:
    st_re = re.compile(r"([0-9]) ([0-9]+) ([0-9]+) ([0-9]+) ([0-9])+" )

    def __repr__(self):
        return repr(self.logclient)
    def st_monitor(self):
        while self.quit_requested == False:
            st = self.logclient.poll()
            match = self.st_re.search(st)
            # status is a match. need to look at group(0). It's NOT a LIST!
            if match:
                statuss = match.groups()
                status1 = [int(x) for x in statuss]
                if self.trace:
                    print("%s <%s" % (repr(self), status1))
                if self.status != None:
#                    print("Status check %s %s" % (self.status0[0], status[0]))
                    if self.status[SF.STATE] != 0 and status1[SF.STATE] == 0:
                        print("%s STOPPED!" % (self.uut))
                        self.stopped.set()
                        self.armed.clear()
#                print("status[0] is %d" % (status[0]))
                    if status1[SF.STATE] == 1:
                        print("%s ARMED!" % (self.uut))
                        self.armed.set()
                        self.stopped.clear()
                    if self.status[SF.STATE] == 0 and status1[SF.STATE] > 1:
                        print("ERROR: %s skipped ARM %d -> %d" % (self.uut, self.status[0], status1[0]))
                        self.quit_requested = True
                        os.kill(self.main_pid, signal.SIGINT)
                        sys.exit(1)
                self.status = status1
            elif self.trace > 1:
                print("%s <%s>" % (repr(self), st))

    def get_state(self):
        return self.status[SF.STATE]

    def wait_event(self, ev, descr):
    #       print("wait_%s 02 %d" % (descr, ev.is_set()))
        while ev.wait(0.1) == False:
            if self.quit_requested:
                print("QUIT REQUEST call exit %s" % (descr))
                sys.exit(1)

#        print("wait_%s 88 %d" % (descr, ev.is_set()))
        ev.clear()
#        print("wait_%s 99 %d" % (descr, ev.is_set()))

    def wait_armed(self):
        """
        blocks until uut is ARMED
        """
        self.wait_event(self.armed, "armed")

    def wait_stopped(self):
        """
        blocks until uut is STOPPED
        """
        self.wait_event(self.stopped, "stopped")

    trace = int(os.getenv("STATUSMONITOR_TRACE", "0"))

    def __init__(self, _uut, _status):
        self.quit_requested = False
        self.trace = Statusmonitor.trace
        self.uut = _uut
        self.main_pid = os.getpid()
        self.status = _status
        self.stopped = threading.Event()
        self.armed = threading.Event()
        self.logclient = netclient.Logclient(_uut, AcqPorts.TSTAT)
        self.st_thread = threading.Thread(target=self.st_monitor)
        self.st_thread.setDaemon(True)
        self.st_thread.start()


class NullFilter:
    def __call__ (self, st):
        pass
null_filter = NullFilter()

class ProcessMonitor:
    st_re = re.compile(r"^END" )

    def st_monitor(self):
        while self.quit_requested == False:
            st = self.logclient.poll()
            self.output_filter(st)
            match = self.st_re.search(st)
            if match:
                self.quit_requested = True

    def __init__(self, _uut, _filter):
        self.quit_requested = False
        self.output_filter = _filter
        self.logclient = netclient.Logclient(_uut, AcqPorts.MGTDRAM)
        self.logclient.termex = re.compile("(\n)")
        self.st_thread = threading.Thread(target=self.st_monitor)
        self.st_thread.setDaemon(True)
        self.st_thread.start()

class Acq400:
    """
    host-side proxy for Acq400 uut.

    discovers and maintains all site servers
    maintains a monitor thread on the monitor port
    handles multiple channel post shot upload

    Args:
        _uut (str) : ip-address or dns name
        monitor=True (bool) : set false to stub monitor,
          useful for tracing on a second connection to an active system.
    """
    @property
    def mod_count(self):
        return self.__mod_count

    def __init__(self, _uut, monitor=True):
        self.NL = re.compile(r"(\n)")
        self.uut = _uut
        self.trace = 0
        self.save_data = None
        self.svc = {}
        self.modules = {}
        self.__mod_count = 0
        s0 = self.svc["s0"] = netclient.Siteclient(self.uut, AcqPorts.SITE0)
        sl = s0.SITELIST.split(",")
        sl.pop(0)
        self.awg_site = 0
        for sm in sl:
            site = int(sm.split("=").pop(0))
            svc = netclient.Siteclient(self.uut, AcqPorts.SITE0+site)
            self.svc["s%d" % site] = svc
            self.modules[site] = svc

            if self.awg_site == 0 and svc.module_name.startswith("ao"):
                self.awg_site = site
            self.__mod_count += 1

# init _status so that values are valid even if this Acq400 doesn't run a shot ..
        _status = [int(x) for x in s0.state.split(" ")]
        if monitor:
            self.statmon = Statusmonitor(self.uut, _status)


    def __getattr__(self, name):
        if self.svc.get(name) != None:
            return self.svc.get(name)
        else:
            msg = "'{0}' object has no attribute '{1}'"
            raise AttributeError(msg.format(type(self).__name__, name))

    def state(self):
        return self.statmon.status[SF.STATE]
    def post_samples(self):
        return self.statmon.status[SF.POST]
    def pre_samples(self):
        return self.statmon.status[SF.PRE]
    def elapsed_samples(self):
        return self.statmon.status[SF.ELAPSED]
    def demux_status(self):
        return self.statmon.status[SF.DEMUX]
    def samples(self):
        return self.pre_samples() + self.post_samples()

    def read_chan(self, chan, nsam = 0):
        if nsam == 0:
            nsam = self.pre_samples()+self.post_samples()
        cc = Channelclient(self.uut, chan)
        data_size = 4 if self.s0.data32 == '1' else 2
        ccraw = cc.read(nsam, data_size = data_size)

        if data_size == 4:
            try:
                if self.s1.adc_18b == '1':
                    print("scale ccraw >> 14")
                    ccraw = ccraw >> 14
            except AttributeError:
                pass

        if self.save_data:
            try:
                os.makedirs(self.save_data)
            except OSError as exception:
                if exception.errno != errno.EEXIST:
                    raise

            with open("%s/%s_CH%02d" % (self.save_data, self.uut, chan), 'wb') as fid:
                ccraw.tofile(fid, '')

        return ccraw

    def nchan(self):
        return int(self.s0.NCHAN)

    def read_channels(self, channels=()):
        """read all channels post shot data.

        Returns:
            chx (list) of numpy arrays.
        """


        if channels == ():
            channels = range(1, self.nchan()+1)
        elif type(channels) == int:
            channels = (channels,)

    #      print("channels {}".format(channels))

        chx = []
        for ch in channels:
            if self.trace:
                print("%s CH%02d start.." % (self.uut, ch))
                start = timeit.default_timer()

            chx.append(self.read_chan(ch))

            if self.trace:
                tt = timeit.default_timer() - start
                print("%s CH%02d complete.. %.3f s %.2f MB/s" %
                      (self.uut, ch, tt, len(chx[-1])*2/1000000/tt))

        return chx

    # DEPRECATED
    def load_segments(self, segs):
        with netclient.Netclient(self.uut, AcqPorts.SEGSW) as nc:
            for seg in segs:
                nc.sock.send((seg+"\n").encode())
    # DEPRECATED
    def show_segments(self):
        with netclient.Netclient(self.uut, AcqPorts.SEGSR) as nc:
            while True:
                buf = nc.sock.recv(1024)
                if buf:
                    print(buf)
                else:
                    break

    def clear_counters(self):
        for s in self.svc:
            self.svc[s].sr('*RESET=1')

    def set_sync_routing_master(self, clk_dx="d1", trg_dx="d0"):
        self.s0.SIG_SYNC_OUT_CLK = "CLK"
        self.s0.SIG_SYNC_OUT_CLK_DX = clk_dx
        self.s0.SIG_SYNC_OUT_TRG = "TRG"
        self.s0.SIG_SYNC_OUT_TRG_DX = trg_dx

    def set_sync_routing_slave(self):
        self.set_sync_routing_master()
        self.s0.SIG_SRC_CLK_1 = "HDMI"
        self.s0.SIG_SRC_TRG_0 = "HDMI_TRG"

    def set_sync_routing(self, role):
        # deprecated
        # set sync mode on HDMI daisychain
        # valid roles: master or slave
        if role == "master":
            self.set_sync_routing_master()
        elif role == "slave":
            self.set_sync_routing_slave()
        else:
            raise ValueError("undefined role {}".format(role))

    def set_mb_clk(self, hz=4000000, src="zclk", fin=1000000):
        # valid ACQ1001
        if src == "zclk":
            self.s0.SIG_ZCLK_SRC = "INT33M"
            self.s0.SYS_CLK_FPMUX = "ZCLK"
            self.s0.SIG_CLK_MB_FIN = 33333000
        elif src == "xclk":
            self.s0.SYS_CLK_FPMUX = "XCLK"
            self.s0.SIG_CLK_MB_FIN = 32768000
        else:
            self.s0.SYS_CLK_FPMUX = "FPCLK"
            self.s0.SIG_CLK_MB_FIN = fin

        if hz >= 4000000:
            self.s0.SIG_CLK_MB_SET = hz
            return
        else:
            for clkdiv in range(1,2000):
                if hz*clkdiv >= 4000000:
                    self.s0.SIG_CLK_MB_SET = hz*clkdiv
                    self.s1.CLKDIV = clkdiv
                    return
            raise ValueError("frequency out of range {}".format(hz))

    def load_gpg(self, stl, trace = False):
        termex = re.compile("\n")
        with netclient.Netclient(self.uut, AcqPorts.GPGSTL) as nc:
            lines = stl.split("\n")
            for ll in lines:
                if trace:
                    print("> {}".format(ll))
                if len(ll) < 2:
                    if trace:
                        print("skip blank")
                    continue
                if ll.startswith('#'):
                    if trace:
                        print("skip comment")
                    continue
                nc.sock.send((ll+"\n").encode())
                rx = nc.sock.recv(4096)
                if trace:
                    print("< {}".format(rx))
            nc.sock.send("EOF\n".encode())
            nc.sock.shutdown(socket.SHUT_WR)
            rx = nc.sock.recv(4096)
            if trace:
                print("< {}".format(rx))

        with netclient.Netclient(self.uut, AcqPorts.GPGDUMP) as nc:
            while True:
                txt = nc.sock.recv(4096)
                if txt:
                    sys.stdout.write(txt)
                    if txt.find("EOF") != -1:
                        break
                else:
                    break


    class AwgBusyError(Exception):
        def __init__(self, value):
            self.value = value
        def __str__(self):
            return repr(self.value)

    def load_awg(self, data, autorearm=False):
        if self.awg_site > 0:
            if self.modules[self.awg_site].task_active == '1':
                raise self.AwgBusyError("awg busy")
        port = AcqPorts.AWG_AUTOREARM if autorearm else AcqPorts.AWG_ONCE

        with netclient.Netclient(self.uut, port) as nc:
            nc.sock.send(data)
            nc.sock.shutdown(socket.SHUT_WR)
            while True:
                rx = nc.sock.recv(128)
                if not rx or rx.startswith("DONE"):
                    break
            nc.sock.close()

    def run_service(self, port, eof="EOF", prompt='>'):
        txt = ""
        with netclient.Netclient(self.uut, port) as nc:
            while True:
                rx = nc.receive_message(self.NL, 256)
                txt += rx
                txt += "\n"
                print("{}{}".format(prompt, rx))
                if rx.startswith(eof):
                    break
            nc.sock.shutdown(socket.SHUT_RDWR)
            nc.sock.close()

        return txt

    def run_oneshot(self):
        with netclient.Netclient(self.uut, AcqPorts.ONESHOT) as nc:
            while True:
                rx = nc.receive_message(self.NL, 256)
                print("{}> {}".format(self.s0.HN, rx))
                if rx.startswith("SHOT_COMPLETE"):
                    break
            nc.sock.shutdown(socket.SHUT_RDWR)
            nc.sock.close()

    def run_livetop(self):
        with netclient.Netclient(self.uut, AcqPorts.LIVETOP) as nc:
            print(nc.receive_message(self.NL, 256))
            nc.sock.shutdown(socket.SHUT_RDWR)
            nc.sock.close()


class Acq2106(Acq400):
    def __init__(self, _uut, monitor=True, has_mgtdram=False):
        print("Acq2106 %s" % (_uut))
        Acq400.__init__(self, _uut, monitor)
        site = 13
        for sm in [ 'cA', 'cB']:
            self.svc[sm] = netclient.Siteclient(self.uut, AcqPorts.SITE0+site)
            self.mod_count += 1
            site = site - 1
        if (has_mgtdram):
            self.svc['s14'] = netclient.Siteclient(self.uut, AcqPorts.SITE0+14)

    def set_mb_clk(self, hz=4000000, src="zclk", fin=1000000):
        Acq400.set_mb_clk(self, hz, src, fin)
        self.s0.SYS_CLK_DIST_CLK_SRC = 'Si5326'
        self.s0.SYS_CLK_OE_CLK1_ZYNQ = '1'

    def set_sync_routing_slave(self):
        Acq400.set_sync_routing_slave(self)
        self.s0.SYS_CLK_OE_CLK1_ZYNQ = '1'

    def set_master_trg(self, trg, edge = "rising", enabled=True):
        if trg == "fp":
            self.s0.SIG_SRC_TRG_0 = "EXT" if enabled else "HOSTB"
        elif trg == "int":
            self.s0.SIG_SRC_TRG_1 = "STRIG"


    def run_mgt(self, _filter = null_filter):
        pm = ProcessMonitor(self.uut, _filter)
        while pm.quit_requested != True:
            time.sleep(1)






def run_unit_test():
    SERVER_ADDRESS = '10.12.132.22'
    if len(sys.argv) > 1:
        SERVER_ADDRESS = sys.argv[1]

    print("create Acq400 %s" %(SERVER_ADDRESS))
    uut = Acq400(SERVER_ADDRESS)
    print("MODEL %s" %(uut.s0.MODEL))
    print("SITELIST %s" %(uut.s0.SITELIST))
    print("MODEL %s" %(uut.s1.MODEL))

    print("Module count %d" % (uut.mod_count))
    print("POST SAMPLES %d" % uut.post_samples())

    for sx in sorted(uut.svc):
        print("SITE:%s MODEL:%s" % (sx, uut.svc[sx].sr("MODEL")) )


if __name__ == '__main__':
    run_unit_test()

