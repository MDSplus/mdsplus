import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JComponent;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import javax.swing.TransferHandler;
import java.awt.datatransfer.DataFlavor;
import java.util.*;

public class DeviceField extends DeviceComponent
{
	// Inner class ToTransferHandler to handle drag and drop
	class ToTransferHandler extends TransferHandler
	{
		/**
			 *
			 */
		private static final long serialVersionUID = 1L;

		@Override
		public boolean canImport(TransferHandler.TransferSupport support)
		{
			if (support.isDrop() && support.isDataFlavorSupported(DataFlavor.stringFlavor))
				return true;
			return false;
		}

		@Override
		public Transferable createTransferable(JComponent comp)
		{
			return new StringSelection(textF.getSelectedText());
		}

		@Override
		public int getSourceActions(JComponent comp)
		{
			return COPY_OR_MOVE;
		}

		@Override
		public boolean importData(TransferHandler.TransferSupport support)
		{
			if (!canImport(support))
				return origTH.importData(support);
			if (!editable)
				return false;
			try
			{
				final String data = (String) support.getTransferable().getTransferData(DataFlavor.stringFlavor);
				if (data.indexOf(":\\") != -1)
				{
					final StringTokenizer st = new StringTokenizer(data, ":");
					final String experiment = st.nextToken();
					final String path = data.substring(experiment.length() + 1);
					textF.setText(path);
				}
				else
					textF.setText(data);
			}
			catch (final Exception exc)
			{
				return false;
			}
			return true;
		}
	} // End Inner class ToTransferHandler
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	String data;
	public boolean textOnly = false;
	public boolean showState = false;
	public boolean displayEvaluated = false;
	public String labelString = "";
	public int numCols = 10;
	private boolean initial_state;
	protected boolean initializing = false;
	GridBagLayout gridbag;
	protected int preferredWidth = -1;
	JPanel jp;
	protected boolean isGridBag = false;
	protected String initialField;
	private boolean reportingChange = false;

	private final TransferHandler origTH;

	protected JCheckBox checkB;

	protected JLabel label;

	protected JTextField textF;

	public DeviceField()
	{
		initializing = true;
		jp = new JPanel();
		jp.add(checkB = new JCheckBox());
		checkB.setVisible(false);
		jp.add(label = new JLabel());
		add(jp);
		add(textF = new JTextField(10));
		textF.setEnabled(editable);
		textF.setEditable(editable);
		origTH = textF.getTransferHandler();
		textF.setTransferHandler(new ToTransferHandler());
		// setLayout(gridbag = new GridBagLayout());
		initializing = false;
	}

	@Override
	public Component add(Component c)
	{
		if (!initializing)
		{
			JOptionPane.showMessageDialog(null,
					"You cannot add a component to a Device Field. Please remove the component.",
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
					"You cannot add a component to a Device Field. Please remove the component.",
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
					"You cannot add a component to a Device Field. Please remove the component.",
					"Error adding Device field", JOptionPane.WARNING_MESSAGE);
			return null;
		}
		return super.add(c);
	}

	protected void dataChanged(int offsetNid, String data)
	{
		if (reportingChange || this.offsetNid != offsetNid)
			return;
		textF.setText(data);
	}

