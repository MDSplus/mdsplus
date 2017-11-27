def MdsDevices():
    from MDSplus import Device,tdi,StringArray,version
    ans = [[version.tobytes(d),b'pydevice'] for d in Device.findPyDevices()]
    devices = tdi("[if_error(MitDevices(),*),if_error(RfxDevices(),*),if_error(W7xDevices(),*)]").data()
    ans = dict(devices.reshape((int(devices.shape[0]/2),2)).tolist()+ans)
    keys = list(ans.keys());    keys.sort()
    return StringArray([(k,ans[k]) for k in keys])
