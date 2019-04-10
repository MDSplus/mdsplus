//package jTraverser;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

public class ModifyData extends NodeEditor
{
    boolean is_editable;
    ActionEditor action_edit = null;
    DataEditor data_edit = null;
    DispatchEditor dispatch_edit = null;
    RangeEditor range_edit = null;
    TaskEditor task_edit = null;
    WindowEditor window_edit = null;
    Editor curr_edit;
    JButton ok_b, apply_b, reset_b, cancel_b;
    JLabel tags;
    JLabel onoff;
    TreeDialog dialog;



    public ModifyData() {this(true);}
    public ModifyData(boolean editable)
    {
	    is_editable = editable;
	    setLayout(new BorderLayout());
	    JPanel ip = new JPanel();
	    ip.add(onoff = new JLabel(""));
	    ip.add(new JLabel("Tags: "));
	    ip.add(tags = new JLabel(""));
	    add(ip, "North");
	    JPanel jp = new JPanel();
	    add(jp, "South");
	    if(is_editable)
	    {
	        ok_b = new JButton("Ok");
    	    ok_b.addActionListener(new ActionListener () {
    	        public void actionPerformed(ActionEvent e) {ok();}
    	    });
	        jp.add(ok_b);
	        apply_b = new JButton("Apply");
    	    apply_b.addActionListener(new ActionListener () {
    	        public void actionPerformed(ActionEvent e) {apply();}
          	});
	        jp.add(apply_b);
	        reset_b = new JButton("Reset");
        	reset_b.addActionListener(new ActionListener () {
        	    public void actionPerformed(ActionEvent e) {
        		    reset();}
        	});
	        jp.add(reset_b);
	        addKeyListener(new KeyAdapter() {
	            public void keyTyped(KeyEvent e)
	            {
	                if(e.getKeyCode() == KeyEvent.VK_ENTER)
	                    ok();
	            }
	        });
	    }
	    cancel_b = new JButton("Cancel");
    	cancel_b.addActionListener(new ActionListener () {
    	    public void actionPerformed(ActionEvent e) {cancel();}
    	});
    	cancel_b.setSelected(true);
	    jp.add(cancel_b);
    }

    private void replace(Editor edit)
    {
	    if(curr_edit != null && curr_edit != edit)
	        remove((Component)curr_edit);
	    curr_edit = edit;
	    add((Component)edit, "Center");
	    //add(edit);
    }

    public void setNode(Node _node)
    {	Data data;
	node = _node;
	try{
	    data = node.getData();

	}catch(Exception e){data = null;}

	switch(node.getUsage())
	{
	    case NodeInfo.USAGE_ACTION:
		if(action_edit == null)
		    action_edit = new ActionEditor(data, frame);
		else
		    action_edit.setData(data);
		action_edit.setEditable(is_editable);
		replace(action_edit);
		break;
	    case NodeInfo.USAGE_DISPATCH:
		if(dispatch_edit == null)
		    dispatch_edit = new DispatchEditor(data, frame);
		else
		    dispatch_edit.setData(data);
		replace(dispatch_edit);
		dispatch_edit.setEditable(is_editable);
		break;
	    case NodeInfo.USAGE_TASK:
		if(task_edit == null)
		    task_edit = new TaskEditor(data, frame);
		else
		    task_edit.setData(data);
		replace(task_edit);
		task_edit.setEditable(is_editable);
		break;
	    case NodeInfo.USAGE_WINDOW:
		if(window_edit == null)
		    window_edit = new WindowEditor(data, frame);
		else
		    window_edit.setData(data);
		replace(window_edit);
		window_edit.setEditable(is_editable);
		break;
	    case NodeInfo.USAGE_AXIS:
	    if(data instanceof RangeData)
	    {
		    if(range_edit == null)
		        range_edit = new RangeEditor((RangeData)data);
		    else
		        range_edit.setData(data);
    		replace(range_edit);
		    range_edit.setEditable(is_editable);
		    break;
		}
	    default :
		if(data_edit == null)
		    data_edit = new DataEditor(data, frame);
		else
		    data_edit.setData(data);
		replace(data_edit);
		data_edit.setEditable(is_editable);
	}

	if(node.isOn())
	    onoff.setText("Node is On   ");
	else
	    onoff.setText("Node is Off  ");
	try {
	    if(is_editable)
	        frame.setTitle("Modify data of " + node.getInfo().getFullPath());
	    else
	        frame.setTitle("Display data of " + node.getInfo().getFullPath());
	}catch(Exception exc){}

	tags.setText(tagList(node.getTags()));

	this.node = node;
    }

    private void cancel()
    {
	frame.dispose();
    }

    private boolean apply()
    {
	try {
	    node.setData(curr_edit.getData());
	} catch(Exception e)
	    {
		JOptionPane.showMessageDialog(frame, e.getMessage(), "Error writing datafile", JOptionPane.ERROR_MESSAGE);
		return false;
	    }
	return true;
    }

    private void reset()
    {
	curr_edit.reset();
	validate();
	repaint();
    }

    private void ok()
    {
	if(apply())
	    cancel();
    }

    private String tagList(String [] tags)
    {
	if(tags == null || tags.length == 0)
	    return "<no tags>";
	StringBuffer sb = new StringBuffer();
	for(int i = 0; i < tags.length; i++)
	{
	    sb.append(tags[i]);
	    if(i < tags.length - 1)
	    sb.append(", ");
	}
	return new String(sb);
    }

}