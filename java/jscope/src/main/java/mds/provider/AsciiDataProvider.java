package mds.provider;

import java.io.*;
import java.util.Properties;
import java.util.StringTokenizer;

import javax.swing.JFrame;

import mds.connection.ConnectionListener;
import mds.connection.UpdateEventListener;
import mds.wave.*;

public class AsciiDataProvider implements DataProvider
{
	private boolean xPropertiesFile = false;
	private boolean yPropertiesFile = false;
	String error = null;
	String path_exp = null;
	long curr_shot = -1;
	float y[];
	float x[];

	/*
	 * File structure estensione prop Title= Signal= XLabel= YLabel= ZLabel=
	 * Dimension= Time=t_start:t_end:dt;....;t_start:t_end:dt or t1,t2,...,tn
	 * Data=y1,y2,y3....,yn X=x1,x2,x3....,xn
	 */
	class SimpleWaveData implements WaveData
	{
		String file_x, file_y;
		int dimension;
		Properties x_prop = new Properties();
		Properties y_prop = new Properties();

		@Override
		public boolean supportsStreaming()
		{
			return false;
		}

		public SimpleWaveData(String in_y)
		{
			file_y = getPathValue(in_y);
			xPropertiesFile = yPropertiesFile = setPropValues(file_y, y_prop);
			x_prop = y_prop;
			file_x = null;
		}

		public SimpleWaveData(String in_y, String in_x)
		{
			file_y = getPathValue(in_y);
			yPropertiesFile = setPropValues(file_y, y_prop);
			file_x = getPathValue(in_x);
			xPropertiesFile = setPropValues(file_x, x_prop);
		}

		private String getPathValue(String in)
		{
			String out = "";
			if (path_exp != null)
				out = path_exp;
			if (curr_shot > 0)
				out = out + File.separatorChar + curr_shot;
			if (out != null && out.length() > 0)
				out = out + File.separatorChar + in;
			else
				out = in;
			return out;
		}

		private int numElement(String val, String separator)
		{
			final StringTokenizer st = new StringTokenizer(val, separator);
			return (st.countTokens());
		}

		private boolean isPropertiesFile(Properties prop)
		{
			final String val = prop.getProperty("Time");
			if (val == null || numElement(val, ",") < 2)
				return false;
			return true;
		}

		private float[] resizeBuffer(float[] b, int size)
		{
			final float[] newB = new float[size];
			System.arraycopy(b, 0, newB, 0, size);
			return newB;
		}

		private void loadSignalValues(String in) throws Exception
		{
			final BufferedReader bufR = new BufferedReader(new FileReader(in));
			String ln;
			StringTokenizer st;
			while ((ln = bufR.readLine()) != null)
			{
				st = new StringTokenizer(ln);
				final int numColumn = st.countTokens();
				if (numColumn == 2 && st.nextToken().equals("Time") && st.nextToken().equals("Data"))
				{
					x = new float[1000];
					y = new float[1000];
					int count = 0;
					int maxCount = 1000;
					while ((ln = bufR.readLine()) != null)
					{
						st = new StringTokenizer(ln);
						if (count == maxCount)
						{
							x = resizeBuffer(x, x.length + 1000);
							y = resizeBuffer(y, y.length + 1000);
							maxCount = y.length;
						}
						x[count] = Float.parseFloat(st.nextToken());
						y[count] = Float.parseFloat(st.nextToken());
						count++;
					}
					x = resizeBuffer(x, count);
					y = resizeBuffer(y, count);
				}
			}
			bufR.close();
			if (x == null || y == null)
				throw (new Exception("No data in file or file syntax error"));
		}

		private boolean setPropValues(String in, Properties prop)
		{
			boolean propertiesFile = false;
			try
			{
				prop.load(new FileInputStream(in));
				propertiesFile = isPropertiesFile(prop);
				if (!propertiesFile)
				{
					loadSignalValues(in);
				}
			}
			catch (final Exception exc)
			{
				error = "File " + in + " error : " + exc.getMessage();
			}
			return false;
		}

