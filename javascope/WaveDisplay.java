import java.awt.*;
import java.net.URL;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import java.security.AccessControlException;

public class WaveDisplay extends JApplet implements WaveformListener
{
    MultiWaveform w;
    MultiWavePopup wave_popup;
    private Frame frame;
    JTextField shot_txt;

    public WaveDisplay()
    {
        DataAccessURL.addProtocol(new RdaAccess());
        DataAccessURL.addProtocol(new MdsAccess());
        DataAccessURL.addProtocol(new TwuAccess());
        
        JPanel panel = new JPanel();
        w = new MultiWaveform();
        w.setWaveInterface(new WaveInterface());
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
        wave_popup = new MultiWavePopup();
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
        /*
        panel1.add(new JLabel("Shot"));
        shot_txt = new JTextField(20);
        shot_txt.addActionListener(
            new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    try 
                    {
                        WaveInterface wi = w.getWaveInterface();                   
                        wi.setShotArray(shot_txt.getText());
	                    wi.StartEvaluate();
		                wi.EvaluateOthers();
		                w.Update(wi.signals);
		            } catch (Exception exc){}
                }
            }
        );
        
        panel1.add(shot_txt);
    */        
        
        
        w.SetGridMode(Grid.IS_DOTTED, true, true);
        panel.setLayout(new BorderLayout());
        panel.add(w, BorderLayout.CENTER);
        getContentPane().setLayout(new BorderLayout());
        getContentPane().add(panel, BorderLayout.CENTER);
        getContentPane().add(panel1, BorderLayout.SOUTH);
    }

    public void init()
    {
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
	    MultiWaveform w = (MultiWaveform)we.getSource();
	    WaveInterface  wi = w.getWaveInterface();
	    int we_id = we.getID();
	        
	    switch(we_id)
	    {
    	    case WaveformEvent.MEASURE_UPDATE:
	        case WaveformEvent.POINT_UPDATE:
	        case WaveformEvent.POINT_IMAGE_UPDATE:
       	        s = we.toString();
	            if(wi.shots != null)
	            {
		            s = (s +
		                " Expr : " + w.getSignalName(we.signal_idx) +  
		                " Shot = " + wi.shots[we.signal_idx]);
	            } else {
		            s  = (s +
		                " Expr : " + w.getSignalName(we.signal_idx));  
	            }
	        break;
	        case WaveformEvent.STATUS_INFO:
	            s = we.status_info;
	        break;
	    }

//	    s = we.toString()+" "+w.getSignalName();
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
        String global_autentication, signal_autentication, 
               autentication;
        boolean async_update = false;

        global_autentication = getParameter("AUTENTICATION");
        
        param = getParameter("ASYNC_UPDATE");
        if(param != null)
            async_update = this.translateToBoolean(param); 
                
        try 
        {
            if((sig_param = getParameter("SIGNAL")) != null)
            {
                
                signal_autentication = getParameterValue(sig_param, "autentication");
                if(signal_autentication != null)
                   autentication = signal_autentication;
                else
                   autentication = global_autentication;
                
                url = getParameterValue(sig_param, "url");
                
                //Signal s = DataAccessURL.getSignal(url, autentication);
                DataAccess da = DataAccessURL.getDataAccess(url);
                //if(s != null)
                if(da != null)
                {
                    da.setPassword(autentication);
                    da.setProvider(url);
                    
                    WaveInterface wi = w.getWaveInterface();
                    wi.SetDataProvider(da.getDataProvider());
                    wi.experiment = da.getExperiment();
                    wi.AddSignal(da.getSignal());
                    
                    color = getParameterValue(sig_param, "color");
                    
                    
                    wi.full_flag = !async_update;
                    
                    
                    wi.setShotArray(da.getShot());

	                wi.StartEvaluate();
		            wi.EvaluateOthers();
                   
                    Signal s;
                    if(wi.signals != null && (s = wi.signals[0]) != null)
                    {
                        color = getParameterValue(sig_param, "color");
                        if(color != null)
                            s.setColor(new Color(Integer.decode(color).intValue()));                    
                        
                        marker = getParameterValue(sig_param, "marker");
                        s.setMarker(marker);
                        
                        name = getParameterValue(sig_param, "name");
                        if(name != null)
                            s.setName(name);
                        else
                            s.setName(wi.in_y[0]);
                            
                        title = getParameterValue(sig_param, "title");
                        if(title != null)
                            w.SetTitle(title);
                    }                   
		            w.Update(wi.signals);		            
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
		                                        e.toString(), 
		                                        "alert", 
		                                        JOptionPane.ERROR_MESSAGE);
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

