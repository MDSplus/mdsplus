import MDSplus
import socket

class CHECK_MARTE2(MDSplus.Device):
    """Use heartbeat messages to check if MARTe2 processes are alive"""
    parts = [
        {'path':':COMMENT','type':'text'}, 
    ]
    for i in range(8):
        parts.extend([
            {'path':'.MARTE2_%d'%(i+1),'type':'structure'},
            {'path':'.MARTE2_%d:IP'%(i+1),'type':'text'},
            {'path':'.MARTE2_%d:PORT'%(i+1),'type':'numeric'},
        ])
    parts.extend([
        {'path':':CHECK_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('MARTE_SERVER','PULSE_ON',1,None),Method(None,'check',head))",
        'options':('no_write_shot',)}
    ])
    del(i)

    def check(self):
        for i in range(8):
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                currIp = self.getNode('.MARTE2_%d:IP'%(i+1)).data()
                currPort = self.getNode('.MARTE2_%d:PORT'%(i+1)).data()
            except:
                continue
            try:
                s.connect((currIp, currPort))
                s.sendall('1234')
                data = s.recv(4)
                if data != '1234':
                    printf('Internal error: wrong data read')
                    raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
                print('MARTe2 engine at '+currIp+ 'port '+str(currPort)+ ' is alive!')
                s.close()
            except:
                raise MDSplus.mdsExceptions.TclFAILED_ESSENTIAL
