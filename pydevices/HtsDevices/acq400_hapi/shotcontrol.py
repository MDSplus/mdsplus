import signal
import sys
import threading
import time

def intSI(x):
    x = str(x)
    units = x.find('M')
    if units >= 0:
        return int(x[0:units])*1000000
    else:
        units = x.find('k')
        if units >= 0:
            return int(x[0:units])*1000
        else:
            return int(x)


def wait_for_state(uut, state, timeout=0):
    UUTS = [uut]
    time0 = 0
    if time0 == 0:
        time0 = time.time()
    for uut in UUTS:
        olds = ""
        finished = False
        dots = 0
        pollcat = 0

        while not finished:
            st = uut.s0.TRANS_ACT_STATE.split(' ')[1] #Real name TRANS_ACT:STATE
            finished = st == state
            news = "polling {}:{} {} waiting for {}".format(uut.uut, st, 'DONE' if finished else '', state)
            if news != olds:
                sys.stdout.write("\n{:06.2f}: {}".format(time.time() - time0, news))
                olds = news
            else:
                sys.stdout.write('.')
                dots += 1
                if dots >= 20:
                    dots = 0
                    olds = ""
            if not finished:
                if timeout and (time.time() - time0) > timeout:
                    sys.exit("\ntimeout waiting for {}".format(news))
                time.sleep(1)
            pollcat += 1
    print("")


class ActionScript:
    def __init__(self, script_and_args):
        self.sas = script_and_args.split()
        print("ActionScript creates {}".format(self.sas))
    def __call__(self):
        print("ActionScript: call()")
        call(self.sas)



class ShotController:
    """ShotController handles shot synchronization for a set of uuts
    """
    def prep_shot(self):
        for u in self.uuts:
            u.statmon.stopped.clear()
            u.statmon.armed.clear()

        self.tp = [ threading.Thread(target=u.statmon.wait_stopped) for u in self.uuts]

        for t in self.tp:
            t.setDaemon(True)
            t.start()

        self.ta = [threading.Thread(target=u.statmon.wait_armed) for u in self.uuts]

        for t in self.ta:
            t.setDaemon(True)
            t.start()

    def wait_armed(self):
        for t in self.ta:
            t.join()

    def wait_complete(self):
        for t in self.tp:
            t.join()

    def arm_shot(self):
        for u in self.uuts:
            print("%s set_arm" % (u.uut))
            u.s0.set_arm = 1
        self.wait_armed()

    def abort_shot(self):
        for u in self.uuts:
            print("%s set_abort" % (u.uut))
            u.s0.set_abort = 1

    def on_shot_complete(self):
        """runs on completion, expect subclass override."""
        for u in self.uuts:
            print("%s SHOT COMPLETE shot:%s" % (u.uut, u.s1.shot))

    def run_shot(self, soft_trigger=False, acq1014_ext_trigger=0,
                remote_trigger=None):
        """run_shot() control an entire shot from client.

           for more control, use the individual methods above.

           Args:
               soft_trigger=False (bool) : trigger when armed

        """
        if acq1014_ext_trigger:
            # block external triggers with temp switch to soft trigger
            self.uuts[0].s2.acq1014_trg = 1
        self.prep_shot()
        self.arm_shot()
        if soft_trigger:
            if soft_trigger < 0:
                print("hit return for soft_trigger")
                sys.stdin.readline()
            else:
                while soft_trigger > 1:
                    print("sleep {}".format(soft_trigger))
                    time.sleep(1)
                    soft_trigger = soft_trigger - 1

            print("%s soft_trigger" % (self.uuts[0].uut))
            self.uuts[0].s0.soft_trigger = 1
        elif remote_trigger != None:
            remote_trigger()

        if acq1014_ext_trigger > 0:
            time.sleep(acq1014_ext_trigger)
            self.uuts[0].s2.acq1014_trg = 0

        self.wait_complete()
        self.on_shot_complete()

    def map_channels(self, channels):
        cmap = {}
        #print("map_channels {}".format(channels))
        ii = 0
        for u in self.uuts:
            if channels == ():
                cmap[u] = list(range(1, u.nchan()+1))  # default : ALL
            elif type(channels) == int:
                cmap[u] = channels                  # single value
            elif type(channels[0]) != tuple:
                cmap[u] = channels                  # same tuple all UUTS
            else:
                try:
                    cmap[u] = channels[ii]          # dedicated tuple
                except:
                    cmap[u] = 1                     # fallback, ch1

            ii = ii + 1
        return cmap

    def read_channels(self, channels=()):
        cmap = self.map_channels(channels)
        chx = [u.read_channels(cmap[u]) for u in self.uuts]

        if self.uuts[0].save_data:
            with open("%s/format" % (self.uuts[0].save_data), 'w') as fid:
                for u in self.uuts:
                    for ch in cmap[u]:
                        fid.write("%s_CH%02d RAW %s 1\n" % (u.uut, ch, 's'))


        return (chx, len(self.uuts), len(chx[0]), len(chx[0][0]))

    def __init__(self, _uuts, shot=None):
        self.uuts = _uuts
        if shot != None:
            for u in self.uuts:
                u.s1.shot = shot
