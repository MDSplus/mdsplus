//import com.apple.mrj.*;
import java.io.*;
import java.awt.*;


class ScopeConfiguration {
    final static String msg_error_title = "File configuration sintax error\n";
    String title, event;
    String error, msg_error;
    StringBuffer msg_error_buffer = new StringBuffer(msg_error_title);
    int num_conf = 0;
    int columns = 1;
    int rows[] = new int[4];
    int prec_rows[] = new int[4];
    private WaveformConf wc[];
    WaveformConf gwc;
    int select_wc_idx;
    int height = 500, width = 700, xpos = 50, ypos = 50;
    boolean fast_network_access = true;
    float height_percent[], width_percent[]; 
    BufferedWriter out;


    ScopeConfiguration()
    {
	rows[0] = 1;
	prec_rows[0] = 1;
	gwc = new WaveformConf();
    }
    
    public void SetWaveformsConf(WaveformConf[] wc)
    {
	this.wc = wc;
    }
    
    public void SetWaveformConf(int idx, WaveformConf _wc)
    {
	if(wc != null && idx < wc.length)
	    wc[idx] = _wc;
    }

    public WaveformConf CopyWaveformConf(int idx)
    {
	if(wc[idx] == null) wc[idx] = new WaveformConf();
	if(wc[select_wc_idx] != null)
	    wc[idx].Copy(wc[select_wc_idx]);
	return wc[idx];
    }
    
    public WaveformConf GetWaveformConf(int idx)
    {
	if(wc != null && idx < wc.length)
	    return wc[idx];
	else
	    return null;
    }

    public void SetModified(int idx, boolean mod)
    {
	wc[idx].modified = true;
    }
        
    public WaveformConf GetWaveformConf(int idx, boolean def)
    {
	WaveformConf wc_new = null;
	
	if(wc != null && idx < wc.length) 
	{
	    wc_new = new WaveformConf();
	    wc_new.Copy(wc[idx]);
	    if(def)
	    {
		for(int i=0; i < 32; i++)
		{
		    switch(i)
		    {
			case WaveformConf.B_title:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.title = gwc.title; break; 
			case WaveformConf.B_shot:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.shot_str = gwc.shot_str;break; 
			case WaveformConf.B_exp:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.experiment = gwc.experiment;break; 
			case WaveformConf.B_x_max:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.x_max = gwc.x_max; break; 
			case WaveformConf.B_x_min:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.x_min = gwc.x_min; break; 
			case WaveformConf.B_x_label:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.x_label = gwc.x_label;break; 
			case WaveformConf.B_y_max:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.y_max = gwc.y_max; break; 
			case WaveformConf.B_y_min:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.y_min = gwc.y_min; break; 
			case WaveformConf.B_y_label:
			    if ((wc_new.defaults & (1<<i)) == 1<<i) wc_new.y_label = gwc.y_label;break; 
		    }
		}
    	    }
	} 
        return wc_new;
    }
    
    public void ChangeRowColumn(int _row[])
    {
	columns = 0;
	for(int i=0; i < 4; i++) {
	    prec_rows[i] = rows[i];
	    rows[i] = _row[i];
	    if(rows[i] != 0)
		columns++;
	}
    }	

    private void UpdateRows(int col, int row)
    {
	prec_rows[col] = rows[col];
	rows[col] = row;
	num_conf += rows[col];
	WaveformConf new_wc[] = new WaveformConf[num_conf];
	float new_height_percent[] = new float[num_conf];
	for(int i = 0; i < num_conf; i++)
	    if(i < num_conf - rows[col]) {
		new_wc[i] = wc[i];
		new_height_percent[i] = height_percent[i];
	    } else {
		new_wc[i] = new WaveformConf();
	    }
	wc = new_wc;
	height_percent = new_height_percent;
    } 
  
