//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class AxisEditor extends JPanel implements ActionListener, Editor
{
    RangeEditor range_edit;
    LabeledExprEditor expr_edit, units_edit;
    JComboBox combo;
    JPanel mode_panel;
    int mode_idx, curr_mode_idx;
    Data data;
    RangeData range;
    Data units;
    boolean editable = true;
    TreeDialog dialog;

    public AxisEditor(Data data, TreeDialog dialog)
    {
	this.dialog = dialog;
	this.data = data;
	if(data == null)
	{
	    mode_idx = 0;
	    data = null;
	    range = null;
	    units = null;
	}
	else
	{
	    if(data.dtype == Data.DTYPE_WITH_UNITS)
	    {
		units = ((WithUnitsData)data).getUnits();
		this.data = ((WithUnitsData)data).getDatum();
	    }
	    else
		this.data = data;
	    if(this.data.dtype == Data.DTYPE_RANGE)
	    {
		mode_idx = 1;
		range = (RangeData)this.data;
		this.data = null;
	    }
	    else
		mode_idx = 2;
	}
	curr_mode_idx = mode_idx;
	String names[] = {"Undefined", "Range", "Expression"};
	combo = new JComboBox(names);
	combo.setEditable(false);
	combo.setSelectedIndex(mode_idx);
	combo.addActionListener(this);
	mode_panel = new JPanel();
	mode_panel.add(combo);
	setLayout(new BorderLayout());
	add(mode_panel, "North");
	addEditor();
    }
    private void addEditor()
    {
	switch(curr_mode_idx) {
	    case 0: return;
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


    public void actionPerformed(ActionEvent e)
    {
	if(!editable)
	{
	    combo.setSelectedIndex(curr_mode_idx);
	    return;
	}
	int idx = combo.getSelectedIndex();
	if(idx == curr_mode_idx)  return;
	switch(curr_mode_idx)  {
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


    public void reset()
    {
	switch(curr_mode_idx)  {
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

    public Data getData()
    {
	switch(curr_mode_idx)  {
	    case 0: return null;
	    case 1:
		Data units = units_edit.getData();
		if(units != null)
		    return new WithUnitsData(range_edit.getData(), units);
		else
		    return range_edit.getData();
	    case 2:
		units = units_edit.getData();
		if(units != null)
		    return new WithUnitsData(expr_edit.getData(), units);
		else
		    return expr_edit.getData();
	}
	return null;
    }

    public void setData(Data data)
    {
	this.data = data;
	if(data == null)
	{
	    mode_idx = 0;
	    data = null;
	    range = null;
	    units = null;
	}
	else
	{
	    if(data.dtype == Data.DTYPE_WITH_UNITS)
	    {
		units = ((WithUnitsData)data).getUnits();
		this.data = ((WithUnitsData)data).getDatum();
	    }
	    else
		this.data = data;
	    if(this.data.dtype == Data.DTYPE_RANGE)
	    {
		mode_idx = 1;
		range = (RangeData)this.data;
		this.data = null;
	    }
	    else
		mode_idx = 2;
	}
	reset();
    }

    public void setEditable(boolean editable)
    {
	this.editable = editable;
	if(expr_edit != null) expr_edit.setEditable(editable);
	if(range_edit != null) range_edit.setEditable(editable);
	if(units_edit != null) units_edit.setEditable(editable);
   }



}
