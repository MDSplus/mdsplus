import java.awt.*;
import java.util.*;
import java.io.*;

/**
Class MultiWaveform extends the capability of class Waveform to deal with multiple
waveforms.
*/
public class jScopeMultiWave extends MultiWaveform implements NetworkEventListener
{
    MdsWaveInterface wi;
   
    
    public jScopeMultiWave(DataProvider dp, jScopeDefaultValues def_values)
    {
	    super();
	    wi = new MdsWaveInterface(dp, def_values);
	    wi.wave = this;
    }
    
    public void processNetworkEvent(NetworkEvent e)
    {
         System.out.println("Evento su waveform "+e.name);
         WaveformEvent we = new WaveformEvent(this, WaveformEvent.EVENT_UPDATE,  "Update on event " + e.name);
         dispatchWaveformEvent(we);
//         Refresh();
    }

    public String Refresh()
    {
        String error;
        
        int curr_mode = GetMode();
	    SetMode(Waveform.MODE_WAIT);

        try
        {
            error = wi.Update();
            if(error == null)
                jScopeWaveUpdate();
        } 
        catch (IOException e) 
        {
            wi.error = e.getMessage();
        }
        
    	SetMode(curr_mode);

        return wi.error;
    }
    
    
    public void jScopeErase()
    {
        Erase();
        wi.Erase();
        System.gc();
    }
    
    public String Refresh(boolean add_sig, boolean brief_error)
    {
        
        String error;
        
        AddEvent();
//        wi.SetMainShot(main_s);
        error = Refresh();
        if(error != null)
        {
            int idx = (error.indexOf('\n') == -1 ? error.length() : error.indexOf('\n'));
            if(brief_error)
                error = error.substring(0, idx)+"\n";
        } else
            error = wi.GetErrorString(add_sig, brief_error);
        
        return error;
    }


    //public String Update()
    public void jScopeWaveUpdate() throws IOException
    {	
	    if(wi == null)
	        return;// "Undefine signal to evaluate";
	    
	    if(wi.modified)
	    {
	        wi.StartEvaluate();
//	        if(wi.error == null)
		        wi.EvaluateOthers();
	    }

	    Update(wi);

	    wi.modified = (wi.error != null);
	
	    //return  wi.error;		
    }


    public void Update(WaveInterface _wi)
    {
	    wi = (MdsWaveInterface)_wi;	     	    
	    	    
	    orig_signals = null;
//	    if(wi.colors != null && wi.colors.length > 0)
//	    {
//	        colors = wi.colors;
	        //num_colors =  wi.colors.length;
//	        crosshair_color = colors[curr_point_sig_idx % wi.colors.length];
//	        first_set_point = true;
//	    }
        super.x_label = wi.xlabel;
	    super.y_label = wi.ylabel;
	    super.x_log = wi.x_log;
	    super.y_log = wi.y_log;
	    
	    String error = wi.GetErrorTitle();
	    if(error == null)
	        super.title = wi.title;
	    else
	        super.title = error;
	    
        super.make_legend = wi.make_legend;
	    super.legend_x = wi.legend_x;
        super.legend_y = wi.legend_y;
        super.is_image = wi.is_image;
        super.frames = wi.frames;

        System.gc();

	    if(wi.signals != null)
	    {
	        boolean all_null = true;
	        for(int i = 0; i < wi.signals.length; i++)
		        if(wi.signals[i] != null)
		        {
		            all_null = false;
		            if(wi.in_label[i] != null && wi.in_label[i].length() != 0)
		                wi.signals[i].setName(wi.in_label[i]);
		            else
		                wi.signals[i].setName(wi.in_y[i]);		            
		            wi.signals[i].setMarker(wi.markers[i]);
		            wi.signals[i].setMarkerStep(wi.markers_step[i]);
		            wi.signals[i].setInterpolate(wi.interpolates[i]);
		            wi.signals[i].setColorIdx(wi.colors_idx[i]);
		            //Update(wi.signals, wi.num_waves, wi.markers, wi.markers_step, wi.interpolates);
                    //Update(wi.signals);//, wi.in_label,  wi.markers, wi.markers_step, 
                                       //wi.interpolates, wi.colors_idx);
		        }
            if(!all_null)
            {
                Update(wi.signals);	    
                return;
            }
        }
	    
	    if(wi.is_image && wi.frames != null)
	    {
            super.frames.setAspectRatio(wi.keep_ratio);
            super.curr_point_sig_idx = 0;
            signals.removeAllElements();
	        not_drawn = true;
	        frame = 0;
	        return;
	    }
	        	        
	    Erase();

    }

