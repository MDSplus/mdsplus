import java.awt.*;

import javax.swing.*;

public class DeviceLabel extends DeviceComponent
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	String data;
	public boolean textOnly = false;
	public boolean displayEvaluated = false;
	public String labelString = "<empty>";
	public int numCols = 10;
	protected boolean initializing = false;
	protected int preferredWidth = -1;
	JPanel jp;
	protected String initialField = "<empty>";
	private final boolean reportingChange = false;
	protected JLabel label;

	public DeviceLabel()
	{
		initializing = true;
		add(label = new JLabel("<empty>"));
		setLayout(new FlowLayout(FlowLayout.LEFT, 2, 0));
		initializing = false;
	}

	@Override
	public Component add(Component c)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Label. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public Component add(Component c, int intex)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Label. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public Component add(String name, Component c)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Label. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	@Override
	public void apply()
	{
		return;
	}

	@Override
	public void apply(int currBaseNid)
	{
		return;
	}

	protected void dataChanged(int offsetNid, String data)
	{
		if (reportingChange || this.offsetNid != offsetNid)
			return;
		label.setText(data);
	}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		this.data = data;
		/*
		 * if (showState) checkB.setSelected(is_on);
		 */
		if (data != null)
		{
			String textString;
			if (displayEvaluated)
			{
				try
				{
					initialField = textString = subtree.execute(data);
				}
				catch (final Exception exc)
				{
					textString = data;
				}
			}
			else
				textString = data;
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

	@Override
	protected String getData()
	{
		final String dataString = label.getText();
		if (dataString == null)
			return null;
		if (textOnly)
			return "\"" + dataString + "\"";
		else
			return dataString;
	}

	public boolean getDisplayEvaluated()
	{ return displayEvaluated; }

	public String getLabelString()
	{ return labelString; }

	public int getNumCols()
	{ return numCols; }

	public int getPreferredWidth()
	{ return preferredWidth; }

	@Override
	protected boolean getState()
	{ return true; }

	public boolean getTextOnly()
	{ return textOnly; }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		initializing = true;
		displayData(data, is_on);
		redisplay();
		initializing = false;
	}

	@Override
	protected boolean isDataChanged()
	{
		if (displayEvaluated && initialField != null)
			return !(label.getText().equals(initialField));
		else
			return true;
	}

	@Override
	public void setBounds(int x, int y, int width, int height)
	{
		super.setBounds(x, y, width, height);
		setPreferredSize(new Dimension(width, height));
	}

	public void setDisplayEvaluated(boolean displayEvaluated)
	{ this.displayEvaluated = displayEvaluated; }

	public void setLabelString(String labelString)
	{
		this.labelString = labelString;
		label.setText(labelString);
	}

	public void setNumCols(int numCols)
	{
		this.numCols = numCols;
		final FontMetrics fm = this.getFontMetrics(this.getFont());
		this.setSize(numCols * fm.charWidth('A'), fm.getHeight() + 4);
	}

	public void setPreferredWidth(int preferredWidth)
	{ this.preferredWidth = preferredWidth; }

	public void setTextOnly(boolean textOnly)
	{ this.textOnly = textOnly; }
}
