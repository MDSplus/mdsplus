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

public class ProgramEditor extends JPanel implements Editor
{
    ProgramData program;
    LabeledExprEditor program_edit, timeout_edit;
    
    public ProgramEditor() {this(null);}
    public ProgramEditor(ProgramData program)
    {
	this.program = program;
	if(this.program == null)
	{
	    this.program = new ProgramData(null, null);
	}
	program_edit = new LabeledExprEditor("Program", new ExprEditor(
	    this.program.getProgram(), true));
	timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor( 
	    this.program.getTimeout(), false));
    JPanel jp =new JPanel();
	jp.setLayout(new GridLayout(2,1));
	jp.add(program_edit);
	jp.add(timeout_edit);
	setLayout(new BorderLayout());
    add(jp, BorderLayout.NORTH);
    }
    
    public void reset()
    {
	program_edit.reset();
	timeout_edit.reset();
    }
    
    public Data getData()
    {
	return new ProgramData(timeout_edit.getData(), program_edit.getData());
    }
    
    public void setData(Data data)
    {
	this.program = (ProgramData)data;
	if(this.program == null)
	{
	    this.program = new ProgramData(null, null);
	}
	reset();
    }
	
    public void setEditable(boolean editable)
    {
	if(program_edit != null) program_edit.setEditable(editable);
	if(timeout_edit != null) timeout_edit.setEditable(editable);
    }
	
}