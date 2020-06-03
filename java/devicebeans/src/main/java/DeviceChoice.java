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
	private static final long serialVersionUID = 1L;
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
		if (comboB != null)
		{
			comboB.removeAllItems();
			if(choiceItems != null)
				for(String item : choiceItems)
					comboB.addItem(item);
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

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		initial_state = is_on;
		initializing = true;
		displayData(data, is_on);
		comboB.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				if(initializing) return;
				reportingChange = true;
				reportDataChanged(new Integer(comboB.getSelectedIndex()));
				reportingChange = false;

				if(updateIdentifier == null || updateIdentifier.equals(""))
					return;
				String currItem = (String)comboB.getSelectedItem();
				master.fireUpdate(updateIdentifier, currItem);
			}
		});
		initializing = false;
	}

	@Override
	protected void dataChanged(int offsetNid, Object data)
	{
		if(reportingChange || this.offsetNid != offsetNid)
			return;
		try {
			if(data instanceof Integer)
				comboB.setSelectedIndex( ( (Integer) data).intValue());
		}catch(Exception exc){System.err.println("DeviceChoice.dataChanged: " + exc);}
	}


	@Override
	public void postConfigure()
	{
		String currItem = (String)comboB.getSelectedItem();
		if(master != null && updateIdentifier != null)
			master.fireUpdate(updateIdentifier, currItem);
	}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		initial_state = is_on;
		if(showState)
			checkB.setSelected(is_on);

		if(convert)
		{
			int data_value;
			try {
				data_value = subtree.getInt(data);
			} catch(Exception e){
				data_value = 0;
			}
			if(choiceIntValues != null)
			{
				for(int i = 0; i < choiceIntValues.length; i++) {
					if(data_value == choiceIntValues[i]) {
						comboB.setSelectedIndex(i);
						break;
					}
				}
			}
			else
				comboB.setSelectedIndex(data_value);
		}
		else try
		{
			final String data_value = subtree.getString(data);
			for(int i = 0; i < choiceItems.length; i++) {
				if (choiceItems[i].equals(data_value)) {
					comboB.setSelectedIndex(i);
					break;
				}
			}
		} catch (Exception e0) {
			try
			{
				if(choiceIntValues != null)
				{
					final int data_value = subtree.getInt(data);
					for(int i = 0; i < choiceIntValues.length; i++) {
						if (data_value == choiceIntValues[i]) {
							comboB.setSelectedIndex(i);
							break;
						}
					}


				}
				else if(choiceFloatValues != null)
				{
					final float data_value = subtree.getFloat(data);
					for(int i = 0; i < choiceFloatValues.length; i++) {
						if (data_value != choiceFloatValues[i]) {
							comboB.setSelectedIndex(i);
							break;
						}
					}

				}
				else if(choiceDoubleValues != null)
				{
					final double data_value = subtree.getDouble(data);
					for(int i = 0; i < choiceDoubleValues.length; i++) {
						if (data_value != choiceDoubleValues[i]) {
							comboB.setSelectedIndex(i);
							break;
						}
					}

				}
			}
			catch(Exception e){}
		}
		setEnabled(is_on);
	}


	@Override
	protected String getData()
	{
		int curr_idx = comboB.getSelectedIndex();
		if(convert)
		{
			if(choiceIntValues != null)
				return ""+choiceIntValues[curr_idx];
			return ""+curr_idx;
		}
		else
		{
			if(choiceIntValues != null)
				return ""+choiceIntValues[curr_idx];
			if(choiceFloatValues != null)
				return "" + choiceFloatValues[curr_idx];
			if(choiceDoubleValues != null)
				return "" + choiceDoubleValues[curr_idx];
			return choiceItems[curr_idx];
		}
	}

	@Override
	protected boolean getState()
	{
		if(!showState)
			return initial_state;
		else
			return checkB.isSelected();
	}

	@Override
	public void setEnabled(boolean state)
	{
		//if(checkB != null) checkB.setEnabled(state);
		if(comboB != null) comboB.setEnabled(state);
		if(label != null) label.setEnabled(state);
	}
	@Override
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

	@Override
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

	@Override
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
	@Override
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

