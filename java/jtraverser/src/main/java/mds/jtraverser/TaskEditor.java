package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class TaskEditor extends JPanel implements ActionListener, Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	int dtype_idx, curr_dtype_idx;
	LabeledExprEditor expr_edit;
	MethodEditor method_edit;
	RoutineEditor routine_edit;
	ProgramEditor program_edit;
	ProcedureEditor procedure_edit;
	JComboBox<String> combo;
	MDSplus.Data data;
	boolean editable = true;
	TreeDialog dialog;

	public TaskEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
			dtype_idx = 0;
		else
		{
			if (data instanceof MDSplus.Method)
				dtype_idx = 1;
			else if (data instanceof MDSplus.Routine)
				dtype_idx = 2;
			else if (data instanceof MDSplus.Procedure)
				dtype_idx = 3;
			else if (data instanceof MDSplus.Program)
				dtype_idx = 4;
			else
				dtype_idx = 5;
		}
		curr_dtype_idx = dtype_idx;
		final String names[] =
		{ "Undefined", "Method", "Routine", "Procedure", "Program", "Expression" };
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
			remove(method_edit);
			break;
		case 2:
			remove(routine_edit);
			break;
		case 3:
			remove(procedure_edit);
			break;
		case 4:
			remove(program_edit);
			break;
		case 5:
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
				method_edit = new MethodEditor((MDSplus.Method) data);
			else
				method_edit = new MethodEditor(null);
			add(method_edit);
			break;
		case 2:
			if (dtype_idx == curr_dtype_idx)
				routine_edit = new RoutineEditor((MDSplus.Routine) data);
			else
				routine_edit = new RoutineEditor(null);
			add(routine_edit);
			break;
		case 3:
			if (dtype_idx == curr_dtype_idx)
				procedure_edit = new ProcedureEditor((MDSplus.Procedure) data);
			else
				procedure_edit = new ProcedureEditor(null);
			add(procedure_edit);
			break;
		case 4:
			if (dtype_idx == curr_dtype_idx)
				program_edit = new ProgramEditor((MDSplus.Program) data);
			else
				program_edit = new ProgramEditor(null);
			add(program_edit);
			break;
		case 5:
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
			return method_edit.getData();
		case 2:
			return routine_edit.getData();
		case 3:
			return procedure_edit.getData();
		case 4:
			return program_edit.getData();
		case 5:
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
			remove(method_edit);
			break;
		case 2:
			remove(routine_edit);
			break;
		case 3:
			remove(procedure_edit);
			break;
		case 4:
			remove(program_edit);
			break;
		case 5:
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
		else
		{
			if (data instanceof MDSplus.Method)
				dtype_idx = 1;
			else if (data instanceof MDSplus.Routine)
				dtype_idx = 2;
			else if (data instanceof MDSplus.Procedure)
				dtype_idx = 3;
			else if (data instanceof MDSplus.Program)
				dtype_idx = 4;
			else
				dtype_idx = 5;
		}
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (expr_edit != null)
			expr_edit.setEditable(editable);
		if (method_edit != null)
			method_edit.setEditable(editable);
		if (routine_edit != null)
			routine_edit.setEditable(editable);
		if (program_edit != null)
			program_edit.setEditable(editable);
		if (procedure_edit != null)
			procedure_edit.setEditable(editable);
	}
}
