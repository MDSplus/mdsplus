import java.lang.*;
import java.awt.*;

class WaveformConf {
    boolean modified = true;
    int     num_expr;
    int	    num_shot;
    String  title;
    String  x_expr[];
    String  y_expr[];
    String  up_err[];
    String  low_err[];
    String  x_max;
    String  x_min;
    String  x_label;
    String  y_max;
    String  y_min;
    String  y_label;
    int     colors_idx[];
    int	    markers[];
    boolean interpolates[];
    boolean x_log;
    boolean y_log;
    int     grid_mode;
    String  shot_str;
    String  experiment;
    int     defaults = 0;
    static final int B_shot = 8, B_x_min = 12, B_x_max = 13, B_y_min = 14, B_y_max = 15,
		     B_title = 16, B_x_label = 10, B_y_label = 11, B_exp = 7; 

    private boolean equalsString(String s1, String s2)
    {
	boolean res = false;
	
	if(s1 != null) {
	    if(s2 == null && s1.length() == 0) res = true;
	    else
		if(s1.equals(s2)) res = true;
	} else {
	    if(s2 != null && s2.length() != 0) res =  false;
	    else res = true;
	}
	return res;	        
    }

    public boolean equals(WaveformConf in)
    {
	if(num_expr != in.num_expr) return false;
	if(num_shot != in.num_shot) return false;
	
	if(!equalsString(title,   in.title))     return false;	
	if(!equalsString(x_max,   in.x_max))     return false;
	if(!equalsString(x_min,   in.x_min))     return false;
	if(!equalsString(x_label, in.x_label))   return false;
	if(x_log != in.x_log)			 return false;
	if(!equalsString(y_max,   in.y_max))     return false;
	if(!equalsString(y_min,   in.y_min))     return false;
	if(!equalsString(y_label, in.y_label))   return false;	
	if(y_log != in.y_log)			 return false;		

    	if(!equalsString(shot_str,   in.shot_str))   return false;	
	if(!equalsString(experiment, in.experiment)) return false;
	if(defaults != in.defaults) return false;	

    	for(int i = 0 ; i < num_expr; i++)
	{
	    if(!equalsString(x_expr[i],  in.x_expr[i]))   return false;
	    if(!equalsString(y_expr[i],  in.y_expr[i]))   return false;
	    if(!equalsString(up_err[i],  in.up_err[i]))   return false;
	    if(!equalsString(low_err[i], in.low_err[i]))  return false;
	}
	return true;
    }
    	    
    public void Copy(WaveformConf in)
    {
	modified = in.modified;
	num_expr = in.num_expr;
        num_shot = in.num_shot;
	if(in.title != null)
	    title = new String(in.title);
	x_expr = new String[num_expr];
	y_expr = new String[num_expr];
	up_err = new String[num_expr];
	low_err = new String[num_expr];	
	for(int i = 0 ; i < num_expr; i++)
	{   
	    if(in.x_expr[i] != null)
	       x_expr[i] = new String(in.x_expr[i]);
	    if(in.y_expr[i] != null) 
	       y_expr[i] = new String(in.y_expr[i]);
	    if(in.up_err[i] != null)
	       up_err[i] = new String(in.up_err[i]);
	    if(in.low_err[i] != null)
	       low_err[i] = new String(in.low_err[i]);
	}
	x_max = in.x_max;
	x_min = in.x_min;
	if(in.x_label != null)
	  x_label = new String(in.x_label);
	y_max = in.y_max;
	y_min = in.y_min;
    	if(in.y_label != null)
	  y_label = new String(in.y_label);
	if(in.markers != null)
	{
	    colors_idx = new int[in.colors_idx.length];
	    markers = new int[in.markers.length];
	    interpolates = new boolean[in.interpolates.length];
	    for(int i=0; i < in.markers.length; i++) {
		colors_idx[i]   = in.colors_idx[i];
    		markers[i]      = in.markers[i];
		interpolates[i] = in.interpolates[i];
	    }
	}
	x_log = in.x_log;
	y_log = in.y_log;
	if(in.shot_str != null)
	    shot_str = new String(in.shot_str);
	if(in.experiment != null)
	    experiment = new String(in.experiment);
	else
	    experiment = null;
	defaults = in.defaults;
    }
    
    public boolean useDefaultShot()
    {
	return ((defaults & (1 << B_shot)) != 0);	    
    }
}


