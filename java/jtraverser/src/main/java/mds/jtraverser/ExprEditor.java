package mds.jtraverser;

//package jTraverser;
import javax.swing.*;

import java.awt.*;

import java.awt.event.*;

public class ExprEditor extends JPanel implements ActionListener, Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	String expr;
	MDSplus.Data data;
	int rows, columns;
	JButton left = null, right = null;
	JTextArea text_area;
	JTextField text_field;
	JPanel pl, pr;
	boolean default_scroll;
	boolean default_to_string;
	boolean quotes_added;
	boolean editable = true;

	public ExprEditor(boolean default_to_string)
	{
		this(null, default_to_string, 1, 20);
	}

	public ExprEditor(MDSplus.Data data, boolean default_to_string)
	{
		this(data, default_to_string, 1, 20);
	}

	public ExprEditor(MDSplus.Data data, boolean default_to_string, int rows, int columns)
	{
		boolean quotes_needed;
		JScrollPane scroll_pane;
		this.rows = rows;
		this.columns = columns;
		this.default_to_string = default_to_string;
		if (rows > 1)
			default_scroll = true;
		if (data != null)
			expr = data.toString();
		else
			expr = null;
		quotes_needed = (default_to_string && (expr == null || expr.charAt(0) == '\"'));
		if (quotes_needed)
		{
			quotes_added = true;
			left = new JButton("\"");
			right = new JButton("\"");
			left.setMargin(new Insets(0, 0, 0, 0));
			right.setMargin(new Insets(0, 0, 0, 0));
			left.addActionListener(this);
			right.addActionListener(this);
			if (expr != null)
				expr = expr.substring(1, expr.length() - 1);
		}
		else
			quotes_added = false;
		setLayout(new BorderLayout());
		if (default_scroll)
		{
			text_area = new JTextArea(rows, columns);
			final Dimension d = text_area.getPreferredSize();
			text_area.setText(expr);
			d.height += 20;
			d.width += 20;
			scroll_pane = new JScrollPane(text_area);
			scroll_pane.setPreferredSize(d);
			if (quotes_needed)
			{
				pl = new JPanel();
				pl.setLayout(new BorderLayout());
				pl.add(left, "North");
				add(pl, "East");
			}
			add(scroll_pane, "Center");
			if (quotes_needed)
			{
				pr = new JPanel();
				pr.setLayout(new BorderLayout());
				pr.add(right, "North");
				add(pr, "West");
			}
		}
		else
		{
			if (quotes_needed)
				add(left, BorderLayout.LINE_START);
			text_field = new JTextField(columns);
			text_field.setText(expr);
			add(text_field);
			if (quotes_needed)
				add(right, BorderLayout.LINE_END);
		}
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		if (!editable)
			return;
		quotes_added = false;
		if (default_scroll)
		{
			remove(pl);
			remove(pr);
		}
		else
		{
			remove(left);
			remove(right);
		}
		left = right = null;
		if (default_scroll)
			expr = text_area.getText();
		else
			expr = text_field.getText();
		expr = "\"" + expr + "\"";
		if (default_scroll)
			text_area.setText(expr);
		else
			text_field.setText(expr);
		validate();
		repaint();
	}

	@Override
	public MDSplus.Data getData()
	{
		if (default_scroll)
			expr = text_area.getText();
		else
			expr = text_field.getText();
		if (expr == null || expr.trim().length() == 0)
			return null;
		if (quotes_added)
			return Tree.curr_experiment.tdiCompile("\"" + expr + "\"");
		else
			return Tree.curr_experiment.tdiCompile(expr);
	}

	@Override
	public void reset()
	{
		if (data == null)
			expr = "";
		else
			expr = data.toString();
		if (default_to_string)
		{
			final int len = expr.length();
			if (len >= 2)
				expr = expr.substring(1, len - 1);
		}
		if (default_scroll)
			text_area.setText(expr);
		else
			text_field.setText(expr);
	}

	public void setData(MDSplus.Data data)
	{
		this.data = data;
		reset();
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
