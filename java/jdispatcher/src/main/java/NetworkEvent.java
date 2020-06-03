import java.awt.AWTEvent;

public class NetworkEvent extends AWTEvent {

    String name;
    int    idx;
    Object source;

    public NetworkEvent(Object source, String event, int id) {
	super(source, id);
	name = event;
	idx = id;
    }

    public String paramString() {
	return new String("Event name : " + name + " event id : " + idx);
    }
}
