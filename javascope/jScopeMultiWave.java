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
	    wi = new MdsWaveInterface(this, dp, def_values);
    }
    
    public void processNetworkEvent(NetworkEvent e)
    {
         //System.out.println("Evento su waveform "+e.name);
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
            else
                Update(wi);
        } 
        catch (IOException e) 
        {
            wi.error = e.getMessage();
            Update(wi);
        }
        
    	SetMode(curr_mode);

        return wi.error;
    }
    
    
    public void jScopeErase()
    {
        Erase();
        wi.Erase();
     //   System.gc();
    }
    
    private String getBriefError(String er, boolean brief)
    {
        if(brief)
        {
            int idx = (er.indexOf('\n') == -1 ? er.length() : er.indexOf('\n'));
            er = er.substring(0, idx);
        }
        return er;
    }
    
    public String Refresh(boolean add_sig, boolean brief_error)
    {
        
        String error;
        try
        {
            AddEvent();
            error = Refresh();
            if(error != null)
                error = getBriefError(error, brief_error);
            else
                error = wi.GetErrorString(add_sig, brief_error);
        } 
        catch(IOException e)
        {
            error = e.getMessage();
        }
        
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
	        super.title = getBriefError(error, true);
	    
        super.show_legend = wi.show_legend;
	    super.legend_x = wi.legend_x;
        super.legend_y = wi.legend_y;
        super.is_image = wi.is_image;
        super.frames = wi.frames;

       // System.gc();

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
	        Update();
	        return;
	    }
	        	        
	    Erase();

    }

    protected Color getSignalColor(int i)
    {
        if(i > wi.num_waves) return Color.black;
        return colors[wi.colors_idx[i] % Waveform.colors.length];
    }

    public int GetMarker(int idx)
    {
        if(idx < wi.num_waves)
        {
            return wi.markers[idx] ;
        }
        return 0;
    }

    public int getSignalCount()
    {
        return wi.num_waves;
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
    }
    
    protected String getSignalInfo(int i)
    {
        String s;
        String name = (wi.in_label[i] != null && wi.in_label[i].length() > 0) ? wi.in_label[i] : wi.in_y[i]; 
        String er = wi.w_error[i] != null ? " ERROR " : "";
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
        return s;
    }

    protected boolean isSignalShow(int i)
    {
       return wi.GetSignalState(i);
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

    public void AddEvent(String event)  throws IOException
    {
        wi.AddEvent(this, event);
    }

    public void RemoveEvent(String event)  throws IOException
    {
        wi.AddEvent(this, event);
    }

    public void AddEvent()  throws IOException
    {
        wi.AddEvent(this);
    }

    public void RemoveEvent()  throws IOException
    {
        wi.RemoveEvent(this);
    }
    
    public void removeNotify()
    {
        //System.out.println("Rimuovo jScopeMultiWave");
        try {
            RemoveEvent();
        } 
        catch(IOException e){}

        this.wi = null;
        signals = null;
        orig_signals = null;
        Graphics g = getGraphics();
        g.dispose();
        super.removeNotify();
    }
    
}
