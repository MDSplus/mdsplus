import java.awt.*;

public class WaveInterface
{
    public boolean full_flag;
    public int num_waves;
    public boolean x_log, y_log;
    public String  in_x[], in_y[], in_up_err[], in_low_err[];
    public String in_xmin, in_xmax, in_ymax, in_ymin;
    public int   markers[];
    public Color colors[];
    public boolean interpolates[];
    public String experiment; 
    public String in_title, in_xlabel, in_ylabel; 
    public int shots[];
    public String error;
    public String provider;
    
    public Signal signals[];
    public double xmax, xmin, ymax, ymin; 
    public String title, xlabel, ylabel;
    private DataProvider dp;
// Used for asynchronous Update  
    public boolean asynch_update = true;  
    Signal wave_signals[];
    double wave_xmin, wave_xmax;
    int wave_timestamp;
    AsynchUpdater du;
    boolean request_pending;
    double orig_xmin, orig_xmax;
    

    static final double HUGE = 1E8;
    static final int MAX_POINTS = 1000;
    
    public WaveInterface(WaveInterface wi)
    {
	full_flag = false;
	provider = wi.provider;
	num_waves = wi.num_waves;
	x_log = wi.x_log;
	y_log = wi.y_log;
	in_x = new String[num_waves];
	in_y = new String[num_waves];
	in_up_err = new String[num_waves];
	in_low_err = new String[num_waves];
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
	if(wi.in_xmin != null)
	    in_xmin = new String(wi.in_xmin);
	else
	    in_xmin = null;
	if(wi.in_ymin != null)
	    in_ymin = new String(wi.in_ymin);
	else
	    in_ymin = null;

        markers = wi.markers;
	colors = wi.colors;
	interpolates = wi.interpolates;
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

        shots = wi.shots;
	error = null;
	dp = wi.dp;
    }	
    /*public WaveInterface()
    {
	full_flag = false;
	provider = null;
	dp = new DataProvider();
	experiment = null;
	shots = null;
	in_xmin = in_xmax = in_ymin = in_ymax = in_title = in_xlabel = in_ylabel = null;
	markers = null;
	colors = null;
	interpolates = null;
	du = null;  
	x_log = y_log = false;                                                       
    }
    public WaveInterface(String _provider)
    {
	full_flag = false;
	provider = _provider;
	dp = new DataProvider(provider);
	experiment = null;
	shots = null;
	in_xmin = in_xmax = in_ymin = in_ymax = in_title = in_xlabel = in_ylabel = null;
	markers = null;
	colors = null;
	interpolates = null;
	du = null;  
	x_log = y_log = false;                                                       
    }
    */
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
    
