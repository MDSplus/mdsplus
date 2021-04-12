package mds.jtraverser;

//package jTraverser;
import javax.swing.*;
import java.awt.*;

public class ArgEditor extends JScrollPane
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	int num_args;
	Dimension preferred;
	ExprEditor[] args;

	public ArgEditor()
	{
		this(null, 9, new Dimension(220, 89));
	}

	public ArgEditor(MDSplus.Data[] data)
	{
		this(data, 9, new Dimension(220, 89));
	}

	public ArgEditor(MDSplus.Data[] data, int num_args, Dimension preferred)
	{
		if (data == null)
			data = new MDSplus.Data[num_args];
		this.preferred = preferred;
		this.num_args = num_args;
		final JPanel jp = new JPanel();
		jp.setLayout(new GridLayout(num_args, 1));
		args = new ExprEditor[num_args];
		for (int i = 0; i < num_args; i++)
		{
			if (i < data.length)
				args[i] = new ExprEditor(data[i], false);
			else
				args[i] = new ExprEditor(null, false);
			jp.add(new LabeledExprEditor("Argument " + (i + 1), args[i]));
		}
		final JPanel jp2 = new JPanel();
		jp2.setLayout(new BorderLayout());
		jp2.add(jp, BorderLayout.NORTH);
		setViewportView(jp2);
		setPreferredSize(preferred);
		getVerticalScrollBar().setUnitIncrement(43);
	}

	public MDSplus.Data[] getData()
	{
		int actArgs = 0;
		for (int i = 0; i < num_args; i++)
		{
			if (args[i].getData() == null)
				break;
			actArgs++;
		}
		final MDSplus.Data data[] = new MDSplus.Data[actArgs];
		for (int i = 0; i < actArgs; i++)
		{
			data[i] = args[i].getData();
			data[i].setCtxTree(Tree.curr_experiment);
		}
		return data;
	}

	public void reset()
	{
		for (int i = 0; i < num_args; i++)
			args[i].reset();
	}

	public void setData(MDSplus.Data[] data)
	{
		int min_len, i;
		if (data == null)
			min_len = 0;
		else if (data.length < num_args)
			min_len = data.length;
		else
			min_len = num_args;
		for (i = 0; i < min_len; i++)
			args[i].setData(data[i]);
		for (; i < num_args; i++)
			args[i].setData(null);
	}

	public void setEditable(boolean editable)
	{
		for (int i = 0; i < num_args; i++)
			args[i].setEditable(editable);
	}
}
