//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class ActionEditor extends JPanel implements ActionListener, Editor
{
    TaskEditor task_edit;
    DispatchEditor dispatch_edit;
    ExprEditor expr_edit;
    LabeledExprEditor notify_edit;
    JComboBox combo;
    int mode_idx, curr_mode_idx;
    Data data;
    JPanel notify_panel;
    boolean editable;
    TreeDialog dialog;
    
    public ActionEditor(Data data, TreeDialog dialog)
    {
	this.dialog = dialog;
	this.data = data;
	if(data == null)
	    mode_idx = 0;
	    
	else
	{
	    if(data.dtype == Data.DTYPE_ACTION)
		mode_idx = 1;
	    else
		mode_idx = 2;
	}
	if(data == null)
	    this.data = new ActionData(null, null, null, null, null);
	    
	curr_mode_idx = mode_idx;
	String names[] = {"Undefined", "Action", "Expression"};
	combo = new JComboBox(names);
	combo.setEditable(false);
	combo.setSelectedIndex(mode_idx);
	combo.addActionListener(this);
	JPanel jp = new JPanel();
	jp.add(combo);
	setLayout(new BorderLayout());
	add(jp, "North");
	addEditor();
    }    private void addEditor()
    {
	switch(curr_mode_idx) {
	    case 0: return;
	    case 1: 
		if(curr_mode_idx == mode_idx)
		{
		    dispatch_edit = new DispatchEditor(((ActionData)data).getDispatch(),dialog);
		    task_edit = new TaskEditor(((ActionData)data).getTask(),dialog);
		    notify_edit = new LabeledExprEditor(
			"Notify: ", new ExprEditor(((ActionData)data).getErrorlogs(), true));

		}
		else
		{
		    dispatch_edit = new DispatchEditor(null, dialog);
		    task_edit = new TaskEditor(null, dialog);
		    notify_edit = new LabeledExprEditor(
			"Notify: ", new ExprEditor(null, true));
		}
		add(dispatch_edit, "West");
		add(task_edit, "East");
		notify_panel = new JPanel();
		notify_panel.add(notify_edit);
		add(notify_panel, "South");
		break;
	    case 2: 
		if(curr_mode_idx == mode_idx)
		    expr_edit = new ExprEditor(data, false, 5, 30);
		else
		    expr_edit = new ExprEditor(null, false, 5, 30);
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
	if(idx == curr_mode_idx) return;
	switch(curr_mode_idx)  {
	    case 1: 
		remove(dispatch_edit);
		dispatch_edit = null; 
		remove(task_edit);
		task_edit = null;
		remove(notify_panel);
		notify_panel = null;
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

		
   public void reset()
    {
    	combo.setSelectedIndex(mode_idx);
	switch(curr_mode_idx)  {
		case 1: 
		    remove(dispatch_edit); 
		    remove(task_edit);
		    remove(notify_panel);
		    break;
		case 2: remove(expr_edit); break;
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
		Data data1 = dispatch_edit.getData();
		Data data2 = task_edit.getData();
		return new ActionData(dispatch_edit.getData(), task_edit.getData(),
		null, null, null); 
	    case 2: return expr_edit.getData(); 
	}
	return null;
    }
    
    public void setData(Data data)
    {
	this.data = data;
	if(data == null)
	    mode_idx = 0;
	else
	{
	    if(data.dtype == Data.DTYPE_ACTION)
		mode_idx = 1;
	    else
		mode_idx = 2;
	}
	if(data == null)
	    this.data = new ActionData(null, null, null, null, null);
	reset();
    }
    
    public void setEditable(boolean editable)
    {
	this.editable = editable;
	if(task_edit != null) task_edit.setEditable(editable);
	if(dispatch_edit != null) dispatch_edit.setEditable(editable);
	if(expr_edit != null) expr_edit.setEditable(editable);
	if(notify_edit != null) notify_edit.setEditable(editable);
    }
	
}	
	