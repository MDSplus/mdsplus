def MdsDevices():
    from MDSplus import Device,tdi,StringArray,version
    ans = [[version.tobytes(d),b'pydevice'] for d in Device.findPyDevices()]
    try: import MitDevices;ans += [[k,'MitDevices'] for k,v in MitDevices.__dict__.items() if isinstance(v,int.__class__) and issubclass(v,Device)]
    except ImportError: pass
    try: import RfxDevices;ans += [[k,'RfxDevices'] for k,v in RfxDevices.__dict__.items() if isinstance(v,int.__class__) and issubclass(v,Device)]
    except ImportError: pass
    try: import W7xDevices;ans += [[k,'W7xDevices'] for k,v in W7xDevices.__dict__.items() if isinstance(v,int.__class__) and issubclass(v,Device)]
    except ImportError: pass
    devices = tdi("[if_error(MitDevices(),*),if_error(RfxDevices(),*),if_error(W7xDevices(),*)]").data()
    ans = dict(devices.reshape((int(devices.shape[0]/2),2)).tolist()+ans)
    keys = list(ans.keys());    keys.sort()
    return StringArray([(k,ans[k]) for k in keys])
