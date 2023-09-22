def setAbsoluteTime(devtype, devnum, node, delay):
   from ctypes import CDLL, c_int, byref, c_uint64
  
   tcnLib = None
   if tcnLib is None:
      tcnLib = CDLL("libtcn.so", use_errno=True)
   print('OK')

   niLib = None
   if niLib is None:
      niLib = CDLL("libnisync.so", use_errno=True)

   print('OK')

   status = tcnLib.tcn_init()

   print('OK', status)

   FD =  niLib.nisync_open_device(c_int(devtype), c_int(devnum))

   print('OK', FD)

   curr_nanos = c_uint64()

   status = niLib.nisync_get_time_ns(c_int(FD), byref(curr_nanos))

   print('OK', status)

   # SAVING THE TIME INFOMRATION IN THE PULSE
   node.putData(curr_nanos.value + delay*1000000000)

   return(1)

