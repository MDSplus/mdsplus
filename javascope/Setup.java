import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;
//import com.apple.mrj.*;

public class Setup extends Object implements WaveSetup {
    Label coords_label; 
    SetupDataDialog sd;	
    Button3Menu menu;	
    public MultiWaveform waves[];
    public int num_waves;
    public jScope main_scope;
    public int wave_idx;
    
    String title, event, print_event;
    String data_server_address;
    int num_conf = 0;
    int columns = 1;
    int rows[] = new int[4];
    int prec_rows[] = new int[4];
    int height = 500, width = 700, xpos = 50, ypos = 50;
    boolean fast_network_access = false;
    float height_percent[], width_percent[]; 
    boolean modified;
    MultiWaveform sel_wave;
    int signal_idx;



   public Setup(Frame pw, Label label)
    {
	    main_scope = (jScope)pw;
	    rows[0] = 1;
	    prec_rows[0] = 1;
	    coords_label = label;
	    sd = new SetupDataDialog(pw, "Setup data");
	    //if(!main_scope.is_applet) boooo!!!!!!!
	    {
	        menu = new Button3Menu(pw, sd, this);
	        pw.add(menu);
	    }
    }
    
    public void newWaves(int num)
    {
	waves = new MultiWaveform[num];
	for(int i = 0; i < num; i++)
	    waves[i] = new MultiWaveform(this);
	num_waves = num;
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
    
    public Point GetWavePos(Waveform w)
    {
	int col = 0, row = 0;
	int idx = GetWaveIndex(w) + 1;
	for(col = 0; col < 4; col++)
	{
	    for(row = 0; row < rows[col] && idx != 0; row++)
		idx--;
	    if(idx == 0) break;
	}
	Point p = new Point(col + 1, row);
	return p;
    }

    public void SetSetup(Waveform w, int x, int y)    
    {
        if(menu == null) return;
	    wave_idx = GetWaveIndex(w);
    	menu.Show((MultiWaveform)w, x, y);
    }

    
    public void SetAllWaveformPointer(int pointer_mode)
    {
	    for(int i = 0; i < waves.length; i++)
	        waves[i].SetMode(pointer_mode);	
    }

    public  void SetErrorTitle(MultiWaveform w)
    {
        int n_error = 0;

        w.error_flag = false;
		if(w.wi != null)
		{
                if(w.wi.num_waves == 0)
                {
			        if(w.wi.error != null)
			            w.SetTitle(jScope.GetFirstLine(new String(w.wi.error)));
                        return;
		        }
		        for(int ii = 0; ii < w.wi.num_waves; ii++)
		        {		                
			        if(w.wi.w_error != null && w.wi.w_error[ii] != null)
			            n_error++;
			                
			    }
			    if(w.wi.error == null &&  n_error > 1 && n_error == w.wi.num_waves)
			    {
			        w.SetTitle("Evaluation error on all signals");
			    } else {
			        if(w.wi.error != null)
			            w.SetTitle(w.wi.error);
			        else {
			            if(n_error == 1 && w.wi.num_waves == 1) {
			                w.SetTitle(jScope.GetFirstLine(w.wi.w_error[0]));
			             } else
			                if(n_error > 0)
			                    w.SetTitle("< Evaluation error on " + n_error + " signals >");
			       }    
              }
		 }
    }

    public void SetAllErrorTitle()
    {
            
	    for(int i = 0, k = 0; i < 4; i++)
	    {
		    for(int j = 0, n_error; j < rows[i]; j++, k++) 
		    {
		        SetErrorTitle(waves[k]);
		    }
	    }
    }


    public void RemoveSelection()
    {
	    for(int i = 0; i < waves.length; i++)
	        if(waves[i].IsSelected()) 
	        {
		        main_scope.wi_source = null;
		        waves[i].SetSelected(false);
		        break;
	        } 		
    }
        
        

    
    
    public void ChangeDataProvider()
    {
	    for(int i = 0; i < waves.length; i++)
	        if(waves[i].wi != null)
		        waves[i].wi.SetDataProvider(main_scope.db);
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

    public void DisplayCoords(Waveform w, double curr_x, double curr_y, 
                              double dx, double dy, int sig_idx, 
                              boolean measure, boolean broadcast)
    { 
        if(coords_label != null)
	    {   
	        String s;
	        signal_idx = sig_idx;
	        WaveInterface  wi = ((MultiWaveform)w).wi;
	        
	        if(measure)
	        {
	         double dx_f;
	         
	         if(Math.abs(dx) < 1.e-20)
	            dx_f = 1.e-20;
	         else
	            dx_f = Math.abs(dx);
	         
            s = SetStrSize("[" + Waveform.ConvertToString(curr_x, false) + ", " 
				               + Waveform.ConvertToString(curr_y, false) + "; dx "
				               + Waveform.ConvertToString(dx, false) + "; dy "
				               + Waveform.ConvertToString(dy, false) + "; 1/dx "
				               + Waveform.ConvertToString(1./dx_f, false) +
				               "]", 80);

	        } else {
	            s = SetStrSize("[" + Waveform.ConvertToString(curr_x, false) + ", " 
				                    + Waveform.ConvertToString(curr_y, false) + "]", 30);
	        }
	        
	        if(wi.shots != null)
	        {
		        coords_label.setText( s +
		            " Expr : " + wi.in_y[sig_idx] +  
		            " Shot = " + wi.shots[sig_idx]);
	        } else {
		        coords_label.setText(s +
		            " Expr : " + wi.in_y[sig_idx]);  
	        }
	        //if(broadcast)
	        if(true) //Like Scope always broadcast
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
	    } else {
	        main_scope.wi_source = null;
	    }
    }
    
    public void NotifyChange(Waveform dest, Waveform source)    
    {       
	  ((MultiWaveform)dest).wi = new WaveInterface(main_scope.wi_source);
	  ((MultiWaveform)dest).wi.SetDataProvider(main_scope.db);
	  Refresh(dest, "");
    }
   
   public void SetFontToWaves(Font f)
   {
      waves[0].SetFont(f);
      for(int i = 0; i < num_waves; i++)
         waves[i].Repaint();
   }
   
   public String UpdateWave(MultiWaveform wave)
   {
	WaveInterface wi = wave.wi;
	
	wave.error_flag = false;
	if(wi == null)
	    return "Undefine signal to evaluate";
	    
	if(wi.modified)
	{
	    wi.StartEvaluate();
	    if(wi.error == null)
		    wi.EvaluateOthers();
	}
	wave.Update(wi);
	wi.modified = (wi.error != null);
	
	return  wi.error;		
   }
   
       
    public void SetFastNetworkAll(boolean state)
    {
    	fast_network_access = state;
	    for(int i = 0; i < num_waves; i++)
	        if(waves[i].wi != null)
		        waves[i].wi.full_flag = !state;
    }    
 

    public void Hide(){	
    }    	
    
    public void UpdatePoints(double x, Waveform curr_w)
    {
        int i = 0, ii;
        int idx = GetWaveIndex(curr_w);
        
        for(i = 0, ii = 0; i < 4; i++)
        {
            if(idx < ii + rows[i])
                break;
            ii += rows[i];
        }
        
        i = 0;
		while(i < num_waves)
		{
            if(ii != idx)
		        waves[ii].UpdatePoint(x);
		    ii = (ii+1)%num_waves;
		    i++;
		}
    }
    
    public void AutoscaleAll()
    {
	for(int i = 0; i < num_waves; i++) {
	    waves[i].Autoscale();
//	    waves[i].repaint();
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
    
    public synchronized void Refresh(Waveform w, String label)
    {
        Point p = GetWavePos(w);
	    
	    w.SetMode(Waveform.MODE_WAIT);
	    main_scope.SetStatusLabel("Refresh wave row " + p.x + " column " + p.y + label);
	    //main_scope.updateMainShot();
	    main_scope.setup_default.updateDefaultWI(((MultiWaveform)w).wi);
        main_scope.updateMainShot();	    
	    main_scope.updateShotWI(((MultiWaveform)w).wi);
	    if(((MultiWaveform)w).wi != null)
	        ((MultiWaveform)w).wi.modified = true; 
	    UpdateWave((MultiWaveform)w);	    
	    SetErrorTitle(((MultiWaveform)w));
	    main_scope.SetStatusLabel("Wave row " + p.x + " column "+ p.y + " refreshed");	    
	    w.SetMode(main_scope.wave_mode);
    }    
    
    public void RemovePanel(Waveform w)
    {
        Point p = GetWavePos(w);
        MultiWaveform _waves[] = new MultiWaveform[num_waves - 1];
        int new_rows[] = new int[4];
        
        for(int i = 0, k = 0, kk = 0; i < 4; i++) {
            new_rows[i] = rows[i]; 
            for(int j = 0; j < rows[i]; j++, kk++) {
                if(i == p.x-1 && j == p.y-1)
                {
                    if(waves[kk] == sel_wave)
                        sel_wave = null;
                    if(waves[kk].IsSelected())
                    {
                        main_scope.wi_source = null;
		                waves[kk].SetSelected(false);
		            }
                    new_rows[i]--;
                    continue;
                }
                 _waves[k] = waves[kk];
                 k++;
            }
        }
	    columns = 0;
	    
	    for(int i=0; i < 4; i++) {
	        if(new_rows[i] != 0)
	            columns = i + 1;
	        else
	            if(i < 3 && new_rows[i+1] != 0)
	            {
	                columns = i + 1;
	                new_rows[i] = new_rows[i+1];
	                new_rows[i+1] = 0;	                
	            }
            	        
	    }

	    prec_rows = rows;
	    rows = new_rows;
        waves = _waves;
        num_waves--;
        main_scope.draw_pan.updateWavePanel();    
    }
    
    public void ChangeRowColumn(int _row[])
    {
	    columns = 0;
	    for(int i=0; i < 4; i++) {
	        prec_rows[i] = rows[i];
	        rows[i] = _row[i];
	        if(rows[i] != 0)
	            columns = i + 1;
		        //columns++;
	    }
    }
    
    public int GetNumWave()
    {
	    int n_wave = 0;
    
	    for(int i = 0; i < 4; i++)
	        n_wave += rows[i];
	    return n_wave;
    } 
    
    public void SaveAsText(MultiWaveform w)
    {
        if(w == null || w.wi == null) return;
        
	    FileDialog file_diag = new FileDialog(main_scope, "Save signals as", FileDialog.SAVE);
        file_diag.pack();
        file_diag.show();
	    String txtsig_file = file_diag.getDirectory() + file_diag.getFile();
	    if(txtsig_file != null)
	    {
            try {
	            BufferedWriter out = new BufferedWriter(new FileWriter(txtsig_file));
	            for(int i = 0; i < w.wi.signals.length; i++)
	            {
	                out.write("x : " + w.wi.in_x[i]);
	                out.newLine();
	                out.write("y : " + w.wi.in_y[i]);
	                out.newLine();
	                out.write("shot : " + w.wi.shots[i]);
	                out.newLine();
	                for(int j = 0; j < w.wi.signals[i].x.length; j++)
	                {
	                    out.write(""+w.wi.signals[i].x[j]+" "+""+w.wi.signals[i].y[j]);
	                    out.newLine();	                    
	                }
	            }
	            out.close();
	        } catch (IOException e) {
	            System.out.println(e);
	        }
	    }
	    file_diag = null;
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

    public int fromFile(ReaderConfig in, String prompt) throws IOException
    {
	String str;
	int error = 0;

	num_conf = 0;
	for(int i=0; i < 4; i++)
	    rows[i] = 0;
     
	while((str = in.readLine()) != null) 
	{
	    if(str.trim().length() != 0 && str.indexOf("Scope") != 0) {
		return 1;
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

	    if(str.indexOf("Scope.update_event:") == 0) {
		event = str.substring("Scope.update_event: ".length(), str.length());
		continue;
	    }

	    if(str.indexOf("Scope.print_event:") == 0) {
		print_event = str.substring("Scope.print_event: ".length(), str.length());
		continue;
	    }

	    if(str.indexOf("Scope.fast_network_access:") != -1)
	    {
		fast_network_access =  new Boolean(str.substring("Scope.fast_network_access: ".length(), 
										str.length())).booleanValue();
		continue;		
	    }
	    
	    if(str.indexOf("Scope.data_server_address:") != -1)
	    {
		    data_server_address = str.substring("Scope.data_server_address: ".length(), str.length());
            if(!main_scope.is_applet)
                main_scope.server_diag.addServerIp(data_server_address);
		    continue;	
	    }
					
	    if(str.indexOf("Scope.rows_in_column_") != -1)
	    {
		int len;
		int c = new Integer(str.substring("Scope.rows_in_column_".length(), len = str.indexOf(":"))).intValue() - 1;
		int r = new Integer(str.substring(len + 2, str.length())).intValue();
		rows[c] = r;
		continue;		
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
	
	//Evaluate real number of columns
	int r_columns =  0;
	for(int i = 0; i < 4; i++)
	    if(rows[i] != 0)
		    r_columns = i + 1;

	if(width_percent == null)
	{
	    width_percent = new float[r_columns];
	    columns = 0;
    }
	    //Silent file configuration correction
	    //possible define same warning information	    	    

	    if(columns != r_columns)
	    {			
		columns = r_columns;
		width_percent = new float[columns];
		for(int i = 0; i < columns; i++)
		    width_percent[i] = (float) 1./columns;
	    } else {			    
		if(columns == 4)
		    width_percent[3] = (float)((1000 - width_percent[2])/1000.);
		if(columns >= 3)
		    width_percent[2] = (float)(((width_percent[2] != 0) ? (width_percent[2] - width_percent[1]): 1000 - width_percent[1])/1000.);
		if(columns >= 2)
		    width_percent[1] = (float)(((width_percent[1] != 0) ? (width_percent[1] - width_percent[0]): 1000  - width_percent[0])/1000.);
		if(columns >= 1)
		    width_percent[0] = (float)(((width_percent[0] == 0) ? 1000 : width_percent[0])/1000.);
	    }
	
	
	return error;
    }
    
    public void updateHeight()
    {
	    float height = 1.F;
        Dimension d = waves[0].getMinimumSize();
	
	    height_percent = new float[waves.length];
    
    	for(int i=0; i < rows[0]; i++)
	        height += waves[i].wi.height;
      
	    for(int j = 0, k = 0; j < columns; j++)
	        for(int i = 0; i < rows[j]; i++, k++)
	        {
		        if( waves[k].wi.height < d.height)//RowColumnLayout.MIN_SIZE_H)
		        {
		            k -= i;
		            for(i = 0; i < rows[j]; i++)
			            height_percent[k++] = (float)1./rows[j];			
		            break;
		        }
	            height_percent[k] = (float)(waves[k].wi.height/height);
	        }
    }

    private void SaveConfiguration(PrintWriter out, boolean is_html) throws IOException
    {
	    MultiWaveform w[] = waves;//main_scope.setup.waves;
	    int wc_idx, height_w[] = new int[4];
                
	        if(is_html)
	        {         
	            out.write("<HTML>\n");
	            out.write("<HEAD>\n");
	            out.write("<TITLE>Autogenerated HTML</TITLE>\n");
                out.write("</HEAD>\n");
                out.write("<BODY>\n");
                out.write("<APPLET CODE=\"Applet1.class\" WIDTH=0 HEIGHT=0>\n");
                out.write("<PARAM NAME = CONFIGURATION VALUE = \"\n");
	        }
	        
	        out.println("Scope.geometry: " + width + "x" + height + "+" + ypos + "+" + xpos);

	        jScope.writeLine(out, "Scope.title: ", title);		    
	        jScope.writeLine(out, "Scope.update_event: ", event);		    
	        jScope.writeLine(out, "Scope.print_event: ", print_event);		    
        
	        out.println("Scope.columns: " + columns);

	        jScope.writeLine(out, "Scope.fast_network_access: ", ""+fast_network_access);		    
	        jScope.writeLine(out, "Scope.data_server_address: ",    data_server_address);		    

            main_scope.font_dialog.toFile(out, "Scope.font");
	        main_scope.color_dialog.toFile(out, "Scope.item_color_");
	        main_scope.setup_default.toFile(out, "Scope.global_1_1.");
            main_scope.pub_var_diag.toFile(out, "Scope.public_variable_");

				    	    	    
	        for(int i = 0, c = 1, k = 0; i < columns; i++,c++)
	        {
		        jScope.writeLine(out, "Scope.rows_in_column_" + c + ": " , ""+rows[i]);
    		    for(int j = 0, r = 1; j < rows[i]; j++, r++)
		        {		
		            out.println("\n");

		            jScope.writeLine(out, "Scope.plot_" + r + "_" + c + ".height: "          , ""+w[k].getSize().height );
		            jScope.writeLine(out, "Scope.plot_" + r + "_" + c + ".grid_mode: "       , ""+w[k].grid_mode);
                    if(w[k].wi != null)
		                w[k].wi.toFile(out, "Scope.plot_" + r + "_" + c + ".", main_scope);
		            k++;
		        }
	        }
	    
	        for(int i = 1, k = 0, pos = 0; i < columns; i++)
	        {
		        pos += (int)(((float)w[k].getSize().width/ width) * 1000.);
		        jScope.writeLine(out, "Scope.vpane_" + i + ": " , ""+pos);
		        k += rows[i-1];
	        } 	    

	        if(is_html)
	        {         
	            out.write("\"></APPLET>\n");
	            out.write("</BODY>\n");
	            out.write("</HTML>\n");
	        }
        
    }

    public void toURL(String conf_file, boolean is_html)
    {
         PrintWriter out;

         try {
        
            URL save_url = new URL(main_scope.url_path+"jScopeSave");   
            URLConnection save_con = save_url.openConnection();
            save_con.setDoOutput(true);   
          
            out = new PrintWriter(save_con.getOutputStream());
            out.println(conf_file);
	        SaveConfiguration(out, is_html);
	        out.close();

            BufferedReader in = new BufferedReader(
				new InputStreamReader(save_con.getInputStream()));

	        in.close();
	     
          } catch (UnknownHostException e) { 
            System.out.println("Unknown host -- check for network or DNS problems."); 
          } catch (MalformedURLException e) { 
            System.out.println("PROBLEM: Bad or no URL received from the server."); 
          } catch (IOException e) { 
            System.out.println("IO exception: " + e.getMessage()); 
          } 

    }

    public void toFile(String conf_file, boolean is_html)    
    {
        PrintWriter out;
	    File f;	
	
//	    if(System.getProperty("os.name").equals("Mac OS"))
//	    {	
//	        MRJFileUtils.setDefaultFileType(new MRJOSType("TEXT"));
//	        MRJFileUtils.setDefaultFileCreator(new MRJOSType("JSCP"));
//	    } 
	
	    f = new File(conf_file);    
	    if(f.exists()) f.delete();   
	
        try {
	        //out = new BufferedWriter(new FileWriter(f));
	        out = new PrintWriter(new FileWriter(f));
	        SaveConfiguration(out, is_html);
	        out.close();
    	} catch(IOException e) {
	        System.out.println("Errore : " + e);
	    }		
    }    
}

class Button3Menu extends PopupMenu implements ActionListener, ItemListener {
	MultiWaveform   wave;
	MenuItem setup, autoscale, autoscaleY, autoscaleAll, autoscaleAllY,
		 allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
		 resetScales, resetAllScales, refresh, saveAsText, selectWave,
		 legend, remove_legend, remove_panel;
	Menu signalList, markerList, colorList, markerStep;
	CheckboxMenuItem interpolate_f;
	SetupDataDialog sd;
	int curr_x, curr_y;
	Setup controller;

    public Button3Menu(Frame pw, SetupDataDialog _sd, Setup _controller)
    {
	sd = _sd;
	controller = _controller;
	
	add(setup = new MenuItem("Setup data source..."));
	setup.addActionListener(this);
	add(selectWave = new MenuItem("Select wave panel"));
	selectWave.addActionListener(this);
	add(legend = new MenuItem("Position legend"));
	legend.addActionListener(this);
	legend.setEnabled(false);
	add(remove_legend = new MenuItem("Remove legend"));
	remove_legend.setEnabled(false);
	remove_legend.addActionListener(this);
	add(remove_panel = new MenuItem("Remove panel"));
	remove_panel.setEnabled(false);
	remove_panel.addActionListener(this);
	
	add(new MenuItem("-"));
	add(signalList = new Menu("Signals"));
	signalList.setEnabled(false);	
	add(markerList = new Menu("Markers"));	
	CheckboxMenuItem ob;
    for(int i = 0; i < jScope.markerList.length; i++)
    {
        markerList.add(ob = new CheckboxMenuItem(jScope.markerList[i]));
        ob.addItemListener(this);
    }
	markerList.setEnabled(false);	    
	add(markerStep = new Menu("Markers step"));	
    for(int i = 0; i < jScope.markerStepList.length; i++)
    {
        markerStep.add(ob = new CheckboxMenuItem(""+jScope.markerStepList[i]));
        ob.addItemListener(this);
    }
	markerStep.setEnabled(false);
	add(colorList = new Menu("Colors"));
	colorList.setEnabled(false);
    add(interpolate_f = new CheckboxMenuItem("Interpolate", false));
	interpolate_f.setEnabled(false);
    interpolate_f.addItemListener(this);
	add(new MenuItem("-"));
	add(autoscale = new MenuItem("Autoscale"));
	autoscale.addActionListener(this);
	add(autoscaleY = new MenuItem("Autoscale Y"));
	autoscaleY.addActionListener(this);
	add(autoscaleAll = new MenuItem("Autoscale all", new MenuShortcut(KeyEvent.VK_B)));
	autoscaleAll.addActionListener(this);
	add(autoscaleAllY = new MenuItem("Autoscale all Y", new MenuShortcut(KeyEvent.VK_Y)));
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
	add(new MenuItem("-"));
	add(saveAsText = new MenuItem("Save as text ..."));
	saveAsText.addActionListener(this);
    }
	
	private void SelectListItem(Menu m, int idx)
	{
        for(int i = 0; i < m.getItemCount(); i++)
	        ((CheckboxMenuItem)m.getItem(i)).setState(false);
	    if(idx < m.getItemCount())
            ((CheckboxMenuItem)m.getItem(idx)).setState(true);
	}
	
	private int GetSelectedItem(Menu m, Object ob)
	{
	    int idx = 0;
	    for(int i = 0; i < m.getItemCount(); i++)
	    {
	        if(m.getItem(i) == ob) {
	            idx = i;
	            continue;
	        }
	        if(((CheckboxMenuItem)m.getItem(i)).getState())
	            ((CheckboxMenuItem)m.getItem(i)).setState(false);
	    }
	    return idx;
	}
    public void Show(MultiWaveform w, int x, int y)
    {
        if(controller.sel_wave == w)
            selectWave.setLabel("Deselect wave panel");
        else
            selectWave.setLabel("Select wave panel");

        remove_panel.setEnabled(controller.waves.length > 1);
        
        signalList.removeAll();
        if(w.wi != null && w.wi.num_waves != 0)
        {
            String s_name[] = w.wi.getSignalsName();
            boolean s_state[] = w.wi.getSignalsState();


            
            boolean state = ((w.mode == Waveform.MODE_POINT && controller.signal_idx != -1) || 
                                w.num_signals == 1);
            
            markerList.setEnabled(state);
            colorList.setEnabled(state);	
            interpolate_f.setEnabled(state);
            if(state) {
                if(w.num_signals == 1)
                    controller.signal_idx = 0;
                interpolate_f.setState(w.wi.interpolates[controller.signal_idx]);
                boolean state_m = state && (w.wi.markers[controller.signal_idx] != Waveform.NONE 
                                        && w.wi.markers[controller.signal_idx] != Waveform.POINT);
                markerStep.setEnabled(state_m);
            } else
                markerStep.setEnabled(false);
            
            if(state)
            {
                SelectListItem(markerList, w.wi.markers[controller.signal_idx]);
                int st;
                for(st = 0; st < jScope.markerStepList.length; st++)
                    if(jScope.markerStepList[st] == w.wi.markers_step[controller.signal_idx])
                        break;
                SelectListItem(markerStep, st);
                        
                colorList.removeAll();

	            String[] colors_name = controller.main_scope.color_dialog.GetColorsName();
	            CheckboxMenuItem ob = null;
	            if(colors_name != null)
	            {
	                for(int i = 0; i < colors_name.length; i++) {
                        colorList.add(ob = new CheckboxMenuItem(colors_name[i]));
                        ob.addItemListener(this);
                    }
	            }
                SelectListItem(colorList, w.wi.colors_idx[controller.signal_idx]);

            }
            
            CheckboxMenuItem ob;
            if(s_name != null)
            {
                signalList.setEnabled(s_name.length != 0);
	            legend.setEnabled(s_name.length != 0);
                for(int i = 0; i < s_name.length; i++)
                {
                    signalList.add(ob = new CheckboxMenuItem(s_name[i]));
                    ob.setState(s_state[i]);
                    ob.addItemListener(this);
                }
            }
            
            if(w.wi.make_legend)
            	remove_legend.setEnabled(true);
        } else {
            signalList.setEnabled(false);
            markerList.setEnabled(false);
            colorList.setEnabled(false);	
            interpolate_f.setEnabled(false);
            markerStep.setEnabled(false);
	        legend.setEnabled(false);
            remove_legend.setEnabled(false);
        }
        
        
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
//	    wave.repaint();
	}
	if(target == (Object)autoscaleY)
	{
	    wave.AutoscaleY();
//	    wave.repaint();
	}
	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
//	    wave.repaint();
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
//	    wave.repaint();
	}
	if(target == (Object)resetAllScales)
	{
	    controller.ResetAllScales();	
	}
	if(target == (Object)refresh)
	{
	    controller.Refresh(wave, "");
	   // wave.repaint();	
	}
	if(target == (Object)setup) {
	    if(wave.mode == Waveform.MODE_POINT && controller.signal_idx != -1)
	    {
	        sd.selectSignal(controller.signal_idx);
	    } else
	        if(wave.num_signals > 0)
	            sd.selectSignal(0);
	    
	    sd.Show(wave, controller);
	}

	if(target == (Object)saveAsText) {
	    controller.SaveAsText(wave);
	}
       
	if(target == (Object)selectWave) {
	    if(controller.sel_wave != null)
	        controller.sel_wave.DeselectWave();
	    if(controller.sel_wave == wave)
	    {
	        controller.sel_wave = null;
	        return;
	    }
	    controller.sel_wave = wave;
	    wave.SelectWave();
	}
	
	if(target == legend)
	{
	    wave.wi.make_legend = true;
	    wave.wi.legend_x = curr_x;
	    wave.wi.legend_y = curr_y;
	    wave.SetLegend(new Point(curr_x, curr_y));
	}
	if(target == remove_legend)
	{
	    wave.wi.make_legend = false;
	    wave.RemoveLegend();
	    remove_legend.setEnabled(false);
	}

	if(target == remove_panel)
	{
	    controller.RemovePanel(wave);
	}
       
       
    }
    
    public void itemStateChanged(ItemEvent e)
    {
	    Object target = e.getSource();
	    
	    if(target instanceof CheckboxMenuItem)
	    {
	        CheckboxMenuItem cb = (CheckboxMenuItem)target;
	        Menu parent = (Menu)cb.getParent();
	        
	        if(target == interpolate_f)
	        {
	            wave.wi.interpolates[controller.signal_idx] = ((CheckboxMenuItem)target).getState(); 
	            wave.Repaint();
	        }
	        
	        if(parent == signalList)
	        {
	            wave.wi.setWaveState(((CheckboxMenuItem)target).getLabel(), 
	                         ((CheckboxMenuItem)target).getState());
	            controller.Refresh(wave, "");
	            //wave.Repaint();
	        }
	        
	        if(parent == markerList)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {       
	                if(wave.wi.markers[controller.signal_idx] == idx)
	                {
	                    cb.setState(true);
	                } else {
	                    wave.wi.markers[controller.signal_idx] = idx;
	                }
	            }
	            wave.Repaint();	                
	        }

	        if(parent == markerStep)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {       
	                if(wave.wi.markers_step[controller.signal_idx] == jScope.markerStepList[idx])
	                {
	                    cb.setState(true);
	                } else {
	                    wave.wi.markers_step[controller.signal_idx] = jScope.markerStepList[idx];
	                }
	            }
	            wave.Repaint();	                
	        }

	        
	        if(parent == colorList)
	        {
	            int idx = 0;
	            idx = GetSelectedItem(parent, cb);
	            if(idx < parent.getItemCount())
	            {       
	                if(wave.wi.colors_idx[controller.signal_idx] == idx)
	                {
	                    cb.setState(true);
	                } else {
	                    wave.wi.colors_idx[controller.signal_idx] = idx;
	                    wave.wi.colors[controller.signal_idx] = controller.main_scope.color_dialog.GetColorAt(idx);
                        wave.SetCrosshairColor(wave.wi.colors[controller.signal_idx]);
	                }
	            }
	            wave.Repaint();	                
	        }
	    }
	}
    
}
