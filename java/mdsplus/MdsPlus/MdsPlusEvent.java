package MdsPlus;

import java.net.URL;


	/** this class implements an MdsPlus event notifier.
	*/
public class MdsPlusEvent extends Object implements Runnable {
	private MdsPlusEvents m_target;
	protected Object m_mds;
	/** the object supplied in the constructor. Used by the target's EventAction method.
	*/
	public Object m_arg;
	/** the name of the MdsPlus event that is being monitored.
	*/
	public String m_name;
	/** a byte array containing 12 bytes of data associated with the event.
	*/
	public byte[] m_data;
	/** a long to contain the last time the event occurred.
	*/
	public long m_lastFired;
	/** a long to contain the last time the event was cheched (for polling).
	*/
	public long m_lastChecked;
	/** a string containing the host of this event.
	*/
	public String m_host;
	/** an int containing the port of this event.
	*/
	public int m_port;
	protected byte    m_id;


	/** constructs an MdsPlusEvent.
	* @param codebase the URL of the original applet. Used to find the remote host to connect to.
	* @param name the name of the MdsPlus event to monitor.
	* @param target the name of an object that implements MdsPlusEvents. The EventAction
	* method of this object will be called each time the event occurs.
	* @param arg an object which will be passed to the EventAction methor of the target.
	* @exception UnknownHostException if unable to locate the remote host.
	* @exception IOException if there are problems communicating with the remote host.
	* @exception MdsPlusException if there is a problem setting up the event monitor.
	*/
	public MdsPlusEvent(URL codebase, String name, MdsPlusEvents target, Object arg)
		throws java.net.UnknownHostException,java.io.IOException,MdsPlusException

	{
		init(codebase.getHost(),8001,name,target,arg);
//		m_name = name;
//		m_target = target;
//		m_arg = arg;
//		m_id = MdsPlus.EventAst(this, codebase);
	}

	/** constructs an MdsPlusEvent.
	* @param host the name of the remote host.
	* @param port the port used for monitoring events.
	* @param name the name of the MdsPlus event to monitor.
	* @param target the name of an object that implements MdsPlusEvents. The EventAction
	* method of this object will be called each time the event occurs.
	* @param arg an object which will be passed to the EventAction methor of the target.
	* @exception UnknownHostException if unable to locate the remote host.
	* @exception IOException if there are problems communicating with the remote host.
	* @exception MdsPlusException if there is a problem setting up the event monitor.
	*/
	public MdsPlusEvent(String host, int port, String name, MdsPlusEvents target, Object arg)
		throws java.net.UnknownHostException,java.io.IOException,MdsPlusException
	{
		init(host,port,name,target,arg);
	}


	private void init(String host, int port, String name, MdsPlusEvents target, Object arg)
		throws java.net.UnknownHostException,java.io.IOException,MdsPlusException
	{
		m_name = name;
		m_target = target;
		m_arg = arg;
		m_lastFired = -1;
		m_lastChecked = 0;
		m_host = host;
		m_port = port;
		m_id = MdsPlus.EventAst(this, host, port);
	}

	/** the target EventAction method is executed in a separate thread.
	*/
	public void run() {
		m_lastFired = System.currentTimeMillis();
		if (m_target != null)
		{
			try
			{
				m_target.EventAction(m_arg, m_data);
			}
			catch (Exception e)
			{
				Cancel();
				System.out.println("Error executing event action, canceling");
			}
		}
	}

	/** cancels an event monitor.
	*/
	public void Cancel()
	{
		MdsPlus.Cancel(this);
	}

	/** check to see of event occurred since last time checked. Used for polling.
	*/
	public boolean CheckEvent()
	{
		boolean ans = m_lastFired > m_lastChecked;
		m_lastChecked = System.currentTimeMillis();
		return ans;
	}

	public String toString()
	{
		return "MdsPlusEvent "+m_host+":"+m_port+":"+m_name;
	}
}
