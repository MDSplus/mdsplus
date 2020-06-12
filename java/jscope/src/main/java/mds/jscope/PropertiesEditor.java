package mds.jscope;

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileWriter;
import java.io.IOException;

import javax.swing.*;

public class PropertiesEditor extends JDialog
{
	private static final long serialVersionUID = 1L;
	JEditorPane text;
	String prFile;

	public PropertiesEditor(JFrame owner, String propetiesFile)
	{
		super(owner);
		this.setTitle("jScope properties file editor : " + propetiesFile);
		prFile = propetiesFile;
		text = new JEditorPane();
		text.setEditable(true);
		try
		{
			text.setPage("file:" + propetiesFile);
		}
		catch (final IOException exc)
		{}
		final JScrollPane scroller = new JScrollPane();
		final JViewport vp = scroller.getViewport();
		vp.add(text);
		getContentPane().add(scroller, BorderLayout.CENTER);
		final JPanel p = new JPanel();
		final JButton save = new JButton("Save");
		save.setSelected(true);
		p.add(save);
		save.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				try
				{
					text.write(new FileWriter(prFile));
					JOptionPane.showMessageDialog(PropertiesEditor.this,
							"The changes will take effect the next time you restart jScope.", "Info",
							JOptionPane.WARNING_MESSAGE);
				}
				catch (final IOException exc)
				{
					exc.printStackTrace();
				}
			}
		});
		final JButton close = new JButton("Close");
		close.setSelected(true);
		p.add(close);
		close.addActionListener(new ActionListener()
		{
			@Override
			public void actionPerformed(ActionEvent e)
			{
				setVisible(false);
				dispose();
			}
		});
		getContentPane().add(p, BorderLayout.SOUTH);
		pack();
		setSize(680, 700);
	}
}
