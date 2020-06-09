package mds.jscope;

import java.io.IOException;

import javax.swing.JFrame;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;
import mds.wavedisplay.*;

public class NotConnectedDataProvider implements DataProvider
{
	final String error = "Not Connected";

	@Override
	public WaveData GetWaveData(String in, int row, int col, int index)
	{
		return null;
	}

	@Override
	public WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
	{
		return null;
	}

	public WaveData GetResampledWaveData(String in, double start, double end, int n_points)
	{
		return null;
	}

	public WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
	{
		return null;
	}

	public void enableAsyncUpdate(boolean enable)
	{}

	@Override
	public void Dispose()
	{}

	public boolean SupportsCompression()
	{
		return false;
	}

	public void SetCompression(boolean state)
	{}

	public boolean SupportsContinuous()
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
		return false;
	}

	@Override
	public void SetArgument(String arg)
	{}

	@Override
	public boolean SupportsTunneling()
	{
		return false;
	}

	@Override
	public void SetEnvironment(String exp)
	{}

	@Override
	public void Update(String exp, long s)
	{}

	@Override
	public String GetString(String in, int row, int col, int index)
	{
		return "";
	}

	@Override
	public double GetFloat(String in, int row, int col, int index)
	{
		return Double.parseDouble(in);
	}

	public float[] GetFloatArray(String in_x, String in_y, float start, float end)
	{
		return null;
	}

	public float[] GetFloatArray(String in)
	{
		return null;
	}

	@Override
	public long[] GetShots(String in, String experiment)
	{
		final long d[] = new long[1];
		d[0] = 0;
		return d;
	}

	@Override
	public String ErrorString()
	{
		return error;
	}

	@Override
	public void AddUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void RemoveUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void AddConnectionListener(ConnectionListener l)
	{}

	@Override
	public void RemoveConnectionListener(ConnectionListener l)
	{}

	@Override
	public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		return null;
	}

	public boolean DataPending()
	{
		return false;
	}

	@Override
	public boolean isBusy()
	{ return false; }
}
