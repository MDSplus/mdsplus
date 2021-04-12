package mds.connection;

/* $Id$ */
import java.awt.AWTEvent;

public class ConnectionEvent extends AWTEvent
{
	/**
	 * ConnectionEvent instances describe the current status of the data transfer
	 * and are passed by the DataProvider implementation to jScope by means of
	 * ConnectionListener.processConnectionEvent method. ConnectionEvent instances
	 * can also signal a connection lost. In this case field id (inherited by
	 * AWTEvent) is set to ConnectionEvent.LOST_CONNECTION, otherwise field id
	 * should be set to 0.
	 *
	 * @see ConnectionListener
	 */
	private static final long serialVersionUID = 1L;
	public static final int LOST_CONNECTION = AWTEvent.RESERVED_ID_MAX + 1;
	/**
	 * Total size of the data to be transferred.
	 */
	public int total_size;
	/**
	 * Number ofbytes transferred so far.
	 */
	public int current_size;
	/**
	 * Additional string information, shown in the status bar of jScope.
	 */
	public String info;

	public ConnectionEvent(final Object source, final String info, final int total_size, final int current_size)
	{
		super(source, 0);
		this.total_size = total_size;
		this.current_size = current_size;
		this.info = info;
	}

	public String getInfo()
	{ return info; }

	public ConnectionEvent(final Object source, final int total_size, final int current_size)
	{
		super(source, 0);
		this.total_size = total_size;
		this.current_size = current_size;
		this.info = null;
	}

	public ConnectionEvent(final Object source, final String info)
	{
		super(source, 0);
		this.info = new String(info);
	}

	public ConnectionEvent(final Object source, final int event_id, final String info)
	{
		super(source, event_id);
		this.info = new String(info);
	}
}