    public synchronized int Evaluate()
    {
    
//System.out.println("Start Evaluate");    
	int curr_wave, x_samples = 0;
	double up_err[] = null, low_err[] = null;
	double expanded_x[];
	error = null;
	if(in_y == null || in_x == null || shots == null)
	{
	    error = "Missing shot or Y or X values";
	    return 0;
	}    
	num_waves = in_y.length;
	if(in_x != null && num_waves != in_x.length)
	{
	    error = "X values are different from Y values";
	    return 0;
	}
	        
	dp.Update(experiment, shots[0]);
//compute limits
	if(in_xmin != null && (in_xmin.trim()).length() != 0)
	{
	    xmin = dp.GetDouble(in_xmin);
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
	    xmax = dp.GetDouble(in_xmax);
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
	    ymax = dp.GetDouble(in_ymax);
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
	    ymin = dp.GetDouble(in_ymin);
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

 // Compute signals
// System.out.println("Creo Signal" + num_waves);
	signals = new Signal[num_waves];    
	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	{
// System.out.println("Leggo Signal");
	
	    signals[curr_wave] = GetSignal(curr_wave, -1E8, 1E8);
// System.out.println("Signal letti");
	    
	    if(signals[curr_wave] == null)
		return 0;
	    if(xmin != -HUGE) signals[curr_wave].xmin = xmin;
	    if(xmax !=  HUGE) signals[curr_wave].xmax = xmax;
	    if(ymin != -HUGE) signals[curr_wave].ymin = ymin;
	    if(ymax !=  HUGE) signals[curr_wave].ymax = ymax;	    	    
	}
	return 1;
    }
	
    
	    
		    
    private Signal GetSignal(int curr_wave, double xmin, double xmax)
    {			    
		
	String limits = "FLOAT("+new Double(xmin).toString()+"), " +		    		    
	    "FLOAT("+new Double(xmax).toString()+")";			    		    
	dp.Update(experiment, shots[curr_wave]);
	if(in_y[curr_wave] == null)
	{
	    error = "Missing Y value";
	    return null;
	}
	double curr_y[] = null, curr_x[] = null, up_err[] = null,
	    low_err[] = null, expanded_x[] = null;
	int x_samples = 0;
	
	if(in_x[curr_wave] != null && (in_x[curr_wave].trim()).length() != 0)
	{
	    if(full_flag)
		curr_y = dp.GetDoubleArray(in_y[curr_wave]);
	    else
		curr_y = dp.GetDoubleArray("JAVA_RESAMPLE("+ "FLOAT("+in_y[curr_wave]+ "), " +
		   "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");	
	    if(curr_y != null && curr_y.length > 1 && in_up_err != null && 
		    in_up_err[curr_wave] != null && (in_up_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    up_err = dp.GetDoubleArray(in_up_err[curr_wave]);
		else
		    up_err = dp.GetDoubleArray("JAVA_RESAMPLE("+ "FLOAT("+in_up_err[curr_wave]+ "), " +
		      "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");
		if(up_err == null || up_err.length <= 1)
			curr_y = null;			
	    }
	    if(curr_y != null && in_low_err != null && 
		    in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    low_err = dp.GetDoubleArray(in_low_err[curr_wave]);
		else
		    low_err = dp.GetDoubleArray("JAVA_RESAMPLE("+ "FLOAT("+in_low_err[curr_wave]+ "), " +
		      "FLOAT("+in_x[curr_wave]+ "), " + limits + ")");
		if(low_err == null || low_err.length <= 1)
			curr_y = null;			
	    }
	    if(curr_y != null) 
	    {
		if(full_flag)
		{
		    curr_x = dp.GetDoubleArray(in_x[curr_wave]);
		    if(curr_x == null || curr_x.length <= 1)
			curr_y = null;
		}
		else
		{
		    curr_x = dp.GetDoubleArray("JAVA_DIM(FLOAT("+ in_x[curr_wave]+ "), " +
			limits + ")"); 
		    if(curr_x == null || curr_x.length <= 1)
			curr_y = null;
		    else
		    {
			expanded_x = new double[curr_y.length];
			x_samples = ExpandTimes(curr_x, expanded_x);
			curr_x = expanded_x;
		    }
		}
	    }
	}
	else
	{
	    if(full_flag)
		curr_y = dp.GetDoubleArray(in_y[curr_wave]);
	    else
		curr_y = dp.GetDoubleArray("JAVA_RESAMPLE("+ "FLOAT("+in_y[curr_wave]+ "), "+
		    "FLOAT(DIM_OF("+in_y[curr_wave]+")), "+ limits + ")");
	    if(curr_y != null && curr_y.length > 1 && in_up_err != null && in_up_err[curr_wave] != null 
		    && (in_up_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    up_err = dp.GetDoubleArray(in_up_err[curr_wave]);
		else
		    up_err = dp.GetDoubleArray("JAVA_RESAMPLE(FLOAT("+ in_up_err[curr_wave]+ "), DIM_OF(FLOAT("+
			in_y[curr_wave] + ")), "+limits +")");
		if(up_err == null || up_err.length <= 1)
		    curr_y = null;
	    
	    }				
	    if(curr_y != null && in_low_err != null && 
		    in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	    {
		if(full_flag)
		    low_err = dp.GetDoubleArray(in_low_err[curr_wave]);
		else
		    low_err = dp.GetDoubleArray("JAVA_RESAMPLE(FLOAT("+in_low_err[curr_wave] +
			 "), DIM_OF(FLOAT("+ in_y[curr_wave] + ")),"+ limits + ")");
			
		if(low_err == null || low_err.length <= 1)
		    curr_y = null;
	    }				
		
	    if(curr_y != null)
	    {
		if(full_flag)
		    curr_x = dp.GetDoubleArray("DIM_OF("+in_y[curr_wave]+")");
		else
		{
		    curr_x = dp.GetDoubleArray("JAVA_DIM(FLOAT(DIM_OF("+ in_y[curr_wave]+ 
			")), "+ limits + ")"); 
		    if(curr_x != null || curr_y.length <= 1)
		    {
			expanded_x = new double[curr_y.length];
			x_samples = ExpandTimes(curr_x, expanded_x);
			curr_x = expanded_x;
		    }
		}
		if(curr_x == null)
		    curr_y = null;
	    }
	}
	if(curr_x == null)
	{
	    error = dp.ErrorString();
	    return null;
	}
	int min_len;
	if(full_flag)
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
    
    
    private int ExpandTimes(double coded_time[], double expanded_time[])
    {
	int max_len = expanded_time.length;
	int num_blocks = (coded_time.length-1) / 3;
//each block codes start, end, delta
	int out_idx, in_idx, curr_block;
	double curr_time;
	
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


    void AsynchUpdate(Signal sigs[], double xmin, double xmax, 
	double _orig_xmin, double _orig_xmax, int timestamp, boolean panning, MultiWaveform w)
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
				    				
			