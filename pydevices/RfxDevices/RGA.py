from MDSplus import Device, Data, Range, Dimension, Window, Signal, Int16Array
from ctypes import CDLL,c_char_p,c_short,byref, c_int
import socket

class RGA(Device):
    """Residual Gas Analizer"""
    parts = [
        {'path':':COMMENT','type':'text'}, 
        {'path':':FILE','type':'text'},
        {'path':':SERVER_ADDR','type':'text'},
        {'path':':SERVER_PORT','type':'numeric'},
        {'path':':PRESSURE','type':'signal', 'options':('no_write_model','compress_on_put')}
    ]
    for i in range(101):
        parts.extend([
            {'path':'.MASS_%03d'%(i),'type':'structure'},
            {'path':'.MASS_%03d:DESCRIPTON'%(i),'type':'text'},
            {'path':'.MASS_%03d:DATA'%(i),'type':'signal', 'options':('no_write_model','compress_on_put')}
        ])
    parts.extend([
        {'path':':START_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('SERVER','INIT',50,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)},
        {'path':':STOP_ACTION','type':'action',
        'valueExpr':"Action(Dispatch('SERVER','STORE',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)},
    ])
    del(i)

    def sendMessage(self, msg):

        print('Send message ', msg)
        try:
            serverAddr = self.server_addr.data();
        except:
            emsg = 'Missing server address'
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
            raise DevBAD_PARAMETER

        try:
            serverPort = self.server_port.data();
        except:
            emsg = 'Missing server port'
            Data.execute('DevLogErr($1,$2)', self.getNid(), emsg )
            raise DevBAD_PARAMETER

        # Create a datagram socket
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM);
        
        s.sendto(msg, (serverAddr, serverPort))


    def start_store(self):
        msg = 'START GVS %d'%(self.getTree().shot)
        self.sendMessage(msg)

    def stop_store(self):
        msg = 'STOP GVS %d'%(self.getTree().shot)
        self.sendMessage(msg)

