import java.awt.AWTEvent;
import java.awt.Event;

public class NetworkEvent extends AWTEvent {

    String name;
    int    idx;
    Object source;
    int    total_size;
    int    current_size;
    String info;
    /**
     * Constructs an MdsEvent object with the specified source object.
     * @param source the object where the event originated
     * @param id the type of event
     * @param command the command string for this action event
     */
    public NetworkEvent(Object source, String event, int id) {
        super(source, id);
        name = event;
        idx = id;
    }

    public NetworkEvent(Object source, int  total_size, int current_size) 
    {
        super(source, 0);
        this.total_size   = total_size;
        this.current_size = current_size;
        this.info = null;
    }

    public NetworkEvent(Object source, String info) 
    {
        super(source, 0);
        this.info = new String(info);
    }
 
    public String paramString() {
        return new String("Event name : " + name + " event id : " + idx);
    }
}
