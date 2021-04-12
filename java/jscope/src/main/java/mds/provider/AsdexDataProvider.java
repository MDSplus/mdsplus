package mds.provider;

import java.io.IOException;
import java.util.StringTokenizer;

import javax.swing.JFrame;

import mds.wave.*;

public class AsdexDataProvider extends MdsDataProvider
{
	class SimpleWaveData implements WaveData
	{
		String in_x, in_y;
		float xmax, xmin;
		int n_points;
		boolean resample = false;
		boolean _jscope_set = false;
		int v_idx;

		RealArray currXData = null;

		public SimpleWaveData(String in_y)
		{
			this.in_y = in_y;
			v_idx = var_idx;
		}

		public SimpleWaveData(String in_y, float xmin, float xmax, int n_points)
		{
			resample = true;
			this.in_y = in_y;
			this.xmin = xmin;
			this.xmax = xmax;
			this.n_points = n_points;
			v_idx = var_idx;
		}

		public SimpleWaveData(String in_y, String in_x)
		{
			this.in_y = in_y;
			this.in_x = in_x;
			v_idx = var_idx;
		}

		public SimpleWaveData(String in_y, String in_x, float xmin, float xmax, int n_points)
		{
			resample = true;
			this.in_y = in_y;
			this.in_x = in_x;
			this.xmin = xmin;
			this.xmax = xmax;
			this.n_points = n_points;
			v_idx = var_idx;
		}

		@Override
		public void addWaveDataListener(WaveDataListener listener)
		{}

