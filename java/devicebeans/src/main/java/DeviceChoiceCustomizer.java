import java.awt.BorderLayout;
import java.awt.Button;
import java.awt.Checkbox;
import java.awt.Choice;
import java.awt.Label;
import java.awt.Panel;
import java.awt.TextArea;
import java.awt.TextField;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.beans.Customizer;
import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.StringTokenizer;

import javax.swing.JOptionPane;

public class DeviceChoiceCustomizer extends DeviceCustomizer implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceChoice bean = null;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField labelString, identifier, updateIdentifier;
	TextArea itemsArea, codesArea;
	Label itemsLabel, codesLabel;
	Choice nids, mode;
	Button doneButton;
	Checkbox showState;

	public DeviceChoiceCustomizer()
	{}

	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{
		listeners.addPropertyChangeListener(l);
	}

	protected float[] convertFloat(String inText)
	{
		final String[] items = convertText(inText);
		final float out[] = new float[items.length];
		for (int i = 0; i < items.length; i++)
			out[i] = (new Float(items[i])).floatValue();
		return out;
	}

	protected int[] convertInt(String inText)
	{
		final String[] items = convertText(inText);
		final int out[] = new int[items.length];
		for (int i = 0; i < items.length; i++)
			out[i] = (new Integer(items[i])).intValue();
		return out;
	}

	protected String[] convertText(String inText)
	{
		int i = 0;
		final StringTokenizer st = new StringTokenizer(inText, "\n\r");
		final String[] items = new String[st.countTokens()];
		while (st.hasMoreTokens())
			items[i++] = st.nextToken();
		return items;
	}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{
		listeners.removePropertyChangeListener(l);
	}

	@Override
	public void setObject(Object o)
	{
		bean = (DeviceChoice) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new BorderLayout());
		Panel jp1 = new Panel();
		jp1.add(new Label("Label: "));
		jp1.add(labelString = new TextField(30));
		labelString.setText(bean.getLabelString());
		jp1.add(showState = new Checkbox("Show state: ", bean.getShowState()));
		jp1.add(new Label("Offset nid: "));
		jp1.add(nids = new Choice());
		final String names[] = getDeviceFields();
		if (names != null)
			for (int i = 0; i < names.length; i++)
				nids.add(names[i]);
		final int idx = bean.getOffsetNid();
		if (idx > 0 && idx <= names.length)
			nids.select(idx - 1);
		jp.add(jp1, "North");
		jp1 = new Panel();
		jp1.add(new Label("Mode: "));
		jp1.add(mode = new Choice());
		mode.add("String");
		mode.add("Integer");
		mode.add("Float");
		mode.add("Code");
		final boolean convert = bean.getConvert();
		final String[] items = bean.getChoiceItems();
		final int[] choiceIntValues = bean.getChoiceIntValues();
		final float[] choiceFloatValues = bean.getChoiceFloatValues();
		if (convert)
			mode.select(3);
		else if (choiceIntValues != null)
			mode.select(1);
		else if (choiceFloatValues != null)
			try
			{
				mode.select(2);
			}
			catch (final Exception exc)
			{
				JOptionPane.showMessageDialog(null, "" + exc, "" + mode, JOptionPane.WARNING_MESSAGE);
			}
		else
			mode.select(0);
		mode.addItemListener(new ItemListener()
		{
			@Override
			public void itemStateChanged(ItemEvent e)
			{
				final int curr_idx = mode.getSelectedIndex();
				if (curr_idx == 3)
				{
					codesLabel.setEnabled(true);
					codesArea.setEnabled(true);
				}
				else
				{
					codesLabel.setEnabled(false);
					codesArea.setEnabled(false);
				}
			}
		});
		jp1.add(itemsLabel = new Label("Items: "));
		jp1.add(itemsArea = new TextArea(5, 15));
		if (items != null)
			for (int i = 0; i < items.length; i++)
				itemsArea.append(items[i] + "\n");
		jp1.add(codesLabel = new Label("Codes: "));
		jp1.add(codesArea = new TextArea(5, 4));
		if (convert && choiceIntValues != null)
		{
			for (int i = 0; i < choiceIntValues.length; i++)
				codesArea.append((new Integer(choiceIntValues[i])).toString() + "\n");
		}
		else
		{
			codesLabel.setEnabled(false);
			codesArea.setEnabled(false);
		}
		jp.add(jp1, "Center");
		jp1 = new Panel();
		jp1.add(new Label("Opt. identifier: "));
		jp1.add(identifier = new TextField(bean.getIdentifier(), 20));
		jp1.add(new Label("Update identifier: "));
		jp1.add(updateIdentifier = new TextField(bean.getUpdateIdentifier(), 20));
		jp.add(jp1, "South");
		add(jp, "Center");
		jp = new Panel();
		jp.add(doneButton = new Button("Apply"));
		doneButton.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				final String oldLabelString = bean.getLabelString();
				bean.setLabelString(labelString.getText());
				listeners.firePropertyChange("labelString", oldLabelString, bean.getLabelString());
				final boolean oldShowState = bean.getShowState();
				bean.setShowState(showState.getState());
				listeners.firePropertyChange("showState", oldShowState, bean.getShowState());
				final int oldOffsetNid = bean.getOffsetNid();
				bean.setOffsetNid(nids.getSelectedIndex() + 1);
				listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
				final int curr_idx = mode.getSelectedIndex();
				final boolean oldConvert = bean.getConvert();
				final String[] oldChoiceItems = bean.getChoiceItems();
				final float[] oldChoiceFloatValues = bean.getChoiceFloatValues();
				final int[] oldChoiceIntValues = bean.getChoiceIntValues();
				switch (curr_idx)
				{
				case 0: // String
					bean.setConvert(false);
					bean.setChoiceItems(convertText(itemsArea.getText()));
					bean.setChoiceIntValues(null);
					bean.setChoiceFloatValues(null);
					break;
				case 1: // Integer
					bean.setConvert(false);
					bean.setChoiceItems(convertText(itemsArea.getText()));
					bean.setChoiceIntValues(convertInt(itemsArea.getText()));
					bean.setChoiceFloatValues(null);
					break;
				case 2: // Float
					bean.setConvert(false);
					bean.setChoiceItems(convertText(itemsArea.getText()));
					bean.setChoiceIntValues(null);
					bean.setChoiceFloatValues(convertFloat(itemsArea.getText()));
					break;
				case 3: // Code
					bean.setConvert(true);
					bean.setChoiceItems(convertText(itemsArea.getText()));
					bean.setChoiceFloatValues(null);
					bean.setChoiceIntValues(convertInt(codesArea.getText()));
					break;
				}
				listeners.firePropertyChange("convert", oldConvert, bean.getConvert());
				listeners.firePropertyChange("choiceItems", oldChoiceItems, bean.getChoiceItems());
				listeners.firePropertyChange("choiceFloatValues", oldChoiceFloatValues, bean.getChoiceFloatValues());
				listeners.firePropertyChange("choiceIntValues", oldChoiceIntValues, bean.getChoiceIntValues());
				final String oldIdentifier = bean.getIdentifier();
				bean.setIdentifier(identifier.getText());
				listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
				final String oldUpdateIdentifier = bean.getUpdateIdentifier();
				bean.setUpdateIdentifier(updateIdentifier.getText());
				listeners.firePropertyChange("updateIdentifier", oldUpdateIdentifier, bean.getUpdateIdentifier());
			}
		});
		add(jp, "South");
	}
}
