import java.applet.Applet;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import com.apple.mrj.*;

public class Setup extends Object implements WaveSetup {
    Label coords_label; 
    SetupDataDialog sd;	
    Button3Menu menu;	
    public MultiWaveform waves[];
    public int num_waves;
    public jScope main_scope;
    public int wave_idx;
    
    String title, event;
    String data_server_address;
    int num_conf = 0;
    int columns = 1;
    int rows[] = new int[4];
    int prec_rows[] = new int[4];
    int height = 500, width = 700, xpos = 50, ypos = 50;
    boolean fast_network_access = false;
    float height_percent[], width_percent[]; 
    BufferedWriter out;
    boolean modified;



   public Setup(Frame pw, Label label)
    {
	main_scope = (jScope)pw;
	rows[0] = 1;
	prec_rows[0] = 1;
	coords_label = label;
	sd = new SetupDataDialog(pw, "Setup data");
	menu = new Button3Menu(pw, sd, this);
	pw.add(menu);
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
	wave_idx = GetWaveIndex(w);
    	menu.Show((MultiWaveform)w, x, y);
    }

    
    public void SetAllWaveformPointer(int pointer_mode)
    {
	    for(int i = 0; i < waves.length; i++)
	        waves[i].SetMode(pointer_mode);	
    }

