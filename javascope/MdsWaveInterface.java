import java.io.*;
import java.awt.*;
import java.util.*;


class MdsWaveInterface extends WaveInterface {
 
 
    public String  in_def_node, in_upd_event, last_upd_event;
 
    // Configuration parameter
    public String cin_xmin, cin_xmax, cin_ymax, cin_ymin, cin_timemax, cin_timemin;
    public String cin_title, cin_xlabel, cin_ylabel;
    public String cin_def_node, cin_upd_event, cexperiment;

    public String cin_shot;
    public int defaults = 0xffffffff;
    static final int B_shot = 8, B_x_min = 12, B_x_max = 13, B_y_min = 14, B_y_max = 15,
		             B_title = 16, B_x_label = 10, B_y_label = 11, B_exp = 7, B_event = 17,
		             B_default_node = 9;
    public boolean default_is_update = true;
    jScopeDefaultValues def_vals;
    MdsWaveInterface prev_wi = null;
    
    public String previous_shot = "";
    
    public MdsWaveInterface(Waveform wave, DataProvider dp, jScopeDefaultValues def_vals)
    {
        super(dp);
        setDefaultsValues(def_vals);
        this.wave = wave;
    }
    
    public void setDefaultsValues(jScopeDefaultValues def_vals)
    {
        this.def_vals = def_vals;
        default_is_update = false;
        this.def_vals.is_evaluated = false;
    }
    

   public String GetDefaultValue(int i, boolean def_flag)
   {
	    String out = null;
       
	    switch(i)
        {
	        case MdsWaveInterface.B_title:
	        out = def_flag  ? def_vals.title_str : cin_title; break; 
	        case MdsWaveInterface.B_shot:
	        out  = def_flag ? def_vals.shot_str : cin_shot;break; 
	        case MdsWaveInterface.B_exp:
	        out =  def_flag ? def_vals.experiment_str : cexperiment;break; 
	        case MdsWaveInterface.B_x_max:
	            if(is_image)
	                out  = def_flag ? def_vals.xmax : cin_timemax;
	            else
	                out  = def_flag ? def_vals.xmax : cin_xmax; 
	        break;	    
	        case MdsWaveInterface.B_x_min:
	            if(is_image)
	                out  = def_flag ? def_vals.xmin : cin_timemin;
	            else
	                out  = def_flag ? def_vals.xmin : cin_xmin; 
	        break; 
	        case MdsWaveInterface.B_x_label:
	        out =  def_flag ? def_vals.xlabel : cin_xlabel;break; 
	        case MdsWaveInterface.B_y_max:
	        out =  def_flag ? def_vals.ymax : cin_ymax; break; 
	        case MdsWaveInterface.B_y_min:
	        out =  def_flag ? def_vals.ymin : cin_ymin; break; 
	        case MdsWaveInterface.B_y_label:
	        out =  def_flag ? def_vals.ylabel : cin_ylabel;break; 
	        case MdsWaveInterface.B_event:
	        out =  def_flag ? def_vals.upd_event_str : cin_upd_event;break; 
	        case MdsWaveInterface.B_default_node:
	        out =  def_flag ? def_vals.def_node_str : cin_def_node;break; 
	    }
	    return out;
   } 



   public void  UpdateDefault()
   {
      boolean def_flag;
      int bit;      
      
      if(default_is_update) return;
      default_is_update = true;
      
      bit = MdsWaveInterface.B_title;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_title      = GetDefaultValue(bit, def_flag);
      
      /*
      bit = MdsWaveInterface.B_shot;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_shot       = GetDefaultValue(bit ,  def_flag); 
      */
      
      bit =MdsWaveInterface.B_exp;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      experiment = GetDefaultValue(bit , def_flag );
      
      bit = MdsWaveInterface.B_x_max;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      if(is_image) {
        in_timemax  = GetDefaultValue(bit , def_flag );
        in_xmax = cin_xmax;
      } else
        in_xmax = GetDefaultValue(bit , def_flag );
      
      bit = MdsWaveInterface.B_x_min;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      if(is_image) {
        in_timemin  = GetDefaultValue(bit , def_flag );
        in_xmin = cin_xmin;
      } else
        in_xmin = GetDefaultValue(bit , def_flag );
      
      bit = MdsWaveInterface.B_x_label;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_xlabel     = GetDefaultValue(bit , def_flag ); 

      bit = MdsWaveInterface.B_y_max;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_ymax       = GetDefaultValue(bit , def_flag ); 
      
      bit = MdsWaveInterface.B_y_min;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_ymin       = GetDefaultValue(bit , def_flag); 

      bit = MdsWaveInterface.B_y_label;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_ylabel     = GetDefaultValue(bit , def_flag ); 

      bit = MdsWaveInterface.B_default_node;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);
      in_def_node = GetDefaultValue(bit , def_flag );
      
