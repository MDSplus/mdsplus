import java.awt.AWTEvent;
import java.awt.Event;

public class ConnectionEvent extends AWTEvent 
{
    public static final int LOST_CONNECTION = AWTEvent.RESERVED_ID_MAX + 1;

    int    total_size;
    int    current_size;
    String info;

    public ConnectionEvent(Object source, String info, int total_size, int current_size) 
    {
        super(source, 0);
        this.total_size   = total_size;
        this.current_size = current_size;
        this.info = info;
    }

    public ConnectionEvent(Object source, int total_size, int current_size) 
    {
        super(source, 0);
        this.total_size   = total_size;
        this.current_size = current_size;
        this.info = null;
    }

    public ConnectionEvent(Object source, String info) 
    {
        super(source, 0);
        this.info = new String(info);
    }

    public ConnectionEvent(Object source, int event_id, String info) 
    {
        super(source, event_id);
        this.info = new String(info);
    }
}
