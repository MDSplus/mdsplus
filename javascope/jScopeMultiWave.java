import java.awt.*;
import java.util.*;
import java.io.*;
import javax.swing.SwingUtilities;

/**
Class MultiWaveform extends the capability of class Waveform to deal with multiple
waveforms.
*/
public class jScopeMultiWave extends MultiWaveform implements UpdateEventListener
{
   // MdsWaveInterface wi;
   
    
    public jScopeMultiWave(DataProvider dp, jScopeDefaultValues def_values)
    {
	    super();
	    wi = new MdsWaveInterface(this, dp, def_values);
    }
    
    public void processUpdateEvent(UpdateEvent e)
    {
         //System.out.println("Evento su waveform "+e.name);
         WaveformEvent we = new WaveformEvent(this, WaveformEvent.EVENT_UPDATE,  "Update on event " + e.name);
         dispatchWaveformEvent(we);
//         Refresh();
    }

    public void Refresh()
    {
        
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        try
        {
            AddEvent();
        } catch (IOException e){}
        
        Thread p = new Thread() {
            public void run()
            {
                ((MdsWaveInterface)wi).refresh();
                    
	            SwingUtilities.invokeLater(new Runnable() {
	                public void run() {
		            jScopeWaveUpdate();
	                }
	            });                
            }
        };
        p.start();
    }
    
    
    public void jScopeErase()
    {
        Erase();
        wi.Erase();
    }
    
    public String getBriefError(String er, boolean brief)
    {
        if(brief)
        {
            int idx = (er.indexOf('\n') == -1 ? er.length() : er.indexOf('\n'));
            er = er.substring(0, idx);
        }
        return er;
    }
    
    public void jScopeWaveUpdate()
    {
        String out_error;
        
        if(wi.isAddSignal())
        {
            //reset to previous configuration if signal/s are not added
            if(((MdsWaveInterface)wi).prev_wi != null && 
            ((MdsWaveInterface)wi).prev_wi.GetNumEvaluatedSignal() == ((MdsWaveInterface)wi).GetNumEvaluatedSignal()) 
            {
                ((MdsWaveInterface)wi).prev_wi.error = ((MdsWaveInterface)wi).error;            
                ((MdsWaveInterface)wi).prev_wi.w_error = ((MdsWaveInterface)wi).w_error;            
                ((MdsWaveInterface)wi).prev_wi.setAddSignal(wi.isAddSignal());                    
                wi = ((MdsWaveInterface)wi).prev_wi;
                wi.SetIsSignalAdded(false);
            } else
                wi.SetIsSignalAdded(true);
            
            ((MdsWaveInterface)wi).prev_wi = null;
        }
        Update(wi);
        WaveformEvent e = new WaveformEvent(this, WaveformEvent.END_UPDATE);
        dispatchWaveformEvent(e);
    }

    public void Update(WaveInterface _wi)
    {
	    wi = (MdsWaveInterface)_wi;	     	    
	    
	    resetMode();
	    	    
	    orig_signals = null;
        super.x_label = wi.xlabel;
	    super.y_label = wi.ylabel;
	    super.x_log = wi.x_log;
	    super.y_log = wi.y_log;
	    
	  //  String error = null;
	  //  if(!wi.isAddSignal())
	    wave_error = wi.getErrorTitle(true);
	        
	    if(wi.title != null)
	        super.title = wi.title;
	    else 
	        super.title = "";
	    
        super.show_legend = wi.show_legend;
	    super.legend_x = wi.legend_x;
        super.legend_y = wi.legend_y;
        super.is_image = wi.is_image;
        SetFrames(wi.frames);

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

            if(signals.size() != 0)
                signals.removeAllElements();

	        not_drawn = true;
	        frame = 0;
	        super.Update();
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
        String name = (wi.in_label != null &&  wi.in_label[i] != null && wi.in_label[i].length() > 0 ) ? wi.in_label[i] : wi.in_y[i]; 
        String er = (wi.w_error != null && wi.w_error[i] != null) ? " ERROR " : "";
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

    public void AddEvent(String event)  throws IOException
    {
        ((MdsWaveInterface)wi).AddEvent(this, event);
    }

    public void RemoveEvent(String event)  throws IOException
    {
        ((MdsWaveInterface)wi).AddEvent(this, event);
    }

    public void AddEvent()  throws IOException
    {
        ((MdsWaveInterface)wi).AddEvent(this);
    }

    public void RemoveEvent()  throws IOException
    {
        ((MdsWaveInterface)wi).RemoveEvent(this);
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
