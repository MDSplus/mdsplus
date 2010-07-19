from MDSplus import *
from ctypes import *

class FEMTO(Device):
  parts=[{'path':':NAME','type':'text'},{'path':':COMMENT', 'type':'text'},
  {'path':':IP_ADDR', 'type':'text'},{'path':':COM_PORT', 'type':'text', 'value':'COM1'}]
    
  for i in range(1,66):
      parts.append({'path':'.CHANNEL_%02d'%(i),'type':'structure'})
      parts.append({'path':'.CHANNEL_%02d:GAIN'%(i),'type':'text', 'value':'0'})
      parts.append({'path':'.CHANNEL_%02d:ACDC'%(i),'type':'text', 'value':'0'})
      parts.append({'path':'.CHANNEL_%02d:HSLN'%(i),'type':'text', 'value':'0'})

  parts.append({'path':':INIT_ACTION','type':'action',
	'valueExpr':"Action(Dispatch('FEDE_SERVER','INIT',50,None),Method(None,'init',head))",
	'options':('no_write_shot',)})

  del i  


######################################INIT#################################################################
  def init(self,arg):
 #   from ctypes import CDLL, c_char, c_char_p, c_short, byref
    from array import array
    print "Init Femto Amplifier: reading data from traverser..."

    # Get Name
    try:
        name = self.name.data()
	print 'Name=',name
    except:
	Data.execute('DevLogErr($1,$2)', self.nid, 'WARNING: device with no name')
        #return 0
	      
    # Get IP Address
    try:
        ipAddr = self.ip_addr.data()
	print 'Remote Mode: IP address = ',ipAddr, ';note: empty for Local Mode.'
    except:
        ipAddr = ""
	Data.execute('DevLogErr($1,$2)', self.nid, 'WARNING: Local Mode. Put a valid IP address.')
	

    # Get ComPort
    try:
        comPort = self.com_port.data()
	print 'COM port = ',comPort
    except:
	Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: No COM port. Put COMx in traverser.')
	return 0

    femto = array('B')
    
    gainDict = {"10^3":0, "10^4":1, "10^5":2, "10^6":3, "10^7":4, "10^8":5, "10^9":6, "0":0}
    acdcDict = {"AC":0, "DC":8, "0":0}
    hslnDict = {"HS":0, "LN":16, "0":0}

    for i in range(1,66):
        try:
            gain = self.__getattr__('channel_%02d_gain'%(i)).data()
            #print 'gain=',gain
            gainVal = gainDict[gain]
            #print 'gainVal=',gainVal
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: Invalid gain setting in Femto %02d.'%(i))
            return 0

        try:
            acdc = self.__getattr__('channel_%02d_acdc'%(i)).data()
            #print 'acdc=',acdc
            acdcVal = acdcDict[acdc]
            #print 'acdcVal=',acdcVal
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: Invalid acdc setting in Femto %02d.'%(i))
            return 0

        try:
            hsln = self.__getattr__('channel_%02d_hsln'%(i)).data()
            #print 'hsln=',hsln
            hslnVal = hslnDict[hsln]
            #print 'hslnVal=',hslnVal
        except:
            Data.execute('DevLogErr($1,$2)', self.nid, 'ERROR: Invalid hsln setting in Femto %02d.'%(i))
            return 0
        
        try:
            conf=gainVal+acdcVal+hslnVal
            #print 'Femto %02d conf.='%(i),conf
            femto.append(conf)
        except:
            print "Error appending data to array."
            return 0
    del i

    print "Init Femto Amplifier: data reading completed."


    print "Init Femto Amplifier: writing data..."
    #LOCAL MODE
    if ipAddr=="":    
        try:
            deviceLib = CDLL("RS232Lib.dll")
        except:
            print 'ERROR: Cannot link to device library femto.dll'
            return 0
        try:
            femtostr=femto.tostring()
            #print 'femtostrtype=',type(femtostr),'val=',femtostr
        except:
            print 'Error converting array to string.'
            return 0
        try:     
            deviceLib.Femto232Write.argtypes = [c_char_p, c_char_p]
            deviceLib.Femto232Write(c_char_p(comPort), c_char_p(femtostr))
        except:
            print 'Error doing Femto232Write() in .dll'
            return 0

    #REMOTE MODE
    else:
        # Connect via MdsIP
        status = Data.execute('MdsConnect("'+ ipAddr +'")')
        if status == 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, "Cannot Connect to specified IP ADDRESS: ", ipAddr)
            return 0
        # init
        status = Data.execute('MdsValue("RS232Lib->Femto232Write($1,$2)",$1,$2)', comPort, Int8Array(femto))
        for i in range(0,65):
            print 'Femto %02d-> '%(i+1),'%02x'%(femto[i])
        del i
	if status == 0:
            Data.execute('MdsDisconnect()')
            Data.execute('DevLogErr($1,$2)', self.nid, "ERROR doing MdsValue(femto->Femto232Write(...))")
            return 0
        # disconnect
	Data.execute('MdsDisconnect()')


    print 'Init Femto Amplifier: end.'
    return 1

