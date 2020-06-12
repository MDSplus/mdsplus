/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mds.wave;

import java.util.*;

/**
 *
 * @author manduchi Basic version of WaveData which keeps arrays for X and Y
 *         (old style)
 */
public class XYWaveData implements WaveData
{
	// Inner class AsyncUpdater
	class AsyncUpdater extends Thread
	{
		double lowerBound;
		double upperBound;
		double resolution;

		AsyncUpdater(double lowerBound, double upperBound, double resolution)
		{
			this.lowerBound = lowerBound;
			this.upperBound = upperBound;
			this.resolution = resolution;
		}

		@Override
		public void run()
		{
			// System.out.println("GET DATA ASYNC "+lowerBound+" "+upperBound+"
			// "+resolution);
			final XYData newData = getData(lowerBound, upperBound, 1000);
			if (isLong)
				fireListeners(newData.xLong, newData.y, newData.resolution);
			else
				fireListeners(newData.x, newData.y, newData.resolution);
		}
	}
	int type;
	boolean isLong = false;
	boolean increasingX = true;
	double x[];
	float y[];
	double x2D[];
	long x2DLong[];
	float y2D[];
	float z[];
	long xLong[];
	Vector<WaveDataListener> listeners = new Vector<>();

	// For test
	boolean liveUpdateStarted = false;

	XYWaveData(double x[], float y[])
	{
		this(x, y, x.length);
	}

	XYWaveData(double x[], float y[], float z[])
	{
		type = Signal.TYPE_2D;
		this.x2D = new double[x.length];
		this.y2D = new float[y.length];
		this.z = new float[z.length];
		for (int i = 0; i < x.length; i++)
			this.x2D[i] = (float) x[i];
		for (int i = 0; i < y.length; i++)
			this.y2D[i] = y[i];
		for (int i = 0; i < z.length; i++)
			this.z[i] = z[i];
		if (z.length != x.length * y.length)
			System.out.println("INTERNAL ERROR: WRONG DIMENSIONS FOR 2D SIGNAL");
	}

	XYWaveData(double x[], float y[], int numPoints)
	{
		type = Signal.TYPE_1D;
		int len = numPoints;
		if (x.length < len)
			len = x.length;
		if (y.length < len)
			len = y.length;
		this.x = new double[len];
		this.y = new float[len];
		for (int i = 0; i < len; i++)
		{
			this.x[i] = x[i];
			this.y[i] = y[i];
		}
		checkIncreasingX();
	}

	XYWaveData(float x[], float y[])
	{
		this(x, y, x.length);
	}

	XYWaveData(float x[], float y[], float z[])
	{
		type = Signal.TYPE_2D;
		this.x2D = new double[x.length];
		this.y2D = new float[y.length];
		this.z = new float[z.length];
		for (int i = 0; i < x.length; i++)
			this.x2D[i] = x[i];
		for (int i = 0; i < y.length; i++)
			this.y2D[i] = y[i];
		for (int i = 0; i < z.length; i++)
			this.z[i] = z[i];
		if (z.length != x.length * y.length)
			System.out.println("INTERNAL ERROR: WRONG DIMENSIONS FOR 2D SIGNAL");
	}

	XYWaveData(float x[], float y[], int numPoints)
	{
		type = Signal.TYPE_1D;
		int len = numPoints;
		if (x.length < len)
			len = x.length;
		if (y.length < len)
			len = y.length;
		this.x = new double[len];
		this.y = new float[len];
		for (int i = 0; i < len; i++)
		{
			this.x[i] = x[i];
			this.y[i] = y[i];
		}
		checkIncreasingX();
	}

	XYWaveData(long x[], float y[])
	{
		type = Signal.TYPE_1D;
		int len = x.length;
		if (y.length < len)
			len = y.length;
		this.x = new double[len];
		this.y = new float[len];
		this.xLong = new long[len];
		for (int i = 0; i < len; i++)
		{
			this.x[i] = x[i];
			this.y[i] = y[i];
			this.xLong[i] = xLong[i];
		}
		isLong = true;
		checkIncreasingX();
	}

