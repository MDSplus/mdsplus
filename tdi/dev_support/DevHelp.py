from MDSplus import Device,Data,TreeNode

_device_list = None

def _devHelpDevtype(devtype, full):
  from pydoc import TextDoc
  global _device_list
  if _device_list is None:
    alldevices=Data.execute('MDSDEVICES()')
    _device_list=[item[0].strip() for item in alldevices]
  if ('*' in devtype) or ('?' in devtype):
    devnames=[]
    for device in _device_list:
      if (Data.execute('MdsShr->StrMatchWild(descr($),descr($))',(device.upper(),devtype.upper())) & 1) == 1:
        devnames.append(DevHelp(device,-1))
    return '\n'.join(devnames)
  else:
    try: 
        cls = Device.PyDevice(devtype)
        if full == 1:
          return TextDoc().docclass(cls)
        elif full == -1:
          return "%s: python device" % devtype
        else:
          return cls.__doc__
    except:
      for device in _device_list:
        if device.upper() == devtype.upper():
          return "%s: tdi, java or shared library device" % device
      return "Error obtaining help on device " + devtype

def _devHelpNode(node,full):
  try:
    elt = int(node.conglomerate_elt)
    if elt == 0:
      return ""
    if elt == 1:
      return DevHelp(node.record.model,full)
    else:
      cls = node.head.record.getDevice()
      return cls.parts[elt-2].get('help',"")
  except Exception as e:
    return "ERROR: %s"%(str(e),)

def DevHelp(dev,full=0):
  if isinstance(dev,TreeNode):
    return _devHelpNode(dev,int(full))
  else:
    return _devHelpDevtype(str(dev),int(full))
    
