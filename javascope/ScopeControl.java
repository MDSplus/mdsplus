
import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;

public class ScopeControl extends Applet
{
    public Label label;
    Applet scope;
    Checkbox zoom, point, pan;
    Button autoscale;
    String param;
    public void init()
    {
	setLayout(new GridLayout(1,4));
	CheckboxGroup cbg = new CheckboxGroup();
	zoom = new Checkbox("Zoom", cbg, true);
	point = new Checkbox("Point", cbg, false);
	pan = new Checkbox("Pan", cbg, false);
	autoscale = new Button("Autoscale");	
	add(zoom);
	add(point);
	add(pan);
	add(autoscale);
	scope = null;
	param = getParameter("NAME");
	validate();
    }

    
    public boolean action(Event e, Object arg)
    {
	Object target = e.target;
	MultiWaveform wave;  
	
	if(scope == null)
	{ 
	    if(param.equals("CONTR1"))
		scope = getAppletContext().getApplet("WAVE1");
	    if(param.equals("CONTR2"))
		scope = getAppletContext().getApplet("WAVE2");
	    if(param.equals("CONTR3"))
		scope = getAppletContext().getApplet("WAVE3");
	}
	wave = ((DemoScope)scope).wave;
	if(target == (Object)zoom)
	{
	    System.out.println("zoom"+wave);
	    wave.SetMode(Waveform.MODE_ZOOM);
	    wave.repaint();
	    return true;
	}
	if(target == (Object)point)
	{
	    wave.SetMode(Waveform.MODE_POINT);
	    wave.repaint();
	    return true;
	}
	if(target == (Object)pan)
	{
	    wave.SetMode(Waveform.MODE_PAN);
	    wave.repaint();
	    return true;
	}
    	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
	    wave.repaint();
	    return true;
	}
    return false;	
    }
    
}

