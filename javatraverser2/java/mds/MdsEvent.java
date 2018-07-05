package mds;

/* $Id$ */
import java.awt.AWTEvent;
import java.sql.Connection;

/**
 * ConnectionEvent instances describe the current status of the data transfer and are passed by the DataProvider
 * implementation to jScope by means of ConnectionListener.processConnectionEvent method.
 * ConnectionEvent instances can also signal a connection lost. In this case field id (inherited by AWTEvent) is set
 * to ConnectionEvent.LOST_CONNECTION, otherwise field id should be set to 0.
 *
 * @see Connection Listener
 */
@SuppressWarnings("serial")
public class MdsEvent extends AWTEvent{
    public static final int IDLE         = AWTEvent.RESERVED_ID_MAX;
    public static final int TRANSFER     = AWTEvent.RESERVED_ID_MAX + 1;
    public static final int HAVE_CONTEXT = AWTEvent.RESERVED_ID_MAX + 2;
    public static final int LOST_CONTEXT = AWTEvent.RESERVED_ID_MAX + 3;
    /**
     * Number of bytes transferred so far.
     */
    public final int        current_size;
    /**
     * Additional string information, shown in the status bar of jScope.
     */
    String                  info;
    /**
     * Total size of the data to be transferred.
     */
    public final int        total_size;

    public MdsEvent(final Object source){
        super(source, MdsEvent.IDLE);
        this.info = null;
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final int total_size, final int current_size){
        super(source, MdsEvent.TRANSFER);
        this.total_size = total_size;
        this.current_size = current_size;
        this.info = null;
    }

    public MdsEvent(final Object source, final int event_id, final String info){
        super(source, event_id);
        this.info = new String(info);
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final String info){
        super(source, 0);
        this.info = new String(info);
        this.current_size = this.total_size = 0;
    }

    public MdsEvent(final Object source, final String info, final int total_size, final int current_size){
        super(source, MdsEvent.TRANSFER);
        this.total_size = total_size;
        this.current_size = current_size;
        this.info = info;
    }

    public String getInfo() {
        return this.info;
    }
}
