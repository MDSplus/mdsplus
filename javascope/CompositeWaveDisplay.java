/* $Id$ */
import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import java.lang.NumberFormatException;
import java.util.*;
import javax.swing.*;
import java.security.AccessControlException;
import java.awt.print.*;

public class CompositeWaveDisplay extends JApplet implements WaveContainerListener
{
    private WaveformContainer wave_container;
    private boolean           automatic_color = false;
    private boolean           isApplet = true;
    private JLabel            point_pos;
    private int               print_scaling = 100;
    private boolean           fixed_legend = false;
    static  private JFrame    f = null;
    PrinterJob                prnJob;
    PageFormat                pf;


//A SAMPLE SESSION ILLUSTRATING THE USAGE OF COMPOSITE WAVE DISPLAY
    public static void main(String args[])
    {
        //Create a new instance of CompositeWaveDisplay
        CompositeWaveDisplay cd = createWindow("test");
        //Add signals (see below)
        testSignal((CompositeWaveDisplay)cd);
        //Display Window
        cd.showWindow(50, 50, 500, 400);
    }
    
    
    private static void testSignal(CompositeWaveDisplay cd)
    {
        //Create a couple of signals
        float y[] = new float[1000], y1[] = new float[1000], y2[] = new float[1000], x [] = new float[1000];
        for(int i = 0; i < 1000; i++)
        {
            x[i] = (float)(i/1000.);
            y[i] = (float)Math.sin(i/300.);
            y1[i] = (float)Math.cos(i/300.);
        }
        //A sample ceation of two static signals in one panel
        cd.addSignal(
            x,          //X Axis
            y,          //Y Axis
            1,          //row in panel
            2,          //Column in panel
            "blue",     //Colour
            "my signal1", //Legend
            true,       //Display lines connecting points
            0);         //Marker step: 0 means that all points are displayed
       //Another signal in the same panel 
        cd.addSignal(x,y1,1,2,"green", "my signal 2", true, 0);
            
        //Another definition for signal. In this case a URL like specification is provided. 
        //The first part defines the DataProvider to be used. In this case the demo data provider is used
        //Demo is a sample data provider already defined in jScope, which has the ability 
        //of showing signals evolving in time. The second part of the URL is a name which is
        //interpreted by the associated Data Provider 
        cd.addSignal(
            "demo://sin",           //Signal Specification: <DataProvider>://<name>
            1,                      //row in panel
            1,                      //Column in panel
            "red",                  //Colour
            "evolving signal",      //Legend
            true,                   //Display lines connecting points
            0);                     //Marker step: 0 means that all points are displayed
            
            
        //Set the limits of the corresponding panel    
        cd.setLimits(
            1,      //row idx
            1,      //column idx
            0,   //min X
            100,    //max X
            -2,     //min Y
            2);     //Max Y
          
    }
    
 //-------------------------------------------End Sample code   
    
