import java.awt.*;
import java.io.*;
import java.awt.image.*;
import java.util.Vector;

public class WaveInterface
{
    static final int MAX_NUM_SHOT = 30;
    public Waveform wave;
    public boolean full_flag;
    public int     num_waves;
    public boolean x_log, y_log;
    public String  in_label[], in_x[], in_y[], in_up_err[], in_low_err[];
    
    // Prameter used to evaluate waveform
    public String  in_xmin, in_xmax, in_ymax, in_ymin, in_timemax, in_timemin;
    public String  in_title, in_xlabel, in_ylabel;
    public String  experiment;
    public int     in_grid_mode;
    public int     height; 

    public String in_shot;
    public int num_shot = 1;
    protected boolean modified = true;
   
    boolean reversed = false;
    boolean show_legend = false;
    double legend_x, legend_y;
     
    public  int     markers_step[];
    public  int     markers[];
    public  int     colors_idx[];
    public  boolean interpolates[];
    public  int     shots[];
    public  String  error;
    private String  curr_error;
    public  String  provider;
    public  String  w_error[];
    public  Signal  signals[];
    public  float   xmax, xmin, ymax, ymin, timemax, timemin; 
    public  String  title, xlabel, ylabel;
    protected DataProvider dp;
    
// Used for asynchronous Update  
    public boolean asynch_update = true;  
    Signal wave_signals[];
    float wave_xmin, wave_xmax;
    int wave_timestamp;
    AsynchUpdater du;
    boolean request_pending;
    float orig_xmin, orig_xmax;
    protected boolean evaluated[];
        
    static final float HUGE = (float)1E8;
//    static int MAX_POINTS = 1000;
   
    protected boolean is_image = false;
    boolean use_jai = false;
    boolean keep_ratio = true;
    boolean horizontal_flip = false;
    boolean vertical_flip = false;
    int     signal_select = -1;
    Frames  frames;
    static public boolean auto_color_on_expr = false;
    
    //True when a signal is added
    protected boolean  add_signal = false;
    
        
    public WaveInterface()
    {
        CreateWaveInterface(null);
    }    


    public WaveInterface(DataProvider dp)
    {
        CreateWaveInterface(dp);
    }
    
    public void Erase()
    {
        num_waves = 0;
        in_label = null; 
        in_x = null; 
        in_y = null; 
        in_up_err = null; 
        in_low_err = null;    
        in_xmin = null; 
        in_xmax = null; 
        in_ymax = null; 
        in_ymin = null; 
        in_timemax = null; 
        in_timemin = null;
        in_title = null; 
        in_xlabel = null; 
        in_ylabel = null;
        experiment = null;
        in_shot = null;
        num_shot = 1;
        modified = true;    
        markers_step = null;
        markers = null;
        colors_idx = null;
        interpolates = null;
        shots = null;
        error = null;
        curr_error = null;
        w_error = null;
        signals = null;
        title = null; 
        xlabel = null; 
        ylabel = null;
        is_image = false;
        use_jai = false;
        keep_ratio = true;
        horizontal_flip = false;
        vertical_flip = false;
        frames = null;
        show_legend = false;
    }
    
    static void WriteLine(PrintWriter out, String prompt, String value)
    {
	    if(value != null && !value.equals("null") && value.length() != 0)
	    {
		    out.println(prompt + value);
	    }
    }
    

    static String TrimString(String s)
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
    

    static String RemoveNewLineCode(String s)
    {
	    String  y_new = new String();
	    int     new_pos = 0, old_pos = 0;
	
	   // s = TrimString(s);
	    while((new_pos = s.indexOf("|||", old_pos)) != -1)
	    {
	        y_new = y_new.concat(s.substring(old_pos, new_pos));
	        old_pos = new_pos + "|||".length();			  
	        y_new += '\n';
	    }
	    y_new = y_new.concat(s.substring(old_pos, s.length()));
	    return y_new;
    }
     
    static String AddNewLineCode(String s)
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
    
    
    
