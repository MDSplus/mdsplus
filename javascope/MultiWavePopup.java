import java.awt.CheckboxMenuItem;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;
import java.awt.MenuItem;
import java.awt.Panel;
import java.awt.TextField;
import java.awt.Label;
import java.awt.Menu;
import java.awt.CheckboxMenuItem;



public class MultiWavePopup extends WavePopup {
	protected MultiWaveform   wave = null;
	protected MenuItem legend, remove_legend;
    protected Menu signalList;
	
    public MultiWavePopup()
    {
        super();
        
	    this.insert(legend = new MenuItem("Position legend"), 0);
	    legend.addActionListener(new ActionListener()
	        {
                public void actionPerformed(ActionEvent e)
	            {
	                PositionLegend(new Point(curr_x, curr_y));	    
	            }
	        });
	    legend.setEnabled(false);
        insert(remove_legend = new MenuItem("Remove legend"), 1);
	    remove_legend.addActionListener(new ActionListener()
	        {
                public void actionPerformed(ActionEvent e)
	            {
	                RemoveLegend();    
	            }
	        });
	    remove_legend.setEnabled(false);

	    insert(signalList = new Menu("Signals"), 2);
	    signalList.setEnabled(false);

        
    }
    
	protected void RemoveLegend()
	{        
	   wave.RemoveLegend();    
    }
    
    protected void PositionLegend(Point p)
	{   
       wave.SetLegend(p);	    
    }
    
	
	protected void SetMenuItem(boolean is_image)
	{
	    super.SetMenuItem(is_image);
        if(!is_image)
        {
	       insert(legend, 0);	
           insert(remove_legend, 1);
           insert(signalList, 2);
        }
	}
	
	protected void SetSignalMenu()
	{
	    super.SetSignalMenu();
        if(wave.GetSignalCount() == 0)
        {
	       legend.setEnabled(false);
           remove_legend.setEnabled(false); 
           signalList.setEnabled(false); 
        }
	    
	}
		
	protected void InitOptionMenu()
	{
	    int sig_idx;
            String s_name[] = wave.GetSignalsName();
            boolean s_state[] = wave.GetSignalsState();
            
            if( !(s_name != null && s_state != null && 
                  s_name.length > 0 && s_name.length > 0 &&
                  s_name.length == s_state.length) )
                return;
        
            boolean state = (wave.mode == Waveform.MODE_POINT || 
                              wave.GetSignalCount() == 1);
            
            markerList.setEnabled(state);
            colorList.setEnabled(state);	
            interpolate_f.setEnabled(state);
            set_point.setEnabled(wave.mode == Waveform.MODE_POINT);
            
            if(state) {
                if(wave.GetSignalCount() == 1)
                    sig_idx = 0;
                else
                    sig_idx = wave.PointSignalIndex();
                    
                interpolate_f.setState(wave.GetInterpolate(sig_idx));
                boolean state_m = state && (wave.GetMarker(sig_idx) != Signal.NONE 
                                        && wave.GetMarker(sig_idx) != Signal.POINT);
                markerStep.setEnabled(state_m);
                SelectListItem(markerList, wave.GetMarker(sig_idx));

                int st;
                for(st = 0; st < Signal.markerStepList.length; st++)
                      if(Signal.markerStepList[st] == wave.GetMarkerStep(sig_idx))
                        break;
                SelectListItem(markerStep, st);

                SelectListItem(colorList, wave.GetColorIdx(sig_idx));

            } else
                markerStep.setEnabled(false);
                        
            CheckboxMenuItem ob;
            if(s_name != null)
            {
                signalList.removeAll();        
                signalList.setEnabled(s_name.length != 0);
	            legend.setEnabled(s_name.length != 0);
                for(int i = 0; i < s_name.length; i++)
                {
                    signalList.add(ob = new CheckboxMenuItem(s_name[i]));
                    ob.setState(s_state[i]);
	                ob.addItemListener(new ItemListener()
	                {
                        public void itemStateChanged(ItemEvent e)
	                    {
	                        Object target = e.getSource();

	                        SetSignalState(((CheckboxMenuItem)target).getLabel(), 
	                                        ((CheckboxMenuItem)target).getState());
                            wave.Repaint(true);
	                    }
	                });
                }
            }
           
           if(wave.IsShowLegend())
            	remove_legend.setEnabled(true);
	}
	


    protected void SetMenu()
    {
 	   this.wave = (MultiWaveform)super.wave;
 	   super.SetMenu();
    }

    protected void SetInterpolate(boolean state)
    {
        wave.SetInterpolate(wave.PointSignalIndex(), state);
    }

    public void SetSignalState(String label, boolean state)
    {        
        wave.SetSignalState(label, state);
    }

    public boolean SetMarker(int idx)
    {
        if(wave.GetMarker(wave.PointSignalIndex()) != idx)
        {
            wave.SetMarker(wave.PointSignalIndex(), idx);
            return false;
        }
	    return true;
    }

    public boolean SetMarkerStep(int step)
    {
        if(wave.GetMarkerStep(wave.PointSignalIndex()) != step)
        {
            wave.SetMarkerStep(wave.PointSignalIndex(), step);
            return false;
        }
	    return true;
    }

    public boolean SetColor(int idx)
    {
        if(wave.GetColorIdx(wave.PointSignalIndex()) != idx)
        {
            wave.SetColorIdx(wave.PointSignalIndex(), idx);
            return false;
        }
	    return true;
    }

}
