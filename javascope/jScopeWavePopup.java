import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.io.File;

import java.io.FileWriter;
import java.io.BufferedWriter;
import java.io.IOException;
import javax.swing.*;


class jScopeWavePopup extends MultiWavePopup {
	private   SetupDataDialog setup_dialog;
	protected JMenuItem refresh, saveAsText;
	protected JMenuItem  selectWave, remove_panel, profile_dialog;
	protected JSeparator sep1, sep3;
        
    public jScopeWavePopup(SetupDataDialog setup_dialog)
    {
        super();
        /*
	    insert(setup = new JMenuItem("Setup data source..."), 0);
	    setup.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                jScopeWavePopup.this.ShowSetupDialog();  
	            }
	        }
	    );
	    this.setup_dialog = setup_dialog;
        */
        setup.setText("Setup data source...");
	    this.setup_dialog = setup_dialog;
        
	    insert(selectWave = new JMenuItem("Select wave panel"), 2);
	    selectWave.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                if(wave != ((WaveformManager)jScopeWavePopup.this.parent).GetSelected())
	                    ((WaveformManager)jScopeWavePopup.this.parent).Select(wave);
	                else
	                    ((WaveformManager)jScopeWavePopup.this.parent).Deselect();	        
	            }
	        }
	    );
	  	    
	    insert(remove_panel = new JMenuItem("Remove panel"), 3);
	    remove_panel.setEnabled(false);
	    remove_panel.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                Object[] options = {"Yes",
                                        "No"};
                    int opt = JOptionPane.showOptionDialog(null,
                            "Are you sure you want to remove this wave panel?",
                            "Warning",
                            JOptionPane.YES_NO_OPTION,
                            JOptionPane.QUESTION_MESSAGE,
                            null,
                            options,
                            options[1]);
	                switch(opt)
	                {
		                case JOptionPane.YES_OPTION :
		                    ((WaveformManager)jScopeWavePopup.this.parent).removePanel(wave);;
		                break;
	                }
	                
	            }
	        }
	    );
	
	    insert(sep1 = new JSeparator(), 6);

	    add(refresh = new JMenuItem("Refresh"));
	    refresh.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((jScopeWaveContainer)jScopeWavePopup.this.parent).Refresh(((jScopeMultiWave)wave), "Refresh ");
	            }
	        }
	    );
	    
	    add(sep3 = new JSeparator());
	    add(saveAsText = new JMenuItem("Save as text ..."));
	    saveAsText.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((jScopeWaveContainer)jScopeWavePopup.this.parent).SaveAsText(((jScopeMultiWave)wave), false);
	            }
	        }
	    );

	    profile_dialog = new JMenuItem("Show profile dialog");
	    profile_dialog.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((jScopeWaveContainer)jScopeWavePopup.this.parent).ShowProfileDialog(((jScopeMultiWave)wave));
	            }
	        }
	    );

    }

    protected void ShowDialog()
    {
	    jScopeWavePopup.this.ShowSetupDialog();  
    }

    
	protected void SetMenuItem(boolean is_image)
	{	   
	   super.SetMenuItem(is_image);
	   if(setup_dialog != null)
           insert(setup, 0);
       add(refresh);
       if(is_image)
	   {
           insert(profile_dialog, 2);
           insert(remove_panel, 3);
       } else {
           insert(selectWave, 2);
           insert(remove_panel, 3);
	       insert(sep1, (wave.isFixedLegend() ? 5: 6));
	       add(sep3);
	       add(saveAsText);
       }
       
       //come patch da eliminare
       if( wave != null && ((jScopeMultiWave)wave).wi.num_waves == 1) 
            ((jScopeMultiWave)wave).wi.signal_select = 0;
	}

    protected void SetMenuLabel()
    {
        super.SetMenuLabel();
        if(!wave.IsImage())
        {
            if(wave.IsSelected())
                selectWave.setText("Deselect wave panel");
            else
                selectWave.setText("Select wave panel");
        } else {
            profile_dialog.setEnabled(!wave.isSendProfile());
            //if(wave.sendProfile())
                //profile_dialog.setLabel("Remove profile dialog");
            //else
                //profile_dialog.setLabel("Show profile dialog");
        }
    }

    protected void SetMenu()
    {
 	   super.SetMenu();
 	   this.wave = (jScopeMultiWave)super.wave;
       remove_panel.setEnabled(((WaveformManager)parent).GetWaveformCount() > 1);
       jScope_1.jScopeSetUI(this);
    }

     
    protected void ShowSetupDialog()
    {
 	   jScopeMultiWave w = (jScopeMultiWave)wave;
       if(w.mode == Waveform.MODE_POINT)// && w.wi.signal_select != -1)
       {
	        setup_dialog.selectSignal(w.GetSelectedSignal());
	   } else
	        if(w.GetShowSignalCount() > 0 || w.is_image && w.wi.num_waves != 0)
	            setup_dialog.selectSignal(1);
        Timer t = new Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) 
            {
                Point p = ((WaveformManager)jScopeWavePopup.this.parent).getWavePosition(wave);	   
                setup_dialog.Show(wave, p.x, p.y);
            }
        });
        t.setRepeats(false);
        t.start();
    }

	protected void RemoveLegend()
	{        
       super.RemoveLegend();
	   ((jScopeMultiWave)wave).wi.ShowLegend(false);
    }
    
    protected void PositionLegend(Point p)
	{   
       super.PositionLegend(p);
	   ((jScopeMultiWave)wave).wi.ShowLegend(true);
	   ((jScopeMultiWave)wave).wi.SetLegendPosition(((jScopeMultiWave)wave).GetLegendXPosition(), ((jScopeMultiWave)wave).GetLegendYPosition());
    }


    protected void SetInterpolate(boolean state)
    {
        super.SetInterpolate(state);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        w.wi.interpolates[w.GetSelectedSignal()] = state;
    }

    public void SetDeselectPoint(Waveform w)
    {
        
        String f_name = System.getProperty("jScope.save_selected_points");
        
        if(w.ShowMeasure() && f_name != null && f_name.length() != 0)
        {
            int shot = 0;
            jScopeMultiWave mw = (jScopeMultiWave) w;
            if(mw.wi.shots != null)
                shot = mw.wi.shots[mw.GetSelectedSignal()];
            
            try
            {
                boolean exist = false;
                File f = new File(f_name);
                if(f.exists())
                    exist = true;
                BufferedWriter  out = new BufferedWriter(new FileWriter(f_name, true));
                if(!exist)
                {
                    out.write(" Shot X1 Y1 X2 Y2");
                    out.newLine();
                }
                out.write(" "+shot+w.getIntervalPoints());
                out.newLine();
                out.close();
            } catch(IOException e){}
        }        
        super.SetDeselectPoint(w);
    }
    
    public void SetSignalState(String label, boolean state)
    {
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        w.SetSignalState(label, state);
    }

    public void SetMarker(int idx)
    {
        super.SetMarker(idx);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        if(w.wi.markers[w.GetSelectedSignal()] != idx)
		    w.wi.markers[w.GetSelectedSignal()] = idx;
    }

    public void SetMarkerStep(int step)
    {
        super.SetMarkerStep(step);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        if(w.wi.markers_step[w.GetSelectedSignal()] != step)
            w.wi.markers_step[w.GetSelectedSignal()] = step;
    }

    public void SetColor(int idx)
    {
        super.SetColor(idx);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
	    if(w.wi.colors_idx[w.GetSelectedSignal()] != idx)
        {
	        w.wi.colors_idx[w.GetSelectedSignal()] = idx % w.colors.length;
            w.SetCrosshairColor(idx);
	    }
    }
}
