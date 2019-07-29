package mds;

/**
 * $Id$
 * Interface ConnectionListener describes method processConnectionEvent called by the DataProvider
 * implementation to notify jScope of the current status of the data transfer. ConnectionListener objects
 * register themselves by means of DataProvider.addConnectionListener() method.
 *
 * @see ConnectionEvent
 * @see DataProvider
 */
import java.util.EventListener;

public interface MdsListener extends EventListener{
    public void processMdsEvent(MdsEvent e);
}