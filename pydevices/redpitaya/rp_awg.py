#!/usr/bin/env python
"""RedPitaya-Arbitrary-Waveform-Generator for default image using scpi.

Depends on rp.pr_scpi.
"""

import os
import sys
import socket
import time

import numpy
import MDSplus
import rp


@MDSplus.with_mdsrecords
class RP_AWG(MDSplus.Device):
    """RedPitaya-Arbitrary-Waveform-Generator for default image using scpi."""

    parts = [
        {'path': ':ACTIONSERVER',
         'options': ('no_write_shot', 'write_once'),
         'type': 'TEXT',
         },
        {'path': ':ACTIONSERVER:INIT',
         'options': ('no_write_shot', 'write_once'),
         'type': 'ACTION',
         'valueExpr': ('Action('
                       'Dispatch(head.ACTIONSERVER, "INIT", 51),'
                       'Method(None, "init", head))'),
         },
        {'path': ':ACTIONSERVER:DEINIT',
         'options': ('no_write_shot', 'write_once'),
         'type': 'ACTION',
         'valueExpr': ('Action('
                       'Dispatch(head.ACTIONSERVER, "DEINIT", 51),'
                       'Method(None, "deinit", head))'),
         },
        {'path': ':HOST',
         'options': ('no_write_shot',),
         'type': 'TEXT',
         'filter': str,
         },
        {'path': ':INFO',
         'options': ('no_write_shot',),
         'type': 'TEXT',
         'filter': str,
         },
    ]
    for ch in (1, 2):
        parts.extend([
            {'path': ':OUT%d' % ch,
             'options': ('no_write_shot',),
             'type': 'TEXT',
             'help': 'python (numpy) expr of secondary output',
             'filter': str,
             'default': 'linspace(-1, 1, N),-1'
             },
            {'path': ':OUT%d:PERIOD' % ch,
             'options': ('no_write_shot',),
             'type': 'NUMERIC',
             'help': 'Period of one cycle in s - default 1e-3',
             'filter': float,
             'default': 1e-3,
             },
            {'path': ':OUT%d:WAIT' % ch,
             'options': ('no_write_shot',),
             'type': 'NUMERIC',
             'help': ('Period between two cycles in s - default 0\n'
                      'Should be such that 0.5 % period+wait == 0'),
             'filter': float,
             'default': 0,
             },
            {'path': ':OUT%d.CAL' % ch},
            {'path': ':OUT%d.CAL:VERSION' % ch,
             'options': ('no_write_shot',),
             'type': 'TEXT',
             'filter': str,
             },
            {'path': ':OUT%d.CAL:IN' % ch,
             'options': ('no_write_shot',),
             'help': 'Calibration data as programmed for OUT%d' % ch,
             'type': 'NUMERIC',
             'default': None,
             },
            {'path': ':OUT%d.CAL:OUT' % ch,
             'options': ('no_write_shot',),
             'help': 'Calibration data as measured in response to -:IN',
             'type': 'NUMERIC',
             'default': None,
             },
        ])
    del(ch)

    def get_output(self, n, suffix):
        """Return node of output n or member."""
        name = "out%d" % n
        if suffix:
            name = '_'.join((name, suffix))
        return getattr(self, name)

    def get_output_val(self, n, suffix=None):
        """Return value of output n or member."""
        name = "_out%d" % n
        if suffix:
            name = '_'.join((name, suffix))
        return getattr(self, name)

    def scpi(self):
        """Return scpi inteface."""
        scpi = rp.rp_scpi(host=self._host, timeout=5)
        scpi.debug = self.debug
        return scpi

    def _get_band_conf(self, ch):
        return (
            self.get_output_val(ch),
            self.get_output_val(ch, 'period'),
            self.get_output_val(ch, 'wait'),
            self.get_output_val(ch, 'cal_in'),
            self.get_output_val(ch, 'cal_out'),
        )

    def _update(self, node, new):
        new = MDSplus.Float32(new)
        rec = node.getRecord(None)
        if isinstance(rec, (MDSplus.BUILD_PARAM,)):
            rec[0] = new
        else:
            rec = MDSplus.BUILD_PARAM(new, None, rec)
        node.write_once = node.no_write_shot = False
        try:
            node.record = rec
        finally:
            node.write_once = node.no_write_shot = True

    def _fill_wait(self, volt, N, B):
        return numpy.concatenate((volt, numpy.linspace(
            volt[-1], volt[0], B-N)))

    def _compile_wfm(self, ch, wfm, N):
        """Return float array of output voltages [-1..1] and final length."""
        try:
            X = eval(wfm, numpy.__dict__, {'N': N})
        except Exception as e:
            raise MDSplus.DevBAD_PARAMETER(
                "OUT%d: Could not compile waveform: %s" % (ch, e))
        if isinstance(X, tuple):
            try:
                X = numpy.concatenate(
                    tuple(numpy.array(x).flatten() for x in X))
            except Exception as e:
                raise MDSplus.DevBAD_PARAMETER(
                    "OUT%d: Could not join waveform parts: %s" % (ch, e))
        return X, len(X)

    def _calculate_FBN(self, ch, per, wait, bufsize, minbufsize=0x400):
        """Return freq, nominal and total buffer size to be used."""
        total = (per + wait)
        F = int(numpy.round(.5/total))
        if F <= 0:
            raise MDSplus.DevBAD_PARAMETER("OUT%d: period + wait > 0.5" % ch)
        B = bufsize >> 1
        N = int(numpy.round(B*per/total))
        while F & 1 == 0 and B > minbufsize and N & 1 == 0:
            F, B, N = F >> 1, B >> 1, N >> 1
        self.dprint(1, "OUT%d: F = %d, B = %d, N = %d", ch, F, B, N)
        return F, B, N

    def _update_inputs(self, ch, per, wait, bufsize, F, B, N):
        """Calculate actual period and wait values and update nodes."""
        D = bufsize//B
        buffer_per_s = F*D
        s_per_sample = 1./buffer_per_s/B
        per_o = N * s_per_sample
        wait_o = (B-N) * s_per_sample
        self.dprint(1, "OUT%d: D = %d, period = %g, wait = %g",
                    ch, D, per_o, wait_o)
        self._update(self.get_output(ch, 'period'), per_o)
        self._update(self.get_output(ch, 'wait'), wait_o)

    def _transform_volt(self, x, cin, cout):
        """Apply calibration correction on x using cin and cout."""
        return numpy.interp(x, cout, cin, left=cin[0], right=cin[-1])

    def _init_output(self, scpi, ch, bufsize):
        """Initialize output channel."""
        wfm, per, wait, cin, cout = self._get_band_conf(ch)
        F, B, N = self._calculate_FBN(ch, per, wait, bufsize)
        self._update_inputs(ch, per, wait, bufsize, F, B, N)
        x, n = self._compile_wfm(ch, wfm, N)
        if n < B:
            x = self._fill_wait(x, n, B)[:B]
        if cin is None or cout is None:
            volt = x
        else:
            volt = self._transform_volt(x, cin, cout)
        scpi.GenWaveform(ch, rp.FUNC.ARBITRARY)
        scpi.GenFreq(ch, F)
        scpi.GenAmp(ch, 1)
        scpi.GenOffset(ch, 0)
        scpi.GenMode(ch, rp.GENMODE.CONTINUOUS)
        scpi.GenArbWaveform(ch, volt)
        scpi.wait()

    def init(self):
        """Initialize device - configure and enable outputs."""
        are_enabled = self.out1.on, self.out2.on
        scpi = self.scpi()
        bufsize = scpi.AcqGetBufSize()
        try:
            self.dprint(1, scpi.IDN)
            scpi.GenReset()
            scpi.wait()
            for ch, enabled in enumerate(are_enabled, 1):
                if enabled:
                    self._init_output(scpi, ch, bufsize)
            scpi.DpinSetState(rp.DPIN.LED0, False)
            for ch, enabled in enumerate(are_enabled, 1):
                if enabled:
                    scpi.GenOutEnable(ch)
            for led, on in zip((rp.DPIN.LED1, rp.DPIN.LED2), are_enabled):
                scpi.DpinSetState(led, on)
        except socket.timeout:
            raise MDSplus.DevCOMM_ERROR("Timeout")
        finally:
            scpi.close()

    def deinit(self):
        """Deinitialize device - disable outputs."""
        scpi = self.scpi()
        try:
            scpi.GenReset()
            scpi.DpinSetState(rp.DPIN.LED1, False)
            scpi.DpinSetState(rp.DPIN.LED2, False)
            scpi.wait()
            scpi.DpinSetState(rp.DPIN.LED0, True)
        finally:
            scpi.close()


