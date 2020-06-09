package mds.provider;

// -------------------------------------------------------------------------------------------------
// TwuDataProvider
// An implementation of "DataProvider" for signals from a TEC Web-Umbrella (TWU) server.
//
// The first versions of this class (cvs revisions 1.x) were designed and written
// by Gabriele Manduchi and with some minor hacks by Jon Krom.
// Marco van de Giessen <A.P.M.vandeGiessen@phys.uu.nl> did some major surgery on
// this class (starting revision line 2.x) mainly in order to ensure that zooming
// works in more situations. (See also the cvs log.)
//
// Most of this revision 2.x work has now, from 2.21 onwards, been moved into separate classes.
// No new major revision number was started; has little use in CVS.
//
//
// -------------------------------------------------------------------------------------------------
import java.io.IOException;
import java.util.StringTokenizer;
import java.util.Vector;

import javax.swing.JFrame;

import mds.connection.*;
import mds.provider.twu.*;
import mds.wavedisplay.*;

public class TwuDataProvider implements DataProvider
{
	private String experiment;
	private long shot;
	private String error_string;
	private transient Vector<ConnectionListener> connection_listener = new Vector<>();
	private TwuWaveData lastWaveData = null;
	private String user_agent;

	public String getUserAgent()
	{ return user_agent; }

	public long getShot()
	{ return shot; }

	// DataProvider implementation
	public boolean SupportsCompression()
	{
		return false;
	}

	public void SetCompression(boolean state)
	{}

	@Override
	public void SetEnvironment(String s)
	{}

	@Override
	public void Dispose()
	{}

	@Override
	public String GetString(String in, int row, int col, int index)
	{
		return in;
	}

	@Override
	public double GetFloat(String in, int row, int col, int index)
	{
		return Double.parseDouble(in);
	}

	@Override
	public String ErrorString()
	{
		return error_string;
	}

	@Override
	public void AddUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void RemoveUpdateEventListener(UpdateEventListener l, String event)
	{}

	public boolean SupportsContinuous()
	{
		return false;
	}

	public boolean DataPending()
	{
		return false;
	}

	@Override
	public int InquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	public boolean SupportsFastNetwork()
	{
		return true;
	}

	@Override
	public void SetArgument(String arg)
	{}

	@Override
	public boolean SupportsTunneling()
	{
		return false;
	}

	// --------------------------------------------------------------------------------------------
	// interface methods for getting *Data objects
	// ---------------------------------------------------
	public void enableAsyncUpdate(boolean enable)
	{}

