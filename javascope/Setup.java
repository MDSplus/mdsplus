import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.io.*;


public class Setup extends Object implements WaveSetup {
    Label coords_label; 
    SetupDataDialog sd;	
    Button3Menu menu;	
    public MultiWaveform waves[];
    public int num_waves;
    public javaScope main_scope;
    public int wave_idx;


   public Setup(Frame pw, Label label)
    {
	main_scope = (javaScope)pw;
	coords_label = label;
	sd = new SetupDataDialog(pw, "Setup data");
	menu = new Button3Menu(pw, sd, this);
	pw.add(menu);
    }

    public int GetWaveIndex(Waveform w)
    {
	int idx;
	for(idx = 0; idx < num_waves &&  waves[idx] != w; idx++);
	if(idx < num_waves)
	    return idx;
	else
	    return -1;
    }
    
    public void SetSetup(Waveform w, int x, int y)    
    {
	wave_idx = GetWaveIndex(w);
    	menu.Show((MultiWaveform)w, x, y);
    }


    public WaveformConf GetWaveformConf()
    {
	return main_scope.sc.GetWaveformConf(wave_idx);
    }
    
    
    public WaveformConf GetWaveformConf(int idx)
    {
	return main_scope.sc.GetWaveformConf(idx);
    }

    public WaveformConf SaveWaveformConf(WaveformConf wc)
    {
	main_scope.sc.SetWaveformConf(wave_idx, wc);
	return(main_scope.sc.GetWaveformConf(wave_idx, true)); // Ritorno wave configuration con i 
							      //  dati di default inizializzati
	//return wc;
    }
    
    public void SetAllWaveformPointer(int pointer_mode)
    {
	for(int i = 0; i < waves.length; i++)
	    waves[i].SetMode(pointer_mode);	
    }
    
    
    public void ResetWaveInterface(int i)
    {
	waves[i].wi = null;
    }
    
    
    public void ResetAllWaveInterface()
    {
	for(int i = 0; i < waves.length; i++)
	    ResetWaveInterface(i);
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

    public void DisplayCoords(Waveform w, double curr_x, double curr_y, int sig_idx, boolean broadcast)
    { 
	
        if(coords_label != null)
	{   
	    //WaveInterface  wi = ((MultiWaveform)w).GetWaveInterface();
	    WaveInterface  wi = ((MultiWaveform)w).wi;
  	    coords_label.setText(SetStrSize("[" + Grid.ConvertToString(curr_x, false) + ", " 
				 + Grid.ConvertToString(curr_y, false) + "]", 30) +
		    " Expr : " + wi.in_y[sig_idx] +  
		    " Shot = " + wi.shots[sig_idx]);
	    if(true)
		UpdatePoints(curr_x, w);
	    else
		w.UpdatePoint(curr_x);
	}	
    }
    
    public WaveInterface GetSource()
    {
	return main_scope.wi_source;
    }
    
    public void SetSourceCopy(Waveform source)
    {
	if(source != null) {
	    main_scope.wi_source = ((MultiWaveform)source).GetWaveInterface();
	    main_scope.wc_source = main_scope.sc.GetWaveformConf(GetWaveIndex(source), false);
	    main_scope.wc_source.modified = true; 
	} else {
	    main_scope.wi_source = null;
	    main_scope.wc_source = null;
	}
    }
    
    public void NotifyChange(Waveform dest, Waveform source)    
    {
	
	WaveformConf   wc_dest = GetWaveformConf(GetWaveIndex(dest));
	wc_dest.Copy(main_scope.wc_source);
	WaveformConf new_wc = main_scope.sc.GetWaveformConf(GetWaveIndex(dest), true); 
	main_scope.UpdateWave(((MultiWaveform) dest), new_wc);

/*
	main_scope.SetStatusLabel("Update signals for shots " + main_scope.wc_source.shot_str);		
	main_scope.ew.UpdateWave(((MultiWaveform)dest), main_scope.wc_source, 
				    main_scope.sc.getWaveformShot(main_scope.wc_source, main_scope.GetMainShot()));
	main_scope.WaveCheckError(GetWaveIndex(dest));

    	main_scope.sc.modified = true;
	main_scope.setWindowTitle();
*/
    }

    public void Hide(){	
    }    	
    
    public void UpdatePoints(double x, Waveform curr_w)
    {
	for(int i = 0; i < num_waves; i++)
	    if(waves[i] != curr_w)
		waves[i].UpdatePoint(x);
    }
    
    public void AutoscaleAll()
    {
	for(int i = 0; i < num_waves; i++) {
	    waves[i].Autoscale();
	    waves[i].repaint();
	}
    }

    public void AutoscaleAllY()
    {
	for(int i = 0; i < num_waves; i++)
	    waves[i].AutoscaleY();
    }

    public void AllSameScale(MultiWaveform curr_w)
    {
	for(int i = 0; i < num_waves; i++)
	    if(waves[i] != curr_w)
		waves[i].SetScale(curr_w);
    }	

    public void AllSameXScaleAutoY(MultiWaveform curr_w)
    {
	for(int i = 0; i < num_waves; i++)
	waves[i].SetXScaleAutoY(curr_w);
    }

    public void AllSameYScale(MultiWaveform curr_w)
    {
	for(int i = 0; i < num_waves; i++)
	    if(waves[i] != curr_w)
		waves[i].SetYScale(curr_w);
    }
		
    public void AllSameXScale(MultiWaveform curr_w)
    {
	for(int i = 0; i < num_waves; i++)
		waves[i].SetXScale(curr_w);
    }

    public void ResetAllScales()
    {
	for(int i = 0; i < num_waves; i++)
	    waves[i].ResetScales();
    }
    
    public void BroadcastScale(Waveform w)
    {
	AllSameScale((MultiWaveform)w);
    }
    
    public void Refresh(Waveform w)
    {
	EvaluateWaveform ew;
	WaveformConf	wc = main_scope.sc.GetWaveformConf(wave_idx, true);
	String shot = main_scope.sc.getWaveformShot(wc, main_scope.GetMainShot());

    	ew = main_scope.ew;
	ew.error_msg.resetMsg();
	ew.evaluateMainShot(shot);
	ew.UpdateWave((MultiWaveform)w, wc, shot);
	ew.error_msg.showMessage();
    }
    
}

class Button3Menu extends PopupMenu implements ActionListener {
	MultiWaveform   wave;
	MenuItem setup, autoscale, autoscaleY, autoscaleAll, autoscaleAllY,
		 allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
		 resetScales, resetAllScales, refresh;
	SetupDataDialog sd;
	int curr_x, curr_y;
	Setup controller;