    private void CreateWaveInterface(DataProvider dp)
    {
        this.wave = wave;
	    full_flag = true;
	    this.dp = dp;
	    experiment = null;
	    shots = null;
	    in_xmin = in_xmax = in_ymin = in_ymax = in_title = null; 
	    in_xlabel = in_ylabel = in_timemax = in_timemin= null;
	    markers = null;
	    interpolates = null;
	    du = null;  
	    x_log = y_log = false;
	    show_legend = false;
	    reversed = false;
    }

   
    public void SetDataProvider(DataProvider _dp)
    {
	    dp = _dp;
		error = null;
		curr_error = null;
		w_error = null;
    }

    public void setSignalState(String name, boolean state)
    {
        
        if(num_waves != 0)
        {
            int ns = (num_shot == 0) ? 1 : num_shot;
            for(int i = 0; i < num_waves; i++)
                if(name.equals(in_y[i]) || name.equals(in_label[i]))
                {
                    for(int j = i; j < i + ns; j++)
                    {
                        interpolates[j] = state;
                        markers[i] = Signal.NONE;
                    }
                    return;
                }
        }
    }
    
    public void ShowLegend(boolean state){show_legend = state;}
    public void setModified(boolean state){modified = state;}
    

    public void SetLegendPosition(double x, double y)
    {
        legend_x = x;
        legend_y = y;
        show_legend = true;
    }


    private String GetFirstLine(String str)
    {
	    int idx = str.indexOf("\n");
	    if(idx != -1)
	        return str.substring(0, idx);
	    else
	        return str;
    }

    public  String getErrorTitle(boolean brief)
    {
        int n_error = 0;
        String er = error;

        if( num_waves == 0 || ( is_image &&  frames == null))
        {
			if( error != null)
			    if(brief)
			        er = GetFirstLine(new String(error));
            return er;
		}

		     
		for(int ii = 0; ii <   num_waves; ii++)
		{   		                
		    if(  w_error != null &&   w_error[ii] != null)
			    n_error++;               
	    }
			 
	    if(  error == null &&  n_error > 1 && n_error ==   num_waves)
	    {
		    er = "Evaluation error on all signals";
	    } 
		else 
		{
		    if(error != null)
			    er =  error;
			else 
			{
			    if(n_error == 1 && num_waves == 1)
			    {
			        er = w_error[0];
			    } else
			        if(n_error > 0)
			            er = "< Evaluation error on " + n_error + " signal"+ (n_error > 1 ? "s":"") +" >";
			}    
        }
        
        if(er != null && brief)
        {
            int idx = (er.indexOf('\n') == -1 ? er.length() : er.indexOf('\n'));
            er = er.substring(0, idx);
        }
        return er;
    }

    public boolean GetSignalState(int i)
    {
        boolean state = false;
        //int idx = i * (num_shot > 0 ? num_shot : 1);
        if(i < num_waves)
           state = (interpolates[i] || (markers[i] != Signal.NONE));
        return state;
    }
    
    public boolean[] GetSignalsState()
    {
        boolean state[] = null;
        
        if(num_waves != 0)
        {
            int ns = (num_shot > 0 ? num_shot : 1);
            state = new boolean[num_waves/ns];
            for(int i = 0, j = 0; i < num_waves; i+= ns)
                state[j++] = (interpolates[i] || (markers[i] != Signal.NONE));
        }
        return state;
    }

    public String[] GetSignalsName()
    {
        String name[] = null, s;
        
        if(num_waves != 0)
        {
            int ns = (num_shot > 0 ? num_shot : 1);
            name = new String[num_waves/ns];
            for(int i = 0, j = 0; i < num_waves; i+= ns)
            {
                s = (in_label[i] != null && in_label[i].length() != 0) ? in_label[i] : in_y[i];
                name[j++] = s;
            }
        }
        return name;
    }
    