    public void RemoveSelection()
    {
	for(int i = 0; i < waves.length; i++)
	    if(waves[i].IsSelected()) 
	    {
		main_scope.wi_source = null;
		waves[i].SetSelected(false);
	    } 		
    }
        
        
    public void WaveCheckError(int k)
    {
	if(waves[k].wi == null || waves[k].wi.shots == null || waves[k].wi.shots[0] == jScope.UNDEF_SHOT) {
	    if(waves[k].wi == null )
		waves[k].SetTitle("Error during evaluate wave");
	    else
		waves[k].SetTitle("Undefined Shot");
	    return;	
	}
	if(waves[k].wi.error != null) {
	    String str;
	    int idx = waves[k].wi.error.indexOf("\n");
	    if(idx != -1)
		str = waves[k].wi.error.substring(0, idx);
	    else
		str = waves[k].wi.error;
	    waves[k].SetTitle(str);
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

    public void DisplayCoords(Waveform w, double curr_x, double curr_y, int sig_idx, boolean broadcast)
    { 
        if(coords_label != null)
	    {   
	        WaveInterface  wi = ((MultiWaveform)w).wi;
	        if(wi.shots != null)
	        {
		        coords_label.setText(SetStrSize("[" + Waveform.ConvertToString(curr_x, false) + ", " 
				 + Waveform.ConvertToString(curr_y, false) + "]", 30) +
		            " Expr : " + wi.in_y[sig_idx] +  
		            " Shot = " + wi.shots[sig_idx]);
	        } else {
		        coords_label.setText(SetStrSize("[" + Waveform.ConvertToString(curr_x, false) + ", " 
				 + Waveform.ConvertToString(curr_y, false) + "]", 30) +
		            " Expr : " + wi.in_y[sig_idx]);  
	        }
	        if(true)
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
	  ((MultiWaveform)dest).wi = main_scope.wi_source;
	  Refresh(dest, "");
    }
   
   public String UpdateWave(MultiWaveform wave)
   {
	WaveInterface wi = wave.wi;
	
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
	for(int i = 0; i < num_waves; i++)
	    if(waves[i] != curr_w)
		waves[i].UpdatePoint(x);
    }
    
    public void AutoscaleAll()
    {
	for(int i = 0; i < num_waves; i++) {
	    waves[i].Autoscale();
	    waves[i].repaint();
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
	    main_scope.updateMainShot();
	    main_scope.setup_default.updateDefaultWI(((MultiWaveform)w).wi);
	    main_scope.updateShotWI(((MultiWaveform)w).wi);	
	    UpdateWave((MultiWaveform)w);
	    WaveCheckError(GetWaveIndex(w));
	    main_scope.SetStatusLabel("Wave row " + p.x + " column "+ p.y + " refreshed");	    
	    w.SetMode(main_scope.wave_mode);
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
    public int GetNumWave()
    {
	int n_wave = 0;
    
	for(int i = 0; i < 4; i++)
	    n_wave += rows[i];
	return n_wave;
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

    public int fromFile(BufferedReader in, String prompt) throws IOException
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
	    
	    if(str.indexOf("Scope.data_server_address:") != -1)
	    {
		data_server_address = str.substring("Scope.data_server_address: ".length(), str.length());
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
	
	if(width_percent != null)
	{
	    //Evaluate real number of columns
	    int r_columns =  0;
	    for(int i = 0; i < 4; i++)
		if(rows[i] != 0)
		    r_columns++;

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
	} else
	    error = 1;
	return error;
    }
    
    public void updateHeight()
    {
	float height = 1.F;
	
	height_percent = new float[waves.length];
    
    	for(int i=0; i < rows[0]; i++)
	    height += waves[i].wi.height;
      
	for(int j = 0, k = 0; j < columns; j++)
	    for(int i = 0; i < rows[j]; i++, k++) {
		if( waves[k].wi.height < RowColumnLayout.MIN_SIZE) {
		    k -= i;
		    for(i = 0; i < rows[j]; i++)
			height_percent[k++] = (float)1./rows[j];			
		    break;
		}
	        height_percent[k] = (float)(waves[k].wi.height/height);
	    }
     }


    public void toFile(String conf_file)    
    {
	MultiWaveform w[] = main_scope.setup.waves;
	int wc_idx, height_w[] = new int[4];
	File f;
	
	
	if(System.getProperty("os.name").equals("Mac OS"))
	{	
	    MRJFileUtils.setDefaultFileType(new MRJOSType("TEXT"));
	    MRJFileUtils.setDefaultFileCreator(new MRJOSType("JSCP"));
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

	    jScope.writeLine(out, "Scope.fast_network_access: ", ""+fast_network_access);		    
	    jScope.writeLine(out, "Scope.data_server_address: ",    data_server_address);		    

	    main_scope.color_dialog.toFile(out, "Scope.item_color_");
	    main_scope.setup_default.toFile(out, "Scope.global_1_1.");	    
				    	    	    
	    for(int i = 0, c = 1, k = 0; i < columns; i++,c++)
	    {
		jScope.writeLine(out, "Scope.rows_in_column_" + c + ": " , ""+rows[i]);
    		for(int j = 0, r = 1; j < rows[i]; j++, r++)
		{		
		    out.newLine();

		    jScope.writeLine(out, "Scope.plot_" + r + "_" + c + ".height: "          , ""+w[k].getSize().height );
		    jScope.writeLine(out, "Scope.plot_" + r + "_" + c + ".grid_mode: "       , ""+w[k].grid_mode);
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
	    
	    

	    out.close();
    	} 
	catch(IOException e) {
	    System.out.println("Errore : " + e);
	}		
    }    
}

class Button3Menu extends PopupMenu implements ActionListener {
	MultiWaveform   wave;
	MenuItem setup, autoscale, autoscaleY, autoscaleAll, autoscaleAllY,
		 allSameScale, allSameXScale, allSameXScaleAutoY, allSameYScale,
		 resetScales, resetAllScales, refresh;
	SetupDataDialog sd;
	int curr_x, curr_y;
	Setup controller;

    public Button3Menu(Frame pw, SetupDataDialog _sd, Setup _controller)
    {
	sd = _sd;
	controller = _controller;
	
	add(setup = new MenuItem("Setup data source..."));
	setup.addActionListener(this);
	add(autoscale = new MenuItem("Autoscale"));
	autoscale.addActionListener(this);
	add(autoscaleY = new MenuItem("Autoscale Y"));
	autoscaleY.addActionListener(this);
	add(autoscaleAll = new MenuItem("Autoscale all"));
	autoscaleAll.addActionListener(this);
	add(autoscaleAllY = new MenuItem("Autoscale all Y"));
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
    }
	
    public void Show(MultiWaveform w, int x, int y)
    {
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
	    wave.repaint();
	}
	if(target == (Object)autoscaleY)
	{
	    wave.AutoscaleY();
	    wave.repaint();
	}
	if(target == (Object)autoscale)
	{
	    wave.Autoscale();
	    wave.repaint();
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
	    wave.repaint();
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
	    sd.Show(wave, controller);
	}
       
    }
}