      /*
      bit = MdsWaveInterface.B_event;
      def_flag =    ((defaults & (1<<bit)) == 1<<bit);      
      in_upd_event = GetDefaultValue(bit , def_flag );
      */
   }
   
  
  /**
    Check which shot string the wave interface 
    used:
	0 wave setup defined shot;
	1 global setting defined shot
	2 main scope defined shot
  */
  
    public int GetShotIdx()
    {  
        String main_shot_str = ((jScopeWaveContainer)(wave.getParent())).getMainShotStr();
        if(UseDefaultShot())
        {
	        if(main_shot_str != null && main_shot_str.length() != 0)
	            return 2;
	        else
	            return 1;
        } else
	        return 0;
    }
    
  public String GetUsedShot()
  {
    String out = null;
  
	switch(GetShotIdx())
	{
	    case 0: out = cin_shot; break;
	    case 1: out = this.def_vals.shot_str;break;
	    case 2: out = ((jScopeWaveContainer)(wave.getParent())).getMainShotStr(); break;
	}
	return out;  
  }
  
  
  public String Update() throws IOException
  {
     UpdateShot();
     if(error == null)
     {
        UpdateDefault();
        dp.SetDefaultNid(in_def_node);
     } else {
        signals = null;
     }
     return error;
  }

  public void UpdateShot() throws IOException
  {
	int curr_shots[] = null;
	//, l = 0, curr_num_shot;
	String c_shot_str = this.GetUsedShot();
  
    error = null;
    
    if(c_shot_str != null)
    {
        if(previous_shot.equals(c_shot_str))
            return;
        previous_shot = new String(c_shot_str);
    }
    else
    {
       if(previous_shot.equals("not defined"))
           return;
       previous_shot = "not defined";
    }
    
    if(UseDefaultShot())
    {
        String main_shot_str = ((jScopeWaveContainer)(wave.getParent())).getMainShotStr();
	    if(main_shot_str != null && main_shot_str.length() != 0)
	    {
            curr_shots = ((jScopeWaveContainer)(wave.getParent())).getMainShots();
            if(curr_shots == null)
                error = "Main Shot evaluation error: " + ((jScopeWaveContainer)(wave.getParent())).getMainShotError(true);
	    } else {
	      if(def_vals.is_evaluated)
	        curr_shots =  def_vals.shots;
	      else 
	      {
	        curr_shots = GetShotArray(def_vals.shot_str);
	        if(error == null) 
	        {
	          def_vals.shots = curr_shots; 
	          def_vals.is_evaluated = true;
	        }
	      }  
	    }
	} else {
	  curr_shots = GetShotArray(cin_shot);
	}
	
	in_shot = c_shot_str;
    if(!UpdateShot(curr_shots))
       previous_shot = "not defined";
  }
  
    public MdsWaveInterface(MdsWaveInterface wi)
    {
        previous_shot = wi.previous_shot;
	    full_flag = wi.full_flag;
	    provider = wi.provider;
	    num_waves = wi.num_waves;
	    num_shot  = wi.num_shot;
	    defaults  = wi.defaults;
	    modified = wi.modified;//********true;
	    in_grid_mode = wi.in_grid_mode;
	    x_log = wi.x_log;
	    y_log = wi.y_log;
	    is_image = wi.is_image;
	    use_jai = wi.use_jai;
	    keep_ratio = wi.keep_ratio;
	    vertical_flip = wi.vertical_flip;
	    horizontal_flip = wi.horizontal_flip;	
	    show_legend = wi.show_legend;
	    reversed = wi.reversed;
	    legend_x = wi.legend_x;
	    legend_y = wi.legend_y;
	    in_label = new String[num_waves];
	    in_x = new String[num_waves];
	    in_y = new String[num_waves];
	    in_up_err = new String[num_waves];
	    in_low_err = new String[num_waves];
	    markers = new int[num_waves];
	    markers_step = new int[num_waves];
	    colors_idx = new int[num_waves];
	    interpolates = new boolean[num_waves];
	    
	    w_error = new String[num_waves];
	    evaluated = new boolean[num_waves];
	    signals = new Signal[num_waves];
	    
	    if(wi.in_shot == null || wi.in_shot.trim().length() == 0)
	       shots = wi.shots = null;
	    else
	       shots = new int[num_waves];
    	
	    for(int i = 0; i < num_waves; i++)
	    {
	        if(wi.in_label[i] != null)
		        in_label[i] = new String(wi.in_label[i]);
	        else
		        in_label[i] = null;
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
	        colors_idx[i] = wi.colors_idx[i];
	        interpolates[i] = wi.interpolates[i];
	        if(wi.shots != null)
	            shots[i] = wi.shots[i];
/*****/
	        if(wi.evaluated != null)
	            evaluated[i] = wi.evaluated[i];
	        else
	            evaluated[i] = false;
	        if(wi.signals != null)
	            signals[i] = wi.signals[i];
	        if(wi.w_error != null)
	            w_error[i] = wi.w_error[i];
/*****/	            
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
    	    
	    if(wi.in_timemax != null)
	        in_timemax = new String(wi.in_timemax);
	    else
	        in_timemax = null;
	    if(wi.in_timemin != null)
	        in_timemin = new String(wi.in_timemin);
	    else
	        in_timemin = null;

    			
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

    /*
	    if(wi.in_upd_event != null)
	        in_upd_event = new String(wi.in_upd_event);
	    else
	        in_upd_event = null;
    */
	    if(wi.in_def_node != null)
	        in_def_node = new String(wi.in_def_node);
	    else
	        in_def_node = null;


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

	    if(wi.cin_timemax != null)
	        cin_timemax = new String(wi.cin_timemax);
	    else
	        cin_timemax = null;
	    if(wi.cin_timemin != null)
	        cin_timemin = new String(wi.cin_timemin);
	    else
	        cin_timemin = null;


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
    	    	
	    def_vals = wi.def_vals;
    	    	
	    // error = null;
	    SetDataProvider(wi.dp);	
        //super.SetDataProvider(wi.dp);
        //dp = wi.dp;
    }	


    public boolean UseDefaultShot()
    {
	    return ((defaults & (1 << B_shot)) != 0);	    
    }
    
   
    public String getErrorString(boolean brief_error)
    {
       String full_error = null;
       
       if(w_error == null || w_error.length == 0) return null;
       
	   if(!is_image)
	   {
	        int i;
	        int idx = 0;
	        
	        String e;
	        
	        if(isAddSignal())
	            i = w_error.length - 1;
	        else
	            i = 0;
	        
	        for(; i < w_error.length; i++)
	        {
	            e = w_error[i];
	            if(e != null) 
	            {
	                if(!isAddSignal())
	                    e = "<Wave "+(i+1)+ "> " + e;
	                if(brief_error)
	                {
	                    idx = e.indexOf('\n');
	                    if(idx < 0)
	                        idx = e.length();
	                }
	                if(full_error == null)
	                {
	                    if(brief_error)
		                    full_error = e.substring(0, idx) + "\n";
	                    else 
		                    full_error = e + "\n";
	                } 
	                else 
	                {
	                    if(brief_error)
		                    full_error = full_error + e.substring(0, idx) + "\n";
		                else
		                    full_error = full_error + e + "\n";
		            }
		        }
		    }
        }
        return full_error;
    }
    
    public void SetDataProvider(DataProvider _dp)
    {
	    super.SetDataProvider(_dp);
	    default_is_update = false;
    }

    public synchronized void refresh()
    {
        try
        {
            error = Update();
            if(error == null)
            {	    
	            //if(modified)
	            {
	                StartEvaluate();
	                if(error == null)
		                EvaluateOthers();
	            }

	            //modified = (error != null);
            }
        } 
        catch (IOException e) 
        {
            error = e.getMessage();
        }
    }
    
   public void Erase()
    {
        super.Erase();
        in_def_node = null;  
        in_upd_event = null;  
        last_upd_event = null;
        cin_xmin = null;  
        cin_xmax = null;  
        cin_ymax = null;  
        cin_ymin = null;  
        cin_timemax = null;  
        cin_timemin = null;
        cin_title = null;  
        cin_xlabel = null;  
        cin_ylabel = null;
        cin_def_node = null;  
        cin_upd_event = null;  
        cexperiment = null;
        in_shot = null;  
        cin_shot = null;
        defaults = 0xffffffff;
        default_is_update = false;
        previous_shot = "";
    }
   
    public void CreateVector()
    {
        in_label = new String[num_waves];      
	    shots	 = new int[num_waves];
	    in_y	 = new String[num_waves];
	    in_x	 = new String[num_waves];
	    in_up_err    = new String[num_waves];
	    in_low_err   = new String[num_waves];
	    markers      = new int[num_waves];
	    markers_step = new int[num_waves];
	    colors_idx   = new int[num_waves];
	    interpolates = new boolean[num_waves];
	
	    for(int i = 0; i < num_waves; i++)
	    {
	        markers[i]      = 0;
	        markers_step[i] = 1;
    	    colors_idx[i]   = i % Waveform.colors.length;
	        interpolates[i] = true;
	    }
    }

    public void AddEvent(NetworkListener w) throws IOException
    {
        int bit = MdsWaveInterface.B_event;
        boolean def_flag = ((defaults & (1<<bit)) == 1<<bit);      
        String new_event = GetDefaultValue(bit , def_flag );
        if(in_upd_event == null || !in_upd_event.equals(new_event))
        {
            AddEvent(w, new_event);
        }
    }

    public void RemoveEvent(NetworkListener w)  throws IOException
    {
        if(in_upd_event != null)
        {
            dp.removeNetworkListener(w, in_upd_event);
            in_upd_event = null;
        }
    }

    public void RemoveEvent(NetworkListener w, String event)  throws IOException
    {
        dp.removeNetworkListener(w, event);
    }
   
    public void AddEvent(NetworkListener w, String event) throws IOException
    {   
        
       if( in_upd_event != null && in_upd_event.length() != 0)
       {
            if(event == null || event.length() == 0)
            {
                dp.removeNetworkListener(w, in_upd_event);
                in_upd_event = null;
            } 
            else 
            {
                if(!in_upd_event.equals(event))
                {
                    dp.removeNetworkListener(w, in_upd_event);
                    dp.addNetworkListener(w, event);
                    in_upd_event = event;
                }
            }
        } else 
            if(event != null && event.length() != 0) 
            { 

                dp.addNetworkListener(w, event);
                in_upd_event = event;
            }
    }

    public void ToFile(PrintWriter out, String prompt) throws IOException
    { 
	    int exp, exp_n, sht, sht_n, cnum_shot, eval_shot = 1; 

 	    cnum_shot = num_shot;
 	    
	    if(UseDefaultShot())
	    {
	        if(cin_shot != null && cin_shot.length()> 0)
	        {
		        cnum_shot = (GetShotArray(cin_shot)).length;
	        } else {
		        cnum_shot = 1;
		    }
	    }

	    WaveInterface.WriteLine(out,prompt + "x_label: "         , cin_xlabel);
	    WaveInterface.WriteLine(out,prompt + "y_label: "         , cin_ylabel);
	    if(!is_image)
	    {
	        WaveInterface.WriteLine(out,prompt + "x_log: "           , ""+x_log);
	        WaveInterface.WriteLine(out,prompt + "y_log: "           , ""+y_log);
	        if(show_legend) {
	            WaveInterface.WriteLine(out,prompt + "legend: "           , "("+legend_x+","+legend_y+")");
	        }
	    } else {
	       WaveInterface.WriteLine(out,prompt + "is_image: "          , ""+is_image);
	       WaveInterface.WriteLine(out,prompt + "use_jai: "           , ""+use_jai);
	       WaveInterface.WriteLine(out,prompt + "keep_ratio: "        , ""+keep_ratio);
	       WaveInterface.WriteLine(out,prompt + "horizontal_flip: "   , ""+horizontal_flip);
	       WaveInterface.WriteLine(out,prompt + "vertical_flip: "     , ""+vertical_flip);
	    }    
	    
	   WaveInterface.WriteLine(out,prompt + "experiment: "      , cexperiment);
	   WaveInterface.WriteLine(out,prompt + "event: "           , cin_upd_event);
	   WaveInterface.WriteLine(out,prompt + "default_node: "    , cin_def_node);

	    if(cnum_shot != 0) {
	        if(UseDefaultShot())
	            eval_shot = num_shot > 0 ? num_shot : 1;
	        else
	            eval_shot = cnum_shot;
	       WaveInterface.WriteLine(out,prompt + "num_expr: "        , ""+num_waves/eval_shot);
	    } else
	       WaveInterface.WriteLine(out,prompt + "num_expr: "        , ""+num_waves);
	
	   WaveInterface.WriteLine(out,prompt + "num_shot: "        , ""+cnum_shot);
	   WaveInterface.WriteLine(out,prompt + "shot: "            , cin_shot);
	   WaveInterface.WriteLine(out,prompt + "ymin: "            , cin_ymin);
	   WaveInterface.WriteLine(out,prompt + "ymax: "            , cin_ymax);
	   WaveInterface.WriteLine(out,prompt + "xmin: "            , cin_xmin);
	   WaveInterface.WriteLine(out,prompt + "xmax: "            , cin_xmax);
        if(is_image)
        {
	       WaveInterface.WriteLine(out,prompt + "time_min: "            , cin_timemin);
	       WaveInterface.WriteLine(out,prompt + "time_max: "            , cin_timemax);
        }
	   WaveInterface.WriteLine(out,prompt + "title: "           , cin_title);
	   WaveInterface.WriteLine(out,prompt + "global_defaults: " , ""+defaults);
	
		    
	    for(exp = 0, exp_n = 1; exp < num_waves; exp += eval_shot, exp_n++)
	    {
	       WaveInterface.WriteLine(out,prompt + "label"     + "_" + exp_n + ": " , in_label[exp]);
	       WaveInterface.WriteLine(out,prompt + "x_expr"     + "_" + exp_n + ": " , AddNewLineCode(in_x[exp]));
	       WaveInterface.WriteLine(out,prompt + "y_expr"     + "_" + exp_n + ": " , AddNewLineCode(in_y[exp]));
	        
	        if(!is_image)
	        {
	           WaveInterface.WriteLine(out,prompt + "up_error"   + "_" + exp_n + ": " , in_up_err[exp]);
	           WaveInterface.WriteLine(out,prompt + "low_error"  + "_" + exp_n + ": " , in_low_err[exp]);
			
	            for(sht = 0, sht_n = 1; sht < cnum_shot; sht++, sht_n++)
	            {			    
		           WaveInterface.WriteLine(out,prompt + "interpolate"+"_"+exp_n+"_"+sht_n+": ",""+interpolates[exp + sht]);								    
		           WaveInterface.WriteLine(out,prompt + "color"      +"_"+exp_n+"_"+sht_n+": ",""+colors_idx[exp + sht]);
		           WaveInterface.WriteLine(out,prompt + "marker"     +"_"+exp_n+"_"+sht_n+": ",""+markers[exp + sht]);
		           WaveInterface.WriteLine(out,prompt + "step_marker"+"_"+exp_n+"_"+sht_n+": ",""+markers_step[exp + sht]);
	            }
	        }
	    } 
    }


    public void FromFile(Properties pr, String prompt) throws IOException
    { 
	    String prop = null;
	    int len, pos, num_expr = 0;
     
        Erase();

        try
        {
            prop = pr.getProperty(prompt+".height");
            if(prop != null)
 		        height = new Integer(prop).intValue();
           
            prop = pr.getProperty(prompt+".grid_mode");
            if(prop != null)
 		        in_grid_mode = new Integer(prop).intValue();
            
		    cin_xlabel = pr.getProperty(prompt+".x_label");

		    cin_ylabel = pr.getProperty(prompt+".y_label");
    		
		    cin_title = pr.getProperty(prompt+".title");

		    cin_ymin = pr.getProperty(prompt+".ymin");

		    cin_ymax = pr.getProperty(prompt+".ymax");
    		
		    cin_xmin = pr.getProperty(prompt+".xmin");

		    cin_xmax = pr.getProperty(prompt+".xmax");
    		
		    cin_timemin = pr.getProperty(prompt+".time_min");
    		
		    cin_timemax = pr.getProperty(prompt+".time_max");

		    cin_def_node = pr.getProperty(prompt+".default_node");

		    cin_upd_event = pr.getProperty(prompt+".event");
    				
            prop = pr.getProperty(prompt+".x_log");
            if(prop != null)
 		        x_log = new Boolean(prop).booleanValue();;
    		
            prop = pr.getProperty(prompt+".y_log");
            if(prop != null)
 		        y_log = new Boolean(prop).booleanValue();;		
    		        
            prop = pr.getProperty(prompt+".legend");
            if(prop != null)
            {
		        show_legend = true;
		        legend_x = Double.valueOf(prop.substring(prop.indexOf("(") + 1, prop.indexOf(","))).doubleValue();
		        legend_y = Double.valueOf(prop.substring(prop.indexOf(",") + 1, prop.indexOf(")"))).doubleValue();
            }
     		            
            prop = pr.getProperty(prompt+".is_image");
            if(prop != null)
 		        is_image = new Boolean(prop).booleanValue();;		
    		        
            prop = pr.getProperty(prompt+".use_jai");
            if(prop != null)
 		        use_jai = new Boolean(prop).booleanValue();;		

            prop = pr.getProperty(prompt+".keep_ratio");
            if(prop != null)
 		        keep_ratio = new Boolean(prop).booleanValue();;		
    		            
            prop = pr.getProperty(prompt+".horizontal_flip");
            if(prop != null)
 		        horizontal_flip = new Boolean(prop).booleanValue();;		
    		        
            prop = pr.getProperty(prompt+".vertical_flip");
            if(prop != null)
 		        vertical_flip = new Boolean(prop).booleanValue();;		
    		        

		    cexperiment = pr.getProperty(prompt+".experiment");

		    cin_shot = pr.getProperty(prompt+".shot");

		    prop = pr.getProperty(prompt+".x");
		    if(prop != null)
		    {
		        String x_str = prop;
    				
		        x_str = RemoveNewLineCode(x_str);
		        if(in_x == null || in_x.length == 0)
			        in_x = new String[1];
		        in_x[0] = x_str;
		    }

		    prop = pr.getProperty(prompt+".y");
		    if(prop != null)
		    {
		        StringTokenizer st_y = null;
		        StringTokenizer st_x = null;		        
		        num_shot = 1;
		        String token = null;
		        String y_str = prop.toLowerCase();
		        String x_str = null;

		        y_str = RemoveNewLineCode(y_str);
    		    
		        if(in_x != null && in_x.length != 0)
			        x_str = in_x[0];
    																	    
		        if(y_str.indexOf("[") == 0 && y_str.indexOf("$roprand") != -1)
		        {
    		        //Parse [expression1,$roprand,expression2,....,$roprand,expressionN]  
    		        st_y = new StringTokenizer(y_str, "[]");
    		        token = st_y.nextToken();
    		        st_y = new StringTokenizer(token, ",");
    		       
    		       
    		        while(st_y.hasMoreTokens())
    		        {
    		            token = st_y.nextToken().trim();
    		            if(token.indexOf("$roprand") != -1)
    		                continue;
			            num_expr++;    		                
    		        }
    		    } else
    		        num_expr = 1; 
    		
		        num_waves = num_expr * num_shot;
		        CreateVector();

		        if(num_expr == 1) 
		        {
			        in_y[0] = y_str;
			        if( in_y[0].indexOf("multitrace") != -1) 
			        {
			            in_y[0] = "compile"+y_str.substring(y_str.indexOf("("));
			            //in_y[0] =  in_y[0].substring(in_y[0].indexOf("\"") + 1, in_y[0].indexOf("\")"));  
			        }
			        
			        if(x_str != null)
			            in_x[0] = new String(x_str);
			        else
			            in_x[0] = null;
    		    
		        } else {
		            
    		        st_y = new StringTokenizer(y_str, "[]");
    		        String token_y = st_y.nextToken();
    		        st_y = new StringTokenizer(token_y, ",");
                    if(x_str != null)
                    {
    		            st_x = new StringTokenizer(x_str, "[]");
    		            token = st_x.nextToken();
    		            st_x = new StringTokenizer(token, ",");
                    }
		            
		            
		            int i = 0;			
		            while(st_y.hasMoreTokens())
    		        {
    		            token_y = st_y.nextToken().trim();
   		            
     		            if(st_x != null)
    		               token = st_x.nextToken().trim(); 	               		                

   		            
    		            if(token_y.indexOf("$roprand") != -1)
    		                continue;
    		                    		                
    		            in_y[i] = token_y;
    		            if(st_x != null)
    		               in_x[i] = token; 	               		                
			            if(in_y[i].indexOf("multitrace") != -1)
			            {
				            in_y[i] =   "compile"+in_y[i].substring(in_y[i].indexOf("("));  
			            }
			            i++; 
    		        } 
		        }    
		    }
            
		    prop = pr.getProperty(prompt+".num_expr");
		    if(prop != null)
		    {
		        num_expr = new Integer(prop).intValue();
		   //     num_expr = (num_expr > 0) ? num_expr : 1;						
            } 
             
		    prop = pr.getProperty(prompt+".num_shot");
		    if(prop != null)
		    {
		        num_shot = new Integer(prop).intValue();
		        num_shot = (num_shot > 0) ? num_shot : 1;
		        if(num_expr != 0)
		        {
		            num_waves = num_expr * num_shot;
		            CreateVector();
		        }
            } 



		    prop = pr.getProperty(prompt+".global_defaults");
		    if(prop != null)
            {
		        defaults = new Integer(prop).intValue();
            }
            
            int expr_idx;
            
            for(int idx = 1; idx <= num_expr; idx++)
            {
		        prop = pr.getProperty(prompt+".label_"+idx);
		        if(prop != null)
                {
                    expr_idx = (idx - 1)*num_shot;
		            in_label[expr_idx] = prop;
		            for(int j = 1; j < num_shot; j++)
			            in_label[expr_idx + j] = prop;			
                }
     
		        prop = pr.getProperty(prompt+".x_expr_"+idx);
		        if(prop != null)
                {
                    expr_idx = (idx - 1)*num_shot;
		            in_x[expr_idx] = RemoveNewLineCode(prop);
		            for(int j = 1; j < num_shot; j++)
			            in_x[expr_idx + j] = in_x[expr_idx];			
                }            
     
		        prop = pr.getProperty(prompt+".y_expr_"+idx);
		        if(prop != null)
                {
                    expr_idx = (idx - 1)*num_shot;
		            in_y[expr_idx] = RemoveNewLineCode(prop);
		            for(int j = 1; j < num_shot; j++)
			            in_y[expr_idx + j] = in_y[expr_idx];			
                }
                
		        prop = pr.getProperty(prompt+".up_error_"+idx);
		        if(prop != null)
                {
                    expr_idx = (idx - 1)*num_shot;
		            in_up_err[expr_idx] = prop;
		            for(int j = 1; j < num_shot; j++)
			            in_up_err[expr_idx + j] = prop;			
                }
                
		        prop = pr.getProperty(prompt+".in_low_err"+idx);
		        if(prop != null)
                {
                    expr_idx = (idx - 1)*num_shot;
		            in_low_err[expr_idx] = prop;
		            for(int j = 1; j < num_shot; j++)
			            in_low_err[expr_idx + j] = prop;			
                }
                
                for(int s = 1; s <= num_shot; s++)
                {
                    expr_idx = (idx - 1)*num_shot - 1;
                    
		            prop = pr.getProperty(prompt+".interpolate_"+idx+"_"+s);
		            if(prop != null)
                    {
                        interpolates[expr_idx + s] = new Boolean(prop).booleanValue();               
                    }
     
		            prop = pr.getProperty(prompt+".color_"+idx+"_"+s);
		            if(prop != null)
                    {
                        try
                        {
                            colors_idx[expr_idx + s] = new Integer(prop).intValue();
                        } catch(Exception e) {colors_idx[expr_idx + s] = 0;}
                    }
                    
		            prop = pr.getProperty(prompt+".marker_"+idx+"_"+s);
		            if(prop != null)
                    {
                        try
                        {
                            markers[expr_idx + s] = new Integer(prop).intValue();
                        } catch(Exception e) {markers[expr_idx + s] = 0;}
                    }
                    
		            prop = pr.getProperty(prompt+".step_marker_"+idx+"_"+s);
		            if(prop != null)
                    {
                        try
                        {
                            markers_step[expr_idx + s] = new Integer(prop).intValue();
                        } catch(Exception e) {markers_step[expr_idx + s] = 0;}
                    }            
                }
            }
        }
        catch (Exception e)
        {
            throw(new IOException(e + " \n when parsing " + prop ));
        }
    }	 
}		

