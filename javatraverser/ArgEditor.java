//package jTraverser;
import javax.swing.*;
import java.awt.*;

import java.awt.event.*;

public class ArgEditor extends JPanel
{
    int num_args;
    Dimension preferred;
    ExprEditor [] args;
    public ArgEditor(Data []data) {this(data, 8, new Dimension(220, 90));}
    public ArgEditor() {this(null, 8, new Dimension(220, 90));}
    public ArgEditor(Data [] data, int num_args, Dimension preferred)
    {
	if(data == null)
	    data = new Data[num_args];
	this.preferred = preferred;
	this.num_args = num_args;
	JPanel jp = new JPanel();
	GridLayout gl = new GridLayout(num_args, 1);
	gl.setVgap(0);
	jp.setLayout(gl);
	args = new ExprEditor[num_args];
	for(int i = 0; i < num_args; i++)
	{   if(i < data.length)
		args[i] = new ExprEditor(data[i], false);
	    else
		args[i] = new ExprEditor(null, false);
	    jp.add(new LabeledExprEditor("Arg " + (i+1) + ":", args[i]));
	}
	JScrollPane jscroll = new JScrollPane(jp);
	jscroll.setPreferredSize(preferred);
	add(jscroll);
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
    
    