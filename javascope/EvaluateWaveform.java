import java.io.*;
import java.awt.*;

//******
//Classe utilizzata per aggiornare la wave selezionate
//****** 

 class EvaluateWaveform {
   javaScope        main_scp;
   MultiWaveform    wave;
   WaveSetupData    signals[];   
   int		    shots[];
   int		    num_shot, num_signal;
   ErrorMessage	    error_msg;

   EvaluateWaveform(javaScope _main_scp)
   {
      main_scp = _main_scp;
      error_msg = new ErrorMessage(main_scp);
   }
      
   public void setMainScope(javaScope _main_scp)
   {
      main_scp = _main_scp;
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
      
	signals[idx].color       = ws.color;
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

   public int[] evaluateShot(String in_shots)
   {
	int[]  s_value = new int[1];
	int  s_range, s_curr; 
	int error_cd = 0;
	byte[] shots_str;	
	byte[] shot_str = new byte[8];
    	int i = 0, k = 0, j = 0;
	boolean interval_flag = false;
	
	if(in_shots == null || in_shots.length() == 0)
	{
	    s_value[0] = WaveSetupData.UNDEF_SHOT;
	    return s_value;
	}    
	shots_str =  new byte[in_shots.length() + 1];	
	in_shots = in_shots.concat(new String("F"));    
    	shots_str = in_shots.getBytes();	
	
	for(i = 0, k = 0;  i < in_shots.length() && error_cd == 0 && k < 8; i++)
       	{
	    if(j > WaveSetupData.MAX_NUM_SHOT)
	    {
		error_cd = 5;
		break;
	    }
	    if(k ==  0 && shots_str[i] == ' ')
		continue;
			    	
	    if(k !=  0 && (shots_str[i] == ' ' || shots_str[i] == '-') ) {
		error_cd = 4; //Carattere non ammesso
		break;	
	    }
	    
    	    if((shots_str[i] >= '0' && shots_str[i] <= '9') || shots_str[i] == '-')
		shot_str[k++] = shots_str[i];
	    else {
		if(shots_str[i] == ',' || shots_str[i] == 'F') {
		    if(interval_flag) {
			s_range = (new Integer(new String(shot_str, 0, k))).intValue();
			if(s_value[j-1] < s_range) {
			    for(s_curr = s_value[j-1]+1; s_curr <= s_range && j < WaveSetupData.MAX_NUM_SHOT; s_curr++)
				s_value = addNewShot(s_curr, j++, s_value);
			    interval_flag = false;
			    k = 0;
    			} else
			    error_cd = 3; //Intervallo shot errato	
    		   } else {
		     s_value = addNewShot((new Integer(new String(shot_str, 0, k))).intValue(), j++, s_value);
		     k = 0;
		   }  
		} else {
		    if(shots_str[i] == ':' && !interval_flag) {			
			s_value = addNewShot((new Integer(new String(shot_str, 0, k))).intValue(), j++, s_value);
			k = 0;
		        interval_flag = true;
		    } else {
			if(interval_flag)
			    error_cd = 1; //Definizione intervallo errata
			else
			    error_cd = 2; //Carattere non ammesso 
    		    }
    	    	} 
	    }	  	
    	}
	if(error_cd != 0) {
	    error_msg.setMessage("Shots syntax error \n Max number of shots is 10 \n Syntax shots interval: start:end \n shots list: shot1, shot2, etc..\n");
	    error_msg.showMessage();
    	}
    	return (s_value); 
   } 

   public void signalsRefresh(String in_shot, String in_main_shots, boolean use_main_shot)
   {
    int new_num_shot, new_num_signal = 0, sig_idx, new_main_shot, 
	new_shots[], main_shots[];
    WaveSetupData[] new_sigs;
    int color_idx = 0;
	
	new_shots  = evaluateShot(in_shot);
	if(new_shots != null)
	    new_num_shot = new_shots.length;
	else
	    new_num_shot = 0;
	    
	if(use_main_shot)
	    num_shot = new_num_shot;
	    
	for(int i = 0; i < new_num_shot; i++)
	    if(new_shots[i] < 10 && new_shots[i] > -10 && new_shots[i] != 0) {
		use_main_shot = false;
		break;
	    }
	    	    	    
	main_shots = evaluateShot(in_main_shots);
	if(main_shots[0] != WaveSetupData.UNDEF_SHOT)
	    new_main_shot = main_shots.length;
	else
	    new_main_shot = 0;
	    
	if(new_main_shot != 0 && use_main_shot)
	{
//	    for(int i = 0; i < new_main_shot; i++)
//		new_shots[i] = main_shots[i];
	    new_shots    = main_shots;
    	    new_num_shot = new_main_shot;
	}
		
	for(int i = 0; i < new_num_shot; i++)
	    if(new_shots[i] < 10 && new_shots[i] > -10 && new_main_shot != 0)
		new_shots[i] += main_shots[0];

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
		    color_idx = (color_idx + 1) % WaveSetupData.MAX_COLOR;
		    new_sigs[new_num_signal].color = WaveSetupData.COLOR_SET[color_idx];
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
	if(new_num_shot != 0)
	{    
	    num_shot   = new_num_shot;
	    shots      = new_shots;
	} else {
	    num_shot   = 1;
	    shots      = new int[1];
	    shots[0]   = WaveSetupData.UNDEF_SHOT;
	}
   } 
   
   public void initSignals(WaveformConf wc, String curr_shot)   
   { 
	if(wc == null) 	    
	    return;

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
		if(wc.colors.length > k)
		{
		    signals[k].interpolate = wc.interpolates[k];
		    signals[k].marker      = wc.markers[k];
		    if(wc.colors[k] == null)
			signals[k].color = wc.colors[k] = WaveSetupData.COLOR_SET[k%WaveSetupData.MAX_COLOR];
		    else
			signals[k].color = wc.colors[k];		    
		} else {
		    signals[k].interpolate = true;
		    signals[k].marker = 0;
		    signals[k].color = WaveSetupData.COLOR_SET[k%WaveSetupData.MAX_COLOR];
		}
		k++;
	    }
	}  
    }
   
   public void initSignals(WaveInterface wi)   
   {
	if(wi == null) 	    
	    return;    
	num_signal = wi.num_waves;
	signals = new WaveSetupData[num_signal];
	for(int i=0; i < num_signal; i++)
	{
	    signals[i]             = new WaveSetupData();
	    signals[i].x_expr      = wi.in_x[i];
	    signals[i].y_expr      = wi.in_y[i];
	    signals[i].shot        = wi.shots[i];
	    signals[i].interpolate = wi.interpolates[i];
	    signals[i].marker      = wi.markers[i];
	    signals[i].up_err      = wi.in_up_err[i];
	    signals[i].low_err     = wi.in_low_err[i];
	    signals[i].color       = wi.colors[i];
	}    	     	      
   }
   
   public void UpdateWave(WaveformConf wc)
   {
   
      if(num_signal == 0)
	return;
		
      WaveInterface wi = wave.GetWaveInterface();
 
      if(wi == null)
	 wi = new WaveInterface(main_scp.db);
	 
	wi.num_waves    = num_signal;
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
	wi.full_flag	= !wc.opt_network;
     
    	wi.in_x         = new String[num_signal];
	wi.in_y         = new String[num_signal];
	wi.in_up_err    = new String[num_signal];
	wi.in_low_err   = new String[num_signal];
	wi.markers      = new int[num_signal];
	wi.colors       = new Color[num_signal];
	wi.interpolates = new boolean[num_signal];
	wi.shots        = new int[num_signal];				
				      	
	for(int i = 0; i < num_signal; i++)      
	{
	  if(signals[i].x_expr != null)
	     wi.in_x[i]         = new String(signals[i].x_expr);
	  if(signals[i].y_expr != null)	     
	    wi.in_y[i]         = new String(signals[i].y_expr);
	  wi.markers[i]      = signals[i].marker;
	  wi.interpolates[i] = signals[i].interpolate;
	  wi.shots[i]        = signals[i].shot;
	  if(signals[i].up_err != null)
	    wi.in_up_err[i] = new String(signals[i].up_err);
	  if(signals[i].low_err != null)      
    	    wi.in_low_err[i] = new String(signals[i].low_err);	    
	  wi.colors[i]       = signals[i].color;
	}
      
	main_scp.setCursor(new Cursor(Cursor.WAIT_CURSOR));
	wi.Evaluate();
	main_scp.setCursor(Cursor.getDefaultCursor());
	wave.y_log       = wc.y_log;
	wave.x_log       = wc.x_log;
	wave.Update(wi);
	wave.SetMode(main_scp.wave_mode);
	if(wi.error != null)
	{
	    error_msg.addMessage(wi.error);
	}	
   }
   		     
   public void UpdateWave(MultiWaveform _wave, WaveformConf wc, String main_shots)
   {	
    	wave = _wave;
	initSignals(wc, wc.shot_str);    
	signalsRefresh(wc.shot_str, main_shots, true);
	UpdateWave(wc);
     }	    						     	     	     
 }
