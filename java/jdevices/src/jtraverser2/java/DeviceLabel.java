import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.FontMetrics;

import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

import mds.MdsException;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.StringDsc;

public class DeviceLabel extends DeviceComponent
{
  Descriptor<?> data;
  public boolean textOnly = false;
  public boolean displayEvaluated = false;
  public String labelString = "<empty>";
  public int numCols = 10;
  protected boolean initializing = false;
  protected int preferredWidth = -1;
  JPanel jp;

  protected String initialField = "<empty>";

  private boolean reportingChange = false;


  public void setNumCols(int numCols)
  {
    this.numCols = numCols;
    FontMetrics fm = this.getFontMetrics(this.getFont());
    this.setSize(numCols * fm.charWidth('A'),  fm.getHeight() + 4);
    //redisplay();
  }

  public int getNumCols()
  {
    return numCols;
  }
/*
  public void setEditable(boolean editable)
  {
    this.editable = editable;
  }

  public boolean getEditable()
  {
    return editable;
  }
*/
  public boolean getDisplayEvaluated()
  {
    return displayEvaluated;
  }

  public void setDisplayEvaluated(boolean displayEvaluated)
  {
    this.displayEvaluated = displayEvaluated;
  }

  public void setPreferredWidth(int preferredWidth)
  {
    this.preferredWidth = preferredWidth;
  }

  public int getPreferredWidth()
  {
    return preferredWidth;
  }

  public void setLabelString(String labelString)
  {
    this.labelString = labelString;
    label.setText(labelString);
    //redisplay();
  }

  public String getLabelString()
  {
    return labelString;
  }

/*
  public void setShowState(boolean showState)
  {
    this.showState = showState;
    if (showState)
      checkB.setVisible(true);
    else
      checkB.setVisible(false);
      //redisplay();
  }
*/
/*
  public boolean getShowState()
  {
    return showState;
  }
*/
  public void setTextOnly(boolean textOnly)
  {
    this.textOnly = textOnly;
  }

  public boolean getTextOnly()
  {
    return textOnly;
  }

//  protected JCheckBox checkB;
  protected JLabel label;
//  protected JTextField textF;

  public DeviceLabel()
  {
    initializing = true;
//   jp = new JPanel();
//    jp.add(checkB = new JCheckBox());
//    checkB.setVisible(false);
    add(label = new JLabel("<empty>"));
//    add(jp);
//    add(textF = new JTextField(10));
//    textF.setEnabled(editable);
//    textF.setEditable(editable);
    //setLayout(gridbag = new GridBagLayout());
    setLayout(new FlowLayout(FlowLayout.LEFT, 2, 0));
   // this.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED));
    initializing = false;
  }

  protected void initializeData(Descriptor<?> data, boolean is_on)
  {
    initializing = true;
    displayData(data, is_on);
    redisplay();
    initializing = false;
  }

  protected void dataChanged(int offsetNid, Object data)
  {
    if (reportingChange || this.offsetNid != offsetNid)
      return;
    try
    {
	String textData = ((Descriptor<?>)data).decompile();
	label.setText(textData);
    }
    catch (Exception exc)
    {}
  }

   public void apply(int currBaseNid)
   {
       return;
   }

   public void apply()
   {
       return;
   }

  protected void displayData(Descriptor<?> data, boolean is_on)
  {
    this.data = data;
    if (data != null)
    {
      String textString = null;
      if (displayEvaluated)
      {
	try
	{
	  initialField = data.evaluate().decompile();
	}
	catch (Exception exc)
	{
	  textString =  data.decompile();
	}
      }
      else
	textString = data.decompile();
      if (textString != null)
      {

	if (textOnly && textString.charAt(0) == '"')
	  label.setText(textString.substring(1, textString.length() - 1));
	else
	  label.setText(textString);

       }
    }
    else
      label.setText("<empty>");
  }

  protected Descriptor<?> getData()
  {
	 String dataString = label.getText();
	 if (dataString == null)
	     return null;
   try
   {
   	if(dataString.trim().startsWith("[")) //If it begins with a [ it is assumed to be an array of strings
   	    return subtree.api.tdiCompile(subtree, dataString).getData();
   	else
   	    return new StringDsc(dataString);
   }
   catch (MdsException e)
   {
   	e.printStackTrace();
   	return null;
   }
  }

  protected boolean getState() {
      return true;
  }


  public Component add(Component c)
  {
    if (!initializing)
    {
      JOptionPane.showMessageDialog(null,
	  "You cannot add a component to a Device Label. Please remove the component.",
	                            "Error adding Device field",
	                            JOptionPane.WARNING_MESSAGE);
      return null;
    }
    return super.add(c);
  }

  public Component add(String name, Component c)
  {
    if (!initializing)
    {
      JOptionPane.showMessageDialog(null,
	  "You cannot add a component to a Device Label. Please remove the component.",
	                            "Error adding Device field",
	                            JOptionPane.WARNING_MESSAGE);
      return null;
    }
    return super.add(c);
  }

  public Component add(Component c, int intex)
  {
    if (!initializing)
    {
      JOptionPane.showMessageDialog(null,
	  "You cannot add a component to a Device Label. Please remove the component.",
	                            "Error adding Device field",
	                            JOptionPane.WARNING_MESSAGE);
      return null;
    }
    return super.add(c);
  }
/*
  public boolean supportsState()
  {
    return showState;
  }
*/
  public void setBounds(int x, int y, int width, int height)
  {
    super.setBounds(x, y, width, height);
    setPreferredSize(new Dimension(width, height));
  }

  protected boolean isDataChanged()
  {
    if (displayEvaluated && initialField != null)
      return! (label.getSize().equals(initialField));
    else
      return true;
  }

}
