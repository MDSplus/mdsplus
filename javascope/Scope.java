import java.applet.Applet;
import java.awt.*;
import java.io.*;

import Setup;
import Waveform;
import MultiWaveform;
import PopScopes;


public class Scope extends Panel
{
    public Setup setup;	
    public MultiWaveform wave1, wave2;
    Signal signal;
    CheckboxGroup cbg;
    Checkbox point_cb, zoom_cb;
    Label coords, signame_l, shot_l;
    TextField signame, shot;
    Button apply;
	public PopScopes controller;


    public Scope(Frame pw)
    {
	coords = new Label();
	setup = new Setup(pw, coords);
	wave1 = new MultiWaveform(setup);
	wave2 = new MultiWaveform(setup);
//	wave1.marker = Waveform.CIRCLE;
//	wave2.marker = Waveform.TRIANGLE;
//	wave1.interpolate = wave2.interpolate = false;
	setup.waves = new MultiWaveform[2];
	setup.waves[0] = wave1;
	setup.waves[1] = wave2;
	setup.num_waves = 2;
	Build(null, 0);
    }


	public void UpdatePoint(double curr_x)
	{
		wave1.UpdatePoint(curr_x);
	}

	public void Synchronize(boolean on)
	{
	}

/*	public void Update(String name, int shot_n)
	{
		Integer n = new Integer(shot_n);
		Signal sig = new Signal(name, shot_n);
		signame.setText(name);
		shot.setText(n.toString());
		wave1.Update(sig);
	}
*/		
    void Build(String signam, int sh)
    {
	Panel p = new Panel();
	GridBagLayout gridbag = new GridBagLayout();
	GridBagConstraints c = new GridBagConstraints();
	setLayout(gridbag);
	c.fill = GridBagConstraints.BOTH;
	c.weighty = 1.0;
	c.weightx = 1.0;
	c.gridheight = 5;
	c.gridwidth = GridBagConstraints.REMAINDER;
	p.setLayout(new GridLayout(1,2));
	p.add(wave1);
	p.add(wave2);
	gridbag.setConstraints(p, c);
//	gridbag.setConstraints(wave1, c);
	add(p);
	p.add(wave2);
//	gridbag.setConstraints(p, c);
//	add(p);
	c.weighty = 0.0;
	c.gridheight = 1;
	cbg = new CheckboxGroup();
	point_cb = new Checkbox("Point", cbg, false);
	c.gridwidth = 1;
	gridbag.setConstraints(point_cb, c);
	add(point_cb);
	zoom_cb = new Checkbox("Zoom", cbg, true);
	gridbag.setConstraints(zoom_cb, c);
	add(zoom_cb);
	c.fill = GridBagConstraints.VERTICAL;
	c.anchor = GridBagConstraints.EAST;
	shot_l = new Label("Shot: ");
	c.gridwidth = GridBagConstraints.RELATIVE;
	gridbag.setConstraints(shot_l, c);
	add(shot_l);
	if(signam != null)
	{
	    Integer ic = new Integer(sh);
	    shot = new TextField(ic.toString(), 5);
	}
	else
	    shot = new TextField(5);
	c.fill = GridBagConstraints.BOTH;
	c.gridwidth = GridBagConstraints.REMAINDER;
	gridbag.setConstraints(shot, c);
	add(shot);
	c.anchor = GridBagConstraints.CENTER;
	c.gridwidth = 1;
	signame_l = new Label("Signal: ");
	gridbag.setConstraints(signame_l, c);
	add(signame_l);
	if(signam != null)
	    signame = new TextField(signam, 30);
	else
	    signame = new TextField(30);
	c.gridwidth = 2;
	gridbag.setConstraints(signame, c);
	add(signame);
	c.gridwidth = GridBagConstraints.REMAINDER;
	apply = new Button("Apply");
	gridbag.setConstraints(apply, c);
	add(apply);

	gridbag.setConstraints(coords, c);
	add(coords);
    }

    public boolean action(Event e, Object arg)
    {
	Object target = e.target;
	if(target == zoom_cb)
	{
	    wave1.SetMode(Waveform.MODE_ZOOM);
	    wave2.SetMode(Waveform.MODE_ZOOM);
	    coords.setText("");
	    return true;
	}
	if(target == point_cb)
	{
	    wave1.SetMode(Waveform.MODE_POINT);
	    wave2.SetMode(Waveform.MODE_POINT);
	    return true;
	}
	if(target == apply)
	{
	    String 
		shot_name = shot.getText(),
		sig_name = signame.getText();
	    Integer inte;

System.out.println(shot_name + "\n" + sig_name + "\n");

	    if(shot_name.length() == 0 || sig_name.length() == 0)
		return true;
	    inte = new Integer(sig_name.length());
System.out.println(inte.toString() + "\n");

	    inte = new Integer(shot_name);
	    int shotnum = inte.intValue();
	    try {
//	    	signal = new Signal(sig_name, shotnum);
		double x[] = new double[1000], y1[] = new double[1000], y2[] = new double[1000];
		for(int i = 0; i < 1000; i++)
		{
		    x[i] = i/(double)100;
		    y1[i] = (double)Math.sin(x[i]);
	  	    y2[i] = (double)Math.cos(x[i]);
		}

		Signal signals[] = new Signal[2];
		signals[0] = new Signal(x, y1, 1000);
		signals[1] = new Signal(x,y2, 1000);
	    	wave1.Update(signals, 2);
	    	wave2.Update(signals, 2);
	    } catch (Exception ex) {
		System.err.println("Errore in Apply: " + ex + "\n"); }
	    return true;
	}
	return false;
    }
}  