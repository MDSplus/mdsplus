package mds.jtraverser.dialogs;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import mds.MdsException;
import mds.jtraverser.Node;

public final class TagsDialog extends JDialog
{
	private static final long serialVersionUID = 1L;

	public static TagsDialog open(final Node node)
	{
		final TagsDialog dialog = new TagsDialog(node);
		dialog.setLocation(10, 10);
		dialog.setVisible(true);
		return dialog;
	}

	private final JTextField curr_tag_selection;
	private final DefaultListModel<String> curr_taglist_model;
	private Node currnode;
	private final JList<String> modify_tags_list;
	private String[] tags;

	private TagsDialog(final Node node)
	{
		super(JOptionPane.getRootFrame());
		final JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		final JPanel jp1 = new JPanel();
		jp1.setLayout(new BorderLayout());
		this.modify_tags_list = new JList<>();
		this.curr_taglist_model = new DefaultListModel<>();
		this.modify_tags_list.addListSelectionListener(new ListSelectionListener()
		{
			@Override
			public void valueChanged(final ListSelectionEvent e)
			{
				final int idx = TagsDialog.this.modify_tags_list.getSelectedIndex();
				if (idx != -1)
					TagsDialog.this.curr_tag_selection.setText(TagsDialog.this.curr_taglist_model.getElementAt(idx));
			}
		});
		final JScrollPane scroll_list = new JScrollPane(this.modify_tags_list);
		jp1.add(new JLabel("Tag List:"), BorderLayout.NORTH);
		jp1.add(scroll_list, BorderLayout.CENTER);
		final JPanel jp2 = new JPanel();
		jp2.setLayout(new GridLayout(2, 1));
		final JButton add_tag = new JButton("Add Tag");
		add_tag.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				final String curr_tag = TagsDialog.this.curr_tag_selection.getText().toUpperCase();
				if (curr_tag == null || curr_tag.length() == 0)
					return;
				for (int i = 0; i < TagsDialog.this.curr_taglist_model.getSize(); i++)
					if (curr_tag.equals(TagsDialog.this.curr_taglist_model.getElementAt(i)))
						return;
				TagsDialog.this.curr_taglist_model.addElement(curr_tag);
			}
		});
		jp2.add(add_tag);
		final JButton remove_tag = new JButton("Remove Tag");
		remove_tag.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				int idx;
				if ((idx = TagsDialog.this.modify_tags_list.getSelectedIndex()) != -1)
					mds.jtraverser.dialogs.TagsDialog.this.curr_taglist_model.removeElementAt(idx);
			}
		});
		jp2.add(remove_tag);
		final JPanel jp4 = new JPanel();
		jp4.add(jp2);
		jp1.add(jp4, BorderLayout.EAST);
		this.curr_tag_selection = new JTextField(30);
		final JPanel jp5 = new JPanel();
		jp5.add(new JLabel("Current Selection: "));
		jp5.add(this.curr_tag_selection);
		jp1.add(jp5, BorderLayout.SOUTH);
		jp.add(jp1, BorderLayout.CENTER);
		final JPanel jp3 = new JPanel();
		final JButton ok_b = new JButton("Ok");
		ok_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				TagsDialog.this.ok();
			}
		});
		jp3.add(ok_b);
		final JButton reset_b = new JButton("Reset");
		reset_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				TagsDialog.this.curr_taglist_model.clear();
				for (final String tag : TagsDialog.this.tags)
					TagsDialog.this.curr_taglist_model.addElement(tag);
				TagsDialog.this.modify_tags_list.setModel(TagsDialog.this.curr_taglist_model);
			}
		});
		jp3.add(reset_b);
		final JButton cancel_b = new JButton("Cancel");
		cancel_b.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(final ActionEvent e)
			{
				TagsDialog.this.dispose();
			}
		});
		jp3.add(cancel_b);
		jp.add(jp3, BorderLayout.SOUTH);
		this.getContentPane().add(jp);
		this.addKeyListener(new KeyAdapter()
		{
			@Override
			public void keyTyped(final KeyEvent e)
			{
				if (e.getKeyCode() == KeyEvent.VK_ENTER)
					TagsDialog.this.ok();
			}
		});
		if (node == null)
			return;
		this.currnode = node;
		String[] new_tags;
		try
		{
			new_tags = this.currnode.getTags();
		}
		catch (final Exception exc)
		{
			MdsException.stderr("Error getting tags", exc);
			new_tags = new String[0];
		}
		this.curr_taglist_model.clear();
		for (final String tag : new_tags)
			this.curr_taglist_model.addElement(tag);
		this.setTitle("Modify tags of " + this.currnode.getFullPath());
		this.modify_tags_list.setModel(this.curr_taglist_model);
		this.curr_tag_selection.setText("");
		this.pack();
	}

	public final void ok()
	{
		final String[] out_tags = new String[this.curr_taglist_model.getSize()];
		for (int i = 0; i < this.curr_taglist_model.getSize(); i++)
			out_tags[i] = this.curr_taglist_model.getElementAt(i);
		try
		{
			this.currnode.setTags(out_tags);
		}
		catch (final Exception exc)
		{
			JOptionPane.showMessageDialog(JOptionPane.getRootFrame(), exc.getMessage(), "Error adding tags",
					JOptionPane.WARNING_MESSAGE);
		}
		this.currnode = null;
		this.dispose();
	}
}
