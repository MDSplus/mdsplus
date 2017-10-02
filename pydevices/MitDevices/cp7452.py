# 
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from MDSplus import Device,Data,Action,Dispatch,Method

class CP7452(Device):
    """Adlink CP7452 DIO"""

    parts=[{'path':':NODE','type':'text','options':('noshot_write',)},
          {'path':':BOARD','type':'numeric','value':1,'options':('no_write_shot',)},
          {'path':':COMMENT','type':'text'},
          {'path':'.DIGITAL_OUTS','type':'structure'}]
    for i in range(4):
        parts.append({'path':'.DIGITAL_OUTS:DO%d'%(i,),'type':'numeric','value':0,'options':('no_write_shot',)})
    parts.append({'path':'.DIGITAL_INS','type':'structure'})
    for i in range(4):
        parts.append({'path':'.DIGITAL_INS:DI%d'%(i,),'type':'numeric','options':('no_write_model','write_once')})
    parts.append({'path':':INIT_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                 'options':('no_write_shot',)})
    parts.append({'path':':STORE_ACTION','type':'action',
                 'valueExpr':"Action(Dispatch('CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                 'options':('no_write_shot',)})
    del i


    def init(self):
        """Initialize digital outputs of CP7452 cpci board.
        Connects to the host and for each of the DIGITAL_OUTS nodes which are turned on, write the value to the digital output.
        """
        import os
        from MDSplus import Uint32
        debug=os.getenv("DEBUG_DEVICES")
        try:
            host=str(self.node.record.data())
        except:
            host='local'
        if Data.execute('mdsconnect($)',host) == 0:
            raise Exception,"Error connecting to host: "+host
        board=int(self.board.record)
        for i in range(4):
            do_nid=self.__getattr__('digital_outs_do%d'%(i,))
            if do_nid.on:
                try:
                    exp='MdsValue("_lun=fopen(\\\"/sys/module/cp7452_drv/parameters/format\\\",\\\"r+\\"); write(_lun,\\\"1\\\"); fclose(_lun);'
                    exp=exp+'_lun=fopen(\\\"/dev/cp7452.%d/DO%d\\\",\\\"r+\\\"); write(_lun,\\\"%x\\\"); fclose(_lun)")' % (board,i,int(Uint32(do_nid.record.data()).data()))
                    if debug:
                        print exp
                    Data.execute(exp)
                except Exception,e:
                    print "Error outputing to DO%d\n\t%s" % (i,str(e),)
        return 1
    INIT=init

    def store(self):
        """Stores the digital input values into the tree.
        Connects to the host and for each of the DIGITAL_INS nodes which are turned on, read the digital input and store the value in the node.
        """
        import os
        debug=os.getenv("DEBUG_DEVICES")
        try:
            host=str(self.node.record.data())
        except:
            host='local'
        if Data.execute('mdsconnect($)',host) == 0:
            raise Exception,"Error connecting to host: "+host
        board=int(self.board.record)
        for i in range(4):
            di_nid=self.__getattr__('digital_ins_di%d'%(i,1))
            if di_nid.on:
                try:
                    exp='MdsValue("_lun=fopen(\\\"/sys/module/cp7452_drv/parameters/format\\\",\\\"r+\\\"); write(_lun,\\\"1\\\"); fclose(_lun);'
                    exp=exp+'_lun=fopen(\\\"/dev/cp7452.%d/DI%d\\\",\\\"r\\\"); _ans=read(_lun); fclose(_lun),_ans")' % (board,i)
                    if debug:
                        print exp
                    value=eval('0x'+str(Data.execute(exp)))
                    di_nid.record=value
                except Exception,e:
                    print "Error inputting from DI%d\n\t%s" % (i,str(e),)
        return 1
    STORE=store
