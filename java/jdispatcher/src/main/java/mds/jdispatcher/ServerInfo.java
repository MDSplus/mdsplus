package mds.jdispatcher;
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author taliercio
 */
import javax.swing.JButton;

import java.io.*;

 public class ServerInfo
 {

	private final String  className;
	private final String  address;
	private final String  subTree;
	private final boolean isJava;
	private final int     watchdogPort;
	private final String  startScript;
	private final String  stopScript;
	private MdsMonitorEvent  monitorEvent;
	private int     pos;
	boolean killed = false;
	private jServer server;
	private boolean active;
	
	public class TableButton extends JButton
	{
		private static final long serialVersionUID = 1L;
		private final ServerInfo serverInfo;
	    public TableButton(String label, ServerInfo serverInfo)
	    {
	        super(label);
	        this.serverInfo = serverInfo;
	    }

	    public ServerInfo getInfo()
	    {
	        return serverInfo;
	    }
	    public String toString()
	    {
	        return "Class "+ serverInfo.className +" Address " + serverInfo.address;
	    }
	}

	public ServerInfo(String className, String address, String subTree,
	                  boolean isJava, int watchdogPort,
	                  String startScript, String stopScript)
	{
	    this.active = false;
	    this.className = className;
	    this.address = address;
	    this.subTree = subTree;
	    this.isJava = isJava;
	    this.watchdogPort = watchdogPort;
	    this.startScript = startScript;
	    this.stopScript = stopScript;

	}

	public String getClassName()
	{
	    return className;
	}

	public String getStartScript()
	{
	    return startScript;
	}

	public String getStopScript()
	{
	    return stopScript;
	}

	public String getAddress()
	{
	    return address;
	}

	public String getSubTree()
	{
	    return subTree;
	}

	public int getWarchdogPort()
	{
	    return watchdogPort;
	}

	public boolean isJava()
	{
	    return isJava;
	}

	public boolean isActive()
	{
	    return isJava;
	}

	public void setActive(boolean active)
	{
		if (this.killed && active)
			this.killed = false;
	    this.active = active;
	}

	public MdsMonitorEvent getAction()
	{
	    return monitorEvent;
	}

	public void setAction(MdsMonitorEvent monitorEvent)
	{
	    this.monitorEvent = monitorEvent;
	}

	public int getPos(){return pos;}
	public void setPos(int pos){this.pos=pos;}

	public Object[] getAsArray()
	{
	    Object out[] = new Object[6];
	    out[0] = new Boolean(active);
	    out[1] = className;
	    out[2] = address;
	    out[3] = monitorEvent;

/*-----------------------------------------------------------------------------
	    TableButton b = new TableButton ("Start", this);
	    b.addMouseListener( new MouseAdapter()
	    {
	        public void mouseClicked(MouseEvent e)
	        {
	            ServerInfo si = ((TableButton) e.getSource()).getInfo();
	            System.out.println("ACION Start " + si +" "+ si.getStartScript() );
	            startServer(si);
	        }
	    });
	    out[4] = b;

	    b = new TableButton ("Stop", this);
	    b.addMouseListener( new MouseAdapter()
	    {
	        public void mouseClicked(MouseEvent e)
	        {
	            ServerInfo si = ((TableButton) e.getSource()).getInfo();
	            System.out.println("ACION Stop " + si +" "+ si.getStopScript() );
	            stopServer(si);
	        }
	    });
	    out[5] = b;
------------------------------------------------------------------------------*/
	    return out;
	}

	public void startServer()
	{
		startServer(this);
	}

	private static void startServer(ServerInfo si)
	{
	    try
	    {
	    	if (si.server != null) return;
	    	
	        if( si.getStartScript() != null)
	        {
	            //final Process p = 
	            Runtime.getRuntime().exec(si.getStartScript());
	            // p could be logged here
	        } 
	        else if (si.address.startsWith("localhost:"))
	        {
	        	final int port = Integer.valueOf(si.address.substring(10));
	        	si.server = new jServer(port);
	        	si.server.start();
	        }
	    }
	    catch(IOException exc)
	    {
	        System.out.println("Start " + exc);
	    }
	}

	public void stopServer()
	{
	    stopServer(this);
	}

	public void stopServer(ServerInfo si)
	{
	    try
	    {
	    	if (si.server != null)
	    	{
	    		si.server.stop();
	    		si.server = null;
	    		si.killed = true;
	    	}
	    	else if( si.getStopScript() != null)
	        {
	            Runtime.getRuntime().exec(si.getStopScript());
	        }
	        
	    }
	    catch(IOException exc)
	    {
	        System.out.println("Stop " + exc);
	    }
	}

	public String toString()
	{
	    return "Class :"+ className +"\n"+
	            "Address : "+ address +"\n"+
	            "Sub Tree : "+ subTree +"\n"+
	            "Java Server : "+ isJava +"\n"+
	            "Watchdog port  : "+ watchdogPort +"\n"+
	            "Start Script : "+ startScript +"\n"+
	            "Stop  Script : "+ stopScript +"\n";

	}

    }// End ServerInfo class
