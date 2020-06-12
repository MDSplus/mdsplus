package mds.wave;

import java.io.IOException;

/**
 * When a signal is requested to a DataProvider, it is returned by means of an
 * object implementing WaveData interface. WaveData defines all the data access
 * method for the corresponding signal.
 *
 * @see DataProvider
 */
public interface WaveData
{
	/**
	 * Register a new WaveData listener. jScope defines a publish-subscribe pattern
	 * to handle dynamic data visualization. Whenever new data are available for
	 * visualization, the DataProvider instance will notify the registered listeners
	 * via WaveDataListener.dataRegionUpdated() methods (two methods are defined
	 * format for handling absolute or relative times, respectively). DataProvider
	 * implementations that do not handle dynamic data display will implement this
	 * method as void.
	 *
	 * @param listener: The passed WaveDataListener instance.
	 * @see WaveDataListener
	 */
	void addWaveDataListener(WaveDataListener listener);

	/**
	 * Get X and Y data for unidimensional signals within a given time interval The
	 * returned container object is an instance of XYData bringing information about
	 * X (time) and Y axis
	 *
	 * @param xmin:      Lower bound of X region of interest
	 * @param xmax:      Upper bound of X region of interest
	 * @param numPoints: The suggested number of samples. The returned number of
	 *                   samples actually returned in XYData instance can be
	 *                   different, it is up to the DataProvider implementation
	 *                   decide the resampling policy. The passed number is a hint
	 *                   from jScope
	 * @return The XYData instance describing X and Y values of the returned signal.
	 * @exception java.io.IOException
	 * @see XYData
	 */
	XYData getData(double xmin, double xmax, int numPoints) throws IOException;

	/**
	 * Get X and Y data for unidimensional signals The returned container object is
	 * an instance of XYData bringing information about X (time) and Y axis
	 *
	 * @param numPoints: The suggested number of samples. The returned number of
	 *                   samples actually returned in XYData instance can be
	 *                   different, it is up to the DataProvider implementation
	 *                   decide the resampling policy. The passed number is a hint
	 *                   from jScope
	 * @return The XYData instance describing X and Y values of the returned signal.
	 * @see XYData
	 */
	XYData getData(int numPoints) throws IOException;

	/**
	 * Get X and Y data for unidimensional signals within a given time interval
	 * expressed in absolute times The returned container object is an instance of
	 * XYData bringing information about X (time) and Y axis
	 *
	 * @param xmin:      Lower bound of X region of interest
	 * @param xmax:      Upper bound of X region of interest
	 * @param numPoints: The suggested number of samples. The returned number of
	 *                   samples actually returned in XYData instance can be
	 *                   different, it is up to the DataProvider implementation
	 *                   decide the resampling policy. The passed number is a hint
	 *                   from jScope
	 * @return The XYData instance describing X and Y values of the returned signal.
	 * @exception java.io.IOException
	 * @see XYData
	 */
	XYData getData(long xmin, long xmax, int numPoints) throws IOException;

	/**
	 * Start asynchronous data readout. Expected to return soon, being the data
	 * chunk communicated later by notifying the registered listeners DataProvider
	 * instances supporting neither dynamic resample (for data reduction) nor
	 * support for signals evolving over time will implement this method as void. In
	 * this case the full signal will be returned by getData() method. getDataAsynch
	 * will be called by jScope whenever the required resolution of a zoomed portion
	 * of signal is less then current resolution of the stored signal. The
	 * computation of the signal resolution if left to the DataProvider
	 * implementation that will communicate it to jScope either via the returned
	 * XYData object in method getData() or in the resolution argument of the
	 * WaveDataListener callback methods. If the DataProvider implementation does
	 * not handle dynamic resolution, it will return Double.MAX_VALUE in the
	 * resolution argument of the XYData constructor, this disabling further calls
	 * of method getDataAsync().
	 *
	 *
	 * @param lowerBound: Lower bound of X region of interest
	 * @param upperBound: Upper bound of X region of interest
	 * @param numPoints:  Estimated number of requested point
	 */
	void getDataAsync(double lowerBound, double upperBound, int numPoints);

	/**
	 * Returns the number of dimensions of the corresponding signal. Currently only
	 * unidimensional and bidimensional signals are supported by jScope.
	 *
	 * @return The number of dimension for the represented signal.
	 * @exception java.io.IOException
	 */
	public int getNumDimension() throws IOException;

	/**
	 * Get the title associated with that signal. It is displayed if no title is
	 * defined in the setup data definition.
	 *
	 * @return The title string.
	 * @exception java.io.IOException
	 */
	public String GetTitle() throws IOException;

	/**
	 * Get X data for bidimensional signals. Called when the returned number of
	 * dimensions is 2 The X values are assumed to represent the time values
	 *
	 * @return The unidimensional array of X values.
	 */
	double[] getX2D();

	/**
	 * Get X data for bidimensional signals in absolute time units. Called when the
	 * returned number of dimensions is 2 and returned value of isXLong() is true
	 * The X values are assumed to represent the absolute time values
	 *
	 * @return The unidimensional array of X values.
	 */
	long[] getX2DLong();

	/**
	 * Get the associated label for X axis. It is displayed if no X axis label is
	 * defined in the setup data definition.
	 *
	 * @return The X label string.
	 * @exception java.io.IOException
	 */
	public String GetXLabel() throws IOException;

	/**
	 * Get Y data for bidimensional signals. Called when the returned number of
	 * dimensions is 2
	 *
	 * @return The unidimensional array of Y values.
	 */
	float[] getY2D();

	/**
	 * Get the associated label for Y axis. It is displayed if no Y axis label is
	 * defined in the setup data definition.
	 *
	 * @return The Y label string.
	 * @exception java.io.IOException
	 */
	public String GetYLabel() throws IOException;

	/**
	 * Get Z data for bidimensional signals. Called when the returned number of
	 * dimensions is 2
	 *
	 * @return The bisimensional array of Z values (z=f(x,y)). Row first order is
	 *         assumed.
	 */
	float[] getZ();

	/**
	 * Get the associated label for Z axis (for bi-dimensional signals only). It is
	 * displayed if no X axis label is defined in the setup data definition.
	 *
	 * @return The Z label string.
	 * @exception java.io.IOException
	 */
	public String GetZLabel() throws IOException;

	/**
	 * Check if absolute or relative times are used. Absolute times are stored as 8
	 * bytes long. The time interpretation in jScope visualization will depend on
	 * the jScope.time_format property definition (default is Linux time format, can
	 * be EPICS or VMS). Relative times are represented by floating point
	 *
	 * @return true if absolute times are used, false otherwise
	 */
	boolean isXLong();

	/**
	 * Unregister a WaveData listener.
	 */
	void removeWaveDataListener(WaveDataListener listener);

	/**
	 * Check whether this data supports streaming
	 *
	 * @return True if streaming is supported, false otherwise
	 **/
	boolean supportsStreaming();
}
