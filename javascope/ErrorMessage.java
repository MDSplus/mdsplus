import java.io.*;
import java.awt.*;
import java.awt.event.*;

/**
 ** Error message dialog
*/

class ErrorMessage extends ScopePositionDialog {
    private TextArea message;
    private Button acknowledge;
    private Button bt_1, bt_2, bt_3;
    private Label  lab;
    private int n_row = 0;
    private Frame f;
    public static final int MAX_ERROR_ROW  = 10;
    public static final int WARNING_TYPE = 1, ERROR_TYPE = 2;
    boolean is_message_show = false;
    private int msg_type;
    private int state = 0;
    static ErrorMessage errmsg;
    ErrorMessage(Frame fw)
    {
	super(fw, "Message", true);
	setResizable(true);
	
	msg_type = ERROR_TYPE;
	f = fw; 	
	setTitle("Error Message");
	setLayout(new GridLayout(10, 5, 0, 0));    
    }
    
    public static void ShowErrorMessage(Frame f, String msg)
    {
        if(errmsg == null)
            errmsg = new ErrorMessage(f, ERROR_TYPE);
        errmsg.setMessage(msg);
        errmsg.showMessage();
    }
    
    
    ErrorMessage(Frame fw, int _msg_type)
    {
	super(fw, "Error message", true);
	setResizable(true);	
	msg_type = _msg_type;
	f = fw; 	
		
	switch(msg_type) {
	    case WARNING_TYPE :
		setTitle("WARNING");
		Panel p0 = new Panel(new GridLayout(2, 0, 0, 0));
		Panel p1 = new Panel(new FlowLayout(FlowLayout.CENTER));
		Panel p2 = new Panel(new FlowLayout(FlowLayout.CENTER));
		p1.add(lab = new Label());
		p2.add(bt_1 = new Button());
		bt_1.addActionListener(this);
		p2.add(bt_2 = new Button());
		bt_2.addActionListener(this);
    		p2.add(bt_3 = new Button());
		bt_3.addActionListener(this);
		p0.add(p1);
		p0.add(p2);
		add(p0);
	    break;
	}
		
    }
    public void setLabels(String msg, String b1_lab, String b2_lab, String b3_lab)
    {    
	lab.setText(msg);
	bt_1.setLabel(b1_lab);
	bt_2.setLabel(b2_lab);
	bt_3.setLabel(b3_lab);    
    }
    
    
    /**
     ** Add a raw to the message dialog label
     */
    public void addMessage(String error)
    {
	int end;
	
	if(is_message_show) 
	    return;

	if(n_row < MAX_ERROR_ROW)
	{	
	    if((end = error.indexOf(10)) != -1)
		add(new Label(error.substring(0, end)));
	    else
		add(new Label(error.substring(0, error.length())));	
	    n_row++;
	}
    }
    
    public int getState()
    {
	return state;
    }
    
    /**
     ** Show error message dialog
     */
    public int showMessage()
    {
	switch(msg_type)
	{
	    case ERROR_TYPE :	
		if(n_row == 0 || is_message_show) 
		return 0;
		is_message_show = true;
		setLayout(new GridLayout(n_row + 1, 0));
		Panel p = new Panel(new FlowLayout(FlowLayout.CENTER, 0, 0));
		p.add(acknowledge = new Button("Acknowledge"));
		acknowledge.addActionListener(this);
		add(p);
	    break;
	    case WARNING_TYPE :	
	    break;	    
	}
	pack();
	setResizable(false);	    
	setPosition(f);
	show();
	return 1;
    }
    /**
     ** Set a measage string 
     */
    public void setMessage(String error)
    {
	int start = 0, end = 0;

	if(is_message_show || error == null) 
	    return;

    	removeAll();
	
	if(error.indexOf('\n', start) != -1)
	{	
	    while((end = error.indexOf('\n', start)) != -1)
	    {
		add(new Label(error.substring(start, end)));
		start = end + 1;
		n_row++;
	    }
	} else {
	    add(new Label(error));
	    n_row++;
	}    
	
 
    }
    
    public void resetMsg()
    {
	removeAll();
	n_row = 0;	   
    }
    
    public void actionPerformed(ActionEvent e)
    {
         Object ob = e.getSource();	

	f.validate();
	switch(msg_type)
	{
	    case ERROR_TYPE :	
		is_message_show = false;
		setResizable(true);
		setVisible(false);
		resetMsg();
	    break;
	    case WARNING_TYPE :	
		if(ob == bt_1)
		    state = 1; 
		if(ob == bt_2)
		    state = 2; 
		if(ob == bt_3)
		    state = 3; 
		setVisible(false);	    
	    break;	    
	}
    }    
}
