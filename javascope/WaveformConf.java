import java.awt.*;

class WaveformConf {
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
    Color   colors[];
    int	    markers[];
    boolean interpolates[];
    boolean x_log;
    boolean y_log;
    boolean opt_network = true;
    int grid_mode;
    String  shot_str;
    String  experiment;
    int     defaults = 0;
    static final int B_shot = 8, B_x_min = 12, B_x_max = 13, B_y_min = 14, B_y_max = 15,
		     B_title = 16, B_x_label = 10, B_y_label = 11, B_exp = 7; 

    public void setWaveformConf(WaveInterface wi)
    {
	int prec_shot;
	boolean sh_list = false;
	StringBuffer shot_buff = new StringBuffer(512);

	if(wi == null) 	    
	    return;

	for(num_shot = 1; num_shot < wi.shots.length && wi.shots[0] != wi.shots[num_shot] ; num_shot++);

	prec_shot = -100;
	for(int i = 0; i < num_shot; i++)
	{
	    if(prec_shot == -100) {
		shot_buff.append(wi.shots[i]);
		prec_shot = wi.shots[i];
	    } else {
		if(wi.shots[i] - prec_shot == 1 && i + 1 < num_shot) 
		{
 		    prec_shot = wi.shots[i];
		    sh_list = true;  		
		} else {
		    if (sh_list)
		    {
			sh_list = false;
			if(wi.shots[i] - prec_shot == 1)
			    shot_buff.append(":" + wi.shots[i]);
			else
			    shot_buff.append(":" + prec_shot + "," + wi.shots[i]);
			prec_shot = wi.shots[i];
		    } else {
			shot_buff.append("," + wi.shots[i]);
			prec_shot = wi.shots[i];
		    }
		}		 
	    }
	}
	
	shot_str  = new String(shot_buff);				    		        
	num_expr  = wi.num_waves/num_shot;

	title	  = new String(wi.in_title);
	if(wi.in_xmax != null)
	    x_max          = new String(wi.in_xmax);
	if(wi.in_ymax != null)
	    x_min          = new String(wi.in_xmax);
	if(wi.in_ymax != null)
	    y_max          = new String(wi.in_ymax);
	if(wi.in_ymin != null)
	    y_min          = new String(wi.in_ymin);
	if(wi.in_xlabel != null)
	    x_label        = new String(wi.in_xlabel);
	if(wi.in_ylabel != null)
	    y_label        = new String(wi.in_ylabel);
	    
	x_log	  = wi.x_log;
	y_log	  = wi.y_log;
	opt_network = !wi.full_flag;
	experiment   = new String(wi.experiment);
	x_expr       = new String[num_expr];
	y_expr       = new String[num_expr];
	up_err       = new String[num_expr];
	low_err      = new String[num_expr];
	interpolates = new boolean[num_expr * num_shot];
	markers      = new int[num_expr * num_shot];
	colors       = new Color[num_expr * num_shot];
	for(int i=0, k=0; i < num_expr; i++)
	{
	
	    if(wi.in_x[i] != null)
		x_expr[i]      = new String(wi.in_x[i]);
	    if(wi.in_y[i] != null)
		y_expr[i]      = new String(wi.in_y[i]);
	    if(wi.in_up_err[i] != null)
		up_err[i]      = new String(wi.in_up_err[i]);
	    if(wi.in_low_err[i] != null)
		low_err[i]     = new String(wi.in_low_err[i]);
	    
	    for(int j=0; j < num_shot; j++)
	    {
		interpolates[k] = wi.interpolates[k];
		markers[k]      = wi.markers[k];
		colors[k]     = wi.colors[k];
		k++;
	    }
	}    	
    }
    
	    
    public void Copy(WaveformConf in)
    {
	
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
	if(in.colors != null)
	{
	    colors = new Color[in.colors.length];
	    markers = new int[in.markers.length];
	    interpolates = new boolean[in.interpolates.length];
	    for(int i=0; i < in.colors.length; i++) {
		colors[i]       = in.colors[i];
		markers[i]      = in.markers[i];
		interpolates[i] = in.interpolates[i];
	    }
	}
	x_log = in.x_log;
	y_log = in.y_log;
	opt_network = in.opt_network;
	if(in.shot_str != null)
	    shot_str = new String(in.shot_str);
	if(in.experiment != null)
	    experiment = new String(in.experiment);
	defaults = in.defaults;
    }
}
