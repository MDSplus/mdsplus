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

public class MethodEditor extends JPanel implements Editor
{
    MethodData method;
    LabeledExprEditor device_edit, method_edit, timeout_edit;
    ArgEditor arg_edit;
    
    public MethodEditor() {this(null);}
    public MethodEditor(MethodData method)
    {
	this.method = method;
	if(this.method == null)
	{
	    this.method = new MethodData(null, null, null, new Data[0]);
	}
	setLayout(new BorderLayout());
	device_edit = new LabeledExprEditor("Device", new ExprEditor(
	    this.method.getDevice(), true));
	method_edit = new LabeledExprEditor("Method", new ExprEditor(
	    this.method.getMethod(), true));
    JPanel jp = new JPanel();
    jp.setLayout(new GridLayout(2,1));
	jp.add(device_edit);
	jp.add(method_edit);
    add(jp, BorderLayout.NORTH);
   	arg_edit = new ArgEditor(this.method.getArguments());
	add(arg_edit, BorderLayout.CENTER);
	timeout_edit = new LabeledExprEditor("Timeout", new ExprEditor( 
	    this.method.getTimeout(), false));
	add(timeout_edit, BorderLayout.SOUTH);
    }
    
    public void reset()
    {
	device_edit.reset();
	method_edit.reset();
	arg_edit.reset();
	timeout_edit.reset();
    }
    
    public Data getData()
    {
	return new MethodData(timeout_edit.getData(), method_edit.getData(), 
	    device_edit.getData(), arg_edit.getData());
    }
    
    public void setData(Data data)
    {
    	this.method = (MethodData) data;
	if(this.method == null)
	{
	    this.method = new MethodData(null, null, null, new Data[0]);
	}
	reset();
    }

    public void setEditable(boolean editable)
    {
        if(device_edit != null) device_edit.setEditable(editable);
	if(method_edit != null) method_edit.setEditable(editable);
	if(timeout_edit != null) timeout_edit.setEditable(editable);
	if(arg_edit != null) arg_edit.setEditable(editable);
    }

    
	
}