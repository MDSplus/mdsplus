/* Decompiled by Mocha from Reloader.class */
/* Originally compiled from Reloader.java */
package MdsPlus;

import netscape.javascript.*;
import MdsPlus.MdsPlusEvent;
import MdsPlus.MdsPlusEvents;
import java.lang.String;
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
    private boolean m_started;

    public MdsPlusEventMonitor()
    {
    }

    public void EventAction(Object object, byte ab[])
    {
        if (m_started)
        {
            try
            {
                JSObject window=JSObject.getWindow(this);
                try {
		    window.eval("var MdsEventDataString='" + (new String(ab)).trim() + "';");
		} catch (JSException e) {
		}
                window.eval("var MdsEventData = new Array(12);");
                for (int i=0;i<12;i++)
		    window.eval("MdsEventData[" + i + "]="+ab[i]+";");
                window.eval(m_jscript);
            }
            catch (JSException e)
            {
                if (e.getWrappedException()==null)
		    System.out.println(e);
		else
		    System.out.println(e.getWrappedException());
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
