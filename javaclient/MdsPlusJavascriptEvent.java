package MdsPlus;
import java.applet.*;
import java.lang.Object;
import netscape.javascript.*;

/*
 *
 * MdsplusJavascriptEvent
 *
 */
/** This class supports the execution of javascript when an MdsPlus event occurs on
* the remote server.<BR>
* <B>Note: This class uses the netscape.javascript package an therefore only operates on
* Netscape browsers.</B>
*
* <pre>
* Usage:
*
* &lt;Applet mayscript code=MdsPlus.MdsPlusJavascriptEvent width=0 height=2&gt;
* &lt;Param name=event value=my_mdsplus_event&gt;
* &lt;Param name=function value="my_javascript_function"&gt;
* &lt;/Applet&gt;
*
* or:
*
* &lt;Applet mayscript code=MdsPlus.MdsPlusJavascriptEvent width=0 height=2&gt;
* &lt;Param name=event value=my_mdsplus_event&gt;
* &lt;Param name=statement value='alert("Got Event");'&gt;
* &lt;/Applet&gt;
*
*/
public class MdsPlusJavascriptEvent extends Applet implements MdsPlusEvents
{
	MdsPlusEvent ast;
	public void init()
	{
		show();
	}
	public void start()
	{
		try{
		ast = new MdsPlusEvent(getCodeBase(), getParameter("event"), this, (Object)null);
		}
		catch (Exception e){System.out.println(e);}
	}
	public void stop()
	{
		ast.Cancel();
	}
	public void EventAction(Object arg, byte[] ev_data)
	{
		if (getParameter("function") != null)
		{
			Object args[] = new Object[2];
			args[0] = getParameter("event");
			args[1] = ev_data;
			try {
  			  JSObject.getWindow(this).call(getParameter("function"),args);
			}
			catch (Exception e) {System.out.println(e);}
		}
		if (getParameter("statement") != null)
		{
			try {
				JSObject.getWindow(this).eval(getParameter("statement"));
			}
			catch (Exception e) {System.out.println(e);}
		}
	}
}

