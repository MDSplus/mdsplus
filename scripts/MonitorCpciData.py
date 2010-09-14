#!/usr/bin/python
import time
import sys
import os

CPCI_DATA_DIR=os.getenv("CPCI_DATA_DIR")


if len(sys.argv) == 1:
    import gamin
    import subprocess

    def callback(path,event):
        try:
            if event == gamin.GAMCreated or event == gamin.GAMChanged:
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
                print node
                if node[0] == "\\":
                    node="\\"+node
                subprocess.Popen((sys.argv[0],path,tree,shot,node))
        except:
            raise

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
                except Exception,e:
                    print "%s, Done %s (STOREFTP) in %s shot %d - FAILED: %s" % (time.asctime(),node,tree,shot,e)
                if os.getenv("DELETE_TRIGGER_FILES") is not None:
                    os.remove(CPCI_DATA_DIR+"/triggers/"+trigger_file)
                if os.getenv("DELETE_CPCI_DATA") is not None:
                    os.system("rm -Rf %s/%s/%d/%s" % (CPCI_DATA_DIR,tree.lower(),shot,node.lower()))
            except:
                pass

