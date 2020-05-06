from threading import Lock

 # locks the cache
def MDSDEVICES(lock=Lock(), cache=[None]):
  with lock:
   if cache[0] is None:
    from MDSplus import Device,tdi,version,getenv
    from numpy import array
    def importDevices(name):
        bname = version.tobytes(name)
        try:
            module = __import__(name)
            ans = [[version.tobytes(k),bname] for k,v in module.__dict__.items() if isinstance(v,int.__class__) and issubclass(v,Device)]
        except (ImportError, ImportWarning): ans = []
        tdidev = tdi("if_error(%s(),*)"%name)
        if tdidev is None: return ans
        tdidev = [[k.rstrip(), v.rstrip()] for k,v in tdidev.value.reshape((int(tdidev.value.size/2),2)).tolist()]
        return tdidev+ans
    ans = [[version.tobytes(d),b'pydevice'] for d in Device.findPyDevices().keys()]
    mdsdevs=getenv('MDS_DEVICES')
    if mdsdevs is not None:
      modules=mdsdevs.split(':')
    else:
      modules=["KbsiDevices","MitDevices","RfxDevices","W7xDevices"]
    for module in modules:
        ans += importDevices(module)
    ans = array(list(dict(ans).items()))
    ans.view('%s,%s'%(ans.dtype,ans.dtype)).sort(order=['f0'], axis=0)
    cache[0] = ans
   return cache[0]
