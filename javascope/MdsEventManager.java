import java.io.*;
import java.util.Vector;


class MdsEventManager {
    
    
    private Vector mdsEventList     = new Vector();

    static class MdsEventItem
    {
        String  name;
        Vector  listener = new Vector();
    
        MdsEventItem (String name, MdsEventListener l)
        {
            this.name = name;
            listener.addElement((Object) l);
        }
    
        public String toString()
        {
            return name;
        }
    }
    
    public synchronized int AddEvent(MdsEventListener l, String event_name)
    {
       int i, eventid = -1;
       MdsEventItem mdsEventItem; 
        
        for(i = 0; i < mdsEventList.size() && 
                        !((MdsEventItem)mdsEventList.elementAt(i)).name.equals(event_name);
        /*System.out.println("Add "+((MdsEventItem)mdsEventList.elementAt(i)).name+" "+i),*/ i++);
        if(i == mdsEventList.size())
        {
            mdsEventItem = new MdsEventItem(event_name, l);
            mdsEventList.addElement((Object)mdsEventItem);
	        eventid = mdsEventList.size() - 1;
        } else {
            if(!((MdsEventItem)mdsEventList.elementAt(i)).listener.contains((Object)l))
                ((MdsEventItem)mdsEventList.elementAt(i)).listener.addElement(l);
        }     

        return eventid;
    }
    
    public synchronized int RemoveEvent(MdsEventListener l, String event_name)
    {
        int i, eventid = -1;
        MdsEventItem mdsEventItem; 

    
        for(i = 0; i < mdsEventList.size() && 
                        !((MdsEventItem)mdsEventList.elementAt(i)).name.equals(event_name);
            /*System.out.println("Remove "+((MdsEventItem)mdsEventList.elementAt(i)).name+" "+i),*/i++);
        if(i < mdsEventList.size())
        {
            ((MdsEventItem)mdsEventList.elementAt(i)).listener.removeElement(l);
            if(((MdsEventItem)mdsEventList.elementAt(i)).listener.size() == 0)
            {
                mdsEventList.removeElementAt(i);
                eventid = i;
            }
        }     
        return eventid;        
    }
    
    public synchronized void FireEvent(int eventid)
    {
        MdsEventItem mdsEventItem = ((MdsEventItem)mdsEventList.elementAt(eventid));
        Vector mdsEventListener = mdsEventItem.listener;
        MdsEvent e = new MdsEvent(this, mdsEventItem.name, eventid);
    
//        System.out.println("Processo Evento nuovo " + mdsEventItem);
        for(int i = 0; i < mdsEventListener.size(); i++)
	        ((MdsEventListener)mdsEventListener.elementAt(i)).processMdsEvent(e);
    }   
}
