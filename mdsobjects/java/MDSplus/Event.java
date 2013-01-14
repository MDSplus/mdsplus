package MDSplus;

public class Event 
{ 
    long time = 0;
    Data data = null;
    byte[] dataBuf;
    java.lang.String name;
    boolean disposed = false;
    long eventId;
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
    public byte []  getRaw() { return dataBuf;}
    public void run(){}
    public void dispose()
    {   
        if(disposed) return;
        disposed = true;
        unregisterEvent(eventId); 
    }

    static public void setEvent(java.lang.String evName)
    {
        setEventRaw(evName, new byte[0]);
    }
    static public void setEvent(java.lang.String evName, Data data)
    {
        setEventRaw(evName, (data == null)?new byte[0]:data.serialize());
    }
     static public void setevent(java.lang.String evName)
    {
        setEvent(evName);
    }
    static public void setevent(java.lang.String evName, Data data)
    {
        setEvent(evName, data);
    }
   void intRun(byte []buf, long time)
    {
        this.time = time;
        dataBuf = buf;
        if(buf.length == 0)
            data = null;
        else
        {
            try {
                data = Data.deserialize(buf);
            }catch(Exception exc){} //No handling required, may be raw data
        }
        run();
    }
    static public void seteventRaw(java.lang.String evName, byte[] buf){setEventRaw(evName, buf);}
    static public native void setEventRaw(java.lang.String evName, byte[] buf);
    native long registerEvent(java.lang.String name);
    native void unregisterEvent(long eventId);
}