    private Color StringToColor(String str)
    {
	int pos;
	String tmp = str.substring(str.indexOf("=") + 1, pos = str.indexOf(","));
	int r = new Integer(tmp).intValue();
	tmp = str.substring(pos + 3, pos = str.indexOf(",", pos + 1));
	int g = new Integer(tmp).intValue();
	tmp = str.substring(pos + 3, str.indexOf("]", pos + 1));
	int b = new Integer(tmp).intValue();
	int c = (r<<16 | g << 8 | b);
	return(new Color(c));
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

    private String addNewLineCode(String s)
    {
	String  s_new = new String();
	int     new_pos = 0, old_pos = 0;
	while((new_pos = s.indexOf("\n", old_pos)) != -1)
	{
	    s_new = s_new.concat(s.substring(old_pos, new_pos));
	    old_pos = new_pos + "\n".length();			  
	    s_new += "|||";
	}
	s_new = s_new.concat(s.substring(old_pos, s.length()));
	return s_new;
    }
    
    ScopeConfiguration(String conf_file, ColorDialog c_list)    
    {
	WaveformConf new_wc[];
	String str = null;
	int wc_idx, height_w[] = new int[4];
	boolean first_color = true;

		
	gwc = new WaveformConf();
        try {
	    BufferedReader in = new BufferedReader(new FileReader(conf_file));
     
	    while((str = in.readLine()) != null) {
	      try {
		if(str.trim().length() != 0 && str.indexOf("Scope") == -1) {
		    rows[0] = 1;
		    error = new String("Invalid file configuration\n");
    		    return;
		}
    		if(str.indexOf("Scope.geometry:") == 0) {
		    int pos;
		    int len = "Scope.geometry: ".length();
		    width = new Integer(str.substring(len, pos = str.indexOf("x", len))).intValue();
		    height = new Integer(str.substring(pos + 1, pos = str.indexOf("+", pos))).intValue();
		    xpos= new Integer(str.substring(pos + 1, pos = str.indexOf("+", pos + 1))).intValue();
		    ypos = new Integer(str.substring(pos + 1, pos = str.length())).intValue();
		    continue;
    		}
		if(str.indexOf("Scope.columns:") == 0) {
		    int len = "Scope.columns: ".length();
		    columns = new Integer(str.substring(len, str.length())).intValue();
		    width_percent = new float[columns];
		    continue;
    		}
		if(str.indexOf("Scope.title:") == 0) {
		    title = str.substring("Scope.title: ".length(), str.length());
		    continue;
    		}
		if(str.indexOf("Scope.title_event:") == 0) {
		    event = str.substring("Scope.title_event: ".length(), str.length());
		    continue;	
		}

		if(str.indexOf("Scope.fast_network_access:") != -1)
		{
		    fast_network_access =  new Boolean(str.substring("Scope.fast_network_access: ".length(), 
										    str.length())).booleanValue();
		    continue;		
		}
				    
		if(str.indexOf("Scope.item_color_") != -1)
		{
		    int len;
		    int i = new Integer(str.substring("Scope.item_color_".length(), len = str.indexOf(":"))).intValue();
		    String name = new String(str.substring(len  + 2, len = str.indexOf(",")));
		    Color cr = StringToColor(new String(str.substring(len + 2, str.length())));
		    if(first_color) {
			first_color = false;
			c_list.removeAllColorItems();
		    }
		    c_list.insertItemAt(name, cr, i);
		    continue;
    		}
	    
    		if(str.indexOf("Scope.rows_in_column_") != -1)
		{
		    int len;
		    int c = new Integer(str.substring("Scope.rows_in_column_".length(), len = str.indexOf(":"))).intValue() - 1;
		    int r = new Integer(str.substring(len + 2, str.length())).intValue();
		    UpdateRows(c, r);	
		    continue;		
		}
				    
		if(str.indexOf("Scope.global_1_1") == 0)
		{
		    int len = str.indexOf(":") + 2;

		    if(str.indexOf(".grid_mode:") != -1)
		    {
			gwc.grid_mode = new Integer(str.substring(len, str.length())).intValue();
   			continue;		
		    }

		    if(str.indexOf(".xmax:") != -1)
		    {
			gwc.x_max = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".xmin:") != -1)
		    {
			gwc.x_min = str.substring(len, str.length());
			continue;		
		    }
		    		    
		    if(str.indexOf(".x_label:") != -1)
		    {
			gwc.x_label = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymax:") != -1)
		    {
			gwc.y_max = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymin:") != -1)
		    {
			gwc.y_min = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".y_label:") != -1)
		    {
			gwc.y_label = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".x_log:") != -1)
		    {
			gwc.x_log = new Boolean(str.substring(len, str.length())).booleanValue();
			continue;		
		    }
		    if(str.indexOf(".y_log:") != -1)
		    {
			gwc.y_log = new Boolean(str.substring(len, str.length())).booleanValue();
			continue;		
		    }		    
		    if(str.indexOf(".experiment:") != -1)
		    {
			gwc.experiment = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".shot:") != -1)
		    {
			gwc.num_shot = 1;
			gwc.shot_str = str.substring(len, str.length());
			if(gwc.shot_str.indexOf("_shots") != -1) {
			    gwc.shot_str  =  gwc.shot_str.substring(gwc.shot_str.indexOf("[")+1, gwc.shot_str.indexOf("]")); 
			    char[] sh = gwc.shot_str.toCharArray();
    			    for(int i = 0; i < gwc.shot_str.length(); i++)
				if(sh[i] == ',') gwc.num_shot++;
    			}
    			continue;		
		    }		
		}
		if(str.indexOf("Scope.plot_") == 0)
		{
		    int pos;
		    wc_idx = 0;
		    int len = "Scope.:plot_".length() - 1;
		    int row = new Integer(str.substring(len, pos = str.indexOf("_", len))).intValue() - 1;
		    int col = new Integer(str.substring(pos + 1, str.indexOf(".", pos))).intValue() - 1;
		    for(int i = 0; i < col; i++)
			wc_idx += rows[i];
		    wc_idx += row;
		    len = str.indexOf(":") + 2;
		    if(str.indexOf(".height:") != -1)
		    {
			int h = new Integer(str.substring(len, str.length())).intValue();
			height_percent[wc_idx] = (float) h;
			height_w[col] += h; 
   			continue;		
		    }
		    
		    if(str.indexOf(".grid_mode:") != -1)
		    {
			wc[wc_idx].grid_mode = new Integer(str.substring(len, str.length())).intValue();
   			continue;		
		    }
		    
		    if(str.indexOf(".x_label:") != -1)
		    {
			wc[wc_idx].x_label = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".y_label:") != -1)
		    {
			wc[wc_idx].y_label = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".x_log:") != -1)
		    {
			wc[wc_idx].x_log = new Boolean(str.substring(len, str.length())).booleanValue();
			continue;		
		    }
		    if(str.indexOf(".y_log:") != -1)
		    {
			wc[wc_idx].y_log = new Boolean(str.substring(len, str.length())).booleanValue();
			continue;		
		    }		    
		    if(str.indexOf(".experiment:") != -1)
		    {
			wc[wc_idx].experiment = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".shot:") != -1)
		    {
			wc[wc_idx].num_shot = 1;
			wc[wc_idx].shot_str = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".x:") != -1)
		    {
			String x_str = str.substring(len, str.length());
		    		    
			x_str = removeNewLineCode(x_str);
			if(wc[wc_idx].num_expr == 0)
			    wc[wc_idx].x_expr          = new String[1];
			wc[wc_idx].x_expr[0] = x_str;
			continue;		
		    }
		    if(str.indexOf(".y:") != -1)
		    {
		        int n_shot;
			String y_str = str.substring(len, str.length());
			String x_str = null;

			y_str = removeNewLineCode(y_str);
			
			if(wc[wc_idx].x_expr != null)
			    x_str = wc[wc_idx].x_expr[0];
																		
			if(y_str.toLowerCase().indexOf("$roprand") != -1)
			{
		    
			    pos = y_str.toLowerCase().indexOf(",$roprand");
			    do {
				wc[wc_idx].num_expr++;
				pos += ",$roprand".length();
			    } while((pos = y_str.toLowerCase().indexOf(",$roprand", pos)) != -1 );
			    wc[wc_idx].num_expr++;
			} else {
			    wc[wc_idx].num_expr = 1;
			}
		    
			n_shot = (gwc.num_shot > 1) ? gwc.num_shot:1;
			wc[wc_idx].y_expr          = new String[wc[wc_idx].num_expr];
			wc[wc_idx].x_expr          = new String[wc[wc_idx].num_expr];
			wc[wc_idx].markers         = new int[wc[wc_idx].num_expr * n_shot];
		      //wc[wc_idx].colors        = new Color[wc[wc_idx].num_expr * n_shot];
			wc[wc_idx].colors_idx      = new int[wc[wc_idx].num_expr * n_shot];
			wc[wc_idx].interpolates    = new boolean[wc[wc_idx].num_expr * n_shot];
			for(int i = 0; i < n_shot * wc[wc_idx].num_expr; i++) {
			    wc[wc_idx].interpolates[i] = true;
			    wc[wc_idx].colors_idx[i] = -1;
			}
			wc[wc_idx].up_err          = new String[wc[wc_idx].num_expr];
			wc[wc_idx].low_err         = new String[wc[wc_idx].num_expr];

			if(wc[wc_idx].num_expr == 1) {
			    wc[wc_idx].y_expr[0] = y_str;
			    if( wc[wc_idx].y_expr[0].toLowerCase().indexOf("multitrace") != -1)
			    {
				wc[wc_idx].y_expr[0] =  wc[wc_idx].y_expr[0].substring(wc[wc_idx].y_expr[0].indexOf("\\") + 1, wc[wc_idx].y_expr[0].indexOf("\")"));  
			    }
			    if(x_str != null)
				wc[wc_idx].x_expr[0] = new String(x_str);
			    else
				wc[wc_idx].x_expr[0] = null;
			    
    			} else {
			  int pos_x = (x_str != null) ? x_str.indexOf("[") : 0;
			  int pos_y = y_str.indexOf("[");			  
			  int i;			
			    for(i = 0; i < wc[wc_idx].num_expr - 1; i++)
			    {
				wc[wc_idx].y_expr[i] = y_str.substring(pos_y + 1, pos_y = y_str.toLowerCase().indexOf(",$roprand", pos_y));
				if(x_str != null) {
				    if(x_str.indexOf(",0", pos_x) != -1)
					wc[wc_idx].x_expr[i] = x_str.substring(pos_x  + 1, pos_x =  x_str.indexOf(",0", pos_x));			    			
				}
    				if(wc[wc_idx].y_expr[i].toLowerCase().indexOf("multitrace") != -1)
				{
				    wc[wc_idx].y_expr[i] =  wc[wc_idx].y_expr[i].substring(wc[wc_idx].y_expr[i].indexOf("\\") + 1, 
										  wc[wc_idx].y_expr[i].indexOf("\")"));  
				}
				pos_y += ",$roprand".length();
				pos_x += ",0".length();
			    }
			    wc[wc_idx].y_expr[i] = y_str.substring(pos_y + 1,  y_str.indexOf("]", pos_y));
			    if(x_str != null)
				wc[wc_idx].x_expr[i] = x_str.substring(pos_x + 1, x_str.indexOf("]", pos_x));			    			
			}    
			continue;		
		    }
		    if(str.indexOf(".num_expr:") != -1)
		    {
			wc[wc_idx].num_expr = new Integer(str.substring(len, str.length())).intValue();
			wc[wc_idx].y_expr   = new String[wc[wc_idx].num_expr];
			wc[wc_idx].x_expr   = new String[wc[wc_idx].num_expr];
			wc[wc_idx].up_err   = new String[wc[wc_idx].num_expr];
		        wc[wc_idx].low_err  = new String[wc[wc_idx].num_expr];
   			continue;		
		    }		    
		    if(str.indexOf(".num_shot:") != -1)
		    {
			wc[wc_idx].num_shot     = new Integer(str.substring(len, str.length())).intValue();			
			wc[wc_idx].markers      = new int[wc[wc_idx].num_expr * wc[wc_idx].num_shot];
		      //wc[wc_idx].colors       = new Color[wc[wc_idx].num_expr * wc[wc_idx].num_shot];
			wc[wc_idx].colors_idx   = new int[wc[wc_idx].num_expr * wc[wc_idx].num_shot];
			wc[wc_idx].interpolates = new boolean[wc[wc_idx].num_expr * wc[wc_idx].num_shot];
   			continue;		
		    }
		    if(str.indexOf(".global_defaults:") != -1)
		    {
			wc[wc_idx].defaults = new Integer(str.substring(len, str.length())).intValue();
   			continue;		
		    }		    
		    if((pos = str.indexOf(".x_expr_")) != -1)
		    {			
			pos += ".x_expr_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].x_expr[expr_idx] = removeNewLineCode(str.substring(pos + 2, str.length()));
			continue;		
    		    }		    
		    if((pos = str.indexOf(".y_expr_")) != -1)
		    {			
			pos += ".y_expr_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].y_expr[expr_idx] = removeNewLineCode(str.substring(pos + 2, str.length()));
			continue;		
    		    }		    
		    if((pos = str.indexOf(".up_error_")) != -1)
		    {			
			pos += ".up_error_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].up_err[expr_idx] = str.substring(pos + 2, str.length());
			continue;		
    		    }		    
		    if((pos = str.indexOf(".low_error_")) != -1)
		    {			
			pos += ".low_error_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].low_err[expr_idx] = str.substring(pos + 2, str.length());
			continue;		
    		    }
		    
