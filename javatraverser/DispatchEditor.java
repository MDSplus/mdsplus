//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DispatchEditor extends JPanel implements ActionListener,Editor
{
    DispatchEdt dispatch_edit;
    ExprEditor expr_edit;
    JComboBox combo;
    Data data;
    int dtype_idx, curr_dtype_idx;
    boolean editable = true;
    TreeDialog dialog;
    
    public DispatchEditor(Data data, TreeDialog dialog)
    {
	this.dialog = dialog;
	this.data = data;
	if(data == null)
	    dtype_idx = 0;
	else if(data.dtype == Data.DTYPE_DISPATCH)
	{
	    DispatchData ddata = (DispatchData)data;
	    if(ddata.getType() == DispatchData.SCHED_SEQ)
		dtype_idx = 1;
	    else
		dtype_idx = 2;
	}
	else
	    dtype_idx = 3;
	curr_dtype_idx = dtype_idx;
	String[] names = {"Undefined", "Sequential", "Conditional", "Expression"};
	combo = new JComboBox(names);
	combo.setEditable(false);
	combo.setSelectedIndex(dtype_idx);
	combo.addActionListener(this);
	setLayout(new BorderLayout());
	JPanel jp = new JPanel();
	jp.add(combo);
	add(jp, "North");
	addEditor();
    }
    
    private void addEditor()
    {
	switch(curr_dtype_idx) {
	    case 0: return;
	    case 1: 
		if(dtype_idx == curr_dtype_idx) 
		    dispatch_edit = new DispatchEdt((DispatchData)data, false);
		else
		    dispatch_edit = new DispatchEdt(null, false);
		add(dispatch_edit, "South");
		break;
	    case 2: 
		if(dtype_idx == curr_dtype_idx) 
		    dispatch_edit = new DispatchEdt((DispatchData)data, true);
		else
		    dispatch_edit = new DispatchEdt(null, true);
		add(dispatch_edit, "South");
		break;
	    case 3: 
		if(dtype_idx == curr_dtype_idx) 
		    expr_edit = new ExprEditor(data, false, 5,30);
		else
		    expr_edit = new ExprEditor(null, false, 5, 30);
		add(expr_edit, "South");
		break;
	}
    } 
	
	
    
    public void actionPerformed(ActionEvent e)
    {
	if(!editable)
	{
	    combo.setSelectedIndex(curr_dtype_idx);
	    return;
	}
	int idx = combo.getSelectedIndex();
	if(idx == curr_dtype_idx) return;
	switch(curr_dtype_idx)  {
	    case 1: 
	    case 2: remove(dispatch_edit); break;
	    case 3: remove(expr_edit); break;
	}
	curr_dtype_idx = idx;
	addEditor();
	validate();
	dialog.repack();
    }

		
    public void reset()
    {
	switch(curr_dtype_idx)  {
		case 1: 
		case 2: remove(dispatch_edit); break;
		case 3: remove(expr_edit); break;
	}
	curr_dtype_idx = dtype_idx;
	addEditor();
	validate();
	repaint();
    }
    
    public Data getData()
    {
	switch(curr_dtype_idx)  {
	    case 0: return null;
	    case 1: 
	    case 2: return dispatch_edit.getData(); 
	    case 3: return expr_edit.getData(); 
	}
	return null;
    }
    
    public void setData(Data data)
    {
    	this.data = (DispatchData)data;
	if(data == null)
	    dtype_idx = 0;
	else if(data.dtype == Data.DTYPE_DISPATCH)
	{
	    DispatchData ddata = (DispatchData)data;
	    if(ddata.getType() == DispatchData.SCHED_SEQ)
		dtype_idx = 1;
	    else
		dtype_idx = 2;
	}
	else
	    dtype_idx = 3;
	reset();
    }

    public void setEditable(boolean editable)
    {
	this.editable = editable;
	if(dispatch_edit != null) dispatch_edit.setEditable(editable);
	if(expr_edit != null) expr_edit.setEditable(editable);
    }


class DispatchEdt extends JPanel
{
    boolean is_sequential = false;
    DispatchData data;
    LabeledExprEditor ident_edit, phase_edit, sequence_edit, completion_edit;
    public DispatchEdt(DispatchData data, boolean is_conditional)
    {
	this.data = data;
	if(this.data == null)
	{
	    if(is_conditional)
	        this.data = new DispatchData(DispatchData.SCHED_COND, null, null, null, null);
        else
	        this.data = new DispatchData(DispatchData.SCHED_SEQ, null, null, null, null);
    }
	if(this.data.getType() == DispatchData.SCHED_SEQ)
	    is_sequential = true;

    	GridLayout gl = new GridLayout(4,1);
	gl.setVgap(0);
	setLayout(gl);
	ident_edit = new LabeledExprEditor("Ident:     ", new ExprEditor(this.data.getIdent(), true));
	phase_edit = new LabeledExprEditor("Phase:     ", new ExprEditor(this.data.getPhase(), true));
	ident_edit = new LabeledExprEditor("Ident:     ", new ExprEditor(this.data.getIdent(), true));
	if(is_sequential)
	    sequence_edit = new LabeledExprEditor("Sequence:  ", 
		new ExprEditor(this.data.getWhen(), false));
	else
    	    sequence_edit = new LabeledExprEditor("After:     ", 
		new ExprEditor(this.data.getWhen(), false));
	
	completion_edit = new LabeledExprEditor(  "Completion:", 
	    new ExprEditor(this.data.getCompletion(), true));
	    
	add(ident_edit);
	add(phase_edit);
	add(sequence_edit);
	add(completion_edit);
    }
    public void reset()
    {	
	combo.setSelectedIndex(dtype_idx);
	ident_edit.reset();
	phase_edit.reset();
	sequence_edit.reset();
	completion_edit.reset();
    }
    
    public Data getData()
    {
	Data data;
	data = ident_edit.getData();
	data = phase_edit.getData();
	data = sequence_edit.getData();
	data = completion_edit.getData();
    
    
	return new DispatchData(is_sequential?DispatchData.SCHED_SEQ:DispatchData.SCHED_COND,
	    ident_edit.getData(), phase_edit.getData(), sequence_edit.getData(),
	    completion_edit.getData());
    }
    
    public void setEditable(boolean editable)
    {
	if(ident_edit != null) ident_edit.setEditable(editable);
	if(phase_edit != null) phase_edit.setEditable(editable);
	if(sequence_edit != null) sequence_edit.setEditable(editable);
	if(completion_edit != null) completion_edit.setEditable(editable);
    }
    

    
}
}