		@Override
		public int getNumDimension() throws IOException
		{
			try
			{
				dimension = Integer.parseInt(y_prop.getProperty("Dimension"));
				return dimension;
			}
			catch (final NumberFormatException exc)
			{
				return (dimension = 1);
			}
		}

		public float[] GetFloatData() throws IOException
		{
			if (xPropertiesFile)
				return decodeValues(x_prop.getProperty("Data"));
			else
			{
				if (y == null)
					throw (new IOException(error));
				return y;
			}
		}

		public double[] GetXDoubleData()
		{
			return null;
		}

		public long[] GetXLongData()
		{
			return null;
		}

		public float[] GetXData() throws IOException
		{
			if (file_x == null)
				if (yPropertiesFile)
					return decodeTimes(y_prop.getProperty("Time"));
				else
				{
					if (x == null)
						throw (new IOException(error));
					return x;
				}
			else if (xPropertiesFile)
				return decodeValues(x_prop.getProperty("Data"));
			else
			{
				if (y == null)
					throw (new IOException(error));
				return y;
			}
		}

		public float[] GetYData() throws IOException
		{
			if (xPropertiesFile)
				return decodeValues(x_prop.getProperty("X"));
			else
			{
				error = "2D signal in column ASCII file format not yet supported";
				return null;
				// throw( new IOException(error) );
			}
		}

		@Override
		public String GetTitle() throws IOException
		{
			return y_prop.getProperty("Title");
		}

		@Override
		public String GetXLabel() throws IOException
		{
			if (file_x == null)
				return y_prop.getProperty("XLabel");
			else
				return x_prop.getProperty("YLabel");
		}

		@Override
		public String GetYLabel() throws IOException
		{
			return y_prop.getProperty("YLabel");
		}

		@Override
		public String GetZLabel() throws IOException
		{
			return y_prop.getProperty("ZLabel");
		}

		private float[] decodeValues(String val)
		{
			if (val == null)
			{
				error = "File syntax error";
				return null;
			}
			final StringTokenizer st = new StringTokenizer(val, ",");
			final int num = st.countTokens();
			float out[] = new float[num];
			String d_st;
			int i = 0;
			try
			{
				while (st.hasMoreElements())
				{
					d_st = st.nextToken().trim();
					out[i++] = Float.parseFloat(d_st);
				}
			}
			catch (final NumberFormatException exc)
			{
				error = "File syntax error : " + exc.getMessage();
				out = null;
			}
			return out;
		}

		private float[] decodeTimes(String val)
		{
			float out[] = null;
			StringTokenizer st;
			try
			{
				if (val == null)
				{
					error = "File syntax error";
					return null;
				}
				st = new StringTokenizer(val, ":");
				if (st.countTokens() > 1)
				{
					st = new StringTokenizer(val, ";");
					final int num_base = st.countTokens();
					final ByteArrayOutputStream aos = new ByteArrayOutputStream();
					final DataOutputStream dos = new DataOutputStream(aos);
					String time_st;
					for (int i = 0; i < num_base; i++)
					{
						time_st = st.nextToken();
						final StringTokenizer st1 = new StringTokenizer(time_st, ":");
						final float start = Float.parseFloat(st1.nextToken().trim());
						final float end = Float.parseFloat(st1.nextToken().trim());
						final float dt = Float.parseFloat(st1.nextToken().trim());
						for (float t = start; t <= end; t += dt)
							dos.writeFloat(t);
					}
					out = byteArrayToFloat(aos.toByteArray());
				}
				else
				{
					out = decodeValues(val);
				}
			}
			catch (final Exception exc)
			{
				error = "File sintax error : " + exc.getMessage();
				out = null;
			}
			return out;
		}

		private float[] byteArrayToFloat(byte a[])
		{
			final int size = a.length / 4;
			float out[] = new float[size];
			final DataInputStream dis = new DataInputStream(new ByteArrayInputStream(a));
			try
			{
				for (int i = 0; i < size; i++)
					out[i] = dis.readFloat();
				dis.close();
			}
			catch (final Exception exc)
			{
				error = "File sintax error : " + exc.getMessage();
				out = null;
			}
			return out;
		}
		// GAB JULY 2014 NEW WAVEDATA INTERFACE RAFFAZZONATA

