import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;

/**
 ** Dialog window to edit end show long expression
 ** defined in SetupDialog window
*/
class ExpandExpressionDialog extends ScopePositionDialog { 

 private Label lab;
 private TextArea x_expr, y_expr;
 private Button ok, cancel;
 private SetupData  conf_dialog;
 
 ExpandExpressionDialog(Frame _fw, SetupData conf_diag)
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


