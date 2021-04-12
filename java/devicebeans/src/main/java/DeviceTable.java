import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.util.StringTokenizer;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableCellRenderer;

public class DeviceTable extends DeviceComponent
{
	/**
		 *
		 */
	private static final long serialVersionUID = 1L;
	// int preferredWidth = 200;
//    int preferredHeight = 100;
	static public final int NORMAL = 0, REFLEX = 1, REFLEX_INVERT = 2;
	static String copiedColItems[], copiedRowItems[], copiedItems[];
	int refMode = NORMAL;
	int numRows = 3;
	int numCols = 3;
	int currRow = -1;
	int currCol = -1;
	String columnNames[] = new String[0];
	String rowNames[] = new String[0];
	boolean state = true;
	String labelString = "";
	boolean initializing = false;
	boolean editable = true;
	boolean displayRowNumber = false;
	boolean binary = false;
	boolean useExpressions = false;
	JPopupMenu popM = null;
	JMenuItem copyRowI, copyColI, copyI, pasteRowI, pasteColI, pasteI, propagateToRowI, propagateToColI;
	protected int preferredColumnWidth = 30;
	protected int preferredHeight = 70;
	protected JScrollPane scroll;
	protected JTable table;
	protected JLabel label;
	protected String items[] = new String[9];

