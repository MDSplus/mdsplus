import java.awt.MenuItem;
import java.awt.Point;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;


class jScopeWavePopup extends MultiWavePopup {
//	jScopeMultiWave wave;
	SetupDataDialog setup_dialog;
	protected MenuItem setup, refresh, saveAsText, sep1, sep3;
	protected MenuItem  selectWave, remove_panel;
    

    public jScopeWavePopup(SetupDataDialog setup_dialog)
    {
        super();
	    insert(setup = new MenuItem("Setup data source..."), 0);
	    setup.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                jScopeWavePopup.this.ShowSetupDialog();  
	            }
	        }
	    );
	    this.setup_dialog = setup_dialog;

	    insert(selectWave = new MenuItem("Select wave panel"), 1);
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
	  	    
	    insert(remove_panel = new MenuItem("Remove panel"), 2);
	    remove_panel.setEnabled(false);
	    remove_panel.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((WaveformManager)jScopeWavePopup.this.parent).removePanel(wave);
	            }
	        }
	    );
	
	    insert(sep1 = new MenuItem("-"), 5);

	    add(refresh = new MenuItem("Refresh"));
	    refresh.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((jScopeWaveContainer)jScopeWavePopup.this.parent).Refresh(((jScopeMultiWave)wave), "Refresh ");
	            }
	        }
	    );
	    
	    add(sep3 = new MenuItem("-"));
	    add(saveAsText = new MenuItem("Save as text ..."));
	    saveAsText.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                ((jScopeWaveContainer)jScopeWavePopup.this.parent).SaveAsText(((jScopeMultiWave)wave));
	            }
	        }
	    );

    }
    
    
	protected void SetMenuItem(boolean is_image)
	{	   
	   super.SetMenuItem(is_image);
	   if(setup_dialog != null)
           insert(setup, 0);
       add(refresh);
       if(is_image)
	   {
           insert(remove_panel, 1);
       } else {
           insert(selectWave, 1);
           insert(remove_panel, 2);
	       insert(sep1, 5);
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
                selectWave.setLabel("Deselect wave panel");
            else
                selectWave.setLabel("Select wave panel");
        }
    }

    protected void SetMenu()
    {
 	   super.SetMenu();
 	   this.wave = (jScopeMultiWave)super.wave;
       remove_panel.setEnabled(((WaveformManager)parent).GetWaveformCount() > 1);
    }

     
    protected void ShowSetupDialog()
    {
 	   jScopeMultiWave w = (jScopeMultiWave)wave;
       if(w.mode == Waveform.MODE_POINT && w.wi.signal_select != -1)
       {
	        setup_dialog.selectSignal(w.wi.signal_select);
	   } else
	        if(w.GetSignalCount() > 0 || w.is_image && w.wi.num_waves != 0)
	            setup_dialog.selectSignal(0);
       Point p = ((WaveformManager)parent).getWavePosition(w);	   
       setup_dialog.Show(w, p.x, p.y);
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
        w.wi.interpolates[w.wi.signal_select] = state;
    }

/*
    protected void SetWaveState(String label, boolean state)
    {
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        w.wi.setWaveState(label, state);
    }
*/
    public boolean SetMarker(int idx)
    {
        super.SetMarker(idx);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        if(w.wi.markers[w.wi.signal_select] != idx)
	    {
		    w.wi.markers[w.wi.signal_select] = idx;
            return false;
	    }
	    return true;
    }

    public boolean SetMarkerStep(int step)
    {
        super.SetMarkerStep(step);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
        if(w.wi.markers_step[w.wi.signal_select] != step)
	    {
            w.wi.markers_step[w.wi.signal_select] = step;
            return false;
	    }
	    return true;
    }

    public boolean SetColor(int idx)
    {
        super.SetColor(idx);
	    jScopeMultiWave w = (jScopeMultiWave)wave;
	    if(w.wi.colors_idx[w.wi.signal_select] != idx)
        {
	        w.wi.colors_idx[w.wi.signal_select] = idx;
//	        wave.wi.colors[wave.wi.signal_select] = controller.main_scope.color_dialog.GetColorAt(idx);
//            w.SetCrosshairColor(w.wi.colors[w.wi.signal_select]);
            w.SetCrosshairColor(idx);
            return false;
	    }
	    return true;
    }


}
