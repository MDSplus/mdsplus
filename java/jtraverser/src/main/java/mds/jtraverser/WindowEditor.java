package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class WindowEditor extends JPanel implements ActionListener, Editor
{
	static class WindowEdt extends JPanel
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;
		MDSplus.Window window;
		LabeledExprEditor startidx_edit, endidx_edit, value0_edit;

		public WindowEdt()
		{
			this(null);
		}

		public WindowEdt(MDSplus.Window window)
		{
			this.window = window;
			if (this.window == null)
			{
				this.window = new MDSplus.Window(null, null, null);
			}
			this.window.setCtxTree(Tree.curr_experiment);
			final GridLayout gl = new GridLayout(3, 1);
			gl.setVgap(0);
			setLayout(gl);
			startidx_edit = new LabeledExprEditor("Start Idx", new ExprEditor(this.window.getStartIdx(), false));
			add(startidx_edit);
			endidx_edit = new LabeledExprEditor("End Idx", new ExprEditor(this.window.getEndIdx(), false));
			add(endidx_edit);
			value0_edit = new LabeledExprEditor("Time of Zero", new ExprEditor(this.window.getValueAt0(), false));
			add(value0_edit);
		}

		public MDSplus.Data getData()
		{
			final MDSplus.Data w = new MDSplus.Window(startidx_edit.getData(), endidx_edit.getData(),
					value0_edit.getData());
			w.setCtxTree(Tree.curr_experiment);
			return w;
		}

		public void reset()
		{
			startidx_edit.reset();
			endidx_edit.reset();
			value0_edit.reset();
		}

		public void setEditable(boolean editable)
		{
			if (startidx_edit != null)
				startidx_edit.setEditable(editable);
			if (endidx_edit != null)
				endidx_edit.setEditable(editable);
			if (value0_edit != null)
				value0_edit.setEditable(editable);
		}
	}

	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	ExprEditor expr_edit;
	WindowEdt window_edit;
	JComboBox<String> combo;
	int mode_idx, curr_mode_idx;
	MDSplus.Data data;
	boolean editable = true;
	TreeDialog dialog;

	public WindowEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
			mode_idx = 0;
		else if (data instanceof MDSplus.Window)
			mode_idx = 1;
		else
			mode_idx = 2;
		curr_mode_idx = mode_idx;
		final String names[] =
		{ "Undefined", "Window", "Expression" };
		combo = new JComboBox<>(names);
		combo.setEditable(false);
		combo.setSelectedIndex(mode_idx);
		combo.addActionListener(this);
		setLayout(new BorderLayout());
		final JPanel jp = new JPanel();
		jp.add(combo);
		add(jp, "North");
		addEditor();
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		if (!editable)
		{
			combo.setSelectedIndex(curr_mode_idx);
			return;
		}
		final int idx = combo.getSelectedIndex();
		if (idx == curr_mode_idx)
			return;
		switch (curr_mode_idx)
		{
		case 1:
			remove(window_edit);
			break;
		case 2:
			remove(expr_edit);
			break;
		}
		curr_mode_idx = idx;
		addEditor();
		validate();
		dialog.repack();
	}

	private void addEditor()
	{
		switch (curr_mode_idx)
		{
		case 0:
			return;
		case 1:
			if (mode_idx == 1)
				window_edit = new WindowEdt((MDSplus.Window) data);
			else
				window_edit = new WindowEdt(null);
			add(window_edit, "Center");
			break;
		case 2:
			if (mode_idx == 2)
				expr_edit = new ExprEditor(data, false, 8, 30);
			else
				expr_edit = new ExprEditor(null, false, 8, 30);
			add(expr_edit, "Center");
			break;
		}
	}

	@Override
	public MDSplus.Data getData()
	{
		switch (curr_mode_idx)
		{
		case 0:
			return null;
		case 1:
			return window_edit.getData();
		case 2:
			return expr_edit.getData();
		}
		return null;
	}

	@Override
	public void reset()
	{
		switch (curr_mode_idx)
		{
		case 1:
			remove(window_edit);
			break;
		case 2:
			remove(expr_edit);
			break;
		}
		curr_mode_idx = mode_idx;
		addEditor();
		validate();
		repaint();
	}

	public void setData(MDSplus.Data data)
	{
		this.data = data;
		if (data == null)
			mode_idx = 0;
		else if (data instanceof MDSplus.Window)
			mode_idx = 1;
		else
			mode_idx = 2;
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (expr_edit != null)
			expr_edit.setEditable(editable);
		if (window_edit != null)
			window_edit.setEditable(editable);
	}
}
