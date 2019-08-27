import java.io.*;
import java.net.*;
import java.util.*;
import jScope.*;

class MdsIp implements Runnable
{
    int port;
    public MdsIp(int port) {this.port = port; }
    ServerSocket server_sock;
    boolean listening = true;
    Vector<ConnectionListener> listeners = new Vector<ConnectionListener>();
    Thread listen_thread;

    public Thread getListenThread() { return listen_thread; }


    public boolean start()
    {
	try {
	    server_sock = new ServerSocket(port);
	}catch(Exception e) {
	    System.err.println("Could not listen on port: " + port);
	    return false;
	}
	listen_thread = new Thread(this);
	listen_thread.start();
	return true;
    }



    public void run()
    {
	while(listening)
	{
	    try{
	        new ReceiverThread(server_sock.accept()).start();
	    }catch(Exception exc) {fireConnectionEvent(); break;}
	}

	try {
	    server_sock.close();
	}catch(Exception exc) {}
    }

   public synchronized void addConnectionListener(ConnectionListener listener)
   {
	listeners.addElement(listener);
   }

   protected synchronized void fireConnectionEvent()
   {
	Enumeration<ConnectionListener> listener_list = listeners.elements();
	while(listener_list.hasMoreElements())
	{
	    ConnectionListener listener = listener_list.nextElement();
	    listener.processConnectionEvent(new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION,
	        "Lost connection to mdsip client"));
	}
   }

   public /*synchronized*/ MdsMessage handleMessage(MdsMessage messages[])
   {
	return new MdsMessage();
   }


    class ReceiverThread extends Thread
    {
	Socket sock;
	DataInputStream dis;
	DataOutputStream dos;

	public ReceiverThread(Socket sock)
	{
	    this.sock = sock;
	    try {
	        dis = new DataInputStream(new BufferedInputStream(sock.getInputStream()));
	        dos = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream()));
	    }catch(Exception exc) {fireConnectionEvent(); dis = null; dos = null;}
	}

	public void run()
	{
	    if(dis == null || dos == null) return;
	    MdsMessage msg = new MdsMessage((byte)0, (byte)0, (byte)0, null, new byte[0]);
	    try {
	        msg.Receive(dis);  //Connection message
	        (new MdsMessage()).Send(dos);
	    }catch(Exception exc){fireConnectionEvent(); return; }
//            System.out.println("Received connection from "+new String(msg.body));
	    while(true)
	    {
	        try {
	            MdsMessage curr_msg = new MdsMessage((byte)0, (byte)0, (byte)0, null, new byte[0]);
	            curr_msg.Receive(dis);

//		    System.out.println("Devo ricevere n messaggi "  + curr_msg.nargs);

	            MdsMessage messages[] = new MdsMessage[curr_msg.nargs];
	            messages[0] = curr_msg;
	            int nargs = curr_msg.nargs;
	            for(int i = 0; i < nargs-1; i++)
	            {
	                messages[i+1] = new MdsMessage((byte)0, (byte)0, (byte)0, null, new byte[0]);
	                messages[i+1].Receive(dis);

//			System.out.println("Ricevuto messaggio  "+ i + " " + new String(messages[i+1].body));
	            }
	            MdsMessage answ = handleMessage(messages);
	            answ.Send(dos);
	        }catch(Exception exc){fireConnectionEvent(); break; }
	    }
	}
    }
}





