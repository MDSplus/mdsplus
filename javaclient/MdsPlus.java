package MdsPlus;

import java.util.Hashtable;
import java.applet.*;
import java.io.*;
import java.net.*;

/*
 *
 * MdsPlus
 *
 */

	/** The MdsPlus class provides a socket interface to the MdsPlus data system
	* @author T.W.Fredian
	*/

public class MdsPlus extends Object implements Runnable
{
	private Socket s;
	private byte message_id;
	private Hashtable<Object, Object> m_usersHash = null;
	private Hashtable<String, MdsPlusEvent> m_eventHash = null;
	private Thread thread = null;
	private byte m_eventNextId=0;

	private static Hashtable<String, MdsPlus> m_connectionHash = new Hashtable<String, MdsPlus>();

	private static final byte MAX_BYTE=127;
	private static final MdsPlusDescriptor EVENT_REQUEST = new MdsPlusDescriptor("---EVENTAST---REQUEST---");
	private static final MdsPlusDescriptor EVENT_CANCEL = new MdsPlusDescriptor("---EVENTCAN---REQUEST---");
	private static final String USER = "java-client";
	private static final MdsPlusException UNAUTHORIZED =
		new MdsPlusException("You are not authorized to connect to this host");
	private static final byte JAVA_CLIENT=3;
	private static final byte BIG_ENDIAN=(byte)0x80;

	/** Connect to MdsPlus server. <BR>
	*   <B>Note that browsers only allow connecting back
	*   to the host from which the java applet was downloaded.</B>
	* @param host the remote MdsPlus server tcpip address.<BR>
	* @param port the port used by the remote server.
	* @exception java.io.UnknownHostException if host cannot be found.
	* @exception java.io.IOException if remote host drops connection
	*/

	public MdsPlus(String host, int port) throws UnknownHostException,IOException,MdsPlusException
	{
		connect(host,port);
	}

	public MdsPlus(String host, int port, Object owner) throws UnknownHostException,IOException,MdsPlusException
	{
		connect(host,port);
		m_usersHash.put(owner,owner);
	}

	public static MdsPlus connect(String host, int port, Object owner)
	  throws UnknownHostException,IOException,MdsPlusException
	{
		MdsPlus mds = null;
		InetAddress hostAddr = InetAddress.getByName(host);
		String key = hostAddr.toString() + ":" + port;
		synchronized(m_connectionHash)
		{
			if (!m_connectionHash.containsKey(key))
			{
				mds = new MdsPlus(host,port,owner);
				m_connectionHash.put(key,mds);
			}
			else
			{
				mds = m_connectionHash.get(key);
				if (mds.s == null)
				{
					mds.connect(host,port);
				}
				mds.m_usersHash.put(owner,owner);
			}
		}
		return mds;
	}

	public void disconnect(Object owner)
	{
		synchronized(m_connectionHash)
		{
			if (m_usersHash.containsKey(owner))
			{
				m_usersHash.remove(owner);
			}
			if (m_usersHash.isEmpty())
			{
				System.out.println("Closing data source");
				close();
			}
		}
	}


	/** Close the connection to the host. The MdsPlus object is no longer useable after
	*   the close has been performed.
	*/
	public void close() {
		try {
		if (s != null)
			s.close();
		s = null;
		m_eventHash = new Hashtable<String, MdsPlusEvent>();
		m_usersHash = new Hashtable<Object, Object>();
		m_eventNextId = 0;
		} catch(Exception e){};
	}

	public void connect(String host, int port) throws UnknownHostException,IOException,MdsPlusException
	{
		s = new Socket(host,port);
		m_usersHash = new Hashtable<Object, Object>();
		SendMsg((byte)0,(byte)0,new MdsPlusDescriptor(USER));
		MdsPlusDescriptor ans = GetMsg();
		if ((ans.status & 1) == 0)
		{
			close();
			throw UNAUTHORIZED;
		}
	}

