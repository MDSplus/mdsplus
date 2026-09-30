#
# TSSLICE_LABEL - Find the channel with the specified label, 
#              and return the appropriate TDI expression
#

import MDSplus
import os
import numpy as np
import re

def tsslice_label(sig,shot,label):
    '''
    Find the channel with the specified label, and return 
    the appropriate TDI expression
    
    :param sig:  'TSNE' or  'TSTE' (i.e. electron density or temperature)
    :param shot: 5 or 6 digit d3d shot number
    :param label: channel label/ptname (c26b)
    
    :return: String expression yielding channel with the specified label or 
             empty string if no channel found
    '''

    # For Testing as a python script, but commented out for production use as a MDSplus "TDI" function...
    #os.environ["electrons_path"] = "atlas.gat.com::"
    #os.environ["tscal_path"] = "atlas.gat.com::"
    
    #the ptnames are lower case, make the label lower case to compare
    if not label.islower():
        label = label.lower()
    label = label.strip()

    if label.upper()[0] == 'C':
        sys_short = 'CORE'
        sys = 'CORE'
    elif label.upper()[0] == 'D':
        sys_short = 'DIV'
        sys = 'DIVERTOR'
    elif label.upper()[0] == 'T' or label.upper()[0] == 'H':
        sys_short = 'TAN'
        sys = 'TANGENTIAL'
    else:
        print("not valid pointname: ", label)
        return

    # Init
    tree = MDSplus.Tree('electrons',shot)
    expr = ''
     
    try:
            ts_ptname = tree.getNode('TS.BLESSED.{0}.PTNAME'.format(sys)).data().tolist()
            ts_ptname = [x.decode("latin-1") for x in ts_ptname]
    except :
            ts_ptname = []

    # pt_name not exist in electrons tree, try the tscal tree
    if len(ts_ptname) == 0:
        calibID = tree.getNode('TS.BLESSED.HEADER.CALIB_NUMS').data()[0]
        tscal_tree = MDSplus.Tree('tscal',calibID)
        try:
            ts_ptname = tscal_tree.getNode('{0}.PTNAME'.format(sys)).data().tolist()
            ts_ptname = [x.decode("latin-1") for x in ts_ptname]
        except:
            ts_ptname = []
    
    # Some shots need whitespaces stripped, others need a \x00 cut
    ts_ptname = list(map(str.strip,ts_ptname))
    for x in range(len(ts_ptname)):
    	ts_ptname[x] = ts_ptname[x].split('\x00',1)[0]

    if len(ts_ptname) != 0:
        # the new ptname format should start with 'h' for tangential system
        if label.lower()[0] == 't':
            #label is not python string, label.lower() is python string
            label = MDSplus.String("h" + label.lower()[1:])
        try:
            idx = ts_ptname.index(label)
            expr='TSSLICE2("\\{sig}_{sys_short}",{idx},"{sys_short}")'.format(**locals())
        except ValueError:
            pass

    #old label, no ptname; c00, d00, t00
    if len(ts_ptname) == 0 and len(label) == 3:
        try:
            sIdx = int(label.upper()[-2:])
        except:
            sIdx = -1
        if sIdx >= 0:
            expr='TSSLICE2("\\{sig}_{sys_short}",{sIdx},"{sys_short}")'.format(**locals())

    if expr:
        print('channel with label ', label, ':', expr)
    else:
        print('No point found for label ', label)
    return tree.tdiExecute(expr)

# For Testing as a python script, but commented out for production use as a MDSplus "TDI" function...
#if __name__=='__main__':
#    os.environ["electrons_path"] = "atlas.gat.com::"
#    os.environ["tscal_path"] = "atlas.gat.com::"
#    assert tsslice_label(sig='TSTE', shot=187325, label='c17b') == 'TSSLICE2("\\TSTE_CORE",36,"CORE")'
