import java.io.*;
import java.util.Vector;


class MdsEventManager {
    static final int MAX_NUM_EVENTS = 256;    
    private boolean event_flags[] = new boolean[MAX_NUM_EVENTS];
    private Vector mdsEventList     = new Vector();

    static class EventItem
    {
        String  name;
        int     eventid;
        Vector  listener = new Vector();
    
        EventItem (String name, int eventid, MdsEventListener l)
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

   
    public synchronized int AddEvent(MdsEventListener l, String event_name)
    {
       int i, eventid = -1;
       EventItem mdsEventItem; 
        
        for(i = 0; i < mdsEventList.size() && 
             !((EventItem)mdsEventList.elementAt(i)).name.equals(event_name);i++);
        if(i == mdsEventList.size())
        {
	        eventid = getEventId();
            mdsEventItem = new EventItem(event_name, eventid, l);
            mdsEventList.addElement((Object)mdsEventItem);
            //System.out.println("Add "+mdsEventItem);
	        //eventid = mdsEventList.size() - 1;
        } else {
            if(!((EventItem)mdsEventList.elementAt(i)).listener.contains((Object)l))
            {
                ((EventItem)mdsEventList.elementAt(i)).listener.addElement(l);
                System.out.println("Add listener to event "+event_name);
            }
        }     

        return eventid;
    }
    
    public synchronized int RemoveEvent(MdsEventListener l, String event_name)
    {
        int i, eventid = -1;
        EventItem mdsEventItem; 

    
        for(i = 0; i < mdsEventList.size() && 
              !((EventItem)mdsEventList.elementAt(i)).name.equals(event_name);i++);
        if(i < mdsEventList.size())
        {
            ((EventItem)mdsEventList.elementAt(i)).listener.removeElement(l);
            //System.out.println("Remove listener to event "+((EventItem)mdsEventList.elementAt(i)));
            if(((EventItem)mdsEventList.elementAt(i)).listener.size() == 0)
            {
                eventid = ((EventItem)mdsEventList.elementAt(i)).eventid;
                event_flags[eventid]  = false;
               // System.out.println("Remove "+((EventItem)mdsEventList.elementAt(i)));
                mdsEventList.removeElementAt(i);
            }
        }     
        return eventid;        
    }
    
    public synchronized void FireEvent(int eventid)
    {
        int i;

        
        for(i = 0; i < mdsEventList.size() && 
                        ((EventItem)mdsEventList.elementAt(i)).eventid != eventid; i++);
                        
        if(i > mdsEventList.size()) return;
        EventItem mdsEventItem = ((EventItem)mdsEventList.elementAt(i));
        Vector mdsEventListener = mdsEventItem.listener;
        MdsEvent e = new MdsEvent(this, mdsEventItem.name, eventid);
    
        for(i = 0; i < mdsEventListener.size(); i++)
	        ((MdsEventListener)mdsEventListener.elementAt(i)).processMdsEvent(e);
    }   
}
