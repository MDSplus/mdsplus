#!/usr/bin/python
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