    public boolean AddSignal(String s)
    {
        int new_num_waves, expr_idx = 0;
        
        if(num_waves != 0)
        {
            for(int i = 0; i < num_waves; i++)
                if(s.equals(in_y[i]))
                    return false;
            new_num_waves = num_waves + (num_shot != 0 ? num_shot : 1);
            expr_idx = (new_num_waves/(num_shot != 0 ? num_shot : 1)) - 1;
        } else 
            new_num_waves = 1;
        
        
        String new_in_label[] = new String[new_num_waves];
        String new_in_x[] = new String[new_num_waves];
	    String new_in_y[] = new String[new_num_waves];
	    String new_in_up_err[] = new String[new_num_waves];
	    String new_in_low_err[] = new String[new_num_waves];
    	int new_markers[] = new int[new_num_waves];
	    int new_markers_step[] = new int[new_num_waves];
	    int new_colors_idx[] = new int[new_num_waves];
	    boolean new_interpolates[] = new boolean[new_num_waves];
	    int new_shots[] = null;
	    if(shots != null) 
	        new_shots = new int[new_num_waves];
	    
	    boolean new_evaluated[] = new boolean[new_num_waves];
	    Signal new_signals[] = new Signal[new_num_waves];    
	    String new_w_error[] = new String[new_num_waves];    
	    
	    for(int i=0; i < num_waves; i++)
	    {
	        new_in_label[i] = in_label[i];
            new_in_x[i] = in_x[i];
	        new_in_y[i] = in_y[i];
	        new_in_up_err[i] = in_up_err[i];
	        new_in_low_err[i] = in_low_err[i];
    	    new_markers[i] = markers[i];
	        new_markers_step[i] = markers_step[i];
	        new_colors_idx[i] = colors_idx[i];
	        new_interpolates[i] = interpolates[i];
	        if(shots != null)
	            new_shots[i] = shots[i];
	        if(evaluated != null)
	            new_evaluated[i] = evaluated[i];
	        else
	            new_evaluated[i] = false;
	        if(signals != null)
	            new_signals[i] = signals[i];
	        if(w_error != null)
	            new_w_error[i] = w_error[i];
	    }
	    
	    	        
	    for(int i= num_waves; i < new_num_waves; i++)
	    {
	        new_in_label[i] = "";
            new_in_x[i] = "";
	        new_in_y[i] = new String(s);
	        new_in_up_err[i] = "";
	        new_in_low_err[i] = "";
    	    new_markers[i] = 0;
	        new_markers_step[i] = 1;
	        
	        
		    if(auto_color_on_expr)
	            new_colors_idx[i] = expr_idx;
            else	        
	            new_colors_idx[i] = (i - num_waves) % Waveform.colors.length;
	        
	        new_interpolates[i] = true;
	        new_evaluated[i] = false;
	        if(shots != null && shots.length != 0)
	            new_shots[i] = shots[(i - num_shot> 0) ? (i - num_shot) : 0];
        }

        in_label = new_in_label;
        in_x = new_in_x;
	    in_y = new_in_y;
	    in_up_err = new_in_up_err;
	    in_low_err = new_in_low_err;
	    markers = new_markers;
	    markers_step = new_markers_step;
	    colors_idx = new_colors_idx;
	    interpolates = new_interpolates;
	    shots = new_shots;
	    num_waves = new_num_waves;
	    evaluated = new_evaluated;
	    signals = new_signals;
	    w_error = new_w_error;
//	    modified = true;
	    
	    add_signal = true;
	    return true;
    }
    
    public boolean isAddSignal() {return add_signal;}
    public void    setAddSignal(boolean add_signal) {this.add_signal = add_signal;}
    
