import java.awt.*;
import java.io.*;

public class WaveInterface
{
    public boolean full_flag;
    public int     num_waves;
    public boolean x_log, y_log;
    public String  in_x[], in_y[], in_up_err[], in_low_err[];
    
    // Prameter used to evaluate waveform
    public String in_xmin, in_xmax, in_ymax, in_ymin;
    public String in_title, in_xlabel, in_ylabel;
    public String in_def_node, in_upd_event, experiment;
    public int    in_grid_mode;
    public int    height; 

    // Configuration parameter
    public String cin_xmin, cin_xmax, cin_ymax, cin_ymin;
    public String cin_title, cin_xlabel, cin_ylabel;
    public String cin_def_node, cin_upd_event, cexperiment; 
     
    public  int     markers_step[];
    public  int     markers[];
    public  Color   colors[];
    public  int     colors_idx[];
    public  boolean interpolates[];
    public  int     shots[];
    public  String  error;
    private String  curr_error;
    public  String  provider;
    
    public Signal signals[];
    public float xmax, xmin, ymax, ymin; 
    public String title, xlabel, ylabel;
    //private DataProvider dp;
    public DataProvider dp;
    
// Used for asynchronous Update  
    public boolean asynch_update = true;  
    Signal wave_signals[];
    float wave_xmin, wave_xmax;
    int wave_timestamp;
    AsynchUpdater du;
    boolean request_pending;
    float orig_xmin, orig_xmax;
    private boolean evaluated[];
    
    public String in_shot, cin_shot;
    public int defaults;
    public int num_shot;
    public boolean modified = true;
    static final int B_shot = 8, B_x_min = 12, B_x_max = 13, B_y_min = 14, B_y_max = 15,
		             B_title = 16, B_x_label = 10, B_y_label = 11, B_exp = 7, B_event = 17,
		             B_default_node = 9; 


    static final float HUGE = (float)1E8;
    static final int MAX_POINTS = 1000;
    
    public WaveInterface(WaveInterface wi)
    {
	full_flag = wi.full_flag;
	provider = wi.provider;
	num_waves = wi.num_waves;
	num_shot  = wi.num_shot;
	defaults  = wi.defaults;
	modified = true;
	in_grid_mode = wi.in_grid_mode;
	x_log = wi.x_log;
	y_log = wi.y_log;
	in_x = new String[num_waves];
	in_y = new String[num_waves];
	in_up_err = new String[num_waves];
	in_low_err = new String[num_waves];
	markers = new int[num_waves];
	markers_step = new int[num_waves];
	colors = new Color[num_waves];
	colors_idx = new int[num_waves];
	interpolates = new boolean[num_waves];
	shots = new int[num_waves];
	
	for(int i = 0; i < num_waves; i++)
	{
	    if(wi.in_x[i] != null)
		in_x[i] = new String(wi.in_x[i]);
	    else
		in_x[i] = null;
	    if(wi.in_y[i] != null)
		in_y[i] = new String(wi.in_y[i]);
	    else
		in_y[i] = null;
	    if(wi.in_up_err[i] != null)
		in_up_err[i] = new String(wi.in_up_err[i]);
	    else
		in_up_err[i] = null;
	    if(wi.in_low_err[i] != null)
		in_low_err[i] = new String(wi.in_low_err[i]);
	    else
		in_low_err[i] = null;
	}
	

	for(int i = 0; i < num_waves; i++)
	{
	    markers[i] = wi.markers[i];
	    markers_step[i] = wi.markers_step[i];
	    colors[i] = wi.colors[i];
	    colors_idx[i] = wi.colors_idx[i];
	    interpolates[i] = wi.interpolates[i];
	    shots[i] = wi.shots[i];
	}

	if(wi.in_xmin != null)
	    in_xmin = new String(wi.in_xmin);
	else
	    in_xmin = null;
	if(wi.in_ymin != null)
	    in_ymin = new String(wi.in_ymin);
	else
	    in_ymin = null;	
	if(wi.in_xmax != null)
	    in_xmax = new String(wi.in_xmax);
	else
	    in_xmax = null;
	if(wi.in_ymax != null)
	    in_ymax = new String(wi.in_ymax);
	else
	    in_ymax = null;
			
	if(wi.in_shot != null)
	    in_shot = new String(wi.in_shot);
	else
	    in_shot = null;
	if(wi.experiment != null)
	    experiment = new String(wi.experiment);
	else
	    experiment = null;
	if(wi.in_title != null)
	    in_title = new String(wi.in_title);
	else
	    in_title = null;
	if(wi.in_xlabel != null)
	    in_xlabel = new String(wi.in_xlabel);
	else
	    in_xlabel = null;
	if(wi.in_ylabel != null)
	    in_ylabel = new String(wi.in_ylabel);
	else
	    in_ylabel = null;	


	if(wi.cin_xmin != null)
	    cin_xmin = new String(wi.cin_xmin);
	else
	    cin_xmin = null;
	if(wi.cin_ymin != null)
	    cin_ymin = new String(wi.cin_ymin);
	else
	    cin_ymin = null;	
	if(wi.cin_xmax != null)
	    cin_xmax = new String(wi.cin_xmax);
	else
	    cin_xmax = null;
	if(wi.cin_ymax != null)
	    cin_ymax = new String(wi.cin_ymax);
	else
	    cin_ymax = null;

	if(wi.cin_shot != null)
	    cin_shot = new String(wi.cin_shot);
	else
	    cin_shot = null;
	if(wi.cexperiment != null)
	    cexperiment = new String(wi.cexperiment);
	else
	    cexperiment = null;
	if(wi.cin_title != null)
	    cin_title = new String(wi.cin_title);
	else
	    cin_title = null;
	if(wi.cin_xlabel != null)
	    cin_xlabel = new String(wi.cin_xlabel);
	else
	    cin_xlabel = null;
	if(wi.cin_ylabel != null)
	    cin_ylabel = new String(wi.cin_ylabel);
	else
	    cin_ylabel = null;	

	if(wi.error != null)
	    error = new String(wi.error);
	else
	    error = null;

	if(wi.cin_upd_event != null)
	    cin_upd_event = new String(wi.cin_upd_event);
	else
	    cin_upd_event = null;

	if(wi.cin_def_node != null)
	    cin_def_node = new String(wi.cin_def_node);
	else
	    cin_def_node = null;
	    	
	error = null;
	dp = wi.dp;
    }	

    
    public WaveInterface(DataProvider _dp)
    {
	full_flag = false;
	dp = _dp;
	experiment = null;
	shots = null;
	in_xmin = in_xmax = in_ymin = in_ymax = in_title = in_xlabel = in_ylabel = null;
	markers = null;
	colors = null;
	interpolates = null;
	du = null;  
	x_log = y_log = false;                                                       
    }
    
