import java.io.*;
import java.awt.*;
import java.net.*;
/*
import Signal;
import Waveform;  
import MultiWaveform;
*/

class WindowSetupInfo {
   String title;
   String x_label;
   String y_label;
   int    grid_mode;
   int    num_shot;  
   int[]  shots;
   String x_max, x_min;
   String y_max, y_min; 
   int    num_signal;
   SignalSetupInfo[] signals;
}

class SignalSetupInfo {
   boolean update_flag;
   String  x_expr;
   String  y_expr;
   int     color_idx;
   Color   color;
   boolean interpolation;
   int     marker;
   int     error;
   String  error_up;     	 
   String  error_down;
   Signal  shot_wave[];

   public boolean equals(SignalSetupInfo ws)
   {
     if(!x_expr.equals(ws.x_expr) || !y_expr.equals(ws.y_expr)) 
        return false;
     return true;  
   }
}

class ErrorW extends Frame {
   TextField e_up, e_down;
   Button ok, cancel;

   ErrorW() {
     setTitle("Error Setup");

     setLayout(new GridLayout(3, 0));
     add(new Label("Error up"));
     add(e_up = new TextField(20));


     add(new Label("Error down"));
     add(e_down = new TextField(20));
		
     add(ok = new Button("Ok"));
     add(cancel = new Button("Cancel"));
   } 

   public void resetError()
   {
     e_up.setText("");
     e_down.setText("");
   }	

   public boolean action(Event e, Object o)
   {

     if(e.target == ok) {
       setVisible(false);	
     }

     if(e.target == cancel) {
       setVisible(false);	
     }

     return false;

   }
}

