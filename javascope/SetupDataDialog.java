import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
import java.util.Vector;
import javax.swing.*;
import javax.swing.event.*;




//*****
//Form di visualizzazione della configurazione della
//wave selezionata
//*****
 class SetupDataDialog extends JDialog implements ActionListener, 
                                                  ItemListener,
                                                  KeyListener,
                                                  WaveformListener
 {
 
   private JLabel	    lab;
   private SError	    error_w;
   private jScope_1	    main_scope;
   private ExpandExp    expand_expr;
   private SList	    signalList;
   private jScopeMultiWave    wave;	    
   public  MdsWaveInterface   wi;
   private Point        wave_coord;
   
    JCheckBox title_b = new JCheckBox("Title");
    JTextField title = new JTextField(50);
    JButton expand = new JButton("Expand Expr.");
    JLabel sig_label = new JLabel();
    JTextField signal_label = new JTextField(50);
    JButton error = new JButton("Error");
    JLabel y_lab = new JLabel();
    JTextField y_expr = new JTextField(58);
    JCheckBox y_min_b = new JCheckBox("Y min");
    JTextField y_min = new JTextField(10);
    JLabel pix_y_min = new JLabel();
    JCheckBox y_max_b = new JCheckBox("Y max");
    JTextField y_max = new JTextField(10);
    JLabel pix_y_max = new JLabel();
    JCheckBox image_b = new JCheckBox("Is image");
    JCheckBox use_jai_b = new JCheckBox("Use JAI");
    JLabel x_lab = new JLabel();
    JTextField x_expr = new JTextField(58);
    JCheckBox x_log = new JCheckBox("Log scale");
    JCheckBox x_label_b = new JCheckBox("X Label");
    JTextField x_label = new JTextField(20);
    JCheckBox y_log = new JCheckBox("Log scale");
    JCheckBox y_label_b = new JCheckBox("Y Label");
    JTextField y_label = new JTextField(20);
    JCheckBox x_min_b = new JCheckBox("X min");
    JCheckBox time_min_b = new JCheckBox("T min");
    JLabel pix_x_max = new JLabel();
    JTextField x_min = new JTextField(10);
    JTextField time_min = new JTextField(10);
    JCheckBox x_max_b = new JCheckBox("X max");
    JCheckBox time_max_b = new JCheckBox("T max");
    JLabel pix_x_min = new JLabel();
    JTextField x_max = new JTextField(10);
    JTextField time_max = new JTextField(10);
    JCheckBox keep_ratio_b = new JCheckBox("Keep ratio");
    JCheckBox experiment_b = new JCheckBox("Experiment");
    JTextField experiment = new JTextField(20);
    JCheckBox shot_b = new JCheckBox("Shot");
    JTextField shot = new JTextField(25);
    JCheckBox horizontal_flip_b = new JCheckBox("Horizontal flip");
    JCheckBox upd_event_b = new JCheckBox("Update event");
    JTextField upd_event = new JTextField(20);
    JCheckBox def_node_b = new JCheckBox("Default node");
    JTextField def_node = new JTextField(20);
    JCheckBox vertical_flip_b = new JCheckBox("Vertical Flip");
    JPanel p9 = new JPanel(new BorderLayout(2,2));
    JButton ok = new JButton("Ok");
    JButton apply = new JButton("Apply");
    JButton reset = new JButton("Reset");
    JButton erase = new JButton("Erase");
    JButton cancel = new JButton("Cancel");
   
     
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
   
   
   class SError extends JDialog implements ActionListener 
   {

      private JTextField e_up, e_low;
      private JButton ok, cancel;
      private Data ws;
    
      SError(Frame fw) 
      {               
	        super(fw, "Error Setup", true); 	
//	        super.setFont(new Font("Helvetica", Font.PLAIN, 10)); 
	        setResizable(false);   
        	    
	        JLabel label;
        		    
	        GridBagConstraints c = new GridBagConstraints();
	        GridBagLayout gridbag = new GridBagLayout();
	        getContentPane().setLayout(gridbag);        
            
	        c.insets = new Insets(4, 4, 4, 4);
	        c.fill = GridBagConstraints.BOTH;
        	             
	        c.gridwidth = GridBagConstraints.BOTH;
	        label = new JLabel("Error up");
	        gridbag.setConstraints(label, c);
	        getContentPane().add(label);
        		    
	        c.gridwidth = GridBagConstraints.REMAINDER;
	        e_up = new JTextField(40);
	        gridbag.setConstraints(e_up, c);
	        getContentPane().add(e_up);
            
	        c.gridwidth = GridBagConstraints.BOTH;
	        label = new JLabel("Error low");
	        gridbag.setConstraints(label, c);
	        getContentPane().add(label);
        		    
	        c.gridwidth = GridBagConstraints.REMAINDER;
	        e_low = new JTextField(40);
	        gridbag.setConstraints(e_low, c);
	        getContentPane().add(e_low);
            
	        JPanel p = new JPanel();
	        p.setLayout(new FlowLayout(FlowLayout.CENTER));
        	    
	        ok = new JButton("Ok");
	        ok.addActionListener(this);	
	        p.add(ok);
            
	        cancel = new JButton("Cancel");
	        cancel.addActionListener(this);	
	        p.add(cancel);
        			    
	        c.gridwidth = GridBagConstraints.REMAINDER;
	        gridbag.setConstraints(p, c);
	        getContentPane().add(p);


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


   class SList extends JPanel implements ItemListener, 
                                         TextListener 
   {
    private JList            sig_list;
    private DefaultListModel list_model = new DefaultListModel();
    private JComboBox        show_type, color, marker;
    private JTextField	     marker_step_t;
    private Vector	         signals = new Vector();
    private String	         shot_str;
    private int		         shots[]=null, list_num_shot = 0;  
    private int              sel_signal = -1;
    private int              UNDEF_SHOT     = -99999;
    
    public SList() 
    {
	    BorderLayout bl= new BorderLayout(25,1);
	    setLayout(bl );

	    lab = new JLabel("Signals list");
	    add("North", lab);

	    sig_list = new JList(list_model);
        JScrollPane scroll_sig_list = new JScrollPane(sig_list);
        sig_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        sig_list.addListSelectionListener(
	        new ListSelectionListener()
	        {
                public void valueChanged(ListSelectionEvent e)
                {
                    SList.this.sel_signal = ((JList)e.getSource()).getSelectedIndex() - 1;
                    SList.this.signalSelect(sel_signal);
                }
	        });

	    
	    sig_list.addKeyListener( 
	        new KeyAdapter()
	        {
	            public void keyPressed(KeyEvent e)
	            {
	                char key  = e.getKeyChar();
	                if(key == KeyEvent.VK_DELETE)
		                removeSignalSetup();	
	            }
	        });
	    add("Center",scroll_sig_list);	

	    JPanel p = new JPanel(new GridLayout(4,1));

	    GridBagLayout gridbag = new GridBagLayout();
	    GridBagConstraints c = new GridBagConstraints();
    	
	    c.gridwidth = 1;			
	    c.gridheight = 1;			
	    c.fill =  GridBagConstraints.HORIZONTAL;
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    c.insets = new Insets(5,5,5,5);

        p.setLayout(gridbag);		


        show_type = new JComboBox();
	    show_type.addItem("Line"); 
	    show_type.addItem("No Line"); 
	    show_type.addItemListener(this);
	    gridbag.setConstraints(show_type, c);
	    p.add(show_type);	

	    color = new JComboBox();
	    SetColorList();	
	    color.addItemListener(this);
	    gridbag.setConstraints(color, c);
	    p.add(color);	

	    marker = new JComboBox();
	    for(int i = 0; i < Signal.markerList.length; i++)
	        marker.addItem(Signal.markerList[i]);

	    marker.addItemListener(this);
	    gridbag.setConstraints(marker, c);
	    p.add(marker);
    	
	    c.gridwidth = 1;
	    lab = new JLabel("M.Step");
	    gridbag.setConstraints(lab, c);
	    p.add(lab);
    	
	    c.fill =  GridBagConstraints.NONE;
	    c.gridwidth = GridBagConstraints.REMAINDER;	
	    marker_step_t = new JTextField(3);
	    //marker_step_t.addTextListener(this);
	    gridbag.setConstraints(marker_step_t, c);
	    p.add(marker_step_t);
    	
	    add("East", p);
    	
    	lab = new JLabel("");
	    add("South", lab);
    	
	    setOptionState(false);

   }
      
      public void setSignalSelect(int sig)
      {
	    sel_signal = sig;
      }
      
      public int getSignalSelect()
      {
	    return sel_signal;
      }
      
      
      private void signalSelect(int sig)
      {
        if(sig+1 >= sig_list.getModel().getSize())
            return;
	    sel_signal = sig;
        sig_list.setSelectedIndex(sel_signal + 1);
        if(sig >= 0)
            if(sel_signal < signals.size())
                putSignalSetup((Data)signals.elementAt(sel_signal));
        else
        	resetSignalSetup();
	    setOptionState(sel_signal >= 0);  
      }
      
      private void setOptionState(boolean state)
      {
	    marker.setEnabled(state);
	    show_type.setEnabled(state);
	    color.setEnabled(state);
      }
      
      public int getNumShot()
      {
        if(shots != null)
	        return shots.length;
	    else
	        return 0;
      }
      
      public void SetColorList()
      {
	    String[] colors_name = main_scope.color_dialog.GetColorsName();
	    if(color.getItemCount() != 0)
	        color.removeAllItems();
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
	    int num_shot = 1;
	    int num_signal = signals.size();  		
          	
          	
        if(shots != null && shots.length > 0)  	
          	num_shot = shots.length;
          	
	    if(sel_signal != -1) 
	    {
    	    start_idx = (sel_signal/num_shot) * num_shot; // Divisione intera
	        end_idx   = start_idx + num_shot; 
	        for (i = 0; i < num_signal; i++)
		    if(i >= start_idx && i < end_idx) 
		    { 
		        list_model.removeElementAt(start_idx + 1);
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
	    show_type.setSelectedIndex((ws.interpolate ? 0 : 1));
	    marker.setSelectedIndex(ws.marker);
	    marker_step_t.setText(""+ws.marker_step);
	    setMarkerTextState(ws.marker);			 
	    color.setSelectedIndex(ws.color_idx);
	    if(error_w.isVisible())
	        error_w.setError(ws);	
      }	
      
      private void resetSignalSetup()
      {
        signal_label.setText("");
	    x_expr.setText("");      
	    y_expr.setText("");      
	    show_type.setSelectedIndex(0);
	    marker.setSelectedIndex(0);
	    marker_step_t.setText("1");
	    setMarkerTextState(0);			 
	    color.setSelectedIndex(0);      
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
	    if(signals.size() != 0)
	        signals.removeAllElements();
      }
      
      public void init(WaveInterface wi)
      {
	    Data ws;
      
	    if(wi != null)
	    {
	        /*
	        shots = new int[wi.num_shot];
	    
	        if(wi.shots == null)
	            for(int i = 0; i < wi.num_shot; i++)
		            shots[i] = jScope.UNDEF_SHOT;	    
	        else
	            for(int i = 0; i < wi.num_shot; i++)
		            shots[i] = wi.shots[i];
		    */
		    
		    if(wi.shots != null)
		    {
	            shots = new int[wi.num_shot];
	            for(int i = 0; i < wi.num_shot; i++)
		            shots[i] = wi.shots[i];
		    }
		    
	        list_num_shot = wi.num_shot;
	        shot_str = wi.in_shot;
	    
	        for(int i = 0; i < wi.num_waves; i++)
	        {
		        ws = new Data();
		        ws.label         = wi.in_label[i];
		        ws.x_expr        = wi.in_x[i];
		        ws.y_expr        = wi.in_y[i];
		        ws.up_err	     = wi.in_up_err[i];
		        ws.low_err	     = wi.in_low_err[i];
		        ws.interpolate   = wi.interpolates[i];
		        ws.marker        = wi.markers[i];	
		        ws.marker_step	 = wi.markers_step[i];
		        ws.color_idx     = wi.colors_idx[i];
		        if(wi.shots != null)
		            ws.shot = wi.shots[i];
		        else
		            ws.shot = UNDEF_SHOT;
		        addSignalSetup(ws);       
	        }
	        signalListRefresh();	    
	    }
	    
	    if(sel_signal == -1 && wi.num_waves > 0)
	        sel_signal = 0;
	        
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
	    if( idx == -1) 
	    {
      
	        String x_e =  new String(x_expr.getText() == null ? "" : x_expr.getText()) ; 
	        String y_e =  new String(y_expr.getText());

	        if(shots != null && shots.length != 0)
	        {
		        for (int i = 0; i < shots.length; i++, ws = getSignalSetup())
		        {
		            ws.shot = shots[i];
		            ws.color_idx = color_idx;
		            color_idx = (color_idx + 1) %  main_scope.color_dialog.GetNumColor();
		            addSignalSetup(ws);
		            signalListAdd(ws);
		        }	
	        } else {
		        ws.shot = UNDEF_SHOT;
		        addSignalSetup(ws);
		        signalListAdd(ws);
	        } 
	        signalSelect(findSignalSetup(ws));	    
	        //putSignalSetup(ws);
	      }	 
       }
       
       public boolean evaluateShotList(String in_shot) throws IOException
       {
        
        if(in_shot != null) in_shot = in_shot.trim();
	    if((shot_str == null && in_shot != null && in_shot.length()!= 0) || 
	       (shot_str != null && !shot_str.equals(in_shot)))
	    {
		    if(shots != null && shots.length != 0)
		        list_num_shot = shots.length;
		    else
		        list_num_shot = 1;
		    shot_str = in_shot;
		    shots = wi.GetShotArray(in_shot);
		    if(shots == null)
		    {
		        shot_str = null;
		    } else {
		        if(image_b.isSelected())
		        {
		            if(shots.length > 1)
		            {
		                int sh[] = new int[1];
		                sh[0]=shots[0];
		                shots = sh;
		                list_num_shot = 1;
		            }
		        }
		    }
		    return true;
	    } else
		    return false;	    
       }


       public void signalsRefresh()
       { 
	    int color_idx = 0, k = 0, l, num_sig, n_shot; 
	    
	    n_shot = (shots != null ? shots.length : 1);
	    num_sig = signals.size()/list_num_shot * n_shot;
    	    
	    for(int j = 0; j < num_sig; j += n_shot)
	    {	    		    
		    for(int i = 0; i < n_shot; i++)
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
		        if(shots != null)
		            ((Data)signals.elementAt(k)).shot = shots[i];
		        else
		            ((Data)signals.elementAt(k)).shot = UNDEF_SHOT;
		        
		        k++;
		    }
		    for(l = n_shot; l < list_num_shot; l++)
		        signals.removeElementAt(j + n_shot);
	     }
       } 

       public void updateSignals()
       {
	        int i, j, sig_idx, start_idx, end_idx;
	        int num_shot = 1;
	        int num_signal = signals.size(); 
      
      
	        if(findSignalSetup(getSignalSetup()) != -1)
	            return;	
          	

	        if(sel_signal != -1) 
	        {
                if(shots != null && shots.length > 0)
                    num_shot = shots.length;
	            
	            start_idx = (sel_signal/num_shot) * num_shot; // Divisione intera
	            end_idx   = start_idx + num_shot; 

	            for (i = 0,  j = 0 ; i < num_signal; i++)
		            if(i >= start_idx && i < end_idx)
		            {
		                ((Data)signals.elementAt(i)).label     = signal_label.getText();
		                ((Data)signals.elementAt(i)).x_expr    = x_expr.getText();
		                ((Data)signals.elementAt(i)).y_expr    = y_expr.getText();
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
//	    if(sig_list.getItemCount() > 1)
        if(!list_model.isEmpty())
		    list_model.removeAllElements();
	    list_model.addElement("Select this item to add new expression");
        if(signals.size() == 0) return;
	    for(int i = 0; i < signals.size(); i++)
		    signalListAdd((Data)signals.elementAt(i));
//	    signalSelect(sel_signal); 	
	  } 

	  private void signalListAdd(Data ws)
	  {
	  
	    if(ws.shot != UNDEF_SHOT)
	    {
    		if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		        list_model.addElement("Y : " + ws.y_expr + " Shot : " + ws.shot);
		    else
		        list_model.addElement("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : " + ws.shot);	
	    } else {
		    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		        list_model.addElement("Y : " + ws.y_expr + " Shot : Undef");
		    else
		        list_model.addElement("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : Undef");	
	    }
	  }
	  
	  private void signalListReplace(int idx, Data ws)
	  {
	    if(ws.shot != UNDEF_SHOT)
	    {
		    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		        list_model.setElementAt("Y : " + ws.y_expr + " Shot : " + ws.shot, idx);
		    else
		        list_model.setElementAt("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : " + ws.shot, idx);	
	    } else {
		    if(ws.x_expr == null || ws.x_expr.length() == 0) 	    
		        list_model.setElementAt("Y : " + ws.y_expr + " Shot : Undef", idx);
		    else
		        list_model.setElementAt("Y : " + ws.y_expr + " X : " + ws.x_expr + " Shot : Undef", idx);	
	    }
	}

      public void updateList() throws IOException
      {
    	    if(evaluateShotList(shot.getText())) 
    	    {
		        signalsRefresh();
		        signalListRefresh();
	        }
    	    if(sel_signal == -1)	    
		        signalList.addSignals();
	        else {
	            if(y_expr.getText().trim().length() != 0)
		            signalList.updateSignals();
		        else
		            removeSignalSetup();
		    }
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
     
//      public void actionPerformed(ActionEvent e) {}
      
      public void itemStateChanged(ItemEvent e)
      {
	    Object ob = e.getSource();

	    if(ob instanceof JCheckBox)
	        DefaultButtonChange(ob);

    	if(sel_signal == -1)
	        return;
        	    
	    if(ob == marker) 
	    {
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


  class ExpandExp extends JDialog implements ActionListener { 

  private JLabel lab_x, lab_y;
  private JTextArea x_expr, y_expr;
  private JButton ok, cancel;
  private SetupDataDialog  conf_dialog;
 
     ExpandExp(Frame _fw, SetupDataDialog conf_diag)
     {
	    super(_fw, "Expand Expression Dialog", false);
//	    super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
	    setModal(true);
	    conf_dialog = conf_diag;
  
        
	    getContentPane().setLayout(new BorderLayout());		

        JPanel p1 = new JPanel();
        p1.setLayout(new BoxLayout(p1, BoxLayout.Y_AXIS));

        JPanel p2 = new JPanel(new FlowLayout(FlowLayout.LEFT));
	    lab_y = new JLabel("Y Expression:");
	    p2.add(lab_y);		 	
        p1.add(p2);
        
	    y_expr = new JTextArea(10,50);
	    y_expr.setBorder(BorderFactory.createLoweredBevelBorder());
	    p1.add(y_expr);
    	  
        JPanel p3 = new JPanel(new FlowLayout(FlowLayout.LEFT));
	    lab_x = new JLabel("X Expression:");	  
	    p3.add(lab_x);		 	
        p1.add(p3);
        
	    x_expr = new JTextArea(10,50);
	    x_expr.setBorder(BorderFactory.createLoweredBevelBorder());
	    p1.add(x_expr);
        
	    JPanel p = new JPanel();
	    p.setLayout(new FlowLayout(FlowLayout.CENTER));
    	    
	    ok = new JButton("Ok");
	    ok.addActionListener(this);	
	    p.add(ok);
        
	    cancel = new JButton("Cancel");
	    cancel.addActionListener(this);	
	    p.add(cancel);
    			    	    
	    getContentPane().add("Center", p1); 
	    getContentPane().add("South", p); 
	    
	    
     }
     
     /**
      ** Set values of x and/or y expression field
     */
     public void setExpressionString(String x, String y)
     {
        if(image_b.isSelected())
        {
	        lab_x.setText("Times Expression:");	  
	        lab_y.setText("Frames Expression:");	              
        } else {
	        lab_x.setText("X Expression:");	  
	        lab_y.setText("Y Expression:");	              
        }
        
	    if(x != null)
	        x_expr.setText(x);
	    if(y != null)
	        y_expr.setText(y);
     }
     
     public void actionPerformed(ActionEvent e)
     {
	    Object ob = e.getSource();	
    
	    if(ob == ok)
	    {
	        conf_dialog.x_expr.setText(x_expr.getText());
	        conf_dialog.y_expr.setText(y_expr.getText());
	    }
	    setVisible(false);	   
     }      

  }


   public SetupDataDialog(Frame fw, String frame_title)
   {
      super(fw, frame_title, false);
      setModal(true);
      setResizable(false);    

      main_scope  = (jScope_1)fw;
      error_w     = new SError(fw);
      expand_expr = new ExpandExp(fw, this);	    

		getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
		setVisible(false);
		
		JPanel p1 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		p1.add(title_b);
		p1.add(title);
		p1.add(expand);
		
		JPanel p2 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		sig_label.setText("Signal Label");
		p2.add(sig_label);
		p2.add(signal_label);
		p2.add(error);
		
		JPanel p3 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		y_lab.setText("Y");
		p3.add(y_lab);
		p3.add(y_expr);
		p3.add(y_log);
        
        JPanel p4 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		p4.add(y_label_b);
		p4.add(y_label);

		pix_y_min.setText("Pixel Y min");
		p4.add(pix_y_min);
        pix_y_min.setVisible(false);

		p4.add(y_min_b);
		p4.add(y_min);
		
		pix_y_max.setText("Pixel Y max");
		p4.add(pix_y_max);
        pix_y_max.setVisible(false);
		
		p4.add(y_max_b);
		p4.add(y_max);
		
		p4.add(image_b);
		
		JPanel p5 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		x_lab.setText("X");
		p5.add(x_lab);
		p5.add(x_expr);

		p5.add(time_min_b);
        time_min_b.setVisible(false);
		p5.add(time_min);
        time_min.setVisible(false);

		p5.add(time_max_b);
		p5.add(time_max);
        time_max_b.setVisible(false);
        time_max.setVisible(false);
		
		p5.add(x_log);
		
		p5.add(use_jai_b);
        use_jai_b.setVisible(false);

        JPanel p6 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		p6.add(x_label_b);
		p6.add(x_label);

		pix_x_min.setText("Pixel X min");
		p6.add(pix_x_min);
		pix_x_min.setVisible(false);

		p6.add(x_min_b);
		p6.add(x_min);
		
		pix_x_max.setText("Pixel X max");
		p6.add(pix_x_max);
		pix_x_max.setVisible(false);

		p6.add(x_max_b);
		p6.add(x_max);

		p6.add(keep_ratio_b);
		keep_ratio_b.setVisible(false);
		
		JPanel p7 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		p7.add(experiment_b);
		p7.add(experiment);
		p7.add(shot_b);
		p7.add(shot);
		
		p7.add(horizontal_flip_b);
		horizontal_flip_b.setVisible(false);
		
		JPanel p8 = new JPanel(new FlowLayout(FlowLayout.LEFT));
		p8.add(upd_event_b);
		p8.add(upd_event);
		p8.add(def_node_b);
		p8.add(def_node);

		p8.add(vertical_flip_b);
        vertical_flip_b.setVisible(false);

        p9.setBorder(BorderFactory.createLoweredBevelBorder());
        signalList = new SList(); 
		p9.add("Center", signalList);

        JPanel p10 = new JPanel();
		p10.setLayout(new FlowLayout());
		p10.setBounds(12,347,660,40);
		p10.add(ok);
		p10.add(apply);
		p10.add(reset);
		p10.add(erase);
		p10.add(cancel);

		getContentPane().add(p1);
		getContentPane().add(p2);
		getContentPane().add(p3);
		getContentPane().add(p4);
		getContentPane().add(p5);
		getContentPane().add(p6);
		getContentPane().add(p7);
		getContentPane().add(p8);
		getContentPane().add(p9);
		getContentPane().add(p10);

      title_b.addItemListener(this);
      expand.addActionListener(this);
      signal_label.addKeyListener(this);
      error.addActionListener(this);
      y_expr.addKeyListener(this);
      y_label_b.addItemListener(this);
      y_min_b.addItemListener(this);
      y_min.addKeyListener(this);
      y_max_b.addItemListener(this);
      y_max.addKeyListener(this);
      image_b.addItemListener(this);
     // use_jai_b.addItemListener(this);
      x_expr.addKeyListener(this);
      x_label_b.addItemListener(this);
      x_min_b.addItemListener(this);
      x_min.addKeyListener(this);
      x_max_b.addItemListener(this);
      x_max.addKeyListener(this);
      time_max_b.addItemListener(this);
      time_max.addKeyListener(this);
      time_min_b.addItemListener(this);
      time_min.addKeyListener(this);
      experiment_b.addItemListener(this);
      experiment.addKeyListener(this);
      shot_b.addItemListener(this);      
      shot.addKeyListener(this);
      //shot.addFocusListener(this);
      upd_event_b.addItemListener(this);
      def_node_b.addItemListener(this);      
      ok.addActionListener(this);
      apply.addActionListener(this);
      reset.addActionListener(this);
      erase.addActionListener(this);
      cancel.addActionListener(this);
   }
   

    public void Show(Waveform w, int col, int row)
    {
      wave_coord = new Point(row, col);
      wave = (jScopeMultiWave)w;
      wave.addWaveformListener(this);
      wi = wave.wi;
      wi = new MdsWaveInterface(wave, wave.wi.dp, wave.wi.def_vals);
      wi.defaults = wave.wi.defaults;
      putWindowSetup((MdsWaveInterface)wave.wi);
      updateDataSetup();
      setLocationRelativeTo(w.getParent());
      this.signalList.signalSelect(wave.GetSelectedSignal());
      setTitle("Wave Setup for column " + col + " row " + row);
      jScope_1.jScopeSetUI(this);
      jScope_1.jScopeSetUI(error_w);
	  error_w.pack();
      jScope_1.jScopeSetUI(expand_expr);
	  expand_expr.pack();
      pack();
      show(); 
   }
   
   
   public void SetColorList()
   {
	    signalList.SetColorList();
   }

   public void selectSignal(int sig)
   {
      signalList.setSignalSelect(sig);
   }

   private void resetDefaultFlags()
   {
    boolean state = true;
    wi.defaults = 0xffffffff;
    if(wi.is_image)
        wi.defaults = wi.defaults &  ~( (1 << MdsWaveInterface.B_y_max) + 
                                                      (1 << MdsWaveInterface.B_y_min));
    
        
	title_b.setSelected(state);
	title.setEditable(!state); 
	shot_b.setSelected(state); 
	shot.setEditable(!state); 
	experiment_b.setSelected(state); 
	experiment.setEditable(!state);
	if(image_b.isSelected())
	{
	    time_max_b.setSelected(state); 
	    time_max.setEditable(!state); 
	    time_min_b.setSelected(state); 
	    time_min.setEditable(!state);

	    y_max_b.setSelected(false); 
	    y_max.setEditable(true); 
	    y_min_b.setSelected(false); 
	    y_min .setEditable(true); 

	    x_max_b.setSelected(false); 
	    x_max.setEditable(true); 
	    x_min_b.setSelected(false); 
	    x_min.setEditable(true);
	
	} else {

	    y_max_b.setSelected(state); 
	    y_max.setEditable(!state); 
	    y_min_b.setSelected(state); 
	    y_min .setEditable(!state); 

	    x_max_b.setSelected(state); 
	    x_max.setEditable(!state); 
	    x_min_b.setSelected(state); 
	    x_min.setEditable(!state);
	}
	x_label_b.setSelected(state); 
	x_label.setEditable(!state); 
	y_label_b.setSelected(state);  
	y_label.setEditable(!state);
	upd_event_b.setSelected(state);
	upd_event.setEditable(!state);
    def_node_b.setSelected(state);
    def_node.setEditable(!state);
    PutDefaultValues();
   } 
     
   private void setDefaultFlags(int flags)
   {   
	for(int i=0; i < 32; i++)
	{
	    switch(i)
	    {
		case MdsWaveInterface.B_title:
		    title_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_shot:
		    shot_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_exp:
		    experiment_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_x_max:
		    if(image_b.isSelected())
		        time_max_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);
		    else        
		        x_max_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);
		break; 
		case MdsWaveInterface.B_x_min:
		    if(image_b.isSelected())
		        time_min_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false); 
		    else
		        x_min_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);
		break; 
		case MdsWaveInterface.B_x_label:
		    x_label_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_y_max:
		    y_max_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_y_min:
		    y_min_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break; 
		case MdsWaveInterface.B_y_label:
		    y_label_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break;  
		case MdsWaveInterface.B_event:
		    upd_event_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break;  
		case MdsWaveInterface.B_default_node:
		    def_node_b.setSelected(((flags & (1<<i)) == 1<<i)?true:false);break;  
	    }
	}       
   }
   
   private int getDefaultFlags()
   {
       int value = 0;
       
	if  (title_b.isSelected())	 value |= 1<<MdsWaveInterface.B_title; 
				    else value &= ~(1<<MdsWaveInterface.B_title); 
	if  (shot_b.isSelected()  )	 value |= 1<<MdsWaveInterface.B_shot  ; 
				    else value &= ~(1<<MdsWaveInterface.B_shot); 
	if  (experiment_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_exp ; 
				    else value &= ~(1<<MdsWaveInterface.B_exp);
	if(image_b.isSelected())
	{
	    if  (time_max_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_x_max ; 
				    else value &= ~(1<<MdsWaveInterface.B_x_max); 
	    if  (time_min_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_x_min ;
				    else value &= ~(1<<MdsWaveInterface.B_x_min);
	}
	else
	{
	    if  (x_max_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_x_max ; 
				    else value &= ~(1<<MdsWaveInterface.B_x_max); 
	    if  (x_min_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_x_min ;
				    else value &= ~(1<<MdsWaveInterface.B_x_min);
	}
	if  (x_label_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_x_label ; 
				    else value &= ~(1<<MdsWaveInterface.B_x_label); 
	if  (y_max_b.isSelected() )        value |= 1<<MdsWaveInterface.B_y_max ; 
				    else value &= ~(1<<MdsWaveInterface.B_y_max); 
	if  (y_min_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_y_min ; 
				    else value &= ~(1<<MdsWaveInterface.B_y_min); 
	if  (y_label_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_y_label ; 
				    else value &= ~(1<<MdsWaveInterface.B_y_label);
	if  (upd_event_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_event ; 
				    else value &= ~(1<<MdsWaveInterface.B_event);
	if  (def_node_b.isSelected() )	 value |= 1<<MdsWaveInterface.B_default_node ; 
				    else value &= ~(1<<MdsWaveInterface.B_default_node);
	return (value);  
   }

   public void PutDefaultValues()
   { 
        boolean def_flag;
        
	    defaultButtonOperation(title, def_flag = title_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_title, def_flag));
	    putShotValue(shot_b.isSelected());
	    defaultButtonOperation(experiment, def_flag = experiment_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_exp, def_flag));
	    if(image_b.isSelected())
	    {
	        defaultButtonOperation(time_max, def_flag = time_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_max, def_flag));
	        if(!def_flag)
	            time_max.setText(wi.cin_timemax);
	        defaultButtonOperation(time_min, def_flag = time_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_min, def_flag));
	        if(!def_flag)
	            time_min.setText(wi.cin_timemin);
	        x_min.setText(wi.cin_xmin);
	        x_max.setText(wi.cin_xmax);
	    } else {
	        defaultButtonOperation(x_max, def_flag = x_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_max, def_flag));
	        defaultButtonOperation(x_min, def_flag = x_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_min, def_flag));
	    }
	    defaultButtonOperation(x_label, def_flag = x_label_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_label, def_flag));
	    defaultButtonOperation(y_max, def_flag = y_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_max, def_flag));
	    defaultButtonOperation(y_min, def_flag = y_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_min, def_flag));
	    defaultButtonOperation(y_label, def_flag = y_label_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_label, def_flag));
	    defaultButtonOperation(upd_event, def_flag = upd_event_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_event, def_flag));
	    defaultButtonOperation(def_node,  def_flag = def_node_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_default_node, def_flag));    
   }
   
   public void putWindowSetup(MdsWaveInterface wi)      
   {	
	boolean def_flag;
		
	if(wi == null) {
	    eraseForm();	    
	    return;
	}


    if(wi.is_image)
        wi.defaults = wi.defaults &  ~( (1 << MdsWaveInterface.B_y_max) + 
                                                (1 << MdsWaveInterface.B_y_min));

    setImageDialog(wi.is_image);

	this.wi.cexperiment     = wi.cexperiment;	
	this.wi.cin_shot        = wi.cin_shot;	
	
	this.wi.cin_upd_event   = wi.cin_upd_event;	

	this.wi.in_upd_event    = wi.in_upd_event;	
	
	this.wi.last_upd_event  = wi.last_upd_event;	
	this.wi.cin_def_node    = wi.cin_def_node;	
    this.wi.cin_xmax        = wi.cin_xmax;
	this.wi.cin_xmin        = wi.cin_xmin;
	this.wi.cin_ymax        = wi.cin_ymax;
	this.wi.cin_ymin        = wi.cin_ymin;

	this.wi.cin_timemin        = wi.cin_timemin;
	this.wi.cin_timemax        = wi.cin_timemax;
	
	this.wi.cin_title       = wi.cin_title;
	this.wi.cin_xlabel      = wi.cin_xlabel;
	this.wi.cin_ylabel      = wi.cin_ylabel;
	this.wi.legend_x        = wi.legend_x;
	this.wi.legend_y        = wi.legend_y;
    this.wi.show_legend     = wi.show_legend;
    this.wi.reversed        = wi.reversed;
    image_b.setSelected(wi.is_image);
    use_jai_b.setSelected(wi.use_jai);
    keep_ratio_b.setSelected(wi.keep_ratio);
    horizontal_flip_b.setSelected(wi.horizontal_flip);
    vertical_flip_b.setSelected(wi.vertical_flip);


	setDefaultFlags(wi.defaults);
	PutDefaultValues();
	

//    if(!wi.is_image)
//    {
        signal_label.setText("");
        x_expr.setText("");
        y_expr.setText("");
	    x_log.setSelected(wi.x_log);
	    y_log.setSelected(wi.y_log);
//	}
//	else {
 //       x_expr.setText("");
 //      if(wi.in_label != null && wi.in_label[0].trim().length() > 0)
 //           signal_label.setText(wi.in_label[0]);
 //       if(wi.in_y != null && wi.in_y[0].trim().length() > 0)
 //           y_expr.setText(wi.in_y[0]);
//	}


    signalList.init(wi);
   }
   
  private void setImageDialog(boolean state)
  {
    if(state)
    {
        sig_label.setVisible(false);
        signal_label.setVisible(false);

        time_min_b.setVisible(true);
        time_min.setVisible(true);
        time_max_b.setVisible(true);
        time_max.setVisible(true);

        
		pix_x_max.setVisible(true);
		pix_x_min.setVisible(true);
		x_max_b.setVisible(false);
		x_min_b.setVisible(false);

		pix_y_max.setVisible(true);
		pix_y_min.setVisible(true);
		y_max_b.setVisible(false);
		y_min_b.setVisible(false);

        x_log.setVisible(false);
        y_log.setVisible(false); 
        signalList.setVisible(false);        
        p9.setVisible(false);        
        error.setVisible(false);
        use_jai_b.setVisible(true);
        keep_ratio_b.setVisible(true);
        horizontal_flip_b.setVisible(true);
        vertical_flip_b.setVisible(true);
//        panel1.setBounds(12,197,660,40);
		y_lab.setText("Frames");
//		y_lab.setBounds(10,55,35,20);
//		y_expr.setBounds(47,54,577,20);
		x_lab.setText("Times");
//		x_lab.setBounds(10,103,35,20);
        x_expr.setColumns(24);
//		x_expr.setBounds(47,102,271,20);
		//setSize(730,255);
    } else {

        sig_label.setVisible(true);
        signal_label.setVisible(true);
        
        time_min_b.setVisible(false);
        time_min.setVisible(false);
        time_max_b.setVisible(false);
        time_max.setVisible(false);

		pix_x_max.setVisible(false);
		pix_x_min.setVisible(false);
		x_max_b.setVisible(true);
		x_min_b.setVisible(true);

		pix_y_max.setVisible(false);
		pix_y_min.setVisible(false);
		y_max_b.setVisible(true);
		y_min_b.setVisible(true);

        x_log.setVisible(true);
        y_log.setVisible(true); 
        signalList.setVisible(true);        
        p9.setVisible(true);        
        error.setVisible(true);
        x_lab.setVisible(true);
        use_jai_b.setVisible(false);
        keep_ratio_b.setVisible(false);
        horizontal_flip_b.setVisible(false);
        vertical_flip_b.setVisible(false);
//		panel1.setBounds(12,347,660,40);
		y_lab.setText("Y");
//		getContentPane().add(y_lab);
//		y_lab.setBounds(10,55,12,20);
//		y_expr.setBounds(28,54,596,20);
		x_lab.setText("X");
//		x_lab.setBounds(10,103,12,20);
        x_expr.setColumns(58);
//		x_expr.setBounds(28,102,596,20);
        //pack();
		//setSize(730,405);
    }
    pack();
    validate();
  }

  public void eraseForm()
  {
	title.setText("");
	signal_label.setText("");
	x_expr.setText("");
	x_label.setText("");
	x_label.setForeground(Color.black);		
	time_max.setText("");
	time_max.setForeground(Color.black);		
	x_max.setText("");
	x_max.setForeground(Color.black);		
	x_min.setText("");
	x_min.setForeground(Color.black);		
	time_min.setText("");
	time_min.setForeground(Color.black);		
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
//	if(!wi.is_image)
	{
	    signalList.reset();
	    signalList.signalListRefresh();

	}
  } 
 

    public boolean isChanged(Data s[])
    {
        
	MdsWaveInterface wave_wi = (MdsWaveInterface)wave.wi;

    	if(wave_wi == null) return true;
	
	if(wave_wi.modified) return true;
    
	if(s.length     != wave_wi.num_waves) return true;
	if(signalList.getNumShot() != wave_wi.num_shot) return true;
	
	if(!main_scope.equalsString(title.getText(),   wave_wi.cin_title))    return true;	
	if(!main_scope.equalsString(x_max.getText(),   wave_wi.cin_xmax))     return true;
	if(!main_scope.equalsString(x_min.getText(),   wave_wi.cin_xmin))     return true;
	if(image_b.isSelected())
	{
	    if(!main_scope.equalsString(time_max.getText(),   wave_wi.cin_timemax))     return true;
	    if(!main_scope.equalsString(time_min.getText(),   wave_wi.cin_timemin))     return true;
	}
	
	if(!main_scope.equalsString(x_label.getText(), wave_wi.cin_xlabel))   return true;
	if(x_log.isSelected() != wave_wi.x_log)				                  return true;
	if(!main_scope.equalsString(y_max.getText(),   wave_wi.cin_ymax))     return true;
	if(!main_scope.equalsString(y_min.getText(),   wave_wi.cin_ymin))     return true;
	if(!main_scope.equalsString(y_label.getText(), wave_wi.cin_ylabel))   return true;	
	if(y_log.isSelected() != wave_wi.y_log)				                      return true;		
    if(!main_scope.equalsString(shot.getText(),   wave_wi.cin_shot))          return true;	
    if(!main_scope.equalsString(upd_event.getText(),   wave_wi.cin_upd_event))  return true;	
    if(!main_scope.equalsString(def_node.getText(),    wave_wi.cin_def_node))   return true;	
	if(!main_scope.equalsString(experiment.getText(), wave_wi.cexperiment))   return true;
	if(getDefaultFlags() != wave_wi.defaults)				                  return true;	
	if(image_b.isSelected() != wave_wi.is_image)				                  return true;		
	if(use_jai_b.isSelected() != wave_wi.use_jai)				                  return true;		
	if(keep_ratio_b.isSelected() != wave_wi.keep_ratio)				          return true;		
	if(horizontal_flip_b.isSelected() != wave_wi.horizontal_flip)				  return true;		
	if(vertical_flip_b.isSelected() != wave_wi.vertical_flip)				      return true;		

    for(int i = 0 ; i < wave_wi.num_waves; i++)
	{
	    if(!main_scope.equalsString(s[i].x_expr,  wave_wi.in_x[i]))        return true;
	    if(!main_scope.equalsString(s[i].y_expr,  wave_wi.in_y[i]))        return true;
	    if(!main_scope.equalsString(s[i].up_err,  wave_wi.in_up_err[i]))   return true;
	    if(!main_scope.equalsString(s[i].low_err, wave_wi.in_low_err[i]))  return true;
	}
	
	return false;
    }

    private void updateDataSetup()
    {        
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        try
        {
	        signalList.updateList();
	    } catch (Throwable e) {
	        main_scope.SetStatusLabel("Error during update list "+e);	    
	    }
        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
   }

   private void updateGlobalWI()
   {
    
      if(!experiment_b.isSelected() && !main_scope.equalsString(experiment.getText(), wi.cexperiment))
	    wi.cexperiment   = experiment.getText();	
      if(!shot_b.isSelected() && !main_scope.equalsString(shot.getText(),   wi.cin_shot))
	    wi.cin_shot      = shot.getText();	
      if(!upd_event_b.isSelected() && !main_scope.equalsString(upd_event.getText(),   wi.cin_upd_event))
	    wi.cin_upd_event      = upd_event.getText();	
      if(!def_node_b.isSelected() && !main_scope.equalsString(def_node.getText(),   wi.cin_def_node))
	    wi.cin_def_node       = def_node.getText();

      if(!time_max_b.isSelected() && !main_scope.equalsString(time_max.getText(),   wi.cin_timemax))
	    wi.cin_timemax      = time_max.getText();
      if(!time_min_b.isSelected() && !main_scope.equalsString(time_min.getText(),   wi.cin_timemin))
	    wi.cin_timemin      = time_min.getText();
	    
      if(!x_max_b.isSelected() && !main_scope.equalsString(x_max.getText(),   wi.cin_xmax))
	    wi.cin_xmax      = x_max.getText();
      if(!x_min_b.isSelected() && !main_scope.equalsString(x_min.getText(),   wi.cin_xmin))
	    wi.cin_xmin      = x_min.getText();
	    
      if(!y_max_b.isSelected() && !main_scope.equalsString(y_max.getText(),   wi.cin_ymax))
	    wi.cin_ymax      = y_max.getText();
      if(!y_min_b.isSelected() && !main_scope.equalsString(y_min.getText(),   wi.cin_ymin))
	    wi.cin_ymin      = y_min.getText();
      if(!title_b.isSelected() && !main_scope.equalsString(title.getText(),   wi.cin_title))
	    wi.cin_title     = title.getText();
      if(!x_label_b.isSelected() && !main_scope.equalsString(x_label.getText(), wi.cin_xlabel))
	    wi.cin_xlabel    = x_label.getText();
      if(!y_label_b.isSelected() && !main_scope.equalsString(y_label.getText(), wi.cin_ylabel))
	    wi.cin_ylabel    = y_label.getText();
	    
   }

   private int updateWI()
   {
	  Data[] s;
	  int num_signal;
	      
	  s = signalList.getSignals();
	  num_signal = s.length;

 	  if(num_signal == 0)
 	  {
//	      wave.wi = new MdsWaveInterface(main_scope.db, wave.controller, main_scope.def_values);
//	      wi.is_image = image_b.isSelected();
//	      wi.use_jai = use_jai_b.isSelected();
          if(wave.wi != null)
            wave.wi.Erase();
	      return 1;
      }

	  wi.modified = isChanged(s);
	  wi.is_image = image_b.isSelected();
	  wi.use_jai = use_jai_b.isSelected();
	  wi.keep_ratio = keep_ratio_b.isSelected();
	  wi.horizontal_flip = horizontal_flip_b.isSelected();
	  wi.vertical_flip = vertical_flip_b.isSelected();
	  
	  if(!wi.modified)
	  {
		wave.wi.full_flag = !main_scope.wave_panel.GetFastNetworkState();
        if(wi.is_image)
            return 0;

		for(int i = 0; i < wave.wi.num_waves; i++)      
		{
		    wave.wi.markers[i]      = s[i].marker;
		    wave.wi.markers_step[i] = s[i].marker_step;
		    wave.wi.interpolates[i] = s[i].interpolate;
//		    wave.wi.colors[i]       = main_scope.color_dialog.GetColorAt(s[i].color_idx);
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
     // wi.in_upd_event   = new String(upd_event.getText());
     // wi.cin_upd_event   = new String(upd_event.getText());
	  wi.in_xmax        = new String(x_max.getText());
	  wi.in_xmin        = new String(x_min.getText());

	  wi.in_timemax        = new String(time_max.getText());
	  wi.in_timemin        = new String(time_min.getText());
	  
	  wi.in_ymax        = new String(y_max.getText());
	  wi.in_ymin        = new String(y_min.getText());
	  wi.in_title       = new String(title.getText());
	  wi.in_xlabel      = new String(x_label.getText());
	  wi.in_ylabel      = new String(y_label.getText());
	    
	  wi.x_log        = x_log.isSelected();
	  wi.y_log        = y_log.isSelected();
	  wi.full_flag    = !main_scope.wave_panel.GetFastNetworkState();
	  wi.num_shot     = signalList.getNumShot();
	  wi.defaults     = getDefaultFlags();
     
	  wi.in_label     = new String[num_signal];
	  wi.in_x         = new String[num_signal];
	  wi.in_y         = new String[num_signal];
	  
	  wi.in_up_err    = new String[num_signal];
      wi.in_low_err   = new String[num_signal];
      wi.markers      = new int[num_signal];
      wi.markers_step = new int[num_signal];	  
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
		        
	        if(wi.shots != null)
		        wi.shots[i]        = s[i].shot;
		        
		    if(!wi.is_image)
		    {
	            wi.markers[i]      = s[i].marker;
	            wi.markers_step[i] = s[i].marker_step;
	            wi.interpolates[i] = s[i].interpolate;
	            if(s[i].up_err != null)
		            wi.in_up_err[i] = new String(s[i].up_err);
	            if(s[i].low_err != null)      
		            wi.in_low_err[i] = new String(s[i].low_err);	    
//	            wi.colors[i]       = main_scope.color_dialog.GetColorAt(s[i].color_idx);
	            wi.colors_idx[i]   = s[i].color_idx;
	        }
	      }
	      
	      //if(wi.shots[0] == jScope.UNDEF_SHOT)
	   	  //   wi.shots = null;

	  	    
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
		        JOptionPane.showMessageDialog(null, "Experiment defined but undefined shot", "alert", JOptionPane.ERROR_MESSAGE); 
                return 1;
		    }    
		    /*
	        if(!def_exp && !setup.data_server_address.equals("Ftu data") && !image_b.isSelected()) {	    
		        error_msg.addMessage("Shot defined but undefined experiment\n");
	            error = 1;
	        }
	        */
	    }

	    updateDataSetup();
	
	    if(updateWI() != 0)
	    {
		    JOptionPane.showMessageDialog(null, "Nothing to evaluate", "alert", JOptionPane.ERROR_MESSAGE); 
	        error = 1;
	    }
    	
	    return error;

    }


   private  void applyWaveform()
   {
        //if(checkSetup() == 0)
        checkSetup();
        {       
            setCursor(new Cursor(Cursor.WAIT_CURSOR));
            try
            {
                main_scope.wave_panel.Refresh(wave, "Update ");
	        } catch (Throwable e) {	        
	            main_scope.SetStatusLabel("Error during apply: "+e);	    
                setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	        }
        } 
        //else
	    //  wave.Erase();
   }
	    	
    public void processWaveformEvent(WaveformEvent e) 
    {
        jScopeMultiWave w = (jScopeMultiWave)e.getSource();
        switch(e.getID())
        {
             case WaveformEvent.END_UPDATE :
                String full_error = w.wi.getErrorString(main_scope.wave_panel.getBriefError());
                if(full_error != null)
                {
		            JOptionPane.showMessageDialog(SetupDataDialog.this, full_error, 
		                                            "alert", JOptionPane.ERROR_MESSAGE);
                }
                setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
             break;
        }
    }
   public void actionPerformed(ActionEvent e)
   {
      Object ob = e.getSource();

      if(getCursor().getType() == Cursor.WAIT_CURSOR)
        return;

      if(ob == erase) {
	    eraseForm();
//	    wave.wi = new MdsWaveInterface(main_scope.db, main_scope.def_values);
//	    wave.wi.is_image = image_b.isSelected();
//	    wave.jScopeErase();
      }
		
      if(ob == cancel) {
        wave.removeWaveformListener(this);
	    setVisible(false);
	    signalList.reset();
      }
	
      if(ob == apply || ob == ok)
      {
            applyWaveform();
            if(ob == ok) 
            {
                wave.removeWaveformListener(this);
                setVisible(false);
                signalList.reset();
            }
	   }
 
      if(ob == reset) 
      {
	        signalList.reset();
	        putWindowSetup((MdsWaveInterface)wave.wi); 	
      }
      
      if(ob == error && y_expr.getText().trim().length() != 0)
      {
	        updateDataSetup();
	        signalList.updateError();
	        error_w.setLocationRelativeTo(this);
	        error_w.show();
      }
      
      if(ob == expand)
      {	
	        expand_expr.setExpressionString(x_expr.getText(), y_expr.getText());
	        expand_expr.setLocationRelativeTo(this);	        
	        expand_expr.show();
      }	
   }

   public void keyReleased(KeyEvent e){}
   public void keyTyped(KeyEvent e){}
   
   public void keyPressed(KeyEvent e)
   {
      Object ob = e.getSource();
      char key  = e.getKeyChar();
      
      
     if(key == KeyEvent.CHAR_UNDEFINED)
	    return;		  	      	

     if(key == KeyEvent.VK_ENTER)
     { 
	    if(ob == y_expr || ob == x_expr || ob == shot || ob == experiment || ob == signal_label) {
	      updateDataSetup();
        } 
     }
      
     if(ob instanceof TextField)
     {
	    if(ob == x_max || ob == y_max || ob == x_min 
	       || ob == y_min || ob == shot || ob == time_max || ob == time_min)
	    {
	        Character ch = new Character((char)key);
            if(!ch.isDigit((char)key) && 
		    key != KeyEvent.VK_DELETE && 
	        key != '.' && key != '+' && key != '-')
		    return;
        }
      } 
   }
   
   private void  defaultButtonOperation(JTextField text, boolean state, String val)
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
        if(def_flag)
            wi.defaults |= (1 << MdsWaveInterface.B_shot);
        else
            wi.defaults &= ~(1 << MdsWaveInterface.B_shot);
        
	    switch(wi.GetShotIdx())
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
        shot.setText(wi.GetUsedShot());	 
   }

   private void  DefaultButtonChange(Object ob)
   {
    
	boolean def_flag;
   
	if(ob == title_b)
	    defaultButtonOperation(title, def_flag = title_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_title, def_flag));
    if(ob == shot_b) 
	    putShotValue(shot_b.isSelected());
	if(ob == experiment_b)
	    defaultButtonOperation(experiment, def_flag = experiment_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_exp, def_flag));
	if(ob == upd_event_b)
	    defaultButtonOperation(upd_event, def_flag = upd_event_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_event, def_flag));
	if(ob == def_node_b)
	    defaultButtonOperation(def_node, def_flag = def_node_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_default_node, def_flag));
	if(ob == x_max_b)
	    defaultButtonOperation(x_max, def_flag = x_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_max, def_flag));
	if(ob == x_min_b)
	    defaultButtonOperation(x_min, def_flag = x_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_min, def_flag));
	if(ob == time_max_b)
	    defaultButtonOperation(time_max, def_flag = time_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_max, def_flag));
	if(ob == time_min_b)
	    defaultButtonOperation(time_min, def_flag = time_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_min, def_flag));
	if(ob == x_label_b)
	    defaultButtonOperation(x_label, def_flag = x_label_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_x_label, def_flag));
	if(ob == y_max_b)
	    defaultButtonOperation(y_max, def_flag = y_max_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_max, def_flag));
	if(ob == y_min_b)
	    defaultButtonOperation(y_min, def_flag = y_min_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_min, def_flag));
	if(ob == y_label_b)
	    defaultButtonOperation(y_label, def_flag = y_label_b.isSelected(), wi.GetDefaultValue(MdsWaveInterface.B_y_label, def_flag));
   }

   public void itemStateChanged(ItemEvent e)
   {
	    Object ob = e.getSource();
	    if(ob instanceof JCheckBox)
	    {
	        if(ob == image_b)
            {
                eraseForm();
                setImageDialog(image_b.isSelected());
                return;
            }
	        DefaultButtonChange(ob);
        }
    }   
}