	private short ArgLen(MdsPlusDescriptor a) {
		short len;
		switch (a.dtype)
		{
		case MdsPlusDescriptor.DTYPE_USHORT:
		case MdsPlusDescriptor.DTYPE_SHORT: len = 2; break;
		case MdsPlusDescriptor.DTYPE_UINT:
		case MdsPlusDescriptor.DTYPE_INT:
		case MdsPlusDescriptor.DTYPE_FLOAT: len = 4; break;
		case MdsPlusDescriptor.DTYPE_DOUBLE:
		case MdsPlusDescriptor.DTYPE_COMPLEX: len = 8; break;
		default: len = 1; break;
		case MdsPlusDescriptor.DTYPE_CSTRING: len = (short)a.data.length; break;
		}
		return len;
	}

	private void SendMsg(byte idx, byte nargs, MdsPlusDescriptor descr) throws IOException
	{
		int i;
		Object ans;
		ByteArrayOutputStream buffer = new ByteArrayOutputStream(48+descr.data.length);
		DataOutputStream out = new DataOutputStream(buffer);
		out.writeInt(48+descr.data.length);
		out.writeInt(0);
		out.writeShort(ArgLen(descr));
		out.writeByte(nargs);
		out.writeByte(idx);
		out.writeByte(message_id);
		out.writeByte(descr.dtype);
		out.writeByte((byte)(JAVA_CLIENT | BIG_ENDIAN));
		out.writeByte(descr.dims.length);
		for (i=0;i<8;i++) out.writeInt(i < descr.dims.length ? descr.dims[i] : 0);
		out.write(descr.data,0,descr.data.length);
		out.flush();
		buffer.writeTo(s.getOutputStream());
		if (idx >= (nargs - 1))
			message_id++;
	}


	private static int FlipBytes(int in) {
		return ((in >> 24) & 0xff) | (in << 24) |
			   ((in & 0xff0000) >> 8) | ((in & 0xff00) << 8);
	}

	private static short FlipBytes(short in) {
		return (short)((in >> 8) | (in << 8));
	}


	private MdsPlusDescriptor GetMsg() throws IOException {
		MdsPlusDescriptor d = new MdsPlusDescriptor();
		byte bytes[]=null;
		DataInputStream in = new DataInputStream(s.getInputStream());
		int message_length = FlipBytes(in.readInt());
//		System.out.println("message_length="+(new Integer(message_length)).toString());
		d.status = FlipBytes(in.readInt());
//		System.out.println("status="+(new Integer(d.status)).toString());
		short length = FlipBytes(in.readShort());
//		System.out.println("length="+(new Integer((int)length)).toString());
		byte nargs = in.readByte();
//		System.out.println("nargs="+(new Integer((int)nargs)).toString());
		byte didx = in.readByte();
//		System.out.println("didx="+(new Integer((int)didx)).toString());
		byte message_id = in.readByte();
//		System.out.println("message_id="+(new Integer((int)message_id)).toString());
		d.dtype = in.readByte();
//		System.out.println("dtype="+(new Integer((int)d.dtype)).toString());
		byte client_type = in.readByte();
//		System.out.println("client_type="+(new Integer((int)client_type)).toString());
		byte ndims = in.readByte();
//		System.out.println("ndims="+(new Integer((int)ndims)).toString());
		d.dims = new int[ndims];
		int i;
		for (i=0;i<8;i++) {
			int dim= FlipBytes(in.readInt());
	//		System.out.println("dim="+(new Integer(dim)).toString());
			if (i < ndims) d.dims[i] = dim;
		}
//		System.out.println("About to allocate "+(message_length - 48)+" bytes for data buffer");
		d.data = new byte[message_length - 48];
//		System.out.println("Successfully allocated buffer");
		int offset;
		int len;
	for (offset = 0; offset < d.data.length; offset += len)
		{
			len = d.data.length - offset;
			len = Math.min(8192,len);
		    in.readFully(d.data,offset,len);
//			System.out.println("Read "+(offset+len)+" of "+d.data.length);
		}
//		System.out.println("Completed data read");
		return d;
	}


	/** Returns the value of the MdsPlus expression.
	* @param expression a String containing a valid MdsPlus expression.
	* @return an MdsPlusDescriptor containing the resultant data.
	* @exception IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is an error evaluating the expression.
	*/

	public MdsPlusDescriptor Value(String expression) throws IOException,MdsPlusException
	{
		return Value(expression,new MdsPlusDescriptor[0]);
	}

