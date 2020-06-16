package mds.jtraverser;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.event.*;

//package jTraverser;
import javax.swing.*;

public class ModifyData extends NodeEditor
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
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

	public ModifyData()
	{
		this(true);
	}

	public ModifyData(boolean editable)
	{
		is_editable = editable;
		setLayout(new BorderLayout());
		final JPanel ip = new JPanel();
		ip.add(onoff = new JLabel(""));
		ip.add(new JLabel("Tags: "));
		ip.add(tags = new JLabel(""));
		add(ip, "North");
		final JPanel jp = new JPanel();
		add(jp, "South");
		if (is_editable)
		{
			ok_b = new JButton("Ok");
			ok_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					ok();
				}
			});
			jp.add(ok_b);
			apply_b = new JButton("Apply");
			apply_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					apply();
				}
			});
			jp.add(apply_b);
			reset_b = new JButton("Reset");
			reset_b.addActionListener(new ActionListener()
			{
				@Override
				public void actionPerformed(ActionEvent e)
				{
					reset();
				}
			});
			jp.add(reset_b);
			addKeyListener(new KeyAdapter()
			{
				@Override
				public void keyTyped(KeyEvent e)
				{
					if (e.getKeyCode() == KeyEvent.VK_ENTER)
						ok();
				}
			});
		}
		cancel_b = new JButton("Cancel");
		cancel_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				cancel();
			}
		});
		cancel_b.setSelected(true);
		jp.add(cancel_b);
	}

	private boolean apply()
	{
		try
		{
			node.setData(curr_edit.getData());
		}
		catch (final Exception e)
		{
			JOptionPane.showMessageDialog(frame, e.getMessage(), "Error writing datafile", JOptionPane.ERROR_MESSAGE);
			return false;
		}
		return true;
	}

	private void cancel()
	{
		frame.dispose();
	}

	private void ok()
	{
		if (apply())
			cancel();
	}

	private void replace(Editor edit)
	{
		if (curr_edit != null && curr_edit != edit)
			remove((Component) curr_edit);
		curr_edit = edit;
		add((Component) edit, "Center");
		// add(edit);
	}

	private void reset()
	{
		curr_edit.reset();
		validate();
		repaint();
	}

	@Override
	public void setNode(Node _node)
	{
		MDSplus.Data data;
		node = _node;
		try
		{
			data = node.getData();
		}
		catch (final Exception e)
		{
			data = null;
		}
		final String usage = node.getUsage();
		if (usage.equals("ACTION"))
		{
			if (action_edit == null)
				action_edit = new ActionEditor(data, frame);
			else
				action_edit.setData(data);
			action_edit.setEditable(is_editable);
			replace(action_edit);
		}
		else if (usage.equals("DISPATCH"))
		{
			if (dispatch_edit == null)
				dispatch_edit = new DispatchEditor(data, frame);
			else
				dispatch_edit.setData(data);
			replace(dispatch_edit);
			dispatch_edit.setEditable(is_editable);
		}
		else if (usage.equals("TASK"))
		{
			if (task_edit == null)
				task_edit = new TaskEditor(data, frame);
			else
				task_edit.setData(data);
			replace(task_edit);
			task_edit.setEditable(is_editable);
		}
		else if (usage.equals("WINDOW"))
		{
			if (window_edit == null)
				window_edit = new WindowEditor(data, frame);
			else
				window_edit.setData(data);
			replace(window_edit);
			window_edit.setEditable(is_editable);
		}
		else if (usage.equals("AXIS"))
		{
			// if(data instanceof MDSplus.Range)
			{
				if (range_edit == null)
					range_edit = new RangeEditor((MDSplus.Range) data);
				else
					range_edit.setData(data);
				replace(range_edit);
				range_edit.setEditable(is_editable);
			}
		}
		else
		{
			if (data_edit == null)
				data_edit = new DataEditor(data, frame);
			else
				data_edit.setData(data);
			replace(data_edit);
			data_edit.setEditable(is_editable);
		}
		if (node.isOn())
			onoff.setText("Node is On   ");
		else
			onoff.setText("Node is Off  ");
		try
		{
			if (is_editable)
				frame.setTitle("Modify data of " + node.getInfo().getFullPath());
			else
				frame.setTitle("Display data of " + node.getInfo().getFullPath());
		}
		catch (final Exception exc)
		{}
		tags.setText(tagList(node.getTags()));
	}

	private String tagList(String[] tags)
	{
		if (tags == null || tags.length == 0)
			return "<no tags>";
		final StringBuffer sb = new StringBuffer();
		for (int i = 0; i < tags.length; i++)
		{
			sb.append(tags[i]);
			if (i < tags.length - 1)
				sb.append(", ");
		}
		return new String(sb);
	}
}
