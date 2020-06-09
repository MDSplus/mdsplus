package mds.provider.twu;

// ---------------------------------------------------------------------------------------------
// TwuWaveData
// An implementation of "WaveData" for signals from a TEC Web-Umbrella (Twu) server.
//
// $Id$
//
// ---------------------------------------------------------------------------------------------
import java.io.IOException;

import mds.provider.TwuDataProvider;
import mds.wavedisplay.*;

public class TwuWaveData implements WaveData
{
	private long shotOfThisData = 0;
	private TwuSingleSignal mainSignal = null;
	private TwuSingleSignal abscissa_X = null;
	private String mainSignal_name = null;
	private String abscissa_X_name = null;

	public TwuWaveData()
	{}

	public TwuWaveData(TwuDataProvider dp, String in_y)
	{
		init(dp, in_y, null);
	}

	public TwuWaveData(TwuDataProvider dp, String in_y, String in_x)
	{
		init(dp, in_y, in_x);
	}

	@Override
	public boolean supportsStreaming()
	{
		return false;
	}

	protected void init(TwuDataProvider dp, String in_y, String in_x)
	{
		in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
		in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
		shotOfThisData = dp.getShot();
		mainSignal_name = in_y;
		abscissa_X_name = in_x;
		mainSignal = new TwuSingleSignal(dp, mainSignal_name);
		if (abscissa_X_name != null)
			abscissa_X = new TwuSingleSignal(dp, abscissa_X_name);
		else
			abscissa_X = new TwuSingleSignal(dp, mainSignal);
	}

	public void setZoom(float xmin, float xmax, int n_points)
	{
		// this method allows reusing of this object
		// to fetch data from the same signal at a
		// different zoomrange.
		try
		{
			setFetchOptions(abscissa_X.FindIndicesForXRange(shotOfThisData, xmin, xmax, n_points));
		}
		catch (final Exception e)
		{}
		// the TwuSingleSignal already has the error flag set (?),
		// and it will throw an exception when jscope tries to
		// call GetFloatData().
	}

	public void setFullFetch()
	{
		try
		{
			setFetchOptions(new TwuFetchOptions());
		}
		catch (final IOException e)
		{}
		// same story as above, in setZoom.
	}

	public boolean notEqualsInputSignal(String in_y, String in_x, long requestedShot)
	{
		// this uses a simple (i.e. imperfect) comparison approach to see
		// if the WaveData for in_x, in_y has already been created ...
		if (shotOfThisData != requestedShot)
			return true;
		in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
		in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
		final boolean y_equals = (in_y == null) ? (mainSignal_name == null)
				: (mainSignal_name != null && in_y.equalsIgnoreCase(mainSignal_name));
		final boolean x_equals = (in_x == null) ? (abscissa_X_name == null)
				: (abscissa_X_name != null && in_x.equalsIgnoreCase(abscissa_X_name));
		return !(x_equals && y_equals);
	}

	// JScope has an inconsistent way of dealing with data: GetFloatData() is used
	// to
	// get the Y data, and *if* there's an abscissa (aka time data, aka X data) this
	// is retrieved using GetXData(). however, GetYData() is not used ?! MvdG
	// It is used! it represents the second abscissa, for a 2D signal! JGK
	public float[] GetFloatData() throws IOException
	{
		if (mainSignal == null || mainSignal.error())
			return null;
		return mainSignal.getData();
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
		return abscissa_X.getData();
	}

	public float[] GetYData() throws IOException
	{
		return mainSignal.getData(); // used to be : return null; ... :o
		// Wrong !! should return Abscissa.1 data!
		// TODO: To be fixed later! JGK.
	}

	@Override
	public String GetXLabel() throws IOException
	{
		return abscissa_X.getTwuProperties(shotOfThisData).Units();
	}

	@Override
	public String GetYLabel() throws IOException
	{
		return mainSignal.getTwuProperties(shotOfThisData).Units();
	}

	@Override
	public String GetZLabel() throws IOException
	{
		return null;
	}

	@Override
	public int getNumDimension() throws IOException
	{ return mainSignal.getTwuProperties(shotOfThisData).Dimensions(); }

	private String title = null;

	@Override
	public String GetTitle() throws IOException
	{
		// now has a special treatment for scalars ...
		if (title != null)
			return title;
		final int dim = getNumDimension();
		if (dim != 0)
			title = mainSignal.getTwuProperties(shotOfThisData).Title();
		else
		{
			try
			{
				title = mainSignal.ScalarToTitle(shotOfThisData);
			}
			catch (final IOException e)
			{
				throw e;
			}
			catch (final Exception e)
			{
				TwuSingleSignal.handleException(e);
				throw new IOException(e.toString());
			}
		}
		return title;
	}

	// A little utility method for the subclasses ...
	// (most fetch options, particularly settings involved with zoom range,
	// should be the same for both x and y data.)
	//
	protected void setFetchOptions(TwuFetchOptions opt) throws IOException
	{
		mainSignal.setFetchOptions(opt);
		abscissa_X.setFetchOptions(opt);
	}

	// another utility method. needed by TwuAccess (via via).
	// this is an efficient way to do it because it allows storage
	// of the properties within the TwuSingleSignal, so it won't
	// need to be retrieved time after time ...
	//
	public TwuProperties getTwuProperties() throws IOException
	{ return mainSignal.getTwuProperties(shotOfThisData); }

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
// ---------------------------------------------------------------------------------------------
// End of: $Id$
// ---------------------------------------------------------------------------------------------
