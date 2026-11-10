import MDSplus
import socket

class CHECK_MARTE2(MDSplus.Device):
    """Use heartbeat messages to check if MARTe2 processes are alive"""
    parts = [
        {'path':':COMMENT','type':'text'}, 
    ]
    for i in range(16):
        parts.extend([
            {'path':'.MARTE2_%d'%(i+1),'type':'structure'},
            {'path':'.MARTE2_%d:IP'%(i+1),'type':'text'},
            {'path':'.MARTE2_%d:SUPERVISOR'%(i+1),'type':'numeric'},
        ])
    parts.extend([
        {'path':':CHECK_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PULSE_ON',1,None),Method(None,'check',head))",
        'options':('no_write_shot',)}
    ])
    del(i)

    def check(self):
        serversOk = True
        for i in range(8):
            try:
                currSupervisor = self.getNode('.MARTE2_%d:SUPERVISOR'%(i+1)).getData()
                if not currSupervisor.isOn():
                    continue 
                currIp = self.getNode('.MARTE2_%d:IP'%(i+1)).data()
                currPort = currSupervisor.getNode(':ALIVE_PORT').data()
            except:
                continue
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((currIp, currPort))
                s.sendall('1234')
                data = s.recv(4)
                if data != '1234':
                    printf('Internal error: wrong data read')
                    raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
                print('MARTe2 engine at '+currIp+ ' port '+str(currPort)+ ' is alive!')
                s.close()
            except:
                serversOk = False
                print('MARTe2 engine at '+currIp+ ' port '+str(currPort)+ ' IS DEAD!!!!!!')

        if not serversOk:
            raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
