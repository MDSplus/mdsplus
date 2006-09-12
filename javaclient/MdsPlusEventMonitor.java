/* Decompiled by Mocha from Reloader.class */
/* Originally compiled from Reloader.java */
package MdsPlus;

//import netscape.javascript.*;
import MdsPlus.MdsPlusEvent;
import MdsPlus.MdsPlusEvents;
import java.lang.String;
import java.lang.reflect.*;
import java.awt.Component;
import java.applet.Applet;
import java.applet.AppletContext;
import java.io.PrintStream;
import java.net.URL;
import java.io.ByteArrayOutputStream;
public class MdsPlusEventMonitor extends Applet implements MdsPlusEvents
{
    private int m_eventPort;
    private String m_host;
    private String m_eventName;
    private MdsPlusEvent m_event;
    private String m_jscript;
    private String m_jscript_function;
    private boolean m_started;
    public static final long serialVersionUID=1L;

    public MdsPlusEventMonitor()
    {
    }

    public void EventAction(Object object, byte ab[])
    {
        if (m_started) {
            if (m_jscript_function != null) {
		String js;
		js = "javascript: " + m_jscript_function + "('" + (new String(ab)).trim() + "',new Array(";
		for (int i=0;i<11;i++) js +=  "" + ab[i]+",";
                js += "" + ab[11] + "));";
	    	try
		    {
			getAppletContext().showDocument(new URL(js));
	    	    }
	            catch (java.net.MalformedURLException e)
	    	    {
			System.out.println(e);
	    	    }
	    }
	    else {
		try {
		    String js;
		    js = "var MdsEventDataString='" + (new String(ab)).trim() + "'; " +
			"var MdsEventData = new Array(12); ";
		    for (int i=0;i<12;i++)
			js += "MdsEventData[" + i + "]="+ab[i]+"; ";
		    js += m_jscript;
		    Object a[] = new Object[1];
		    Object jswin = null;
		    Method getw = null, eval = null;
		    Class c = Class.forName("netscape.javascript.JSObject");
		    Method ms[] = c.getMethods();
		    for (int i = 0; i < ms.length; i++) {
			if (ms[i].getName().compareTo("eval") == 0)
			    eval = ms[i];
			if (ms[i].getName().compareTo("getWindow") == 0)
			    getw = ms[i];
		    }
		    a[0] = this;
		    jswin = getw.invoke(c, a);
		    a[0] = js;
		    eval.invoke(jswin,a);
		}
		catch (Exception e) {
		    System.out.println(e);
		}
	    }
	}
    }

    public void destroy()
    {
        System.out.println("I am in destroy");
        m_event.Cancel();
    }

    public String getAppletInfo()
    {
        return "Name:    MdsPlusEventMonitor\r\nVersion: 1.0\r\nAuthor:  Tom Fredian\r\nCopyright 2005 Massachusetts Institute of Technology";
    }

    public String[][] getParameterInfo()
    {
        String aastring[][] = { { "host", "String", "host serving the events" }, { "port", "int", "port number to connect to (8001)" }, { "EventName", "String", "Name of the event to reload on" } };
        return aastring;
    }

    public void init()
    {
        System.out.println("I am in init");
        Component c;
        m_started = false;
        usePageParams();
        try
        {
            m_event = new MdsPlusEvent(m_host, m_eventPort, m_eventName, this, null);
        }
        catch (Exception e)
        {
            System.out.println(e);
            e.printStackTrace();
        }
    }

    public void start()
    {
        System.out.println("I am in start");
        m_started = true;
    }

    public void stop()
    {
        System.out.println("I am in stop");
        m_started = false;
    }

    private void usePageParams()
    {
        m_host = getCodeBase().getHost();
        if (m_host == null || m_host.length() == 0)
            m_host = "localhost";
        m_eventName = getParameter("eventName");
        if (m_eventName == null)
            m_eventName = "LOGBOOK_EVENT";
        m_jscript = getParameter("jscript");
        m_jscript_function = getParameter("jscript_function");
        try
        {
            m_eventPort = Integer.parseInt(getParameter("eventPort"));
        }
        catch (Exception e1)
        {
            m_eventPort = 8001;
        }
    }
}
