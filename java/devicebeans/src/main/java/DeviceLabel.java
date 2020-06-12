import java.awt.Component;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.FontMetrics;

import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class DeviceLabel extends DeviceComponent
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	String data;
	public boolean textOnly = false;
//public boolean showState = false;
	public boolean displayEvaluated = false;
	public String labelString = "<empty>";
	public int numCols = 10;
	protected boolean initializing = false;
//  GridBagLayout gridbag;
	protected int preferredWidth = -1;
	JPanel jp;
//protected boolean isGridBag = false;
	protected String initialField = "<empty>";
	private final boolean reportingChange = false;

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
		// setLayout(gridbag = new GridBagLayout());
		setLayout(new FlowLayout(FlowLayout.LEFT, 2, 0));
		// this.setBorder(BorderFactory.createBevelBorder(BevelBorder.LOWERED));
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
		/*
		 * label.setEnabled(is_on); textF.setEnabled(is_on & editable);
		 * textF.setEditable(is_on & editable);
		 */
	}
	/*
	 * protected Data getData() { String dataString = textF.getText(); if
	 * (dataString == null) return null; if (textOnly) return Tree.dataFromExpr("\""
	 * + dataString + "\""); else return Tree.dataFromExpr(dataString); }
	 *
	 *
	 * protected boolean getState() { if (!showState) return initial_state; else
	 * return checkB.isSelected(); }
	 *
	 * public void setEnabled(boolean state) { if (!editable && state) return; //Do
	 * not set enabled if not editable //if(checkB != null)
	 * checkB.setEnabled(state); if (textF != null) { textF.setEnabled(state);
	 * textF.setEditable(state); } if (label != null) label.setEnabled(state);
	 * //if(checkB != null) checkB.setSelected(state); //initial_state = state; }
	 */

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

/*
	 * public void setEditable(boolean editable) { this.editable = editable; }
	 *
	 * public boolean getEditable() { return editable; }
	 */
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
		// initialField = Tree.dataToString(data);
		/*
		 * Container parent = getParent();
		 *
		 * if (parent.getLayout() == null) { isGridBag = false; } else isGridBag = true;
		 *
		 * GridBagConstraints gc = null; if (isGridBag) { setLayout(gridbag = new
		 * GridBagLayout()); gc = new GridBagConstraints(); gc.anchor =
		 * GridBagConstraints.WEST; gc.gridx = gc.gridy = 0; gc.gridwidth =
		 * gc.gridheight = 1; gc.weightx = gc.weighty = 1.; gc.fill =
		 * GridBagConstraints.NONE; gridbag.setConstraints(jp, gc); }
		 */
		/*
		 * if (showState) { //add(checkB = new JCheckBox()); checkB.setVisible(true);
		 * checkB.setSelected(is_on); checkB.addChangeListener(new ChangeListener() {
		 * public void stateChanged(ChangeEvent e) { boolean state =
		 * checkB.isSelected(); if (label != null) label.setEnabled(state); if (textF !=
		 * null && editable) { textF.setEnabled(state); textF.setEditable(state); } }
		 * }); } if (textF != null && isGridBag) { gc.gridx++; gc.anchor =
		 * GridBagConstraints.EAST; gridbag.setConstraints(textF, gc); }
		 */
		displayData(data, is_on);
		/*
		 * setEnabled(is_on); textF.addKeyListener(new KeyAdapter() { public void
		 * keyTyped(KeyEvent e) { reportingChange = true;
		 * reportDataChanged(textF.getText()); reportingChange = false; } });
		 *
		 * textF.setEnabled(editable); textF.setEditable(editable); if (preferredWidth >
		 * 0) { setPreferredSize(new Dimension(preferredWidth,
		 * getPreferredSize().height)); setSize(new Dimension(preferredWidth,
		 * getPreferredSize().height)); }
		 */
		redisplay();
		initializing = false;
	}

	@Override
	protected boolean isDataChanged()
	{
		if (displayEvaluated && initialField != null)
			return !(label.getSize().equals(initialField));
		else
			return true;
	}

	/*
	 * public boolean supportsState() { return showState; }
	 */
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
		// redisplay();
	}

	public void setNumCols(int numCols)
	{
		this.numCols = numCols;
		final FontMetrics fm = this.getFontMetrics(this.getFont());
		this.setSize(numCols * fm.charWidth('A'), fm.getHeight() + 4);
		// redisplay();
	}

	public void setPreferredWidth(int preferredWidth)
	{ this.preferredWidth = preferredWidth; }

	/*
	 * public void setShowState(boolean showState) { this.showState = showState; if
	 * (showState) checkB.setVisible(true); else checkB.setVisible(false);
	 * //redisplay(); }
	 */
	/*
	 * public boolean getShowState() { return showState; }
	 */
	public void setTextOnly(boolean textOnly)
	{ this.textOnly = textOnly; }
}