def main(args=None):
    """Run some tests."""
    if args is None:
        args = sys.argv[1:]
    srcdir = os.path.realpath(os.path.dirname(__file__))
    sys.path.insert(0, srcdir)
    try:
        MDSplus.setenv('MDS_PYDEVICE_PATH', srcdir)
        MDSplus.setenv('test_path', '/tmp')
        with MDSplus.Tree('test', -1, 'NEW') as t:
            d = t.addDevice('RP_AWG', 'RP_AWG')
            t.write()
        t.open()
        # this should produce a sine wave
        d.out1 = 'linspace(-1, 1, N//2),linspace(1, -1, N-N//2)'
        d.out1_period = 1e-3
        d.out1_wait = 3e-3
        x = d.out1_cal_out = numpy.linspace(-1, 1, 255)
        d.out1_cal_in = numpy.sin(x * numpy.pi/2)
        d.host.record = args[0] if args else 'RP-F0432C'
        t.createPulse(1)
        t.open(shot=1)
        d.debug = 1
        d.init()
        time.sleep(1)
        d.deinit()
        _ = d.out1_period.record
        assert abs(_.data() - _.validation) < 1e-9
        _ = d.out1_wait.record
        assert abs(_.data() - _.validation) < 1e-9
        _ = d.out2_period.record
        assert abs(_.data() - 1e-3) < 1e-9
        _ = d.out2_wait.record
        assert abs(_.data() - 0.) < 1e-9
    finally:
        del(sys.path[0])


if __name__ == '__main__':
    sys.exit(main())