	public DeviceTable()
	{
		initializing = true;
		if (rowNames.length > 0)
			displayRowNumber = true;
		table = new JTable();
		scroll = new JScrollPane(table);
		table.setPreferredScrollableViewportSize(new Dimension(200, 70));
		label = new JLabel();
		setLayout(new BorderLayout());
		final JPanel jp = new JPanel();
		jp.add(label);
		add(jp, "North");
		add(scroll, "Center");
		table.addMouseListener(new MouseAdapter()
		{
			@Override
			public void mousePressed(MouseEvent e)
			{
				if ((e.getModifiers() & Event.META_MASK) != 0)
				{ // If MB3
					showPopup(e.getX(), e.getY());
				}
			}
		});
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

	@Override
	public void apply() throws Exception
	{
		final CellEditor ce = table.getCellEditor();
		if (ce != null)
			ce.stopCellEditing();
		super.apply();
	}

	private boolean balancedParenthesis(String inStr)
	{
		int roundCount = 0;
		int squareCount = 0;
		int braceCount = 0;
		for (int i = 0; i < inStr.length(); i++)
		{
			switch (inStr.charAt(i))
			{
			case '(':
				roundCount++;
				break;
			case ')':
				roundCount--;
				break;
			case '[':
				squareCount++;
				break;
			case ']':
				squareCount--;
				break;
			case '{':
				braceCount++;
				break;
			case '}':
				braceCount--;
				break;
			}
		}
		return (roundCount == 0 && squareCount == 0 && braceCount == 0);
	}

	private boolean balancedSquareParenthesis(String inStr)
	{
		int squareCount = 0;
		for (int i = 0; i < inStr.length(); i++)
		{
			switch (inStr.charAt(i))
			{
			case '[':
				squareCount++;
				break;
			case ']':
				squareCount--;
				break;
			}
		}
		return (squareCount == 0);
	}

	void completeTable()
	{
		if (refMode != REFLEX && refMode != REFLEX_INVERT)
			return;
		// First row
		String sign = "";
		if (refMode == REFLEX_INVERT)
			sign = "- ";
		// First row
		for (int i = 1; i < numCols / 2; i++)
			items[numCols - i] = sign + items[i];
		// First column
		for (int i = 1; i < numRows / 2; i++)
		{
			items[(numRows - i) * numCols] = sign + items[i * numCols];
		}
		// Remaining
//        for(int i = 1; i < numRows/2; i++)
		for (int i = 1; i < numRows; i++)
			for (int j = 1; j < numCols / 2; j++)
			{
				items[(numRows - i) * numCols + numCols - j] = sign + items[i * numCols + j];
			}
		System.out.println("HERMITIAN:");
		for (int i = 0; i < numRows; i++)
		{
			for (int j = 0; j < numCols; j++)
				System.out.print(items[i * numCols + j] + " ");
			System.out.println("");
		}
	}

	void copy()
	{
		copiedItems = new String[items.length];
		for (int i = 0; i < items.length; i++)
			copiedItems[i] = items[i];
	}

	void copyCol(int col)
	{
		if (displayRowNumber)
			col--;
		copiedColItems = new String[numRows];
		for (int i = 0; i < numRows; i++)
			copiedColItems[i] = items[col + i * numCols];
	}

	void copyRow(int row)
	{
		if (row == -1)
			return;
		copiedRowItems = new String[numCols];
		for (int i = 0; i < numCols; i++)
			copiedRowItems[i] = items[row * numCols + i];
	}

	@Override
	public void displayData(String data, boolean is_on)
	{
		state = is_on;
		final String decompiled = data;
		final StringTokenizer st = new StringTokenizer(decompiled, ",[]");
		items = new String[numCols * numRows];
		int idx = 0;
		while (idx < numCols * numRows && st.hasMoreTokens())
			items[idx++] = st.nextToken();
		int actCols = numCols;
		if (refMode == REFLEX || refMode == REFLEX_INVERT)
			actCols = numCols / 2 + 1;
//        for(int i = 0; i < numCols; i++)
		for (int i = 0; i < actCols; i++)
		{
			table.getColumnModel().getColumn(i).setMinWidth(6);
			table.getColumnModel().getColumn(i).setPreferredWidth(6);
			table.getColumnModel().getColumn(i).setWidth(6);
		}
		completeTable();
		table.repaint();
		redisplay();
	}

	private String expandBackslash(String str)
	{
		String outStr = "";
		for (int i = 0; i < str.length(); i++)
		{
			final char currChar = str.charAt(i);
			outStr += currChar;
			if (currChar == '\\')
				outStr += currChar;
		}
		return outStr;
	}

	public boolean getBinary()
	{ return binary; }

	public String[] getColumnNames()
	{ return columnNames; }

	@Override
	public String getData()
	{
		if (refMode == REFLEX || refMode == REFLEX_INVERT)
			completeTable();
		final int n_data = items.length;
		String dataString = "[";
		for (int i = 0; i < n_data; i++)
		{
			if (i % numCols == 0)
				dataString += "[";
			if (items[i].trim().equals(""))
				dataString += "0";
			else
				dataString += items[i];
			if (i % numCols == numCols - 1)
			{
				dataString += "]";
				if (i < n_data - 1 && items[i + 1] != null)
					dataString += ",";
				else if (i == n_data - 1)
					dataString += "]";
			}
			else
				dataString += ",";
		}
		if (useExpressions)
			return "COMPILE(\'" + expandBackslash(dataString) + "\')";
		else
			return dataString;
	}

	public boolean getDisplayRowNumber()
	{ return displayRowNumber; }

	public boolean getEditable()
	{ return editable; }

	public String getLabelString()
	{ return labelString; }

	public int getNumCols()
	{ return numCols; }

	public int getNumRows()
	{ return numRows; }

	public int getPreferredColumnWidth()
	{ return preferredColumnWidth; }

	public int getPreferredHeight()
	{ return preferredHeight; }

	public int getRefMode()
	{ return refMode; }

	public String[] getRowNames()
	{ return rowNames; }

	@Override
	public boolean getState()
	{ return state; }

	public boolean getUseExpressions()
	{ return useExpressions; }

	@Override
	public void initializeData(String data, boolean is_on)
	{
		initializing = true;
		String decompiled = "";
		try
		{
			if (useExpressions)
			{
				decompiled = data;
				if (decompiled.startsWith("COMPILE(\'") || decompiled.startsWith("COMPILE(\""))
					decompiled = shrinkBackslash(decompiled.substring(9, decompiled.length() - 2));
			}
			else
				decompiled = subtree.execute(data);
		}
		catch (final Exception exc)
		{
			System.err.println(exc);
			decompiled = null;
		}
		items = new String[numCols * numRows];
		int idx = 0;
		for (int i = 0; i < items.length; i++)
			items[i] = "";
		/*
		 * if(decompiled != null) { StringTokenizer st = new StringTokenizer(decompiled,
		 * ",[]"); if (!decompiled.startsWith("[")) st.nextToken(); while (idx < numCols
		 * * numRows && st.hasMoreTokens()) { items[idx] += st.nextToken();
		 * if(balancedParenthesis(items[idx])) idx++; else items[idx]+=","; } }
		 */ if (decompiled != null)
		{
			// Remove surrounding [
			decompiled = decompiled.trim();
			while (decompiled.startsWith("[") && decompiled.endsWith("]") && balancedSquareParenthesis(decompiled))
			{
				decompiled = decompiled.substring(1, decompiled.length() - 1);
				decompiled = decompiled.trim();
			}
			final StringTokenizer st = new StringTokenizer(decompiled, ",");
			while (idx < numCols * numRows && st.hasMoreTokens())
			{
				String currToken = st.nextToken();
				currToken = currToken.trim();
				while (currToken.startsWith("[") && !balancedSquareParenthesis(currToken))
					currToken = currToken.substring(1);
				while (currToken.endsWith("]") && !balancedSquareParenthesis(currToken))
					currToken = currToken.substring(0, currToken.length() - 1);
				while (currToken.startsWith("[") && currToken.endsWith("]") && balancedSquareParenthesis(currToken))
				{
					currToken = currToken.substring(1, currToken.length() - 1);
					currToken = currToken.trim();
				}
				items[idx] += currToken;
				if (balancedParenthesis(items[idx]))
					idx++;
				else
					items[idx] += ",";
			}
		}
		label.setText(labelString);
		table.setModel(new AbstractTableModel()
		{
			private static final long serialVersionUID = 1L;

			@Override
			public Class<?> getColumnClass(int c)
			{
				if (!binary)
					return String.class;
				if ((rowNames != null && (rowNames.length > 0 || displayRowNumber)) && c == 0)
					return String.class;
				return Boolean.class;
			}

			@Override
			public int getColumnCount()
			{
				if (refMode == REFLEX || refMode == REFLEX_INVERT)
				{
					if (displayRowNumber || (rowNames != null && rowNames.length > 0))
					{
						return numCols / 2 + 1 + 1;
					}
					else
						return numCols / 2 + 1;
				}
				else
				{
					if (displayRowNumber || (rowNames != null && rowNames.length > 0))
					{
						return numCols + 1;
					}
					else
						return numCols;
				}
			}

			@Override
			public String getColumnName(int idx)
			{
				try
				{
					if (displayRowNumber || (rowNames != null && rowNames.length > 0))
					{
						if (idx == 0)
							return "";
						else
							return columnNames[idx - 1];
					}
					else
						return columnNames[idx];
				}
				catch (final Exception exc)
				{
					return "";
				}
			}

			@Override
			public int getRowCount()
			{ return numRows; }

			@Override
			public Object getValueAt(int row, int col)
			{
				if (rowNames != null && rowNames.length > 0)
				{
					if (col == 0)
					{
						try
						{
							return rowNames[row];
						}
						catch (final Exception exc)
						{
							return "";
						}
					}
					else
					{
						try
						{
							final String retItem = items[row * numCols + col - 1];
							if (!binary)
								return retItem;
							else if (retItem.trim().equals("0") || retItem.trim().equals("0."))
								return new Boolean(false);
							else
								return new Boolean(true);
						}
						catch (final Exception exc)
						{
							return null;
						}
					}
				}
				else if (displayRowNumber)
				{
					if (col == 0)
						return "" + (row + 1);
					else
					{
						try
						{
							final String retItem = items[row * numCols + col - 1];
							if (!binary)
								return retItem;
							else if (retItem.trim().equals("0") || retItem.trim().equals("0."))
								return new Boolean(false);
							else
								return new Boolean(true);
						}
						catch (final Exception exc)
						{
							return null;
						}
					}
				}
				else
				{
					try
					{
						final String retItem = items[row * numCols + col];
						if (!binary)
							return retItem;
						else if (retItem.trim().equals("0") || retItem.trim().equals("0."))
							return new Boolean(false);
						else
							return new Boolean(true);
					}
					catch (final Exception exc)
					{
						return null;
					}
				}
			}

			@Override
			public boolean isCellEditable(int row, int col)
			{
				return isEditable(row, col);
			}

			@Override
			public void setValueAt(Object value, int row, int col)
			{
				int itemIdx;
				int actCol;
				if ((rowNames != null && rowNames.length > 0) || displayRowNumber)
					actCol = col - 1;
				else
					actCol = col;
				/*
				 * if(refMode == REFLEX || refMode == REFLEX_INVERT) { //binary assumed to be
				 * false if(row == 0 || row == numRows/2) { if(actCol <= numCols/2) { itemIdx =
				 * row * numCols + actCol; items[itemIdx] = (String)value; if(actCol > 0) {
				 * itemIdx = row * numCols + numCols - actCol; items[itemIdx] =
				 * convertValue((String)value); table.repaint(); } } } else if(row <= numRows/2)
				 * { itemIdx = row * numCols + actCol; items[itemIdx] =(String)value; if(actCol
				 * == 0 && row > 0) { itemIdx = (numRows - row) * numCols + actCol;
				 * items[itemIdx] = convertValue((String)value); table.repaint(); } if(actCol >
				 * 0 && row > 0 && (actCol != numCols/2 || row != numRows/2 )) { itemIdx =
				 * (numRows - row) * numCols + numCols - actCol; items[itemIdx] =
				 * convertValue((String)value); table.repaint(); } }
				 *
				 * } else //refMode == NORMAL
				 */ {
					if ((rowNames != null && rowNames.length > 0) || displayRowNumber)
						itemIdx = row * numCols + actCol;
					else
						itemIdx = row * numCols + actCol;
					if (binary)
					{
						final boolean isOn = ((Boolean) value).booleanValue();
						items[itemIdx] = (isOn) ? "1" : "0";
						currRow = row;
						currCol = col;
					}
					else
						items[itemIdx] = (String) value;
					fireTableCellUpdated(row, col);
				}
				if (refMode == REFLEX || refMode == REFLEX_INVERT)
				{
					completeTable();
					table.repaint();
				}
			}
		});
		if (binary)
			table.setRowSelectionAllowed(false);
		if (refMode == REFLEX || refMode == REFLEX_INVERT)
			table.setPreferredScrollableViewportSize(
					new Dimension(preferredColumnWidth * (numCols / 2 + 1), preferredHeight));
		else
			table.setPreferredScrollableViewportSize(new Dimension(preferredColumnWidth * numCols, preferredHeight));
		table.revalidate();
		initializing = false;
	}

	private boolean isEditable(int row, int col)
	{
		if (displayRowNumber && col == 0)
			return false;
		else if (refMode == NORMAL)
			return editable;
		else if (!editable)
			return false;
		else // REFLEX || REFLEX_INVERT
		{
			/*
			 * int actCol = (displayRowNumber)?col-1:col; if((row == 0 || row == numRows/2)
			 * && actCol > numCols/2) return false; else if (row > numRows/2) return false;
			 * return true;
			 */ final int firstCol = (displayRowNumber) ? 1 : 0;
			if (col == firstCol && row > numRows - numRows / 2)
				return false;
			return true;
		}
	}

	void paste()
	{
		try
		{
			for (int i = 0; i < items.length; i++)
				items[i] = copiedItems[i];
		}
		catch (final Exception exc)
		{}
		completeTable();
		table.repaint();
	}

	void pasteCol(int col)
	{
		if (displayRowNumber)
			col--;
		try
		{
			for (int i = 0; i < numRows; i++)
				items[col + i * numCols] = copiedColItems[i];
		}
		catch (final Exception exc)
		{}
		completeTable();
		table.repaint();
	}

	void pasteRow(int row)
	{
		if (row == -1)
			return;
		try
		{
			for (int i = 0; i < numCols; i++)
				items[row * numCols + i] = copiedRowItems[i];
		}
		catch (final Exception exc)
		{}
		completeTable();
		table.repaint();
	}

	void propagateToCol(int row, int col)
	{
		if (row == -1 || col == -1)
		{
			row = currRow;
			col = currCol;
		}
		if (row == -1 || col == -1)
			return;
		if (displayRowNumber)
			col--;
		try
		{
			for (int i = 0; i < numRows; i++)
				items[col + i * numCols] = items[row * numCols + col];
		}
		catch (final Exception exc)
		{}
		completeTable();
		table.repaint();
	}

	void propagateToRow(int row, int col)
	{
		if (row == -1 || col == -1)
		{
			row = currRow;
			col = currCol;
		}
		if (row == -1 || col == -1)
			return;
		if (displayRowNumber)
			col--;
		try
		{
			for (int i = 0; i < numCols; i++)
				items[row * numCols + i] = items[row * numCols + col];
		}
		catch (final Exception exc)
		{}
		completeTable();
		table.repaint();
	}

	void readFromClipboard()
	{
		final Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
		final Transferable transferable = clipboard.getContents(null);
		if (transferable == null)
			return;
		if (transferable.isDataFlavorSupported(DataFlavor.stringFlavor))
		{
			try
			{
				final String tableText = (String) transferable.getTransferData(DataFlavor.stringFlavor);
				final StringTokenizer st = new StringTokenizer(tableText, ",\n");
				int idx = 0;
				while (st.hasMoreTokens() && items.length > idx)
					items[idx++] = st.nextToken();
			}
			catch (final Exception exc)
			{
				System.err.println("Error reading from clipboard: " + exc);
			}
			completeTable();
			table.repaint();
		}
	}

	public void setBinary(boolean binary)
	{ this.binary = binary; }

	public void setColumnNames(String[] columnNames)
	{
		this.columnNames = columnNames;
		redisplay();
	}

	public void setDisplayRowNumber(boolean displayRowNumber)
	{ this.displayRowNumber = displayRowNumber; }

	public void setEditable(boolean state)
	{ editable = state; }

	@Override
	public void setHighlight(boolean highlighted)
	{
		if (highlighted)
		{
			if (label != null)
				label.setForeground(Color.red);
		}
		else
		{
			if (label != null)
				label.setForeground(Color.black);
		}
		super.setHighlight(highlighted);
	}

	public void setLabelString(String labelString)
	{
		this.labelString = labelString;
		redisplay();
	}

	public void setNumCols(int numCols)
	{
		this.numCols = numCols;
		table.setPreferredScrollableViewportSize(new Dimension(preferredColumnWidth * numCols, preferredHeight));
		redisplay();
	}

	public void setNumRows(int numRows)
	{ this.numRows = numRows; }

	public void setPreferredColumnWidth(int preferredColumnWidth)
	{ this.preferredColumnWidth = preferredColumnWidth; }

	public void setPreferredHeight(int preferredHeight)
	{ this.preferredHeight = preferredHeight; }

	public void setRefMode(int refMode)
	{
		this.refMode = refMode;
		if (refMode == REFLEX || refMode == REFLEX_INVERT)
		{
			table.setDefaultRenderer(Object.class, new TableCellRenderer()
			{
				@Override
				public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected,
						boolean hasFocus, int row, int column)
				{
					final JTextField tf = new JTextField();
					tf.setText("" + value);
					tf.setEnabled(DeviceTable.this.isEditable(row, column));
					return tf;
				}
			});
		}
	}