	@Override
	public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		return (new TwuSimpleFrameData(this, in_y, in_x, time_min, time_max));
	}

	// ---------------------------------------------------
	@Override
	public synchronized WaveData GetWaveData(String in, int row, int col, int index)
	{
		return GetWaveData(in, null, 0, 0, 0);
	}

	@Override
	public synchronized WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
	{
		final TwuWaveData find = FindWaveData(in_y, in_x);
		find.setFullFetch();
		return find;
	}

	public synchronized WaveData GetResampledWaveData(String in, double start, double end, int n_points)
	{
		return GetResampledWaveData(in, null, start, end, n_points);
	}

	public synchronized WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
	{
		final TwuWaveData find = FindWaveData(in_y, in_x);
		find.setZoom((float) start, (float) end, n_points);
		return find;
	}

	public synchronized TwuWaveData FindWaveData(String in_y, String in_x)
	{
		if (lastWaveData == null || lastWaveData.notEqualsInputSignal(in_y, in_x, getShot()))
		{
			lastWaveData = new TwuWaveData(this, in_y, in_x);
			try
			{
				// Ensure that the properties are fetched right away.
				lastWaveData.getTwuProperties();
			}
			catch (final IOException e)
			{
				setErrorstring("No Such Signal : " + TwuNameServices.GetSignalPath(in_y, getShot()));
				// throw new IOException ("No Such Signal");
			}
		}
		return lastWaveData;
	}

	// ---------------------------------------------------------------------------------------------
	public synchronized float[] GetFloatArray(String in)
	{
		resetErrorstring(null);
		if (in.startsWith("TIME:", 0))
			in = in.substring(5);
		final TwuWaveData wd = (TwuWaveData) GetWaveData(in, 0, 0, 0);
		float[] data = null;
		try
		{
//            data = wd.GetFloatData() ;
			data = wd.getData(4000).y;
		}
		catch (final Exception e)
		{
			resetErrorstring(e.toString());
			data = null;
		}
		return data;
	}

	// ----------------------------------------------------
	// Methods for TwuAccess.
	// ----------------------------------------------------
	public synchronized float[] GetFloatArray(String in, boolean is_time) throws IOException
	{
		final TwuWaveData wd = (TwuWaveData) GetWaveData(in, 0, 0, 0); // TwuAccess wants to get the full signal data .
		return is_time ? wd.GetXData() : wd.GetYData();
	}

	public synchronized String GetSignalProperty(String prop, String in) throws IOException
	{
		final TwuWaveData wd = (TwuWaveData) GetWaveData(in, 0, 0, 0);
		return wd.getTwuProperties().getProperty(prop);
	}

	// -------------------------------------------------------
	// parsing of / extraction from input signal string
	// -------------------------------------------------------
	@Override
	public long[] GetShots(String in, String experiment)
	{
		resetErrorstring(null);
		long[] result;
		String curr_in = in.trim();
		if (curr_in.startsWith("[", 0))
		{
			if (curr_in.endsWith("]"))
			{
				curr_in = curr_in.substring(1, curr_in.length() - 1);
				final StringTokenizer st = new StringTokenizer(curr_in, ",", false);
				result = new long[st.countTokens()];
				int i = 0;
				try
				{
					while (st.hasMoreTokens())
						result[i++] = Integer.parseInt(st.nextToken());
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		else
		{
			if (curr_in.indexOf(":") != -1)
			{
				final StringTokenizer st = new StringTokenizer(curr_in, ":");
				int start, end;
				if (st.countTokens() == 2)
				{
					try
					{
						start = Integer.parseInt(st.nextToken());
						end = Integer.parseInt(st.nextToken());
						if (end < start)
							end = start;
						result = new long[end - start + 1];
						for (int i = 0; i < end - start + 1; i++)
							result[i] = start + i;
						return result;
					}
					catch (final Exception e)
					{}
				}
			}
			else
			{
				result = new long[1];
				try
				{
					result[0] = Long.parseLong(curr_in);
					return result;
				}
				catch (final Exception e)
				{}
			}
		}
		resetErrorstring("Error parsing shot number(s)");
		return null;
	}

	// -------------------------------------------
	// connection handling methods ....
	// -------------------------------------------
	@Override
	public synchronized void AddConnectionListener(ConnectionListener l)
	{
		if (l == null)
			return;
		connection_listener.addElement(l);
	}

	@Override
	public synchronized void RemoveConnectionListener(ConnectionListener l)
	{
		if (l == null)
			return;
		connection_listener.removeElement(l);
	}

	public void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (connection_listener != null)
			for (int i = 0; i < connection_listener.size(); i++)
				connection_listener.elementAt(i).processConnectionEvent(e);
	}

	// -------------------------------------------
	// Constructor, other small stuff ...
	// -------------------------------------------
	@Override
	public synchronized void Update(String experiment, long shot)
	{
		this.experiment = experiment;
		this.shot = shot;
		resetErrorstring(null);
		lastWaveData = null;
	}

	public synchronized String getExperiment()
	{ return experiment; }

	public synchronized void setErrorstring(String newErrStr)
	{
		if (error_string == null)
			error_string = newErrStr;
	}

	protected synchronized void resetErrorstring(String newErrStr)
	{
		error_string = newErrStr;
	}

	public TwuDataProvider()
	{
		super();
	}

	public static String revision()
	{
		return "$Id$";
	}

	public TwuDataProvider(String user_agent)
	{
		this.user_agent = user_agent;
		// Could be used in the constructor for TwuProperties and in similar get URL
		// actions.
		// A site could used this as a possible (internal) software distribution
		// management
		// tool. In the log of a web-server you can, by checking the user_agent, see
		// which
		// machines are still running old software.
	}

	@Override
	public boolean isBusy()
	{ return false; }
}