	/** Returns the value of the MdsPlus expression. An array of optional arguments to
	* the expression can be specified.
	* @param expression a String containing a valid MdsPlus expression.
	* @param args an array of MdsPlusDescriptors containing substitution arguments for the expression.
	* @return an MdsPlusDescriptor containing the resultant data.
	* @exception java.io.IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is an error evaluating the expression.
	*/

    public MdsPlusDescriptor Value(String expression, MdsPlusDescriptor args[]) throws IOException,MdsPlusException
	{
//		long startTime = System.currentTimeMillis();
//		System.out.println("Sending expression: "+expression);
		MdsPlusDescriptor ans = new MdsPlusDescriptor();
		synchronized(this)
		{
			byte nargs = (byte)(1 + args.length);
			SendMsg((byte)0,nargs,new MdsPlusDescriptor(expression));
			for (int i=0;i<args.length;i++)
				SendMsg((byte)(i+1),nargs,args[i]);
			ans = GetMsg();
		}
		if ((ans.status & 1) == 0)
			throw new MdsPlusException(ans.GetErrorString());
//		System.out.println("Got answer in"+(System.currentTimeMillis() - startTime) + " milliseconds");
		return ans;
	}


	private String GetVmsErrorString(int status) throws IOException,MdsPlusException
	{
//		return Value("_a=Repeat(' ',250),librtl->lib$sys_getmsg("+status+",val(0),descr(_a)),trim(_a)").String();
		return (new Integer(status)).toString();
	}

	private void CheckStatus(String s, MdsPlusDescriptor d) throws IOException,MdsPlusException
	{
		if ((d.status & 1) == 0)
			throw new MdsPlusException(s+d.GetErrorString());
		else
		{
			int status = d.Int()[0];
			if ((status & 1) == 0)
				throw new MdsPlusException(s+GetVmsErrorString(status));
		}
	}

	/** Cause an MdsPlus event to occur on the remote host.
	* @param eventname name of MdsPlus event to generate on remote host.
	*/

	public void SetEvent(String eventname) throws IOException,MdsPlusException
	{
		CheckStatus("Error setting event:",Value("SetEvent('"+eventname+"',*)"));
	}

	/** Open an MdsPlus tree on the remote host.
	* @param tree the name of the MdsPlus tree on the remote system.
	* @param shot the shot number of the tree.
	* @exception java.io.IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is a problem opening the tree.
	*/

	public void OpenTree(String tree,int shot) throws IOException,MdsPlusException
	{
		CheckStatus("Error opening tree ("+tree+","+shot+":",Value("TreeOpen('"+tree+"',"+shot+")"));
	}

    /** Close an MdsPlus tree on the remote host.
	* @param tree the name of the MdsPlus tree on the remote system.
	* @param shot the shot number of the tree.
	* @exception java.io.IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is a problem closing the tree.
	*/

	public void CloseTree(String tree,int shot) throws IOException,MdsPlusException
	{
		CheckStatus("Error closing tree:",Value("TreeClose('"+tree+"',"+shot+")"));
	}

	/** Set current tree position to the specified node name.
	* @param nodename the name of the node to become the new default path
	* @exception java.io.IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is a problem accessing the node
	*/

	public void SetDefault(String nodename) throws IOException,MdsPlusException
	{
		MdsPlusDescriptor args[] = new MdsPlusDescriptor[1];
		args[0] = new MdsPlusDescriptor(nodename);
		CheckStatus("Error setting default:",Value("_NID=0L,TreeSetDefault($,_NID)",args));
	}
	/** Store data in an MdsPlus tree.
	* @param nodename the name of the node to which the data is to be written.
	* @param expression an MdsPlus expression.
	* @param args an array of MdsPlusDescriptors describing substitution data for the expression.
	* @exception java.io.IOException if there is a problem communicating with the remote host.
	* @exception MdsPlus.MdsPlusException if there is a problem writing to the tree.
	*/

