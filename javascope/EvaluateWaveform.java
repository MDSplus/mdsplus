import java.io.*;
import java.awt.*;
import java.util.*;


//******
//Classe utilizzata per aggiornare la wave selezionate
//****** 

 class EvaluateWaveform {
   javaScope        main_scope;
   MultiWaveform    wave;
   WaveSetupData    signals[];   
   int		    shots[], main_shots[];
   int		    num_shot, num_signal;
   ErrorMessage	    error_msg;

   EvaluateWaveform(javaScope _main_scope)
   {
      main_scope = _main_scope;
      error_msg = new ErrorMessage(main_scope);
   }
      
   public void setMainScope(javaScope _main_scope)
   {
      main_scope = _main_scope;
   }
 
   public int findSignalSetup(WaveSetupData ws)
   {
      for(int i=0; i < num_signal; i++)
	if(signals[i].equals(ws))
           return i;	 		
      return -1;
   }
   
  public void updateSignalSetup(int idx, WaveSetupData ws)
  {
      
	signals[idx].color_idx   = ws.color_idx;
	signals[idx].marker      = ws.marker;
	signals[idx].interpolate = ws.interpolate;
	if(ws.up_err != null)
	    signals[idx].up_err  = new String(ws.up_err);
	if(ws.low_err != null)
	    signals[idx].low_err = new String(ws.low_err);
   }
   
   private int[] addNewShot(int s, int idx, int[] s_arr)
   {
	int[] s_arr_new;

	if(idx >= s_arr.length)
	{
	    s_arr_new = new int[s_arr.length + 1];
	    for(int i = 0; i < s_arr.length; i++)
		s_arr_new[i] = s_arr[i];
	    s_arr = s_arr_new;
	}
	s_arr[idx] = s;
	return s_arr;
   }

   public int evaluateMainShot(String in_main_shots)
   {   
	main_shots = evaluateShot(in_main_shots);
	if(main_shots[0] == WaveSetupData.UNDEF_SHOT)
	    return 0;
    	return 1;   
   }
   
   
   private int[] evaluateShot(String in_shots)
   {
	return evaluateShotTdi(in_shots);
   }
   
   
   private int[] evaluateShotTdi(String in_shots)
   {
	int int_data[];
	
	if(in_shots == null || in_shots.length() == 0)
	{
	    int_data = new int[1];
	    int_data[0] = WaveSetupData.UNDEF_SHOT;
	//    error_msg.setMessage("Undefined shot\n");
	//    error_msg.showMessage();
	    return int_data;
	}
	
	main_scope.db.Update(null, 0);
	int_data = main_scope.db.GetIntArray(in_shots);
	if( int_data == null || int_data.length == 0 || int_data.length > WaveSetupData.MAX_NUM_SHOT)
	{
	    if(int_data != null && int_data.length > WaveSetupData.MAX_NUM_SHOT)
		error_msg.setMessage("Too many shots. Max shot list elements " + WaveSetupData.MAX_NUM_SHOT +"\n");
	    else {
		if(main_scope.db.ErrorString() != null)	    
		    error_msg.setMessage(main_scope.db.ErrorString());
		else
		    error_msg.setMessage("Shot syntax error\n");
		
	    }
	    error_msg.showMessage();
	    int_data = new int[1];
	    int_data[0] = WaveSetupData.UNDEF_SHOT;
	}
	return int_data;
   }

   private int[] evaluateShotString(String in_shots)
   {
	int[]  s_value = new int[1];
	int  s_range, s_curr; 
	int error_cd = 0;
    	int i = 0, k = 0, j = 0;
	
	String token = null;
	char delim = ' ', last_delim = ' ';

	if(in_shots == null || in_shots.length() == 0)
	{
	    s_value[0] = WaveSetupData.UNDEF_SHOT;
	    return s_value;
	}
	    
	StringTokenizer st = new StringTokenizer(in_shots, ",:");
		
	try {
	    while(st.hasMoreTokens()) {

		if(j > WaveSetupData.MAX_NUM_SHOT)
		{
		    error_cd = 1;
		    break;
		}

    		token = st.nextToken();
		i += token.length();
		delim = (i < in_shots.length() ? in_shots.charAt(i) : ' ');	
		if(delim == ':' && delim == last_delim)
		{
		    error_cd = 2; //Definizione intervallo errata
		    break;
		} else {
		    switch(last_delim)
		    {
			case ',' :
			case ' ' :
			    s_value = addNewShot(Integer.parseInt(token.trim()), j++, s_value);
			break;
			case ':' :
			    s_range = Integer.parseInt(token.trim());
			    if(s_value[j-1] < s_range) {
				for(s_curr = s_value[j-1]+1; s_curr <= s_range && j < WaveSetupData.MAX_NUM_SHOT; s_curr++)
				    s_value = addNewShot(s_curr, j++, s_value);
			    } else
				error_cd = 3; //Intervallo shot errato	
			break;
		    }
		}
		i++;
		last_delim = delim;
	     }
	}
	catch(NumberFormatException e)
	{
	    error_cd = 4;
	    error_msg.setMessage("Invalid shot value : " + token + "\n");
	} 
	
	
	if(error_cd != 0) {
	    if(error_cd != 4)
		error_msg.setMessage("Shots syntax error \n Max number of shots is 10 \n Syntax shots interval: start:end \n shots list: shot1, shot2, etc..\n");
	    error_msg.showMessage();
	    s_value[0] = WaveSetupData.UNDEF_SHOT;
    	}
	
    	return (s_value); 
   } 

//    public void signalsRefresh(String in_shot, String in_main_shots, boolean use_main_shot)
    public void signalsRefresh(String in_shot)
   {
	int new_num_shot, new_num_signal = 0, sig_idx, new_main_shot, new_shots[]; 
	WaveSetupData[] new_sigs;
	int color_idx = 0;
    
	if(in_shot == null)
	    return;
	
	new_shots  = evaluateShot(in_shot);
	if(new_shots != null)
	    new_num_shot = new_shots.length;
	else
	    new_num_shot = 0;


	if(num_shot == 0)
	    sig_idx = 1;				        
	else
	    sig_idx = num_shot;
	        
	new_sigs = new WaveSetupData[(num_signal/sig_idx) * new_num_shot];
	    
    	for(int j = 0; j < num_signal; j += sig_idx)
	{	    		    
	    for(int i = 0; i < new_num_shot; i++)
	    {
		new_sigs[new_num_signal] = new WaveSetupData();
		if(i < num_shot)
		    new_sigs[new_num_signal].copy(signals[j + i]);
		else {
		    new_sigs[new_num_signal].copy(signals[j]);
		    color_idx = (color_idx + 1) % main_scope.color_dialog.GetNumColor();
		    new_sigs[new_num_signal].color_idx = color_idx;
		}
		new_sigs[new_num_signal].shot = new_shots[i];
		new_num_signal++;
	    }
	}
	
	if(new_num_signal != 0)
	{
	    num_signal = new_num_signal;
	    signals    = new_sigs;
	}

    	num_shot   = new_num_shot;
	shots      = new_shots;
    } 
   
   public void initSignals(WaveformConf wc, String curr_shot)   
   { 
	if(wc == null) {
    	    shots = null;
	    signals = null;
	    num_shot = 0;
	    num_signal = 0;
    	    return;
	}
    	shots = new int[wc.num_shot];
	shots = evaluateShot(curr_shot);
	wc.num_shot = shots.length;
	num_shot = wc.num_shot;
	num_signal = wc.num_expr * wc.num_shot;
	signals = new WaveSetupData[num_signal];
	
	for(int j = 0, k = 0;  j < wc.num_expr; j++)
	{ 
	    for(int i = 0; i < wc.num_shot; i++)
	    {
		signals[k] = new WaveSetupData();
		if(wc.y_expr[j] != null)
		    signals[k].y_expr = new String(wc.y_expr[j]);
		if(wc.x_expr[j] != null)
		    signals[k].x_expr = new String(wc.x_expr[j]);
		signals[k].shot  = shots[i];
		if(wc.up_err[j] != null)
		   signals[k].up_err = new String(wc.up_err[j]);
		if(wc.low_err[j] != null)
    		   signals[k].low_err = new String(wc.low_err[j]);
		if(wc.markers.length > k)
		{
		    signals[k].interpolate = wc.interpolates[k];
		    signals[k].marker      = wc.markers[k];
		    if(wc.colors_idx[k] == -1)
			signals[k].color_idx = wc.colors_idx[k] = k % main_scope.color_dialog.GetNumColor();
		    else
			signals[k].color_idx = wc.colors_idx[k];		    
		} else {
		    signals[k].interpolate = true;
		    signals[k].marker = 0;
		    signals[k].color_idx = k %main_scope.color_dialog.GetNumColor();
		}
		k++;
	    }
	}  
    }

   private void InitWaveInterface(WaveformConf wc)
   {
      WaveInterface wi = wave.wi;
              		
      if(num_signal == 0)
      {	
	wave.wi = new WaveInterface(main_scope.db);
	return;
      }
      
      if( wi != null && !wc.modified)
      {
	    for(int i = 0; i < wi.num_waves; i++)      
	    {
		wi.markers[i]      = signals[i].marker;
		wi.interpolates[i] = signals[i].interpolate;
		wi.colors[i]       = main_scope.color_dialog.GetColorAt(signals[i].color_idx);
	    }
	    return;	 
      }
 
      if(wi == null)
	 wi = new WaveInterface(main_scope.db);
	 
	wi.num_waves    = num_signal;
	if(wc.experiment != null && wc.experiment.length() == 0)
	    wi.experiment   = null;
	else
	    wi.experiment   = wc.experiment;
	wi.in_xmax      = wc.x_max;
	wi.in_xmin      = wc.x_min;
	wi.in_ymax      = wc.y_max;
	wi.in_ymin      = wc.y_min;
	wi.in_title     = wc.title;
	wi.in_xlabel    = wc.x_label;
	wi.in_ylabel    = wc.y_label;
	wi.y_log        = wc.y_log;
	wi.x_log        = wc.x_log;
	wi.full_flag	= !main_scope.GetFastNetworkState();
     
    	wi.in_x         = new String[num_signal];
	wi.in_y         = new String[num_signal];
	wi.in_up_err    = new String[num_signal];
	wi.in_low_err   = new String[num_signal];
	wi.markers      = new int[num_signal];
	wi.colors       = new Color[num_signal];
	wi.interpolates = new boolean[num_signal];

//	if(signals[0].shot == WaveSetupData.UNDEF_SHOT)
//	    wi.shots        = null;
//	else
	    wi.shots        = new int[num_signal];				

	for(int i = 0; i < num_signal; i++)      
	{
	  if(signals[i].x_expr != null)
	     wi.in_x[i]         = new String(signals[i].x_expr);
	  if(signals[i].y_expr != null)	     
	    wi.in_y[i]         = new String(signals[i].y_expr);
	  wi.markers[i]      = signals[i].marker;
	  wi.interpolates[i] = signals[i].interpolate;
	  if(wi.shots != null)
	    wi.shots[i]        = signals[i].shot;
	  if(signals[i].up_err != null)
	    wi.in_up_err[i] = new String(signals[i].up_err);
	  if(signals[i].low_err != null)      
    	    wi.in_low_err[i] = new String(signals[i].low_err);	    
	  wi.colors[i]       = main_scope.color_dialog.GetColorAt(signals[i].color_idx);
	}
	wave.wi = wi;        
   }

   public void RefreshWave(WaveformConf wc)
   {
 	InitWaveInterface(wc);
	WaveInterface wi = wave.wi;
        
	if(wi == null)
	    return;
	if(wi.error == null)
	{
	    wave.Update(wi);
	} 		
   }
   	       	    
   public void UpdateWave(WaveformConf wc)
   {
	InitWaveInterface(wc);
	WaveInterface wi = wave.wi;
        
	if(wi == null)
	    return;
	        	
	wi.StartEvaluate();
	if(wi.error == null)
	{
	    wi.EvaluateOthers();
	    wave.Update(wi);
	} 
	
   }

   private void UpdateWaveOther(MultiWaveform w)
   {
	WaveInterface wi = w.wi;

	if(wi == null) return;
	            
	wi.EvaluateOthers();
	w.Update(wi);
   }

   public void UpdateWave(MultiWaveform w, int shot)
   {
	WaveInterface wi = w.wi;
          
	if(wi.error == null)
	{
	    wi.EvaluateShot(shot);
	} 		    
   }

   public void UpdateInterface(MultiWaveform _wave, WaveformConf wc, String shots)
   {	
    	wave = _wave;	
	initSignals(wc, shots);  
	InitWaveInterface(wc);
	if(wave.wi != null)
	    wave.wi.StartEvaluate();	
    }	    						     	     	     
		        		     
   public void UpdateWave(MultiWaveform _wave, WaveformConf wc, String shots)
   {	
    	wave = _wave;
	initSignals(wc, shots);  
	UpdateWave(wc);
   }	    						     	     	     

   public void RepaintWave(MultiWaveform _wave, WaveformConf wc, String shots)
   {	
    	wave = _wave;
	initSignals(wc, shots);  
	InitWaveInterface(wc);	
	UpdateWaveOther(wave);
    }	    						     	     	     
 }