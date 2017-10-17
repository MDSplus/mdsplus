import java.awt.Color;
import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;

public class DeviceChoice extends DeviceComponent
{
    protected boolean showState = false;
    protected String labelString = null;
    private boolean initial_state;
    protected String choiceItems[];
    protected int choiceIntValues[] = null;
    protected float choiceFloatValues[] = null;
    protected double choiceDoubleValues[] = null;
    protected boolean convert = false;
    private boolean reportingChange = false;

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
    public void setChoiceDoubleValues(double choiceDoubleValues[]) {this.choiceDoubleValues = choiceDoubleValues; }
    public double [] getChoiceDoubleValues() {return choiceDoubleValues; }

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
        initializing = true;
        displayData(data, is_on);
        comboB.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e)
            {
                if(initializing) return;
                reportingChange = true;
                reportDataChanged(new Integer(comboB.getSelectedIndex()));
                reportingChange = false;

                if(updateIdentifier == null || updateIdentifier.equals(""))
                  return;
                String currItem = (String)comboB.getSelectedItem();
                master.fireUpdate(updateIdentifier, new StringData(currItem));
            }
        });
        initializing = false;
    }

    protected void dataChanged(int offsetNid, Object data)
    {
      if(reportingChange || this.offsetNid != offsetNid)
        return;
      try {
          if(data instanceof Integer)
              comboB.setSelectedIndex( ( (Integer) data).intValue());
      }catch(Exception exc){System.err.println("DeviceChoice.dataChanged: " + exc);}
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
        double data_double;
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
            else if(choiceDoubleValues != null)
            {
                try {
                    data_double = data.getDouble();
                    for(curr_idx = 0; curr_idx < choiceDoubleValues.length && data_double != choiceDoubleValues[curr_idx]; curr_idx++);
                    if(curr_idx < choiceDoubleValues.length)
                    comboB.setSelectedIndex(curr_idx);
                }
                catch(Exception e){}
            }
        }
        setEnabled(is_on);
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
            if(choiceDoubleValues != null)
                return new DoubleData(choiceDoubleValues[curr_idx]);
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
        if(comboB != null) comboB.setEnabled(state);
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
    public void setHighlight(boolean highlighted)
    {
        if(highlighted)
        {
            if (label != null) label.setForeground(Color.red);
        }
        else
        {
           if (label != null) label.setForeground(Color.black);
       }
       super.setHighlight(highlighted);
    }


}

