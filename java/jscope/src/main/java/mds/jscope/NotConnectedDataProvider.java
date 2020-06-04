package mds.jscope;

import java.io.*;
import javax.swing.JFrame;

import mds.connection.*;
import mds.wavedisplay.*;


class NotConnectedDataProvider implements DataProvider
{
    final String error = "Not Connected";


    public WaveData GetWaveData(String in, int row, int col, int index)
    {
	return null;
    }
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
    public void enableAsyncUpdate(boolean enable){}

    public void    Dispose(){}
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public boolean SupportsContinuous() { return false; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg){}
    public boolean SupportsTunneling() {return false; }

    public void SetEnvironment(String exp)
    {
    }

    public void Update(String exp, long s)
    {
    }

    public String GetString(String in, int row, int col, int index)
    {
	return "";
    }

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

    public long[] GetShots(String in, String experiment)
    {
	long d[] = new long[1];
	d[0] = 0;
	return d;
    }

    public String ErrorString()
    {
	return error;
    }

    public void AddUpdateEventListener(UpdateEventListener l, String event)
    {
    }

    public void RemoveUpdateEventListener(UpdateEventListener l, String event)
    {
    }

    public void AddConnectionListener(ConnectionListener l)
    {
    }

    public void RemoveConnectionListener(ConnectionListener l)
    {
    }


    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
	return null;
    }

    public boolean DataPending()
    {
	return false;
    }
	@Override
	public boolean isBusy() {return false;}
}
