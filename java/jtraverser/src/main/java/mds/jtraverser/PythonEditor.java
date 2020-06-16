package mds.jtraverser;

//package jTraverser;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class PythonEditor extends JPanel implements Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	static final int OPC_FUN = 162;
	String retVar;
	String program;
	int rows = 7, columns = 20;
	JTextArea text_area;
	JTextField text_field;
	boolean default_scroll;
	boolean editable = true;

	public PythonEditor(MDSplus.Data[] dataArgs)
	{
		JScrollPane scroll_pane;
		if (rows > 1)
			default_scroll = true;
		if (dataArgs != null)
		{
			getProgram(dataArgs);
		}
		else
		{
			program = "";
			retVar = "";
		}
		text_area = new JTextArea(rows, columns);
		text_area.setText(program);
		text_field = new JTextField(10);
		text_field.setText(retVar);
		final Dimension d = text_area.getPreferredSize();
		d.height += 20;
		d.width += 20;
		final JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		final JPanel jp1 = new JPanel();
		jp1.setLayout(new BorderLayout());
		jp1.setBorder(BorderFactory.createTitledBorder("Return Variable"));
		jp1.add(text_field);
		jp.add(jp1, "North");
		final JPanel jp2 = new JPanel();
		jp2.setLayout(new BorderLayout());
		jp2.setBorder(BorderFactory.createTitledBorder("Program"));
		scroll_pane = new JScrollPane(text_area);
		scroll_pane.setPreferredSize(d);
		jp2.add(scroll_pane);
		jp.add(jp2, "Center");
		setLayout(new BorderLayout());
		add(jp, "Center");
	}

	@Override
	public MDSplus.Data getData()
	{
		final String programTxt = text_area.getText();
		if (programTxt == null || programTxt.equals(""))
			return null;
		final StringTokenizer st = new StringTokenizer(programTxt, "\n");
		final String[] lines = new String[st.countTokens()];
		int idx = 0;
		int maxLen = 0;
		while (st.hasMoreTokens())
		{
			lines[idx] = st.nextToken();
			if (maxLen < lines[idx].length())
				maxLen = lines[idx].length();
			idx++;
		}
		for (int i = 0; i < lines.length; i++)
		{
			final int len = lines[i].length();
			for (int j = 0; j < maxLen - len; j++)
				lines[i] += " ";
		}
		final MDSplus.StringArray stArr = new MDSplus.StringArray(lines);
		final String retVarTxt = text_field.getText();
		MDSplus.Data retArgs[];
		if (retVarTxt == null || retVarTxt.equals(""))
		{
			retArgs = new MDSplus.Data[3];
			retArgs[0] = null;
			retArgs[1] = new MDSplus.String("Py");
			retArgs[2] = stArr;
		}
		else
		{
			retArgs = new MDSplus.Data[4];
			retArgs[0] = null;
			retArgs[1] = new MDSplus.String("Py");
			retArgs[2] = stArr;
			retArgs[3] = new MDSplus.String(retVarTxt);
		}
		final MDSplus.Data f = new MDSplus.Function(OPC_FUN, retArgs);
		f.setCtxTree(Tree.curr_experiment);
		return f;
	}

	void getProgram(MDSplus.Data[] dataArgs)
	{
		if (dataArgs.length <= 3)
			retVar = "";
		else
		{
			try
			{
				retVar = dataArgs[3].getString();
			}
			catch (final Exception exc)
			{
				retVar = "";
			}
		}
		String[] lines;
		try
		{
			if (dataArgs[2] instanceof MDSplus.String)
				lines = dataArgs[2].getStringArray();
			else
			{
				lines = new String[1];
				lines[0] = dataArgs[1].getString();
			}
			program = "";
			for (int i = 0; i < lines.length; i++)
			{
				program += lines[i] + "\n";
			}
		}
		catch (final Exception exc)
		{
			program = "";
		}
	}

	@Override
	public void reset()
	{
		text_area.setText(program);
		text_field = new JTextField(retVar);
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (text_area != null)
			text_area.setEditable(editable);
		if (text_field != null)
			text_field.setEditable(editable);
	}
}
