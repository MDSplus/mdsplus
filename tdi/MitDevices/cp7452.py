class CP7452(object):
    """Adlink CP7452 DIO"""
    def __init__(self,node):
        self.node=node
        return

    def init(self,arg):
        from MDSplus import Data
        nids=self.node.conglomerate_nids
        try:
            host=str(nids[1].record.data())
        except:
            host='local'
        if Data.execute('mdsconnect($)',host) != 0:
            print "Connected to: "+host
        else:
            raise Exception,"Error connecting to host: "+host
        board=int(nids[2].record)
        for i in range(4):
            do_nid=nids[i+5]
            if do_nid.on:
                try:
                    exp='MdsValue("_lun=fopen(\\\"/sys/module/cp7452_drv/parameters/format\\\",\\\"r+\\"); write(_lun,\\\"1\\\"); fclose(_lun);'
                    exp=exp+'_lun=fopen(\\\"/dev/cp7452.%d/DO%d\\\",\\\"r+\\\"); write(_lun,\\\"%x\\\"); fclose(_lun)")' % (board,i,int(do_nid.record))
                    print exp
                    Data.execute(exp)
                except Exception,e:
                    print "Error outputing to DO%d\n\t%s" % (i,str(e),)
        return 1

    def store(self,arg):
        from MDSplus import Data
        nids=self.node.conglomerate_nids
        try:
            host=str(nids[1].record.data())
        except:
            host='local'
        if Data.execute('mdsconnect($)',host) != 0:
            print "Connected to: "+host
        else:
            raise Exception,"Error connecting to host: "+host
        board=int(nids[2].record)
        for i in range(4):
            do_nid=nids[i+10]
            if do_nid.on:
                try:
                    exp='MdsValue("_lun=fopen(\\\"/sys/module/cp7452_drv/parameters/format\\\",\\\"r+\\\"); write(_lun,\\\"1\\\"); fclose(_lun);'
                    exp=exp+'_lun=fopen(\\\"/dev/cp7452.%d/DI%d\\\",\\\"r\\\"); _ans=read(_lun); fclose(_lun),_ans")' % (board,i)
                    print exp
                    value=eval('0x'+str(Data.execute(exp)))
                    do_nid.record=value
                except Exception,e:
                    print "Error inputting from DI%d\n\t%s" % (i,str(e),)
        return 1

    def ORIGINAL_PART_NAME(self,arg):
        names=('',':NODE',':BOARD',':COMMENT','.DIGITAL_OUTS',
               '.DIGITAL_OUTS:DO0','.DIGITAL_OUTS:DO1','.DIGITAL_OUTS:DO2','.DIGITAL_OUTS:DO3',
               '.DIGITAL_INS',
               '.DIGITAL_INS:DI0','.DIGITAL_INS:DI1','.DIGITAL_INS:DI2','.DIGITAL_INS:DI3',
               ':INIT_ACTION',':STORE_ACTION',)
        return names[int(self.node.conglomerate_elt-1)]