    public static CompositeWaveDisplay createWindow(String title)
    {
       if(title != null)
            f = new JFrame(title);
        else
            f = new JFrame();
 
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {e.getWindow().dispose(); }
        });
        
        CompositeWaveDisplay cwd = new CompositeWaveDisplay(false);
        cwd.init();
        f.getContentPane().add(cwd);
        return cwd;
    }

    public CompositeWaveDisplay()
    {
        super();
    }
    
    private CompositeWaveDisplay(boolean isApplet)
    {
        super();
        this.isApplet = isApplet;
    }
    
    public void addProtocol(DataAccess da)
    {
       DataAccessURL.addProtocol(da); 
    }
    
    private void setDataAccess()
    {
        DataAccessURL.addProtocol(new RdaAccess());
        DataAccessURL.addProtocol(new MdsAccess());
        DataAccessURL.addProtocol(new TwuAccess());
        DataAccessURL.addProtocol(new DemoAccess());            
    }

    public void init()
    {
        
        if(isApplet)
        {
            Waveform.zoom_on_mb1 = false;
            setDataAccess();
        }
        
        setBackground(Color.lightGray);
        wave_container = new WaveformContainer();
        wave_container.addWaveContainerListener(this);
        WavePopup wave_popup = new MultiWavePopup(new SetupWaveformParams(f, "Waveform Params"));
        wave_container.setPopupMenu(wave_popup);
        wave_container.SetMode(Waveform.MODE_ZOOM);
                
        getContentPane().setLayout(new BorderLayout());
              
        JRadioButton point = new JRadioButton("Point", false);
        point.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_POINT);
		       }
		    });

        JRadioButton zoom  = new JRadioButton("Zoom", true);
        zoom.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_ZOOM);
		       }
		    });

        JRadioButton pan  = new JRadioButton("Pan", false);
        pan.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_PAN);
		       }
		    });

        ButtonGroup       pointer_mode = new ButtonGroup();
        pointer_mode.add(point);
        pointer_mode.add(zoom);
        pointer_mode.add(pan);

        JPanel panel1 = new JPanel();
        panel1.setLayout(new FlowLayout(FlowLayout.LEFT, 1, 3));
        panel1.add(point);
        panel1.add(zoom);
        panel1.add(pan);
	
        JPanel panel = new JPanel()
        {
            public void print(Graphics g){}
            public void printAll(Graphics g){}
        };    
            
        panel.setLayout(new BorderLayout());
        panel.add("West", panel1);
        if(!isApplet)
        {            
            point_pos = new JLabel("[0.000000000, 0.000000000]");
            point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
            panel.add("Center", point_pos);

            prnJob = PrinterJob.getPrinterJob();
            pf = prnJob.defaultPage();
   	    
	        JButton print = new JButton("Print");
	        print.addActionListener(new ActionListener()
		    {
		        public void actionPerformed(ActionEvent e)
		        {
		            
		            Thread t = new Thread() {
		                
		                public void run() 
		                {
    		            
				            try 
					        {
					        pf = prnJob.pageDialog(pf);
					        if(prnJob.printDialog())
					            {
                                prnJob.setPrintable(wave_container, pf);
						        prnJob.print();
					            }
					        } 
				            catch (Exception ex){}
                        }
                    };
			        t.start();  			
		        }
		    });
	        panel.add("East", print);

	    }

        getContentPane().add("Center", wave_container);
        getContentPane().add("South", panel);
            
        if(isApplet) getSignalsParameter();
        wave_container.update();
    }
    
    public void setSize(int width, int height)
    {
        super.setSize(width, height);
        validate();
    }
                    

    private boolean translateToBoolean(String value)
    {
        if(value.toUpperCase().equals("TRUE"))
            return  true;
        else
            return false;            
    }
        
    
    private void getSignalsParameter()
    {
        String url = null, gain, offset, row, col, color, marker, name;
        String sig_param;
        String sig_attr = "SIGNAL_";
        int i = 1;
        String global_autentication, signal_autentication, autentication, param;

        global_autentication = getParameter("AUTENTICATION");
        
        param = getParameter("PRINT_SCALING");
        if(param != null)
        {
            try {
                print_scaling = Integer.parseInt(param);
            } catch (NumberFormatException e){}
        }

        param = getParameter("FIXED_LEGEND");
        if(param != null)
        {
            fixed_legend = translateToBoolean(param);
            wave_container.setLegendMode(MultiWaveform.LEGEND_BOTTOM);
        }
        
        param = getParameter("PRINT_WITH_LEGEND");
        if(param != null)
            wave_container.setPrintWithLegend(translateToBoolean(param));

        param = getParameter("PRINT_BW");
        if(param != null)
            wave_container.setPrintBW(translateToBoolean(param));


        try 
        {                
            while((sig_param = getParameter(sig_attr + i)) != null)
            {
                signal_autentication = getParameterValue(sig_param, "autentication");
                if(signal_autentication != null)
                   autentication = signal_autentication;
                else
                   autentication = global_autentication;

                url = getParameterValue(sig_param, "url");
                Signal s = DataAccessURL.getSignal(url, autentication);
                if(s != null)
                {
                    float k = 1.0F, q = 0.0F;
                    gain = getParameterValue(sig_param, "gain");
                    offset = getParameterValue(sig_param, "offset");
                    if(gain != null || offset != null)
                    {
                        if(offset != null)
                            q = Float.valueOf(offset).floatValue();
                        if(gain != null)
                            k = Float.valueOf(gain).floatValue();
                        
                        s.setCalibrate(k, q);
                    }

                    color = getParameterValue(sig_param, "color");
                    if(color != null)
                    {
                        s.setColor(new Color(Integer.decode(color).intValue()));
                        automatic_color = false;
                    } else
                        automatic_color = true;
                    
                    
                    marker = getParameterValue(sig_param, "marker");
                    s.setMarker(marker);
                    
                    name = getParameterValue(sig_param, "name");
                    if(name != null)
                        s.setName(name);
                    else 
                    {
                        int idx = url.indexOf("/") + 2;
                        idx = url.indexOf("/", idx) + 1;
                        s.setName(url.substring(idx, url.length()));
                    }
                    row = getParameterValue(sig_param, "row");
                    col = getParameterValue(sig_param, "col");
                    if(row != null && col != null)
                    {
                        addSignal(s, Integer.parseInt(row), Integer.parseInt(col));
                    }
                
                }
                i++;
            }
        } 
        catch(Exception e)
        {
            if(e instanceof  AccessControlException)
            {
		        JOptionPane.showMessageDialog(this, 
		                                        e.toString()+"\n url "+url +
		                                        "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n", 
		                                        "alert", 
		                                        JOptionPane.ERROR_MESSAGE);
            } else {
		        JOptionPane.showMessageDialog(this, 
		                                        e.toString()+" url "+url, 
		                                        "alert", 
		                                        JOptionPane.ERROR_MESSAGE);
		    }
        }
    }
    
    private String getParameterValue(String context, String param)
    {
        boolean found = false;
        String value = null;
        
        StringTokenizer st = new StringTokenizer(context);
        while (st.hasMoreTokens()) {
            value = st.nextToken();
            if(value.equals(param))
            {
                if(st.nextToken().equals("="))
                {
                    value = st.nextToken();
                    break;
                } else
                    return null;
                
            }
            value = null;    
        }
        return value;         
    }
    
    public void processWaveContainerEvent(WaveContainerEvent e)
    {
        String s = null;
	    int event_id = e.getID();
	    	 	 
	    switch(event_id)
	    {
	        case WaveContainerEvent.WAVEFORM_EVENT:
	    
	            WaveformEvent we = (WaveformEvent)e.we;
	            Waveform w = (Waveform)we.getSource();

	            s = we.toString();
                if(w instanceof MultiWaveform)
                {
                    MultiWaveform mw = (MultiWaveform)w;
                    String n = mw.getSignalName(we.signal_idx);
                    if(n != null)
		                s = s + " " + n;  
	            }
                if(isApplet) 
                    showStatus(s);
                else
                    point_pos.setText(s);
	        break;
	    }        
    }
    

    /**
     * Add new signal, defined by  x and y float vectors, to the
     * panel in (row, column) position. The panel
     * is created if not already  present.
     * 
     * @param x x array values
     * @param y y array values
     * @param row row position, starting from 1
     * @param column column position, starting from 1
     * @param color Signal color
     * @param label Signal name
     * @param inter Interpolation flag, if true a line is draw between adiacent point
     * @param marker Marker point
     */
    public void addSignal(float [] x, float [] y,int row, int column,  
        String color, String label, boolean inter, int marker)
    {
        Signal sig = new Signal(x, y);
        
        if(color != null)
	    {
	        if(color.equals("Automatic"))
	        {
		        automatic_color = true;
	        }
	        else
	        {
		        automatic_color = false;
		        sig.setColorIdx(Waveform.ColorNameToIndex(color));
	        }
	    }
        if(label != null && label.length() != 0)
            sig.setName(label);
        
        sig.setInterpolate(inter);
        sig.setMarker(marker);
        
        addSignal(sig, row, column);
        if(isShowing() && !isValid())
            wave_container.update();
    	
	}
	
	
	public void setLimits(int row, int column, float xmin, float xmax, float ymin, float ymax)
	{
        Component c = null;
        MultiWaveform w = null;
        WaveInterface wi = null;
        
        c = wave_container.getGridComponent(row, column);

        if(c != null)
        {
            w = (MultiWaveform)c;
            wi = w.getWaveInterface();
            if( wi == null)
            {            
                w.lx_max = xmax;
                w.lx_min = xmin;
                w.ly_max = ymax;
                w.ly_min = ymin;    
                w.setLimits();
            }
            else
            {
                wi.in_xmax = ""+xmax;
                wi.in_xmin = ""+xmin;
                wi.in_ymax = ""+ymax;
                wi.in_ymin = ""+ymin;
                try
                {
	                wi.StartEvaluate();
	                wi.setLimits();
	            } catch(Exception e){}
            }
            w.Update();
        }
	}

    public void addSignal(String url,int row, int column,  
        String color, String label, boolean inter, int marker)
    {
        
        Component c = null;
        MultiWaveform w = null;
        WaveInterface wi = null;
        DataAccess da = null;
        Signal s;

        if(DataAccessURL.getNumProtocols() == 0)
            setDataAccess();
        
        c = wave_container.getGridComponent(row, column);
        
        try
        {
            da = DataAccessURL.getDataAccess(url);
            da.setProvider(url);
            
            if(c != null)
            {
                w = (MultiWaveform)c;
                                
                wi  = w.getWaveInterface();
                if(wi == null) 
                {
                    if( w.GetSignals() != null && w.GetSignals().size() != 0)
                    {                       
		                JOptionPane.showMessageDialog(this, 
		                                            "In the selected waveform panel there are raw signals.\n" + 
		                                            "\nDefine a new waveform panel to show signals from "+
		                                            da.getDataProvider().getClass().getName() + 
		                                            " data provider.",
		                                            "alert", 
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }
                                        
                    w.setWaveInterface((wi = new WaveInterface()));
                    wi.SetDataProvider(da.getDataProvider());
                }
                else
                {
                    if(!wi.getDataProvider().getClass().getName().equals(da.getDataProvider().getClass().getName()))
                    {
		                JOptionPane.showMessageDialog(this, 
		                                            "The selected waveform panel is already connected to " + 
		                                            wi.getDataProvider().getClass().getName() +
		                                            " data provider.\nDefine a new waveform panel to show signals from "+
		                                            da.getDataProvider().getClass().getName() + 
		                                            " data provider.",
		                                            "alert", 
		                                            JOptionPane.ERROR_MESSAGE);
                        return;
                    }
                }
            } else {
                w = (MultiWaveform)wave_container.CreateWaveComponent();
                w.setWaveInterface((wi = new WaveInterface()));            
                wave_container.add(w, row, column);                
                wi.SetDataProvider(da.getDataProvider());
            }
                      
            
            if(da != null && wi != null)
            {
                                       
                wi.experiment = da.getExperiment();
                wi.AddSignal(da.getSignal());
                wi.setShotArray(da.getShot());
                                            
	            wi.StartEvaluate();
		        wi.EvaluateOthers();
    		    
		        //If added signal has been evaluated without
		        //error it is stored in wi.signals vector
                       
                if(wi.signals != null && (s = wi.signals[wi.signals.length-1]) != null)
                {
                    s.setColorIdx(Waveform.ColorNameToIndex(color));                                            
                    s.setMarker(marker);
                            
                    if(label != null && label.length() != 0)
                        s.setName(label);
                    else
                        s.setName(wi.in_y[0]);
                }
                
		        w.Update(wi.signals);
		        
            }
        }
        catch(Exception e)
        {
            if(e instanceof  AccessControlException)
            {
		        JOptionPane.showMessageDialog(this, 
		                                        e.toString()+"\n url "+url +
		                                        "\nUse policytool.exe in  JDK or JRE installation directory to add socket access permission\n", 
		                                        "alert", 
		                                        JOptionPane.ERROR_MESSAGE);
            } else {
		        JOptionPane.showMessageDialog(this, 
		                                        e.toString(), 
		                                        "alert", 
		                                        JOptionPane.ERROR_MESSAGE);
            }
        }            
    }
	
	
	/**
	 * Set Window dialog title.
	 * 
	 * @param title Title string
	 */
	public void setTitle(String title)
	{
	    if(f != null)
	        f.setTitle(title);	    
	}

	/**
	 * Remove all signals added to the panels.
	 */
	public void removeAllSignals()
	{
	    if(wave_container != null)
	        wave_container.RemoveAllSignals();	    
	}


    /**
     * Add new signal, defined as x and y float vectors, to 
     * panel in (row, column) position. The panel
     * is created if not already  present.
     * 
     * @param x x array values
     * @param y y array values
     * @param row Row  position
     * @param column Column  position
     * @param color Signal color
     * @param label Signal name
     */
    public void addSignal(float [] x, float [] y,int row, int column,  
        String color, String label)
    {
    	addSignal(x, y, row, column, color, label, true, 0);

    }
        
    /**
     * Show window with defined position and size.
     * 
     * @param x position
     * @param y position
     * @param width dimension
     * @param height dimension
     */
    public void showWindow(int x, int y, int width, int height)
    {
        wave_container.update();
        if(!this.isShowing())
        {
            ((JFrame)f).pack();
            ((JFrame)f).setBounds(new Rectangle(x, y, width, height));
            ((JFrame)f).show();
        }
    }


    /**
     * Add new signal, defined as Signal class, to 
     * panel in (row, column) position. The panel
     * is created if not already  present.
     * 
     * @param s Signal to add
     * @param row Row MultiWaveform position
     * @param column Column MultiWaveform position
     * @param col
     */
    public void addSignal(Signal s, int row, int col)
    {
        Component c = null;
        MultiWaveform w = null;
        
        c = wave_container.getGridComponent(row, col);
        
        if(c != null)
        {
            w = (MultiWaveform)c;
            
            if(w.getWaveInterface() != null)
            {
		        JOptionPane.showMessageDialog(this, 
		                                    "The selected waveform panel is  connected to " + 
		                                    w.getWaveInterface().getDataProvider().getClass().getName() +
		                                    " data provider.\nDefine a new waveform panel to show raw signals",
		                                    "alert", 
		                                    JOptionPane.ERROR_MESSAGE);
                return;
            }
            
            if(automatic_color)
                s.setColorIdx(w.GetShowSignalCount());
            w.addSignal(s);
        } else {
            w = (MultiWaveform)wave_container.CreateWaveComponent();
            w.addSignal(s);
            w.setLegendMode(MultiWaveform.LEGEND_BOTTOM);
            wave_container.add(w, row, col);
        }
        if(isApplet) showStatus("Add "+s.getName()+" col "+col+" row "+row);
        w.Update();
    }    
}

