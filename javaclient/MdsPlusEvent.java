/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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
