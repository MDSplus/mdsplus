import java.io.*;
import java.util.Vector;


class NetworkEventManager {
    static final int MAX_NUM_EVENTS = 256;    
    private boolean event_flags[] = new boolean[MAX_NUM_EVENTS];
    private Vector event_list     = new Vector();

    static class EventItem
    {
        String  name;
        int     eventid;
        Vector  listener = new Vector();
    
        EventItem (String name, int eventid, NetworkEventListener l)
        {
            this.name = name;
            this.eventid = eventid;
            listener.addElement((Object) l);
        }
    
        public String toString()
        {
            return new String("Event name = "+ name + " Event id = " + eventid);
        }
    }
    
    private int getEventId()
    {
        int i;
        for(i=0;i<MAX_NUM_EVENTS && event_flags[i];i++);
        if(i == MAX_NUM_EVENTS)
            return -1;
        event_flags[i] = true;    
        return i;            
    }

   
    public synchronized int AddEvent(NetworkEventListener l, String event_name)
    {
       int i, eventid = -1;
       EventItem event_item; 
        
        for(i = 0; i < event_list.size() && 
             !((EventItem)event_list.elementAt(i)).name.equals(event_name);i++);
        if(i == event_list.size())
        {
	        eventid = getEventId();
            event_item = new EventItem(event_name, eventid, l);
            event_list.addElement((Object)event_item);
            //System.out.println("Add "+mdsEventItem);
	        //eventid = mdsEventList.size() - 1;
        } else {
            if(!((EventItem)event_list.elementAt(i)).listener.contains((Object)l))
            {
                ((EventItem)event_list.elementAt(i)).listener.addElement(l);
//                System.out.println("Add listener to event "+event_name);
            }
        }     

        return eventid;
    }
    
    public synchronized int RemoveEvent(NetworkEventListener l, String event_name)
    {
        int i, eventid = -1;
        EventItem event_item; 

    
        for(i = 0; i < event_list.size() && 
              !((EventItem)event_list.elementAt(i)).name.equals(event_name);i++);
        if(i < event_list.size())
        {
            ((EventItem)event_list.elementAt(i)).listener.removeElement(l);
            //System.out.println("Remove listener to event "+((EventItem)mdsEventList.elementAt(i)));
            if(((EventItem)event_list.elementAt(i)).listener.size() == 0)
            {
                eventid = ((EventItem)event_list.elementAt(i)).eventid;
                event_flags[eventid]  = false;
               // System.out.println("Remove "+((EventItem)mdsEventList.elementAt(i)));
                event_list.removeElementAt(i);
            }
        }     
        return eventid;        
    }
    
    public synchronized void FireEvent(int eventid)
    {
        int i;

        
        for(i = 0; i < event_list.size() && 
                        ((EventItem)event_list.elementAt(i)).eventid != eventid; i++);
                        
        if(i > event_list.size()) return;
        EventItem event_item = ((EventItem)event_list.elementAt(i));
        Vector event_listener = event_item.listener;
        NetworkEvent e = new NetworkEvent(this, event_item.name, eventid);
    
        for(i = 0; i < event_listener.size(); i++)
	        ((NetworkEventListener)event_listener.elementAt(i)).processNetworkEvent(e);
    }   
}
