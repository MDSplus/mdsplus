package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class DataEditor extends JPanel implements ActionListener, Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	LabeledExprEditor expr_edit, units_edit;
	ParameterEditor param_edit;
	PythonEditor python_edit;
	JPanel panel;
	JComboBox<String> combo;
	int mode_idx, curr_mode_idx;
	MDSplus.Data data;
	MDSplus.Data units;
	boolean editable = true;
	TreeDialog dialog;

	public DataEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
		{
			mode_idx = 0;
			this.data = null;
			units = null;
		}
		else
		{
			if (data.getHelp() != null)
				mode_idx = 2;
			else if (data instanceof MDSplus.Function && ((MDSplus.Function) data).getOpcode() == PythonEditor.OPC_FUN)
			{
				final MDSplus.Data[] args = ((MDSplus.Function) data).getArguments();
				try
				{
					if (args != null && args.length > 2 && args[1] != null && (args[1] instanceof MDSplus.String)
							&& args[1].getString() != null && args[1].getString().toUpperCase().equals("PY"))
						mode_idx = 3;
					else
						mode_idx = 1;
				}
				catch (final Exception exc)
				{
					mode_idx = 1;
				}
			}
			else
				mode_idx = 1;
			if (data.getUnits() != null)
			{
				this.data = data;
				units = data.getUnits();
			}
			else
			{
				this.data = data;
				units = null;
			}
		}
		curr_mode_idx = mode_idx;
		final String names[] =
		{ "Undefined", "Expression", "Parameter", "Python Expression" };
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
		remove(panel);
		curr_mode_idx = idx;
		addEditor();
		validate();
		dialog.repack();
	}

	private void addEditor()
	{
		panel = new JPanel();
		panel.setLayout(new BorderLayout());
		switch (curr_mode_idx)
		{
		case 0:
			return;
		case 1:
			panel.add(expr_edit = new LabeledExprEditor(data));
			break;
		case 2:
			MDSplus.Data _data, _help = null, _validation = null;
			if (data != null && (data.getHelp() != null || data.getValidation() != null))
			{
				_data = data;
				_help = data.getHelp();
				_validation = data.getValidation();
			}
			else
				_data = data;
			param_edit = new ParameterEditor(new ExprEditor(_data, false, 3, 20), new ExprEditor(_help, true, 4, 20),
					new ExprEditor(_validation, false, 1, 20));
			panel.add(param_edit);
			break;
		case 3:
			if (data != null && data instanceof MDSplus.Function)
			{
				python_edit = new PythonEditor(((MDSplus.Function) data).getArguments());
			}
			else
			{
				python_edit = new PythonEditor(null);
			}
			panel.add(python_edit);
			break;
		}
		units_edit = new LabeledExprEditor("Units", new ExprEditor(units, true));
		panel.add(units_edit, BorderLayout.NORTH);
		add(panel, BorderLayout.CENTER);
	}

	@Override
	public MDSplus.Data getData()
	{
		MDSplus.Data units;
		switch (curr_mode_idx)
		{
		case 0:
			return null;
		case 1:
			units = units_edit.getData();
			if (units != null)
			{
				if (units instanceof MDSplus.String && ((MDSplus.String) units).getString().equals(""))
					return expr_edit.getData();
				else
				{
					final MDSplus.Data retData = expr_edit.getData();
					retData.setUnits(units);
					return retData;
				}
			}
			else
				return expr_edit.getData();
		case 2:
			units = units_edit.getData();
			if (units != null)
			{
				if (units instanceof MDSplus.String && ((MDSplus.String) units).getString().equals(""))
					return param_edit.getData();
				else
				{
					final MDSplus.Data retData = param_edit.getData();
					retData.setUnits(units);
					return retData;
				}
			}
			else
				return param_edit.getData();
		case 3:
			units = units_edit.getData();
			if (units != null)
			{
				if (units instanceof MDSplus.String && ((MDSplus.String) units).getString().equals(""))
					return python_edit.getData();
				else
				{
					final MDSplus.Data retData = python_edit.getData();
					retData.setUnits(units);
					return retData;
				}
			}
			else
				return python_edit.getData();
		}
		return null;
	}

	@Override
	public void reset()
	{
		if (curr_mode_idx > 0)
			remove(panel);
		curr_mode_idx = mode_idx;
		combo.setSelectedIndex(mode_idx);
		addEditor();
		validate();
		dialog.repack();
	}

	public void setData(MDSplus.Data data)
	{
		this.data = data;
		if (data == null)
		{
			mode_idx = 0;
			this.data = null;
			units = null;
		}
		else
		{
			if (data.getHelp() != null)
				mode_idx = 2;
			else if (data instanceof MDSplus.Function && ((MDSplus.Function) data).getOpcode() == PythonEditor.OPC_FUN)
			{
				final MDSplus.Data[] args = ((MDSplus.Function) data).getArguments();
				try
				{
					if (args != null && args.length > 2 && args[1] != null && (args[1] instanceof MDSplus.String)
							&& args[1].getString() != null && args[1].getString().toUpperCase().equals("PY"))
						mode_idx = 3;
					else
						mode_idx = 1;
				}
				catch (final Exception exc)
				{
					mode_idx = 1;
				}
			}
			else
				mode_idx = 1;
			if (data.getUnits() != null)
			{
				this.data = data;
				units = data.getUnits();
			}
			else
			{
				this.data = data;
				units = null;
			}
		}
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (expr_edit != null)
			expr_edit.setEditable(editable);
		if (python_edit != null)
			python_edit.setEditable(editable);
		if (units_edit != null)
			units_edit.setEditable(editable);
	}
}
