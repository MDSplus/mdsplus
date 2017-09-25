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

public class ArgEditor extends JScrollPane
{
    int num_args;
    Dimension preferred;
    ExprEditor [] args;
    public ArgEditor(Data []data) {this(data, 9, new Dimension(220, 89));}
    public ArgEditor() {this(null, 9, new Dimension(220, 89));}
    public ArgEditor(Data [] data, int num_args, Dimension preferred)
    {
	if(data == null)
	    data = new Data[num_args];
	this.preferred = preferred;
	this.num_args = num_args;
	JPanel jp = new JPanel();
	jp.setLayout(new GridLayout(num_args, 1));
	args = new ExprEditor[num_args];
	for(int i = 0; i < num_args; i++)
	{   if(i < data.length)
		args[i] = new ExprEditor(data[i], false);
	    else
		args[i] = new ExprEditor(null, false);
	    jp.add(new LabeledExprEditor("Argument " + (i+1), args[i]));
	}
    JPanel jp2 = new JPanel();
	jp2.setLayout(new BorderLayout());
    jp2.add(jp, BorderLayout.NORTH);
	setViewportView(jp2);
    setPreferredSize(preferred);
    getVerticalScrollBar().setUnitIncrement(43);
    }
    public  void reset()
    {
	for(int i = 0; i < num_args; i++)
	    args[i].reset();
    }
    
    public void setEditable(boolean editable)
    {
	for(int i = 0; i < num_args; i++)
	    args[i].setEditable(editable);
    }
    
    public Data [] getData()
    {
	Data data[] = new Data[num_args];
	for(int i = 0; i < num_args; i++)
	    data[i] = args[i].getData();
	return data;
    }
    
    public void setData(Data [] data)
    {
	int min_len, i;
	if(data == null)
	    min_len = 0;
	else
	    if(data.length < num_args)
		min_len = data.length;
	    else
		min_len = num_args;
	for(i = 0; i < min_len; i++)
	    args[i].setData(data[i]);
	for(;i < num_args; i++)
	    args[i].setData(null);
    }
    }
    
    