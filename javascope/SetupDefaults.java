import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
 

public class SetupDefaults extends ScopePositionDialog {
   TextField        title, shot, experiment;
   TextField        x_max, x_min, x_label;
   TextField        y_max, y_min, y_label;
   Button           ok, cancel, reset, erase, apply;
   Label            lab;
   int		    shots[];
   jScope	    main_scope;
   String xmin, xmax, ymax, ymin;
   String title_str, xlabel, ylabel;
   String experiment_str, shot_str;


   public SetupDefaults(Frame fw, String frame_title) 
   {

      super(fw, frame_title, true);
      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);

            
      main_scope = (jScope)fw; 	
  
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(4, 4, 4, 4);

      setLayout(gridbag);		
      c.insets = insets;	
      c.fill =  GridBagConstraints.BOTH;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      lab = new Label("Title");
      gridbag.setConstraints(lab, c);
      add(lab);

      c.gridwidth = GridBagConstraints.REMAINDER; 
      title = new TextField(40);
      gridbag.setConstraints(title, c);
      add(title);		 	
 	                       
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
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      y_max = new TextField(10);
      gridbag.setConstraints(y_max, c);
      add(y_max);	

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
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      x_max = new TextField(10);
      gridbag.setConstraints(x_max, c);
      add(x_max);	
           		
      c.gridwidth = 1;
      lab = new Label("Experiment");
      gridbag.setConstraints(lab, c);
      add(lab);	
      
      experiment = new TextField(25);		 	
      gridbag.setConstraints(experiment, c);
      add(experiment);	

      lab = new Label("Shot");
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      shot = new TextField(30);
      gridbag.setConstraints(shot, c);
      add(shot);
      	
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
   

   public void eraseForm()
   {
	title.setText("");
	x_label.setText("");
	x_max.setText("");
	x_min.setText("");
	y_max.setText("");
	y_min.setText("");
	y_label.setText("");
	experiment.setText("");
	shot.setText("");
   }
   
   private void setTextValue(TextField t, String val)
   {
	if(val != null)
	{
	    t.setText(val);
	}
   }

   private void initialize()      
   { 
   
	eraseForm();	    
	setTextValue(title, title_str);
	setTextValue(y_label, ylabel);
	setTextValue(x_label, xlabel);
        setTextValue(y_max, ymax);
        setTextValue(y_min, ymin);
        setTextValue(x_max, xmax);
        setTextValue(x_min, xmin);
	setTextValue(experiment, experiment_str);
	setTextValue(shot, shot_str);		
   }
   
   private void saveDefaultConfiguration()
   {

      experiment_str	= new String(experiment.getText());
      shot_str		= new String(shot.getText());
      xmax		= new String(x_max.getText());
      xmin		= new String(x_min.getText());
      ymax		= new String(y_max.getText());
      ymin		= new String(y_min.getText());
      title_str		= new String(title.getText());
      xlabel		= new String(x_label.getText());
      ylabel		= new String(y_label.getText());
    } 
    
   public String getDefaultValue(int i, boolean def_flag, WaveInterface wi)
   {
	String out = null;
   
	switch(i)
        {
	    case WaveInterface.B_title:
	      out = def_flag  ? title_str : wi.cin_title; break; 
	    case WaveInterface.B_shot:
	      out  = def_flag ? shot_str : wi.cin_shot;break; 
	    case WaveInterface.B_exp:
	      out =  def_flag ? experiment_str : wi.cexperiment;break; 
	    case WaveInterface.B_x_max:
	      out  = def_flag ? xmax : wi.cin_xmax; break; 
	    case WaveInterface.B_x_min:
	      out  = def_flag ? xmin : wi.cin_xmin; break; 
	    case WaveInterface.B_x_label:
	      out =  def_flag ? xlabel : wi.cin_xlabel;break; 
	    case WaveInterface.B_y_max:
	      out =  def_flag ? ymax : wi.cin_ymax; break; 
	    case WaveInterface.B_y_min:
	      out =  def_flag ? ymin : wi.cin_ymin; break; 
	    case WaveInterface.B_y_label:
	      out =  def_flag ? ylabel : wi.cin_ylabel;break; 
	}
	return out;
   } 
   
   
  
