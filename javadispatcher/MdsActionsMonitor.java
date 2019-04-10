import java.util.*;
import java.net.*;
import java.io.*;
import jScope.*;

class MdsActionsMonitor extends MdsIp implements MonitorListener, Runnable
{
    Vector<BufferedOutputStream> outstream_vect = new Vector<BufferedOutputStream>();
    Vector<MdsMonitorEvent> msg_vect = new Vector<MdsMonitorEvent>();

    public MdsActionsMonitor(int port)
    {
        super(port);
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
            || messages[1].dtype != Descriptor.DTYPE_BYTE)
        {
            System.err.println("Unexpected message has been received by MdsDoneActionsMonitor");
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
                MdsMonitorEvent msg = msg_vect.elementAt(0);
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
                synchronized(MdsActionsMonitor.this)
                {
                    wait();
                }
            }catch(InterruptedException exc) {return; }
        }
    }

    protected void communicate(MonitorEvent event, int mode)
    {
        try {
            MdsMonitorEvent mds_event = null;
            if(event.getAction() == null)
            {
                int currMode = 0;
                switch(event.eventId)
                {
                    case MonitorEvent.CONNECT_EVENT:
                        currMode = MdsMonitorEvent.MonitorServerConnected;
                        mds_event = new MdsMonitorEvent(this, null, 0, 0, 0, null, 1, currMode, null, event.getMessage(), 1);
                    break;
                    case MonitorEvent.DISCONNECT_EVENT:
                        currMode = MdsMonitorEvent.MonitorServerDisconnected;
                        mds_event = new MdsMonitorEvent(this, null, 0, 0, 0, null, 1, currMode, null, event.getMessage(), 1);
                    break;
                    case MonitorEvent.START_PHASE_EVENT:
                        currMode = MdsMonitorEvent.MonitorStartPhase;
                        mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(), MdsHelper.toPhaseId(event.getPhase()), 0, null, 1, currMode, null, null, 1);
                        break;
                    case MonitorEvent.END_PHASE_EVENT:
                        currMode = MdsMonitorEvent.MonitorEndPhase;
                        mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(), MdsHelper.toPhaseId(event.getPhase()), 0, null, 1, currMode, null, null, 1);
                        break;
                    default:
                        mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(), 0, 0, null, 1, mode, null, null, 1);
                }
            }
            else
            {
                Action action = event.getAction();
                mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
                    MdsHelper.toPhaseId(event.getPhase()), action.getNid(),
                    action.getName(), action.isOn()?1:0, mode,
                    ((DispatchData)(action.getAction().getDispatch())).getIdent().getString(),
                    action.getServerAddress(),
                    action.getStatus());
            }
            msg_vect.addElement(mds_event);
            synchronized(MdsActionsMonitor.this)
            {
                notify();
            }
        }catch(Exception exc)
        {
            System.out.println(exc);
        }
    }

    public synchronized void beginSequence(MonitorEvent event)
    {
    }

//    public synchronized void buildBegin(MonitorEvent event)
    public  void buildBegin(MonitorEvent event)
    {
    }

    public  void build(MonitorEvent event)
    {
    }

    public  void buildEnd(MonitorEvent event)
    {
    }

//    public synchronized void dispatched(MonitorEvent event)
    public  void dispatched(MonitorEvent event)
    {
    }

//    public synchronized void doing(MonitorEvent event)
    public  void doing(MonitorEvent event)
    {
    }

    public synchronized void done(MonitorEvent event)
    {
        communicate( event, jDispatcher.MONITOR_DONE );
    }

    public  void disconnect(MonitorEvent event)
    {
    }

    public  void connect(MonitorEvent event)
    {
    }

    public synchronized void endSequence(MonitorEvent event)
    {
    }

    public  void endPhase(MonitorEvent event)
    {
    }

    public  void startPhase(MonitorEvent event)
    {
    }

}

