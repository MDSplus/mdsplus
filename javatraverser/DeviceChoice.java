import javax.swing.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class DeviceChoice extends DeviceComponent
{
    protected boolean showState = false;
    protected String labelString = null;
    private boolean initial_state;
    protected String choiceItems[];
    protected int choiceIntValues[] = null;
    protected float choiceFloatValues[] = null;
    protected boolean convert = false;
    
    public void setConvert(boolean convert) {this.convert = convert; }
    public boolean getConvert() {return convert;}
    public void setChoiceItems(String choiceItems[]) 
    {
        this.choiceItems = choiceItems;
        if(comboB != null && comboB.getItemCount() > 0) 
            comboB.removeAllItems();
        if(choiceItems != null)
        {
            for(int i = 0; i < choiceItems.length; i++)
            {
                comboB.addItem(choiceItems[i]);
            }
        }
    }
    public String [] getChoiceItems() {return choiceItems; }
    public void setChoiceIntValues(int choiceIntValues[]) 
    {
        this.choiceIntValues = choiceIntValues; 
    }
    public int [] getChoiceIntValues() {return choiceIntValues; }
    public void setChoiceFloatValues(float choiceFloatValues[]) {this.choiceFloatValues = choiceFloatValues; }
    public float [] getChoiceFloatValues() {return choiceFloatValues; }
    
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
        redisplay();
    }
    public boolean getShowState() {return showState; }
        
    protected JCheckBox checkB;
    protected JComboBox comboB;
    protected JLabel label;
    protected boolean initializing = false;
    
    public DeviceChoice()
    {       
        initializing = true;
        add(checkB = new JCheckBox());
        checkB.setVisible(false);
        add(label = new JLabel("Choice: "));
        add(comboB = new JComboBox(new String[]{"Item"}));
        initializing = false;
    }
    
    protected void initializeData(Data data, boolean is_on)
    {
        initial_state = is_on;
        String data_string;
        int data_value;
        initializing = true;
        displayData(data, is_on);
        comboB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if(initializing || updateIdentifier == null || updateIdentifier.equals("")) return;
                String currItem = (String)comboB.getSelectedItem();
                master.fireUpdate(updateIdentifier, new StringData(currItem));
            }
        });
        initializing = false;
    }        
      
    public void postConfigure()
    {
        String currItem = (String)comboB.getSelectedItem();
        if(master != null && updateIdentifier != null)
            master.fireUpdate(updateIdentifier, new StringData(currItem));
    }
 
    protected void displayData(Data data, boolean is_on)
    {
        initial_state = is_on;
        String data_string;
        int curr_idx, data_value;
        float data_float;
        if(showState)
            checkB.setSelected(is_on);
         
        if(convert)
        {
            try {
                data_value = data.getInt();
            } catch(Exception e){data_value = 0;}
            if(choiceIntValues != null)
            {
                int i;
                for(i = 0; i < choiceIntValues.length && data_value != choiceIntValues[i]; i++);
                if(i < choiceIntValues.length)
                    comboB.setSelectedIndex(i);
            }
            else
                comboB.setSelectedIndex(data_value);
        }
        else
        {
            if (data instanceof StringData)
            {
                data_string = ((StringData)data).getString();
                for(curr_idx = 0; curr_idx < choiceItems.length && !choiceItems[curr_idx].equals(data_string); curr_idx++);
                if(curr_idx < choiceItems.length)
                    comboB.setSelectedIndex(curr_idx);
            }
            else if(choiceIntValues != null)
            {
                try {
                    data_value = data.getInt();
                    for(curr_idx = 0; curr_idx < choiceIntValues.length && data_value != choiceIntValues[curr_idx]; curr_idx++);
                    if(curr_idx < choiceIntValues.length)
                    comboB.setSelectedIndex(curr_idx);
                }
                catch(Exception e){}
            }
            else if(choiceFloatValues != null)
            {
                try {
                    data_float = data.getFloat();
                    for(curr_idx = 0; curr_idx < choiceFloatValues.length && data_float != choiceFloatValues[curr_idx]; curr_idx++);
                    if(curr_idx < choiceFloatValues.length)
                    comboB.setSelectedIndex(curr_idx);
                }
                catch(Exception e){}
            }
        }
    }   
            
        
    protected Data getData()
    {
        int curr_idx = comboB.getSelectedIndex();
        if(convert)
        {
            if(choiceIntValues != null)
                return new IntData(choiceIntValues[curr_idx]);
            return new IntData(curr_idx);    
        }      
        else
        {
            if(choiceIntValues != null)
                return new IntData(choiceIntValues[curr_idx]);
            if(choiceFloatValues != null)
                return new FloatData(choiceFloatValues[curr_idx]);
            return new StringData(choiceItems[curr_idx]);
        }
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
        //if(comboB != null) comboB.setEnabled(state);
        if(label != null) label.setEnabled(state);
    }
    public Component add(Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Choice. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(String name, Component c)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Choice. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }
        
    public Component add(Component c, int intex)
    {
        if(!initializing)
        {
		    JOptionPane.showMessageDialog(null, "You cannot add a component to a Device Choice. Please remove the component.", 
		        "Error adding Device field", JOptionPane.WARNING_MESSAGE);
            return null;
        }
        return super.add(c);
    }





}
    