//class WSetup extends Frame {
 class SetupData extends Dialog { 
   TextField   title, shot;
   TextField   x_max, x_min, x_label, y_expr;
   TextField   y_max, y_min, y_label, x_expr;
   Button      browser, error, y_browser;
   Button      ok, cancel, reset, erase, apply, x_browser, add_shot;
   List        sig_list, shot_list;
   Choice      show_type, color, marker, grid_mode;
   Color[]     color_set = {Color.black, Color.blue, Color.cyan, Color.darkGray,
			    Color.gray, Color.green, Color.lightGray, 
			    Color.magenta, Color.orange, Color.pink, 
			    Color.red, Color.yellow};
   Label       lab;
   ErrorW      error_w = new ErrorW(); 
   WindowSetupInfo  w_setup;
   MultiWaveform    wave;
   int sel_signal;  
   
   static final int  LINE = 0, POINT = 1, BOTH = 2, NONE = 3; 	 	



//   WSetup() {
     public SetupData(Frame fw) {


      super(fw, "Window Setup", false); 	
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(4, 4, 4, 4);

      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setLayout(gridbag);		
      c.insets = insets;	
      c.fill =  GridBagConstraints.BOTH;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      lab = new Label("Title");
      gridbag.setConstraints(lab, c);
      add(lab);
 
      c.gridwidth = 4;	
      title = new TextField(40);
      gridbag.setConstraints(title, c);
      add(title);		 	

      lab = new Label("Grid");
      c.fill =  GridBagConstraints.NONE;
      c.anchor = GridBagConstraints.EAST;
      gridbag.setConstraints(lab, c);
      add(lab);

      c.gridwidth = GridBagConstraints.REMAINDER;
      c.anchor = GridBagConstraints.WEST;
      c.gridheight = 1;	
      grid_mode = new Choice();
      grid_mode.addItem("DOTTED"); 
      grid_mode.addItem("GRAY"); 
      grid_mode.addItem("NONE");
      gridbag.setConstraints(grid_mode, c);
      add(grid_mode);	
       	

      c.gridwidth = 1;
      lab = new Label("Y");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = GridBagConstraints.REMAINDER;
      y_expr = new TextField(30);
      gridbag.setConstraints(y_expr, c);
      add(y_expr);		 	

      c.fill =  GridBagConstraints.NONE;		 	
      c.gridwidth = 1;
      lab = new Label("Y Label");
      gridbag.setConstraints(lab, c);
      add(lab);	

      y_label = new TextField(25);		 	
      gridbag.setConstraints(y_label, c);
      add(y_label);	

      lab = new Label("Y min");
      gridbag.setConstraints(lab, c);
      add(lab);	

      y_min = new TextField(10);		 	
      gridbag.setConstraints(y_min, c);
      add(y_min);	

      lab = new Label("Y max");
      gridbag.setConstraints(lab, c);
      add(lab);	
	 	
      y_max = new TextField(10);
      gridbag.setConstraints(y_max, c);
      add(y_max);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      y_browser = new Button("Browse...");
      gridbag.setConstraints(y_browser, c);
      add(y_browser);	

      c.gridwidth = 1;
      lab = new Label("X");
      gridbag.setConstraints(lab, c);
      add(lab);		 	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = GridBagConstraints.REMAINDER;
      x_expr = new TextField(30);
      gridbag.setConstraints(x_expr, c);
      add(x_expr);		 	

      c.fill =  GridBagConstraints.NONE;		 	
      c.gridwidth = 1;
      lab = new Label("X Label");
      gridbag.setConstraints(lab, c);
      add(lab);	

      x_label = new TextField(25);		 	
      gridbag.setConstraints(x_label, c);
      add(x_label);	

      lab = new Label("X min");
      gridbag.setConstraints(lab, c);
      add(lab);	

      x_min = new TextField(10);		 	
      gridbag.setConstraints(x_min, c);
      add(x_min);	

      lab = new Label("X max");
      gridbag.setConstraints(lab, c);
      add(lab);	
	 	
      x_max = new TextField(10);
      gridbag.setConstraints(x_max, c);
      add(x_max);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      x_browser = new Button("Browse...");
      gridbag.setConstraints(x_browser, c);
      add(x_browser);	

      c.gridwidth = 5;
      lab = new Label("Signals list");       	
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      lab = new Label("Shot list");       	
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 5;
      c.gridheight = 4;
      sig_list = new List();
      gridbag.setConstraints(sig_list, c);
      add(sig_list);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 1;
      c.gridheight = 1;	
      shot = new TextField();
      gridbag.setConstraints(shot, c);
      add(shot);

      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      show_type = new Choice();
      show_type.addItem("Line"); 
      show_type.addItem("No Line"); 
      gridbag.setConstraints(show_type, c);
      add(show_type);	

      c.fill =  GridBagConstraints.BOTH;
      c.gridwidth = 1;
      c.gridheight = 3;	
      shot_list = new List();		
      gridbag.setConstraints(shot_list, c);
      add(shot_list);	

      c.fill =  GridBagConstraints.NONE;
      c.gridwidth = GridBagConstraints.REMAINDER;
      c.gridheight = 1;	
      color = new Choice();
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

      gridbag.setConstraints(color, c);
      add(color);	
 	
      marker = new Choice();
      marker.addItem("No Marker"); 
      marker.addItem("Square"); 
      marker.addItem("Circle"); 
      marker.addItem("Cross"); 

      gridbag.setConstraints(marker, c);
      add(marker);	

      c.gridwidth = 1;
      c.fill =  GridBagConstraints.NONE;      
      c.gridwidth = GridBagConstraints.REMAINDER;
      c.gridheight = 1;	
      error = new Button("Error...");
      gridbag.setConstraints(error, c);
      add(error);

      c.anchor = GridBagConstraints.CENTER;
      c.gridwidth = 1;
      ok = new Button("Ok");
      gridbag.setConstraints(ok, c);
      add(ok);	
      	
      apply = new Button("Apply");
      gridbag.setConstraints(apply, c);
      add(apply);	

      c.gridwidth = 2;
      reset = new Button("Reset");
      gridbag.setConstraints(reset, c);
      add(reset);	

      erase = new Button("Erase");
      gridbag.setConstraints(erase, c);
      add(erase);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      cancel = new Button("Cancel");
      gridbag.setConstraints(cancel, c);
      add(cancel);

   }

   public void applySignalsParam(WindowSetupInfo win, MultiWaveform w)
   {
      int num_sig = win.num_signal * win.num_shot;
      SignalSetupInfo[]   ws = win.signals;
      int i,j,k, r, g, b;
      float cf;	 

      Color[]   colors        = new Color[num_sig ];
      Signal[]  signals       = new Signal[num_sig];
      int[]     markers       = new int[num_sig]; 
      boolean[] interpolates  = new boolean[num_sig];  
      

//      w.SetGridMode(grid_mode.getSelectedIndex());   

      if(title.getText().length() != 0)	
//         w.SetTitle(title.getText());
      if(x_label.getText().length() != 0)
 //        w.SetLabelX(x_label.getText());
      if(y_label.getText().length() != 0)	
 //        w.SetLabelY(y_label.getText());

      
      for(i = 0, k = 0; i < win.num_signal; i++) {
	r = ws[i].color.getRed();
	g = ws[i].color.getGreen();
	b = ws[i].color.getBlue();

	if(r > g)
	   if(r > b)
	     cf = ((float)255 - r)/ win.num_shot;
           else
	     cf = ((float)255 - b)/ win.num_shot;
        else
	   if(g > b)
             cf = ((float)255 - g)/ win.num_shot;
           else
	     cf = ((float)255 - b)/ win.num_shot;
	System.out.println(""+r+" "+g+" "+b+" "+cf);
 	  
        for(j = 0; j < win.num_shot; j++) {  
          markers[k] = ws[i].marker;	
          interpolates[k] = ws[i].interpolation;
          colors[k] = new Color(r + (j * cf), 
				g + (j * cf),
				b + (j * cf));
          colors[k] = ws[i].color;	
	  if(ws[i].update_flag || ws[i].shot_wave[j] == null) {
	    ws[i].update_flag = false;
            signals[k++] = (ws[i].shot_wave[j] = creaSignal(ws[i].y_expr, ws[i].x_expr, win.shots[j]));
	  } else
            signals[k++] = ws[i].shot_wave[j];
        }	
      }
 
      if(num_sig != 0) {

        signals[0].Autoscale(); 

        if(win.x_min.length() > 0)
	  signals[0].saved_xmin = (new Float(win.x_min)).floatValue();
        else 
	  signals[0].saved_xmin = signals[0].xmin; 

        if(win.x_max.length() > 0)
	  signals[0].saved_xmax = (new Float(win.x_max)).floatValue();
        else
          signals[0].saved_xmax = signals[0].xmax; 


        if(signals[0].saved_xmax < signals[0].saved_xmin) {
	  signals[0].saved_xmax = signals[0].xmax; 
          signals[0].saved_xmin = signals[0].xmin; 
        }

        if(win.y_min.length() > 0)
	  signals[0].saved_ymin = (new Float(win.y_min)).floatValue();
	else
          signals[0].saved_ymin = signals[0].ymin; 

        if(win.y_max.length() > 0)
	  signals[0].saved_ymax = (new Float(win.y_max)).floatValue();
        else
          signals[0].saved_ymax = signals[0].ymax; 

       if(signals[0].saved_ymax < signals[0].saved_ymin) {
	  signals[0].saved_ymax = signals[0].ymax; 
          signals[0].saved_ymin = signals[0].ymin; 
        }

        signals[0].ResetScales(); 

        w.SetColors(colors, num_sig); 
 //       w.Update(signals, num_sig, markers, interpolates);
      }
   }

   private Signal creaSignal(String y_expr, String x_expr, int shot) {

      Socket s = null;
      double x[] = new double[2000] , y[] = new double[2000], 
            err[] = new double[2000];
      float t_start, t_end, dt;
      byte[] b;
      int  num_samples = 0, len;
      int N_SAMPLES = 2000;
 
      try {
	s = new Socket("sun001",5001);
        DataInputStream read_s = new DataInputStream(s.getInputStream());
        DataOutputStream write_s = new DataOutputStream(s.getOutputStream());

        len = y_expr.length();
        write_s.writeInt(len);
        b = new byte[len];
        b = y_expr.getBytes();
        write_s.write(b, 0, len);
        write_s.writeInt(shot);

	num_samples = read_s.readInt();
	for(int i=0; i < num_samples; i++)
          y[i] = read_s.readFloat();
	t_start = read_s.readFloat();
	t_end   = read_s.readFloat();

        write_s.close();
	read_s.close();
        s.close();

	if(x_expr.length() == 0)
	{
          dt = (t_end-t_start)/num_samples;
	  for(int i=0; i < num_samples; i++)
            x[i] = t_start + i * dt;
 	} else {
	  s = new Socket("sun001",5001);
          read_s = new DataInputStream(s.getInputStream());
          write_s = new DataOutputStream(s.getOutputStream());

          len = x_expr.length();
          write_s.writeInt(len);
          b = new byte[len];
          b = x_expr.getBytes();
          write_s.write(b, 0, len);
          write_s.writeInt(shot);

	  num_samples = read_s.readInt();
	  for(int i=0; i < num_samples; i++)
            x[i] = read_s.readFloat();
	  t_start = read_s.readFloat();
	  t_end   = read_s.readFloat();

          write_s.close();
	  read_s.close();
          s.close();

        }

      } catch(IOException e){
        System.err.println("Cannot listen on port "+ "5001" + e);
      }

      return  new Signal(x, y, num_samples);
	
   }


   public void Show(int x, int y, MultiWaveform w, WindowSetupInfo ws)
   {
      setLocation(x - getSize().width/2, y - getSize().height/2);
      wave = w;
      w_setup = ws;
      putWindowSetup(ws); 		
      pack();
      show();
   }

   private boolean findShot(String shot)
   {
      int    num_shot = shot_list.getItemCount();	

      if(shot.length() == 0) return true;

      for(int i=0; i < num_shot; i++) {
         if(shot_list.getItem(i).equals(shot))
	   return true; 
      }	
      return false;	
   } 	

   private int findSignalSetup(SignalSetupInfo ws)
   {
      System.out.println("Signal " + w_setup.num_signal); 	
      for(int i=0; i < w_setup.num_signal; i++)
	if(w_setup.signals[i].equals(ws))
           return i;	 		
      return -1;
   }

   private void removeShot() {
      int i, j, k, idx;

      shot_list.remove(idx = shot_list.getSelectedIndex());
      for(i = 0; i < w_setup.num_signal; i++) {
        Signal[] new_s = new Signal[w_setup.num_shot - 1];
        for( j = 0, k = 0; j < w_setup.num_shot; j++)	
          if(j != idx)
	    new_s[k++] = w_setup.signals[i].shot_wave[j];
        w_setup.signals[i].shot_wave = new_s;
      } 
      w_setup.num_shot--;	
   }


   private void removeSignalSetup()
   {
      int idx = sig_list.getSelectedIndex();
      int i, j; 		
      SignalSetupInfo[] ws_new = new SignalSetupInfo[w_setup.num_signal - 1];

      if(idx != -1) {
	for (i = 0,  j = 0 ; i < w_setup.num_signal; i++)
	  if(i != idx)  
             ws_new[j++] = w_setup.signals[i];
	sig_list.remove(idx);
	w_setup.signals = ws_new;
	w_setup.num_signal--; 
      } 	
   }

   public void getWindowSetup(WindowSetupInfo ws) {


      ws.title = new String(title.getText());		
      ws.y_label = new String(y_label.getText());
      ws.x_label = new String(x_label.getText());
      ws.grid_mode = grid_mode.getSelectedIndex();
      ws.shots = new int[ws.num_shot = shot_list.getItemCount()];

      ws.x_min = x_min.getText();
      ws.x_max = x_max.getText();
      ws.y_min = y_min.getText();
      ws.y_max = y_max.getText();

      for(int i = 0; i < ws.num_shot; i++)
	ws.shots[i] = (new Integer(shot_list.getItem(i))).intValue();
   }

   public void putWindowSetup(WindowSetupInfo ws)
   {
        w_setup = ws;
        
	title.setText(ws.title);
	y_label.setText(ws.y_label);
	x_label.setText(ws.x_label);
        y_max.setText(ws.y_max);
        y_min.setText(ws.y_min);
        x_max.setText(ws.x_max);
        x_min.setText(ws.x_min);
        grid_mode.select(ws.grid_mode);  
	sig_list.removeAll();
	shot_list.removeAll();

        for(int i = 0; i < ws.num_signal; i++)
	   if(ws.signals[i].x_expr.length() == 0)  
             sig_list.add("Y : " + ws.signals[i].y_expr);
           else
             sig_list.add("Y : " + ws.signals[i].y_expr + " X : " + ws.signals[i].x_expr);

	for(int i = 0; i < ws.num_shot; i++)
           shot_list.add(""+ws.shots[i]);

	if(ws.num_signal != 0)
           putSignalSetup(ws.signals[ws.num_signal - 1]);
        else {
           x_expr.setText("");
           y_expr.setText("");
        } 
   }

   public void putSignalSetup(SignalSetupInfo ws)
   {
      x_expr.setText(ws.x_expr);
      y_expr.setText(ws.y_expr);
      error_w.e_up.setText(ws.error_up);
      error_w.e_down.setText(ws.error_down);
      show_type.select((ws.interpolation ? 0 : 1));
      marker.select(ws.marker);		 
      color.select(ws.color_idx);	
   }	

   public SignalSetupInfo getSignalSetup()
   {
      SignalSetupInfo ws = new SignalSetupInfo(); 

      ws.x_expr     = new String(x_expr.getText());
      ws.y_expr     = new String(y_expr.getText());
      ws.error_up   = new String(error_w.e_up.getText());
      ws.error_down = new String(error_w.e_down.getText()); 
      ws.interpolation = (show_type.getSelectedIndex() == 0 ? true : false);
      ws.marker     = marker.getSelectedIndex();	
      ws.color_idx  = color.getSelectedIndex();
      ws.color      = color_set[ws.color_idx];       
      ws.shot_wave  = new Signal[w_setup.num_shot];

      return ws; 	 
   }
  
   private SignalSetupInfo[] addSignalSetup(SignalSetupInfo ws)
   { 
      SignalSetupInfo[] ws_new = new SignalSetupInfo[w_setup.num_signal + 1];
     
      for(int i = 0; i < w_setup.num_signal; i++)
	ws_new[i] = w_setup.signals[i];
      ws_new[w_setup.num_signal++] = ws;
      
      w_setup.signals = ws_new;

      return w_setup.signals;
   }

   private void addShotWave()
   { 
	
      for(int i = 0; i < w_setup.num_signal; i++) {
        Signal[] sw_new = new Signal[w_setup.num_shot + 1];
        for(int j = 0; j < w_setup.num_shot; j++) 
	  sw_new[j] = w_setup.signals[i].shot_wave[j];
        w_setup.signals[i].shot_wave = sw_new;
      }
      w_setup.num_shot++; 	
   }

   private void addSignal()
   {
      int idx;
      SignalSetupInfo ws = getSignalSetup();
      idx = findSignalSetup(ws);
      if( idx == -1) {
	ws.update_flag = true;
        addSignalSetup(ws);
	if(ws.x_expr.length() == 0)  
          sig_list.add("Y : " + ws.y_expr);
        else
          sig_list.add("Y : " + ws.y_expr + " X : " + ws.x_expr);
 

      } else
	updateSignalSetup(idx, ws);
      error_w.resetError();
      sel_signal = w_setup.num_signal - 1;
   }



   private void updateSignalSetup(int idx, SignalSetupInfo ws)
   {
      
      w_setup.signals[idx].color  = ws.color;
      w_setup.signals[idx].error  = ws.error;
      w_setup.signals[idx].marker = ws.marker;
      w_setup.signals[idx].interpolation = ws.interpolation;
      if(ws.error_up != null)
	  w_setup.signals[idx].error_up = new String(ws.error_up);
      if(ws.error_down != null)
          w_setup.signals[idx].error_down = new String(ws.error_down);
   }
   	
 	
   public boolean action(Event e, Object o) {

      if(e.target == cancel)
	setVisible(false);

      if(e.target == apply || e.target == ok)
      {
	getWindowSetup(w_setup);
	applySignalsParam(w_setup, wave); 
	if(e.target == ok)
	  setVisible(false);
      }

      if(e.target == sig_list)
      {
        sel_signal = sig_list.getSelectedIndex();
        putSignalSetup(w_setup.signals[sel_signal]);
      }

      if(e.target == marker) {
	w_setup.signals[sel_signal].marker = marker.getSelectedIndex();
      }	

      if(e.target == show_type) {
	w_setup.signals[sel_signal].interpolation = show_type.getSelectedIndex() == 0 ? true : false;
      }	

      if(e.target == color) {
	w_setup.signals[sel_signal].color_idx = color.getSelectedIndex();
        w_setup.signals[sel_signal].color     = color_set[w_setup.signals[sel_signal].color_idx];       
      }

      if(e.target == error) {	
	 error_w.pack();
	 error_w.show();
      }
	
      return false;
   }

   public boolean keyDown(Event e, int key)
   {
    	      	
System.out.println("Key"+ key);	
      if(key == 127)
      {
	 if(e.target == sig_list)
           removeSignalSetup();	
	 if(e.target == shot_list)
	   removeShot();	
      }

      if(key == 10)
      { 
	if(e.target == y_expr || e.target == x_expr)
	   addSignal();	

	if(e.target == shot) {
	   if(!findShot(shot.getText())) {
	     shot_list.add(shot.getText());
	     shot_list.makeVisible(shot_list.getItemCount()-1);
	     shot.setText("");
	     addShotWave();	
	   }
        }
      }
      if(e.target instanceof TextField) {
         Object ob = e.target;
	 if(ob == x_max || ob == y_max || ob == x_min || ob == y_min || ob == shot)
	 {
	     Character ch = new Character((char)key);
             if(!ch.isDigit((char)key) && key != 127 && key != '.' && key != '+' && key != '-')
               return true;
         }
      } 

      return false;	
   }
}
