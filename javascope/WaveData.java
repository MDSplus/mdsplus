import java.io.IOException;

/**
 * When a signal is requested to a DataProvider, it is returned by means of an object implementing 
 * WaveData interface. WaveData defines all the data access method for the corresponding signal.
 * 
 * @see DataProvider
 */
interface WaveData
{

    /**
     * Returns the number of dimensions of the corresponding signal. Currently only unidimensional signals
     * are supported by jScope.
     * 
     * @return The number of dimension for the represented signal.
     * @exception java.io.IOException
     */
    public int GetNumDimension()throws IOException;

    /**
     * Get Y data (for unidimensional signals) or Z data (for bidimensional signals) as a float array.
     * If bidimensional sugnals are returned, values are ordered by rows.
     * 
     * @return The signal Y or Z data coded as a float array.
     * @exception java.io.IOException
     */
    public float[] GetFloatData() throws IOException;

    /**
     * Get X array, usually representing the time values for signals.
     * 
     * @return The returned X values coded as a float array
     * @exception java.io.IOException
     */
    public float[] GetXData()   throws IOException;

    /**
     * Get Y data description (only for bidimensional signals)
     * 
     * @return The Y data specification coded as a float array.
     * @exception java.io.IOException
     */
    public float[] GetYData()   throws IOException;

    /**
     * Get the associated title for the signal. It is displayed if no title is defined in the setup 
     * data definition.
     * 
     * @return The title string.
     * @exception java.io.IOException
     */
    public String GetTitle()   throws IOException;

    /**
     * Get the associated label for X axis. It is displayed if no X axis label is defined in the setup data 
     * definition.
     * 
     * @return The X label string.
     * @exception java.io.IOException
     */
    public String GetXLabel()  throws IOException;

    /**
     * Get the associated label for Y axis. It is displayed if no Y axis label is defined in the setup data 
     * definition.
     * 
     * @return The Y label string.
     * @exception java.io.IOException
     */
    public String GetYLabel()  throws IOException;

    /**
     * Get the associated label for Z axis (for bidimensional signals only). It is displayed if no X axis label is defined in the setup data 
     * definition.
     * 
     * @return The Z label string.
     * @exception java.io.IOException
     */
    public String GetZLabel()  throws IOException;
}