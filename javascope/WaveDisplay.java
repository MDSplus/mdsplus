import java.awt.*;
import java.net.URL;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import java.security.AccessControlException;

public class WaveDisplay extends JApplet implements WaveformListener
{
    Waveform w;
    WavePopup wave_popup;
    private Frame frame;

    public WaveDisplay()
    {
        System.out.println("Costruttore");
        DataAccessURL.addProtocol(new RdaAccess());
        DataAccessURL.addProtocol(new MdsAccess());
        DataAccessURL.addProtocol(new TwuAccess());
        
        JPanel panel = new JPanel();
        w = new Waveform();
        w.addWaveformListener(this);
        
        /*
        float x[] = new float[1000];
        float y[] = new float[1000];
        for(int j = 0 ; j < 1000; j++)
        {
            x[j] = (float)(6.28/1000*j);
            y[j] = (float)(Math.sin(x[j]));
            continue;               
        }
        Signal s = new Signal(x, y);
        w.Update(s);
        */
        
        
        setBackground(Color.lightGray);
        wave_popup = new WavePopup();
        getContentPane().add(wave_popup);
        panel.addMouseListener( new MouseAdapter()
	        {
	           public void mousePressed(MouseEvent e)
	           {
	                Waveform w = (Waveform)e.getSource();
    	            if(wave_popup != null && w.GetMode() != Waveform.MODE_COPY)
		                wave_popup.Show(w, e.getX(), e.getY());
		       }
	        });


        JRadioButton point = new JRadioButton("Point", false);
        point.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           w.SetMode(Waveform.MODE_POINT);
		       }
		    });

        JRadioButton zoom  = new JRadioButton("Zoom", true);
        zoom.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           w.SetMode(Waveform.MODE_ZOOM);
		       }
		    });

        JRadioButton pan  = new JRadioButton("Pan", false);
        pan.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           w.SetMode(Waveform.MODE_PAN);
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
        
        w.SetGridMode(Grid.IS_DOTTED, true, true);
        panel.setLayout(new BorderLayout());
        panel.add(w, BorderLayout.CENTER);
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(panel, BorderLayout.CENTER);
        getContentPane().add(panel1, BorderLayout.SOUTH);
    }

    public void init()
    {
        System.out.println("Init");
    }

    public void start() {
        getSignalsParameter();
    }

    public void stop() 
    {         
        DataAccessURL.close();
    }

    public void destroy() {
    }
             
    public void processWaveformEvent(WaveformEvent e)
    {
        String s = null;
	    int event_id = e.getID();
	    	 	 
	    WaveformEvent we = (WaveformEvent)e;
	    Waveform w = (Waveform)we.getSource();
	    int we_id = we.getID();

	    s = we.toString()+" "+w.getSignalName();
        showStatus(s);
    }

    protected boolean translateToBoolean(String value)
    {
        if(value.toUpperCase().equals("TRUE"))
            return  true;
        else
            return false;            
    }
        
    
    private void getSignalsParameter()
    {
        String url = null, color, marker, name, title;
        String sig_param;
        String  param;

        try 
        {
            if((sig_param = getParameter("SIGNAL")) != null)
            {
                url = getParameterValue(sig_param, "url");
                Signal s = DataAccessURL.getSignal(url);
                if(s != null)
                {
                    color = getParameterValue(sig_param, "color");
                    if(color != null)
                        s.setColor(new Color(Integer.decode(color).intValue()));                    
                    
                    marker = getParameterValue(sig_param, "marker");
                    s.setMarker(marker);
                    
                    name = getParameterValue(sig_param, "name");
                    if(name != null)
                        s.setName(name);
                        
                    title = getParameterValue(sig_param, "title");
                    if(title != null)
                        w.SetTitle(title);
                        
                    w.Update(s);
                } 
            } 
            else
            {
                String aspect_ratio, horizontal_flip, vertical_flip;
                if((sig_param = getParameter("IMAGE")) != null)
                {
                    url = getParameterValue(sig_param, "url");
                    Frames f = new Frames();
                    
                    horizontal_flip = getParameterValue(sig_param, "h_flip");
                    if(horizontal_flip != null && horizontal_flip.toLowerCase().equals("true"))
                        f.setHorizontalFlip(true);

                    vertical_flip = getParameterValue(sig_param, "v_flip");
                    if(vertical_flip != null && vertical_flip.toLowerCase().equals("true"))
                        f.setVerticalFlip(true);
                        
                    f = DataAccessURL.getImages(url, f);
                    if(f != null)
                    {
                        name = getParameterValue(sig_param, "name");
                        if(name != null)
                            f.setName(name);
                            
                        aspect_ratio = getParameterValue(sig_param, "ratio");
                        if(aspect_ratio != null && aspect_ratio.toLowerCase().equals("false"))
                            f.setAspectRatio(false);
                            
                            
                        w.UpdateImage(f);
                    }
                }
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

                showStatus(e.toString()+" url "+url);
            }
        }
    }
    
    protected String getParameterValue(String context, String param)
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
    public void print_xxx(Graphics g)
    {
        Dimension dim = new Dimension();
        /*
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
	        dim.width = 530;
	        dim.height = 816;
	        w.paint(g, dim, Waveform.PRINT);
    }
}