	@Override
	protected void displayData(String data, boolean is_on)
	{
		this.data = data;
		initial_state = is_on;
		if (showState)
			checkB.setSelected(is_on);
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
					initialField = textString = data;
				}
			}
			else
				initialField = textString = data;
			if (textString != null)
			{
				if (textOnly && textString.charAt(0) == '"')
					textF.setText(textString.substring(1, textString.length() - 1));
				else
					textF.setText(textString);
			}
		}
		else
			textF.setText("");
		label.setEnabled(is_on);
		textF.setEnabled(is_on & editable);
		textF.setEditable(is_on & editable);
	}

	@Override
	protected String getData()
	{
		final String dataString = textF.getText();
		if (dataString == null || dataString.length() == 0)
			return null;
		if (textOnly)
		{
			if (dataString.trim().startsWith("[")) // If it begins with a [ it is assumed to be an array of strings
				return dataString;
			else
				return "\"" + dataString + "\"";
		}
		else
			return dataString;
	}

	public boolean getDisplayEvaluated()
	{ return displayEvaluated; }

	public boolean getEditable()
	{ return editable; }

	public String getLabelString()
	{ return labelString; }

	public int getNumCols()
	{ return numCols; }

	public int getPreferredWidth()
	{ return preferredWidth; }
	public boolean getShowState()
	{ return showState; }
	@Override
	protected boolean getState()
	{
		if (!showState)
			return initial_state;
		else
			return checkB.isSelected();
	}

	public boolean getTextOnly()
	{ return textOnly; }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		initializing = true;
		initial_state = is_on;
		// initialField = Tree.dataToString(data);
		final Container parent = getParent();
		if (parent.getLayout() == null)
		{
			isGridBag = false;
		}
		else
			isGridBag = true;
		GridBagConstraints gc = null;
		if (isGridBag)
		{
			setLayout(gridbag = new GridBagLayout());
			gc = new GridBagConstraints();
			gc.anchor = GridBagConstraints.WEST;
			gc.gridx = gc.gridy = 0;
			// gc.gridwidth = gc.gridheight = 1;
			gc.gridwidth = 1;
			gc.gridheight = 1;
			gc.weightx = gc.weighty = 1.;
			gc.fill = GridBagConstraints.NONE;
			gridbag.setConstraints(jp, gc);
		}
		if (showState)
		{
			// add(checkB = new JCheckBox());
			checkB.setVisible(true);
			checkB.setSelected(is_on);
			checkB.addChangeListener(new ChangeListener()
			{
				@Override
				public void stateChanged(ChangeEvent e)
				{
					final boolean state = checkB.isSelected();
					if (label != null)
						label.setEnabled(state);
					if (textF != null && editable)
					{
						textF.setEnabled(state);
						textF.setEditable(state);
					}
				}
			});
		}
		if (textF != null && isGridBag)
		{
			gc.gridx++;
			gc.anchor = GridBagConstraints.EAST;
			gc.gridwidth = 1;
			gridbag.setConstraints(textF, gc);
		}
		displayData(data, is_on);
		setEnabled(is_on);
		/*
		 * textF.addKeyListener(new KeyAdapter() { public void keyTyped(KeyEvent e) {
		 * reportingChange = true; reportDataChanged(textF.getText()); reportingChange =
		 * false; } });
		 */ textF.getDocument().addDocumentListener(new DocumentListener()
		{
			@Override
			public void changedUpdate(DocumentEvent e)
			{
				reportingChange = true;
				reportDataChanged(textF.getText());
				reportingChange = false;
			}

			@Override
			public void insertUpdate(DocumentEvent e)
			{
				reportingChange = true;
				reportDataChanged(textF.getText());
				reportingChange = false;
			}

			@Override
			public void removeUpdate(DocumentEvent e)
			{
				reportingChange = true;
				reportDataChanged(textF.getText());
				reportingChange = false;
			}
		});
		textF.setEnabled(editable);
		textF.setEditable(editable);
		if (preferredWidth > 0)
		{
			setPreferredSize(new Dimension(preferredWidth, getPreferredSize().height));
			setSize(new Dimension(preferredWidth, getPreferredSize().height));
		}
		redisplay();
		initializing = false;
	}

	@Override
	protected boolean isChanged()
	{
		if (displayEvaluated)
			return false;
		return super.isChanged();
	}

	@Override
	protected boolean isDataChanged()
	{
		if (displayEvaluated && initialField != null)
			return !(textF.getText().equals(initialField));
		else
			return true;
	}

	@Override
	void postApply()
	{
		if (editable || !displayEvaluated || data == null)
			return;
		// Nothing to do if the field is not editable and displays evaulated data
		String textString;
		try
		{
			textString = subtree.execute(data);
		}
		catch (final Exception exc)
		{
			textString = data;
		}
		if (textString != null)
		{
			if (textOnly && textString.charAt(0) == '"')
				textF.setText(textString.substring(1, textString.length() - 1));
			else
				textF.setText(textString);
		}
	}

	@Override
	public void print(Graphics g)
	{
		Font prevLabelFont = null;
		Font prevTextFont = null;
		if (label != null)
		{
			prevLabelFont = label.getFont();
			label.setFont(new Font("Serif", Font.BOLD, 10));
		}
		if (textF != null)
		{
			prevTextFont = textF.getFont();
			textF.setFont(new Font("Serif", Font.BOLD, 10));
		}
		super.print(g);
		if (label != null)
			label.setFont(prevLabelFont);
		if (textF != null)
			textF.setFont(prevTextFont);
	}

	@Override
	public void setBounds(int x, int y, int width, int height)
	{
		super.setBounds(x, y, width, height);
		setPreferredSize(new Dimension(width, height));
	}

	public void setDisplayEvaluated(boolean displayEvaluated)
	{ this.displayEvaluated = displayEvaluated; }

	public void setEditable(boolean editable)
	{ this.editable = editable; }

	@Override
	public void setEnabled(boolean state)
	{
		if (!editable && state)
			return; // Do not set enabled if not editable
		// if(checkB != null) checkB.setEnabled(state);
		if (textF != null)
		{
			textF.setEnabled(state);
			textF.setEditable(state);
		}
		if (label != null)
			label.setEnabled(state);
		// if(checkB != null) checkB.setSelected(state);
		// initial_state = state;
	}

	@Override
	public void setHighlight(boolean highlighted)
	{
		if (highlighted)
		{
			if (label != null)
			{
				label.setEnabled(true);
				label.setForeground(Color.red);
			}
		}
		else
		{
			if (label != null)
			{
				label.setForeground(Color.black);
				label.setEnabled(getState());
			}
		}
		super.setHighlight(highlighted);
	}

	public void setLabelString(String labelString)
	{
		this.labelString = labelString;
		label.setText(labelString);
		// redisplay();
	}

	public void setNumCols(int numCols)
	{
		this.numCols = numCols;
		textF.setColumns(numCols);
		// redisplay();
	}

	public void setPreferredWidth(int preferredWidth)
	{ this.preferredWidth = preferredWidth; }

	public void setShowState(boolean showState)
	{
		this.showState = showState;
		if (showState)
			checkB.setVisible(true);
		else
			checkB.setVisible(false);
		// redisplay();
	}

	public void setTextOnly(boolean textOnly)
	{ this.textOnly = textOnly; }

	@Override
	public boolean supportsState()
	{
		return showState;
	}
}
