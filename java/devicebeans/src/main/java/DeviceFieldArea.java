import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.BorderLayout;

import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.JComponent;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import javax.swing.TransferHandler;
import java.awt.datatransfer.DataFlavor;
import javax.swing.JScrollPane;
import javax.swing.JOptionPane;

import java.util.*;

public class DeviceFieldArea extends DeviceComponent
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
	protected int preferredWidth = -1;
	protected String initialField;
	private boolean reportingChange = false;

	private final TransferHandler origTH;

	protected JTextArea textF;

	public DeviceFieldArea()
	{
		initializing = true;
                setLayout(new BorderLayout());
		add(new JScrollPane(textF = new JTextArea()), "Center");
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

	protected boolean getState()
	{
              return true;
	}
	@Override
	protected void displayData(String data, boolean is_on)
	{
		this.data = data;
		initial_state = is_on;
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
                                textString = textString.replace("\\n", "\n");
                                textString = textString.replace("\\t", "\t");
				if (textOnly && (textString.charAt(0) == '"' || textString.charAt(0) == '\''))
					textF.setText(textString.substring(1, textString.length() - 1));
				else
					textF.setText(textString);
			}
		}
		else
			textF.setText("");
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
                        {
                                java.lang.String outDataString = dataString.replace("\"", "\\\"");
                                System.out.println( "\"" + dataString + "\"");
				return "\"" + outDataString + "\"";
                        }
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
	public boolean getTextOnly()
	{ return textOnly; }

	@Override
	protected void initializeData(String data, boolean is_on)
	{
		initializing = true;
		initial_state = is_on;
		// initialField = Tree.dataToString(data);
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
		if (textF != null)
		{
			prevTextFont = textF.getFont();
			textF.setFont(new Font("Serif", Font.BOLD, 10));
		}
		super.print(g);
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

	public void setHighlight(boolean highlighted)
	{
		super.setHighlight(highlighted);
	}



	public void setPreferredWidth(int preferredWidth)
	{ this.preferredWidth = preferredWidth; }


	public void setTextOnly(boolean textOnly)
	{ this.textOnly = textOnly; }

	@Override
	public boolean supportsState()
	{
		return showState;
	}
}
