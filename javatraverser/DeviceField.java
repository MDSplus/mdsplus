import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;

public class DeviceField extends DeviceComponent
{
    public boolean textOnly = false;
    public boolean showState = false;
    public String labelString = "";
    public int numCols = 10;
    private boolean initial_state;
    protected boolean initializing = false;
    public void setNumCols(int numCols) 
    {
        this.numCols = numCols;
        textF.setColumns(numCols);
        redisplay();
    }
    public int getNumCols() {return numCols; }
    public void setLabelString(String labelString) 
    {
        this.labelString = labelString; 
        label.setText(labelString);
        redisplay();
    }
    public String getLabelString() {return labelString; }
    public void setShowState(boolean showState) 
    {
        this.showState = showState; 
        if(showState)
            checkB.setVisible(true);
        else
            checkB.setVisible(false); 
        redisplay();
    }
    public boolean getShowState() {return showState; }
    public void setTextOnly(boolean textOnly) {this.textOnly = textOnly; }
    public boolean getTextOnly() {return textOnly; }
        
        
    protected JCheckBox  checkB;
    protected JLabel label;
    protected JTextField textF;
     
    public DeviceField()
    {
        initializing = true;
        add(checkB = new JCheckBox());
        checkB.setVisible(false);
        add(label = new JLabel());
        add(textF = new JTextField(10));
        setSize(30, 10);
        initializing = false;
    }
        
    protected void initializeData(Data data, boolean is_on)
    {
        initializing = true;
        initial_state = is_on;
        if(showState)
        {
            add(checkB = new JCheckBox());
            checkB.setSelected(is_on);
            checkB.addChangeListener(new ChangeListener()
            {
                public void stateChanged(ChangeEvent e)
                {
                    boolean state = checkB.isSelected();
                    if(label != null)
                        label.setEnabled(state);
                }
            });
        }
        
        displayData(data, is_on);
        initializing = false;
    }       
    
    protected void displayData(Data data, boolean is_on)
    {
        if(showState)
            checkB.setSelected(is_on);
        if(data != null)
        {
            String textString = data.toString();
            if(textString != null)
            {
                if(textOnly && textString.charAt(0) == '"')
                    textF.setText(textString.substring(1, textString.length() - 1));
                else
                    textF.setText(textString);
            }
        }
        else
            textF.setText("");
        label.setEnabled(is_on);
        //textF.setEnabled(is_on);
    }
    
    
    protected Data getData()
    {
        String dataString = textF.getText();
        if(dataString == null) return null;
        if(textOnly)
            return Data.fromExpr("\""+dataString+"\"");
        else
            return Data.fromExpr(dataString);
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
        //if(checkB != null) checkB.setEnabled(state);
        //if(textF != null) textF.setEnabled(state);
        if(label != null) label.setEnabled(state);
    }

    public Component add(Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(String name, Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(Component c, int intex)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Field. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }



}
    