	public void setRowNames(String[] rowNames)
	{
		this.rowNames = rowNames;
		if (rowNames != null && rowNames.length > 0)
			displayRowNumber = true;
		redisplay();
	}

	public void setUseExpressions(boolean useExpressions)
	{ this.useExpressions = useExpressions; }

	void showPopup(int x, int y)
	{
		if (popM == null)
		{
			popM = new JPopupMenu();
			copyRowI = new JMenuItem("Copy row");
			copyRowI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					copyRow(table.getSelectedRow());
				}
			});
			popM.add(copyRowI);
			copyColI = new JMenuItem("Copy column");
			copyColI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					copyCol(table.getSelectedColumn());
				}
			});
			popM.add(copyColI);
			copyI = new JMenuItem("Copy table");
			copyI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					copy();
				}
			});
			popM.add(copyI);
			pasteRowI = new JMenuItem("Paste row");
			pasteRowI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					pasteRow(table.getSelectedRow());
				}
			});
			popM.add(pasteRowI);
			pasteColI = new JMenuItem("Paste column");
			pasteColI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					pasteCol(table.getSelectedColumn());
				}
			});
			popM.add(pasteColI);
			pasteI = new JMenuItem("Paste table");
			pasteI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					paste();
				}
			});
			popM.add(pasteI);
			final JMenuItem copyClipboardI = new JMenuItem("Clipboard Copy");
			copyClipboardI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					writeToClipboard();
				}
			});
			popM.add(copyClipboardI);
			final JMenuItem pasteClipboardI = new JMenuItem("Clipboard Paste");
			pasteClipboardI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					readFromClipboard();
				}
			});
			popM.add(pasteClipboardI);
			final JMenuItem propagateToRowI = new JMenuItem("Propagate to Row");
			propagateToRowI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					propagateToRow(table.getSelectedRow(), table.getSelectedColumn());
				}
			});
			popM.add(propagateToRowI);
			final JMenuItem propagateToColI = new JMenuItem("Propagate to Column");
			propagateToColI.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					propagateToCol(table.getSelectedRow(), table.getSelectedColumn());
				}
			});
			popM.add(propagateToColI);
		}
		if (copiedRowItems == null)
			pasteRowI.setEnabled(false);
		else
			pasteRowI.setEnabled(true);
		if (copiedColItems == null)
			pasteColI.setEnabled(false);
		else
			pasteColI.setEnabled(true);
		if (copiedItems == null)
			pasteI.setEnabled(false);
		else
			pasteI.setEnabled(true);
		popM.show(table, x, y);
	}

	private String shrinkBackslash(String str)
	{
		String outStr = "";
		char prevChar = str.charAt(0);
		for (int i = 1; i < str.length(); i++)
		{
			final char currChar = str.charAt(i);
			if (!(currChar == '\\' && prevChar == '\\'))
				outStr += currChar;
			prevChar = currChar;
		}
		return outStr;
	}

	void writeToClipboard()
	{
		final Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
		String tableText = "";
		int idx = 0;
		if (refMode == REFLEX || refMode == REFLEX_INVERT)
			for (int i = 0; i < numRows; i++)
			{
				for (int j = 0; j < numCols; j++)
					tableText += " " + items[idx++];
				tableText += "\n";
			}
		clipboard.setContents(new StringSelection(tableText), null);
	}
}
