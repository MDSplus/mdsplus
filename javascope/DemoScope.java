import java.applet.Applet;
import java.awt.*;
import java.io.*;
import java.awt.event.*;



public class DemoScope extends Applet
{
    public MultiWaveform wave;
    Label label;
    Setup setup;
    //static DataProvider provider = new DataProvider("150.178.3.193");

    public void init()
    {
	WaveInterface wi = new WaveInterface(new NetworkProvider("150.178.3.193"));
	String param = getParameter("NAME");
	if(param.equals("WAVE1"))
	{
	    wi.in_x = new String[3];
	    wi.in_y = new String[3];
	    wi.in_y[0] = "\\A::EMRA_IT";
	    wi.in_y[1] = "\\A::EMRA_IT";
	    wi.in_y[2] = "\\A::EMRA_IT";
	    wi.experiment = "RFX";
	    wi.shots = new int[3];
	    wi.shots[0] = 9090;
	    wi.shots[1] = 9091;
	    wi.shots[2] = 9092;
	    wi.interpolates = new boolean[3];
	    wi.interpolates[0] = wi.interpolates[1] = wi.interpolates[2] = true;
	    wi.colors = new Color[3];
	    wi.colors[0] = Color.black;
	    wi.colors[1] = Color.blue;
	    wi.colors[2] = Color.cyan;
	    wi.in_xlabel = "\"time(sec)\"";
	    wi.in_ylabel = "\"Plasma Current(A)\"";
	    showStatus("Loading plasma current from RFX experiment...");
    	    wi.Evaluate();

    	    setup = new Setup(this);
	    setup.label_name = "COORD1";
	    wave = new MultiWaveform(setup);
	    setup.wave = wave;
    	    wave.Update(wi);
    	}
	
	if(param.equals("WAVE2"))
	{
	    double  x1[] = new double[8], x2[] = new double[8], 
		    y1[] = new double[8], y2[] = new double[8], y3[] = new double[8],
		    err[] = new double[8];
		    
	    x1[0] = x2[0] = 0;
	    x1[1] = x2[1] = 0.1;
	    x1[2] = x2[2] = 0.2;
	    x1[3] = x2[3] = 0.3;
	    x1[4] = x2[4] = 0.4;
	    x1[5] = x2[5] = 0.5;
	    x1[6] = x2[6] = 0.6;
	    x1[7] = x2[7] = 0.7;
	    
	    y1[0] = -0.1;
	    y1[1] = 0.18;
	    y1[2] = 0.21;
	    y1[3] = 0.28;
	    y1[4] = 0.301;
	    y1[5] = 0.39;
	    y1[6] = 0.42;
	    y1[7] = 0.61;

    	    y3[0] = -0.05;
	    y3[1] = 0.09;
	    y3[2] = 0.27;
	    y3[3] = 0.32;
	    y3[4] = 0.51;
	    y3[5] = 0.62;
	    y3[6] = 0.79;
	    y3[7] = 0.85;
	    for(int i = 0; i < 8; i++)
	    {
		y2[i] = i*0.1;
		err[i] = 0.05;
	    }
	
	    Signal signals[] = new Signal[3];
	    signals[0] = new Signal(x1, y1);
	    signals[1] = new Signal(x2, y2, err);
	    signals[2] = new Signal(x1, y3);
    	    setup = new Setup(this);
	    setup.label_name = "COORD2";
	    wave = new MultiWaveform(setup);
	    setup.wave = wave;
	    int markers[] = new int[3];
	    markers[0] = Waveform.TRIANGLE;
	    markers[1] = Waveform.CIRCLE;
	    markers[2] = Waveform.SQUARE;
	    boolean interpolates[] = new boolean[3];
	    interpolates[0] = interpolates[2] = false;
	    interpolates[1] = true;
    	    wave.Update(signals, 3, markers, interpolates);
	}
	
	if(param.equals("WAVE3"))
	{
	    wi.in_x = new String[1];
	    wi.in_y = new String[1];
	    wi.in_y[0] = "\\PBMV01_UM01VA";
	    wi.experiment = "POL";
	    wi.shots = new int[1];
	    wi.shots[0] = 9090;
	    wi.interpolates = new boolean[1];
	    wi.interpolates[0] = true;
	    int markers[] = new int[1];
	    markers[0] = Waveform.POINT;
	    wi.markers = markers;
	    showStatus("Loading waveform from RFX experiment...");
    	    wi.Evaluate();

    	    setup = new Setup(this);
	    setup.label_name = "COORD3";
	    wave = new MultiWaveform(setup);
	    wave.SetGridMode(Grid.IS_GRAY);

	    setup.wave = wave;

    	    wave.Update(wi);
	}

	setLayout(new GridLayout(1,1));
	if(wave != null)
	    add(wave);
	validate();
    }

}

