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
	/*
	 * File structure is prop Title= Signal= XLabel= YLabel= ZLabel= Dimension=
	 * Time=t_start:t_end:dt;....;t_start:t_end:dt or t1,t2,...,tn
	 * Data=y1,y2,y3....,yn X=x1,x2,x3....,xn
	 */
	class SimpleWaveData implements WaveData
	{
		String file_x, file_y;
		int dimension;
		Properties x_prop = new Properties();
		Properties y_prop = new Properties();

		public SimpleWaveData(String in_y)
		{
			file_y = getPathValue(in_y);
			// yPropertiesFile =
			xPropertiesFile = setPropValues(file_y, y_prop);
			x_prop = y_prop;
			file_x = null;
		}

		public SimpleWaveData(String in_y, String in_x)
		{
			file_y = getPathValue(in_y);
			// yPropertiesFile = setPropValues(file_y, y_prop);
			file_x = getPathValue(in_x);
			xPropertiesFile = setPropValues(file_x, x_prop);
		}

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{}

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
		public XYData getData(long xmin, long xmax, int numPoints) throws IOException
		{
			final double x[] = GetXDoubleData();
			final float y[] = GetFloatData();
			return new XYData(x, y, Double.MAX_VALUE);
		}

		@Override
		public void getDataAsync(double lowerBound, double upperBound, int numPoints)
		{}

		private float[] GetFloatData() throws IOException
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

		@Override
		public String GetTitle() throws IOException
		{
			return y_prop.getProperty("Title");
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

		private double[] GetXDoubleData()
		{
			return null;
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
		public float[] getY2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public String GetYLabel() throws IOException
		{
			return y_prop.getProperty("YLabel");
		}

		@Override
		public float[] getZ()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		@Override
		public String GetZLabel() throws IOException
		{
			return y_prop.getProperty("ZLabel");
		}

		private boolean isPropertiesFile(Properties prop)
		{
			final String val = prop.getProperty("Time");
			if (val == null || numElement(val, ",") < 2)
				return false;
			return true;
		}

		@Override
		public boolean isXLong()
		{ return false; }

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

		private int numElement(String val, String separator)
		{
			final StringTokenizer st = new StringTokenizer(val, separator);
			return (st.countTokens());
		}

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{}

		private float[] resizeBuffer(float[] b, int size)
		{
			final float[] newB = new float[size];
			System.arraycopy(b, 0, newB, 0, size);
			return newB;
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
		public boolean supportsStreaming()
		{
			return false;
		}
	}

	public static void main(String args[])
	{
		try (final AsciiDataProvider p = new AsciiDataProvider())
		{
			p.getWaveData("c:\\test.txt", 0, 0, 0);
		}
	}
	private boolean xPropertiesFile = false;
	// private boolean yPropertiesFile = false;
	String error = null;
	String path_exp = null;
	long curr_shot = -1;
	float y[];

	float x[];

	@Override
	public void addConnectionListener(ConnectionListener l)
	{}

	@Override
	public void addUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void close()
	{}

	@Override
	public String getError()
	{ return error; }

	@Override
	public double getFloat(String in, int row, int col, int index)
	{
		error = null;
		return Double.parseDouble(in);
	}

	@Override
	public FrameData getFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
	{
		throw (new IOException("Frames visualization on DemoDataProvider not implemented"));
	}

	@Override
	public long[] getShots(String in, String experiment) throws IOException
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
	public String getString(String in, int row, int col, int index)
	{
		error = null;
		return new String(in);
	}

	@Override
	public WaveData getWaveData(String in, int row, int col, int index)
	{
		return new SimpleWaveData(in);
	}

	@Override
	public WaveData getWaveData(String in_y, String in_x, int row, int col, int index)
	{
		return new SimpleWaveData(in_y, in_x);
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	@Override
	public boolean isBusy()
	{ return false; }

	@Override
	public void removeConnectionListener(ConnectionListener l)
	{}

	@Override
	public void removeUpdateEventListener(UpdateEventListener l, String event)
	{}

	@Override
	public void setArgument(String arg)
	{}

	@Override
	public void setEnvironment(String exp)
	{ error = null; }

	@Override
	public boolean supportsTunneling()
	{
		return false;
	}

	@Override
	public void update(String exp, long s)
	{
		error = null;
		path_exp = exp;
		curr_shot = s;
	}
}