    public boolean UpdateShot(int curr_shots[]) throws IOException
    {
	    int l = 0, curr_num_shot;
	    
	    if(curr_shots == null)
	    {
	        curr_num_shot = 1;
	        shots = null;
	        if(num_shot == 0) return false;
	    } else
	        curr_num_shot = curr_shots.length;

	    modified     = true;
    	
	    int num_signal; 
	    int num_expr;
	    if(num_shot == 0)
        {
	        num_signal = num_waves * curr_num_shot;
	        num_expr = num_waves;
        } 
        else 
        {
	        num_signal = num_waves / num_shot * curr_num_shot;
	        num_expr = num_waves / num_shot;
        }

	    if(num_signal == 0)
	        return false;
   
    	
        String[]  in_label     = new String[num_signal];
        String[]  in_x         = new String[num_signal];
	    String[]  in_y         = new String[num_signal];
	    String[]  in_up_err    = new String[num_signal];
	    String[]  in_low_err   = new String[num_signal];
	    int[]     markers      = new int[num_signal];
	    int[]     markers_step = new int[num_signal];
	    int[]     colors_idx   = new int[num_signal];
	    boolean[] interpolates = new boolean[num_signal];
	    int[]     shots = null;
	    if(curr_shots != null)
	        shots = new int[num_signal];				

        int sig_idx = (this.num_shot == 0) ? 1 : this.num_shot;
	    for(int i = 0, k = 0; i < num_expr; i++)      
	    {
	        for(int j = 0; j < curr_num_shot; j++, k++)
	        {
		        in_label[k]     = this.in_label[i * sig_idx];
		        in_x[k]         = this.in_x[i * sig_idx];
		        in_y[k]         = this.in_y[i * sig_idx];
		        if(j < 	this.num_shot)
		        {	
		            markers[k]	    = this.markers[i * this.num_shot + j];	  
		            markers_step[k] = this.markers_step[i * this.num_shot + j];	  
		            interpolates[k] = this.interpolates[i * this.num_shot + j];
		            if(curr_shots != null)
		                shots[k]        = curr_shots[j];
		            in_up_err[k]    = this.in_up_err[i * this.num_shot + j];	    
		            in_low_err[k]   = this.in_low_err[i * this.num_shot + j];	    
		            colors_idx[k]   = this.colors_idx[i * this.num_shot + j];
		        } else {
		            markers[k]	    = this.markers[i * this.num_shot];	  
		            markers_step[k] = this.markers_step[i * this.num_shot];	  
		            interpolates[k] = this.interpolates[i * this.num_shot];
		            in_up_err[k]    = this.in_up_err[i * this.num_shot];	    
		            in_low_err[k]   = this.in_low_err[i * this.num_shot];
    		        
		            if(auto_color_on_expr)
		                colors_idx[k]   = i % Waveform.colors.length;//this.colors_idx[i * this.num_shot];
		            else
		                colors_idx[k]   = j % Waveform.colors.length;
    		            
		            if(curr_shots != null)
		                shots[k]        = curr_shots[j];
 		        }
	        }
	    }
    	
	    this.in_label     = in_label;
	    this.in_x         = in_x;
	    this.in_y         = in_y;
	    this.in_up_err    = in_up_err;
	    this.in_low_err   = in_low_err;
	    this.markers      = markers;
	    this.markers_step = markers_step;
	    this.colors_idx   = colors_idx;
	    this.interpolates = interpolates;
	    this.shots        = shots;
	    if(shots != null)
	        this.num_shot     = curr_num_shot;
	    else
	        this.num_shot = 1;
	    num_waves = num_signal;
	    
	    return true;
    }
    
    
    public synchronized int StartEvaluate() throws IOException
    {
 	    error = null;
		
		if(modified)
	        evaluated = null;

	    if((in_y == null || in_x == null) || (shots == null && 
				(experiment != null && experiment.trim().length() > 0)) )
	    {
	        error = "Missing shot or Y or X values";
	        signals = null;
	        return 0;
	    }    

	    num_waves = in_y.length;
	
		if(modified)
		{
            if(!is_image)
	            signals = new Signal[num_waves];    

	        evaluated = new boolean[num_waves];
            w_error = new String[num_waves];
        }
        
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
	        xmin = (!is_image) ? -HUGE : -1;
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
	        xmax = (!is_image) ? HUGE : -1;
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
	        ymax = (!is_image) ? HUGE : -1;
	    
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
	        ymin = (!is_image) ? -HUGE : -1;

	    if(is_image)
	    {
	        if(in_timemax != null && (in_timemax.trim()).length() != 0)
	        {
	            timemax = dp.GetFloat(in_timemax);
	            if(dp.ErrorString() != null)
	            {
		            error = dp.ErrorString();
		            return 0;
	            }
	        }
	        else
	            timemax = HUGE;

  	        if(in_timemin != null && (in_timemin.trim()).length() != 0)
	        {
	            timemin = dp.GetFloat(in_timemin);
	            if(dp.ErrorString() != null)
	            {
		            error = dp.ErrorString();
		            return 0;
	            }
	        }
	        else
	            timemin =  -HUGE;
        }
	       
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
	        title = dp.GetDefaultTitle(in_y);
	    
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
	        xlabel = dp.GetDefaultXLabel(in_y);
	    
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
	        ylabel = dp.GetDefaultYLabel(in_y);
      
      
	    return 1;
    } 
	     
	
    public synchronized void EvaluateShot(int shot) throws IOException
    {
	    int curr_wave;
        if(xmin > xmax) xmin = xmax;
        if(ymin > ymax) ymin = ymax;
        
        if(is_image)
            return;
                            
    	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	        if(shots[curr_wave] == shot && !evaluated[curr_wave] &&
	            (interpolates[curr_wave] || 
	            markers[curr_wave] != Signal.NONE))
	        {
	            w_error[curr_wave] = null;
		        evaluated[curr_wave] = true;
		        signals[curr_wave] = GetSignal(curr_wave, (float)-1E8, (float)1E8);
		        if(signals[curr_wave] == null)
		        {
		            w_error[curr_wave] = curr_error;
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
    
    public synchronized void EvaluateOthers() throws IOException
    {
	    int curr_wave;
        
        if(is_image)
        {
            InitializeFrames();
            if(frames != null)
                frames.SetViewRect((int)xmin, (int)ymin, (int)xmax, (int)ymax);
            error = curr_error;
            return;
        }
        
        
        if(evaluated == null)
        {
            signals = null;
            return;
        }
        
    	for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	        if(!evaluated[curr_wave] &&  
	           !(!interpolates[curr_wave] && this.markers[curr_wave] == Signal.NONE))
	        {
	            w_error[curr_wave] = null;    
		        evaluated[curr_wave] = true;
		        signals[curr_wave] = GetSignal(curr_wave, (float)-1E8, (float)1E8);
		        if(signals[curr_wave] == null)
		        {
	                w_error[curr_wave] = curr_error;

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
	    modified = false;
    }

    private void CreateNewFramesClass()
    {
                
        if(use_jai)
        {
            try
            {
                Class cl = Class.forName("FrameJAI");
                frames = (Frames)cl.newInstance();
            } 
            catch (Exception e)
            {
                frames = new Frames();
                use_jai = false;
            }
        } else
            frames = new Frames();
    }
    
    
    
    private void InitializeFrames()
    {
        float f_time[];
        int j = 0, i = 0;
        curr_error = null;
        byte buf[];
        WaveformEvent we;

	    if(in_y[0] == null)
	    {
	        curr_error = "Missing Y value";
	        return;
	    }
	    
	    //if(experiment != null && experiment.trim().length() > 0)    			    		    
	    //    dp.Update(experiment, shots[0]);
	    //else
	        if(shots != null && shots.length != 0)
	            dp.Update(experiment, shots[0]);
	            //dp.Update(null, shots[0]);
	        else
	            dp.Update(null, 0);

        try
        {
            we = new WaveformEvent(wave, "Loading single or multi frame image");
            wave.dispatchWaveformEvent(we);
            if( (buf = dp.GetAllFrames(in_y[0])) != null )
            {
                CreateNewFramesClass();
                frames.setHorizontalFlip(horizontal_flip);
                frames.setVerticalFlip(vertical_flip);                
                if(!frames.AddMultiFrame(buf, timemin, timemax))
                {
	                curr_error = " Can't decode multi frame image "; 
                    frames = null;
                } else {
                    we = new WaveformEvent(wave, "Decoding frame");
                    wave.dispatchWaveformEvent(we);
                    frames.WaitLoadFrame();
                }
                buf = null;
            } 
            else 
            {
                String mframe_error = dp.ErrorString();

 	            if(in_x[0] == null || in_x[0].length() == 0)
                    f_time =  dp.GetFrameTimes(in_y[0]);
                else
                    f_time = dp.GetFloatArray(in_x[0]);        
        
                if(f_time == null)
                {
                    if(mframe_error != null)
                        curr_error = " Pulse file or image file not found\nRead on pulse file error\n"+mframe_error+"\nFrame times read error";
                    else
	                    curr_error = " Image file not found "; 
	        
	                if(dp.ErrorString() != null)
	                    curr_error = curr_error +"\n"+dp.ErrorString();
	            
	                return;
                }
                CreateNewFramesClass();
                frames.setHorizontalFlip(horizontal_flip);
                frames.setVerticalFlip(vertical_flip);                
                for(i = 0, j = 0; j < f_time.length; j++)
                {
                    if(f_time[j] < timemin || f_time[j] > timemax)
                        continue;
                    we = new WaveformEvent(wave, "Loading frame "+(j+1)+"/"+f_time.length);
                    wave.dispatchWaveformEvent(we);
                    buf = dp.GetFrameAt(in_y[0], j);
                    if(buf == null)
                    {
                        continue;
                    } else {
                        frames.AddFrame(buf, f_time[j]);
                        i++;
                    }
                }
                frames.WaitLoadFrame();
        
                if(i == 0)
                {
	                curr_error = " Frames not found "; 
                    frames = null;
                }
            }
            if(frames != null)
                frames.setName(in_y[0]);
//            System.gc();
        } catch (Exception e) {
            curr_error = " Load Frames error "+e; 
        }
    }
    
    public void setShotArray(String in_shot) throws IOException
    {
        this.in_shot = in_shot;
        int curr_shots[] = GetShotArray(in_shot);
        UpdateShot(curr_shots);
    }
				    
    public int[] GetShotArray(String in_shots) throws IOException
    {
	    int int_data[] = null;
	
	    if(in_shots == null || in_shots.trim().length() == 0)
	        return int_data;
	
	    dp.Update(null, 0);
	    int_data = dp.GetIntArray(in_shots);
	    if( int_data == null || int_data.length == 0 || int_data.length > MAX_NUM_SHOT)
	    {
	        if(int_data != null && int_data.length > MAX_NUM_SHOT)
                error = "Too many shots. Max shot list elements " + MAX_NUM_SHOT +"\n";
	        else {
		        if(dp.ErrorString() != null)	    
		            error = dp.ErrorString();
		        else
		            error = "Shot syntax error\n";
	        }
	    }
	    return int_data;
   }

    private Signal GetSignal(int curr_wave, float xmin, float xmax) throws IOException
    {			    
		
//	    String limits = "FLOAT("+new Float(xmin).toString()+"), " +		    		    
//	                        "FLOAT("+new Float(xmax).toString()+")";
	                        
	    //if(experiment != null && experiment.trim().length() > 0)    			    		    
	    //    dp.Update(experiment, shots[curr_wave]);
	    //else
	        if(shots != null && shots.length != 0)
	            dp.Update(experiment, shots[curr_wave]);
	            //dp.Update(null, shots[curr_wave]);
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
		    curr_y = dp.GetFloatArray(in_y[curr_wave]);
	
	        if(curr_y != null && curr_y.length > 1 && in_up_err != null && 
		        in_up_err[curr_wave] != null && (in_up_err[curr_wave].trim()).length() != 0)
	        {
		        up_err = dp.GetFloatArray(in_up_err[curr_wave]);
		        if(up_err == null || up_err.length <= 1)
			        curr_y = null;			
	        }
	        if(curr_y != null && in_low_err != null && 
		        in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	        {
		        low_err = dp.GetFloatArray(in_low_err[curr_wave]);
		        if(low_err == null || low_err.length <= 1)
			        curr_y = null;			
	        }
	        if(curr_y != null) 
	        {
		        curr_x = dp.GetFloatArray(in_x[curr_wave]);
		        if(curr_x == null || curr_x.length <= 1)
			        curr_y = null;
	        }
	    }
	    else // Campo X non definito
	    {
	        if(full_flag)
		        curr_y = dp.GetFloatArray(in_y[curr_wave]);
	        else
		        curr_y = dp.GetFloatArray(in_y[curr_wave], in_y[curr_wave], xmin, xmax);
	        
//		        curr_y = dp.GetFloatArray("JavaResample("+ "FLOAT("+in_y[curr_wave]+ "), "+
//		                    "FLOAT(DIM_OF("+in_y[curr_wave]+")), "+ limits + ")");
		
		
	        if(curr_y != null && curr_y.length > 1 && in_up_err != null && in_up_err[curr_wave] != null 
		        && (in_up_err[curr_wave].trim()).length() != 0)
	        {
		        if(full_flag)
		            up_err = dp.GetFloatArray(in_up_err[curr_wave]);
		        else
		            up_err = dp.GetFloatArray(in_up_err[curr_wave], in_y[curr_wave], xmin, xmax);
//		            up_err = dp.GetFloatArray("JavaResample(FLOAT("+ in_up_err[curr_wave]+ "), DIM_OF(FLOAT("+
//			    in_y[curr_wave] + ")), "+limits +")");
		        if(up_err == null || up_err.length <= 1)
		            curr_y = null;
	    
	        }				
	        if(curr_y != null && in_low_err != null && 
		        in_low_err[curr_wave] != null && (in_low_err[curr_wave].trim()).length() != 0)
	        {
		        if(full_flag)
		            low_err = dp.GetFloatArray(in_low_err[curr_wave]);
		        else
		            low_err = dp.GetFloatArray(in_low_err[curr_wave], in_y[curr_wave], xmin, xmax);
//		            low_err = dp.GetFloatArray("JavaResample(FLOAT("+in_low_err[curr_wave] +
//			            "), DIM_OF(FLOAT("+ in_y[curr_wave] + ")),"+ limits + ")");
			
		        if(low_err == null || low_err.length <= 1)
		            curr_y = null;
	        }				
		
	        if(curr_y != null)
	        {
		        if(full_flag)
		            curr_x = dp.GetFloatArray(dp.GetXSpecification(in_y[curr_wave]));
		        else
		        {
		            curr_x = dp.GetFloatArray(dp.GetXSpecification(in_y[curr_wave]), ""+curr_y.length, xmin, xmax );
		            /*
		            curr_x = dp.GetFloatArray("JavaDim(FLOAT(DIM_OF("+ in_y[curr_wave]+ 
			                                    ")), "+ limits + ")"); 
		            if(curr_x != null && curr_x.length > 1)
		            {   
			            expanded_x = new float[curr_y.length];
			            x_samples = ExpandTimes(curr_x, expanded_x);
			            curr_x = expanded_x;
		            }
		            */
		            x_samples = curr_x.length;
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
	    
	    Signal out_signal;
	    String xdata_spec = dp.GetXDataSpecification(in_y[curr_wave]);
	    if(xdata_spec != null)
	    {
	      float x_data[] = dp.GetFloatArray(xdata_spec);
	      if(x_data != null && x_data.length > 1)
	        out_signal = new Signal(curr_y, x_data, curr_x, Signal.MODE_YTIME);
	      else
	      {
    	    out_signal = new Signal(curr_x, curr_y, min_len);
    	    if(x_data != null && x_data.length == 1)
    	        out_signal.setXData(x_data[0]);
	      }
	    } else {
	       // if(full_flag || wave_signals == null || wave_signals.length <= curr_wave || wave_signals[curr_wave] == null)
	       // {
    	        out_signal = new Signal(curr_x, curr_y, min_len);
    	   // } else {
    	   //     out_signal = wave_signals[curr_wave];
    	   //     out_signal.setAxis(curr_x, curr_y, min_len);
    	   // }
    	}
    	
	    if(xmin != -HUGE)
	        out_signal.xmin = out_signal.saved_xmin = xmin;
	    if(xmax != HUGE)
	        out_signal.xmax = out_signal.saved_xmax = xmax;    
    
	    if(in_ymax != null && (in_ymax.trim()).length() != 0)
	        out_signal.ymax = out_signal.saved_ymax = ymax;
	    if(out_signal.xmin > out_signal.xmax)
	        out_signal.xmin = out_signal.xmax;
	    if(in_ymin != null && (in_ymin.trim()).length() != 0)
	        out_signal.ymin = out_signal.saved_ymin = ymin;
	    if(out_signal.ymin > out_signal.ymax)
	        out_signal.ymin = out_signal.ymax;
	    out_signal.up_error = up_err;
            out_signal.low_error = low_err;
        if(up_err != null)
    	    out_signal.error = true;
        if(up_err != null && low_err != null)
    	    out_signal.asym_error = true;
    	
    	out_signal.setFullLoad(full_flag);    
    	
        return out_signal;
    }
    
/*    
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
	            for(in_idx = 0; out_idx < max_len; in_idx++)
	            { 
		            curr_time = coded_time[curr_block*3+1] + 
		            in_idx * coded_time[curr_block*3+3];
		            if(curr_time > coded_time[curr_block*3+2])
		            break;
		            expanded_time[out_idx++] = curr_time;
	            }		    		    
	        }
	    }
	    else //JAVA$DIM did not apply coding
	        for(out_idx = 0; out_idx < max_len && out_idx < coded_time.length-1; out_idx++)
		        expanded_time[out_idx] = coded_time[out_idx+1];		
    	
        return out_idx;
    }
*/

    void AsynchUpdate(/*Signal sigs[],*/ Vector sigs, float xmin, float xmax, 
	                    float _orig_xmin, float _orig_xmax, 
	                    int timestamp, boolean panning, MultiWaveform w)
    {
	    int curr_wave;
	    boolean needs_update = false;
	    if(full_flag)
	        return;
	    wave_signals = new Signal[sigs.size()];
	    for(int i = 0; i < sigs.size(); i++)
	        wave_signals[i] = (Signal)sigs.elementAt(i);
	    wave_xmin = xmin;
	    wave_xmax = xmax;
	    wave_timestamp = timestamp;
	    request_pending = true;
	    orig_xmin = _orig_xmin;
	    orig_xmax = _orig_xmax;
	    for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	        if(wave_signals[curr_wave] == null) continue;
	        if (wave_signals[curr_wave].n_points >= Waveform.MAX_POINTS - 5 || (panning && 
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
		            du.Notify();
	        }
	        else
	        {
	            try
	            {
		            DynamicUpdate(w);
		        } catch (IOException e){}
		    }
	    }
    }

    public synchronized void DynamicUpdate(MultiWaveform w) throws IOException
    {
	    boolean needs_update = false;
	    int curr_wave, saved_timestamp = wave_timestamp;
	    
	    Cursor curr_cursor = w.getCursor();
        w.setCursor(new Cursor(Cursor.WAIT_CURSOR));
	   
        WaveformEvent we = new WaveformEvent(w, "Asyncronous signal loading");
        w.dispatchWaveformEvent(we);
        
	    for(curr_wave = 0; curr_wave < num_waves; curr_wave++)
	    {
	    //if (wave_signals[curr_wave].n_points >= MAX_POINTS - 1)
	    //if (wave_signals[curr_wave].n_points >= MAX_POINTS - 1 || 
		//orig_xmin < wave_xmin || orig_xmax > wave_xmax)
	        {
	            //Check if signal is full loaded so no need dynamic update
        		if(wave_signals[curr_wave] != null && wave_signals[curr_wave].isFullLoad())
        		    continue;
        		    
		        needs_update = true;
		        wave_signals[curr_wave] = GetSignal(curr_wave, wave_xmin, wave_xmax);
	        }
        }
	    if(needs_update && saved_timestamp == wave_timestamp)
	    {
	        w.UpdateSignals(wave_signals, wave_timestamp);
	        w.AutoscaleY();
	        //w.UpdateSignals(wave_signals, wave_signals.length, wave_timestamp);
	    }
        w.setCursor(curr_cursor);
        we = new WaveformEvent(w, "Asyncronous operation completed");
        w.dispatchWaveformEvent(we);
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
    public synchronized void run()
    {
        setName("Asybch Update Thread");
        while(true)
        {
	        while(wi.request_pending)
	        {
	            wi.request_pending = false;
	            try
	            {
	                wi.DynamicUpdate(w);
	            } catch (IOException e) {}
	        }
	        try {
	            wait();
	        } catch (InterruptedException e){}
        }
    }
    public synchronized void Notify()
    {
        notify();
    }
    
}
    	


				    				
			
