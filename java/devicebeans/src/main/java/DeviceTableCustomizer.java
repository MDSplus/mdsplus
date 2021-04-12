import java.beans.*;
import java.util.*;

import java.awt.*;
import java.awt.event.*;

public class DeviceTableCustomizer extends DeviceCustomizer implements Customizer
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceTable bean = null;
	Object obj;
	PropertyChangeSupport listeners = new PropertyChangeSupport(this);
	TextField labelString, identifier, numCols, numRows, columnNames, rowNames, preferredColumnWidthT, preferredHeightT;
	Choice nids, modeChoice;
	Button doneButton;
	Checkbox displayRowNumC, editableC, binaryC, useExpressionsC;

	public DeviceTableCustomizer()
	{}

	@Override
	public void addPropertyChangeListener(PropertyChangeListener l)
	{
		listeners.addPropertyChangeListener(l);
	}

	@Override
	public void removePropertyChangeListener(PropertyChangeListener l)
	{
		listeners.removePropertyChangeListener(l);
	}

	@Override
	public void setObject(Object o)
	{
		bean = (DeviceTable) o;
		setLayout(new BorderLayout());
		Panel jp = new Panel();
		jp.setLayout(new GridLayout(5, 1));
		Panel jp1 = new Panel();
		jp1.add(new Label("Label: "));
		jp1.add(labelString = new TextField(20));
		labelString.setText(bean.getLabelString());
		jp1.add(new Label("Num. Rows: "));
		jp1.add(numRows = new TextField(4));
		final int rows = bean.getNumRows();
		numRows.setText((new Integer(rows)).toString());
		jp1.add(new Label("Num. Columns: "));
		jp1.add(numCols = new TextField(4));
		final int cols = bean.getNumCols();
		numCols.setText((new Integer(cols)).toString());
		jp.add(jp1);
		jp1 = new Panel();
		jp1.add(new Label("Offset nid: "));
		jp1.add(nids = new Choice());
		final String names[] = getDeviceFields();
		if (names != null)
			for (int i = 0; i < names.length; i++)
				nids.addItem(names[i]);
		int offsetNid = bean.getOffsetNid();
		if (offsetNid > 0)
			offsetNid--;
		try
		{
			nids.select(offsetNid);
		}
		catch (final Exception exc)
		{}
		jp1.add(new Label("Opt. identifier: "));
		jp1.add(identifier = new TextField(bean.getIdentifier(), 15));
		jp1.add(displayRowNumC = new Checkbox("Display row num.", bean.getDisplayRowNumber()));
		jp.add(jp1);
		jp1 = new Panel();
		jp1.add(new Label("Pref. Column Width: "));
		jp1.add(preferredColumnWidthT = new TextField("" + bean.getPreferredColumnWidth(), 4));
		jp1.add(new Label("Pref. Height: "));
		jp1.add(preferredHeightT = new TextField("" + bean.getPreferredHeight(), 4));
		jp1.add(editableC = new Checkbox("Editable", bean.getEditable()));
		jp1.add(binaryC = new Checkbox("Binary", bean.getBinary()));
		jp1.add(useExpressionsC = new Checkbox("Use Expressions", bean.getUseExpressions()));
		jp1.add(new Label("Mode: "));
		jp1.add(modeChoice = new Choice());
		modeChoice.add("Normal");
		modeChoice.add("Reflex");
		modeChoice.add("Reflex Inverted");
		modeChoice.select(bean.getRefMode());
		jp.add(jp1);
		jp1 = new Panel();
		jp1.add(new Label("Column Names: "));
		columnNames = new TextField(30);
		final String[] colNamesArray = bean.getColumnNames();
		String cnames = "";
		if (colNamesArray != null)
		{
			for (int i = 0; i < colNamesArray.length; i++)
				cnames += colNamesArray[i] + " ";
		}
		columnNames.setText(cnames);
		jp1.add(columnNames);
		jp.add(jp1);
		jp1 = new Panel();
		jp1.add(new Label("Row Names: "));
		rowNames = new TextField(30);
		final String[] rowNamesArray = bean.getRowNames();
		cnames = "";
		if (rowNamesArray != null)
		{
			for (int i = 0; i < rowNamesArray.length; i++)
				cnames += rowNamesArray[i] + " ";
		}
		rowNames.setText(cnames);
		jp1.add(rowNames);
		jp.add(jp1);
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
				final int oldNumCols = bean.getNumCols();
				bean.setNumCols(Integer.parseInt(numCols.getText()));
				listeners.firePropertyChange("numCols", oldNumCols, bean.getNumCols());
				final int oldNumRows = bean.getNumRows();
				bean.setNumRows(Integer.parseInt(numRows.getText()));
				listeners.firePropertyChange("numRows", oldNumRows, bean.getNumRows());
				final int oldOffsetNid = bean.getOffsetNid();
				bean.setOffsetNid(nids.getSelectedIndex() + 1);
				listeners.firePropertyChange("offsetNid", oldOffsetNid, bean.getOffsetNid());
				final String oldIdentifier = bean.getIdentifier();
				bean.setIdentifier(identifier.getText());
				listeners.firePropertyChange("identifier", oldIdentifier, bean.getIdentifier());
				final boolean oldDisplayRowNumber = bean.getDisplayRowNumber();
				bean.setDisplayRowNumber(displayRowNumC.getState());
				listeners.firePropertyChange("displayRowNumber", oldDisplayRowNumber, bean.getDisplayRowNumber());
				final int oldRefMode = bean.getRefMode();
				bean.setRefMode(modeChoice.getSelectedIndex());
				listeners.firePropertyChange("refMode", oldRefMode, bean.getRefMode());
				final boolean oldEditable = bean.getEditable();
				bean.setEditable(editableC.getState());
				listeners.firePropertyChange("editable", oldEditable, bean.getEditable());
				final boolean oldBinary = bean.getBinary();
				bean.setBinary(binaryC.getState());
				listeners.firePropertyChange("binary", oldBinary, bean.getBinary());
				final boolean oldUseExpressions = bean.getUseExpressions();
				bean.setUseExpressions(useExpressionsC.getState());
				listeners.firePropertyChange("useExpression", oldUseExpressions, bean.getUseExpressions());
				final int oldPreferredColumnWidth = bean.getPreferredColumnWidth();
				bean.setPreferredColumnWidth(Integer.parseInt(preferredColumnWidthT.getText()));
				listeners.firePropertyChange("preferredColumnWidth", oldPreferredColumnWidth,
						bean.getPreferredColumnWidth());
				final int oldPreferredHeight = bean.getPreferredHeight();
				bean.setPreferredHeight(Integer.parseInt(preferredHeightT.getText()));
				listeners.firePropertyChange("preferredHeight", oldPreferredHeight, bean.getPreferredHeight());
				StringTokenizer st = new StringTokenizer(columnNames.getText(), " ");
				final String colNames[] = new String[st.countTokens()];
				int idx = 0;
				while (st.hasMoreTokens())
					colNames[idx++] = st.nextToken();
				final String[] oldColumnNames = bean.getColumnNames();
				bean.setColumnNames(colNames);
				listeners.firePropertyChange("columnNames", oldColumnNames, bean.getColumnNames());
				st = new StringTokenizer(rowNames.getText(), " ");
				final String rowNames[] = new String[st.countTokens()];
				idx = 0;
				while (st.hasMoreTokens())
					rowNames[idx++] = st.nextToken();
				final String[] oldRowNames = bean.getRowNames();
				bean.setRowNames(rowNames);
				listeners.firePropertyChange("rowNames", oldRowNames, bean.getRowNames());
			}
		});
		add(jp, "South");
	}
}
