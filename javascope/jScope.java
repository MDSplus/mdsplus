import java.util.Properties;

public class jScope
{
    static final String VERSION = "jScope (version 7.1)";
    static public boolean is_debug = false;
    
    static boolean IsNewJVMVersion()
    {
        String ver = System.getProperty("java.version");
        return (! (ver.indexOf("1.0") != -1 || ver.indexOf("1.1") != -1) );
    }
  
    public static void main(String args[])
    {
        String file = null;    
                
        jScope_1 win = null;
 
        Properties props = System.getProperties();
        String debug = props.getProperty("debug");
        if(debug != null && debug.equals("true"))
            is_debug = true;
 
        if(System.getProperty("os.name").equals("Mac OS"))
        {
            try
            {
                Class cl = Class.forName("jScopeMac");
                win = (jScope_1)cl.newInstance();
            } 
            catch (Exception e)
            {}
        }
        else
        {
            if(IsNewJVMVersion())
            {
                win = new jScope_2(100, 100);
            }
            else
                win = new jScope_1(100, 100);
        }                    
        win.pack();                 
	    win.setSize(750, 550);
        if(args.length == 1) 
            file = new String(args[0]); 
        
        win.num_scope++;
        win.startScope(file);
        
    }
}