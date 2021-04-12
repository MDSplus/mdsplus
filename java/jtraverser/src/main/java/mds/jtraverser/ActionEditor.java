package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class ActionEditor extends JPanel implements ActionListener, Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	TaskEditor task_edit;
	DispatchEditor dispatch_edit;
	LabeledExprEditor notify_edit, expr_edit;
	JComboBox<String> combo;
	int mode_idx, curr_mode_idx;
	MDSplus.Data data;
	JPanel action_panel;
	boolean editable;
	TreeDialog dialog;

	public ActionEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
			mode_idx = 0;
		else
		{
			if (data instanceof MDSplus.Action)
				mode_idx = 1;
			else
				mode_idx = 2;
		}
		if (data == null)
			this.data = new MDSplus.Action(null, null, null, null, null);
		curr_mode_idx = mode_idx;
		final String names[] =
		{ "Undefined", "Action", "Expression" };
		combo = new JComboBox<>(names);
		combo.setEditable(false);
		combo.setSelectedIndex(mode_idx);
		combo.addActionListener(this);
		setLayout(new BorderLayout());
		final JPanel jp = new JPanel();
		jp.add(combo);
		add(jp, BorderLayout.NORTH);
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
			remove(action_panel);
			action_panel = null;
			task_edit = null;
			dispatch_edit = null;
			remove(notify_edit);
			notify_edit = null;
			break;
		case 2:
			remove(expr_edit);
			expr_edit = null;
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
			if (curr_mode_idx == mode_idx)
			{
				dispatch_edit = new DispatchEditor(((MDSplus.Action) data).getDispatch(), dialog);
				task_edit = new TaskEditor(((MDSplus.Action) data).getTask(), dialog);
				notify_edit = new LabeledExprEditor("Notify",
						new ExprEditor(((MDSplus.Action) data).getErrorLog(), true));
			}
			else
			{
				dispatch_edit = new DispatchEditor(null, dialog);
				task_edit = new TaskEditor(null, dialog);
				notify_edit = new LabeledExprEditor("Notify", new ExprEditor(null, true));
			}
			action_panel = new JPanel();
			action_panel.setLayout(new GridLayout(1, 2));
			action_panel.add(dispatch_edit);
			action_panel.add(task_edit);
			add(action_panel, BorderLayout.CENTER);
			add(notify_edit, BorderLayout.SOUTH);
			break;
		case 2:
			expr_edit = new LabeledExprEditor(data);
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
			final MDSplus.Data a = new MDSplus.Action(dispatch_edit.getData(), task_edit.getData(), null, null, null);
			a.setCtxTree(Tree.curr_experiment);
			return a;
		case 2:
			return expr_edit.getData();
		}
		return null;
	}

	@Override
	public void reset()
	{
		combo.setSelectedIndex(mode_idx);
		switch (curr_mode_idx)
		{
		case 1:
			remove(action_panel);
			remove(notify_edit);
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
		else
		{
			if (data instanceof MDSplus.Action)
				mode_idx = 1;
			else
				mode_idx = 2;
		}
		if (data == null)
			this.data = new MDSplus.Action(null, null, null, null, null);
		this.data.setCtxTree(Tree.curr_experiment);
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (task_edit != null)
			task_edit.setEditable(editable);
		if (dispatch_edit != null)
			dispatch_edit.setEditable(editable);
		if (expr_edit != null)
			expr_edit.setEditable(editable);
		if (notify_edit != null)
			notify_edit.setEditable(editable);
	}
}
