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

public class TaskEditor extends JPanel implements ActionListener, Editor
{
    int dtype_idx, curr_dtype_idx;
    LabeledExprEditor expr_edit;
    MethodEditor method_edit;
    RoutineEditor routine_edit;
    ProgramEditor program_edit;
    ProcedureEditor procedure_edit;
    JComboBox combo;
    Data data;
    boolean editable = true;
    TreeDialog dialog;
    
    public TaskEditor(Data data, TreeDialog dialog)
    {
	this.dialog = dialog;
	this.data = data;
	if(data == null)
	    dtype_idx = 0;
	else
	{
	    switch(data.dtype) {
		case Data.DTYPE_METHOD: dtype_idx = 1; break; 
		case Data.DTYPE_ROUTINE: dtype_idx = 2; break; 
		case Data.DTYPE_PROCEDURE: dtype_idx = 3; break; 
		case Data.DTYPE_PROGRAM: dtype_idx = 4; break; 
		default: dtype_idx  = 5;
	    }
	}
	curr_dtype_idx = dtype_idx;
	String names[] = {"Undefined", "Method", "Routine", "Procedure", "Program",
	    "Expression"};
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
		    method_edit = new MethodEditor((MethodData)data);
		else
		    method_edit = new MethodEditor(null);
		add(method_edit);
		break;
	    case 2: 
		if(dtype_idx == curr_dtype_idx) 
		    routine_edit = new RoutineEditor((RoutineData)data);
		else
		    routine_edit = new RoutineEditor(null);
		add(routine_edit);
		break;
	    case 3: 
		if(dtype_idx == curr_dtype_idx) 
		    procedure_edit = new ProcedureEditor((ProcedureData)data);
		else
		    procedure_edit = new ProcedureEditor(null);
		add(procedure_edit);
		break;
	    case 4: 
		if(dtype_idx == curr_dtype_idx) 
		    program_edit = new ProgramEditor((ProgramData)data);
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
	    case 1: remove(method_edit); break;
	    case 2: remove(routine_edit); break;
	    case 3: remove(procedure_edit); break;
	    case 4: remove(program_edit); break;
	    case 5: remove(expr_edit); break;
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
		case 1: remove(method_edit); break;
		case 2: remove(routine_edit); break;
		case 3: remove(procedure_edit); break;
		case 4: remove(program_edit); break;
		case 5: remove(expr_edit); break;
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
	    case 1: return method_edit.getData(); 
	    case 2: return routine_edit.getData(); 
	    case 3: return procedure_edit.getData(); 
	    case 4: return program_edit.getData(); 
	    case 5: return expr_edit.getData(); 
	}
	return null;
    }
    
    public void setData(Data data)
    {
    	this.data = data;
	if(data == null)
	    dtype_idx = 0;
	else
	{
	    switch(data.dtype) {
		case Data.DTYPE_METHOD: dtype_idx = 1; break; 
		case Data.DTYPE_ROUTINE: dtype_idx = 2; break; 
		case Data.DTYPE_PROCEDURE: dtype_idx = 3; break; 
		case Data.DTYPE_PROGRAM: dtype_idx = 4; break; 
		default: dtype_idx  = 5;
	    }
	}
	reset();
    }
    
    public void setEditable(boolean editable)
    {
	this.editable = editable;
        if(expr_edit != null) expr_edit.setEditable(editable);
	if(method_edit != null) method_edit.setEditable(editable);
	if(routine_edit != null) routine_edit.setEditable(editable);
	if(program_edit != null) program_edit.setEditable(editable);
	if(procedure_edit != null) procedure_edit.setEditable(editable);
    }


} 
	  
	  
		    