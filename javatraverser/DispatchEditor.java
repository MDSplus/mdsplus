/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class DispatchEditor extends JPanel implements ActionListener,Editor
{
    DispatchEdt dispatch_edit;
    LabeledExprEditor expr_edit;
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
	add(jp, BorderLayout.PAGE_START);
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
		add(dispatch_edit);
		break;
	    case 2:
		if(dtype_idx == curr_dtype_idx)
		    dispatch_edit = new DispatchEdt((DispatchData)data, true);
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
	repaint();
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
	ident_edit = new LabeledExprEditor("Ident", new ExprEditor(this.data.getIdent(), true));
	phase_edit = new LabeledExprEditor("Phase", new ExprEditor(this.data.getPhase(), true));
	ident_edit = new LabeledExprEditor("Ident", new ExprEditor(this.data.getIdent(), true));
	if(is_sequential)
	    sequence_edit = new LabeledExprEditor("Sequence",
		new ExprEditor(this.data.getWhen(), false));
	else
    	    sequence_edit = new LabeledExprEditor("After",
		new ExprEditor(this.data.getWhen(), false));

	completion_edit = new LabeledExprEditor(  "Completion",
	    new ExprEditor(this.data.getCompletion(), true));
    JPanel jp =new JPanel();
	jp.setLayout(new GridLayout(4,1));
	jp.add(ident_edit);
	jp.add(phase_edit);
	jp.add(sequence_edit);
	jp.add(completion_edit);
	setLayout(new BorderLayout());
    add(jp, BorderLayout.NORTH);
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