	XYWaveData(long x[], float y[], float z[])
	{
		type = Signal.TYPE_2D;
		this.x2DLong = new long[x.length];
		this.y2D = new float[y.length];
		this.z = new float[z.length];
		for (int i = 0; i < x.length; i++)
			this.x2DLong[i] = x[i];
		for (int i = 0; i < y.length; i++)
			this.y2D[i] = y[i];
		for (int i = 0; i < z.length; i++)
			this.z[i] = z[i];
		if (z.length != x.length * y.length)
			System.out.println("INTERNAL ERROR: WRONG DIMENSIONS FOR 2D SIGNAL");
		isLong = true;
	}

	@Override
	public void addWaveDataListener(WaveDataListener listener)
	{
		listeners.addElement(listener);
	}

	void checkIncreasingX()
	{
		increasingX = true;
		for (int i = 1; i < x.length; i++)
		{
			if (x[i - 1] > x[i])
			{
				increasingX = false;
				return;
			}
		}
	}

	void fireListeners(double[] x, float[] y, double resolution)
	{
		for (int i = 0; i < listeners.size(); i++)
			listeners.elementAt(i).dataRegionUpdated(x, y, resolution);
	}

	void fireListeners(long[] x, float[] y, double resolution)
	{
		for (int i = 0; i < listeners.size(); i++)
			listeners.elementAt(i).dataRegionUpdated(x, y, resolution);
	}

	@Override
	public XYData getData(double xmin, double xmax, int numPoints)
	{
		int minIdx, maxIdx;
		if (!increasingX) // If not increasing return bare data
		{
			return new XYData(x, y, Double.MAX_VALUE, false);
		}
		if (xmin == -Float.MAX_VALUE)
		{
			xmin = x[0];
			minIdx = 0;
		}
		else
		{
			for (minIdx = 0; minIdx < x.length - 2 && x[minIdx] < xmin; minIdx++);
			if (minIdx > 0 && x[minIdx] > xmin)
				minIdx--;
		}
		if (xmax == Float.MAX_VALUE)
		{
			xmax = x[x.length - 1];
			maxIdx = x.length - 1;
		}
		else
		{
			for (maxIdx = minIdx + 1; maxIdx < x.length - 1 && x[maxIdx] < xmax; maxIdx++);
		}
		// OK, trovato l'intervallo tra minIdx e maxIdx
		final double delta = (xmax - xmin) / numPoints;
		boolean showMinMax = false;
		int actPoints;
		// Forces resampling only if there is a significant numebr of points
		if ((maxIdx - minIdx) > 1000 && delta > 4 * (maxIdx - minIdx + 1) / (xmax - xmin))
		{
			// If at least there are four times real points
			actPoints = 2 * (int) ((xmax - xmin) / delta + 0.5);
			showMinMax = true;
		}
		else
		{
			actPoints = maxIdx - minIdx + 1; // No resampling at all
			showMinMax = false;
		}
		final float retY[] = new float[actPoints];
		final double retX[] = new double[actPoints];
		long retXLong[] = null;
		if (isLong)
			retXLong = new long[actPoints];
		if (showMinMax)
		{
			int currIdx = minIdx;
			for (int i = 0; i < actPoints / 2; i++)
			{
				float currMin = y[currIdx];
				float currMax = y[currIdx];
				final double currStart = x[currIdx];
				while (currIdx < x.length - 1 && (x[currIdx] - currStart) < delta)
				{
					if (y[currIdx] < currMin)
						currMin = y[currIdx];
					if (y[currIdx] > currMax)
						currMax = y[currIdx];
					currIdx++;
				}
				retX[2 * i] = retX[2 * i + 1] = (currStart + x[(currIdx == 0) ? 0 : currIdx - 1]) / 2.;
				if (isLong)
					retXLong[2 * i] = retXLong[2 * i
							+ 1] = (long) ((currStart + x[(currIdx == 0) ? 0 : currIdx - 1]) / 2.);
				retY[2 * i] = currMin;
				retY[2 * i + 1] = currMax;
			}
			if (isLong)
				return new XYData(retXLong, retY, actPoints / (xmax - xmin), true);
			else
				return new XYData(retX, retY, actPoints / (xmax - xmin), true);
		}
		else
		{
			for (int i = 0; i < maxIdx - minIdx + 1; i++)
			{
				retY[i] = y[minIdx + i];
				retX[i] = x[minIdx + i];
				if (isLong)
					retXLong[i] = xLong[minIdx + i];
			}
			if (isLong)
				return new XYData(retXLong, retY, Double.MAX_VALUE, true);
			else
				return new XYData(retX, retY, Double.MAX_VALUE, true);
		}
	}

