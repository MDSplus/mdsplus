package mds.connection;

/**
 * An UpdateEvent instance is passed to UpdateEventListener implementations as
 * argument of processUpdateEvent in order to notify the receipt of an
 * asynchronous event triggering the display of a waveform.
 *
 * @see UpdateEventListener
 */
public class UpdateEvent extends java.awt.AWTEvent
{
	private static final long serialVersionUID = 1L;
	String name;

	public UpdateEvent(final Object source, final String event)
	{
		super(source, 0);
		name = event;
	}

	/**
	 * The name of the event just received.
	 */
	public String getName()
	{ return name; }
}
