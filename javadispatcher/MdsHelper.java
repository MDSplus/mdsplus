import java.util.*;
import java.io.*;
class MdsHelper 
{
    static Hashtable name_to_id = new Hashtable(), id_to_name = new Hashtable();
    static String dispatcher_ip = null;
    static {
        Properties properties = new Properties(); 
        try {
            properties.load(new FileInputStream("jDispatcher.properties"));
            int i = 1;
            while(true)
            {
                String phase_name = properties.getProperty("jDispatcher.phase_"+i+".name");
                if(phase_name == null) break;
                String phase_id = properties.getProperty("jDispatcher.phase_"+i+".id");
                if(phase_id == null) break;
                id_to_name.put(new Integer(phase_id), phase_name);
                name_to_id.put(phase_name, new Integer(phase_id));
                i++;
            }
            dispatcher_ip = properties.getProperty("jDispatcher.dispatcher_ip");
        }catch(Exception exc)
        {
            System.err.println("Cannot open phases.properties");
        }
    }
            
            
            
    public static native String getErrorString(int status); 
    public static native void generateEvent(String event, int shot);
 //   public static String getErrorString(int status){return "Error message not yet implemented"; }
    public static int toPhaseId(String phase_name)
    {
        try{
            return ((Integer)name_to_id.get(phase_name)).intValue();
        }catch(Exception exc) {return -1; }
    }
    
    public static String toPhaseName(int phase_id)
    {
        return (String)id_to_name.get(new Integer(phase_id));
    }
    
    public static String getDispatcher()
    {
        return dispatcher_ip;
    }
}