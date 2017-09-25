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

public class ExprEditor extends JPanel implements ActionListener, Editor{
    String expr;
    Data data;
    int rows, columns;
    JButton left = null, right = null;
    JTextArea text_area;
    JTextField text_field;
    JPanel pl, pr;
    boolean default_scroll;
    boolean default_to_string;
    boolean quotes_added;
    boolean editable = true;

    public ExprEditor(Data data, boolean default_to_string)
    {
	this(data, default_to_string, 1, 20);
    }
    public ExprEditor(boolean default_to_string)
    {
	this(null, default_to_string, 1, 20);
    }
    public ExprEditor(Data data, boolean default_to_string, 
	int rows, int columns)
    {
    boolean quotes_needed;
	JScrollPane scroll_pane;
    this.rows = rows;
	this.columns = columns;
	this.default_to_string = default_to_string;
    if(rows > 1)
	    default_scroll = true;
	if(data != null)
	    expr = Tree.dataToString(data);
	else
	    expr = null;
	quotes_needed = (default_to_string && (expr == null || expr.charAt(0) == '\"'));
	if(quotes_needed)
	{
	    quotes_added = true;
	    left = new JButton("\"");
	    right = new JButton("\"");
	    left.setMargin(new Insets(0,0,0,0));
	    right.setMargin(new Insets(0,0,0,0));
	    left.addActionListener(this);
	    right.addActionListener(this);
	    if(expr != null)
		expr = expr.substring(1, expr.length() -1);
	}  
	else
	    quotes_added = false;
    setLayout(new BorderLayout());
	if(default_scroll) 
	{  
	    text_area = new JTextArea(rows,columns);
	    Dimension d = text_area.getPreferredSize();
	    text_area.setText(expr);
	    d.height += 20;
	    d.width += 20;
	    scroll_pane = new JScrollPane(text_area); 
	    scroll_pane.setPreferredSize(d);
	    if(quotes_needed)
	    {
		pl = new JPanel();
		pl.setLayout(new BorderLayout());
		pl.add(left, "North");
		add(pl, "East");
	    }
	    add(scroll_pane, "Center");
	    if(quotes_needed)
	    {
		pr = new JPanel();
		pr.setLayout(new BorderLayout());
		pr.add(right, "North");
		add(pr, "West");
	    }
	}
	else
	{
	    if(quotes_needed) add(left, BorderLayout.LINE_START);
	    text_field = new JTextField(columns);
	    text_field.setText(expr);
        add(text_field);
	    if(quotes_needed) add(right, BorderLayout.LINE_END );
	}
    }
    public void actionPerformed(ActionEvent e)
    {
	if(!editable) return;
	quotes_added = false;
	if(default_scroll)
	{
	    remove(pl);
	    remove(pr);
	}
	else
	{
	    remove(left);
	    remove(right);
	}
	left = right = null;
	if(default_scroll)
	    expr = text_area.getText();
	else
	    expr = text_field.getText();
	expr = "\""+expr+"\"";
	if(default_scroll)
	    text_area.setText(expr);
	else
	    text_field.setText(expr);
	validate();
	repaint();
    }
    public void reset()
    {
	if(data == null)
	    expr = "";
	else
	    expr = Tree.dataToString(data);
	if(default_to_string)
	{
	    int len = expr.length();
	    if(len >= 2)
	        expr = expr.substring(1, len - 1);
	}
	if(default_scroll)
	    text_area.setText(expr);
	else
	    text_field.setText(expr);
    }
    public Data getData()
    {
	if(default_scroll)
	    expr = text_area.getText();
	else
	    expr = text_field.getText();
	    
	if(quotes_added)
	    return Tree.dataFromExpr("\"" + expr + "\"");
	else
	    return Tree.dataFromExpr(expr);
    }
    
    public void setData(Data data)
    {
	this.data = data;
	reset();
    }
    
    public void setEditable(boolean editable)
    {
	this.editable = editable;
	if(text_area != null) text_area.setEditable(editable);
	if(text_field != null) text_field.setEditable(editable);
    }	
    

}



