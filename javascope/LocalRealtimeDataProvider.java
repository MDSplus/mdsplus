import javax.swing.JFrame;
import java.io.IOException;
import java.util.*;

public class LocalRealtimeDataProvider extends LocalDataProvider
{

    Hashtable eventHash = new Hashtable();
    Hashtable nidHash = new Hashtable();
    Vector listeners = new Vector();

    static class EventDescr
    {
        String event;
        UpdateEventListener l;
        EventDescr(String event, UpdateEventListener l)
        {
            this.event = event;
            this.l = l;
        }
    }


    public void AddUpdateEventListener(UpdateEventListener l, String event)
    {
        int nid;
        try {
            nid = Integer.parseInt((String)nidHash.get(event));
        }catch(Exception exc)
        {
            nid = getRealtimeNidOf(event);
            if(nid > 0)
            {
                nidHash.put(event, new Integer(nid));
                setRealtimeCallback(nid);
                eventHash.put(new Integer(nid), event);
            }
        }
        listeners.addElement(new EventDescr(event, l));
    }

    public void RemoveUpdateEventListener(UpdateEventListener l, String event){}


    public void fireEvent(int nid)
    {
        String event = (String)eventHash.get(new Integer(nid));
        for(int idx = 0; idx < listeners.size(); idx++)
        {
            EventDescr evDescr = (EventDescr)listeners.elementAt(idx);
            if(evDescr.event.equals(event))
                evDescr.l.processUpdateEvent(new UpdateEvent(this, event));
        }
    }

    public native int getRealtimeNidOf(String event);
    public native void setRealtimeCallback(int nid);
    native public void Update(String exp, long s);
}
