package jScope;

import java.io.IOException;

/**
 * Interface DataAccess is used by classes WaveDisplay and CompositeWaveDisplay
 * retrieve signals and images given an URL definition.
 * To add a new protocol to those natively supported it is necessary to call 
 * WaveDisplay method addProtocol, passing an implementation of DataAccess.
 * The URL definition defines in its first part the name of the protocol. 
 * The interpretation of the rest of the URL address is entirely managed by the DataAccess
 * implementation.
 * DataAccess defines two mechanisms for retrieval, which need to be both
 * supported by its implementations. The first mechanism allows a Signal or a sequence
 * of frames (specified by an implementation of FrameData) to be accessed directly, given
 * an URL.
 * The second mechanisms delegates data access to the WaveDisplay client. In this
 * case the client needs first to specify the data source by means of method setProvider.
 * An URL is used to specify a data source. Its interpretation is however delegated
 * to the DataAccess implementation.  The client
 * then can retrieve an implementation of DataProvider (method getDataProvider),
 * and use this implementation directly to retrieve signals and frame sequences for 
 * that experiment and shot.
 */
public interface DataAccess
{

    /**
     * Retrieve the X axis of the signal specified by the URL.
     * 
     * @param url The URL of the signal.
     * @return A float array specifying the desired X axis
     * @exception java.io.IOException
     */
    float[]       getX(String url) throws IOException;

    /**
     * Retrieve the Y axis of the signal specified by the URL.
     * 
     * @param url the signal URL
     * @return A float array specifying the desired X axis
     * @exception java.io.IOException
     */
    float[]       getY(String url) throws IOException;

    /**
     * Retrieve the signal specified by the URL as a whole.
     * Its usual implementation is based on methods getX() and getY() as follows:
     * 
     * Signal getSignal(String url)
     * {
     * 	return new Signal(getX(url), getY(url));
     * }
     * 
     * @param url The signal URL
     * @return The signal specified by the URL
     * @exception java.io.IOException
     */
    Signal        getSignal(String url) throws IOException;

    /**
     * Retrieves the sequence of frames (specified by an instance of FrameData)
     * specified by the passed URL. See the specification of FrameData
     * interface for details.
     * 
     * @param url The URL of the frame sequence
     * @return The frame sequence
     * @exception java.io.IOException
     */
    FrameData     getFrameData(String url) throws IOException;

    /**
     * Test if the specified protocol is supported. It is used by WaveDisplay
     * to find out which DataAccess implementation (if any) is able to manage 
     * the specified protocol.
     * 
     * @param url the signal URL. The protocol is specified in the first patrt of the URL.
     * @return True if the protocol is supported, false otherwise.
     */
    boolean       supports(String url);

    /**
     * Last error message
     * 
     * @return The last error message string. If no errors returns null.
     */
    String        getError();

    /**
     * Returns the name of the last accessed signal.
     * 
     * @return The anme of the last accessed signal
     */
    String        getSignalName();

    /**
     * Return the shot number of the last accessed signal
     * 
     * @return The shot number specified as a string
     */
    String        getShot();

    /**
     * Return the experiment name of the last accessed signal
     * 
     * @return The experiment name
     */
    String        getExperiment();

    /**
     * Returns the DataProvider implementation carrying out data access for the experiment and shot
     * specified in the URL passed to method setProvider.
     * 
     * @return The DataProvider implementation.
     */
    DataProvider  getDataProvider();

    /**
     * Specifies experiment name and shot by means of an URL. The DataAccess implementation
     * is fully responsible for the proper interpretation of the URL.
     * 
     * @param url
     * @exception java.io.IOException
     */
    public void   setProvider(String url) throws IOException;

    /**
     * Used to communicate a password for data access autentication. If no 
     * authentication mechanism is defined in the DataAccess implementation, 
     * the method is void.
     * This method is called by the WaveDisplay applet when the HTML 
     * paramenter AUTENTICATION is defined.
     * 
     * @param encoded_credentials The Password, expressed as a string.
     */
    void          setPassword(String encoded_credentials);

    /**
     * Releases all allocated resources.
     */
    void          close();
}
