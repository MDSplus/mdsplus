import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;
import java.lang.NumberFormatException;
import java.util.*;

public class CompositeWaveDisplay extends Applet implements WaveContainerListener
{
    private CheckboxGroup pointer_mode;
    private WaveformContainer wave_container;
    private boolean automatic_color = false;
    private boolean isApplet = true;
    private Label point_pos;

    public static void main(String args[])
    {
        Component cd = createWindow("prova");
        float y[] = new float[1000], y1[] = new float[1000], y2[] = new float[1000], x [] = new float[1000];
        for(int i = 0; i < 1000; i++)
        {
            x[i] = i;
            y[i] = (float)Math.sin(i/300.);
            y1[i] = (float)Math.cos(i/300.);
            y2[i] = (float)Math.sin(i/300.)*(float)Math.sin(i/300.);
        }
        
        float data[] = new float[10*1000], time[] = new float[1000], x_data[] = new float[10]; 
        for(int i = 0; i < 1000; i++)
        {
            time[i] = i;
            data[i] = (float)Math.cos(i/300.);
            for(int j = 0; j < 10; j++)
            {
                data[j * 1000+i] = (j+1) * (float)Math.cos(i/300.);
            }
        }
        for(int i = 0; i < 10; i++)
            x_data[i] = (float)Math.cos(i/2.);
            
        Signal  sig_2d = new Signal(data, x_data, time, Signal.MODE_YTIME , 0); 
        ((CompositeWaveDisplay)cd).addSignal(sig_2d, 1, 2);
        
        ((CompositeWaveDisplay)cd).addSignal(x, y, 1,1,"green", "seno");
        ((CompositeWaveDisplay)cd).addSignal(x, y1, 1,1,"red", "coseno");
        ((CompositeWaveDisplay)cd).addSignal(x, y2, 2,1,"blue", "seno**2");
        ((CompositeWaveDisplay)cd).showWindow(50, 50, 500, 400);
    }
    
