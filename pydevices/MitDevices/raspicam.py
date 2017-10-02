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

import MDSplus

class RASPICAM(MDSplus.Device):
    """
    Device class to support raspberry pi cameras.

    methods:
      init
      store
    """

    parts = [
        {'path':':COMMENT','type':'text'},
        {'path':':COMPRESSED','type':'numeric','value':1,'options':('no_write_shot',)},
        {'path':':KEEP','type':'numeric','value':0,'options':('no_write_shot',)},
        {'path':':DIRECTORY','type':'text','value':'/usr/local/cmod/codes/raspicam/','options':('no_write_shot',)},
        {'path':':TRIGGER','type':'numeric','options':('no_write_shot',)},
        {'path':':WIDTH','type':'numeric','value':1920,'options':('no_write_shot',)},
        {'path':':HEIGHT','type':'numeric','value':1080,'options':('no_write_shot',)},
        {'path':':FPS','type':'numeric','value':30,'options':('no_write_shot',)},
        {'path':':EXPOSURE','type':'numeric','value':10000,'options':('no_write_shot',)},
        {'path':':BRIGHTNESS','type':'numeric','value':50,'options':('no_write_shot',)},
        {'path':':CONTRAST','type':'numeric','value':30,'options':('no_write_shot',)},
        {'path':':NUM_FRAMES','type':'numeric','value':60,'options':('no_write_model','write_once',)},
        {'path':':XTR_RVD','type':'text','value':'--ISO 800 --bitrate 25000000','options':('no_write_shot',)},
        {'path':':XTR_V4L','type':'text','value':' ','options':('no_write_shot',)},
        {'path':':TIMES','type':'axis', 'options':('write_once',)},
        {'path':':R_FRAMES','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':G_FRAMES','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':B_FRAMES','type':'numeric','options':('no_write_model','write_once',)},
        {'path':':FRAMES','type':'signal','options':('no_write_model','write_once',)},        
        {'path':':R_COEFF','type':'numeric','value':.299,'options':('no_write_shot',)},
        {'path':':G_COEFF','type':'numeric','value':.587,'options':('no_write_shot',)},
        {'path':':B_COEFF','type':'numeric','value':.114,'options':('no_write_shot',)},
        {'path':':INTENSITY','type':'signal','options':('write_once',)},
        {'path':':INIT_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
         'options':('no_write_shot',)},
        {'path':':STORE_ACTION','type':'action',
         'valueExpr':"Action(Dispatch('DATA_SERVER','STORE',50,None),Method(None,'STORE',head))",
         'options':('no_write_shot',)}]

    subproc = None

    def fileName(self):
        dir = str(self.directory.record)
        if dir[-1] == '/':
            dir = dir[:-1]
        if self.debugging:
            print "raspicam:  dir is %s"%dir
        return "%s/%s_%d_%s"%(dir,self.local_tree,self.tree.shot,self.local_path.replace('.', '_').replace(':', '_').replace('\\', '_'),)

    def init(self):
        """
        Init method for the raspberry pi camera device.

        Start by deleting any running subrocesses that may be left
        over from previous inits, note the subprocess is stored in a 
        class variable (ONLY one of these per server !)

        Read all of the settings and create a script to take the data.

        Note:  This device supports at most 1 camera per server.  Which is 
        OK since the raspberry pis only have one camera port.
        """

        import os
        import subprocess

        if RASPICAM.subproc :
            try:
                RASPICAM.subproc.terminate()
            except:
                pass

        compressed=int(self.compressed)
        num_frames=int(self.num_frames)
        exposure=int(self.exposure)
        brightness=int(self.brightness)
        contrast=int(self.contrast)
        width=int(self.width)
        height=int(self.height)
        fps=int(self.fps)
        extra_raspivid = str(self.xtr_rvd.record)
        extra_v4l2_ctl = str(self.xtr_v4l.record)

        self.debugging = os.getenv("DEBUG_DEVICES")
        cmds = []
        if compressed:
            cmds = [
                "sudo /usr/local/bin/trig.py\n", 
                "raspivid -w %d -h %d -fps %d -t %d -ss %d -br %d -co %d %s -o %s.h264\n" % (width, height, fps, int(float(num_frames)/fps*1000), exposure, brightness, contrast, extra_raspivid, self.fileName())]
            print cmds
        else:
            cmds = [
                "v4l2-ctl --set-fmt-video=width=%d,height=%d,pixelformat=2 --set-ctrl=exposure_time_absolute=%d,brightness=%d,contrast=%d,auto_exposure=1,white_balance_auto_preset=3\n"%(width, height, exposure, brightness, contrast,),
                "sudo usr/local/bin/trig.py\n", 
                "v4l2-ctl --stream-mmap=%d --stream-count=%d %s --stream-to=%s.rgb\n" % (num_frames, num_frames, extra_v4l2_ctl, self.fileName())]

	RASPICAM.subproc = subprocess.Popen(['/bin/sh'], stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE,shell=False)
        for cmd in cmds:
            if self.debugging:
                print cmd
            RASPICAM.subproc.stdin.write(cmd)
        RASPICAM.subproc.stdin.flush()
        return 1

    INIT=init


    def store(self):
        import os
        import numpy as np
        import MDSplus
        import imageio

        self.debugging = os.getenv("DEBUG_DEVICES")

        num_frames=int(self.num_frames)
        width=int(self.width)
        height=int(self.height)

        compressed=int(self.compressed)
        filename = "%s.%s"%(self.fileName(), ('h264' if compressed else 'rgb'), )
        if self.debugging:
            print "raspicam: reading %s"%filename
        self.times.record = MDSplus.Data.compile('$1 : $1+($2-1)/float($3) : 1./$3', self.trigger, self.num_frames, self.fps)
        self.intensity.record = MDSplus.Data.compile("MAKE_SIGNAL($*$+$*$+$*$,*,$)",self.r_frames, self.r_coeff, self.g_frames, self.g_coeff, self.b_frames, self.b_coeff, self.times)
        if not compressed:
            img = np.fromfile(filename, dtype=np.uint8)
            img=img.reshape(num_frames, height, width, 3)
            self.r_frames.record = img[:,:,:,0]
            self.g_frames.record = img[:,:,:,1]
            self.b_frames.record = img[:,:,:,2]
        else:
            ans = None
            count = 0
            vid = imageio.get_reader(filename, 'ffmpeg')
            meta = vid.get_meta_data()
            ans = np.empty((num_frames, height, width, 3),dtype=np.uint8)
            try:
                for i in range(num_frames):
                    im = vid.get_data(i)
                    ans[i,:,:,:] = im
            except Exception, e:
                print e
            if self.debugging:
                print "chop the answer to the number of frames"
            ans = ans[0:i-1,:,:,:]
            if self.debugging:
                print "shape is ", ans.shape
                print ans[:,:,:,0][0]
            self.r_frames.record=ans[:,:,:,0]
            if self.debugging:
                print "write g"
            self.g_frames.record=ans[:,:,:,1]
            if self.debugging:
                print "write b"
            self.b_frames.record=ans[:,:,:,2]
            if self.debugging:
                print "write frames record"
        self.frames.record = MDSplus.Data.compile("MAKE_SIGNAL(RASPI_RGB($,$,$), *, $)", self.r_frames, self.g_frames, self.b_frames, self.times)
        if self.keep.record == 0:
            os.remove(filename)
        return 1
    STORE=store

