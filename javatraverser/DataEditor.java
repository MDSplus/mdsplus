package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DataEditor extends JPanel implements ActionListener, Editor
{
    ExprEditor expr_edit;
    LabeledExprEditor units_edit;
    JComboBox combo;
    JPanel mode_panel;
    int mode_idx, curr_mode_idx;
    Data data;
    Data units;
    boolean editable = true;
    TreeDialog dialog;
    
    public DataEditor(Data data, TreeDialog dialog)
    {
	this.dialog = dialog;
	this.data = data;
	if(data == null)
	{
	    mode_idx = 0;
	    this.data = null;
	    units = null;
	}
	else
	{
	    mode_idx = 1;
	    if(data.dtype == Data.DTYPE_WITH_UNITS)
	    {
		this.data = ((WithUnitsData)data).getDatum();
		units = ((WithUnitsData)data).getUnits();
	    }
	    else
	    {
		this.data = data;
		units = null;
	    }
	}

    	curr_mode_idx = mode_idx;
	String names[] = {"Undefined", "Expression"};
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
		if(data != null && data.dtype == Data.DTYPE_T)
		    expr_edit = new ExprEditor(data, true, 8, 30);
		else
		    expr_edit = new ExprEditor(data, false, 8, 30);
		units_edit = new LabeledExprEditor("Units: ", new ExprEditor(units, true));
		add(expr_edit, "Center");
		mode_panel.add(units_edit);
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
	if(curr_mode_idx == 1)
	{
	    remove(expr_edit);
	    mode_panel.remove(units_edit);
	}
	curr_mode_idx = idx;
	addEditor();
	validate();
	dialog.repack();
    }

		
    public void reset()
    {
	if(curr_mode_idx == 1)  
	{
	    remove(expr_edit); 
	    mode_panel.remove(units_edit);
	}
	curr_mode_idx = mode_idx;
	combo.setSelectedIndex(mode_idx);
	addEditor();
	validate();
	dialog.repack();
    }
    
    public Data getData()
    {
	switch(curr_mode_idx)  {
	    case 0: return null;
	    case 1: 
		Data units = units_edit.getData();
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
	    this.data = null;
	    units = null;
	}
	else
	{
	    mode_idx = 1;
	    if(data.dtype == Data.DTYPE_WITH_UNITS)
	    {
		this.data = ((WithUnitsData)data).getDatum();
		units = ((WithUnitsData)data).getUnits();
	    }
	    else
	    {
		this.data = data;
		units = null;
	    }
	}
	reset();
    }
    public void setEditable(boolean editable)
    {
	this.editable = editable;
    	if(expr_edit != null) expr_edit.setEditable(editable);
	if(units_edit != null) units_edit.setEditable(editable);
    }

	
}	
	