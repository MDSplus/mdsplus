import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
import java.util.Vector;

//*****
//Form di visualizzazione della configurazione della
//wave selezionata
//*****
 class SetupDataDialog extends ScopePositionDialog {
 
 
   TextField		    title, shot, experiment;
   TextField		    x_max, x_min, x_label, y_expr, signal_label;
   TextField		    y_max, y_min, y_label, x_expr;
   TextField            upd_event, def_node;
   private Button       browser, error, y_browser, expand;
   private Button       ok, cancel, reset, erase, apply, x_browser, add_shot;
   private Checkbox	    x_log, y_log, opt_network;
   private Checkbox	    title_b, shot_b, experiment_b, x_max_b, x_min_b, 
			            x_label_b, y_max_b, y_min_b, y_label_b, def_node_b,
			            upd_event_b;  
   private Choice       show_type;
   private Label	    lab;
   private SError	    error_w;
   private ErrorMessage	error_msg;
   private jScope	    main_scope;
   private Setup        setup;       
   private ExpandExp    expand_expr;
   private SList	    signalList;
   private MultiWaveform    wave;	    
   public  WaveInterface    wi;
   //static  BrowseSig	    browse_x_w, browse_y_w;
  
     
   static final int  LINE = 0, POINT = 1, BOTH = 2, NONE = 3;
   static final int  BROWSE_X = 0, BROWSE_Y = 1;
    
	    
   class Data {
  
    String  label;
	String  x_expr;
	String  y_expr;
	int     shot;
	int     color_idx;
	boolean interpolate;
	int     marker;
	int     marker_step;
	String  up_err;
	String  low_err;
	

	public boolean equals(Data ws)
	{
	    
	    if(x_expr != null) {
	        if(!x_expr.equals(ws.x_expr))
	            return false;
	    } else
	        if(ws.x_expr != null && ws.x_expr.length() != 0)
	            return false;

	    if(y_expr != null) {
	        if(!y_expr.equals(ws.y_expr))
	            return false;
	    } else
	        if(ws.y_expr != null && ws.y_expr.length() != 0)
	            return false;
	    
	    if(label != null) {
	        if(!label.equals(ws.label))
	            return false;
	    } else
	        if(ws.label != null  && ws.label.length() != 0)
	            return false;
	    
	    return true;
	    
/*	    
	    if(x_expr != null && y_expr != null){
		    if(!y_expr.equals(ws.y_expr) || !x_expr.equals(ws.x_expr)) 
		        return false;
		    else
		        return true;
	    } else {
		    if(x_expr != null)
		        if(!x_expr.equals(ws.x_expr) || ws.y_expr != null) 
			        return false;
		        else
			        return true;
		    else
		        if(!y_expr.equals(ws.y_expr) || ws.x_expr != null) 
			        return false;
		        else
			        return true;
	     }
*/
	}
    
       public void copy(Data ws)
       {
        if(ws.label != null)
	       label = new String(ws.label);        
	    if(ws.x_expr != null)
	       x_expr = new String(ws.x_expr);
	    if(ws.y_expr != null)
	       y_expr = new String(ws.y_expr);
	    if(ws.up_err != null)
	       up_err = new String(ws.up_err);
	    if(ws.low_err != null)
	       low_err = new String(ws.low_err);
	    shot          = ws.shot;
	    color_idx     = ws.color_idx;
	    interpolate   = ws.interpolate;
	    marker        = ws.marker;
	    marker_step   = ws.marker_step;
	} 
   }
   
   
   class SError extends ScopePositionDialog {

       private TextField e_up, e_low;
       private Button ok, cancel;
       private Data ws;
    
       SError(Frame fw) {
       
	  super(fw, "Error Setup", true); 	
	  super.setFont(new Font("Helvetica", Font.PLAIN, 10)); 
	  setResizable(false);   
	    
	  Label label;
		    
	  GridBagConstraints c = new GridBagConstraints();
	  GridBagLayout gridbag = new GridBagLayout();
	  setLayout(gridbag);        
    
	  c.insets = new Insets(4, 4, 4, 4);
	  c.fill = GridBagConstraints.BOTH;
	  
    
	  c.gridwidth = GridBagConstraints.BOTH;
	  label = new Label("Error up");
	  gridbag.setConstraints(label, c);
	  add(label);
		    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  e_up = new TextField(40);
	  gridbag.setConstraints(e_up, c);
	  add(e_up);
    
	  c.gridwidth = GridBagConstraints.BOTH;
	  label = new Label("Error low");
	  gridbag.setConstraints(label, c);
	  add(label);
		    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  e_low = new TextField(40);
	  gridbag.setConstraints(e_low, c);
	  add(e_low);
    
	  Panel p = new Panel();
	  p.setLayout(new FlowLayout(FlowLayout.CENTER));
	    
	  ok = new Button("Ok");
	  ok.addActionListener(this);	
	  p.add(ok);
    
	  cancel = new Button("Cancel");
	  cancel.addActionListener(this);	
	  p.add(cancel);
			    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  gridbag.setConstraints(p, c);
	  add(p);
	     
       } 
	  
       public void resetError()
       {
	 e_up.setText("");
	 e_low.setText("");
       }
       
       
       public void setError(Data ws_in)
       {
	 ws = ws_in;
	 resetError();
	 if(ws.up_err != null)
	    e_up.setText(ws.up_err);
	 if(ws.up_err != null)
	    e_low.setText(ws.low_err);
       }
      
    
       public void actionPerformed(ActionEvent e)
       {
	 Object ob = e.getSource();	
    
	 if(ob == ok) { 
	   ws.up_err   = new String(e_up.getText());
	   ws.low_err  = new String(e_low.getText());
	   setVisible(false);	    
	 }
	 if(ob == cancel)
	   setVisible(false);    
       }
   }
   
   class BrowseSig extends ScopePositionDialog {
       private Button dismiss;
       private List   sig_list;
       private int    type;
       private Frame  dw;
       final int  BROWSE_X = 0, BROWSE_Y = 1;
       
       BrowseSig(Frame fw, String title, int _type) {	
	      super(fw, title, true); 	
	      //super.setFont(new Font("Helvetica", Font.PLAIN, 10));
	      dw = fw;
	      type = _type;
          setResizable(false);
	 
	      BorderLayout bl= new BorderLayout(25,1);
	      setLayout(bl);
  
	      sig_list = new List(8, false);
	      sig_list.addItemListener(this);
	      add("Center",sig_list);
	      LoadSignal(main_scope.getSignalsFile());	

	      dismiss = new Button("Dismiss");
	      dismiss.addActionListener(this);
	      add("South", dismiss);	  		    	     
       } 
       
       public void LoadSignal(String file_list)
       {
	        BufferedReader in;
	        String str;
	   
	        try {
	   	        in = new BufferedReader(new FileReader(file_list));
		        while((str = in.readLine()) != null) 
		        {
		            sig_list.add(str);
		        }
	        } catch (IOException e) {
		        //error_msg.addMessage("Signal list file must be specified\n jScope ");
	        }
       }
       
       private void SetSignalString()
       {
       
	        String sig = sig_list.getSelectedItem();
	        if(sig != null) {
		        if(type == BROWSE_X)
		            x_expr.setText(sig);
		    if(type == BROWSE_Y)
		            y_expr.setText(sig);
	        } 
       }
       
       	         
       public void Show()
       {
	        pack();
	        setPosition(dw);
	        show();
       }
       
       public void actionPerformed(ActionEvent e)
       {
	        Object ob = e.getSource();	
    
	        if(ob == dismiss)  
	            setVisible(false);	   
       }
       
       public void itemStateChanged(ItemEvent e)
       { 
	        Object ob = e.getSource();

    	    if(ob == sig_list)
	            SetSignalString();
       }
   
  }



   class SList extends Container implements ActionListener, KeyListener, ItemListener, TextListener {
    private List             sig_list;
    private Choice           show_type, color, marker;
    private TextField	     marker_step_t;
    private Vector	     signals = new Vector();
    private String	     shot_str;
    private int		     shots[]={jScope.UNDEF_SHOT}, list_num_shot = 1;
    private int              sel_signal = -1;

    
      public SList() {

	BorderLayout bl= new BorderLayout(25,1);
	setLayout(bl );

	lab = new Label("Signals list");
	add("North", lab);

	sig_list = new List(8, false);
	sig_list.addItemListener(this);
	sig_list.addKeyListener(this);
	add("Center",sig_list);	

	Panel p = new Panel(new GridLayout(4,1));

	GridBagLayout gridbag = new GridBagLayout();
	GridBagConstraints c = new GridBagConstraints();
	
	c.gridwidth = 1;			
	c.gridheight = 1;			
	c.fill =  GridBagConstraints.HORIZONTAL;
	c.gridwidth = GridBagConstraints.REMAINDER;
	c.insets = new Insets(5,5,5,5);

        p.setLayout(gridbag);		

    	show_type = new Choice();
	show_type.addItem("Line"); 
	show_type.addItem("No Line"); 
	show_type.addItemListener(this);
	gridbag.setConstraints(show_type, c);
	p.add(show_type);	

	color = new Choice();
	SetColorList();	
	color.addItemListener(this);
	gridbag.setConstraints(color, c);
	p.add(color);	

	marker = new Choice();
	for(int i = 0; i < jScope.markerList.length; i++)
	    marker.addItem(jScope.markerList[i]);
	/*
	marker.addItem("No Marker"); 
	marker.addItem("Square"); 
	marker.addItem("Circle"); 
	marker.addItem("Cross");
	marker.addItem("Triangle");      
	marker.addItem("Point");
	*/
	marker.addItemListener(this);
	gridbag.setConstraints(marker, c);
	p.add(marker);
	
	c.gridwidth = 1;
	lab = new Label("M.Step");
	gridbag.setConstraints(lab, c);
	p.add(lab);
	
	c.fill =  GridBagConstraints.NONE;
	c.gridwidth = GridBagConstraints.REMAINDER;	
	marker_step_t = new TextField(3);
	marker_step_t.addTextListener(this);
	gridbag.setConstraints(marker_step_t, c);
	p.add(marker_step_t);
	
	add("East", p);
	
    	lab = new Label("");
	add("South", lab);
	
	setOptionState(false);

      }
      
      private void signalSelect(int sig)
      {
	sel_signal = sig;
        sig_list.select(sel_signal + 1);
	if(sig >= 0)
	    setOptionState(true);  
	else
	    setOptionState(false);  
      }
      
      private void setOptionState(boolean state)
      {
	marker.setEnabled(state);
	show_type.setEnabled(state);
	color.setEnabled(state);
      }
      
      public int getNumShot()
      {
	    return shots.length;
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

     public int findSignalSetup(Data ws)
     {
	for(int i=0; i < signals.size(); i++)
	    if(((Data)signals.elementAt(i)).equals(ws))
		return i;	 		
	return -1;
     }
   
     public void updateSignalSetup(int idx, Data ws)
     {
	signals.setElementAt(ws, idx);
     }

     public void removeSignalSetup()
     {
	int i, sig_idx, start_idx, end_idx;
	int num_shot = shots.length;
	int num_signal = signals.size();  		
          	
	if(sel_signal != -1) 
	{
    	    start_idx = (sel_signal/num_shot) * num_shot; // Divisione intera
	    end_idx   = start_idx + num_shot; 
	    for (i = 0; i < num_signal; i++)
		if(i >= start_idx && i < end_idx) { 
		    sig_list.remove(start_idx + 1);
		    signals.removeElementAt(start_idx);
		}	     	     
	}
	signalSelect(-1);
	signal_label.setText("");
	x_expr.setText("");
	y_expr.setText("");      
     }
     
     private void setMarkerTextState(int marker_idx)
     {
	if(marker_idx > 0 && marker_idx < 5)
	    marker_step_t.setEditable(true);
	else {
	    marker_step_t.setText("1");
	    marker_step_t.setEditable(false);
	}
     }

     public void putSignalSetup(Data ws)
     {
        
	    if(ws.label != null)
	        signal_label.setText(ws.label);
	    else
	        signal_label.setText("");      
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
	    marker_step_t.setText(""+ws.marker_step);
	    setMarkerTextState(ws.marker);			 
	    color.select(ws.color_idx);
	    if(error_w.isVisible())
	        error_w.setError(ws);	
      }	
      
      private void resetSignalSetup()
      {
        signal_label.setText("");
	    x_expr.setText("");      
	    y_expr.setText("");      
	    show_type.select(0);
	    marker.select(0);
	    marker_step_t.setText("1");
	    setMarkerTextState(0);			 
	    color.select(0);      
      }
      

      public Data getSignalSetup()
      {
	Data ws = new Data(); 

    ws.label         = new String(signal_label.getText());
	ws.x_expr        = new String(x_expr.getText());
	ws.y_expr        = new String(y_expr.getText());
	ws.interpolate   = (show_type.getSelectedIndex() == 0 ? true : false);
	ws.marker        = marker.getSelectedIndex();
	
	//if(marker_step_t.getText() != null && marker_step_t.getText().length() != 0)
	try{	
	    ws.marker_step   = new Integer(marker_step_t.getText()).intValue();
	//else
	} catch(NumberFormatException e) {
	    ws.marker_step   = 1;	
	}
	ws.color_idx     = color.getSelectedIndex();       

	return ws; 	 
      }
      
      public void reset()
      {
	signalSelect(-1);
	signals.removeAllElements();
      }
      
      public void init(WaveInterface wi)
      {
	  Data ws;
      
	  if(wi != null)
	  {
	    shots = new int[wi.num_shot];
	    
	    if(wi.shots == null)
	      for(int i = 0; i < wi.num_shot; i++)
		shots[i] = jScope.UNDEF_SHOT;	    
	    else
	      for(int i = 0; i < wi.num_shot; i++)
		shots[i] = wi.shots[i];
		
	    list_num_shot = wi.num_shot;
	    shot_str = wi.in_shot;
	    
	    for(int i = 0; i < wi.num_waves; i++)
	    {
		ws = new Data();
		ws.label         = wi.in_label[i];
		ws.x_expr        = wi.in_x[i];
		ws.y_expr        = wi.in_y[i];
		ws.up_err	 = wi.in_up_err[i];
		ws.low_err	 = wi.in_low_err[i];
		ws.interpolate   = wi.interpolates[i];
		ws.marker        = wi.markers[i];	
		ws.marker_step	 = wi.markers_step[i];
		ws.color_idx     = wi.colors_idx[i];
		if(wi.shots != null)
		    ws.shot = wi.shots[i];
		else
		    ws.shot = jScope.UNDEF_SHOT;
		addSignalSetup(ws);       
	    }
	    signalListRefresh();	    
	  }
	  signalSelect(sel_signal);
      }
      
      public Data[] getSignals()
      {
	  Data s[] = new Data[signals.size()];
	  signals.copyInto(s);
	  return s;
      }
    
      private void addSignalSetup(Data ws)
      { 
	signals.addElement((Object)ws);
      }
      
      public void addSignals()
      {
	int idx, color_idx = 0;
	Data ws;
      
	if(y_expr.getText().length() == 0)
	    return;
	
	ws = getSignalSetup();
	idx = findSignalSetup(ws);
	if( idx == -1) {
      
	    String x_e =  new String(x_expr.getText() == null ? "" : x_expr.getText()) ; 
	    String y_e =  new String(y_expr.getText());

	    if(shots.length != 0)
	    {
		for (int i = 0; i < shots.length; i++, ws = getSignalSetup())
		{
		    ws.shot = shots[i];
		    ws.color_idx = color_idx;
		    color_idx = (color_idx + 1) % setup.main_scope.color_dialog.GetNumColor();
		    addSignalSetup(ws);
		    signalListAdd(ws);
		}	
	    } else {
		ws.shot = jScope.UNDEF_SHOT;
		addSignalSetup(ws);
		signalListAdd(ws);
	    } 
	    signalSelect(findSignalSetup(ws));	    
	    putSignalSetup(ws);
	 }	 
       }
       
       public boolean evaluateShotList(String in_shot)
       {
	    if((shot_str == null && in_shot != null && in_shot.length()!= 0) || 
	       (shot_str != null && !shot_str.equals(in_shot)))
	    {
		    if(shots != null && shots.length != 0)
		        list_num_shot = shots.length;
		    else
		        list_num_shot = 1;
		    shot_str = in_shot;
		    shots  = main_scope.evaluateShot(in_shot);
		    if(shots == null)
		        shot_str = null;
		    return true;
	    } else
		    return false;	    
       }


       public void signalsRefresh()
       { 
	    int color_idx = 0, k = 0, l, num_sig = signals.size()/list_num_shot * shots.length;
    	    
	    for(int j = 0; j < num_sig; j += shots.length)
	    {	    		    
		for(int i = 0; i < shots.length; i++)
		{
		    if(i < list_num_shot) {
			signals.setElementAt(signals.elementAt(j+i),k);
		    } else {
			Data ws = new Data();
			ws.copy(((Data)signals.elementAt(j)));
			color_idx = (color_idx + 1) % main_scope.color_dialog.GetNumColor();
			ws.color_idx = color_idx;
			signals.insertElementAt(ws, k);
		    }
		    ((Data)signals.elementAt(k)).shot = shots[i];
		    k++;
		}
		for(l = shots.length; l < list_num_shot; l++)
		    signals.removeElementAt(j + shots.length);
	    }
       } 

       public void updateSignals()
       {
	  int i, j, sig_idx, start_idx, end_idx;
	  int num_shot = shots.length;
	  int num_signal = signals.size(); 
      
	  if(findSignalSetup(getSignalSetup()) != -1)
	    return;	
          	
	  if(sel_signal != -1) 
	  {
	    start_idx = (sel_signal/num_shot) * num_shot; // Divisione intera
	    end_idx   = start_idx + num_shot; 

	
	    for (i = 0,  j = 0 ; i < num_signal; i++)
		if(i >= start_idx && i < end_idx)  {
		    ((Data)signals.elementAt(i)).label = signal_label.getText();
		    ((Data)signals.elementAt(i)).x_expr = x_expr.getText();
		    ((Data)signals.elementAt(i)).y_expr = y_expr.getText();
		    signalListReplace(i + 1, (Data)signals.elementAt(i));
		}		
	    signalSelect(start_idx);
	   }
	 }
	 
	 public void updateError()
	 {
	    if(sel_signal == -1)
		return; 
	    error_w.setError((Data)signals.elementAt(sel_signal));	
	 }
         
	 public void signalListRefresh()
	 {
	    if(sig_list.getItemCount() != 0)
		sig_list.removeAll();
	    sig_list.add("Select this item to add new expression");
	    for(int i = 0; i < signals.size(); i++)
		signalListAdd((Data)signals.elementAt(i));
	    signalSelect(sel_signal); 	
	  } 

	  private void signalListAdd(Data ws)
	  {
	  
	    if(ws.shot != jScope.UNDEF_SHOT)
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
	  
	  private void signalListReplace(int idx, Data ws)
	  {
	    if(ws.shot != jScope.UNDEF_SHOT)
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

      public void updateList()
      {
    	    if(evaluateShotList(shot.getText())) {
		signalsRefresh();
		signalListRefresh();
	    }
    	    if(sel_signal == -1)	    
		signalList.addSignals();
	    else
		signalList.updateSignals();			    	
      }
 
     
      public void textValueChanged(TextEvent e)
      {
	    Object ob = e.getSource();
	    
	    if(ob == marker_step_t && sel_signal != -1)
	    {
		try {
		    ((Data)signals.elementAt(sel_signal)).marker_step = new Integer(marker_step_t.getText()).intValue();
		} catch (NumberFormatException ex) {
		    marker_step_t.setText("1");
		}

    	    }		  	      
      }
     
      public void actionPerformed(ActionEvent e) 
      {}
      public void keyReleased(KeyEvent e)
      {}
      public void keyTyped(KeyEvent e)
      {}
      
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
      }
      
      public void itemStateChanged(ItemEvent e)
      {
	Object ob = e.getSource();

    	if(ob == sig_list)
	{
           sel_signal = sig_list.getSelectedIndex() - 1;
	   if(sel_signal >= 0) {
	       setOptionState(true);
	       signalList.putSignalSetup((Data)signals.elementAt(sel_signal));
	   } else {
	       signalList.resetSignalSetup();
	       setOptionState(false);
	    }
	} 

	if(ob instanceof Checkbox)
	    defaultButtonChange(ob);

    	if(sel_signal == -1)
	    return;
	    
	if(ob == marker) {
	    int m_idx =  marker.getSelectedIndex();
	    ((Data)signals.elementAt(sel_signal)).marker = m_idx;
	    setMarkerTextState(m_idx);
    	}	

	if(ob == show_type) {
	    ((Data)signals.elementAt(sel_signal)).interpolate = show_type.getSelectedIndex() == 0 ? true : false;
	}	

	if(ob == color) {
	    ((Data)signals.elementAt(sel_signal)).color_idx = color.getSelectedIndex();       
	}
      
      }
   }


  class ExpandExp extends ScopePositionDialog { 

  private Label lab;
  private TextArea x_expr, y_expr;
  private Button ok, cancel;
  private SetupDataDialog  conf_dialog;
 
     ExpandExp(Frame _fw, SetupDataDialog conf_diag)
     {
	super(_fw, "Expand Expression Dialog", false);
	super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
	setModal(true);
	conf_dialog = conf_diag;
	GridBagLayout gridbag = new GridBagLayout();
	GridBagConstraints c = new GridBagConstraints();
	Insets insets = new Insets(4, 4, 4, 4);
    
	setLayout(gridbag);		
    
	  c.anchor = GridBagConstraints.NORTH;
	  c.fill =  GridBagConstraints.BOTH;
	  c.gridwidth = 1;
	  c.gridheight = 1;
	  lab = new Label("Y");
	  gridbag.setConstraints(lab, c);
	  add(lab);		 	
    
	  c.gridheight = 11;
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  y_expr = new TextArea(10,80);
	  gridbag.setConstraints(y_expr, c);
	  add(y_expr);
	  
	  c.anchor = GridBagConstraints.NORTH;
	  c.fill =  GridBagConstraints.BOTH;
	  c.gridwidth = 1;
	  c.gridheight = 1;    
	  lab = new Label("X");
	  gridbag.setConstraints(lab, c);
	  add(lab);		 	
    
	  c.gridheight = 11;
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  x_expr = new TextArea(10,80);
	  gridbag.setConstraints(x_expr, c);
	  add(x_expr);
    
	  Panel p = new Panel();
	  p.setLayout(new FlowLayout(FlowLayout.CENTER));
	    
	  ok = new Button("Ok");
	  ok.addActionListener(this);	
	  p.add(ok);
    
	  cancel = new Button("Cancel");
	  cancel.addActionListener(this);	
	  p.add(cancel);
			    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  gridbag.setConstraints(p, c);
	  add(p);
    
     }
     
     /**
      ** Set values of x and/or y expression field
     */
     public void setExpressionString(String x, String y)
     {
	if(x != null)
	    x_expr.setText(x);
	if(y != null)
	    y_expr.setText(y);
     }
     
     public void actionPerformed(ActionEvent e)
     {
	 Object ob = e.getSource();	
    
	 if(ob == ok) {
	    conf_dialog.x_expr.setText(x_expr.getText());
	    conf_dialog.y_expr.setText(y_expr.getText());
	 }
	 setVisible(false);	   
     }      

  }



   public SetupDataDialog(Frame fw, String frame_title) {

      super(fw, frame_title, false);
      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);    

      main_scope = (jScope)fw;
      error_w = new SError(fw);
      error_msg = new ErrorMessage(fw);
      expand_expr = new ExpandExp(fw, this);	    
     
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(2,2,2,2);

      setLayout(gridbag);		
      c.insets = insets;
      c.anchor = GridBagConstraints.WEST;	
      c.fill =  GridBagConstraints.HORIZONTAL;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      title_b = new Checkbox("Title");
      title_b.addItemListener(this);
      gridbag.setConstraints(title_b, c);
      add(title_b);
 
      c.gridwidth = 5;	
      title = new TextField(40);
      gridbag.setConstraints(title, c);
      add(title);
      		 	
      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      c.gridheight = 1;
      expand = new Button("Expand Expr.");	
      expand.addActionListener(this);
      gridbag.setConstraints(expand, c);
      add(expand);	      	


      c.fill =  GridBagConstraints.HORIZONTAL;
      c.gridwidth = 1;
      lab = new Label("Signal Label");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.gridwidth = 5;
//      c.fill =  GridBagConstraints.HORIZONTAL;
      signal_label = new TextField();
      signal_label.addKeyListener(this);
      gridbag.setConstraints(signal_label, c);
      add(signal_label);

//      c.gridwidth = 1;
      c.gridwidth = GridBagConstraints.REMAINDER;
      error = new Button("Error...");
      error.addActionListener(this);
      gridbag.setConstraints(error, c);
      add(error);


      c.fill =  GridBagConstraints.HORIZONTAL;
      c.gridwidth = 1;
      lab = new Label("Y");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.gridwidth = 5;
      y_expr = new TextField();
      y_expr.addKeyListener(this);
      gridbag.setConstraints(y_expr, c);
      add(y_expr);

	                
      c.gridwidth = GridBagConstraints.REMAINDER;
      y_log = new Checkbox("Log Scale", null, false); 		 	
      gridbag.setConstraints(y_log, c);
      add(y_log);
       
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

      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      y_browser = new Button("Browse...");
      y_browser.addActionListener(this);
      gridbag.setConstraints(y_browser, c);
      add(y_browser);	

      c.fill =  GridBagConstraints.HORIZONTAL;
      c.gridwidth = 1;
      lab = new Label("X");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.gridwidth = 5;
      x_expr = new TextField(30);
      x_expr.addKeyListener(this);
      gridbag.setConstraints(x_expr, c);
      add(x_expr);		 	

      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      x_log = new Checkbox("Log Scale", null, false); 		 	
      gridbag.setConstraints(x_log, c);
      add(x_log);

      c.fill =  GridBagConstraints.HORIZONTAL;
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

      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      x_browser = new Button("Browse...");
      x_browser.addActionListener(this);
      gridbag.setConstraints(x_browser, c);
      add(x_browser);
 
      c.fill =  GridBagConstraints.HORIZONTAL;			          		
      c.gridwidth = 1;
      experiment_b = new Checkbox("Experiment");
      experiment_b.addItemListener(this);
      gridbag.setConstraints(experiment_b, c);
      add(experiment_b);	
      
      experiment = new TextField(25);
      experiment.addKeyListener(this);
      gridbag.setConstraints(experiment, c);
      add(experiment);	

      c.fill =  GridBagConstraints.NONE;
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


      c.fill =  GridBagConstraints.HORIZONTAL;			          		
      c.gridwidth = 1;
      upd_event_b = new Checkbox("Update event");
      upd_event_b.addItemListener(this);
      gridbag.setConstraints(upd_event_b, c);
      add(upd_event_b);	
      
      upd_event = new TextField(25);
      gridbag.setConstraints(upd_event, c);
      add(upd_event);	

      c.fill =  GridBagConstraints.NONE;
      def_node_b = new Checkbox("Default node");
      def_node_b.addItemListener(this);      
      gridbag.setConstraints(def_node_b, c);
      add(def_node_b);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      def_node = new TextField(30);
 //     def_node.addKeyListener(this);
 //     def_node.addFocusListener(this);
      gridbag.setConstraints(def_node, c);
      add(def_node);

      c.fill =  GridBagConstraints.BOTH;
      signalList = new SList(); 
      gridbag.setConstraints(signalList, c);
      add(signalList);	
      	
      c.fill   = GridBagConstraints.NONE;     
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
      
//      browse_x_w = new BrowseSig(fw, "X signal Broser", BROWSE_X);
//      browse_y_w = new BrowseSig(fw, "Y signal Broser", BROWSE_Y);    
   }
   
   public void Show(MultiWaveform w, Setup _setup)
   {
      setup = _setup;      
      pack();
      wi = new WaveInterface(setup.main_scope.db);
      wave = w;
      putWindowSetup(w.wi);
      setPosition(w.getParent());
      Point p = setup.GetWavePos(w);
      setTitle("Wave Setup for column " + p.x + " row " + p.y); 	
      show();
   }
   
   public void SetColorList()
   {
	signalList.SetColorList();
   }

   private void resetDefaultFlags()
   {
    boolean state = true;
	title_b.setState(state);
	title.setEditable(!state); 
	shot_b.setState(state); 
	shot.setEditable(!state); 
	experiment_b.setState(state); 
	experiment.setEditable(!state); 
	x_max_b.setState(state); 
	x_max.setEditable(!state); 
	x_min_b.setState(state); 
	x_min.setEditable(!state); 
	x_label_b.setState(state); 
	x_label.setEditable(!state); 
	y_max_b.setState(state); 
	y_max.setEditable(!state); 
	y_min_b.setState(state); 
	y_min .setEditable(!state); 
	y_label_b.setState(state);  
	y_label.setEditable(!state);
	upd_event_b.setState(state);
	upd_event.setEditable(!state);
    def_node_b.setState(state);
    def_node.setEditable(!state);
    putDefaultValues();
   } 
     
   private void setDefaultFlags(int flags)
   {   
	for(int i=0; i < 32; i++)
	{
	    switch(i)
	    {
		case WaveInterface.B_title:
		    title_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_shot:
		    shot_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_exp:
		    experiment_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_x_max:
		    x_max_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_x_min:
		    x_min_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_x_label:
		    x_label_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_y_max:
		    y_max_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_y_min:
		    y_min_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case WaveInterface.B_y_label:
		    y_label_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break;  
		case WaveInterface.B_event:
		    upd_event_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break;  
		case WaveInterface.B_default_node:
		    def_node_b.setState(((flags & (1<<i)) == 1<<i)?true:false);break;  
	    }
	}       
   }
   
   private int getDefaultFlags()
   {
       int value = 0;
       
	if  (title_b.getState())	 value |= 1<<WaveInterface.B_title; 
				    else value &= ~(1<<WaveInterface.B_title); 
	if  (shot_b.getState()  )	 value |= 1<<WaveInterface.B_shot  ; 
				    else value &= ~(1<<WaveInterface.B_shot); 
	if  (experiment_b.getState() )	 value |= 1<<WaveInterface.B_exp ; 
				    else value &= ~(1<<WaveInterface.B_exp); 
	if  (x_max_b.getState() )	 value |= 1<<WaveInterface.B_x_max ; 
				    else value &= ~(1<<WaveInterface.B_x_max); 
	if  (x_min_b.getState() )	 value |= 1<<WaveInterface.B_x_min ;
				    else value &= ~(1<<WaveInterface.B_x_min); 
	if  (x_label_b.getState() )	 value |= 1<<WaveInterface.B_x_label ; 
				    else value &= ~(1<<WaveInterface.B_x_label); 
	if  (y_max_b.getState() )        value |= 1<<WaveInterface.B_y_max ; 
				    else value &= ~(1<<WaveInterface.B_y_max); 
	if  (y_min_b.getState() )	 value |= 1<<WaveInterface.B_y_min ; 
				    else value &= ~(1<<WaveInterface.B_y_min); 
	if  (y_label_b.getState() )	 value |= 1<<WaveInterface.B_y_label ; 
				    else value &= ~(1<<WaveInterface.B_y_label);
	if  (upd_event_b.getState() )	 value |= 1<<WaveInterface.B_event ; 
				    else value &= ~(1<<WaveInterface.B_event);
	if  (def_node_b.getState() )	 value |= 1<<WaveInterface.B_default_node ; 
				    else value &= ~(1<<WaveInterface.B_default_node);
	return (value);  
   }

   public void putDefaultValues()
   {
        boolean def_flag;
        
	    defaultButtonOperation(title, def_flag = title_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_title, def_flag, wi));
	    putShotValue(shot_b.getState());
	    defaultButtonOperation(experiment, def_flag = experiment_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_exp, def_flag, wi));
	    defaultButtonOperation(x_max, def_flag = x_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_max, def_flag, wi));
	    defaultButtonOperation(x_min, def_flag = x_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_min, def_flag, wi));
	    defaultButtonOperation(x_label, def_flag = x_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_label, def_flag, wi));
	    defaultButtonOperation(y_max, def_flag = y_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_max, def_flag, wi));
	    defaultButtonOperation(y_min, def_flag = y_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_min, def_flag, wi));
	    defaultButtonOperation(y_label, def_flag = y_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_label, def_flag, wi));
	    defaultButtonOperation(upd_event, def_flag = upd_event_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_event, def_flag, wi));
	    defaultButtonOperation(def_node,  def_flag = def_node_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_default_node, def_flag, wi));    
   }

   public void putWindowSetup(WaveInterface wi)      
   {	
	boolean def_flag;
	
	if(wi == null) {
	    eraseForm();	    
	    return;
	}

	this.wi.cexperiment     = wi.cexperiment;	
	this.wi.cin_shot        = wi.cin_shot;	
	this.wi.cin_upd_event   = wi.cin_upd_event;	
	this.wi.cin_def_node    = wi.cin_def_node;	
    this.wi.cin_xmax        = wi.cin_xmax;
	this.wi.cin_xmin        = wi.cin_xmin;
	this.wi.cin_ymax        = wi.cin_ymax;
	this.wi.cin_ymin        = wi.cin_ymin;
	this.wi.cin_title       = wi.cin_title;
	this.wi.cin_xlabel      = wi.cin_xlabel;
	this.wi.cin_ylabel      = wi.cin_ylabel;
	this.wi.legend_x        = wi.legend_x;
	this.wi.legend_y        = wi.legend_y;
    this.wi.make_legend     = wi.make_legend;

	setDefaultFlags(wi.defaults);
	putDefaultValues();
/*
	defaultButtonOperation(title, def_flag = title_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_title, def_flag, wi));
	putShotValue(shot_b.getState());
	defaultButtonOperation(experiment, def_flag = experiment_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_exp, def_flag, wi));
	defaultButtonOperation(x_max, def_flag = x_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_max, def_flag, wi));
	defaultButtonOperation(x_min, def_flag = x_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_min, def_flag, wi));
	defaultButtonOperation(x_label, def_flag = x_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_label, def_flag, wi));
	defaultButtonOperation(y_max, def_flag = y_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_max, def_flag, wi));
	defaultButtonOperation(y_min, def_flag = y_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_min, def_flag, wi));
	defaultButtonOperation(y_label, def_flag = y_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_label, def_flag, wi));
	defaultButtonOperation(upd_event, def_flag = upd_event_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_event, def_flag, wi));
	defaultButtonOperation(def_node,  def_flag = def_node_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_default_node, def_flag, wi));
*/
	x_log.setState(wi.x_log);
	y_log.setState(wi.y_log);
	
    signal_label.setText("");
    x_expr.setText("");
    y_expr.setText("");

	signalList.init(wi);       		
   }

  public void eraseForm()
  {
	title.setText("");
	signal_label.setText("");
	x_expr.setText("");
	x_label.setText("");
	x_label.setForeground(Color.black);		
	x_max.setText("");
	x_max.setForeground(Color.black);		
	x_min.setText("");
	x_min.setForeground(Color.black);		
	y_expr.setText("");
	y_max.setText("");
	y_max.setForeground(Color.black);		
	y_min.setText("");
	y_min.setForeground(Color.black);		
	y_label.setText("");
	y_label.setForeground(Color.black);		
	experiment.setText("");
	shot.setText("");
	upd_event.setText("");
	def_node.setText("");	
	shot.setForeground(Color.black);		
	resetDefaultFlags();
	signalList.reset();
	signalList.signalListRefresh();
  } 
 

    public boolean isChanged(Data s[])
    {
	WaveInterface wave_wi = wave.wi;

    	if(wave_wi == null) return true;
	
	if(wave_wi.modified) return true;
    
	if(s.length     != wave_wi.num_waves) return true;
	if(signalList.getNumShot() != wave_wi.num_shot) return true;
	
	if(!main_scope.equalsString(title.getText(),   wave_wi.cin_title))    return true;	
	if(!main_scope.equalsString(x_max.getText(),   wave_wi.cin_xmax))     return true;
	if(!main_scope.equalsString(x_min.getText(),   wave_wi.cin_xmin))     return true;
	if(!main_scope.equalsString(x_label.getText(), wave_wi.cin_xlabel))   return true;
	if(x_log.getState() != wave_wi.x_log)				                  return true;
	if(!main_scope.equalsString(y_max.getText(),   wave_wi.cin_ymax))     return true;
	if(!main_scope.equalsString(y_min.getText(),   wave_wi.cin_ymin))     return true;
	if(!main_scope.equalsString(y_label.getText(), wave_wi.cin_ylabel))   return true;	
	if(y_log.getState() != wave_wi.y_log)				                  return true;		
    if(!main_scope.equalsString(shot.getText(),   wave_wi.cin_shot))        return true;	
    if(!main_scope.equalsString(upd_event.getText(),   wave_wi.cin_upd_event))  return true;	
    if(!main_scope.equalsString(def_node.getText(),    wave_wi.cin_def_node))   return true;	
	if(!main_scope.equalsString(experiment.getText(), wave_wi.cexperiment)) return true;
	if(getDefaultFlags() != wave_wi.defaults)				                return true;	

    for(int i = 0 ; i < wave_wi.num_waves; i++)
	{
	    if(!main_scope.equalsString(s[i].x_expr,  wave_wi.in_x[i]))        return true;
	    if(!main_scope.equalsString(s[i].y_expr,  wave_wi.in_y[i]))	       return true;
	    if(!main_scope.equalsString(s[i].up_err,  wave_wi.in_up_err[i]))   return true;
	    if(!main_scope.equalsString(s[i].low_err, wave_wi.in_low_err[i]))  return true;
	}
	return false;
    }

    private void update()
    {
        Cursor c_cursor = getCursor();
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        try
        {
	        signalList.updateList();
            //setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
            setCursor(c_cursor);
	    } catch (Throwable e) {
	        main_scope.SetStatusLabel("Unrecoverable error during apply");	    
	        //setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
            setCursor(c_cursor);
	    }
    }

   private void updateGlobalWI()
   {
      if(!experiment_b.getState() && !main_scope.equalsString(experiment.getText(), wi.cexperiment))
	    wi.cexperiment   = experiment.getText();	
      if(!shot_b.getState() && !main_scope.equalsString(shot.getText(),   wi.cin_shot))
	    wi.cin_shot      = shot.getText();	
      if(!upd_event_b.getState() && !main_scope.equalsString(upd_event.getText(),   wi.cin_upd_event))
	    wi.cin_upd_event      = upd_event.getText();	
      if(!def_node_b.getState() && !main_scope.equalsString(def_node.getText(),   wi.cin_def_node))
	    wi.cin_def_node       = def_node.getText();	
      if(!x_max_b.getState() && !main_scope.equalsString(x_max.getText(),   wi.cin_xmax))
	    wi.cin_xmax      = x_max.getText();
      if(!x_min_b.getState() && !main_scope.equalsString(x_min.getText(),   wi.cin_xmin))
	    wi.cin_xmin      = x_min.getText();
      if(!y_max_b.getState() && !main_scope.equalsString(y_max.getText(),   wi.cin_ymax))
	    wi.cin_ymax      = y_max.getText();
      if(!y_min_b.getState() && !main_scope.equalsString(y_min.getText(),   wi.cin_ymin))
	    wi.cin_ymin      = y_min.getText();
      if(!title_b.getState() && !main_scope.equalsString(title.getText(),   wi.cin_title))
	    wi.cin_title     = title.getText();
      if(!x_label_b.getState() && !main_scope.equalsString(x_label.getText(), wi.cin_xlabel))
	    wi.cin_xlabel    = x_label.getText();
      if(!y_label_b.getState() && !main_scope.equalsString(y_label.getText(), wi.cin_ylabel))
	    wi.cin_ylabel    = y_label.getText();
   }

   private int updateWI()
   {
	  Data[] s;
	  int num_signal;
	      
	  s = signalList.getSignals();
	  num_signal = s.length;

 	  if(num_signal == 0)
	      return 1;


	  wi.modified = isChanged(s);
	  
	  if(!wi.modified)
	  {
//		wave.wi.full_flag = !main_scope.GetFastNetworkState();
		for(int i = 0; i < wave.wi.num_waves; i++)      
		{
		    wave.wi.markers[i]      = s[i].marker;
		    wave.wi.markers_step[i] = s[i].marker_step;
		    wave.wi.interpolates[i] = s[i].interpolate;
		    wave.wi.colors[i]       = main_scope.color_dialog.GetColorAt(s[i].color_idx);
		    wave.wi.colors_idx[i]   = s[i].color_idx;
		    wave.wi.in_label[i]     = s[i].label;
		}
		return 0;	 
	  }

	  updateGlobalWI();
	     	     
	  wi.num_waves = num_signal;
    
	  wi.experiment     = new String(experiment.getText());	
	  wi.in_shot        = new String(shot.getText());	
	  wi.in_def_node    = new String(def_node.getText());	
      wi.in_upd_event   = new String(upd_event.getText());
	  wi.in_xmax        = new String(x_max.getText());
	  wi.in_xmin        = new String(x_min.getText());
	  wi.in_ymax        = new String(y_max.getText());
	  wi.in_ymin        = new String(y_min.getText());
	  wi.in_title       = new String(title.getText());
	  wi.in_xlabel      = new String(x_label.getText());
	  wi.in_ylabel      = new String(y_label.getText());
	    
	  wi.x_log        = x_log.getState();
	  wi.y_log        = y_log.getState();
	  wi.full_flag    = !main_scope.GetFastNetworkState();
	  wi.num_shot     = signalList.getNumShot();
	  wi.defaults     = getDefaultFlags();
     
	  wi.in_label     = new String[num_signal];
	  wi.in_x         = new String[num_signal];
	  wi.in_y         = new String[num_signal];
	  wi.in_up_err    = new String[num_signal];
          wi.in_low_err   = new String[num_signal];
          wi.markers      = new int[num_signal];
          wi.markers_step = new int[num_signal];	  
          wi.colors       = new Color[num_signal];
          wi.colors_idx   = new int[num_signal];
          wi.interpolates = new boolean[num_signal];
    
          wi.shots        = new int[num_signal];				
    
          for(int i = 0; i < num_signal; i++)      
          {
	        if(s[i].label != null)
		        wi.in_label[i]         = new String(s[i].label);

	        if(s[i].x_expr != null)
		        wi.in_x[i]         = new String(s[i].x_expr);
	        if(s[i].y_expr != null)	     
		        wi.in_y[i]         = new String(s[i].y_expr);
	        wi.markers[i]      = s[i].marker;
	        wi.markers_step[i] = s[i].marker_step;
	        wi.interpolates[i] = s[i].interpolate;
	        if(wi.shots != null)
		        wi.shots[i]        = s[i].shot;
	        if(s[i].up_err != null)
		        wi.in_up_err[i] = new String(s[i].up_err);
	        if(s[i].low_err != null)      
		        wi.in_low_err[i] = new String(s[i].low_err);	    
	        wi.colors[i]       = main_scope.color_dialog.GetColorAt(s[i].color_idx);
	        wi.colors_idx[i]   = s[i].color_idx;
	      }
	  
	  
	      if(wi.shots[0] == jScope.UNDEF_SHOT)
	   	     wi.shots = null;

          String event = wave.wi != null ? wave.wi.in_upd_event : null;
  	      main_scope.SetRemoveMdsEvent(wave, event, wi.in_upd_event);
	  	    
	      wave.wi = wi;
	   
	      return 0;
     }

   private int checkSetup()
   {
	int error = 0;
	boolean def_exp = true, def_shot = true;

      main_scope.SetStatusLabel("");
				
	if(experiment.getText() == null || experiment.getText().trim().length() == 0)
	    def_exp = false;
	
	if(shot.getText() == null || shot.getText().trim().length() == 0)
	    def_shot = false;
	
	if(def_exp ^ def_shot)
	{
	    if(!def_shot) {
		    error_msg.addMessage("Experiment defined but undefined shot\n");
		    error = 1;
		}    
	    if(!def_exp && !setup.data_server_address.equals("Ftu data")) {	    
		    error_msg.addMessage("Shot defined but undefined experiment\n");
	        error = 1;
	    }
	}

	update();
	
	if(updateWI() != 0)
	{
	    error_msg.addMessage("Nothing to evaluate\n");
	    error = 1;
	}
	
	return error;

    }


   private  void applyWaveform()
   {
     if(checkSetup() == 0)
     {
        main_scope.evaluateWave(wave, shot.getText());
        /*
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        try
        {
	        wave.SetMode(Waveform.MODE_WAIT);
	        main_scope.SetStatusLabel("Update signals for shots " + shot.getText());
	        String e = setup.UpdateWave(wave);
	        if(e != null) {
	            if(main_scope.briefError())
		            error_msg.addMessage(e);
	            else
		            error_msg.setMessage(e);
	            error_msg.showMessage();
	        }  
	        main_scope.SetStatusLabel("Wave is up to date");
	        wave.SetMode(main_scope.wave_mode);
            setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	    } catch (Throwable e) {	        
	        wave.SetMode(main_scope.wave_mode);
            setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	        main_scope.SetStatusLabel("Unrecoverable error during applies");	    
	    }
	    */
      } else
	     error_msg.showMessage();       	
   }
	    	
   public void actionPerformed(ActionEvent e)
   {
      Object ob = e.getSource();

      if(getCursor().getType() == Cursor.WAIT_CURSOR)
        return;

      if(ob == erase) {
	    eraseForm();
	    wave.wi = null;
	    wave.Erase();
      }
		
      if(ob == cancel) {
	    setVisible(false);
	    signalList.reset();
      }
	
      if(ob == apply || ob == ok)
      {
	    applyWaveform();
	    if(ob == ok) {
	       signalList.reset();
	       setVisible(false);
	    }	
      }
 
      if(ob == reset) {
	    signalList.reset();
	    putWindowSetup(wave.wi); 	
      }
      
      if(ob == error && y_expr.getText().trim().length() != 0)
      {
	    update();
	    signalList.updateError();
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
      
//      if(ob == x_browser)
//	    browse_x_w.Show();
	
//      if(ob == y_browser)
//	    browse_y_w.Show();
	
   }

   public void keyPressed(KeyEvent e)
   {
      Object ob = e.getSource();
      char key  = e.getKeyChar();
      
      
     if(key == KeyEvent.CHAR_UNDEFINED)
	    return;		  	      	

     if(key == KeyEvent.VK_ENTER)
     { 
	    if(ob == y_expr || ob == x_expr || ob == shot || ob == experiment || ob == signal_label) {
	      update();
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
   
   private void  defaultButtonOperation(TextField text, boolean state, String val)
   {
	if(state) {
	    text.setForeground(Color.blue);
	    text.setEditable(false);
	} else {
	    text.setForeground(Color.black);		
	    text.setEditable(true);
	}   
	if(val != null && val.trim().length() != 0)	    
	    text.setText(val);
	else 
	    text.setText("");
   }
   
   private void putShotValue(boolean def_flag)
   {
	switch(main_scope.getUsedShotIdx(def_flag))
        {
		case 0:  
		    shot.setForeground(Color.black);
		    shot.setEditable(true);
		break;
		case 1: 
		    shot.setForeground(Color.blue);
		    shot.setEditable(false);
		break;
		case 2:
		    shot.setForeground(Color.red);
		    shot.setEditable(false);
		break;
	    }
	    shot.setText(main_scope.getUsedShot(def_flag, wi));
   }

   private void  defaultButtonChange(Object ob)
   {
	boolean def_flag;
   
	if(ob == title_b)
	    defaultButtonOperation(title, def_flag = title_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_title, def_flag, wi));
    if(ob == shot_b) 
	    putShotValue(shot_b.getState());
	if(ob == experiment_b)
	    defaultButtonOperation(experiment, def_flag = experiment_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_exp, def_flag, wi));
	if(ob == upd_event_b)
	    defaultButtonOperation(upd_event, def_flag = upd_event_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_event, def_flag, wi));
	if(ob == def_node_b)
	    defaultButtonOperation(def_node, def_flag = def_node_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_default_node, def_flag, wi));
	if(ob == x_max_b)
	    defaultButtonOperation(x_max, def_flag = x_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_max, def_flag, wi));
	if(ob == x_min_b)
	    defaultButtonOperation(x_min, def_flag = x_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_min, def_flag, wi));
	if(ob == x_label_b)
	    defaultButtonOperation(x_label, def_flag = x_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_x_label, def_flag, wi));
	if(ob == y_max_b)
	    defaultButtonOperation(y_max, def_flag = y_max_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_max, def_flag, wi));
	if(ob == y_min_b)
	    defaultButtonOperation(y_min, def_flag = y_min_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_min, def_flag, wi));
	if(ob == y_label_b)
	    defaultButtonOperation(y_label, def_flag = y_label_b.getState(), main_scope.setup_default.getDefaultValue(WaveInterface.B_y_label, def_flag, wi));

   }

   public void itemStateChanged(ItemEvent e)
   {
	Object ob = e.getSource();
	

	if(ob instanceof Checkbox)
	    defaultButtonChange(ob);

	    
    }   
}