    public static CompositeWaveDisplay createWindow(String title)
    {
        Frame f;
        if(title != null)
            f = new Frame(title);
        else
            f = new Frame();
 
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {e.getWindow().dispose(); }
        });
        
        CompositeWaveDisplay cwd = new CompositeWaveDisplay(false);
        cwd.init();
        f.add(cwd);
        return cwd;
    }

    public CompositeWaveDisplay()
    {
        super();
    }
    
    public CompositeWaveDisplay(boolean isApplet)
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
        }
        wave_container = new WaveformContainer();
        wave_container.addWaveContainerListener(this);
        WavePopup wave_popup = new MultiWavePopup();
        wave_container.setPopupMenu(wave_popup);
        wave_container.SetMode(Waveform.MODE_ZOOM);
                
        setLayout(new BorderLayout());
              
    
        pointer_mode = new CheckboxGroup();    
        Checkbox point = new Checkbox("Point", pointer_mode, false);
        point.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_POINT);
		       }
		    });
        Checkbox zoom  = new Checkbox("Zoom", pointer_mode, true);
        zoom.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_ZOOM);
		       }
		    });
        Checkbox pan  = new Checkbox("Pan", pointer_mode, false);
        pan.addItemListener(new ItemListener ()
		    {
		       public void itemStateChanged(ItemEvent e)
		       {
		           wave_container.SetMode(Waveform.MODE_PAN);
		       }
		    });
	
		    /*
            Button print = new Button("Print");
            print.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent e)
                    {
                        print();
                    }
                });
            panel.add("East", print);    
		    */
            Panel panel1 = new Panel();
            panel1.setLayout(new FlowLayout(FlowLayout.LEFT, 1, 3));
            panel1.add(point);
            panel1.add(zoom);
            panel1.add(pan);

            Panel panel = new Panel();
            panel.setLayout(new BorderLayout());
            panel.add("West", panel1);
            if(!isApplet)
            {            
                point_pos = new Label("[0.000000000, 0.000000000]");
                point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
                panel.add("Center", point_pos);
            }
            
            add(wave_container, BorderLayout.CENTER);
            add(panel, BorderLayout.SOUTH);
            
            if(isApplet) getSignalsParameter();
            wave_container.update();
    }
    
    public void setSize(int width, int height)
    {
        super.setSize(width, height);
        validate();
    }
                    
    private String SetStrSize(String s, int size)
    {
	    StringBuffer sb = new StringBuffer(size);
	
	    sb.append(s.substring(0, ((s.length() < size) ? s.length() : size)));

    	if(sb.length() < size)
	    {
	        for(int i = sb.length(); i < size; i++)
		    sb.append(" ");
	    }	
	    return (new String(sb));
    }
    
        
    
    private void getSignalsParameter()
    {
        String url = null, gain, offset, row, col, color, marker, name;
        String sig_param;
        String sig_attr = "SIGNAL_";
        int i = 1;
        String global_autentication, signal_autentication, autentication;

        global_autentication = getParameter("AUTENTICATION");

        while((sig_param = getParameter(sig_attr + i)) != null)
        {
            try {                
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
                        s.setName(url);
                    
                    row = getParameterValue(sig_param, "row");
                    col = getParameterValue(sig_param, "col");
                    if(row != null && col != null)
                    {
                        addSignal(s, Integer.parseInt(row), Integer.parseInt(col));
                    }
                
                }
            } catch(Exception e){
                showStatus(e.toString()+" url "+url);                    
            }
            i++;
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
	        
	            switch(we_id)
	            {
    	            case WaveformEvent.MEASURE_UPDATE:
	                    double dx_f;
	         
	                    if(Math.abs(we.delta_x) < 1.e-20)
	                        dx_f = 1.e-20;
	                    else
	                        dx_f = Math.abs(we.delta_x);
	         
                        s = SetStrSize("[" + Waveform.ConvertToString(we.point_x, false) + ", " 
				                + Waveform.ConvertToString(we.point_y, false) + "; dx "
				                + Waveform.ConvertToString(we.delta_x, false) + "; dy "
				                + Waveform.ConvertToString(we.delta_y, false) + "; 1/dx "
				                + Waveform.ConvertToString(1./dx_f, false) +
				                "]", 80);

	                case WaveformEvent.POINT_UPDATE:
                        if(s == null)
                        {
	                        if(!w.IsImage())
	                            s = SetStrSize("[" + Waveform.ConvertToString(we.point_x, false) + ", " 
				                   + Waveform.ConvertToString(we.point_y, false) + "]", 30);
		                    else
	                            s = SetStrSize("[" + ((int)we.point_x) + ", " 
				                        + ((int)we.point_y) + " : " 
				                       + we.delta_x + "]", 30);
		                }

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
	        break;
	    }        
    }
    

    public void addSignal(float [] x, float [] y,int row, int column,  
        String color, String label)
    {
        Signal sig = new Signal(x, y);
        
        if(color != null)
            sig.setColorIdx(Waveform.ColorNameToIndex(color));
        if(label != null)
            sig.setName(label);
            
        addSignal(sig, row, column);
        if(isShowing() && !isValid())
            wave_container.update();

    }
        
    public void showWindow(int x, int y, int width, int height)
    {
        wave_container.update();
        Component f = getParent();
        if(f instanceof Frame);
        ((Frame)f).pack();
        ((Frame)f).setBounds(new Rectangle(x, y, width, height));
        ((Frame)f).show();
    }


    public void addSignal(Signal s, int row, int col)
    {
        Component c = null;
        MultiWaveform w = null;
        
        c = wave_container.getGridComponent(row, col);
        
        if(c != null)
        {
            w = (MultiWaveform)c;
            if(automatic_color)
                s.setColorIdx(w.GetSignalCount());
            w.addSignal(s);
        } else {
            w = (MultiWaveform)wave_container.CreateWaveComponent();
            w.addSignal(s);
            wave_container.add(w, row, col);
        }
        if(isApplet) showStatus("Add "+s.getName()+" col "+col+" row "+row);
        w.Update();
    }
    
    
    public void print(Graphics g)
    {

        //PrintJob p = getToolkit().getPrintJob(null, "Print", null);
        //if(p != null)
        //if(g instanceof PrintGraphics)
        {
	        System.out.println("------- IN PRINT --------"+g);
	      //  Graphics g = p.getGraphics();
	      //  Dimension dim = p.getPageDimension();
	        Dimension dim = new Dimension(500, 700);
	        System.out.println("------- OK --------");
	        //PrintGraphics pg = (PrintGraphics)g; 
	        //PrintJob p = pg.getPrintJob();
	        //Dimension dim = p.getPageDimension();
	        
	        wave_container.PrintAll(g, 0, 0, dim.width, dim.height); 
	        //g.dispose();
	        //p.end();
	    }
    }
}

