from MDSplus import mdsExceptions, Device, Data, Range, makeArray, Int32, Int32Array, Float64, Float64Array, String
try:
    import acq400_hapi
except:
    pass
import socket
import os
import sys
import numpy as np

class DTACQ_SUPERVISOR(Device):
    """DTACQ device  supervisor"""
    parts = [
             {'path': ':IP_ADDR', 'type': 'text'},
             {'path': ':UDP_ADDR', 'type': 'text'},
             {'path': ':UDP_GATEWAY', 'type': 'text'},
             {'path': ':UDP_PORT', 'type': 'numeric', 'value': 53676},
             {'path': ':AI_R_DEVICE', 'type': 'numeric'},
             {'path': ':AI_B_DEVICE', 'type': 'numeric'},
             {'path': ':AI_SITES', 'type': 'numeric', 'value': Int32Array([1,2])},
             {'path': ':AI_CHANS', 'type': 'numeric', 'value': Int32Array([32,32])},
             {'path': ':SPAD_SIZE', 'type': 'numeric', 'value': 16},
             {'path': ':AO_DEVICE', 'type': 'numeric'},
             {'path': ':AO_SITES', 'type': 'numeric', 'value': Int32Array([3])},
             {'path': ':AO_CHANS', 'type': 'numeric', 'value': Int32Array([32])},
             {'path': ':DIO_SITE', 'type': 'numeric', 'value': 6},
             {'path': ':CLOCK_MODE', 'type': 'text', 'value': 'INTERNAL'},
             {'path': ':CLOCK_FREQ', 'type': 'numeric', 'value': 10000},
             {'path': ':CLOCK_DIV', 'type': 'numeric', 'value': 10},
             {'path': ':EXT_CK_FREQ', 'type': 'numeric', 'value': 10000},
             {'path': ':TRIGGER', 'type': 'numeric', 'value': 0},
             {'path': ':DIO_BYTE1', 'type': 'text', 'value': 'INPUT'},
             {'path': ':DIO_BYTE2', 'type': 'text', 'value': 'INPUT'},
             {'path': ':DIO_BYTE3', 'type': 'text', 'value': 'INPUT'},
             {'path': ':DIO_BYTE4', 'type': 'text', 'value': 'INPUT'},
             {'path': '.PG1', 'type': 'structure'},
             {'path': '.PG1:SITE', 'type': 'numeric', 'value': 4},
             {'path': '.PG1:TRIG_SOURCE', 'type': 'text', 'value': 'SOFTWARE'},
             {'path': '.PG1:MODE', 'type': 'text', 'value': 'SINGLE'},
             {'path': '.PG1:TIME_DIV', 'type': 'numeric', 'value': 400000},
             {'path': '.PG1:LOOP_PERIOD', 'type': 'numeric', 'value': 0},
             {'path': '.PG1:D1_TIMES', 'type': 'numeric'},
             {'path': '.PG1:D2_TIMES', 'type': 'numeric'},
             {'path': '.PG1:D3_TIMES', 'type': 'numeric'},
             {'path': '.PG1:D4_TIMES', 'type': 'numeric'},
             {'path': '.PG1:D5_TIMES', 'type': 'numeric'},
             {'path': '.PG2', 'type': 'structure'},
             {'path': '.PG2:SITE', 'type': 'numeric', 'value': 5},
             {'path': '.PG2:TRIG_SOURCE', 'type': 'text', 'value': 'SOFTWARE'},
             {'path': '.PG2:MODE', 'type': 'text', 'value': 'SINGLE'},
             {'path': '.PG2:TIME_DIV', 'type': 'numeric', 'value': 400000},
             {'path': '.PG2:LOOP_PERIOD', 'type': 'numeric', 'value': 0},
             {'path': '.PG2:D1_TIMES', 'type': 'numeric'},
             {'path': '.PG2:D2_TIMES', 'type': 'numeric'},
             {'path': '.PG2:D3_TIMES', 'type': 'numeric'},
             {'path': '.PG2:D4_TIMES', 'type': 'numeric'},
             {'path': '.PG2:D5_TIMES', 'type': 'numeric'},
             {'path': ':PASSWD', 'type': 'text', 'value':'d-t1012q'},
             {'path': ':INIT', 'type': 'action',
                  'valueExpr': "Action(Dispatch('MARTE_SERVER','INIT',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)}]


    class Args:
        def __init__(self, netmask, tx_ip, rx_ip, gw, port, run0, play0, broadcast = 0, disco = None, hudp_relay = None, 
            spp = 1, hudp_decim = 1):

            self.netmask = netmask
            self.tx_ip = tx_ip
            self.rx_ip = rx_ip
            self.gw = gw
            self.port = port
            self.run0 = run0
            self.play0 = play0
            self.broadcast = broadcast
            self.disco = disco
            self.hudp_relay = hudp_relay
            self.spp = spp
            self.hudp_decim = hudp_decim
#INITIALIZE Pulse Generator
    def mergeStl(self, inAlltimes, loopPeriod):
        print('LOOP PERIOD: ', loopPeriod)
        alltimes = inAlltimes.copy()
        outPatterns=[]
        outTimes = []
        currPattern = int(0)
        while True:
            minTime = sys.maxsize
            minIdx = -1
            for idx in range(len(alltimes)):
                if len(alltimes[idx]) == 0:
                    continue
                if alltimes[idx][0] < minTime:
                    minTime = alltimes[idx][0]
                    minIdx = idx
            if minIdx < 0:
                if loopPeriod > 0:
                    outTimes.append(int(loopPeriod))
                    outPatterns.append(outPatterns[-1])
                return outPatterns, outTimes
            mask = currPattern & (1 << minIdx)
            if(mask != 0): #corresponding bit flips to 0
                currPattern = currPattern & ~(1 << minIdx)
            else: #flips to 1
                currPattern = currPattern | (1 << minIdx)
            alltimes[minIdx] = alltimes[minIdx][1:]
            #check if other times are equals
            for idx in range(len(alltimes)):
                if len(alltimes[idx]) == 0:
                    continue
                if alltimes[idx][0] == minTime:
                    mask = currPattern & (1 << idx)
                    if(mask != 0): #corresponding bit flips to 0
                        currPattern = currPattern &  ~(1 << idx)
                    else: #flips to 1
                        currPattern = currPattern |  (1 << idx)
                    alltimes[idx] = alltimes[idx][1:]
            outPatterns.append(currPattern)
            outTimes.append(minTime)
    


    def pgInitSpec(self, pgIdx):
        site = getattr(self, 'pg%d_site' % (pgIdx)).data()
        try:
            trigSourceT = getattr(self, 'pg%d_trig_source' % (pgIdx)).data()
            trigSourceDict = {'TRIG_IN':'1,0,1', 'SOFTWARE': '1,1,1','ADC_TRIG': '1,2,1','SITE4_TRIG': '1,5,1','SITE5_TRIG': '1,6,1'}
            trigSource = trigSourceDict[trigSourceT]
        except:
            print('Invalid trigger source for PG '+str(pgIdx))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            modeT = getattr(self, 'pg%d_mode' % (pgIdx)).data()
            modeDict = {'SINGLE': 0, 'LOOP': 2, 'LOOPWAIT': 3}
            mode = modeDict[modeT]
        except:
            print('Invalid mode for PG '+str(pgIdx))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if mode == 2:
            try:
                loopPeriod = getattr(self, 'pg%d_loop_period' % (pgIdx)).data()
            except:
                print('No period specified for LOOP mode')
                raise mdsExceptions.TclFAILED_ESSENTIAL
        else:
            loopPeriod = -1
        try:
            timeDiv = int(getattr(self, 'pg%d_time_div' % (pgIdx)).data())
        except:
            print('Cannot get Time division for PG '+str(pgIdx))
            raise mdsExceptions.TclFAILED_ESSENTIAL
        alltimes = []    
        for chIdx in range(1,5):
            try:
                times = getattr(self, 'pg%d_d%d_times' % (pgIdx, chIdx)).data()
            except:
                print('No time array defined for D'+str(chIdx)+' IN PG'+str(pgIdx))
                continue 
            if loopPeriod != -1 and times[-1] > loopPeriod:
                print('Invalid times definition: times must be less that loop period')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            alltimes.append(times)
        if len(alltimes) == 0:
            print('No Time signal defined')
            return
        outPatterns, outTimes = self.mergeStl(alltimes, loopPeriod)
        tempF = open('temp.stl', 'w')
        for i in range(len(outTimes)):
            tempF.write(str(outTimes[i])+','+str(outPatterns[i])+'\n')
        tempF.close()
        command = 'sshpass -p'+self.passwd.data()+' scp temp.stl root@'+self.ip_addr.data()+':'
        print(command)
        os.system(command)
        command =  'sshpass -p'+self.passwd.data()+' ssh root@'+self.ip_addr.data()+' -t \'sh -l -c "CSCALE='+str(timeDiv)+' SITE='+str(site)+' /usr/local/CARE/pg_test ' +str(mode)+' temp.stl"\''
        print(command)
        os.system(command)
        
    def pgInit(self):
        self.pgInitSpec(1)
        self.pgInitSpec(2)

# INIT
    def init(self):
#        import hudp_setup
        print('INIT')
        try:
            ipAddr = self.ip_addr.data()
        except:
            print('Cannot get IP Address')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            uut = acq400_hapi.factory(ipAddr)
            #uut = acq400.Acq400(ipAddr)
        except:
            print('Cannot connect to '+ipAddr)
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            triggerTime = self.trigger.data()
        except:
            print('Cannot read trigger time')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            udpAddress = self.udp_addr.data()
        except:
            print('Cannot read UDP address')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            udpGateway = self.udp_gateway.data()
        except:
            print('Cannot read UDP gateway')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            udpPort = self.udp_port.data()
        except:
            print('Cannot read UDP port')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            clockMode = self.clock_mode.data()
        except:
            print('Cannot read Clock Mode')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if(clockMode != 'INTERNAL') and (clockMode != 'EXTERNAL'):
            print('Invalid clock mode: '+clockMode +' must be either INTERNAL or EXTERNAL')
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            aiSites = self.ai_sites.data()
        except:
            print('Cannot read AI sites')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            l = len(aiSites)
        except:
            aiSites = [aiSites]

        try:
            aiChans = self.ai_chans.data()
        except:
            print('Cannot read AI Chans')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            l = len(aiChans)
        except:
            aiChans = [aiChans]

        if len(aiSites) != len(aiChans):
            print("AI sites and AI Chans must be of the same length")
            raise mdsExceptions.TclFAILED_ESSENTIAL

        try:
            aoSites = self.ao_sites.data()
        except:
            print('No AO sites defined, AO will be skipped')
            aoSites = []
        try:
            l = len(aoSites)
        except:
            aoSites = [aoSites]
        if len(aoSites) > 0:
            try:
                aoChans = self.ao_chans.data()
            except:
                print('Cannot read AO channels')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            try:
                l = len(aoChans)
            except:
                aoChans = [aoChans]
            if len(aoSites) != len(aoChans):
                print("AO sites and AO Chans must be of the same length "+str(len(aoSites)) + ", " +str(len(aoChans)))
                raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            dioSite = self.dio_site.data()
        except:
            print('No DIO modulee defined')
            dioSite = -1
            numDis = 0
            numDos = 0
        if dioSite > 0:    
            if self.dio_byte1.data() == 'INPUT' or self.dio_byte2.data() == 'INPUT' or self.dio_byte3.data() == 'INPUT' or self.dio_byte4.data() == 'INPUT':
                numDis = 1
            else:
                numDis = 0
            if self.dio_byte1.data() == 'OUTPUT' or self.dio_byte2.data() == 'OUTPUT' or self.dio_byte4.data() == 'OUTPUT' or self.dio_byte4.data() == 'OUTPUT':
                numDos = 1
            else:
                numDos = 0
        try:
            spadSize = self.spad_size.data()
        except:
            print('Cannot read SPAD size')
            raise mdsExceptions.TclFAILED_ESSENTIAL


        try:
            clockFreq = self.clock_freq.data()
        except:
            print('Cannot read clock frequency')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        try:
            freqDiv = self.clock_div.data()
        except:
            print('Cannot read clock division')
            raise mdsExceptions.TclFAILED_ESSENTIAL
        if clockMode == 'INTERNAL':
            uut.s0.sync_role = 'master ' + str(int(clockFreq))
        else:
            try:
                extClockFreq = self.ext_ck_freq.data()
            except:
                extClockFreq = clockFreq
            uut.s0.sync_role = 'fpmaster '+str(int(clockFreq) + '  '+str(int(extClockFreq)))

        uut.s0.transient='SOFT_TRIGGER=0'
        uut.s0.spad1_us='1,1,1'
        uut.s0.spad1_us_clk_src='0'
        direction = ''
        if self.dio_byte1.data() == 'OUTPUT':
            direction += '1,'
        else:
            direction += '0,'
        if self.dio_byte2.data() == 'OUTPUT':
            direction += '1,'
        else:
            direction += '0,'
        if self.dio_byte3.data() == 'OUTPUT':
            direction += '1,'
        else:
            direction += '0,'
        if self.dio_byte4.data() == 'OUTPUT':
            direction += '1'
        else:
            direction += '0'

        if dioSite > 0:
            getattr(uut, 's'+str(dioSite)).byte_is_output = direction


        if len(aiSites > 0):
            offsTxt = []
            calsTxt = []
            for i in range(len(aiSites)):
                offsTxt.append(getattr(uut, 's'+str(aiSites[i])).AI_CAL_EOFF)
                calsTxt.append(getattr(uut, 's'+str(aiSites[i])).AI_CAL_ESLO) 
            offs = []
            cals = []
            for offTxt in offsTxt:
                currS = offTxt.split()
                for i in range(3, len(currS)):
                    offs.append(float(currS[i]))

            for calTxt in calsTxt:
                currS = calTxt.split()
                for i in range(3, len(currS)):
                    cals.append(float(currS[i]))

            hasRealtimeAi = False
            hasBulkAi = False
            try:
                dtackAi = self.ai_b_device.getData()
                hasBulkAi = True
                dtackAi.parameters_par_13_value.putData(Int32(int(clockFreq))) #num samples
                dtackAi.parameters_par_1_value.putData(Float64(1)) #1 segment per second
                dtackAi.parameters_par_2_value.putData(Float64(triggerTime))
                dtackAi.parameters_par_3_value.putData(Int32(1))
                dtackAi.parameters_par_4_value.putData(len(aiSites))
                dtackAi.parameters_par_5_value.putData(Int32(numDis))
                dtackAi.parameters_par_6_value.putData(Int32Array(aiChans))
                dtackAi.parameters_par_7_value.putData(Float64Array(cals))
                dtackAi.parameters_par_8_value.putData(Float64Array(offs))
                dtackAi.parameters_par_9_value.putData(String(ipAddr))
                dtackAi.parameters_par_10_value.putData(Int32(4210))
                dtackAi.parameters_par_12_value.putData(Int32(spadSize))
            except:
                print('No Bulk AI MARTe2 device')

            try:
                dtackAi = self.ai_r_device.getData()
                hasRealtimeAi = True
                try :
                    freqDivision = self.clock_div.data()
                except:
                    print('Frequency division not defined for realtime device')
                    raise mdsExceptions.TclFAILED_ESSENTIAL


                dtackAi.parameters_par_13_value.putData(Int32(1)) #num samples
                dtackAi.parameters_par_1_value.putData(Float64(clockFreq/freqDiv))
                dtackAi.parameters_par_2_value.putData(Float64(triggerTime))
                dtackAi.parameters_par_3_value.putData(Int32(2))
                dtackAi.parameters_par_4_value.putData(len(aiSites))
                dtackAi.parameters_par_5_value.putData(Int32(numDis))
                dtackAi.parameters_par_6_value.putData(Int32Array(aiChans))
                dtackAi.parameters_par_7_value.putData(Float64Array(cals))
                dtackAi.parameters_par_8_value.putData(Float64Array(offs))
                dtackAi.parameters_par_9_value.putData(String(udpAddress))
                dtackAi.parameters_par_10_value.putData(Int32(udpPort))
                dtackAi.parameters_par_12_value.putData(Int32(spadSize))
                dtackAi.parameters_par_14_value.putData(Int32(freqDivision))
            except:
                print('No Realtime AI MARTe2 device')

            st = ''
            for i in range(len(aiSites)):
                st += str(aiSites[i])
                if i < len(aiSites) - 1:
                    st += ','
            if numDis > 0:
                st += ','+str(dioSite)
#                st += ' 1,'+str(spadSize)+',0'
            st += ' 1,'+str(spadSize/4)+',0'
            if hasRealtimeAi:
                args = self.Args('255.255.255.0', udpAddress, socket.gethostbyname(socket.gethostname()), udpGateway, udpPort, st, '', hudp_decim = freqDiv)
                self.config_tx_uut(uut, args)
            if hasBulkAi:
                print("BULK ")
                print(st)
                uut.s0.run0 = st

 #endif len(aiSites) > 0
        if len(aoSites) > 0:  #There are Outputs
            try:
                dtackAo = self.ao_device.getData()
            except:
                print('Cannot fine MARTE2_TACQAO device')
                raise mdsExceptions.TclFAILED_ESSENTIAL
            dtackAo.parameters_par_1_value.putData(Int32(len(aoSites)))
            dtackAo.parameters_par_2_value.putData(Int32(numDos))
            dtackAo.parameters_par_3_value.putData(Int32Array(aoChans))
            dtackAo.parameters_par_4_value.putData(udpAddress)
            dtackAo.parameters_par_5_value.putData(Int32(udpPort))

            st = ''
            for i in range(len(aoSites)):
                st += str(aoSites[i])
                if i < len(aoSites) - 1:
                    st += ','
            if numDos > 0:
                st += ','+str(dioSite)
            st += ' 0'

            args = self.Args('255.255.255.0', socket.gethostbyname(socket.gethostname()), udpAddress, udpGateway, udpPort, '', st)
            self.config_rx_uut(uut, args)
    
####################HUDP_SEUP Stuff   
    

    def hudp_init(self, args, uut, ip):
        uut.s10.tx_ctrl = 9
        uut.s10.ip = ip
        uut.s10.gw = args.gw
        uut.s10.netmask = args.netmask
        if args.disco is not None:
            print("enable disco at {}".format(args.disco))
            uut.s10.disco_idx = args.disco
            uut.s10.disco_en  = 1
        else:
            uut.s10.disco_en = 0
    
    def hudp_enable(self, uut):
        uut.s10.tx_ctrl = 1
        
    def ip_broadcast(self, args):
        ip_dest = args.rx_ip.split('.')
        nm = args.netmask.split('.')
        
        for ii in range(3,0,-1):
            if nm[ii] != '0':
                break
            else:
                ip_dest[ii] = '255'
        
        return '.'.join(ip_dest)
            
    MTU = 1400

    # tx: XI : AI, DI       
    def config_tx_uut(self, txuut, args):    
        print("txuut {}".format(txuut.uut))
        print(args.run0)
        if args.run0 != 'notouch':
            txuut.s0.run0 = args.run0
        self.hudp_init(args, txuut, args.tx_ip)
        txuut.s10.hudp_decim = args.hudp_decim
        txuut.s10.src_port = args.port
        txuut.s10.dst_port = args.port
        txuut.s10.dst_ip = args.rx_ip if args.broadcast == 0 else ip_broadcast(args)

        if args.hudp_relay is not None:
            txuut.s10.udp_data_src = 1
            tx_ssb = int(txuut.s0.dssb) - args.hudp_relay
            txuut.s10.slice_len = tx_ssb//4
            txuut.s10.slice_off = args.hudp_relay//4
        else:
            txuut.s10.udp_data_src = 0
            tx_ssb = int(txuut.s0.ssb)

        txuut.s10.tx_sample_sz = tx_ssb
        txuut.s10.tx_spp = args.spp
        tx_pkt_sz = tx_ssb*args.spp                         # compute tx pkt sz and check bounds
        if  tx_pkt_sz > self.MTU:
            print("ERROR packet length {} exceeds MTU {}".format(tx_pkt_sz, self.MTU))
        self.hudp_enable(txuut)
        tx_calc_pkt_sz = int(txuut.s10.tx_calc_pkt_sz)      # actual tx pkt sz computed by FPGA logic.
        if tx_pkt_sz != tx_calc_pkt_sz:
            print("ERROR: set tx_pkt_size {} actual tx_pkt_size {}".format(tx_pkt_sz, tx_calc_pkt_sz))    
        print("TX configured. ssb:{} spp:{} tx_pkt_size {}".format(tx_ssb, args.spp, tx_pkt_sz))

    # rx: XO : AO, DO        
    def config_rx_uut(self, rxuut, args):
        print("rxuut {}".format(rxuut.uut))
        
        if args.play0 != 'notouch':
            rxuut.s0.play0 = args.play0       
        rxuut.s0.distributor = 'comms=U off'        
        rxuut.s0.distributor = 'on'

        self.hudp_init(args, rxuut, args.rx_ip)
        rxuut.s10.rx_src_ip = args.tx_ip
        rxuut.s10.rx_port = args.port
        self.hudp_enable(rxuut)    
        



            

            
                    
                       





        
        