	@Override
	public XYData getData(int numPoints)
	{
		if (type != Signal.TYPE_1D)
		{
			System.out.println("INTERNAL ERROR getData called for non 1 D signal");
			return null;
		}
		if (numPoints >= x.length)
			return new XYData(x, y, Double.MAX_VALUE, true);
		return getData(-Float.MAX_VALUE, Float.MAX_VALUE, numPoints);
	}

	@Override
	public XYData getData(long xmin, long xmax, int numPoints)
	{
		return getData((double) xmin, (double) xmax, numPoints);
	}

	@Override
	public void getDataAsync(double lowerBound, double upperBound, int numPoints)
	{
		// (new AsyncUpdater(lowerBound, upperBound, numPoints/(upperBound -
		// lowerBound))).start();
	}

	@Override
	public int getNumDimension()
	{ return 1; }

	@Override
	public String GetTitle()
	{
		return "Title";
	}

	@Override
	public double[] getX2D()
	{
		if (type == Signal.TYPE_2D)
			return x2D;
		System.out.println("INTERNAL ERROR SimpleWave.getZ for 1D signal");
		return null;
	}

	@Override
	public long[] getX2DLong()
	{
		if (type == Signal.TYPE_2D)
			return x2DLong;
		System.out.println("INTERNAL ERROR SimpleWave.getZ2dLong for 1D signal");
		return null;
	}

	/**
	 * Get the associated label for X axis. It is displayed if no X axis label is
	 * defined in the setup data definition.
	 *
	 * @return The X label string.
	 * @exception java.io.IOException
	 */
	@Override
	public String GetXLabel()
	{
		return "XLabel";
	}

	public double[] getXLimits()
	{
		double xmin, xmax;
		xmin = xmax = x[0];
		for (int i = 0; i < x.length; i++)
		{
			if (x[i] > xmax)
				xmax = x[i];
			if (x[i] < xmin)
				xmin = x[i];
		}
		return new double[]
		{ xmin, xmax };
	}

	public long[] getXLong()
	{
		if (!isLong)
		{
			System.out.println("INTERNAL ERROR: getLong called for non long X");
			return null;
		}
		return xLong;
	}

	@Override
	public float[] getY2D()
	{
		if (type == Signal.TYPE_2D)
			return y2D;
		System.out.println("INTERNAL ERROR SimpleWave.getZ for 1D signal");
		return null;
	}

	/**
	 * Get the associated label for Y axis. It is displayed if no Y axis label is
	 * defined in the setup data definition.
	 *
	 * @return The Y label string.
	 * @exception java.io.IOException
	 */
	@Override
	public String GetYLabel()
	{
		return "YLabel";
	}

	@Override
	public float[] getZ()
	{
		if (type == Signal.TYPE_2D)
			return z;
		System.out.println("INTERNAL ERROR SimpleWave.getZ for 1D signal");
		return null;
	}

	/**
	 * Get the associated label for Z axis (for bidimensional signals only). It is
	 * displayed if no X axis label is defined in the setup data definition.
	 *
	 * @return The Z label string.
	 * @exception java.io.IOException
	 */
	@Override
	public String GetZLabel()
	{
		return "ZLabel";
	}

	@Override
	public boolean isXLong()
	{ return isLong; }

	@Override
	public void removeWaveDataListener(WaveDataListener listener)
	{
		listeners.remove(listener);
	}

	/*
	 * Read data within specified interval. either Xmin or xmax cna specify no limit
	 * (-Float.MAX_VALUE, Float.MAX_VALUE)
	 */
	@Override
	public boolean supportsStreaming()
	{
		return false;
	}
}
