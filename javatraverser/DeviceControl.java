import javax.swing.*;
import java.awt.event.*;
import java.awt.*;

public abstract class DeviceControl extends JButton
{
    DeviceSetup deviceSetup = null;
    protected String [] checkExpressions, checkMessages;

    DeviceControl()
    {
	addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e)
	    {
	        discoverDevice();
	        if(deviceSetup != null)
	            doOperation(deviceSetup);
	    }
	});
    }

    protected void discoverDevice()
    {
	Container curr_container;
	Component curr_component = this;
	do {
	    curr_container = curr_component.getParent();
	    curr_component = curr_container;
	}while ((curr_container != null) && !(curr_container instanceof DeviceSetup));
	if(curr_container != null)
	{
	    deviceSetup = (DeviceSetup) curr_container;
	 }
    }

    public void setCheckExpressions(String [] checkExpressions)
    {
	this.checkExpressions = checkExpressions;
    }
    public String [] getCheckExpressions() {return checkExpressions; }
    public void setCheckMessages(String [] checkMessages)
    {
	this.checkMessages = checkMessages;
    }
    public String [] getCheckMessages() {return checkMessages; }
    protected boolean check()
    {
	if(deviceSetup == null)
	    discoverDevice();
	if(deviceSetup != null && checkExpressions != null && checkMessages != null)
	    return deviceSetup.check(checkExpressions, checkMessages);
	return true;
    }

    void setReadOnly(boolean readOnly){}

    protected abstract void doOperation(DeviceSetup deviceSetup);
}

