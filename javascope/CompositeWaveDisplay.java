import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import java.lang.NumberFormatException;
import java.util.*;
import javax.swing.*;
import java.security.AccessControlException;
//import java.awt.print.*;

public class CompositeWaveDisplay extends JApplet implements WaveContainerListener
{
    private WaveformContainer_2 wave_container;
    private boolean           automatic_color = false;
    private boolean           isApplet = true;
    private JLabel            point_pos;
    private int               print_scaling = 100;
    private boolean           fixed_legend = false;
    static  private JFrame    f = null;
//    PrinterJob                prnJob;
//    PageFormat                pf;

    public static void main(String args[])
    {
        Component cd = createWindow("prova");
        ((CompositeWaveDisplay)cd).testSignal((CompositeWaveDisplay)cd);
        ((CompositeWaveDisplay)cd).showWindow(50, 50, 500, 400);
    }
    
    
    private void testSignal(CompositeWaveDisplay cd)
    {
        float y[] = new float[1000], y1[] = new float[1000], y2[] = new float[1000], x [] = new float[1000];
        for(int i = 0; i < 1000; i++)
        {
            x[i] = (float)(i/1000.);
            y[i] = (float)Math.sin(i/300.);
            y1[i] = (float)Math.cos(i/300.);
            y2[i] = (float)Math.sin(i/300.)*(float)Math.sin(i/300.);
        }
        
        float data[] = new float[100*100], time[] = new float[100], x_data[] = new float[100]; 
        for(int i = 0; i < 100; i++)
        {
            time[i] = (float)(i/100.);
            //data[i] = (float)Math.cos(i/300.);
            for(int j = 0; j < 100; j++)
            {
                data[j * 100+i] = (float)((i + 1)/100.) * (float)Math.cos(j * 6.28/100.) * (float)Math.cos(i * 6.28/100.);
            }
        }
        for(int i = 0; i < 100; i++)
            x_data[i] = i;// (float)Math.sin(i * 6.28/100.);
            
        Signal  sig_2d = new Signal(data, x_data, time, Signal.MODE_YTIME); 
        ((CompositeWaveDisplay)cd).addSignal(sig_2d, 1, 2);
        ((CompositeWaveDisplay)cd).addSignal(x, y, 1,2,"green", "seno");
       
        Signal  sig_2d1 = new Signal(sig_2d);
        ((CompositeWaveDisplay)cd).addSignal(sig_2d1, 1, 1);
        ((CompositeWaveDisplay)cd).addSignal(x, y, 1,1,"green", "seno");
        ((CompositeWaveDisplay)cd).addSignal(x, y1, 1,1,"red", "coseno");
        ((CompositeWaveDisplay)cd).addSignal(x, y2, 2,1,"blue", "seno**2");
    }
    
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

    public void init()
    {
        
        if(this.isApplet)
        {
            Waveform.zoom_on_mb1 = false;
            DataAccessURL.addProtocol(new RdaAccess());
            DataAccessURL.addProtocol(new MdsAccess());
            DataAccessURL.addProtocol(new TwuAccess());
        }
        setBackground(Color.lightGray);
        wave_container = new WaveformContainer_2();
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

/*
            prnJob = PrinterJob.getPrinterJob();
            pf = prnJob.defaultPage();
	        pf.setOrientation(PageFormat.LANDSCAPE);
	        Paper p = pf.getPaper();
	        p.setSize(595.2239, 841.824);
	        p.setImageableArea(13.536, 12.959, 567.288, 816.6239);
	        pf.setPaper(p);
    	
	        JButton print = new JButton("Print");
	        print.addActionListener(new ActionListener()
		    {
		        public void actionPerformed(ActionEvent e)
		        { 			
			    javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
				    public void actionPerformed(ActionEvent ae) 
				    {
				        try 
					    {
					    pf = prnJob.pageDialog(pf);
					    if(prnJob.printDialog())
					        {
						    System.out.println("Stampo");
						    prnJob.setPrintable(wave_container, pf);
						    prnJob.print();
					        }
					    } 
				        catch (Exception ex){}
				    }
			    });
			    t.setRepeats(false);
			    t.start();
    			
		        }
		    });
	        panel.add("East", print);
*/
	    }

        getContentPane().add("Center", wave_container);//, BorderLayout.CENTER);
        getContentPane().add("South", panel);//, BorderLayout.SOUTH);
            
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
               // System.out.println("inizia " + sig_param.toLowerCase());
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
	            int we_id = we.getID();

	            s = we.toString();
                if(w instanceof MultiWaveform)
                {
                    MultiWaveform mw = (MultiWaveform)w;
                    String n = mw.getSignalName(we.signal_idx);
                    if(n != null)
		                s = s + n;  
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

    /*
    public void print()
    {
	 PrintJob pg = getToolkit().getPrintJob(f, "Print Scope", System.getProperties());
	 if(pg != null)
	    {
	        Graphics g = pg.getGraphics();
	        Dimension dim = pg.getPageDimension();
	        int st_x = 0, st_y = 0;
	        
	        //Fix page dimension and margin error on Window System
	        if(!System.getProperty("os.name").equals("Mac OS"))
	        {
		    st_x = 10;
		    dim.width -= 2*st_x;
		    st_x = 15;
                
		    st_y = 10;
		    dim.height -= 2*st_y;
	        }
		System.out.println(" "+dim);
//	        wave_container.PrintAll(g, st_x, st_y, dim.height, dim.width); 
	        g.dispose();
	        pg.end();
	    }	    
     }
  */
  
    public void print(Graphics g)
    {
        
        Dimension dim = getSize();
        /*
        System.out.println(g);
        PrintGraphics pg = (PrintGraphics)g;
        PrintJob pj = pg.getPrintJob();
        System.out.println(pj); 
        System.out.println("Size "+pj.getPageDimension()+ " resolution " + pj.getPageResolution());
        */
        /*
        Dimension dim = new Dimension();       
        Dimension applet_size = getSize();
        if(print_scaling != 100)
        {
	        System.out.println("Proporzionale " + this.getBounds());
	        float ratio = (float)applet_size.width/applet_size.height;
	        dim.width = (int)(applet_size.width/100.* print_scaling);
	        dim.height = (int)(ratio * dim.width);
        } else {
	        dim.width = 530;
	        dim.height = 816;
        }
        */
	    wave_container.PrintAll(g, 0, 0, dim.height-1, dim.width-1); 
    }
    
}