		@Override
		public XYData getData(long xmin, long xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(double xmin, double xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public XYData getData(int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public float[] getZ()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public double[] getX2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public long[] getX2DLong()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public float[] getY2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		public double[] getXLimits()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		public long[] getXLong()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public boolean isXLong()
		{ return false; }

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public void getDataAsync(double lowerBound, double upperBound, int numPoints)
		{}
	}

	@Override
	public WaveData GetWaveData(String in, int row, int col, int index)
	{
		return new SimpleWaveData(in);
	}

	@Override
	public WaveData GetWaveData(String in_y, String in_x, int row, int col, int index)
	{
		return new SimpleWaveData(in_y, in_x);
	}

	public WaveData GetResampledWaveData(String in, double start, double end, int n_points)
	{
		return null;
	}

	public WaveData GetResampledWaveData(String in_y, String in_x, double start, double end, int n_points)
	{
		return null;
	}

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

	public boolean DataPending()
	{
		return false;
	}

	@Override
	public void SetEnvironment(String exp)
	{
		error = null;
	}

	@Override
	public void Update(String exp, long s)
	{
		error = null;
		path_exp = exp;
		curr_shot = s;
	}

	@Override
	public String GetString(String in, int row, int col, int index)
	{
		error = null;
		return new String(in);
	}

	@Override
	public double GetFloat(String in, int row, int col, int index)
	{
		error = null;
		return Double.parseDouble(in);
	}

	@Override
	public long[] GetShots(String in, String experiment) throws IOException
	{
		error = null;
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
						result[i++] = Long.parseLong(st.nextToken());
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
		error = "Error parsing shot number(s)";
		throw (new IOException(error));
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
		throw (new IOException("Frames visualization on DemoDataProvider not implemented"));
	}

	public float[] GetFrameTimes(String in_expr)
	{
		int cnt = 0;
		String n;
		File f;
		float[] out = null;
		String in, ext;
		in = in_expr.substring(0, in_expr.indexOf("."));
		ext = in_expr.substring(in_expr.indexOf("."), in_expr.length());
		for (int i = 0; i < 100; i++)
		{
			if (i < 10)
				n = in + "_00" + (i) + ext;
			else
				n = in + "_0" + (i) + ext;
			f = new File(n);
			if (f.exists())
				cnt++;
		}
		if (cnt != 0)
		{
			out = new float[cnt];
			for (int i = 1; i < out.length; i++)
				out[i] += out[i - 1] + 1;
		}
		return out;
	}

	public byte[] GetAllFrames(String in_frame)
	{
		return null;
	}

	public byte[] GetFrameAt(String in_expr, int frame_idx)
	{
		String n;
		byte buf[] = null;
		long size = 0;
		final int i = frame_idx;
		String in, ext;
		in = in_expr.substring(0, in_expr.indexOf("."));
		ext = in_expr.substring(in_expr.indexOf("."), in_expr.length());
		if (i < 10)
			n = in + "_00" + (i) + ext;
		else
			n = in + "_0" + (i) + ext;
		final File f = new File(n);
		if (f.exists())
		{
			System.out.println("Esiste " + n);
			try
			{
				final FileInputStream bin = new FileInputStream(n);
				size = f.length();
				buf = new byte[(int) size];
				if (buf != null)
					bin.read(buf);
				bin.close();
			}
			catch (final IOException e)
			{}
		}
		else
		{
			System.out.println("Non Esiste " + n);
		}
		return buf;
	}

	public void enableAsyncUpdate(boolean enable)
	{}

	public void getDataAsync(double lowerBound, double upperBound, double resolution)
	{}

	public static void main(String args[])
	{
		final AsciiDataProvider p = new AsciiDataProvider();
		p.GetWaveData("c:\\test.txt", 0, 0, 0);
	}

	@Override
	public boolean isBusy()
	{ return false; }
}
