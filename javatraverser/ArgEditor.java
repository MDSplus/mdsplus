//package jTraverser;
import javax.swing.*;
import java.awt.*;

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
    
    