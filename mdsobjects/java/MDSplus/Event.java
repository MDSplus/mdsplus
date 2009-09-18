package MDSplus;

public class Event 
{ 
    long time = 0;
    Data data = null;
    java.lang.String name;
    boolean disposed = false;
    int eventId;
    static {
        try {
          System.loadLibrary("JavaMds");
        }catch(Throwable e)
        {
              System.out.println("Error loading library javamds: "+e);
              e.printStackTrace();
        }
    }
    
    public Event(java.lang.String name) throws MdsException
    {
        this.name = name;
        eventId = registerEvent(name);
        if(eventId == -1)
            throw new MdsException("Cannot Register to event " + name);
    }
    public java.lang.String getName() {return name;}
    public Uint64 getTime() {return new Uint64(time);}
    public Data getData() { return data;}
    public void run(){}
    public void dispose()
    {   
        if(disposed) return;
        disposed = true;
        unregisterEvent(eventId); 
    }
    static public void setevent(java.lang.String evName, Data data)
    {
        sendEvent(evName, (data == null)?new byte[0]:data.serialize());
    }
    void intRun(byte []buf, long time)
    {
        if(buf.length == 0)
            data = null;
        else data = Data.deserialize(buf);
        run();
    }
    static native void sendEvent(java.lang.String evName, byte[] buf);
    native int registerEvent(java.lang.String name);
    native void unregisterEvent(int eventId);
}