   public void  updateDefaultWI(WaveInterface wi)
   {
      boolean def_flag;
      int bit;
      
      if(wi == null) return;
      
      bit = WaveInterface.B_title;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_title      = getDefaultValue(bit, def_flag, wi);
      bit = WaveInterface.B_shot;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_shot       = getDefaultValue(bit ,  def_flag, wi); 
      bit =WaveInterface.B_exp;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.experiment = getDefaultValue(bit , def_flag , wi);
      bit = WaveInterface.B_x_max;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_xmax       = getDefaultValue(bit , def_flag , wi); 
      bit = WaveInterface.B_x_min;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_xmin       = getDefaultValue(bit , def_flag , wi);
      bit = WaveInterface.B_x_label;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_xlabel     = getDefaultValue(bit , def_flag , wi); 
      bit = WaveInterface.B_y_max;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ymax       = getDefaultValue(bit , def_flag , wi); 
      bit = WaveInterface.B_y_min;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ymin       = getDefaultValue(bit , def_flag, wi); 
      bit = WaveInterface.B_y_label;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ylabel     = getDefaultValue(bit , def_flag , wi);
   }   

   public void  Show()
   {
	initialize();
	show();
   }
   
   
   public void toFile(BufferedWriter out, String prompt)
   {
	jScope.writeLine(out, prompt + "experiment: " , experiment_str);
	jScope.writeLine(out, prompt + "shot: "       , shot_str);
	jScope.writeLine(out, prompt + "title: "      , title_str);
	jScope.writeLine(out, prompt + "xmax: "       , xmax);
	jScope.writeLine(out, prompt + "xmin: "       , xmin);
	jScope.writeLine(out, prompt + "x_label: "    , xlabel);
	jScope.writeLine(out, prompt + "ymax: "       , ymax);
	jScope.writeLine(out, prompt + "ymin: "       , ymin);
	jScope.writeLine(out, prompt + "y_label: "    , ylabel);
   }

   public int fromFile(String conf_file, String prompt)
   {
	String str;
	int error = 0;
   
        try {
	    BufferedReader in = new BufferedReader(new FileReader(conf_file));
     
	    while((str = in.readLine()) != null) {
	      
   		if(str.indexOf("Scope.global_1_1") == 0)
		{
		    int len = str.indexOf(":") + 2;

		    if(str.indexOf(".xmax:") != -1)
		    {
			xmax = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".xmin:") != -1)
		    {
			xmin = str.substring(len, str.length());
			continue;		
		    }
		    		    
		    if(str.indexOf(".x_label:") != -1)
		    {
			xlabel = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymax:") != -1)
		    {
			ymax = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".ymin:") != -1)
		    {
			ymin = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".y_label:") != -1)
		    {
			ylabel = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".experiment:") != -1)
		    {
			experiment_str = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".title:") != -1)
		    {
			title_str = str.substring(len, str.length());
			continue;		
		    }
		    if(str.indexOf(".shot:") != -1)
		    {
			shot_str = str.substring(len, str.length());
			if(shot_str.indexOf("_shots") != -1) 
			    shot_str  =  shot_str.substring(shot_str.indexOf("[")+1, shot_str.indexOf("]")); 
    			shots = main_scope.evaluateShot(shot_str);
    			continue;		
		    }		
		}
	    }
	}   catch(Exception e) {
	    error = 1;
	}
	return error;
   }
    

    
   public void actionPerformed(ActionEvent e)
   {
      Object ob = e.getSource();

      if(ob == erase)
	eraseForm();
		
      if(ob == cancel)
	setVisible(false);

      if(ob == apply || ob == ok)
      {
	saveDefaultConfiguration();
	if(ob == ok)
	    setVisible(false);
	shots = main_scope.evaluateShot(shot_str);    
	main_scope.UpdateAllWaves();
      }
      
      if(ob == reset)
      {
	initialize();
      }      	
   } 
}