		private double[] encodeTimeBase(String expr)
		{
			try
			{
				final int startIdx[] = getIntArray("begin_of(window_of(dim_of(" + expr + ")))");
				final int endIdx[] = getIntArray("end_of(window_of(dim_of(" + expr + ")))");
				if (startIdx.length != 1 || endIdx.length != 1)
					return null;
				final int numPoint = endIdx[0] - startIdx[0] + 1;
				final double delta[] = GetDoubleArray("slope_of(axis_of(dim_of(" + expr + ")))");
				double begin[] = null;
				double end[] = null;
				double curr;
				final double firstTime[] = GetDoubleArray("i_to_x(dim_of(" + expr + ")," + startIdx[0] + ")");
				try
				{
					begin = GetDoubleArray("begin_of(axis_of(dim_of(" + expr + ")))");
					end = GetDoubleArray("end_of(axis_of(dim_of(" + expr + ")))");
				}
				catch (final IOException e)
				{}
				if (delta.length == 1 && numPoint > 1)
				{
					int i, j;
					final double out[] = new double[numPoint];
					for (i = j = 0, curr = firstTime[0]; i < numPoint; i++, j++)
						out[i] = curr + j * delta[0];
					return out;
				}
				if (delta.length > 1 && numPoint > 1)
				{
					int i, j, idx;
					final double out[] = new double[numPoint];
					for (i = j = 0, idx = 0, curr = firstTime[0]; i < numPoint; i++, j++)
					{
						out[i] = curr + j * delta[idx];
						if (out[i] > end[idx])
						{
							out[i] = end[idx];
							idx++;
							curr = begin[idx];
							j = 0;
						}
					}
					return out;
				}
			}
			catch (final Exception exc)
			{} // System.out.println(exc.getMessage());}
			return null;
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

		public float[] GetFloatData() throws IOException
		{
			String in_y;
			in_y = ParseExpression(this.in_y);
			// _jscope_set = true;
			final String in_y_expr = "_jscope_" + v_idx;
			String set_tdivar = "";
			if (!_jscope_set)
			{
				_jscope_set = true;
				set_tdivar = "_jscope_" + v_idx + " = (" + in_y + "), ";
				var_idx++;
			}
			if (resample && in_x == null)
			{
				final String limits = "FLOAT(" + xmin + "), " + "FLOAT(" + xmax + ")";
				// String expr = "JavaResample("+ "FLOAT("+in_y+ "), "+
				// "FLOAT(DIM_OF("+in_y+")), "+ limits + ")";
				final String resampledExpr = "JavaResample(" + "FLOAT(" + in_y_expr + "), " + "FLOAT(DIM_OF("
						+ in_y_expr + ")), " + limits + ")";
				set_tdivar = "_jscope_" + v_idx + " = (" + resampledExpr + "), ";
				// String expr = set_tdivar + "fs_float("+resampledExpr+ ")";
				final String expr = set_tdivar + "fs_float(_jscope_" + v_idx + ")";
				return GetFloatArray(expr);
			}
			else
				return GetFloatArray(set_tdivar + "fs_float(" + in_y_expr + ")");
		}

		@Override
		public int getNumDimension() throws IOException
		{
			String expr;
			if (_jscope_set)
				expr = "shape(_jscope_" + v_idx + ")";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), shape(_jscope_" + v_idx + "))";
				var_idx++;
			}
			final int shape[] = getNumDimensions(expr);
			if (error != null)
			{
				_jscope_set = false;
				error = null;
				return 1;
			}
			return shape.length;
		}

		@Override
		public String GetTitle() throws IOException
		{
			String expr;
			if (_jscope_set)
				expr = "help_of(_jscope_" + v_idx + ")";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), help_of(_jscope_" + v_idx + "))";
				var_idx++;
			}
			final String out = getStringValue(expr);
			if (out == null)
				_jscope_set = false;
			return out;
			// return GetDefaultTitle(in_y);
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

		public float[] GetXData()
		{
			try
			{
				if (currXData == null)
					currXData = GetXRealData();
				return currXData.getFloatArray();
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		public double[] GetXDoubleData()
		{
			try
			{
				if (currXData == null)
					currXData = GetXRealData();
				if (!currXData.isDouble())
					return null;
				return currXData.getDoubleArray();
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		@Override
		public String GetXLabel() throws IOException
		{
			String out = null;
			if (in_x == null || in_x.length() == 0)
			{
				String expr;
				if (_jscope_set)
					expr = "Units(dim_of(_jscope_" + v_idx + ", 1))";
				else
				{
					_jscope_set = true;
					expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ", 1)))";
					var_idx++;
				}
				out = getStringValue(expr);
				// return GetDefaultXLabel(in_y);
			}
			else
			{
				/*
				 * String expr; if(_jscope_set) expr = "Units(_jscope_"+v_idx+")"; else {
				 * _jscope_set = true; expr =
				 * "( _jscope_"+v_idx+" = ("+in_x+"), Units(_jscope_"+v_idx+")"; var_idx++; }
				 * return GetDefaultYLabel(expr);
				 */
				out = getStringValue("Units(" + in_x + ")");
			}
			if (out == null)
				_jscope_set = false;
			return out;
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

		public long[] GetXLongData()
		{
			try
			{
				if (currXData == null)
					currXData = GetXRealData();
				if (!currXData.isLong())
					return null;
				return currXData.getLongArray();
			}
			catch (final Exception exc)
			{
				return null;
			}
		}

		RealArray GetXRealData() throws IOException
		{
			String expr = null;
			double tBaseOut[] = null;
			if (in_x == null)
			{
				if (_jscope_set)
				{
					expr = "dim_of(_jscope_" + v_idx + ")";
					tBaseOut = encodeTimeBase("_jscope_" + v_idx);
					// expr = "JavaDim(dim_of(_jscope_"+v_idx + "), FLOAT("+(-Float.MAX_VALUE)+"), "
					// + "FLOAT("+Float.MAX_VALUE+"))";
					// isCoded = true;
				}
				else
				{
					_jscope_set = true;
					final String in_y_expr = "_jscope_" + v_idx;
					final String set_tdivar = "( _jscope_" + v_idx + " = (" + in_y + "), ";
					var_idx++;
					if (resample)
					{
						final String limits = "FLOAT(" + xmin + "), " + "FLOAT(" + xmax + ")";
						// expr = "DIM_OF(JavaResample("+ "FLOAT("+in_y+ "), "+
						// "FLOAT(DIM_OF("+in_y+")), "+ limits + "))";
						expr = set_tdivar + "JavaResample(" + "FLOAT(" + in_y_expr + "), " + "FLOAT(DIM_OF(" + in_y_expr
								+ ")), " + limits + ")";
					}
					else
					{
						// expr = "dim_of("+in_y+")";
						expr = set_tdivar + "dim_of(" + in_y_expr + ")";
						tBaseOut = encodeTimeBase(in_y);
						// expr = "JavaDim(dim_of("+in_y+"), FLOAT("+(-Float.MAX_VALUE)+"), " +
						// "FLOAT("+Float.MAX_VALUE+"))";
						// isCoded = true;
					}
				}
				if (tBaseOut != null)
					return new RealArray(tBaseOut);
				else
					return GetRealArray(expr);
			}
			else
				return GetRealArray(in_x);
		}

		@Override
		public float[] getY2D()
		{
			System.out.println("BADABUM!!");
			return null;
		}

		public float[] GetYData() throws IOException
		{
			String expr;
			if (_jscope_set)
				expr = "dim_of(_jscope_" + v_idx + ", 1)";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), dim_of(_jscope_" + v_idx + ", 1))";
				var_idx++;
			}
			return GetFloatArray(expr);
			// return GetFloatArray("DIM_OF("+in_y+", 1)");
		}

		@Override
		public String GetYLabel() throws IOException
		{
			String expr;
			if (_jscope_set)
				expr = "Units(_jscope_" + v_idx + ")";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(_jscope_" + v_idx + "))";
				var_idx++;
			}
			final String out = getStringValue(expr);
			if (out == null)
				_jscope_set = false;
			return out;
			// return GetDefaultYLabel(in_y);
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
			String expr;
			if (_jscope_set)
				expr = "Units(dim_of(_jscope_" + v_idx + ", 1))";
			else
			{
				_jscope_set = true;
				expr = "( _jscope_" + v_idx + " = (" + in_y + "), Units(dim_of(_jscope_" + v_idx + ", 1)))";
				var_idx++;
			}
			final String out = getStringValue(expr);
			if (out == null)
				_jscope_set = false;
			return out;
			// return GetDefaultZLabel(in_y);
		}

		@Override
		public boolean isXLong()
		{ return false; }

		@Override
		public void removeWaveDataListener(WaveDataListener listener)
		{}

		@Override
		public boolean supportsStreaming()
		{
			return false;
		}
	}

	public AsdexDataProvider()
	{
		super();
	}

	public AsdexDataProvider(String provider) throws IOException
	{
		super(provider);
	}

	@Override
	public synchronized float[] GetFloatArray(String in) throws IOException
	{
//        String parsed = ParseExpression(in);
		final String parsed = in;
		if (parsed == null)
			return null;
		error = null;
		final float[] out_array = super.GetFloatArray(parsed);
		if (out_array == null && error == null)
			error = "Cannot evaluate " + in + " for shot " + shot;
		if (out_array != null && out_array.length <= 1)
		{
			error = "Cannot evaluate " + in + " for shot " + shot;
			return null;
		}
		return out_array;
	}

	@Override
	public synchronized int[] getIntArray(String in) throws IOException
	{
		return super.getIntArray(ParseExpression(in));
	}

	@Override
	public int[] getNumDimensions(String spec)
	{
		return new int[]
		{ 1 };
	}

	public WaveData GetWaveData(String in)
	{
		return new SimpleWaveData(in);
	}

	@Override
	public int inquireCredentials(JFrame f, DataServerItem server_item)
	{
		return DataProvider.LOGIN_OK;
	}

	private String ParseExpression(String in)
	{
		if (in.startsWith("DIM_OF("))
			return in;
		final StringTokenizer st = new StringTokenizer(in, ":");
		String res;
		try
		{
			final String diag = st.nextToken();
			final String name = st.nextToken();
			res = "augsignal(" + shot + ",\"" + diag + "\",\"" + name + "\")";
		}
		catch (final Exception e)
		{
			error = "Wrong signal format: must be <diagnostic>:<signal>";
			return null;
		}
		System.out.println(res);
		return res;
	}

	@Override
	public void setArgument(String arg) throws IOException
	{
		mds.setProvider(arg);
		mds.setUser("mdsplus");
	}

	@Override
	public void SetCompression(boolean state)
	{}

	@Override
	public boolean SupportsCompression()
	{
		return false;
	}

	@Override
	public synchronized void update(String exp, long s)
	{
		error = null;
		shot = s;
	}
}
