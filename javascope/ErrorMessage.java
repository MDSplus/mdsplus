import java.io.*;
import java.awt.*;
import java.awt.event.*;

/**
 ** Error message dialog
*/

class ErrorMessage extends ScopePositionDialog {
    private TextArea message;
    private Button acknowledge;
    private int n_row = 0;
    private Frame f;
    public static final int MAX_ERROR_ROW  = 10;
    
    ErrorMessage(Frame fw)
    {
	
	super(fw, "Error Setup", true);
	setResizable(false);
	
	f = fw; 	
	setTitle("Error Message");
	setLayout(new GridLayout(20, 0, 0, 0));    
    }
    /**
     ** Add a raw to the message dialog label
     */
    public void addMessage(String error)
    {
	int end;

	if(n_row < MAX_ERROR_ROW)
	{	
	    if((end = error.indexOf(10)) != -1)
		add(new Label(error.substring(0, end)));
	    else
		add(new Label(error.substring(0, error.length())));	
	    n_row++;
	}
    }
    /**
     ** Show error message dialog
     */
    public int showMessage()
    {
	if(n_row == 0) 
	    return 0;
	setLayout(new GridLayout(n_row + 1, 0));
	add(acknowledge = new Button("Acknowledge"));
	acknowledge.addActionListener(this);
	pack();
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

    	removeAll();
	
	while((end = error.indexOf('\n', start)) != -1)
	{
	    add(new Label(error.substring(start, end)));
	    start = end + 1;
	    n_row++;
	}
    }
    
    public void actionPerformed(ActionEvent e)
    {
	setVisible(false);
	removeAll();
	n_row = 0;	   
    }    
}
