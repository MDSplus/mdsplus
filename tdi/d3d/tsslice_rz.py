#
# TSSLICE_RZ - Find the channel closest to r,z within the tolerance distance, 
#              and return the appropriate TDI expression
#

import MDSplus
import os
import numpy as np
import sys

def tsslice_rz(sig,shot,r,z,tolerance=0.005):
    '''
    Find the channel closest to r,z within the tolerance distance, and return 
    the appropriate TDI expression
    
    :param sig:  'TSNE' or  'TSTE' (i.e. electron density or temperature)
    :param shot: 5 or 6 digit d3d shot number
    :param r: desired r position of TS channel in meters
    :param z: desired z position of TS channel in meters
    :param tolerance:  distance to closest channel must be within tolerance distance
    
    :return: String expression yielding channel closests to r,z or 
             empty string if no channel close enough to r,z
    '''

    # Init
    tree = MDSplus.Tree('electrons',shot)
    min_distance = tolerance + 10 
    expr = ''
     
    # Loop over systems
    for sys,sys_short in [('CORE', 'CORE'), ('DIVERTOR', 'DIV'), ('TANGENTIAL', 'TAN')]:
        try:
            tsz = MDSplus.Data(tree.getNode('TS.BLESSED.{0}.Z'.format(sys)).data())
            tsr = MDSplus.Data(tree.getNode('TS.BLESSED.{0}.R'.format(sys)).data())
            distance = MDSplus.Data(np.sqrt((tsr - r)**2 + (tsz - z)**2))
            sys_min_distance = min(distance)
            if (sys_min_distance < min_distance) and (sys_min_distance <= tolerance):
                min_distance = sys_min_distance
                idx = np.argmin(distance)
                expr='TSSLICE2("\\{sig}_{sys_short}",{idx},"{sys_short}")'.format(**locals())
        except:
            pass            
    if expr:
        print('Closest point has min_distance=', min_distance)
        print(expr)
    else:
        print('No point found within tolerance')
    return tree.tdiExecute(expr)

# For Testing as a python script, but commented out for production use as a MDSplus "TDI" function...
#if __name__=='__main__':
#    os.environ["electrons_path"] = "atlas.gat.com::"
#    assert tsslice_rz(sig='TSTE', shot=187001, r=1.94, z=0.363, tolerance=0.005)=='TSSLICE2("\TSTE_CORE",36,"CORE")'
#    assert tsslice_rz(sig='TSTE', shot=187001, r=2, z=-2, tolerance=0.1)==''
#    assert tsslice_rz(sig='TSNE', shot=187001, r=1.49, z=-1.2414, tolerance=0.005)=='TSSLICE2("\TSNE_DIV",15,"DIV")'
