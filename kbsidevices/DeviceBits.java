import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.io.*;


public class DeviceBits extends DeviceComponent
{
    protected boolean initializing = false;
    private boolean showState = false;
    private String labelString = "";
    private String checkString[];
    private int numChecks = 8;
    /* Initial data for reset() */
    Data data;
    private boolean initial_state;
    /* Visual components */
    protected JCheckBox  checkB;
    protected JLabel label;
    protected JCheckBox bits[];

    public void setEditable(boolean editable) {this.editable = editable;}
    public boolean getEditable() { return editable; }
    public void setLabelString(String labelString)
    {
	this.labelString = labelString;
	label.setText(labelString);
	redisplay();
    }
    public String getLabelString() {return labelString; }

/*
    public DeviceBits(LayoutManager layout, boolean isDoubleBuffered) {
	   setLayout(layout);
	   setDoubleBuffered(isDoubleBuffered);
	   setOpaque(true);
//           updateUI();
       }

       /**
	* Create a new buffered JPanel with the specified layout manager
	*
	* @param layout  the LayoutManager to use
	*
       public DeviceBits(LayoutManager layout) {
	   this(layout, true);
       }


    public DeviceBits(boolean isDoubleBuffered) {
	this.setLayout(new VerticalFlowLayout(), isDoubleBuffered);
    }
//
 */
    public void setCheckString(String checkString[])
    {
	int i;
	this.checkString = checkString;
	for (i=0; i<numChecks; i++) {
	  bits[i].setHorizontalTextPosition(SwingConstants.CENTER);
	  bits[i].setVerticalTextPosition(SwingConstants.TOP);
	  bits[i].setText(checkString[i]);
	}
	redisplay();
    }
    public void setShowState(boolean showState)
    {
	this.showState = showState;
	checkB.setVisible(showState);
	redisplay();
    }
    public boolean getShowState() {return showState; }

    private void init()
    {
      int i;

      initializing = true;
      add(checkB = new JCheckBox());
      checkB.setVisible(false);
      add(label = new JLabel());
      bits = new JCheckBox[numChecks];
      for (i=0; i<numChecks; i++) {
	add(bits[i] = new JCheckBox());
	bits[i].setEnabled(editable);
      }
      initializing = false;
    }

    public DeviceBits()
    {
      init();
    }

    public DeviceBits(int num)
    {
      numChecks = num;
      init();
    }

    protected void initializeData(Data data, boolean is_on)
    {
      initializing = true;
	if(showState)
	{
	    checkB.setVisible(true);
	    checkB.setSelected(is_on);
	    checkB.addChangeListener(new ChangeListener()
	    {
	        public void stateChanged(ChangeEvent e)
	        {
	          int i;
	          boolean state = checkB.isSelected();
	          if(label != null)
	            label.setEnabled(state);
	          for (i=0; i<numChecks; i++) bits[i].setEnabled(state);
	        }
	    });
	}
	displayData(data, is_on);
	initializing = false;
    }

    protected void displayData(Data data, boolean is_on)
    {
	int i, n, val;
	this.data = data;
	initial_state = is_on;
	if(showState)
	    checkB.setSelected(is_on);
	if(data != null) {
	  try {
	    val = data.getInt();
	    n = 1;
	    for (i = 0; i < numChecks; i++) {
	      bits[i].setSelected((val & n)!=0);
	      n <<= 1;
	    }
	  }
	  catch(Exception e){}
	}
	else
	  for (i=0; i<numChecks; i++) bits[i].setSelected(false);
	label.setEnabled(is_on);
	for (i=0; i<numChecks; i++) bits[i].setEnabled(is_on & editable);
    }


    protected Data getData()
    {
      int i, n;
      n=0;
      for (i = 0; i < numChecks; i++)
	if (bits[i].isSelected()) n += 1 << i;
      return new IntData(n);
    }

    protected boolean getState()
    {
	if(!showState)
	    return initial_state;
	else
	    return checkB.isSelected();
    }

    public void setEnabled(boolean state)
    {
      int i;
      if(label != null) label.setEnabled(state);
      for (i = 0; i < numChecks; i++) bits[i].setEnabled(state);
    }

    public Component add(Component c)
    {
	if(!initializing)
	{
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Bits. Please remove the component.",
			"Error adding Device bits", JOptionPane.WARNING_MESSAGE);
	    return null;
	}
	return super.add(c);
    }

    public Component add(String name, Component c)
    {
	if(!initializing)
	{
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Bits. Please remove the component.",
			"Error adding Device bits", JOptionPane.WARNING_MESSAGE);
	    return null;
	}
	return super.add(c);
    }

    public Component add(Component c, int intex)
    {
	if(!initializing)
	{
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Bits. Please remove the component.",
			"Error adding Device bits", JOptionPane.WARNING_MESSAGE);
	    return null;
	}
	return super.add(c);
    }

    public boolean supportsState(){return showState;}


}

