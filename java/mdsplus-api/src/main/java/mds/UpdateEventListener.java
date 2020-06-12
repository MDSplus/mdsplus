package mds;

/* $Id$ */
import java.util.EventListener;

/**
 * An implementation of UpdateEventListener is passed in
 * DataProvider.addUpdateEventListener method, to be notified by the
 * DataProvider implementation when the corresponding asynchronous event is
 * received.
 */
public interface UpdateEventListener extends EventListener
{
	/**
	 * Called by the DataProvider implementation to notify jScope that an
	 * asynchronous event has been received.
	 *
	 * @param source The Object where the Event got dispatched.
	 * @param name   The UpdateEvent describing the event (i.e. its name) just
	 *               occurred.
	 */
	public void handleUpdateEvent(Mds source, String name);
}
