package mds.connection;

import java.util.EventListener;

/**
 *
 *
 * Interface ConnectionListener describes method processConnectionEvent called
 * by the DataProvider implementation to notify jScope of the current status of
 * the data transfer. ConnectionListener objects register themselves by means of
 * DataProvider.addConnectionListener() method. In order to notify jScope of
 * connection event (current data size increased in read, lost connection ...)
 * method processConnectionEvent() is called by DataProvider
 *
 * @see ConnectionEvent
 */
public interface ConnectionListener extends EventListener
{
	public void processConnectionEvent(ConnectionEvent e);
}
