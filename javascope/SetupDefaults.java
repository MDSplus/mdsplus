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
   javaScope	    main_scope;
   EvaluateWaveform sd_block;


   public SetupDefaults(Frame fw, String frame_title) 
   {

      super(fw, frame_title, true);
      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);

            
      main_scope = (javaScope)fw; 	
      sd_block = new EvaluateWaveform(main_scope);
  
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
   //   y_min.addKeyListener(this);
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
   
   public void setTextValue(TextField t, String val)
   {
	if(val != null)
	{
	    t.setText(val);
	}
   }

   public void putSetupDefault(WaveformConf wc)      
   { 

	if(wc == null) {
	    eraseForm();	    
	    return;
	}
      
	setTextValue(title, wc.title);
	setTextValue(y_label, wc.y_label);
	setTextValue(x_label, wc.x_label);
        setTextValue(y_max, wc.y_max);
        setTextValue(y_min, wc.y_min);
        setTextValue(x_max, wc.x_max);
        setTextValue(x_min, wc.x_min);
	setTextValue(experiment, wc.experiment);
	setTextValue(shot, wc.shot_str);		
   }
   
   public WaveformConf saveDefaultConfiguration()
   {
      WaveformConf wc = new WaveformConf();

      wc.experiment   = new String(experiment.getText());
      wc.shot_str     = new String(shot.getText());
      wc.x_max        = new String(x_max.getText());
      wc.x_min        = new String(x_min.getText());
      wc.y_max        = new String(y_max.getText());
      wc.y_min        = new String(y_min.getText());
      wc.title        = new String(title.getText());
      wc.x_label      = new String(x_label.getText());
      wc.y_label      = new String(y_label.getText());
      main_scope.sc.gwc = wc;
      return(wc);		     	     
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
	if(shot.getText() != null && shot.getText().length()!= 0)
	    sd_block.evaluateShot(shot.getText());
	saveDefaultConfiguration();
	main_scope.UpdateAllWaves();
	if(ob == ok)
	    setVisible(false);
      }
      
      if(ob == reset)
      {
	putSetupDefault(main_scope.sc.gwc);
      }      	
   } 
}