    public String[] GetSignalsName()
    {
        return wi.GetSignalsName();
    }

    public boolean[] GetSignalsState()
    {
        return wi.GetSignalsState();
    }

    public void SetSignalState(String label, boolean state)
    {
        Signal sig;
        wi.setSignalState(label, state);
        super.SetSignalState(label, state);
        /*
        if(signals != null)
        {
            for(int i = 0; i < signals.size(); i++)
            {
                sig = (Signal)signals.elementAt(i);
                if(sig == null) continue;
                if(sig.getName().equals(label))
                {
                    sig.setInterpolate(state);
                    sig.setMarker(Signal.NONE);
                    if(mode == Waveform.MODE_POINT && curr_point_sig_idx == i)
                    {
                        Dimension d = getSize();
                        double  curr_x = wm.XValue(end_x, d),
	                            curr_y = wm.YValue(end_y, d); 
                        FindPointY(curr_x, curr_y, true);
                        return;
                    }
                }
            }
        }
        */
    }

    
    protected void DrawLegend(Graphics g, Point p)
    {
        int h = g.getFont().getSize() + 2, k = 0;
        Color prev_col = g.getColor();
        Point pts[] = new Point[1];
        String s, er;
        pts[0] = new Point();
        
        wi.legend_x = p.x;
        wi.legend_y = p.y;
        
        for(int i = 0, py = p.y ; i < wi.num_waves; i++)
        {
            if(!wi.interpolates[i] && wi.markers[i] == Signal.NONE)
                continue;
            py += h;    
            //g.setColor(wi.colors[i]);
            g.setColor(colors[wi.colors_idx[i] % colors.length]);
            pts[0].x = p.x - marker_width/2;
            pts[0].y = py - marker_width/2;
            DrawMarkers(g, pts, 1, wi.markers[i], 1);
            String name = (wi.in_label[i] != null && wi.in_label[i].length() > 0) ? wi.in_label[i] : wi.in_y[i]; 
            er = wi.w_error[i] != null ? " ERROR " : "";
            if(wi.shots != null) {
                s = name+" "+wi.shots[i] + er;
            } else {
                s = name + er;
            }
            Signal sign = (Signal)signals.elementAt(i);
            if(sign != null && sign.getType() == Signal.TYPE_2D)
            {
                switch(sign.getMode())
                {
                    case Signal.MODE_YTIME:s = s + " [Y-TIME X = "+ Waveform.ConvertToString(sign.getXData(), false)+" ]";break;
                    case Signal.MODE_XY:s = s + " [X-Y T = "+ Waveform.ConvertToString(sign.getTime(), false)+" ]";break;
                    case Signal.MODE_YX:s = s + " [Y-X T = "+ Waveform.ConvertToString(sign.getTime(), false)+" ]";break;
                }
            }

            g.drawString(s, p.x + marker_width, py);
        }
       
        g.setColor(prev_col);
    }


    protected void NotifyZoom(double start_xs, double end_xs, double start_ys, double end_ys,
	    int timestamp) 
    {
        double x_range = end_xs - start_xs;
        if(orig_signals == null)
        {
	        orig_signals = new Vector();
	        for(int i = 0; i < signals.size(); i++)
	            orig_signals.addElement(signals.elementAt(i));
	        orig_xmin = waveform_signal.xmin;
	        orig_xmax = waveform_signal.xmax;
        }
        
        if(wi != null)
	        wi.AsynchUpdate(signals, (float)(start_xs - x_range), (float)(end_xs + x_range), 
	            (float)orig_xmin, (float)orig_xmax, update_timestamp, mode == MODE_PAN, this);
    }	

    public WaveInterface GetWaveInterface()
    {
        if(wi == null)
	        return null;
        return new MdsWaveInterface((MdsWaveInterface)wi);
    }    

    public void AddEvent(String event)
    {
        wi.AddEvent(this, event);
    }

    public void RemoveEvent(String event)
    {
        wi.AddEvent(this, event);
    }

    public void AddEvent()
    {
        wi.AddEvent(this);
    }

    public void RemoveEvent()
    {
        wi.RemoveEvent(this);
    }
    
    public void removeNotify()
    {
        //System.out.println("Rimuovo jScopeMultiWave");
        RemoveEvent();
        this.wi = null;
        signals = null;
        orig_signals = null;
        Graphics g = getGraphics();
        g.dispose();
        super.removeNotify();
    }
    
}
