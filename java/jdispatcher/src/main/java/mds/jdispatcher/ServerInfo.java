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
		{ return serverInfo; }

		@Override
		public String toString()
		{
			return "Class " + serverInfo.className + " Address " + serverInfo.address;
		}
	}

	private static void startServer(ServerInfo si)
	{
		try
		{
			if (si.server != null)
				return;
			if (si.getStartScript() != null)
			{
				// final Process p =
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
		catch (final IOException exc)
		{
			System.out.println("Start " + exc);
		}
	}

	private static void showtermServer(ServerInfo si) throws NoSuchFieldException
	{
		try
		{
			if (si.server != null)
				return;
			if (si.getShowtermScript() != null)
			{
				// final Process p =
				Runtime.getRuntime().exec(si.getShowtermScript());
				// p could be logged here
			}
                        else
                        {
                                final String msg = "For server class "+ si.getClassName() +" the showterm_script property value is undefined in properties file";
                                System.out.println(msg);
                                throw new NoSuchFieldException(msg);
                        }
		}
		catch (final Exception exc) 
		{
			System.out.println("For server class "+ si.getClassName() + " exception in show terminal " + exc);
		}
	}

	private static void showlogServer(ServerInfo si) throws NoSuchFieldException
	{
		try
		{
			if (si.server != null)
				return;
			if (si.getShowlogScript() != null)
			{
				// final Process p =
				Runtime.getRuntime().exec(si.getShowlogScript());
				// p could be logged here
			}
                        else
                        {
                               final String msg = "For server class "+ si.getClassName() +" showlog_script property value is undefined in properties file";
                               System.out.println(msg);
                               throw new NoSuchFieldException(msg);
                        }
		}
		catch (final IOException exc)
		{
			System.out.println("For server class "+ si.getClassName() + " exception in show log file " + exc);
		}
	}


	private final String className;
	private final String address;
	private final String subTree;
	private final boolean isJava;
	private final int watchdogPort;
	private final String startScript;
	private final String stopScript;
	private final String showtermScript;
	private final String showlogScript;
	private MdsMonitorEvent monitorEvent;
	private int pos;
	boolean killed = false;
	private jServer server;
	private boolean active;

	public ServerInfo(String className, String address, String subTree, boolean isJava, int watchdogPort,
			String startScript, String stopScript, String showtermScript, String showlogScript)
	{
		this.active = false;
		this.className = className;
		this.address = address;
		this.subTree = subTree;
		this.isJava = isJava;
		this.watchdogPort = watchdogPort;
		this.startScript = startScript;
		this.stopScript = stopScript;
		this.showtermScript = showtermScript;
		this.showlogScript = showlogScript;
	}

	public MdsMonitorEvent getAction()
	{ return monitorEvent; }

	public String getAddress()
	{ return address; }

	public Object[] getAsArray()
	{
		final Object out[] = new Object[6];
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

	public String getClassName()
	{ return className; }

	public int getPos()
	{ return pos; }

	public String getStartScript()
	{ return startScript; }

	public String getStopScript()
	{ return stopScript; }

	public String getShowtermScript()
	{ return showtermScript; }

	public String getShowlogScript()
	{ return showlogScript; }

	public String getSubTree()
	{ return subTree; }

	public int getWarchdogPort()
	{ return watchdogPort; }

	public boolean isActive()
	{ return isJava; }

	public boolean isJava()
	{ return isJava; }

	public void setAction(MdsMonitorEvent monitorEvent)
	{ this.monitorEvent = monitorEvent; }

	public void setActive(boolean active)
	{
		if (this.killed && active)
			this.killed = false;
		this.active = active;
	}

	public void setPos(int pos)
	{ this.pos = pos; }

	public void startServer()
	{
		startServer(this);
	}

	public void stopServer()
	{
		stopServer(this);
	}

	public void showtermServer() throws NoSuchFieldException
	{
		showtermServer(this);
	}

	public void showlogServer() throws NoSuchFieldException
	{
		showlogServer(this);
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
			else if (si.getStopScript() != null)
			{
				Runtime.getRuntime().exec(si.getStopScript());
			}
		}
		catch (final IOException exc)
		{
			System.out.println("Stop " + exc);
		}
	}

	@Override
	public String toString()
	{
		return "Class :" + className + "\n" + "Address : " + address + "\n" + "Sub Tree : " + subTree + "\n"
				+ "Java Server : " + isJava + "\n" + "Watchdog port  : " + watchdogPort + "\n" + "Start Script : "
				+ startScript + "\n" + "Stop  Script : " + stopScript + "\n"
				+ "Open Term Script : " + showtermScript + "\n" + "Open Log  Script : " + showlogScript + "\n";
	}
}// End ServerInfo class
