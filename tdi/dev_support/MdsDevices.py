from MDSplus import Data,StringArray
from numpy import array

def MdsDevices(show_dups=False):
    devices=Data.execute("""
    [pydevices(),
    if_error(MitDevices(),*),
    if_error(RfxDevices(),*),
    if_error(W7xDevices(),*)]""")
    skip=False
    dup=False
    ans=list()
    ds=list()
    for i in range(len(devices)/2):
        if str(devices[i*2]).upper() not in ds:
            ans.append(str(devices[i*2]))
            ans.append(str(devices[i*2+1]))
            ds.append(str(devices[i*2]).upper())
        else:
            if show_dups:
                print("Duplicate: %s %s"%(str(devices[i*2]),str(devices[i*2+1])))
    return StringArray(ans)
