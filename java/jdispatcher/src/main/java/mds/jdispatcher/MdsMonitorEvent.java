package mds.jdispatcher;

import java.util.*;
import java.text.*;

class MdsMonitorEvent extends MdsServerEvent
{
	private static final long serialVersionUID = 1L;
	static final int MonitorBeginSequence = 1;
	static final int MonitorBuildBegin = 2;
	static final int MonitorBuild = 3;
	static final int MonitorBuildEnd = 4;
	static final int MonitorCheckin = 5;
	static final int MonitorDispatched = 6;
	static final int MonitorDoing = 7;
	static final int MonitorDone = 8;
	static final int MonitorEndPhase = 9;
	static final int MonitorStartPhase = 10;
	static final int MonitorEndSequence = 11;
	static final int MonitorServerConnected = 12;
	static final int MonitorServerDisconnected = 13;

	public static String msecToString(long msec)
	{
		final int min = (int) (msec / 60000.);
		msec -= min * 60000;
		final int sec = (int) (msec / 1000.);
		msec -= sec * 1000;
		final DecimalFormat df = new DecimalFormat("#00");
		final DecimalFormat df1 = new DecimalFormat("#000");
		return df.format(min) + "." + df.format(sec) + "." + df1.format(msec);
	}

	/*
	 * static final int MonitorServerConnected = 10; static final int
	 * MonitorServerDisconnected = 11; static final int MonitorEndPhase = 12; static
	 * final int MonitorStartPhase = 13; static final int MonitorEndSequence = 9;
	 */
	String tree;
	long shot;
	int phase;
	int nid;
	String name;
	int on;
	int mode;
	String server;
	String server_address;
	int ret_status;
	String node_path;
	Date date;
	String date_st;
	String error_message;
	long execution_time; // msec

	public MdsMonitorEvent(Object source, int id, int flags, int status, String data) throws Exception
	{
		super(source, id, flags, status);
		date = new Date();
		try
		{
			final StringTokenizer buf = new StringTokenizer(data);
			tree = new String(buf.nextToken());
			if (buf.hasMoreTokens())
				shot = Long.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				phase = Integer.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				nid = Integer.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				on = Integer.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				mode = Integer.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				server = new String(buf.nextToken());
			if (buf.hasMoreTokens())
				server_address = new String(buf.nextToken());
			if (buf.hasMoreTokens())
				ret_status = Integer.decode(buf.nextToken()).intValue();
			if (buf.hasMoreTokens())
				node_path = new String(buf.nextToken());
			if (buf.hasMoreTokens())
				date_st = new String(buf.nextToken(";"));
			if (buf.hasMoreTokens())
			{
				error_message = new String(buf.nextToken(""));
				error_message = error_message.substring(1, error_message.length());
			}
		}
		catch (final NumberFormatException e)
		{
			System.out.println(data);
			e.printStackTrace();
			throw (new Exception("Bad monitor event data" + e));
		}
	}

	public MdsMonitorEvent(Object obj, int phase, int nid, String msg)
	{
		super(obj, 0, 0, 1);
		this.tree = null;
		this.shot = 0;
		this.phase = phase;
		this.nid = nid;
		this.error_message = msg;
	}

	public MdsMonitorEvent(Object obj, int mode, String msg)
	{
		super(obj, 0, 0, 1);
		this.mode = mode;
		this.error_message = msg;
	}

	public MdsMonitorEvent(Object obj, String tree, int shot, int phase, int nid, String name, int on, int mode,
			String server, String server_address, int ret_status)
	{
		super(obj, 0, 0, 1);
		this.tree = tree;
		this.shot = shot;
		this.phase = phase;
		this.nid = nid;
		this.name = name;
		this.on = on;
		this.mode = mode;
		this.server = server;
		this.server_address = server_address;
		this.ret_status = ret_status;
		this.error_message = MdsHelper.getErrorString(ret_status);
		date_st = (new Date()).toString();
	}

	private String getMode(int mode_id)
	{
		switch (mode_id)
		{
		case MonitorBeginSequence:
			return "MonitorBeginSequence";
		case MonitorBuildBegin:
			return "MonitorBuildBegin";
		case MonitorBuild:
			return "MonitorBuild";
		case MonitorBuildEnd:
			return "MonitorBuildEnd";
		case MonitorCheckin:
			return "MonitorCheckin";
		case MonitorDispatched:
			return "MonitorDispatched";
		case MonitorDoing:
			return "MonitorDoing";
		case MonitorDone:
			return "MonitorDone";
		case MonitorServerConnected:
			return "MonitorServerConnected";
		case MonitorServerDisconnected:
			return "MonitorServerDisconnected";
		case MonitorEndPhase:
			return "MonitorEndPhase";
		case MonitorStartPhase:
			return "MonitorStartPhase";
		case MonitorEndSequence:
			return "MonitorEndSequence";
		}
		return "";
	}

	public synchronized String getMonitorString()
	{
		final StringBuffer out = new StringBuffer();
		if (mode == MonitorDone)
			out.append("[" + msecToString(execution_time) + "] " + date_st + ", ");
		else
			out.append(date_st + ", ");
		switch (mode)
		{
		case MonitorBuildBegin:
		case MonitorBuild:
		case MonitorBuildEnd:
			final String on_off = (on == 1) ? "ON" : "OFF";
			out.append(" Action " + on_off + " node " + node_path);
			break;
		case MonitorDispatched:
			out.append(" Dispatching node " + node_path + "(" + nid + ")" + " to " + server);
			break;
		case MonitorDoing:
			out.append(" Doing " + node_path + " in " + tree + " shot " + shot + " on " + server);
			break;
		case MonitorDone:
			if ((ret_status & 1) == 1)
				out.append(" Done " + node_path + " in " + tree + " shot " + shot + " status " + ret_status + " on "
						+ server);
			else
				out.append(" Failed " + node_path + " in " + tree + " shot " + shot + " status " + ret_status + " on "
						+ server + " " + error_message);
			break;
		}
		return out.toString().trim();
	}

	public synchronized byte[] toBytes()
	{
		final String out_st = tree + " " + shot + " " + phase + " " + nid + " " + on + " " + mode + " " + server + " "
				+ server_address + " " + ret_status + " " + name + " " + date_st + " ; " + error_message;
		final byte[] msg = out_st.getBytes();
		final String head_st = "" + jobid + " " + flags + " " + status + " " + msg.length;
		final byte[] headmsg = head_st.getBytes();
		final byte[] outmsg = new byte[60 + msg.length];
		System.arraycopy(headmsg, 0, outmsg, 0, headmsg.length);
		System.arraycopy(msg, 0, outmsg, 60, msg.length);
		return outmsg;
	}

	@Override
	public synchronized String toString()
	{
		return "[exp=" + tree + "; shot=" + shot + "; phase=" + phase + "; nid=" + nid + "; on=" + on + "; mode= "
				+ getMode(mode) + "; server=" + server + "; ServerAddress=" + server_address + "; status=" + ret_status
				+ "]";
	}
}
