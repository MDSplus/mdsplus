package mds.wave;

import java.io.IOException;

import javax.swing.JFrame;

import mds.connection.*;

/**
 * The DataProvider interface is used by jScope to deal with signals, regardless
 * their origin. DataProvider defines a set of method for providing the signal
 * and supporting several features which may be enabled or disabled depending on
 * the current implementation of the DataProvider interface. Signals are not
 * directly returned by DataProvider access methods as vectors (as it was done
 * in earlier jScope versions). Rather DataProvider GetWaveData method returns
 * an implementation of WaveData. Interface WaveData defines the methods for
 * collecting signal data.
 *
 * @see WaveData
 */
public interface DataProvider extends AutoCloseable
{
	static final int LOGIN_OK = 1, LOGIN_ERROR = 2, LOGIN_CANCEL = 3;

	/**
	 * Method AddConnectionListener is called by jScope after the instantiation of a
	 * DataProvider implementation, in order to be notified of the status of data
	 * transfer. If a DataProvider implementation has to handle the transfer of
	 * large chunks of data (e.g. frame sequences) requiring a relatively long time,
	 * it should be able to communicate the current status of the transfer to jScope
	 * which displays then a status bar. To do this, the DataProvider implementation
	 * calls ConnectionListener's method processConnectionEvent, passing an istance
	 * of ConnectionEvent as argument. Class ConnectionEvent defines the following
	 * information: <br>
	 * - int total_size: total size of the data to be transferred <br>
	 * - int current_size: the size of the data transferred so far <br>
	 * - String info: an additional information string which is displayed by jScope
	 * <br>
	 * If the DataProvider implementation does not support transfer notification,
	 * method AddConnectionListener is empty.
	 *
	 * @param l The ConnectionListener which has to be notified by means of method
	 *          processConnectionEvent(ConnenctionEvent)
	 * @see ConnectionListener
	 * @see ConnectionEvent
	 */
	public void addConnectionListener(ConnectionListener l);

	/**
	 * Method AddUpdateEventListener is called by jScope when asynchronous update on
	 * event is defined. To connect data visualization to asynchronous event an
	 * event name has to be defined in the Update event field of the setup data form
	 * popup dialog. DataProvider implementations supporting asynchronous signal
	 * updating should notify event occurrence to all registered listeners by
	 * calling method processUpdateEvent(UpdateEvent e). UpdateEvent defines the
	 * following field, in addition to those defined in the AWTEvent superclass:
	 * <br>
	 * - String name: the name of the event <br>
	 * The possibility of connecting data display to asynchronous events is useful
	 * when jScope is used during data acquisition, for an automatic display update
	 * when a signal has been acquired. In this case the data acquisition system
	 * would generate an event when the signal has been acquired, and the
	 * DataProvider implementation would call method processUpdateEvent (defined in
	 * UpdateEventListener interface) to request the display update to jScope. <br>
	 * If the DataProvider implementation does not support asynchronous events, the
	 * method is void <br>
	 *
	 * @param l     The passed instance of UpdateEventListener.
	 * @param event The name of the event for which the UpdateEventListener is
	 *              registering.
	 * @exception java.io.IOException
	 * @see UpdateEventListener
	 * @see UpdateEvent
	 */
	public void addUpdateEventListener(UpdateEventListener l, String event) throws IOException;

	/**
	 * Method Dispose is called by jScope each time a DataProvider is no more used.
	 * Unlike Object.finalize(), method Dispose is guaranteed to be called at the
	 * time the DataProvider implementation is no more used by jScope.
	 */
	@Override
	public void close();

	/**
	 * If an error is encountered in the evaluation of a signal (GetWaveData or
	 * GetResampledWaveData returning null or generating IOException), jScope calls
	 * ErrorString method to retrieve the description of the error just occurred.
	 *
	 * @return A verbose description of the last error.
	 */
	public String getError();

	/**
	 * Method GetFloat is called by jScope to evaluate x min, x max, y min, y max
	 * when defined in the stup data source popup form. The argument is in fact the
	 * string typed by the user in the form. In its simplest implementation, method
	 * GetFloat converts the string into a float value and returns it. Other data
	 * providers, such as MdsDataProvider, evaluate, possibly remotely, the passed
	 * string which may therefore be represented by an expression.
	 *
	 * @param in The specification of the value.
	 * @return The evaluated value.
	 */
	public double getFloat(String in, int row, int col, int index) throws IOException;

	/**
	 * GetFrameData is called by jScope to retrieve and display a frame sequence.
	 * The frame sequence is returned by means of an object implementing the
	 * FrameData interface.
	 *
	 * @param in_frame   The frame sequence specification as defined in the frames
	 *                   field of the setup data popup dialog
	 * @param start_time Initial considered time for the frame sequence.
	 * @param end_time   Final considered time for the frame sequence.
	 * @param in_times   The definition of the time specification for the frame list
	 *                   as defined in the times field of the setup data popup
	 *                   dialog.
	 * @return An implementation of the FrameData interface representing the
	 *         corresponding frame sequence
	 * @exception java.io.IOException
	 * @see FrameData
	 */
	public FrameData getFrameData(String in_frame, String in_times, float start_time, float end_time)
			throws IOException;

	/**
	 * Evaluate the passed string to provide an array of shot numbers.
	 *
	 * @param inShots The shot(s) specification as typed in the "Shot" field of
	 *                jScope panel
	 */
	public long[] getShots(String inShots, String experiment) throws IOException;