	public void Put(String nodename, String expression, MdsPlusDescriptor args[]) throws IOException,MdsPlusException
	{
		String exp = "TreePut('"+nodename+"',$";
		MdsPlusDescriptor args_n[] = new MdsPlusDescriptor[args.length+1];
		args_n[0] = new MdsPlusDescriptor(expression);
		for (int i=0;i<args.length;i++)
		{
			exp += ",$";
			args_n[i+1] = args[i];
		}
		exp +=")";
		CheckStatus("Error during put:",Value(exp,args_n));
	}

	private byte NextEvent(byte event_data[])	throws IOException,MdsPlusException
	{
	  MdsPlusDescriptor m = GetMsg();
	  byte event_id;
	  if (m.data.length == 13)
	  {
		  event_id = m.data[12];
		  for (int i=0;i<event_data.length;i++)
			  event_data[i] = m.data[i];
	  }
	  else
		  throw new MdsPlusException("Invalid event data received");
	  return event_id;
	}

	/** Sets up an MdsPlus event notifier on the remote host. <BR>
	* <B>This procedure is used by
	* the MdsPlusEvent constructor and should not be called directly by an application.</B>
	*/
	public static byte EventAst(MdsPlusEvent ev, String host, int port)
	  throws UnknownHostException,IOException,MdsPlusException
	{
		byte event_id;
		MdsPlus mds;
		synchronized( mds = connect(host,port,ev))
		{
			ev.m_mds = (Object)mds;
			if (mds.m_eventHash == null)
				mds.m_eventHash = new Hashtable<String, MdsPlusEvent>(13,0.5f);
			event_id = mds.m_eventNextId;
			if (mds.m_eventNextId == MAX_BYTE)
				mds.m_eventNextId = 0;
			else
				mds.m_eventNextId++;
			try
			{
				if (mds.thread == null || !mds.thread.isAlive())
				{
					mds.thread = new Thread(mds);
					mds.thread.start();
				}
				if (mds.m_eventHash.containsKey(""+event_id))
				{
					Cancel(mds.m_eventHash.get(""+event_id));
				}
				byte idarr[] = new byte[1];
				idarr[0] = event_id;
				mds.SendMsg((byte)0,(byte)3,EVENT_REQUEST);
				mds.SendMsg((byte)1,(byte)3,new MdsPlusDescriptor(ev.m_name));
				mds.SendMsg((byte)2,(byte)3,new MdsPlusDescriptor(idarr));
				mds.m_eventHash.put(""+event_id, ev);
			}
			catch (Exception e)
			{
				mds.close();
			}
		}
		return event_id;
	}

	/** Sets up an MdsPlus event notifier on the remote host. <BR>
	* <B>This procedure is used by
	* the MdsPlus.MdsPlusEvent constructor and should not be called directly by an application.</B>
	*/
	public static byte EventAst(MdsPlusEvent ev, URL url)
	  throws UnknownHostException,IOException,MdsPlusException
	{
		return EventAst(ev, url.getHost(), 8001);
	}


	private void Cancel(int id)
	{
	}

	/** Cancels the MdsPlus event notification on a particular event. <BR>
	* <B>This procedure is used
	* by the MdsPlusEvent Cancel method and should not be called directly by an application.</B>
	*/

	public static void Cancel(MdsPlusEvent ev) {
		try
		{
			MdsPlus mds = (MdsPlus)ev.m_mds;
			synchronized(mds)
			{
				if (mds.m_eventHash.containsKey(""+ev.m_id))
				{
					mds.m_eventHash.remove(""+ev.m_id);
					try
					{
						mds.SendMsg((byte)0,(byte)2,EVENT_CANCEL);
						mds.SendMsg((byte)1,(byte)2,new MdsPlusDescriptor(ev.m_id));
					}
					catch (Exception e) {}
					if (mds.m_eventHash.isEmpty())
					{
						mds.close();
					}
				}
			}
		}
		catch (Exception e){}
	}

	/** Used for MdsPlus event handling. Do not call directly
	*/

	public void run()
	{
		while (thread != null)
		{
			byte eventid;
			byte event_data[] = new byte[12];
			try
			{
				eventid = NextEvent(event_data);
			}
			catch (Exception e)
			{
				return;
			}
			MdsPlusEvent ev = m_eventHash.get(""+eventid);
			if (ev != null)
			{
				ev.m_data = event_data;
				new Thread(ev).start();
			}
		}
	}

}

