import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;

//*****
//Form di visualizzazione della configurazione della
//wave selezionata
//*****
 class SetupDataDialog extends ScopePositionDialog { 
   TextField        title, shot, experiment;
   TextField        x_max, x_min, x_label, y_expr;
   TextField        y_max, y_min, y_label, x_expr;
   Button           browser, error, y_browser, expand;
   Button           ok, cancel, reset, erase, apply, x_browser, add_shot;
   Checkbox	    x_log, y_log, opt_network;
   Checkbox	    title_b, shot_b, experiment_b, x_max_b, x_min_b, 
		    x_label_b, y_max_b, y_min_b, y_label_b;  
   List             sig_list;
   Choice           show_type, color, marker;
   Label            lab;
   SignalError      error_w;
   ErrorMessage	    error_msg;
   javaScope	    main_scope;
   Setup            setup;       
   EvaluateWaveform ew;
   WaveformConf	    wc_default, curr_wc;
   int              sel_signal = -1;
   ExpandExpressionDialog expand_expr;	    
     
   static final int  LINE = 0, POINT = 1, BOTH = 2, NONE = 3;


    public SetupDataDialog(Frame fw, String frame_title) {

      super(fw, frame_title, false);
      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);    

      main_scope = (javaScope)fw;
      error_w = new SignalError(fw);
      error_msg = new ErrorMessage(fw);
      expand_expr = new ExpandExpressionDialog(fw, this);	    
     
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(4, 4, 4, 4);

      setLayout(gridbag);		
      c.insets = insets;
      c.anchor = GridBagConstraints.WEST;	
      c.fill =  GridBagConstraints.NONE;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      title_b = new Checkbox("Title");
      title_b.addItemListener(this);
      gridbag.setConstraints(title_b, c);
      add(title_b);
 
      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 5;	
      title = new TextField(40);
      gridbag.setConstraints(title, c);
      add(title);
      		 	
//      c.anchor = GridBagConstraints.WEST;
      c.gridwidth = GridBagConstraints.REMAINDER;
      c.gridheight = 1;
      expand = new Button("Expand Expr.");	
      expand.addActionListener(this);
      gridbag.setConstraints(expand, c);
      add(expand);	      	

/*
      c.gridwidth = GridBagConstraints.REMAINDER;
      opt_network = new Checkbox("Opt. Network");
      opt_network.setState(false);
      opt_network.addItemListener(this);
      gridbag.setConstraints(opt_network, c);
      add(opt_network);	
*/
      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 1;
      lab = new Label("Y");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.gridwidth = 4;
      y_expr = new TextField(30);
      y_expr.addKeyListener(this);
      gridbag.setConstraints(y_expr, c);
      add(y_expr);

      c.gridwidth = 1;
      error = new Button("Error...");
      error.addActionListener(this);
      gridbag.setConstraints(error, c);
      add(error);
	                
      c.gridwidth = GridBagConstraints.REMAINDER;
      y_log = new Checkbox("Log Scale", null, false); 		 	
      gridbag.setConstraints(y_log, c);
      add(y_log);
       
      c.fill =  GridBagConstraints.NONE;		 	
      c.gridwidth = 1;
      y_label_b = new Checkbox("Y Label");
      y_label_b.addItemListener(this);
      gridbag.setConstraints(y_label_b, c);
      add(y_label_b);	

      y_label = new TextField(25);		 	
      gridbag.setConstraints(y_label, c);
      add(y_label);	

      y_min_b = new Checkbox("Y min");
      y_min_b.addItemListener(this);
      gridbag.setConstraints(y_min_b, c);
      add(y_min_b);	

      y_min = new TextField(10);		 	
      y_min.addKeyListener(this);
      gridbag.setConstraints(y_min, c);
      add(y_min);	

      y_max_b = new Checkbox("Y max");
      y_max_b.addItemListener(this);
      gridbag.setConstraints(y_max_b, c);
      add(y_max_b);	
	 	
      y_max = new TextField(10);
      y_max.addKeyListener(this);
      gridbag.setConstraints(y_max, c);
      add(y_max);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      y_browser = new Button("Browse...");
      y_browser.addActionListener(this);
      gridbag.setConstraints(y_browser, c);
      add(y_browser);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 1;
      lab = new Label("X");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.gridwidth = 5;
      x_expr = new TextField(30);
      x_expr.addKeyListener(this);
      gridbag.setConstraints(x_expr, c);
      add(x_expr);		 	

      c.gridwidth = GridBagConstraints.REMAINDER;
      x_log = new Checkbox("Log Scale", null, false); 		 	
      gridbag.setConstraints(x_log, c);
      add(x_log);

      c.fill =  GridBagConstraints.NONE;		 	
      c.gridwidth = 1;
      x_label_b = new Checkbox("X Label");
      x_label_b.addItemListener(this);
      gridbag.setConstraints(x_label_b, c);
      add(x_label_b);	

      x_label = new TextField(25);		 	
      gridbag.setConstraints(x_label, c);
      add(x_label);	

      x_min_b = new Checkbox("X min");
      x_min_b.addItemListener(this);
      gridbag.setConstraints(x_min_b, c);
      add(x_min_b);	

      x_min = new TextField(10);		 	
      x_min.addKeyListener(this);
      gridbag.setConstraints(x_min, c);
      add(x_min);	

      x_max_b = new Checkbox("X max");
      x_max_b.addItemListener(this);
      gridbag.setConstraints(x_max_b, c);
      add(x_max_b);	
	 	
      x_max = new TextField(10);
      x_max.addKeyListener(this);
      gridbag.setConstraints(x_max, c);
      add(x_max);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      x_browser = new Button("Browse...");
      x_browser.addActionListener(this);
      gridbag.setConstraints(x_browser, c);
      add(x_browser);
           		
      c.gridwidth = 1;
      experiment_b = new Checkbox("Experiment");
      experiment_b.addItemListener(this);
      gridbag.setConstraints(experiment_b, c);
      add(experiment_b);	
      
      experiment = new TextField(25);
      experiment.addKeyListener(this);
      gridbag.setConstraints(experiment, c);
      add(experiment);	

      shot_b = new Checkbox("Shot");
      shot_b.addItemListener(this);      
      gridbag.setConstraints(shot_b, c);
      add(shot_b);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      shot = new TextField(30);
      shot.addKeyListener(this);
      shot.addFocusListener(this);
      gridbag.setConstraints(shot, c);
      add(shot);

      lab = new Label("Signals list");       	
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 6;
      c.gridheight = 3;
      sig_list = new List(6, false);
      sig_list.addItemListener(this);
      sig_list.addKeyListener(this);
      gridbag.setConstraints(sig_list, c);
      add(sig_list);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 1;
      c.gridheight = 1;	

      c.gridwidth = GridBagConstraints.REMAINDER;
      show_type = new Choice();
      show_type.addItem("Line"); 
      show_type.addItem("No Line"); 
      show_type.addItemListener(this);
      gridbag.setConstraints(show_type, c);
      add(show_type);	

      color = new Choice();
      SetColorList();
      /*
      color.addItem("Black");	
      color.addItem("Blue");	
      color.addItem("Cyan");	
      color.addItem("DarkGray");	
      color.addItem("Gray");	
      color.addItem("Green");	
      color.addItem("LightGray");	
      color.addItem("Magenta");	
      color.addItem("Orange");
      color.addItem("Pink");	
      color.addItem("Red");	
      color.addItem("Yellow");
      */	
      color.addItemListener(this);
      gridbag.setConstraints(color, c);
      add(color);	

      marker = new Choice();
      marker.addItem("No Marker"); 
      marker.addItem("Square"); 
      marker.addItem("Circle"); 
      marker.addItem("Cross");
      marker.addItem("Triangle");      
      marker.addItem("Point"); 
      marker.addItemListener(this);
      gridbag.setConstraints(marker, c);
      add(marker);
      	
      c.fill =  GridBagConstraints.NONE;     
      c.anchor = GridBagConstraints.CENTER;
      c.gridwidth = 1;
      ok = new Button("Ok");
      ok.addActionListener(this);
      gridbag.setConstraints(ok, c);
      add(ok);	
      	
      apply = new Button("Apply");
      apply.addActionListener(this);
      gridbag.setConstraints(apply, c);
      add(apply);	

      c.gridwidth = 2;
      reset = new Button("Reset");
      reset.addActionListener(this);
      gridbag.setConstraints(reset, c);
      add(reset);	

      erase = new Button("Erase");
      erase.addActionListener(this);
      gridbag.setConstraints(erase, c);
      add(erase);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      cancel = new Button("Cancel");
      cancel.addActionListener(this);
      gridbag.setConstraints(cancel, c);
      add(cancel);

   }
   
   public void SetColorList()
   {
	String[] colors_name = main_scope.color_dialog.GetColorsName();
	color.removeAll();
	if(colors_name != null)
	{
	    for(int i = 0; i < colors_name.length; i++)
		color.addItem(colors_name[i]);
	}
   }   

   public void Show(MultiWaveform w, Setup _setup)
   {
      setup = _setup;      
      pack();
      WaveformConf wc = setup.GetWaveformConf();

      sel_signal = -1;
      curr_wc = wc;
//      if(wc.num_expr != 0) sel_signal = 0;	
//      sd_block = new EvaluateWaveform(setup.main_scope);
//      sd_block.wave = w;
      ew = setup.main_scope.ew;
      ew.wave = w;
      wc_default = setup.main_scope.sc.gwc;
      putWindowSetup(wc);       		
      setPosition(w.getParent());	
      show();

   }

   private void resetDefaultFlags()
   {
	title_b.setState(false);
	title.setEditable(true); 
	shot_b.setState(false); 
	shot.setEditable(true); 
	experiment_b.setState(false); 
	experiment.setEditable(true); 
	x_max_b.setState(false); 
	x_max.setEditable(true); 
	x_min_b.setState(false); 
	x_min.setEditable(true); 
	x_label_b.setState(false); 
	x_label.setEditable(true); 
	y_max_b.setState(false); 
	y_max.setEditable(true); 
	y_min_b.setState(false); 
	y_min .setEditable(true); 
	y_label_b.setState(false);  
	y_label.setEditable(true); 
   } 
     
   private void setDefaultFlags(int flags)
   {
   
	for(int i=0; i < 32; i++)
	{
	    switch(i)
	    {
		case WaveformConf.B_title:
		    title_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_shot:
		    shot_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_exp:
		    experiment_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_x_max:
		    x_max_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_x_min:
		    x_min_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_x_label:
		    x_label_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_y_max:
		    y_max_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_y_min:
		    y_min_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveformConf.B_y_label:
		    y_label_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break;  
	    }
	}       
   }
   
   private int getDefaultFlags()
   {
       int value = 0, cc = 0;
       
	if  (title_b.getState())  value |= 1<<WaveformConf.B_title; 
			     else value &= ~(1<<WaveformConf.B_title); 
	if  (shot_b.getState()  )    value |= 1<<WaveformConf.B_shot  ; 
				else value &= ~(1<<WaveformConf.B_shot); 
	if  (experiment_b.getState() ) value |= 1<<WaveformConf.B_exp ; 
				       else value &= ~(1<<WaveformConf.B_exp); 
	if  (x_max_b.getState() )  value |= 1<<WaveformConf.B_x_max ; 
				   else value &= ~(1<<WaveformConf.B_x_max); 
	if  (x_min_b.getState() )   value |= 1<<WaveformConf.B_x_min ;
				    else value &= ~(1<<WaveformConf.B_x_min); 
	if  (x_label_b.getState() )   value |= 1<<WaveformConf.B_x_label ; 
				      else value &= ~(1<<WaveformConf.B_x_label); 
	if  (y_max_b.getState() )  value |= 1<<WaveformConf.B_y_max ; 
				   else value &= ~(1<<WaveformConf.B_y_max); 
	if  (y_min_b.getState() )   value |= 1<<WaveformConf.B_y_min ; 
				    else value &= ~(1<<WaveformConf.B_y_min); 
	if  (y_label_b.getState() )   value |= 1<<WaveformConf.B_y_label ; 
				      else value &= ~(1<<WaveformConf.B_y_label);
	return (value);  
   }
      
   private void removeSignalSetup()
   {
      int idx = sel_signal;
      int i, j, sig_idx, start_idx, end_idx;
      int num_shot = ew.num_shot;
      int num_signal = ew.num_signal;  		
      WaveSetupData[] ws_new;
          	

      if(idx != -1) 
      {
	if(num_shot == 0) {
	    sig_idx = 1;
	    start_idx = idx;
	} else  {
	    sig_idx = num_shot;				
	    start_idx = (idx/num_shot) * num_shot; // Divisione intera
	}
	ws_new = new WaveSetupData[ew.num_signal - sig_idx];	

	if(num_shot != 0)
	    start_idx = (idx/num_shot) * num_shot; // Divisione intera
	end_idx   = start_idx + num_shot; 

	for (i = 0,  j = 0 ; i < num_signal; i++)
	  if(i < start_idx || i >= end_idx)  
             ws_new[j++] = ew.signals[i];
	  else   
	     sig_list.remove(start_idx + 1);
	     
	ew.signals     = ws_new;
	ew.num_signal -= sig_idx; 
      }
      sel_signal = -1;
      sig_list.select(sel_signal + 1);
      x_expr.setText("");
      y_expr.setText("");
      
  }

   public void putWindowSetup(WaveformConf wc)      
   { 
      int shots[] = new int[wc.num_shot];

	if(wc == null) {
	    eraseForm();	    
	    return;
	}

	setDefaultFlags(wc.defaults);
	defaultButtonOperation(title,      title_b.getState(),      wc_default.title,      curr_wc.title);
	defaultButtonOperation(shot,       shot_b.getState(),       wc_default.shot_str,   curr_wc.shot_str);
	defaultButtonOperation(experiment, experiment_b.getState(), wc_default.experiment, curr_wc.experiment);
	defaultButtonOperation(x_max,      x_max_b.getState(),      wc_default.x_max,      curr_wc.x_max);
	defaultButtonOperation(x_min,      x_min_b.getState(),      wc_default.x_min,      curr_wc.x_min);
	defaultButtonOperation(x_label,    x_label_b.getState(),    wc_default.x_label,    curr_wc.x_label);
	defaultButtonOperation(y_max,      y_max_b.getState(),      wc_default.y_max,      curr_wc.y_max);
	defaultButtonOperation(y_min,      y_min_b.getState(),      wc_default.y_min,      curr_wc.y_min);
	defaultButtonOperation(y_label,    y_label_b.getState(),    wc_default.y_label,    curr_wc.y_label);
	
	
	if(setup.main_scope.IsShotDefined() && wc.useDefaultShot())
	{
	    shot.setForeground(Color.red);
	    shot.setText(setup.main_scope.GetMainShot());
	}		    
	    
	x_log.setState(wc.x_log);
	y_log.setState(wc.y_log);
	
	//if(shot.getText() != null && shot.getText().length() != 0)
	    ew.initSignals(wc, shot.getText());
	     	
        x_expr.setText("");
        y_expr.setText("");
	sel_signal = -1;
    	signalListRefresh();  
   }

   public void putSignalSetup(WaveSetupData ws)
   {
      if(ws.x_expr != null)
	x_expr.setText(ws.x_expr);
      else
	x_expr.setText("");      
      if(ws.y_expr != null)
	y_expr.setText(ws.y_expr);
      else
	y_expr.setText("");      
      show_type.select((ws.interpolate ? 0 : 1));
      marker.select(ws.marker);		 
      color.select(ws.color_idx);
      if(error_w.isVisible())
	error_w.setError(ws);	
   }	

   public WaveSetupData getSignalSetup()
   {
      WaveSetupData ws = new WaveSetupData(); 

      ws.x_expr        = new String(x_expr.getText());
      ws.y_expr        = new String(y_expr.getText());
      ws.interpolate   = (show_type.getSelectedIndex() == 0 ? true : false);
      ws.marker        = marker.getSelectedIndex();	
      ws.color_idx     = color.getSelectedIndex();       

      return ws; 	 
   }
  
   private WaveSetupData[] addSignalSetup(WaveSetupData ws)
   { 
      WaveSetupData[] ws_new = new WaveSetupData[ew.num_signal + 1];
     
      for(int i = 0; i < ew.num_signal; i++)
	ws_new[i] = ew.signals[i];
      ws_new[ew.num_signal++] = ws;
      
      ew.signals = ws_new;

      return ew.signals;
   }

   private void addSignals()
   {
      int idx, color_idx = 0;
      WaveSetupData ws;
      
      if(y_expr.getText().length() == 0)
	return;
	
      ws = getSignalSetup();
      idx = ew.findSignalSetup(ws);
      if( idx == -1) {
      
	String x_e =  new String(x_expr.getText() == null ? "" : x_expr.getText()) ; 
	String y_e =  new String(y_expr.getText());

	if(ew.num_shot != 0)
	{
	    for (int i = 0; i < ew.num_shot; i++, ws = getSignalSetup())
    	    {
		ws.shot = ew.shots[i];
		ws.color_idx = color_idx;
		color_idx = (color_idx + 1) % setup.main_scope.color_dialog.GetNumColor();
		addSignalSetup(ws);
		signalListAdd(ws);
	      }	
	 } else {
    	    ws.shot = WaveSetupData.UNDEF_SHOT;
	    addSignalSetup(ws);
	    signalListAdd(ws);
	 } 
	 sel_signal = ew.findSignalSetup(ws);
	 sig_list.select(sel_signal + 1);
	 putSignalSetup(ws);
      }	 
   }

   private void updateSignals()
   {
      int idx = sel_signal;
      int i, j, sig_idx, start_idx, end_idx;
      int num_shot = ew.num_shot;
      int num_signal = ew.num_signal; 
      
      if(ew.findSignalSetup(getSignalSetup()) != -1)
	    return;	
          	
      if(idx != -1) 
      {
	if(num_shot == 0) {
	    sig_idx = 1;
	    start_idx = idx;
	} else  {
	    sig_idx = num_shot;				
	    start_idx = (idx/num_shot) * num_shot; // Divisione intera
	}

	if(num_shot != 0)
	    start_idx = (idx/num_shot) * num_shot; // Divisione intera
	end_idx   = start_idx + num_shot; 

	
	for (i = 0,  j = 0 ; i < num_signal; i++)
	  if(i >= start_idx && i < end_idx)  {
             ew.signals[i].x_expr = x_expr.getText();	        
             ew.signals[i].y_expr = y_expr.getText();
	     signalListReplace(i, ew.signals[i]);
	  }		
	 sig_list.select(start_idx);
	 sel_signal = start_idx;
      }
   }
         
   private void signalListRefresh()
   {
	if(sig_list.getItemCount() != 0)
	    sig_list.removeAll();
	sig_list.add("Select this item to add new expression");
	for(int i = 0; i < ew.num_signal; i++)
	    signalListAdd(ew.signals[i]);
	sig_list.select(sel_signal + 1);
   } 

   private void signalListAdd(WaveSetupData ws)
   {
	if(ws.shot != WaveSetupData.UNDEF_SHOT)
	{
	    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		sig_list.add("Y : " + ws.y_expr + " Shot : " + ws.shot);
	    else
		sig_list.add("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : " + ws.shot);	
    	} else {
    	    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		sig_list.add("Y : " + ws.y_expr + " Shot : Undef");
	    else
	    	sig_list.add("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : Undef");	
	}
  }
   private void signalListReplace(int idx, WaveSetupData ws)
   {
	if(ws.shot != WaveSetupData.UNDEF_SHOT)
	{
	    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		sig_list.replaceItem("Y : " + ws.y_expr + " Shot : " + ws.shot, idx);
	    else
		sig_list.replaceItem("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : " + ws.shot, idx);	
    	} else {
    	    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		sig_list.replaceItem("Y : " + ws.y_expr + " Shot : Undef", idx);
	    else
	    	sig_list.replaceItem("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : Undef", idx);	
	}
  }

  public void eraseForm()
  {
	title.setText("");
	x_expr.setText("");
	x_label.setText("");
	x_max.setText("");
	x_min.setText("");
	y_expr.setText("");
	y_max.setText("");
	y_min.setText("");
	y_label.setText("");
	experiment.setText("");
	shot.setText("");
	resetDefaultFlags();
	ew.num_signal = 0;
	signalListRefresh();
   } 
   
   
   public WaveformConf saveConfiguration()
   {
      WaveformConf wc = new WaveformConf();

      wc.Copy(curr_wc);
            
      wc.num_shot = ew.num_shot;
      
      if(ew.num_shot != 0)
	wc.num_expr = ew.num_signal/ew.num_shot;
      else
	wc.num_expr = ew.num_signal;
      
      if(!experiment_b.getState())
	wc.experiment   = new String(experiment.getText());
      if(!shot_b.getState() && shot.getForeground() == Color.black)
	wc.shot_str     = new String(shot.getText());
      if(!x_max_b.getState())
	wc.x_max        = new String(x_max.getText());
      if(!x_min_b.getState())
	wc.x_min        = new String(x_min.getText());
      if(!y_max_b.getState())
	wc.y_max        = new String(y_max.getText());
      if(!y_min_b.getState())
	wc.y_min        = new String(y_min.getText());
      if(!title_b.getState())
	wc.title        = new String(title.getText());
      if(!x_label_b.getState())
	wc.x_label      = new String(x_label.getText());
      if(!y_label_b.getState())
	wc.y_label      = new String(y_label.getText());
	
      wc.x_log        = x_log.getState();
      wc.y_log        = y_log.getState();
//    wc.opt_network  = opt_network.getState();
      wc.x_expr	      = new String[ew.num_signal];
      wc.y_expr	      = new String[ew.num_signal];
      wc.up_err	      = new String[ew.num_signal];
      wc.low_err      = new String[ew.num_signal];
            
      wc.markers      = new int[ew.num_signal];

      wc.colors_idx   = new int[ew.num_signal];
      wc.interpolates = new boolean[ew.num_signal];
      for(int j = 0, k = 0;  j < wc.num_expr; j++)
      { 
	 if(ew.signals[k].x_expr != null)
	    wc.x_expr[j] = new String(ew.signals[k].x_expr);
	 if(ew.signals[k].y_expr != null)
	    wc.y_expr[j] = new String(ew.signals[k].y_expr);
	 if(ew.signals[k].up_err != null)
	    wc.up_err[j] = new String(ew.signals[k].up_err);
	 if(ew.signals[k].low_err != null)
	    wc.low_err[j] = new String(ew.signals[k].low_err);

	 for(int i=0; i < ew.num_shot; i++)
	 {
	    wc.interpolates[k] = ew.signals[k].interpolate;
	    wc.markers[k]      = ew.signals[k].marker;
	    wc.colors_idx[k]   = ew.signals[k].color_idx;
    	    k++;
	 }
      }
      wc.defaults = getDefaultFlags();
      wc.modified = !wc.equals(curr_wc);
      if(wc.modified) {
	main_scope.sc.modified = true;
	main_scope.setWindowTitle();
      }
      curr_wc = wc;        
      return (setup.SaveWaveformConf(wc));
   } 		  	

   private int applyWaveform(boolean ok_flag)
   {
	int error = 0;
	boolean def_exp = true, def_shot = true;


    	main_scope.SetStatusLabel("");
	ew.wave.SetMode(Waveform.MODE_WAIT);
				
	if(experiment.getText() == null || experiment.getText().trim().length() == 0)
	    def_exp = false;
	
	if(shot.getText() == null || shot.getText().trim().length() == 0)
	    def_shot = false;
	
	if(def_exp ^ def_shot)
	{
	    if(!def_shot)
		error_msg.addMessage("Experiment defined but undefined shot\n");
	    if(!def_exp)
		error_msg.addMessage("Shot defined but undefined experiment\n");
	    error = 1;
	}
	
	ew.signalsRefresh(shot.getText());
	
	if(sel_signal == -1)
	    addSignals();
	else
	    updateSignals();

	WaveformConf wc = saveConfiguration();
	
	if(error == 0) {
	
	    main_scope.SetStatusLabel("Update signals for shots " + shot.getText());
    
    	    if(ew.num_signal != 0) {
//		ew.signalsRefresh(shot.getText());
		signalListRefresh();
	    }
		if(wc.modified)	    		    
		    ew.UpdateWave(wc);
		else
		    ew.RefreshWave(wc);
		
		wc.modified = true;	    
		if(ew.wave.wi.error != null)
		{
		    error = 1;
		    error_msg.addMessage(ew.wave.wi.error);
		    ew.RefreshWave(wc);
		}
	    	 
	}
	
	if(error == 0 && ok_flag)
	    wc.modified = wc.useDefaultShot();

	if(error != 0)
	    wc.modified = true;	    		

	
	ew.wave.SetMode(main_scope.wave_mode);
	main_scope.SetStatusLabel("Wave is up to date");
	
	return error;
   }
	    	
   public void actionPerformed(ActionEvent e)
   {
      Object ob = e.getSource();

      if(ob == erase) {
	eraseForm();
      }
		
      if(ob == cancel)
	setVisible(false);
	
      if(ob == apply || ob == ok)
      {
	
	if(applyWaveform(ob == ok) == 1)
	    error_msg.showMessage();
	if(ob == ok)
	    setVisible(false);		
      }
      
      if(ob == reset)
	putWindowSetup(setup.GetWaveformConf()); 	

      if(ob == error) {
	 if(sel_signal == -1)
	    return; 
	 error_w.setError(ew.signals[sel_signal]);	
	 error_w.pack();
	 error_w.setPosition(this);
	 error_w.show();
      }
      
      if(ob == expand)
      {	
	expand_expr.setExpressionString(x_expr.getText(), y_expr.getText());
	expand_expr.pack();
	expand_expr.setPosition(this);
	expand_expr.show();
      }

   }

   public void keyPressed(KeyEvent e)
   {
      Object ob = e.getSource();
      char key  = e.getKeyChar();
        
     if(key == KeyEvent.CHAR_UNDEFINED)
	 return;		  	      	

      if(key == KeyEvent.VK_DELETE)
      {
	 if(ob == sig_list)
           removeSignalSetup();	
      }

      if(key == KeyEvent.VK_ENTER)
      { 
	 if(ob == y_expr || ob == x_expr || ob == shot || ob == experiment) {
	    ew.evaluateMainShot(setup.main_scope.GetMainShot());
    	    if(sel_signal == -1)	    
		addSignals();
	    else
		updateSignals();	    	
	    if(ew.num_signal != 0) {
		ew.signalsRefresh(shot.getText());
	        signalListRefresh();
	    }	    
         } 
      }
      
      if(ob instanceof TextField)
      {
	 if(ob == x_max || ob == y_max || ob == x_min || ob == y_min || ob == shot)
	 {
	     Character ch = new Character((char)key);
             if(!ch.isDigit((char)key) && 
		key != KeyEvent.VK_DELETE && 
	        key != '.' && key != '+' && key != '-')
		    return;
         }
      } 
   }
   
   private void  defaultButtonOperation(TextField text, boolean state, String def, String cur)
   {
	if(state) {
	    text.setForeground(Color.blue);
	    text.setEditable(false);
	    if(def != null && def.trim().length() != 0)	    
		text.setText(def);
	    else 
		text.setText("");	    
	} else {
	    text.setForeground(Color.black);		
	    text.setEditable(true);
	    if(cur != null && cur.trim().length() != 0)
		text.setText(cur);
	    else {
		text.setText("");
	    }
	}   
   }

   private void  defaultButtonChange(Object ob)
   {
	if(ob == title_b)
	    defaultButtonOperation(title, title_b.getState(),  wc_default.title, curr_wc.title);
	if(ob == shot_b)
	    defaultButtonOperation(shot, shot_b.getState(),  wc_default.shot_str, curr_wc.shot_str);
	if(ob == experiment_b)
	    defaultButtonOperation(experiment, experiment_b.getState(), wc_default.experiment, curr_wc.experiment);
	if(ob == x_max_b)
	    defaultButtonOperation(x_max, x_max_b.getState(), wc_default.x_max, curr_wc.x_max);
	if(ob == x_min_b)
	    defaultButtonOperation(x_min, x_min_b.getState(),wc_default.x_min, curr_wc.x_min);
	if(ob == x_label_b)
	    defaultButtonOperation(x_label, x_label_b.getState(), wc_default.x_label, curr_wc.x_label);
	if(ob == y_max_b)
	    defaultButtonOperation(y_max, y_max_b.getState(), wc_default.y_max, curr_wc.y_max);
	if(ob == y_min_b)
	    defaultButtonOperation(y_min, y_min_b.getState(), wc_default.y_min, curr_wc.y_min);
	if(ob == y_label_b)
	    defaultButtonOperation(y_label, y_label_b.getState(), wc_default.y_label, curr_wc.y_label);

   }

   public void itemStateChanged(ItemEvent e)
   {
	Object ob = e.getSource();
	
	if(ob == sig_list)
	{
           sel_signal = sig_list.getSelectedIndex() - 1;
	   if(sel_signal >= 0)
	       putSignalSetup(ew.signals[sel_signal]);
	   else {
		x_expr.setText("");
		y_expr.setText("");
	    }
	} 

	if(ob instanceof Checkbox)
	    defaultButtonChange(ob);

    	if(sel_signal == -1)
	    return;
	    
	if(ob == marker) {
	    ew.signals[sel_signal].marker = marker.getSelectedIndex();
	}	

	if(ob == show_type) {
	    ew.signals[sel_signal].interpolate = show_type.getSelectedIndex() == 0 ? true : false;
	}	

	if(ob == color) {
	    ew.signals[sel_signal].color_idx = color.getSelectedIndex();       
	}
	
    }
    
    public void focusGained(FocusEvent e)
    {
	Object ob = e.getSource();
/*		
	if(ob == shot) {
	    if(shot_b.getState()) {
	      ((TextField)ob).setForeground(Color.blue);
	      ((TextField)ob).setText(wc_default.shot_str);
	    }	    
	    else
	    {
	      ((TextField)ob).setForeground(Color.black);
	      ((TextField)ob).setText(curr_wc.shot_str);
	    }
	}
*/  
    }    
}

