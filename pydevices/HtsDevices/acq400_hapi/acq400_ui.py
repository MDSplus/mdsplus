
import argparse
from . import acq400
from .shotcontrol import intSI as intSI

class Acq400UI:
    """ Common UI features for consistent args handling across all apps
    """
    @staticmethod
    def _exec_args_trg(uut, args, trg):
        print("exec_args_trg {}".format(trg))
        if trg == 'notouch':
            return
        (typ, edge) = ('int', 'rising')
        try:
            (typ, edge) = trg.split(',')
        except:
            typ = trg

        triplet = "1,%d,%d" % (0 if typ == 'ext' else 1, 0 if edge == 'falling' else 1)
        print("triplet={}".format(triplet))
        if args.pre != 0:
            uut.s1.trg = "1,1,1"
            uut.s1.event0 = triplet
        else:
            uut.s1.trg = triplet
            uut.s1.event0 = "0,0,0"

    @staticmethod
    def _exec_args_clk(uut, clk):
        c_args = clk.split(',')
        src = c_args[0]
        _fin=1000000
        _hz=1000000


        if len(c_args) > 1:
            _hz = intSI(c_args[1])
            if len(c_args) > 2:
                _fin = intSI(c_args[2])

        if src == 'ext' or src == 'fpclk':
            uut.set_mb_clk(hz=_hz, src="fpclk", fin=_fin)
        elif src == 'int' or src == 'zclk':
            uut.set_mb_clk(hz=_hz, src="zclk", fin=_fin)
        elif src == 'xclk':
            uut.set_mb_clk(hz=_hz, src="xclk", fin=_fin)

    @staticmethod
    def _exec_args_sim(uut, sim):
        sim_sites = [ int(s) for s in sim.split(',')]
        for site in uut.modules:
            sim1 = '1' if site in sim_sites else '0'
            uut.svc['s%s' % (site)].simulate = sim1
        #            print "site {} sim {}".format(site, sim1)


    @staticmethod
    def _exec_args_trace(uut, trace):
        if trace is not None:
            for svn, svc in sorted(uut.svc.items()):
                svc.trace = trace

    @staticmethod
    def _exec_args_transient(uut, args):
        pre = intSI(args.pre)
        post = intSI(args.post)
        uut.configure_transient(pre=pre, post=post, \
            auto_soft_trigger=(1 if pre>0 else 0), demux=args.demux)


    @staticmethod
    def add_args(parser, transient=False, post=True, pre=True, demux=1):
        """ generate standard args list

        Args:
             post: set False to disable createing the arg, becomes client app resposibility

        """
        if transient:
            if pre:
                parser.add_argument('--pre', default=0, type=int, help='pre-trigger samples')
            if post:
                parser.add_argument('--post', default=100000, type=int, help='post-trigger samples')
            if demux > -1:
                parser.add_argument('--demux', default='{}'.format(demux), type=int, help='embedded demux')

        parser.add_argument('--clk', default=None, help='int|ext|zclk|xclk,fpclk,SR,[FIN]')
        parser.add_argument('--trg', default=None, help='int|ext,rising|falling')
        parser.add_argument('--sim', default=None, help='s1[,s2,s3..] list of sites to run in simulate mode')
        parser.add_argument('--trace', default=None, help='1 : enable command tracing')

    @staticmethod
    def exec_args(uut, args):
        """ and execute all the args
        """
        print("exec_args" )
        if args.trg:
            Acq400UI._exec_args_trg(uut, args, args.trg)
        if args.clk:
            Acq400UI._exec_args_clk(uut, args.clk)
        if args.sim:
            Acq400UI._exec_args_sim(uut, args.sim)
        if args.trace:
            Acq400UI._exec_args_trace(uut, args.trace)
        if args.post != None:
            Acq400UI._exec_args_transient(uut, args)