		    if((pos = str.indexOf(".interpolate_")) != -1)
		    {			
			pos += ".interpolate_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
			int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].interpolates[expr_idx *  wc[wc_idx].num_shot + shot_idx] = 
						    new Boolean(str.substring(pos + 2, str.length())).booleanValue();
			continue;		
    		    }
		    if((pos = str.indexOf(".color_")) != -1)
		    {
		      pos += ".color_".length();
		      int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
		      int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
		      try {			
//			Color cr = StringToColor(new String(str.substring(pos + 2, str.length())));
//    			wc[wc_idx].colors[expr_idx *  wc[wc_idx].num_shot + shot_idx] = cr;
			int c_idx = new Integer(str.substring(pos + 2, str.length())).intValue();
    			wc[wc_idx].colors_idx[expr_idx *  wc[wc_idx].num_shot + shot_idx] = c_idx;
		      } catch(Exception e) { wc[wc_idx].colors_idx[expr_idx *  wc[wc_idx].num_shot + shot_idx] = 0;}
    			continue;		
    		    }
		    if((pos = str.indexOf(".marker_")) != -1)
		    {			
			pos += ".marker_".length();
			int expr_idx = new Integer(str.substring(pos, pos = str.indexOf("_", pos))).intValue() - 1;
			int shot_idx = new Integer(str.substring(pos + 1, pos = str.indexOf(":", pos))).intValue() - 1;
			wc[wc_idx].markers[expr_idx *  wc[wc_idx].num_shot + shot_idx] = 
						    new Integer(str.substring(pos + 2, str.length())).intValue();
			continue;		
    		    }
		    		    		    
		    if(str.indexOf(".title:") != -1)
		    {
			wc[wc_idx].title = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymin:") != -1)
		    {
			wc[wc_idx].y_min = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymax:") != -1)
		    {
			wc[wc_idx].y_max = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".xmin:") != -1)
		    {
			wc[wc_idx].x_min = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".xmax:") != -1)
		    {
			wc[wc_idx].x_max = str.substring(len, str.length());
			continue;		
		    }
		}
		
		if(str.indexOf("Scope.vpane_") != -1)
	  	{
		    int len;
		    int c = new Integer(str.substring("Scope.vpane_".length(), len = str.indexOf(":"))).intValue() - 1;
		    int w = new Integer(str.substring(len + 2, str.length())).intValue();
		    width_percent[c] = (float)w;
		    continue;		
		}
	      }		    	  		    
	      catch(StringIndexOutOfBoundsException e)
	      {
		 if(str.length() > 70)
		    msg_error_buffer.append(str.substring(0, 65) + " ...\n");
		 else
		    msg_error_buffer.append(str + "\n");
	      }		
	      catch(ArrayIndexOutOfBoundsException e)
	      {
		 if(str.length() > 70)
		    msg_error_buffer.append(str.substring(0, 65) + " ...\n");
		 else
		    msg_error_buffer.append(str + "\n");
	      }		
	    }
	    if(columns == 4)
		width_percent[3] = (float)((1000 - width_percent[2])/1000.);
	    if(columns >= 3)
		width_percent[2] = (float)(((width_percent[2] != 0) ? (width_percent[2] - width_percent[1]): 1000 - width_percent[1])/1000.);
	    if(columns >= 2)
		width_percent[1] = (float)(((width_percent[1] != 0) ? (width_percent[1] - width_percent[0]): 1000  - width_percent[0])/1000.);
	    if(columns >= 1)
		width_percent[0] = (float)((width_percent[0])/1000.);
	    
	    int k = 0;
    	    for(int j = 0; j < columns; j++)
		for(int i = 0; i < rows[j]; i++) {
		    if(height_percent[k] < RowColumnLayout.MIN_SIZE) {
			k -= i;
			for(i = 0; i < rows[j]; i++)
			    height_percent[k++] = (float)1./rows[j];			
			break;
		    }
		    height_percent[k] = height_percent[k++]/height_w[j];
	    	}
		    
	    for(k = 0; k < rows.length && rows[k] != 0; k++);
	    if(k != columns) 
    		error = new String("Configuration file sintax error\n");
	} 
	catch(Exception e) {
	    error = new String("Configuration file not found or invalid file configuration\n");
	}
	
	if(msg_error_buffer.length() > msg_error_title.length())
	    msg_error = msg_error_buffer.toString();
    }

    private void writeLine(String prompt, String value)
    {
        try {
	    if(value != null && !value.equals("null") && value.length() != 0)
	    {
		out.write(prompt + value);
		out.newLine();	    	
	    }
	} catch(IOException e) {
	    System.out.println("Errore : " + e);
	}		
    }

    public void SaveScopeConfiguration(String conf_file, MultiWaveform w[], ColorDialog c_list)    
    {
	String str;
	int wc_idx, height_w[] = new int[4];
	File f;
	
	if(System.getProperty("os.name").equals("Mac OS"))
	{	
	 //   MRJFileUtils.setDefaultFileType(new MRJOSType("TEXT"));
	 //   MRJFileUtils.setDefaultFileCreator(new MRJOSType("JSCP"));
	} 
	f = new File(conf_file);    
	if(f.exists()) f.delete();   
	
        try {
	    out = new BufferedWriter(new FileWriter(f));
	    out.write("Scope.geometry: " + width + "x" + height + "+" + ypos + "+" + xpos);
	    out.newLine();
	    if(title != null) {
		out.write("Scope.title: " + title);
		out.newLine();
	    }	    
	    out.write("Scope.columns: " + columns);
	    out.newLine();

	    writeLine("Scope.fast_network_access: ", ""+fast_network_access);		    

    	    for(int i = 0; i < c_list.GetNumColor(); i++)
	    {
		writeLine("Scope.item_color_" + i + ": " , c_list.GetNameAt(i) + "," + c_list.GetColorAt(i));		
	    }
	    	    
	    writeLine("Scope.global_1_1.experiment: " , gwc.experiment);
	    writeLine("Scope.global_1_1.shot: "       , gwc.shot_str);
	    writeLine("Scope.global_1_1.title: "      , gwc.title);
	    writeLine("Scope.global_1_1.xmax: "       , gwc.x_max);
	    writeLine("Scope.global_1_1.xmin: "       , gwc.x_min);
	    writeLine("Scope.global_1_1.x_label: "    , gwc.x_label);
	    writeLine("Scope.global_1_1.ymax: "       , gwc.y_max);
	    writeLine("Scope.global_1_1.ymin: "       , gwc.y_min);
	    writeLine("Scope.global_1_1.y_label: "    , gwc.y_label);
	    	    
	    for(int i = 0, c = 1, k = 0; i < columns; i++,c++)
	    {
		writeLine("Scope.rows_in_column_" + c + ": " , ""+rows[i]);
    		for(int j = 0, r = 1; j < rows[i]; j++, r++)
		{
		    out.newLine();
		    writeLine("Scope.plot_" + r + "_" + c + ".height: "          , ""+w[k].getSize().height );
		    writeLine("Scope.plot_" + r + "_" + c + ".grid_mode: "       , ""+wc[k].grid_mode);
		    writeLine("Scope.plot_" + r + "_" + c + ".x_label: "         , wc[k].x_label);
		    writeLine("Scope.plot_" + r + "_" + c + ".y_label: "         , wc[k].y_label);
		    writeLine("Scope.plot_" + r + "_" + c + ".x_log: "           , ""+wc[k].x_log);
		    writeLine("Scope.plot_" + r + "_" + c + ".y_log: "           , ""+wc[k].y_log);
		    writeLine("Scope.plot_" + r + "_" + c + ".experiment: "      , wc[k].experiment);
		    writeLine("Scope.plot_" + r + "_" + c + ".shot: "            , wc[k].shot_str);
		    writeLine("Scope.plot_" + r + "_" + c + ".num_expr: "        , ""+wc[k].num_expr);
		    writeLine("Scope.plot_" + r + "_" + c + ".num_shot: "        , ""+wc[k].num_shot);
    		    writeLine("Scope.plot_" + r + "_" + c + ".ymin: "            , wc[k].y_min);
		    writeLine("Scope.plot_" + r + "_" + c + ".ymax: "            , wc[k].y_max);
    		    writeLine("Scope.plot_" + r + "_" + c + ".xmin: "            , wc[k].x_min);
		    writeLine("Scope.plot_" + r + "_" + c + ".xmax: "            , wc[k].x_max);
		    writeLine("Scope.plot_" + r + "_" + c + ".title: "           , wc[k].title);
		    
		    for(int exp = 0, exp_n = 1, s = 0; exp < wc[k].num_expr; exp++, exp_n++)
		    {
			if(wc[k].x_expr[exp] != null) 
			    writeLine("Scope.plot_" + r + "_" + c + ".x_expr" + "_" + exp_n + ": " , addNewLineCode(wc[k].x_expr[exp]));
			if(wc[k].y_expr[exp] != null) 			    
			    writeLine("Scope.plot_" + r + "_" + c + ".y_expr"  + "_" + exp_n + ": " , addNewLineCode(wc[k].y_expr[exp]));
			if(wc[k].up_err[exp] != null) {			    
			    writeLine("Scope.plot_" + r + "_" + c + ".up_error" + "_" + exp_n + ": "  , wc[k].up_err[exp]);
			}			    
			if(wc[k].low_err[exp] != null) {			    
			    writeLine("Scope.plot_" + r + "_" + c + ".low_error"  + "_" + exp_n + ": " , wc[k].low_err[exp]);
			}
			for(int sht = 0, sht_n = 1; sht < wc[k].num_shot; sht++, sht_n++)
			{			    
			    writeLine("Scope.plot_" + r + "_" + c + ".interpolate" + "_" + exp_n + "_" + sht_n + ": " ,
										    ""+wc[k].interpolates[s]);								    
			    writeLine("Scope.plot_" + r + "_" + c + ".color" + "_" + exp_n + "_" + sht_n + ": " , 
										    ""+wc[k].colors_idx[s]);
//										    ""+wc[k].colors[s]);
			    writeLine("Scope.plot_" + r + "_" + c + ".marker" + "_" + exp_n + "_" + sht_n + ": " ,
										    ""+wc[k].markers[s]);
			    s++;
			}			    			    
		    } 
		    writeLine("Scope.plot_" + r + "_" + c + ".global_defaults: " , ""+wc[k].defaults);
		    k++;
		}
	    }
	    
	    for(int i = 1, k = 0, pos = 0; i < columns; i++)
	    {
		pos += (int)(((float)w[k].getSize().width/ width) * 1000.);
		writeLine("Scope.vpane_" + i + ": " , ""+pos);
		k += rows[i-1];
	    } 
	    out.close();
    	} 
	catch(IOException e) {
	    System.out.println("Errore : " + e);
	}		
    }
}

