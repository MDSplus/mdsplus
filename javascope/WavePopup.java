import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
import java.util.*;
import javax.swing.*;


public class WavePopup extends JPopupMenu implements  ItemListener {
		
	protected Waveform   wave = null;
	protected JSeparator sep2;
	protected JMenuItem autoscale, autoscaleY, autoscaleAll, autoscaleAllY,
		      allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
		      resetScales, resetAllScales, playFrame,
		      set_point, undo_zoom; 
	protected JMenu markerList, colorList, markerStep, signal_2d;
	protected JCheckBoxMenuItem interpolate_f;
	protected JRadioButtonMenuItem plot_y_time, plot_x_y, plot_y_x;

	protected ButtonGroup markerList_bg, colorList_bg, markerStep_bg, signal_2d_bg;
	
	protected int curr_x, curr_y;
	protected Container parent;

    public WavePopup()
    {	    
	    set_point = new JMenuItem("Set Point");
	    set_point.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                WavePopup.this.SetDeselectPoint(wave);
	            }
	        }
	    );

	    
	    add(markerList = new JMenu("Markers"));	
	    JRadioButtonMenuItem ob;
        markerList_bg = new ButtonGroup();	 
	 
        for(int i = 0; i < Signal.markerList.length; i++)
        {
            markerList_bg.add(ob = new JRadioButtonMenuItem(Signal.markerList[i]));
            ob.getModel().setActionCommand("MARKER "+i);
            markerList.add(ob);
            ob.addItemListener(this);
        }      
	    markerList.setEnabled(false);
	    
        markerStep_bg = new ButtonGroup();	 
	    add(markerStep = new JMenu("Marker step"));
        for(int i = 0; i < Signal.markerStepList.length; i++)
        {
            markerStep_bg.add(ob = new JRadioButtonMenuItem(""+Signal.markerStepList[i]));
            ob.getModel().setActionCommand("MARKER_STEP "+i);
            markerStep.add(ob);
            ob.addItemListener(this);
        }
	    markerStep.setEnabled(false);
	    
	    add(colorList = new JMenu("Colors"));
	    colorList.setEnabled(false);
	    
        add(interpolate_f = new JCheckBoxMenuItem("Interpolate", false));
	    interpolate_f.setEnabled(false);
        interpolate_f.addItemListener(this);
        
        signal_2d_bg = new ButtonGroup();	 
        signal_2d = new JMenu("signal 2D");
        signal_2d.add(plot_y_time = new JRadioButtonMenuItem("Plot y & time"));
        signal_2d_bg.add(plot_y_time);
        plot_y_time.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            wave.setSignalMode(Signal.MODE_YTIME);
	        }
	    });
        
        signal_2d.add(plot_x_y = new JRadioButtonMenuItem("Plot x & y"));
        signal_2d_bg.add(plot_x_y);
        plot_x_y.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            wave.setSignalMode(Signal.MODE_XY);
	        }
	    });

        signal_2d.add(plot_y_x = new JRadioButtonMenuItem("Plot y & x"));
        signal_2d_bg.add(plot_y_x);
        plot_y_x.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            Object target = e.getSource();
	            wave.setSignalMode(Signal.MODE_YX);
                wave.Update();
	        }
	    });

    
	    add(sep2 = new JSeparator());
	    add(autoscale = new JMenuItem("Autoscale"));
	    autoscale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.Autoscale();  
	            }
	        }
	    );
	    
	    add(autoscaleY = new JMenuItem("Autoscale Y"));
	    autoscaleY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	               wave.AutoscaleY();	                
	            }
	        }
	    );
	    
	    add(autoscaleAll = new JMenuItem("Autoscale all"));
        autoscaleAll.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, ActionEvent.CTRL_MASK));
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
	    
	    
	    add(autoscaleAllY = new JMenuItem("Autoscale all Y"));
        autoscaleAllY.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
	    autoscaleAllY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AutoscaleAllY();
	            }
	        }
	    );
	    
	    add(allSameScale = new JMenuItem("All same scale"));
	    allSameScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameScale(wave);
	            }
	        }
	    );
	    
	    add(allSameXScale = new JMenuItem("All same X scale"));
	    allSameXScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameXScale(wave);
	            }
	        }
	    );
	    
	    add(allSameXScaleAutoY = new JMenuItem("All same X scale (auto Y)"));
	    allSameXScaleAutoY.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
                    ((WaveformManager)WavePopup.this.parent).AllSameXScaleAutoY(wave);
                }
	        }
	    );
	    
	    add(allSameYScale = new JMenuItem("All same Y scale"));
	    allSameYScale.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).AllSameYScale(wave);
	            }
	        }
	    );
	    
	    add(resetScales = new JMenuItem("Reset scales"));
	    resetScales.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.ResetScales();	
	            }
	        }
	    );
	    
	    add(resetAllScales = new JMenuItem("Reset all scales"));
	    resetAllScales.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)WavePopup.this.parent).ResetAllScales();	
	            }
	        }
	    );

	    add(undo_zoom = new JMenuItem("Undo Zoom"));
	    undo_zoom.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                wave.undoZoom();	               	
	            }
	        }
	    );
	    
	    
	
	    playFrame = new JMenuItem();
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
	
	protected void SelectListItem(ButtonGroup bg, int idx)
	{
	    int i;
	    JRadioButtonMenuItem b = null;
	    Enumeration e;
	    
        for (e = bg.getElements(), i = 0 ; e.hasMoreElements() && i <= idx; i++) 
             b = (JRadioButtonMenuItem)e.nextElement();
	    if(b != null)
	        bg.setSelected(b.getModel(), true);
	}

    protected void InitColorMenu()
    {
        if(!Waveform.isColorsChanged() && colorList_bg != null) return;
                
	    colorList.removeAll();

        String[] colors_name = Waveform.getColorsName();
	    JRadioButtonMenuItem ob = null;
        colorList_bg = new ButtonGroup();	 
	    if(colors_name != null)
	    {
	        for(int i = 0; i < colors_name.length; i++) {
                colorList.add(ob = new JRadioButtonMenuItem(colors_name[i]));
                ob.getModel().setActionCommand("COLOR_LIST "+i);
                colorList_bg.add(ob);
                ob.addItemListener(this);
            }
	    }
    }


	protected void SetMenuItem(boolean is_image)
	{
	   
	   removeAll();
	   
	   if(is_image)
	   {
           colorList.setText("Colors");                      
           add(colorList);	
	       add(playFrame);
	       add(sep2);
	       add(autoscale);
	       if(parent instanceof WaveformManager)           
	       {
	            autoscaleAll.setText("Autoscale all images");
	            add(autoscaleAll);
	       }
           if(wave.mode == Waveform.MODE_POINT)// && wave.sendProfile())
           {
              insert(set_point, 0);
           }     
       } else {
        
           add(markerList);
           colorList.setText("Colors");
           add(colorList);	
           add(markerStep);
           add(interpolate_f);
	       add(sep2);           
	       add(autoscale);
	       add(autoscaleY);
	       if(parent instanceof WaveformManager)           
	       {
	            autoscaleAll.setText("Autoscale all");
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
	       
           if(wave.mode == Waveform.MODE_POINT || wave.GetShowSignalCount() == 1)
           {
                if(wave.getSignalType() == Signal.TYPE_2D) {
                    insert(signal_2d, 4);
                    //plot_y_time.setState(false);
                    //plot_x_y.setState(false);
                    //plot_y_x.setState(false);
                    switch (wave.getSignalMode())
                    {
                        case Signal.MODE_YTIME : signal_2d_bg.setSelected(plot_y_time.getModel(), true); break;
                        case Signal.MODE_XY    : signal_2d_bg.setSelected(plot_x_y.getModel(), true); break;
                        case Signal.MODE_YX    : signal_2d_bg.setSelected(plot_y_x.getModel(), true); break;
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
        SelectListItem(colorList_bg, wave.GetColorIdx());
	    playFrame.setEnabled(state);
        set_point.setEnabled(state);
	}
	
	protected void SetSignalMenu()
	{
	    int sig_idx;
	    
	    SetMenuItem(false);
	    if(wave.GetShowSignalCount() != 0)
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
            boolean state = (wave.GetShowSignalCount() == 1);
            markerList.setEnabled(state);
            colorList.setEnabled(state);	
            interpolate_f.setEnabled(state);
            set_point.setEnabled(true);
            
            if(state) {
                interpolate_f.setState(wave.GetInterpolate());
                boolean state_m = (wave.GetMarker() != Signal.NONE);
                markerStep.setEnabled(state_m);
                SelectListItem(markerList_bg, wave.GetMarker());

                int st;
                for(st = 0; st < Signal.markerStepList.length; st++)
                      if(Signal.markerStepList[st] == wave.GetMarkerStep())
                        break;
                SelectListItem(markerStep_bg, st);

                SelectListItem(colorList_bg, wave.GetColorIdx());

            } else
                markerStep.setEnabled(false);
	}
		
    public void Show(Waveform w, int x, int y)
    {
     //   parent = (Container)this.getParent();
        
       // if(wave != w)
        {
 	        wave = w;
 	        SetMenu();
        }
       //else
       //     if(!w.IsImage())
       //         InitOptionMenu();

        SetMenuLabel();
     
	    curr_x = x;
	    curr_y = y;
	    show(w, x, y );	
     }
     
    protected void SetMenuLabel()
    {
        if(!wave.IsImage())
        {	    
            if(wave.ShowMeasure()) {
                set_point.setText("Deselect Point");
            } else
                set_point.setText("Set Point");
        } else {
            
            if(wave.ShowMeasure())// && wave.sendProfile())
                set_point.setText("Deselect Point");
            else
                set_point.setText("Set Point");

            if(wave.is_playing)
	            playFrame.setText("Stop play");
	        else 
	            playFrame.setText("Start play");
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

    protected void SetMarker(int idx)
    {
        if(wave.GetMarker() != idx)
            wave.SetMarker(idx);
    }

    protected void SetMarkerStep(int step)
    {
        if(wave.GetMarkerStep() != step)
            wave.SetMarkerStep(step);
    }
    
    public void setParent(Container parent)
    {
        this.parent = parent;
    }

    protected void SetColor(int idx)
    {
        if(wave.GetColorIdx() != idx)
            wave.SetColorIdx(idx);
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
	    
	    if(target == interpolate_f)
	    {
            SetInterpolate(((JCheckBoxMenuItem)target).getState()); 	            
            wave.Repaint(true);
            return;
	    }
	    
	    if(target instanceof JRadioButtonMenuItem && e.getStateChange() == ItemEvent.SELECTED)
	    {
	        JRadioButtonMenuItem cb = (JRadioButtonMenuItem)target;            
            String action_cmd = cb.getModel().getActionCommand();
	        
	        if(action_cmd == null)
	            return;
	            
	        StringTokenizer act = new StringTokenizer(action_cmd);
	        String action = act.nextToken();
	        int    idx    = Integer.parseInt(act.nextToken());
	        
	        if(action.equals("MARKER"))
	        {
	            SetMarker(idx);
	            markerStep.setEnabled(!(wave.GetMarker() == Signal.NONE || 
	                                    wave.GetMarker() == Signal.POINT));
	            wave.Repaint(true);
	            return;
	        }
	        
	        if(action.equals("MARKER_STEP"))
	        {
	           SetMarkerStep(Signal.markerStepList[idx]);
	           wave.Repaint(true);	                
	           return;
	        }

	        if(action.equals("COLOR_LIST"))
	        {
	           SetColor(idx);
	           wave.Repaint(true);	                
	           return;
	        }	       
	    }
	}   
}