from MDSplus import Device,Data

class CP7452(Device):
    """Adlink CP7452 DIO"""

    part_names=(':NODE',':BOARD',':COMMENT','.DIGITAL_OUTS',
               '.DIGITAL_OUTS:DO0','.DIGITAL_OUTS:DO1','.DIGITAL_OUTS:DO2','.DIGITAL_OUTS:DO3',
               '.DIGITAL_INS',
               '.DIGITAL_INS:DI0','.DIGITAL_INS:DI1','.DIGITAL_INS:DI2','.DIGITAL_INS:DI3',
               ':INIT_ACTION',':STORE_ACTION')

    def init(self,arg):
        """Initialize digital outputs of CP7452 cpci board.
        Connects to the host and for each of the DIGITAL_OUTS nodes which are turned on, write the value to the digital output.
        """
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
                    exp=exp+'_lun=fopen(\\\"/dev/cp7452.%d/DO%d\\\",\\\"r+\\\"); write(_lun,\\\"%x\\\"); fclose(_lun)")' % (board,i,int(do_nid.record))
                    Data.execute(exp)
                except Exception,e:
                    print "Error outputing to DO%d\n\t%s" % (i,str(e),)
        return 1

    def store(self,arg):
        """Stores the digital input values into the tree.
        Connects to the host and for each of the DIGITAL_INS nodes which are turned on, read the digital input and store the value in the node.
        """
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
                    value=eval('0x'+str(Data.execute(exp)))
                    di_nid.record=value
                except Exception,e:
                    print "Error inputting from DI%d\n\t%s" % (i,str(e),)
        return 1
