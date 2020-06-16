package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class DispatchEditor extends JPanel implements ActionListener, Editor
{
	class DispatchEdt extends JPanel
	{
		/**
		 *
		 */
		private static final long serialVersionUID = 1L;
		boolean is_sequential = false;
		MDSplus.Dispatch data;
		LabeledExprEditor ident_edit, phase_edit, sequence_edit, completion_edit;

		public DispatchEdt(MDSplus.Dispatch data, boolean is_conditional)
		{
			this.data = data;
			if (this.data == null)
			{
				if (is_conditional)
					this.data = new MDSplus.Dispatch(MDSplus.Dispatch.SCHED_COND, null, null, null, null);
				else
					this.data = new MDSplus.Dispatch(MDSplus.Dispatch.SCHED_SEQ, null, null, null, null);
			}
			this.data.setCtxTree(Tree.curr_experiment);
			if (this.data.getOpcode() == MDSplus.Dispatch.SCHED_SEQ)
				is_sequential = true;
			ident_edit = new LabeledExprEditor("Ident", new ExprEditor(this.data.getIdent(), true));
			phase_edit = new LabeledExprEditor("Phase", new ExprEditor(this.data.getPhase(), true));
			ident_edit = new LabeledExprEditor("Ident", new ExprEditor(this.data.getIdent(), true));
			if (is_sequential)
				sequence_edit = new LabeledExprEditor("Sequence", new ExprEditor(this.data.getWhen(), false));
			else
				sequence_edit = new LabeledExprEditor("After", new ExprEditor(this.data.getWhen(), false));
			completion_edit = new LabeledExprEditor("Completion", new ExprEditor(this.data.getCompletion(), true));
			final JPanel jp = new JPanel();
			jp.setLayout(new GridLayout(4, 1));
			jp.add(ident_edit);
			jp.add(phase_edit);
			jp.add(sequence_edit);
			jp.add(completion_edit);
			setLayout(new BorderLayout());
			add(jp, BorderLayout.NORTH);
		}

		public MDSplus.Data getData()
		{
			final MDSplus.Data d = new MDSplus.Dispatch(
					(is_sequential) ? MDSplus.Dispatch.SCHED_SEQ : MDSplus.Dispatch.SCHED_COND, ident_edit.getData(),
					phase_edit.getData(), sequence_edit.getData(), completion_edit.getData());
			d.setCtxTree((Tree.curr_experiment));
			return d;
		}

		public void reset()
		{
			combo.setSelectedIndex(dtype_idx);
			ident_edit.reset();
			phase_edit.reset();
			sequence_edit.reset();
			completion_edit.reset();
		}

		public void setEditable(boolean editable)
		{
			if (ident_edit != null)
				ident_edit.setEditable(editable);
			if (phase_edit != null)
				phase_edit.setEditable(editable);
			if (sequence_edit != null)
				sequence_edit.setEditable(editable);
			if (completion_edit != null)
				completion_edit.setEditable(editable);
		}
	}

	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DispatchEdt dispatch_edit;
	LabeledExprEditor expr_edit;
	JComboBox<String> combo;
	MDSplus.Data data;
	int dtype_idx, curr_dtype_idx;
	boolean editable = true;
	TreeDialog dialog;

	public DispatchEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
			dtype_idx = 0;
		else if (data instanceof MDSplus.Dispatch)
		{
			final MDSplus.Dispatch ddata = (MDSplus.Dispatch) data;
			if (ddata.getOpcode() == MDSplus.Dispatch.SCHED_SEQ)
				dtype_idx = 1;
			else
				dtype_idx = 2;
		}
		else
			dtype_idx = 3;
		curr_dtype_idx = dtype_idx;
		final String[] names =
		{ "Undefined", "Sequential", "Conditional", "Expression" };
		combo = new JComboBox<>(names);
		combo.setEditable(false);
		combo.setSelectedIndex(dtype_idx);
		combo.addActionListener(this);
		setLayout(new BorderLayout());
		final JPanel jp = new JPanel();
		jp.add(combo);
		add(jp, BorderLayout.PAGE_START);
		addEditor();
	}

	@Override
	public void actionPerformed(ActionEvent e)
	{
		if (!editable)
		{
			combo.setSelectedIndex(curr_dtype_idx);
			return;
		}
		final int idx = combo.getSelectedIndex();
		if (idx == curr_dtype_idx)
			return;
		switch (curr_dtype_idx)
		{
		case 1:
		case 2:
			remove(dispatch_edit);
			break;
		case 3:
			remove(expr_edit);
			break;
		}
		curr_dtype_idx = idx;
		addEditor();
		validate();
		dialog.repack();
		repaint();
	}

	private void addEditor()
	{
		switch (curr_dtype_idx)
		{
		case 0:
			return;
		case 1:
			if (dtype_idx == curr_dtype_idx)
				dispatch_edit = new DispatchEdt((MDSplus.Dispatch) data, false);
			else
				dispatch_edit = new DispatchEdt(null, false);
			add(dispatch_edit);
			break;
		case 2:
			if (dtype_idx == curr_dtype_idx)
				dispatch_edit = new DispatchEdt((MDSplus.Dispatch) data, true);
			else
				dispatch_edit = new DispatchEdt(null, true);
			add(dispatch_edit);
			break;
		case 3:
			expr_edit = new LabeledExprEditor(data);
			add(expr_edit);
			break;
		}
	}

	@Override
	public MDSplus.Data getData()
	{
		switch (curr_dtype_idx)
		{
		case 0:
			return null;
		case 1:
		case 2:
			return dispatch_edit.getData();
		case 3:
			return expr_edit.getData();
		}
		return null;
	}

	@Override
	public void reset()
	{
		switch (curr_dtype_idx)
		{
		case 1:
		case 2:
			remove(dispatch_edit);
			break;
		case 3:
			remove(expr_edit);
			break;
		}
		curr_dtype_idx = dtype_idx;
		addEditor();
		validate();
		repaint();
	}

	public void setData(MDSplus.Data data)
	{
		this.data = data;
		if (data == null)
			dtype_idx = 0;
		else if (data instanceof MDSplus.Dispatch)
		{
			final MDSplus.Dispatch ddata = (MDSplus.Dispatch) data;
			if (ddata.getOpcode() == MDSplus.Dispatch.SCHED_SEQ)
				dtype_idx = 1;
			else
				dtype_idx = 2;
		}
		else
			dtype_idx = 3;
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (dispatch_edit != null)
			dispatch_edit.setEditable(editable);
		if (expr_edit != null)
			expr_edit.setEditable(editable);
	}
}
