import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.util.*;


public class WavePopup extends PopupMenu implements  ItemListener {
		
	protected Waveform   wave = null;
	protected MenuItem autoscale, autoscaleY, autoscaleAll, autoscaleAllY,
		      allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
		      resetScales, resetAllScales, sep2, playFrame,
		      set_point, undo_zoom; 
	protected Menu markerList, colorList, markerStep;
	protected CheckboxMenuItem interpolate_f;

	protected Menu signal_2d;
	protected CheckboxMenuItem plot_y_time, plot_y_x;
	
	protected Panel setup_dialog = null;
	protected int curr_x, curr_y;
	protected Container parent;

    public WavePopup()
    {	    
	    set_point = new MenuItem("Set Point");
	    set_point.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                WavePopup.this.SetDeselectPoint(wave);
	            }
	        }
	    );

	    
	    add(markerList = new Menu("Markers"));	
	    CheckboxMenuItem ob;
	 
        for(int i = 0; i < Signal.markerList.length; i++)
        {
            markerList.add(ob = new CheckboxMenuItem(Signal.markerList[i]));
            ob.addItemListener(this);
        }
        
	    markerList.setEnabled(false);	    
	    add(markerStep = new Menu("Marker step"));
        for(int i = 0; i < Signal.markerStepList.length; i++)
        {
            markerStep.add(ob = new CheckboxMenuItem(""+Signal.markerStepList[i]));
            ob.addItemListener(this);
        }
	    markerStep.setEnabled(false);
	    add(colorList = new Menu("Colors"));
	    colorList.setEnabled(false);
        add(interpolate_f = new CheckboxMenuItem("Interpolate", false));
	    interpolate_f.setEnabled(false);
        interpolate_f.addItemListener(this);
        
        signal_2d = new Menu("signal 2D");
        signal_2d.add(plot_y_time = new CheckboxMenuItem("Plot y & time"));
        plot_y_time.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            Object target = e.getSource();
	            wave.setSignalMode(Signal.MODE_YTIME);
                wave.Update();
	        }
	    });
        
        signal_2d.add(plot_y_x = new CheckboxMenuItem("Plot y & x"));
        plot_y_x.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            Object target = e.getSource();
	            wave.setSignalMode(Signal.MODE_YX);
                wave.Update();
	        }
	    });
    
	    add(sep2 = new MenuItem("-"));
	    add(autoscale = new MenuItem("Autoscale"));
	    autoscale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.Autoscale();  
	            }
	        }
	    );
	    
	    add(autoscaleY = new MenuItem("Autoscale Y"));
	    autoscaleY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	               wave.AutoscaleY();	                
	            }
	        }
	    );
	    
	    add(autoscaleAll = new MenuItem("Autoscale all", new MenuShortcut(KeyEvent.VK_B)));
	    autoscaleAll.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                if(wave.IsImage())
	                    ((WaveformManager)WavePopup.this.parent).AutoscaleAllImages();
	                else
	                    ((WaveformManager)WavePopup.this.parent).AutoscaleAll();
	            }
	        }
	    );
	    
	    
	    add(autoscaleAllY = new MenuItem("Autoscale all Y", new MenuShortcut(KeyEvent.VK_Y)));
	    autoscaleAllY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AutoscaleAllY();
	            }
	        }
	    );
	    
	    add(allSameScale = new MenuItem("All same scale"));
	    allSameScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameScale(wave);
	            }
	        }
	    );
	    
	    add(allSameXScale = new MenuItem("All same X scale"));
	    allSameXScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameXScale(wave);
	            }
	        }
	    );
	    
	    add(allSameXScaleAutoY = new MenuItem("All same X scale (auto Y)"));
	    allSameXScaleAutoY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
                    ((WaveformManager)WavePopup.this.parent).AllSameXScaleAutoY(wave);
                }
	        }
	    );
	    
	    add(allSameYScale = new MenuItem("All same Y scale"));
	    allSameYScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameYScale(wave);
	            }
	        }
	    );
	    
	    add(resetScales = new MenuItem("Reset scales"));
	    resetScales.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.ResetScales();	
	            }
	        }
	    );
	    
	    add(resetAllScales = new MenuItem("Reset all scales"));
	    resetAllScales.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).ResetAllScales();	
	            }
	        }
	    );

	    add(undo_zoom = new MenuItem("Undo Zoom"));
	    undo_zoom.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.undoZoom();	               	
	            }
	        }
	    );
	    
	    
	
	    playFrame = new MenuItem();
	    playFrame.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                if(wave.Playing())
	                    wave.StopFrame();
	                else
	                    wave.PlayFrame();
	            }
	        }
	    );
    }
	
	protected void SelectListItem(Menu m, int idx)
	{
        for(int i = 0; i < m.getItemCount(); i++)
	        ((CheckboxMenuItem)m.getItem(i)).setState(false);
	    if(idx < m.getItemCount())
            ((CheckboxMenuItem)m.getItem(idx)).setState(true);
	}
	
	protected int GetSelectedItem(Menu m, Object ob)
	{
	    int idx = 0;
	    for(int i = 0; i < m.getItemCount(); i++)
	    {
	        if(m.getItem(i) == ob) {
	            idx = i;
	            continue;
	        }
	        if(((CheckboxMenuItem)m.getItem(i)).getState())
	            ((CheckboxMenuItem)m.getItem(i)).setState(false);
	    }
	    return idx;
	}

    protected void InitColorMenu()
    {
	    colorList.removeAll();

        String[] colors_name = wave.GetColorsName();
	    CheckboxMenuItem ob = null;
	    if(colors_name != null)
	    {
	        for(int i = 0; i < colors_name.length; i++) {
                colorList.add(ob = new CheckboxMenuItem(colors_name[i]));
                ob.addItemListener(this);
            }
	    }
    }


	protected void SetMenuItem(boolean is_image)
	{
	   
	   removeAll();
	   
	   if(is_image)
	   {
           colorList.setLabel("Colors");                      
           add(colorList);	
	       add(playFrame);
	       add(sep2);
	       add(autoscale);
	       if(parent instanceof WaveformManager)           
	       {
	            autoscaleAll.setLabel("Autoscale all images");
	            add(autoscaleAll);
	       }
           if(wave.mode == Waveform.MODE_POINT)// && wave.sendProfile())
           {
              insert(set_point, 0);
           }     
       } else {
        
           add(markerList);
           colorList.setLabel("Colors");
           add(colorList);	
           add(markerStep);
           add(interpolate_f);
	       add(sep2);           
	       add(autoscale);
	       add(autoscaleY);
	       if(parent instanceof WaveformManager)           
	       {
	            autoscaleAll.setLabel("Autoscale all");
	            add(autoscaleAll);
	            add(autoscaleAllY);
	            add(allSameScale);
	            add(allSameXScale);
	            add(allSameXScaleAutoY);
	            add(allSameYScale);
	            add(resetAllScales);
	       }
	       add(resetScales);
	       add(undo_zoom);
	       
           if(wave.mode == Waveform.MODE_POINT || wave.GetSignalCount() == 1)
           {
                if(wave.getSignalType() == Signal.TYPE_2D) {
                    insert(signal_2d, 4);
                    plot_y_time.setState(false);
                    plot_y_x.setState(false);
                    switch (wave.getSignalMode())
                    {
                        case Signal.MODE_YTIME : plot_y_time.setState(true); break;
                        case Signal.MODE_YX : plot_y_x.setState(true); break;
                    }
                }
           }     
           if(wave.mode == Waveform.MODE_POINT)
           {
              insert(set_point, 0);
           }     
       }
	}

	
	protected void SetImageMenu()
	{
	    SetMenuItem(true);
	    boolean state = (wave.frames != null && wave.frames.getNumFrame() != 0);
        colorList.setEnabled(state);	
        SelectListItem(colorList, wave.GetColorIdx());
	    playFrame.setEnabled(state);
        set_point.setEnabled(state);
	}
	
	protected void SetSignalMenu()
	{
	    int sig_idx;
	    
	    SetMenuItem(false);
	    
        
        if(wave.GetSignalCount() != 0)
        {
           InitOptionMenu(); 	
        } else {
            markerList.setEnabled(false);
            colorList.setEnabled(false);	
            interpolate_f.setEnabled(false);
            markerStep.setEnabled(false);
            set_point.setEnabled(false);
       }
       undo_zoom.setEnabled(wave.undoZoomPendig());
   
	}
	
	
	protected void InitOptionMenu()
	{
            boolean state = (wave.GetSignalCount() == 1);
            markerList.setEnabled(state);
            colorList.setEnabled(state);	
            interpolate_f.setEnabled(state);
            set_point.setEnabled(true);
            
            if(state) {
                interpolate_f.setState(wave.GetInterpolate());
                boolean state_m = (wave.GetMarker() != Signal.NONE);
                markerStep.setEnabled(state_m);
                SelectListItem(markerList, wave.GetMarker());

                int st;
                for(st = 0; st < Signal.markerStepList.length; st++)
                      if(Signal.markerStepList[st] == wave.GetMarkerStep())
                        break;
                SelectListItem(markerStep, st);

                SelectListItem(colorList, wave.GetColorIdx());

            } else
                markerStep.setEnabled(false);
	}
		
    public void Show(Waveform w, int x, int y)
    {
        parent = (Container)this.getParent();
        
       // if(wave != w)
       // {
 	        wave = w;
 	        SetMenu();
       // } else
            if(!w.IsImage())
                InitOptionMenu();

        SetMenuLabel();
     
	    curr_x = x;
	    curr_y = y;
	    show(w, x, y );	
     }

    protected void SetMenuLabel()
    {
        if(!wave.IsImage())
        {	    
            if(wave.ShowMeasure())
                set_point.setLabel("Deselect Point");
            else
                set_point.setLabel("Set Point");
        } else {
            
            if(wave.ShowMeasure())// && wave.sendProfile())
                set_point.setLabel("Deselect Point");
            else
                set_point.setLabel("Set Point");

            if(wave.is_playing)
	            playFrame.setLabel("Stop play");
	        else 
	            playFrame.setLabel("Start play");
	    }
    }

    protected void SetMenu()
    {
        InitColorMenu();
        if(wave.is_image)
            SetImageMenu();
        else
            SetSignalMenu();
    }

    protected void SetInterpolate(boolean state)
    {
        wave.SetInterpolate(state);
    }

    protected boolean SetMarker(int idx)
    {
        if(wave.GetMarker() != idx)
        {
            wave.SetMarker(idx);
            return false;
        }
	    return true;
    }

    protected boolean SetMarkerStep(int step)
    {
        if(wave.GetMarkerStep() != step)
        {
            wave.SetMarkerStep(step);
            return false;
        }
	    return true;
    }

    protected boolean SetColor(int idx)
    {
        if(wave.GetColorIdx() != idx)
        {
            wave.SetColorIdx(idx);
            return false;
        }
	    return true;
    }

    public void SetDeselectPoint(Waveform w)
    {
        if(w.ShowMeasure())
        {
            if(parent instanceof WaveformManager)
                ((WaveformManager)parent).SetShowMeasure(false);
            w.SetShowMeasure(false);
        } else {
            if(parent instanceof WaveformManager)
                ((WaveformManager)parent).SetShowMeasure(true);
            w.SetShowMeasure(true);
            w.SetPointMeasure();
        }
        w.repaint();
    }
   
    
    public void itemStateChanged(ItemEvent e)
    {
	    Object target = e.getSource();
	    
	    if(target instanceof CheckboxMenuItem)
	    {
	        CheckboxMenuItem cb = (CheckboxMenuItem)target;
	        Menu parent = (Menu)cb.getParent();
	        
	        if(target == interpolate_f)
	        {
                SetInterpolate(((CheckboxMenuItem)target).getState()); 	            
                wave.Repaint(true);
	        }
	        
	        if(parent == markerList)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {       
	                if(SetMarker(idx)) {
	                    cb.setState(true);
	                }
	                markerStep.setEnabled(!(wave.GetMarker() == Signal.NONE || 
	                                        wave.GetMarker() == Signal.POINT));
	            }
	            wave.Repaint(true);	                
	        }

	        if(parent == markerStep)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {
	                if(SetMarkerStep(Signal.markerStepList[idx]))
	                    cb.setState(true);	                
	            }
	            wave.Repaint(true);	                
	        }

	        
	        if(parent == colorList)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {  
	               if(SetColor(idx))
	                  cb.setState(true);	               
	            }
	            wave.Repaint(true);	                
	        }
	    }
	}   
}