package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

//package jTraverser;
import javax.swing.JComboBox;
import javax.swing.JPanel;

public class AxisEditor extends JPanel implements ActionListener, Editor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	RangeEditor range_edit;
	LabeledExprEditor expr_edit, units_edit;
	JComboBox<String> combo;
	JPanel mode_panel;
	int mode_idx, curr_mode_idx;
	MDSplus.Data data;
	MDSplus.Range range;
	MDSplus.Data units;
	boolean editable = true;
	TreeDialog dialog;

	public AxisEditor(MDSplus.Data data, TreeDialog dialog)
	{
		this.dialog = dialog;
		this.data = data;
		if (data == null)
		{
			mode_idx = 0;
			data = null;
			range = null;
			units = null;
		}
		else
		{
			units = data.getUnits();
			this.data = data;
			if (data instanceof MDSplus.Range)
			{
				mode_idx = 1;
				range = (MDSplus.Range) this.data;
				this.data = null;
			}
			else
				mode_idx = 2;
		}
		curr_mode_idx = mode_idx;
		final String names[] =
		{ "Undefined", "Range", "Expression" };
		combo = new JComboBox<>(names);
		combo.setEditable(false);
		combo.setSelectedIndex(mode_idx);
		combo.addActionListener(this);
		mode_panel = new JPanel();
		mode_panel.add(combo);
		setLayout(new BorderLayout());
		add(mode_panel, "North");
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
			mode_panel.remove(units_edit);
			remove(range_edit);
			break;
		case 2:
			mode_panel.remove(units_edit);
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
			range_edit = new RangeEditor(range);
			units_edit = new LabeledExprEditor("Units", new ExprEditor(units, true));
			mode_panel.add(units_edit);
			add(range_edit, "Center");
			break;
		case 2:
			expr_edit = new LabeledExprEditor(data);
			units_edit = new LabeledExprEditor("Units", new ExprEditor(units, true));
			mode_panel.add(units_edit);
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
		{
			final MDSplus.Data units = units_edit.getData();
			final MDSplus.Data retData = range_edit.getData();
			if (units != null)
				retData.setUnits(units);
			return retData;
		}
		case 2:
		{
			units = units_edit.getData();
			final MDSplus.Data retData = expr_edit.getData();
			if (units != null)
				retData.setUnits(units);
			return retData;
		}
		}
		return null;
	}

	@Override
	public void reset()
	{
		switch (curr_mode_idx)
		{
		case 1:
			mode_panel.remove(units_edit);
			units_edit = null;
			remove(range_edit);
			range_edit = null;
			break;
		case 2:
			mode_panel.remove(units_edit);
			remove(expr_edit);
			expr_edit = null;
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
		{
			mode_idx = 0;
			data = null;
			range = null;
			units = null;
		}
		else
		{
			units = data.getUnits();
			this.data = data;
			if (data instanceof MDSplus.Range)
			{
				mode_idx = 1;
				range = (MDSplus.Range) this.data;
				this.data = null;
			}
			else
				mode_idx = 2;
		}
		reset();
	}

	@Override
	public void setEditable(boolean editable)
	{
		this.editable = editable;
		if (expr_edit != null)
			expr_edit.setEditable(editable);
		if (range_edit != null)
			range_edit.setEditable(editable);
		if (units_edit != null)
			units_edit.setEditable(editable);
	}
}