    public Button3Menu(Frame pw, SetupDataDialog _sd, Setup _controller)
    {
	sd = _sd;
	controller = _controller;
	
	add(setup = new MenuItem("Setup data source..."));
	setup.addActionListener(this);
	add(autoscale = new MenuItem("Autoscale"));
	autoscale.addActionListener(this);
	add(autoscaleY = new MenuItem("Autoscale Y"));
	autoscaleY.addActionListener(this);
	add(autoscaleAll = new MenuItem("Autoscale all"));
	autoscaleAll.addActionListener(this);
	add(autoscaleAllY = new MenuItem("Autoscale all Y"));
	autoscaleAllY.addActionListener(this);
	add(allSameScale = new MenuItem("All same scale"));
	allSameScale.addActionListener(this);
	add(allSameXScale = new MenuItem("All same X scale"));
	allSameXScale.addActionListener(this);
	add(allSameXScaleAutoY = new MenuItem("All same X scale (auto Y)"));
	allSameXScaleAutoY.addActionListener(this);
	add(allSameYScale = new MenuItem("All same Y scale"));
	allSameYScale.addActionListener(this);
	add(resetScales = new MenuItem("Reset scales"));
	resetScales.addActionListener(this);
	add(resetAllScales = new MenuItem("Reset all scales"));
	resetAllScales.addActionListener(this);
	add(refresh = new MenuItem("Refresh"));
	refresh.addActionListener(this);
    }
	
    public void Show(MultiWaveform w, int x, int y)
    {
	curr_x = x;
	curr_y = y;
	wave   = w;
	show(w, x, y );	
     }

    public void actionPerformed(ActionEvent e)
    {
	Object target = e.getSource();

		
    	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
	    wave.repaint();
	}
	if(target == (Object)autoscaleY)
	{
	    wave.AutoscaleY();
	    wave.repaint();
	}
	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
	    wave.repaint();
	}
	if(target == (Object)autoscaleAll)
	{
	    controller.AutoscaleAll();
	}
	if(target == (Object)autoscaleAllY)
	{
	    controller.AutoscaleAllY();
	}
	if(target == (Object)allSameScale)
	{
	    controller.AllSameScale(wave);	
	}
	if(target == (Object)allSameXScale)
	{
	    controller.AllSameXScale(wave);	
	}
	if(target == (Object)allSameXScaleAutoY)
	{
	    controller.AllSameXScaleAutoY(wave);	
	}
	if(target == (Object)allSameYScale)
	{
	    controller.AllSameYScale(wave);	
	}
	if(target == (Object)resetScales)
	{
	    wave.ResetScales();	
	    wave.repaint();
	}
	if(target == (Object)resetAllScales)
	{
	    controller.ResetAllScales();	
	}
	if(target == (Object)refresh)
	{
	    controller.Refresh(wave);
	   // wave.repaint();	
	}
	if(target == (Object)setup) {
	    sd.Show(wave, controller);
	}
       
    }
}
