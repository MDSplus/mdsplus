import java.awt.AWTEvent;
import java.awt.Event;

public class MdsEvent extends AWTEvent {

    String name;
    int    idx;
    Object source;

    /**
     * Constructs an MdsEvent object with the specified source object.
     * @param source the object where the event originated
     * @param id the type of event
     * @param command the command string for this action event
     */
    public MdsEvent(Object source, String event, int id) {
        super(source, id);
        name = event;
        idx = id;
    }

 
    public String paramString() {
        return new String("Event name : " + name + " event id : " + idx);
    }
}
