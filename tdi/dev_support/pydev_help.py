def pydev_help(devtype_in,full_in):
  from MDSplus import Device,Data
  devtype=str(devtype_in).upper()
  devnames=[]
  full=int(full_in)
  if ('*' in devtype) or ('?' in devtype):
    devices = Device.findPyDevices()
    for device in devices:
      if (Data.execute('MdsShr->StrMatchWild(descr($),descr($))',(device,devtype)) & 1) == 1:
        devnames.append(device)
    return '\n'.join(devnames)
  else:
    try: 
	dmod = Device.importPyDeviceModule(devtype)
        cls = eval('dmod.%s' % dmod.__name__.upper())
        if full == 1:
          return help(cls)
        else:
          return cls.__doc__
    except:
      return "Error obtaining help on device " + devtype