class Setup extends Object implements WaveSetup {
    Label coords_label; 
    Button3Menu menu;	
    public MultiWaveform wave;
    public String label_name;
    Applet  app;

    public Setup(Applet _app)
    {
	app = _app;
	coords_label = null;
	//menu = new Button3Menu(wave);
	//pw.add(menu);
    }
    

    public void SetSetup(Waveform w, int x, int y)
    {
    	//menu.Show((MultiWaveform)w, x, y);
    }
    
    public void BroadcastScale(Waveform w){}

    public void NotifyChange(Waveform w1, Waveform w2) {}
    
    public void DisplayCoords(Waveform w, double curr_x, double curr_y, int i, boolean b)
    {
	DisplayCoords(w, curr_x, curr_y);
    }
    public void DisplayCoords(Waveform w, double curr_x, double curr_y)
    { 
	if(coords_label == null)
	{
	    Applet coords_app = app.getAppletContext().getApplet(label_name);
	    coords_label = ((ScopeCoord)coords_app).label;
	}
	else
	{   
  	    coords_label.setText("[" + (new Float(curr_x)).toString() + ", " +
		    (new Float(curr_y)).toString() + "]");
	}	
    }

    public void Hide(){	
    }    	
 }   


class Button3Menu extends PopupMenu implements ActionListener {
	MultiWaveform   wave;
	MenuItem autoscale, zoom, point, pan;
	int curr_x, curr_y;

    public Button3Menu(MultiWaveform _wave)
    {
/*	wave = _wave;
	add(autoscale = new MenuItem("Autoscale"));
	autoscale.addActionListener(this);
	add(zoom = new MenuItem("Zoom"));
	zoom.addActionListener(this);
	add(point = new MenuItem("Point"));
	point.addActionListener(this);
	add(pan = new MenuItem("Pan"));
	pan.addActionListener(this);
*/    }
	
    public void Show(MultiWaveform w, int x, int y)
    {
	curr_x = x;
	curr_y = y;
	wave   = w;
	show(w, x, y );	
     }

    public void actionPerformed(ActionEvent e)
    {
	Object target = e.getSource();

		
    	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
	    wave.repaint();
	}
	if(target == (Object)zoom)
	{
	    wave.SetMode(Waveform.MODE_ZOOM);
	    wave.repaint();
	}
	if(target == (Object)point)
	{
	    wave.SetMode(Waveform.MODE_POINT);
	    wave.repaint();
	}
	if(target == (Object)pan)
	{
	    wave.SetMode(Waveform.MODE_PAN);
	    wave.repaint();
	}
    }

}

/*

public class WaveInterface
{
    public int num_waves;
    public boolean x_log, y_log;
    public String in_x[], in_y[], in_up_err[], in_low_err[];
    public String in_xmin, in_xmax, in_ymax, in_ymin;
    public int markers[];
    public Color colors[];
    public boolean interpolates[];
    public String experiment; 
    public String in_title, in_xlabel, in_ylabel; 
    public int shots[];
    public String error;
    
    public Signal signals[];
    public double xmax, xmin, ymax, ymin; 
    public String title, xlabel, ylabel;
    private DataProvider dp;
// Used for asynchronous Update  
    public boolean asynch_update;  
    Signal wave_signals[];
    double wave_xmin, wave_xmax;
    int wave_timestamp;
    AsynchUpdater du;
    boolean request_pending;
    double orig_xmin, orig_xmax;
    
*/
