import java.util.*;
import java.net.*;
import java.io.*;

class MdsMonitor extends MdsIp implements MonitorListener, Runnable
{
    Vector outstream_vect = new Vector();
    Vector msg_vect = new Vector();
    
    public MdsMonitor(int port)
    {
        super(port);
//        new Thread(this).start();
        new Thread(new Runnable() 
        {
            public void run()
            {
                sendMessages();
            }
         }).start();
    }
    
    
    
    public MdsMessage handleMessage(MdsMessage [] messages)
    {
        if(messages.length < 6 || messages[2].dtype != Descriptor.DTYPE_SHORT 
            || messages[1].dtype != Descriptor.DTYPE_LONG)
        {
            System.err.println("Unexpected message has been received by MdsMonitor");
        }
        else
        {
            try {
                short port = (messages[2].ToShortArray())[0];
                String addr = ""+toShort(messages[1].body[0])+"."+toShort(messages[1].body[1])+"."
                    +toShort(messages[1].body[2])+"."+toShort(messages[1].body[3]);
                Socket sock = new Socket(addr, port);
                outstream_vect.addElement(new BufferedOutputStream(sock.getOutputStream()));
            }catch (Exception exc) {} 
        }
        MdsMessage msg =  new MdsMessage((byte)0, Descriptor.DTYPE_LONG, (byte)0, null, Descriptor.dataToByteArray(new Integer(1)));
        msg.status = 1;
        return msg;
    }
     
    private short toShort(byte b)
    {
        return (short)((short)b & (short)0x00ff);
    }
         
    public void sendMessages()
    {
        while(true)
        {
            while(msg_vect.size() != 0)
            {
                MdsMonitorEvent msg = (MdsMonitorEvent)msg_vect.elementAt(0);
                msg_vect.removeElementAt(0);
                byte [] bin_msg = msg.toBytes();
                Enumeration outstream_list = outstream_vect.elements();
                while(outstream_list.hasMoreElements())
                {
                    OutputStream os = (OutputStream)outstream_list.nextElement();
                    try {
                        os.write(bin_msg);
                        os.flush();
                    }catch(Exception exc){}
                }
            }
            try {
                synchronized(MdsMonitor.this)
                {
                    wait();
                }
            }catch(InterruptedException exc) {return; }
        }
    }
                
    protected void communicate(MonitorEvent event, int mode)
    {
        try {
            MdsMonitorEvent mds_event;
            if(event.getAction() == null)
                mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
                    0, 0, "", 1, mode, "", 1);
            else
            {
                Action action = event.getAction();
                mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
                    MdsHelper.toPhaseId(event.getPhase()), action.getNid(), 
                    action.getName(), action.isOn()?1:0, mode, 
                    ((DispatchData)(action.getAction().getDispatch())).getIdent().getString(),
                    action.getStatus());
            }
            msg_vect.addElement(mds_event);
            notify();
        }catch(Exception exc)
        {
            System.out.println(exc);
        }
    }
    public synchronized void buildBegin(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_BUILD_BEGIN);
    }
    
    public synchronized void build(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_BUILD);
    }
    
    public synchronized void buildEnd(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_BUILD_END);
    }
    public synchronized void dispatched(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_DISPATCHED);
    }
    public synchronized void doing(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_DOING);
    }
    public synchronized void done(MonitorEvent event)
    {
        communicate(event, jDispatcher.MONITOR_DONE);
    }
}
