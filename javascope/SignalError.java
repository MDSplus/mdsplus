import java.io.*;
import java.awt.*;
import java.awt.event.*;

/**
 ** Form di impostazione dei vettori di errore superiore e
 **inferiore associata al segnale
*/

class SignalError extends ScopePositionDialog {

   private TextField e_up, e_low;
   private Button ok, cancel;
   private WaveSetupData ws;

   SignalError(Frame fw) {
   
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
   
   
   public void setError(WaveSetupData ws_in)
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