	/**
	 * GetString is called by jScope to evaluate title, X label and Y label. These
	 * labels are defined as string in the Setup data source popup form. In the
	 * simplest case labels are exactly the same as defined, and therefore GetString
	 * simply returns its argument. In any case the data provider is free to
	 * interpret differently the argument.
	 *
	 * @param in The specification of the label or title.
	 * @return The evaluated label or title.
	 */
	public String getString(String in, int row, int col, int index) throws IOException;

	/**
	 * Method GetWaveData called by jScope when a waveform has to be evaluated and
	 * only the Y axis is defined. In this case jScope assumes that the
	 * specification is enough and it is up to the data provider implementation to
	 * retrieve X and Y axis. The evaluated signal is not directly returned as a
	 * vector, rather as a object implementing the WaveData interface.
	 *
	 * @param in    The specification of the signal, typed in the Y axis field of
	 *              the setup data source popup form, or in the lower right window
	 *              of jScope.
	 * @param row   Row number of the corresponding panel
	 * @param col   Column number of the corresponding panel
	 * @param index Index the corresponding panel
	 * @return The evaluated signal embedded in a WaveData object, or null if an
	 *         error is encountered.
	 * @see WaveData
	 */
	public WaveData getWaveData(String in, int row, int col, int index);

	/**
	 * Method GetWaveData called by jScope when a waveform has to be evaluated and
	 * both X and Y axis are defined. The evaluated signal is not directly treturned
	 * as a vector, rather as a object implementing the WaveData interface. jScope
	 * the uses the returned object to retieve X and Y axis.
	 *
	 * @param in_y  Y axis specification as typed in the setup data source popupo
	 *              dialog.
	 * @param in_x  X axis specification as typed in the setup data source popupo
	 *              dialog.
	 * @param row   Row number of the corresponding panel
	 * @param col   Column number of the corresponding panel
	 * @param index Index the corresponding panel
	 * @return The evaluated signal, embedded in a WaveData object, or null if an
	 *         error is encountered.
	 * @see WaveData
	 */
	public WaveData getWaveData(String in_y, String in_x, int row, int col, int index);

	/**
	 * Some DataProvider implemenations may require additional information (e.g.
	 * username and password) typed by users in a login form. As jScope does not
	 * make any assumption on this kind of information it is up to the DataProvider
	 * implementation to realize such a form (if any). For this reason jScope calls
	 * method InquireCredential passing as argument the container JFrame.
	 * InquireCredentials may return: <br>
	 * -DataProvider.LOGIN_OK if the DataProvider has been succesfully validated
	 * <br>
	 * -DataProvider.LOGIN_ERROR if the DataProvided did not succeed in validation
	 * <br>
	 * -DataProvider.LOGIN_CANCEL if the login proess has been aborted by user
	 * (usually pressing <br>
	 * a cancel button) <br>
	 * Only in the first case jScope will proceede with the usega of the
	 * DataProvider implementation. In the secon case a warning dialog is shown, and
	 * in the third case the DataProvider is simply discarded.
	 *
	 * @param frame The container Frame.
	 * @return The status of the DataProvider specific validation process
	 */
	public int inquireCredentials(JFrame frame, DataServerItem server_item);

	/**
	 * Can be used to check if provider is busy (helps scheduling).
	 *
	 * @return true is busy.
	 */
	public boolean isBusy();

	/**
	 * Called by jScope when transfer notification is no more requested. Empty if
	 * the DataProvider implementation does not support transfer status
	 * notification.
	 *
	 * @param l The previously registered ConnectionListener.
	 * @see ConnectionListener
	 */
	public void removeConnectionListener(ConnectionListener l);

	/**
	 * Method RemoveUpdateEventListeneris called by jScope when the display of a
	 * waveform panel is no more triggered by an event.
	 *
	 * @param l     The instance of UpdateEventListener previously registered.
	 * @param event The event to which the listener was previously registered.
	 * @exception java.io.IOException
	 * @see UpdateEventListener
	 */
	public void removeUpdateEventListener(UpdateEventListener l, String event) throws IOException;

	/**
	 * As DataProvider implementations are instantiated by jScope by means of the
	 * Class.newInstance(), no arguments can be passed to the constructor method. If
	 * an additional argument is required for the proper initialization of the
	 * DataProvider implementation (e.g. the ip address for the MdsDataProvider),
	 * the argument, defined in the server_n.argument item of the property file is
	 * passed through method SetArgument called by jScope just after the
	 * DataProvider instantiation.
	 *
	 * @param arg The argument passed to the DataProvider implementation.
	 * @exception java.io.IOException
	 */
	public void setArgument(String arg) throws IOException;

	/**
	 * Defines the environment for data retrieval. Depending on the nature of the
	 * provider of data, some environment variable may be defined. For example the
	 * MdsDataProvider allows the definition of TDI variables which may be useful
	 * for configuring remote data access. jScope allows the definition of an
	 * arbitrary set of environment variable using the customize--public
	 * variables... option. As jScope does not make any assumption on the syntax of
	 * the defined variables, each variable definition is passed to the Data
	 * provider by means of the SetEnvironment method, whose argument is a String
	 * which defines name value pairs. If the DataProvider implementation does not
	 * support such a feature, it simply returns.
	 *
	 * @param exp The variable definition expressed as name value pair.
	 * @exception java.io.IOException
	 */
	public void setEnvironment(String exp) throws IOException;

	/**
	 * Enable tunneling activation. Return False if no ssh tunneling available.
	 *
	 * @return Tunneling flag
	 **/
	public boolean supportsTunneling();

	/**
	 * Method Update is called by jScope to notify the experiment name and the shot
	 * number. Update can be called several time by jScope for the same DataProvider
	 * implementation in the case user changes either the experiment name or the
	 * shot number.
	 *
	 * @param exp The experiment name
	 * @param s   The shot number.
	 */
	public void update(String exp, long shot);
}
