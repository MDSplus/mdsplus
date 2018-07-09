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
import time
import sys
import os
from threading import Thread

CPCI_DATA_DIR=os.getenv("CPCI_DATA_DIR")

class storeIt(Thread):

	def __init__(self,info):
	  super(storeIt,self).__init__()
	  self.info=info
	  self.start()

	def run(self):
          import subprocess
          subprocess.call(self.info)

if len(sys.argv) == 1:
    import gamin

    def callback(path,event):
        try:
            if event == gamin.GAMChanged:
                time.sleep(1)
                f=open(CPCI_DATA_DIR+"/triggers/"+path,'r')
                info=f.read().split()
                f.close()
                if len(info) != 3:
                    return
                tree=info[0]
                shot=info[1]
                node=info[2]
                if os.getenv(tree.lower()+'_path') is None:
                    return
                try:
                    ishot=int(shot)
                except:
                    return
                if node[0] == "\\":
                    node="\\"+node
                storeIt((sys.argv[0],path,tree,shot,node))
        except Exception,e:
            print e

    if CPCI_DATA_DIR is None:
        print "CPCI_DATA_DIR environment variables required. The monitor will monitor the $CPCI_DATA_DIR/triggers directory"
        sys.exit(1)
    mon = gamin.WatchMonitor()
    mon.watch_directory("%s/triggers" % (CPCI_DATA_DIR,),callback)
    time.sleep(1)
    while mon.handle_one_event():
        pass
else:
    from MDSplus import Tree
    if len(sys.argv) == 5:
        trigger_file=sys.argv[1]
        tree=sys.argv[2].upper()
        if os.getenv(tree.lower()+'_path') is not None:
            try:
                shot=int(sys.argv[3])
                node=sys.argv[4].upper()
                print "%s, Doing %s (STOREFTP) in %s shot %d" % (time.asctime(),node,tree,shot)
                try:
                    Tree(tree,shot).getNode(node).doMethod('storeftp','')
                    print "%s, Done %s (STOREFTP) in %s shot %d" % (time.asctime(),node,tree,shot)
                    if os.getenv("DELETE_TRIGGER_FILES","NO").lower() == "yes":
                        os.remove(CPCI_DATA_DIR+"/triggers/"+trigger_file)
                    if os.getenv("DELETE_CPCI_DATA","NO").lower() == "yes":
                        os.system("rm -Rf %s/%s/%d/%s" % (CPCI_DATA_DIR,tree,shot,node))
                        os.remove("%s/%s_%d_%s.sh" % (CPCI_DATA_DIR,tree,shot,node))
                except Exception,e:
                    print "%s, Done %s (STOREFTP) in %s shot %d - FAILED: %s" % (time.asctime(),node,tree,shot,e)
            except:
                pass