    public boolean UseDefaultShot()
    {
	return ((defaults & (1 << B_shot)) != 0);	    
    }
    
    public void SetDataProvider(DataProvider _dp)
    {
	dp = _dp;
    }
    
    public synchronized int StartEvaluate()
    {
 	error = null;
		
	evaluated = null;

	if((in_y == null || in_x == null) || (shots == null && 
				(experiment != null && experiment.trim().length() > 0)) )
	{
	    error = "Missing shot or Y or X values";
	    return 0;
	}    

	num_waves = in_y.length;
	evaluated = new boolean[num_waves];
	signals = new Signal[num_waves];    

	if(in_x != null && num_waves != in_x.length)
	{
	    error = "X values are different from Y values";
	    return 0;
	}
	
	if(shots != null)
	    dp.Update(experiment, shots[0]);
	else
	    dp.Update(null, 0);
	
	if(dp.ErrorString() != null)
	{
	    error = dp.ErrorString();
	    return 0;
	}
//compute limits
	if(in_xmin != null && (in_xmin.trim()).length() != 0)
	{
	    xmin = dp.GetFloat(in_xmin);
	    if(dp.ErrorString() != null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}
	else
	    xmin = -HUGE;
	if(in_xmax != null && (in_xmax.trim()).length() != 0)
	{
	    xmax = dp.GetFloat(in_xmax);
	    if(dp.ErrorString() != null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}
	else
	    xmax = HUGE;
	if(in_ymax != null && (in_ymax.trim()).length() != 0)
	{
	    ymax = dp.GetFloat(in_ymax);
	    if(dp.ErrorString() != null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}
	else
	    ymax = HUGE;
  	if(in_ymin != null && (in_ymin.trim()).length() != 0)
	{
	    ymin = dp.GetFloat(in_ymin);
	    if(dp.ErrorString() != null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}
	else
	    ymin = -HUGE;
	//Compute title, x label, y_label
	if(in_title != null && (in_title.trim()).length() != 0)
	{
	    title = dp.GetString(in_title);
	    if(title == null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	} 
	else
	    title = null;
	if(in_xlabel != null && (in_xlabel.trim()).length() != 0)
	{
	    xlabel = dp.GetString(in_xlabel);
	    if(xlabel == null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}	
	else
	    xlabel = null;
	if(in_ylabel != null && (in_ylabel.trim()).length() != 0)
	{
	    ylabel = dp.GetString(in_ylabel);
	    if(ylabel == null)
	    {
		error = dp.ErrorString();
		return 0;
	    }
	}	
	else
	    ylabel = null;
      
	return 1;
    } 
	     
	
    public synchronized void EvaluateShot(int shot)
    {
	    int curr_wave;

    	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	        if(shots[curr_wave] == shot)
	        {
		        evaluated[curr_wave] = true;
		        signals[curr_wave] = GetSignal(curr_wave, (float)-1E8, (float)1E8);
		        if(signals[curr_wave] == null)
		        {
		            if(error == null)
			            error = curr_error;
		            else
			            error = error + "\n" + curr_error;
		        }
		        else
		        {
		            if(xmin != -HUGE) signals[curr_wave].xmin = xmin;
		            if(xmax !=  HUGE) signals[curr_wave].xmax = xmax;
		            if(ymin != -HUGE) signals[curr_wave].ymin = ymin;
		            if(ymax !=  HUGE) signals[curr_wave].ymax = ymax;	
		        }
	        }    	    
	    }
    }
    
    public synchronized void EvaluateOthers()
    {
	int curr_wave;
        
        if(evaluated == null)
            return;

    	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	    if(!evaluated[curr_wave])
	    {
		evaluated[curr_wave] = true;
		signals[curr_wave] = GetSignal(curr_wave, (float)-1E8, (float)1E8);
		if(signals[curr_wave] == null)
		{
		    if(error == null)
			error = curr_error;
		    else
			error = error + "\n" + curr_error;
		}
		else
		{
		    if(xmin != -HUGE) signals[curr_wave].xmin = xmin;
		    if(xmax !=  HUGE) signals[curr_wave].xmax = xmax;
		    if(ymin != -HUGE) signals[curr_wave].ymin = ymin;
		    if(ymax !=  HUGE) signals[curr_wave].ymax = ymax;	
		}
	    }    	    
	}
    }
    
				    
    private Signal GetSignal(int curr_wave, float xmin, float xmax)
    {			    
		
	String limits = "FLOAT("+new Float(xmin).toString()+"), " +		    		    
	    "FLOAT("+new Float(xmax).toString()+")";
	if(experiment != null && experiment.trim().length() > 0)    			    		    
	    dp.Update(experiment, shots[curr_wave]);
	else
	    dp.Update(null, 0);
	
	if(dp.ErrorString() != null)
	{
	    error = dp.ErrorString();
	    return null;
	}
	
	if(in_y[curr_wave] == null)
	{
	    curr_error = "Missing Y value";
	    return null;
	}
	float curr_y[] = null, curr_x[] = null, up_err[] = null,
	    low_err[] = null, expanded_x[] = null;
	int x_samples = 0;
	
	if(in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0)
	{
//	    if(full_flag)
		curr_y = dp.GetFloatArray(in_y[curr_wave]);
//	    else 
//		curr_y = dp.GetFloatArray("JAVA_RESAMPLE("+ "FLOAT("+in_y[curr_wave]+ "), " +
//		   "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");
	
	    if(curr_y != null && curr_y.length > 1 && in_up_err != null && 
		    in_up_err[curr_wave] != null && (in_up_err[curr_wave].trim()).length() != 0)
	    {
//		if(full_flag)
		    up_err = dp.GetFloatArray(in_up_err[curr_wave]);
//		else
//		    up_err = dp.GetFloatArray("JAVA_RESAMPLE("+ "FLOAT("+in_up_err[curr_wave]+ "), " +
//		      "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");
		if(up_err == null || up_err.length <= 1)
			curr_y = null;			
	    }
	    if(curr_y != null && in_low_err != null && 
		    in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	    {
//		if(full_flag)
		    low_err = dp.GetFloatArray(in_low_err[curr_wave]);
//		else
//		    low_err = dp.GetFloatArray("JAVA_RESAMPLE("+ "FLOAT("+in_low_err[curr_wave]+ "), " +
//		      "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");
		if(low_err == null || low_err.length <= 1)
			curr_y = null;			
	    }
	    if(curr_y != null) 
	    {
//		if(full_flag)
//		{
		    curr_x = dp.GetFloatArray(in_x[curr_wave]);
		    if(curr_x == null || curr_x.length <= 1)
			curr_y = null;
//		}
//		else
//		{
//		    curr_x = dp.GetFloatArray("JAVA_DIM(FLOAT("+ in_x[curr_wave]+ "), " +
//			limits + ")"); 
//		    if(curr_x == null || curr_x.length <= 1)
//			curr_y = null;
//		    else
//		    {
//			expanded_x = new float[curr_y.length];
//			x_samples = ExpandTimes(curr_x, expanded_x);
//			curr_x = expanded_x;
//		    }
//		}
	    }
	}
	else // Campo X non definito
	{
	    if(full_flag)
		curr_y = dp.GetFloatArray(in_y[curr_wave]);
	    else
		curr_y = dp.GetFloatArray("JavaResample("+ "FLOAT("+in_y[curr_wave]+ "), "+
		    "FLOAT(DIM_OF("+in_y[curr_wave]+")), "+ limits + ")");
	    if(curr_y != null && curr_y.length > 1 && in_up_err != null && in_up_err[curr_wave] != null 
		    && (in_up_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    up_err = dp.GetFloatArray(in_up_err[curr_wave]);
		else
		    up_err = dp.GetFloatArray("JavaResample(FLOAT("+ in_up_err[curr_wave]+ "), DIM_OF(FLOAT("+
			in_y[curr_wave] + ")), "+limits +")");
		if(up_err == null || up_err.length <= 1)
		    curr_y = null;
	    
	    }				
	    if(curr_y != null && in_low_err != null && 
		    in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    low_err = dp.GetFloatArray(in_low_err[curr_wave]);
		else
		    low_err = dp.GetFloatArray("JavaResample(FLOAT("+in_low_err[curr_wave] +
			 "), DIM_OF(FLOAT("+ in_y[curr_wave] + ")),"+ limits + ")");
			
		if(low_err == null || low_err.length <= 1)
		    curr_y = null;
	    }				
		
	    if(curr_y != null)
	    {
		if(full_flag)
		    curr_x = dp.GetFloatArray("DIM_OF("+in_y[curr_wave]+")");
		else
		{
		    curr_x = dp.GetFloatArray("JavaDim(FLOAT(DIM_OF("+ in_y[curr_wave]+ 
			")), "+ limits + ")"); 
		    if(curr_x != null && curr_x.length > 1)
		    {
			expanded_x = new float[curr_y.length];
			x_samples = ExpandTimes(curr_x, expanded_x);
			curr_x = expanded_x;
		    }
		}
		if(curr_x == null)
		    curr_y = null;
	    }
	}
	if(curr_x == null || curr_y == null)
	{
	    curr_error = dp.ErrorString();
	    return null;
	}
	int min_len;
	// Se e' definito il campo x si assume full_flag true
	if(full_flag || (in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0))
	    min_len  = curr_y.length;
	else
	{ 
	    if(curr_y.length < x_samples)
		min_len = curr_y.length;
	    else
		min_len = x_samples;
	}	
	
    	Signal out_signal = new Signal(curr_x, curr_y, min_len);
	if(xmin != -HUGE)
	    out_signal.xmin = out_signal.saved_xmin = xmin;
	if(xmax != HUGE)
	    out_signal.xmax = out_signal.saved_xmax = xmax;    
    
	if(in_ymax != null && (in_ymax.trim()).length() != 0)
	    out_signal.ymax = out_signal.saved_ymax = ymax;
	if(in_ymin != null && (in_ymin.trim()).length() != 0)
	    out_signal.ymin = out_signal.saved_ymin = ymin;
	out_signal.up_error = up_err;
        out_signal.low_error = low_err;
        if(up_err != null)
    	out_signal.error = true;
        if(up_err != null && low_err != null)
    	out_signal.asym_error = true;
        return out_signal;
    }
    
    
    private int ExpandTimes(float coded_time[], float expanded_time[])
    {
	int max_len = expanded_time.length;
	int num_blocks = (coded_time.length-1) / 3;
//each block codes start, end, delta
	int out_idx, in_idx, curr_block;
	float curr_time;
	
	if(coded_time[0] > 0) //JAVA$DIM decided to apply coding
	{
	    for(curr_block = 0, out_idx = 0; out_idx < max_len && curr_block < num_blocks; 
		curr_block++)
	    {   
//		for(in_idx = 0; out_idx < max_len && 
//		 (curr_time = coded_time[curr_block*3+1] + 
//		    in_idx * coded_time[curr_block*3+3]) <= coded_time[curr_block*3+2];
//		in_idx++)
//		    expanded_time[out_idx++] = curr_time;
		for(in_idx = 0; out_idx < max_len; in_idx++)
		{ 
		    curr_time = coded_time[curr_block*3+1] + 
			in_idx * coded_time[curr_block*3+3];
		    if(curr_time > coded_time[curr_block*3+2])
			break;
		    expanded_time[out_idx++] = curr_time;
		}
		    
		    
	    }
//System.out.println("End Expanding");	

	}
	else //JAVA$DIM did not apply coding
	    for(out_idx = 0; out_idx < max_len && out_idx < coded_time.length-1; out_idx++)
		expanded_time[out_idx] = coded_time[out_idx+1];
		
	
    	return out_idx;
    }


    void AsynchUpdate(Signal sigs[], float xmin, float xmax, 
	float _orig_xmin, float _orig_xmax, int timestamp, boolean panning, MultiWaveform w)
    {
	int curr_wave;
	boolean needs_update = false;
	if(full_flag)
	    return;
	wave_signals = sigs;
	wave_xmin = xmin;
	wave_xmax = xmax;
	wave_timestamp = timestamp;
	request_pending = true;
	orig_xmin = _orig_xmin;
	orig_xmax = _orig_xmax;
	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	{
	    if(wave_signals[curr_wave] == null) continue;
	    if (wave_signals[curr_wave].n_points >= MAX_POINTS - 5 || (panning && 
		(orig_xmin < xmin || orig_xmax > xmax)))
		needs_update = true;
	}
	if(needs_update && dp.SupportsAsynch())
	{
	    if(asynch_update)
	    {
		if(du == null)
		{
		    du = new AsynchUpdater(this, w);
		    du.start();
		}
		else 
		    du.resume();
	    }
	    else
		DynamicUpdate(w);    
	}
    }

    public synchronized void DynamicUpdate(MultiWaveform w)
    {
	    boolean needs_update = false;
	    int curr_wave, saved_timestamp = wave_timestamp;
	    for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	//   if (wave_signals[curr_wave].n_points >= MAX_POINTS - 1)
	    //if (wave_signals[curr_wave].n_points >= MAX_POINTS - 1 || 
		//orig_xmin < wave_xmin || orig_xmax > wave_xmax)
	   {
		needs_update = true;
		wave_signals[curr_wave] = GetSignal(curr_wave, wave_xmin, wave_xmax);
	    }
	}
	if(needs_update && saved_timestamp == wave_timestamp)
	    w.UpdateSignals(wave_signals, wave_signals.length, wave_timestamp);
    }

    private String addNewLineCode(String s)
    {
	    String  s_new = new String();
	    int     new_pos = 0, old_pos = 0;
	
	    if(s == null) return null;
	    while((new_pos = s.indexOf("\n", old_pos)) != -1)
	    {
	        s_new = s_new.concat(s.substring(old_pos, new_pos));
	        old_pos = new_pos + "\n".length();			  
	        s_new += "|||";
	    }
	    s_new = s_new.concat(s.substring(old_pos, s.length()));
	    return s_new;
    }
 

    public void toFile(BufferedWriter out, String prompt, jScope main_scope)
    { 
	    int exp, exp_n, sht, sht_n, cnum_shot; 

 	    cnum_shot = num_shot;
	    if(UseDefaultShot())
	    {
	        if(cin_shot != null && cin_shot.length()> 0)
		        cnum_shot = (main_scope.evaluateShot(cin_shot)).length;
	        else
		        cnum_shot = 1;
	    }
 
	    jScope.writeLine(out, prompt + "x_label: "         , cin_xlabel);
	    jScope.writeLine(out, prompt + "y_label: "         , cin_ylabel);
	    jScope.writeLine(out, prompt + "x_log: "           , ""+x_log);
	    jScope.writeLine(out, prompt + "y_log: "           , ""+y_log);
	    jScope.writeLine(out, prompt + "experiment: "      , cexperiment);
	    jScope.writeLine(out, prompt + "event: "           , cin_upd_event);
	    jScope.writeLine(out, prompt + "default_node: "    , cin_def_node);

	    if(cnum_shot != 0)
	        jScope.writeLine(out, prompt + "num_expr: "        , ""+num_waves/cnum_shot);
	    else
	        jScope.writeLine(out, prompt + "num_expr: "        , ""+num_waves);
	
	    jScope.writeLine(out, prompt + "num_shot: "        , ""+cnum_shot);
	    jScope.writeLine(out, prompt + "shot: "            , cin_shot);
	    jScope.writeLine(out, prompt + "ymin: "            , cin_ymin);
	    jScope.writeLine(out, prompt + "ymax: "            , cin_ymax);
	    jScope.writeLine(out, prompt + "xmin: "            , cin_xmin);
	    jScope.writeLine(out, prompt + "xmax: "            , cin_xmax);
	    jScope.writeLine(out, prompt + "title: "           , cin_title);
	    jScope.writeLine(out, prompt + "global_defaults: " , ""+defaults);
	
		    
	    for(exp = 0, exp_n = 1; exp < num_waves; exp += num_shot, exp_n++)
	    {
	        jScope.writeLine(out, prompt + "x_expr"     + "_" + exp_n + ": " , addNewLineCode(in_x[exp]));
	        jScope.writeLine(out, prompt + "y_expr"     + "_" + exp_n + ": " , addNewLineCode(in_y[exp]));
	        jScope.writeLine(out, prompt + "up_error"   + "_" + exp_n + ": " , in_up_err[exp]);
	        jScope.writeLine(out, prompt + "low_error"  + "_" + exp_n + ": " , in_low_err[exp]);
			
	        for(sht = 0, sht_n = 1; sht < cnum_shot; sht++, sht_n++)
	        {			    
		        jScope.writeLine(out, prompt + "interpolate"+"_"+exp_n+"_"+sht_n+": ",""+interpolates[exp + sht]);								    
		        jScope.writeLine(out, prompt + "color"      +"_"+exp_n+"_"+sht_n+": ",""+colors_idx[exp + sht]);
		        jScope.writeLine(out, prompt + "marker"     +"_"+exp_n+"_"+sht_n+": ",""+markers[exp + sht]);
		        jScope.writeLine(out, prompt + "step_marker"+"_"+exp_n+"_"+sht_n+": ",""+markers_step[exp + sht]);
	        }			    			    
	    } 
    }
    
    private String trimString(String s)
    {
	String  s_new = new String();
	int     new_pos = 0, old_pos = 0;
	while((new_pos = s.indexOf(" ", old_pos)) != -1)
	{
	    s_new = s_new.concat(s.substring(old_pos, new_pos));
	    old_pos = new_pos + " ".length();			  
	}
	s_new = s_new.concat(s.substring(old_pos, s.length()));
	return s_new;
    }

    private String removeNewLineCode(String s)
    {
	String  y_new = new String();
	int     new_pos = 0, old_pos = 0;
	
	s = trimString(s);
	while((new_pos = s.indexOf("|||", old_pos)) != -1)
	{
	    y_new = y_new.concat(s.substring(old_pos, new_pos));
	    old_pos = new_pos + "|||".length();			  
	    y_new += '\n';
	}
	y_new = y_new.concat(s.substring(old_pos, s.length()));
	return y_new;
    }
    
    private void createVector()
    {
	shots	 = new int[num_waves];
	in_y	 = new String[num_waves];
	in_x	 = new String[num_waves];
	in_up_err    = new String[num_waves];
	in_low_err   = new String[num_waves];
	markers      = new int[num_waves];
	markers_step = new int[num_waves];
	colors_idx   = new int[num_waves];
	colors       = new Color[num_waves];
	interpolates = new boolean[num_waves];
	
	for(int i = 0; i < num_waves; i++)
	{
	    shots[i]	 = jScope.UNDEF_SHOT;
	    markers[i]      = 0;
	    markers_step[i] = 1;
    	    colors_idx[i]   = i % 12;
	    interpolates[i] = true;
	}
    }
    

    public int fromFile(BufferedReader in, String prompt, jScope main_scope) throws IOException
    { 
	    String str;
	    int error = 0, len, pos, num_expr = 0;
	    boolean shot_evaluated = false;
	    int sh[] = null;
     
	    while((str = in.readLine()) != null) 
	    {
	        if(str.indexOf(prompt) == 0)
	        {
		        len =  str.indexOf(":") + 2;

		        if(str.indexOf(".height:") != -1)
		        {
		            height = new Integer(str.substring(len, str.length())).intValue();
		            continue;		
		        }

		
		        if(str.indexOf(".grid_mode:") != -1)
		        {
		            in_grid_mode = new Integer(str.substring(len, str.length())).intValue();
		            continue;		
		        }
		
		        if(str.indexOf(".x_label:") != -1)
		        {
		            cin_xlabel = str.substring(len, str.length());
		            continue;		
		        }
		        if(str.indexOf(".y_label:") != -1)
		        {
		            cin_ylabel = str.substring(len, str.length());
		            continue;		
		        }
		        if(str.indexOf(".x_log:") != -1)
		        {
		            x_log = new Boolean(str.substring(len, str.length())).booleanValue();
		            continue;		
		        }
		        if(str.indexOf(".y_log:") != -1)
		        {
		            y_log = new Boolean(str.substring(len, str.length())).booleanValue();
		            continue;		
		        }		    
		        if(str.indexOf(".experiment:") != -1)
		        {
		            cexperiment = str.substring(len, str.length());
		            continue;		
		        }
		        if(str.indexOf(".shot:") != -1)
		        {
		            cin_shot = str.substring(len, str.length());
		            sh = main_scope.evaluateShot(cin_shot);
		    
		            if(sh.length == num_shot)
		            {
			            for(int i = 0, k = 0; i < num_expr; i++)
			                for(int j = 0; j < num_shot; j++)
			                {
				                shots[k] = sh[j];
				                k++;
			                }
		            } else
			            shot_evaluated = true;
		            continue;		
		        }
		        if(str.indexOf(".x:") != -1)
		        {
		            String x_str = str.substring(len, str.length());
				
		            x_str = removeNewLineCode(x_str);
		            if(in_x == null || in_x.length == 0)
			            in_x = new String[1];
		            in_x[0] = x_str;
		            continue;		
		        }
		        if(str.indexOf(".y:") != -1)
		        {
		            num_shot = 1;
		            String y_str = str.substring(len, str.length());
		            String x_str = null;

		            y_str = removeNewLineCode(y_str);
		    
		            if(in_x != null && in_x.length != 0)
			            x_str = in_x[0];
																	    
		            if(y_str.indexOf("[") == 0 && y_str.toLowerCase().indexOf("$roprand") != -1)
		            {
		
			            pos = y_str.toLowerCase().indexOf(",$roprand");
			            do {
			                num_expr++;
			                pos += ",$roprand".length();
			            } while((pos = y_str.toLowerCase().indexOf(",$roprand", pos)) != -1 );
			            num_expr++;
		            } else {
			            num_expr = 1;
		            }
		    
		            num_waves = num_expr * num_shot;
		            createVector();

		            if(num_expr == 1) {
			            in_y[0] = y_str;
			            if( in_y[0].toLowerCase().indexOf("multitrace") != -1)
			            {
			                in_y[0] =  in_y[0].substring(in_y[0].indexOf("\\") + 1, in_y[0].indexOf("\")"));  
			            }
			            if(x_str != null)
			                in_x[0] = new String(x_str);
			            else
			                in_x[0] = null;
		    
		                } else {
		                    int pos_x = (x_str != null) ? x_str.indexOf("[") : 0;
		                    int pos_y = y_str.indexOf("[");			  
		                    int i, j;			
			                for(i = 0; i < (num_expr - 1) * num_shot; i += num_shot)
			                {
			                    in_y[i] = y_str.substring(pos_y + 1, pos_y = y_str.toLowerCase().indexOf(",$roprand", pos_y));
			                    if(x_str != null) {
				                    if(x_str.indexOf(",0", pos_x) != -1)
				                    in_x[i] = x_str.substring(pos_x  + 1, pos_x =  x_str.indexOf(",0", pos_x));			    			
			                    }
			                    if(in_y[i].toLowerCase().indexOf("multitrace") != -1)
			                    {
				                    in_y[i] =  in_y[i].substring(in_y[i].indexOf("\\") + 1, in_y[i].indexOf("\")"));  
			                    }
			                    for(j = 1; j < num_shot; j++)
			                    {
				                    in_y[i+j] = new String(in_y[i]);
				                    in_x[i+j] = new String(in_x[i]);				
			                    }
			                    pos_y += ",$roprand".length();
			                    pos_x += ",0".length();
			                }
			                in_y[i] = y_str.substring(pos_y + 1,  y_str.indexOf("]", pos_y));
			                if(x_str != null)
			                    in_x[i] = x_str.substring(pos_x + 1, x_str.indexOf("]", pos_x));			    			
			                for(j = 1; j < num_shot; j++)
			                {
			                    in_y[i+j] = new String(in_y[i]);
			                    in_x[i+j] = new String(in_x[i]);				
			                }
		                }    
		                continue;		
		            }
		            if(str.indexOf(".num_expr:") != -1)
		            {
		                num_expr = new Integer(str.substring(len, str.length())).intValue();
		                num_expr = (num_expr > 0) ? num_expr : 1;						
		                if(num_shot != 0) {
			                num_waves = num_expr * num_shot;
			                createVector();
			                if(shot_evaluated)
			                {
			                    if(sh.length == num_shot)
			                    {
				                    for(int i = 0, k = 0; i < num_expr; i++)
				                    for(int j = 0; j < num_shot; j++)
				                    {
					                    shots[k] = sh[j];
					                    k++;
				                    }
			                    } 			    
			                }
		                 }
		                 continue;		
		            }		    
		            if(str.indexOf(".num_shot:") != -1)
		            {
		
		                num_shot = new Integer(str.substring(len, str.length())).intValue();
		                num_shot = (num_shot > 0) ? num_shot : 1;			
		                if(num_expr != 0) {
			                num_waves = num_expr * num_shot;
			                createVector();
			                if(shot_evaluated)
			                {
			                    if(sh.length == num_shot)
			                    {
				                    for(int i = 0, k = 0; i < num_expr; i++)
				                        for(int j = 0; j < num_shot; j++)
				                        {
					                        shots[k] = sh[j];
					                        k++;
				                        }
			                    } 			    
			                }
		                }
		                continue;		
		            }
		            if(str.indexOf(".global_defaults:") != -1)
		            {
		                defaults = new Integer(str.substring(len, str.length())).intValue();
		                continue;		
		            }		    
		            if((pos = str.indexOf(".x_expr_")) != -1)
		            {			
		                pos += ".x_expr_".length();
		                int expr_idx = (new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1) * num_shot;
		                in_x[expr_idx] = removeNewLineCode(str.substring(pos + 2, str.length()));
		                for(int j = 1; j < num_shot; j++)
			                in_x[expr_idx + j] = in_x[expr_idx];			
		                continue;		
		            }		    
		            if((pos = str.indexOf(".y_expr_")) != -1)
		            {			
		                pos += ".y_expr_".length();
		                int expr_idx = (new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1) * num_shot;
		                in_y[expr_idx] = removeNewLineCode(str.substring(pos + 2, str.length()));
		                for(int j = 1; j < num_shot; j++)
			            in_y[expr_idx + j] = in_y[expr_idx];			
		                continue;		
		            }		    
		            if((pos = str.indexOf(".up_error_")) != -1)
		            {			
		                pos += ".up_error_".length();
		                int expr_idx = (new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1) * num_shot;
		                in_up_err[expr_idx] = str.substring(pos + 2, str.length());
		                for(int j = 1; j < num_shot; j++)
			            in_up_err[expr_idx + j] = in_up_err[expr_idx];			
		                continue;		
		            }		    
		            if((pos = str.indexOf(".low_error_")) != -1)
		            {			
		                pos += ".low_error_".length();
		                int expr_idx = (new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1) * num_shot;
		                in_low_err[expr_idx] = str.substring(pos + 2, str.length());
		                for(int j = 1; j < num_shot; j++)
			                in_low_err[expr_idx + j] = in_low_err[expr_idx];			
		                continue;		
		            }
		
		            if((pos = str.indexOf(".interpolate_")) != -1)
		            {			
		                pos += ".interpolate_".length();
		                int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
		                int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
		                interpolates[expr_idx *  num_shot + shot_idx] = 
						new Boolean(str.substring(pos + 2, str.length())).booleanValue();
		                continue;		
		            }
		            if((pos = str.indexOf(".color_")) != -1)
		            {
		                pos += ".color_".length();
		                int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
		                int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
		                try {			
		                    int c_idx = new Integer(str.substring(pos + 2, str.length())).intValue();
		                    colors_idx[expr_idx *  num_shot + shot_idx] = c_idx;
		                } catch(Exception e) { colors_idx[expr_idx *  num_shot + shot_idx] = 0;}
		                continue;		
		            }
		            if((pos = str.indexOf(".marker_")) != -1)
		            {			
		                pos += ".marker_".length();
		                int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
		                int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
		                markers[expr_idx *  num_shot + shot_idx] = 
						new Integer(str.substring(pos + 2, str.length())).intValue();
		                continue;		
		            }
		
		            if((pos = str.indexOf(".step_marker_")) != -1)
		            {			
		                pos += ".step_marker_".length();
		                int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
		                int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
		                markers_step[expr_idx *  num_shot + shot_idx] = 
						new Integer(str.substring(pos + 2, str.length())).intValue();
		                continue;		
		            }
						
		            if(str.indexOf(".title:") != -1)
		            {
		                cin_title = str.substring(len, str.length());
		                continue;		
		            }
		            if(str.indexOf(".ymin:") != -1)
		            {
		                cin_ymin = str.substring(len, str.length());
		                continue;		
		            }
		            if(str.indexOf(".ymax:") != -1)
		            {
		                cin_ymax = str.substring(len, str.length());
		                continue;		
		            }
		            if(str.indexOf(".xmin:") != -1)
		            {
		                cin_xmin = str.substring(len, str.length());
		                continue;		
		            }
		            if(str.indexOf(".xmax:") != -1)
		            {
		                cin_xmax = str.substring(len, str.length());
		                continue;
		            }
		            if(str.indexOf(".default_node:") != -1)
		            {
		                cin_def_node = str.substring(len, str.length());
		                continue;		
		            }
		            if(str.indexOf(".event:") != -1)
		            {
		                cin_upd_event = str.substring(len, str.length());
		                continue;		
		            }
	            }
	        }
		
	        return error;
        }	 
	      	        
    }		
	
class AsynchUpdater  extends Thread
{
    WaveInterface wi;
    MultiWaveform w;
    
public AsynchUpdater(WaveInterface _wi, MultiWaveform _w)
{
    wi = _wi;
    w = _w;
}
public void run()
{
    while(true)
    {
	while(wi.request_pending)
	{
	    wi.request_pending = false;
	    wi.DynamicUpdate(w);
	}
	suspend();
    }
}
    	

}   
				    				
			