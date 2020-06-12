/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mds.wave;

/**
 *
 * @author manduchi
 */
/**
 * Class XYData is the container of signals returned by WaveData instances. Code
 * immplementing WaveData interface will use this class only via its constructor
 * methods. Besides X and Y arrays, resolution and increasingX information is
 * defined here. Resolution is computed in jScope as NumberOfSamples/XRange and
 * compared with the declared resolution on the XYData instance in order to
 * activate the asyncronous retrieval mechanism (that occurs when a resampled
 * signal is zoomed and more resolution is required in the zoomed region).
 * increasingX flag specifies whether the X samples are in increasing order.
 * jScope handles dynamic resampling only when this flag is true.
 *
 * @see WaveData
 */
public class XYData
{
	public double resolution; // Number of points/interval
	boolean increasingX;
	public int nSamples;
	public double[] x = null;
	public long[] xLong = null;
	public float[] y;
	double xMin, xMax;

	/**
	 * XYData Constructor with automatic derivation of increasingX flag
	 *
	 * @param x:          x (times) array
	 * @param y:          y (values) array
	 * @param resolution: declared resolution
	 */
	public XYData(double x[], float y[], double resolution)
	{
		this.resolution = resolution;
		this.x = x;
		this.y = y;
		increasingX = true;
		nSamples = (x.length < y.length) ? x.length : y.length;
		if (nSamples > 0)
		{
			xMin = xMax = x[0];
			for (int i = 1; i < x.length; i++)
			{
				if (x[i - 1] > x[i])
				{
					increasingX = false;
				}
				if (x[i] > xMax)
					xMax = x[i];
				if (x[i] < xMin)
					xMin = x[i];
			}
		}
	}

	/**
	 * XYData Constructor
	 *
	 * @param x:           x (times) array
	 * @param y:           y (values) array
	 * @param resolution:  declared resolution
	 * @param increasingX: increasing x values flag
	 */
	public XYData(double x[], float y[], double resolution, boolean increasingX)
	{
		this(x, y, resolution, increasingX, x[0], x[x.length - 1]);
	}

	/**
	 * XYData Constructor
	 *
	 * @param x:           x (times) array
	 * @param y:           y (values) array
	 * @param resolution:  declared resolution
	 * @param increasingX: incresing x values flag
	 * @param xMin:        lower bound X region of interest
	 * @param xMax:        upper bound X region of interest
	 */
	public XYData(double x[], float y[], double resolution, boolean increasingX, double xMin, double xMax)
	{
		this.resolution = resolution;
		this.increasingX = increasingX;
		this.x = x;
		this.y = y;
		this.xMin = xMin;
		this.xMax = xMax;
		nSamples = (x.length < y.length) ? x.length : y.length;
	}

	/**
	 * XYData Constructor with absolute times and automatic derivation of
	 * increasingX flag
	 *
	 * @param x:          x (times) array
	 * @param y:          y (values) array
	 * @param resolution: declared resolution
	 */
	public XYData(long x[], float y[], double resolution)
	{
		this.resolution = resolution;
		this.xLong = x;
		this.y = y;
		this.x = new double[x.length];
		for (int i = 0; i < x.length; i++)
			this.x[i] = x[i];
		increasingX = true;
		nSamples = (x.length < y.length) ? x.length : y.length;
		if (nSamples > 0)
		{
			xMin = xMax = x[0];
			for (int i = 1; i < x.length; i++)
			{
				if (x[i - 1] > x[i])
				{
					increasingX = false;
				}
				if (x[i] > xMax)
					xMax = x[i];
				if (x[i] < xMin)
					xMin = x[i];
			}
		}
	}

	/**
	 * XYData Constructor with absolute times
	 *
	 * @param x:           x (times) array
	 * @param y:           y (values) array
	 * @param resolution:  declared resolution
	 * @param increasingX: incresing x values flag
	 * @param xMin:        lower bound X region of interest
	 * @param xMax:        upper bound X region of interest
	 */
	public XYData(long[] x, float[] y, double resolution, boolean increasingX)
	{
		this.resolution = resolution;
		this.increasingX = increasingX;
		this.xLong = x;
		this.y = y;
		this.x = new double[x.length];
		for (int i = 0; i < x.length; i++)
			this.x[i] = x[i];
		nSamples = (x.length < y.length) ? x.length : y.length;
	}
}
