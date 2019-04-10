package jScope;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;


public class PropertiesEditor extends JDialog
{
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
	    text.setPage("file:"+propetiesFile);
	}
	catch(IOException exc){}


	JScrollPane scroller = new JScrollPane();
	JViewport vp = scroller.getViewport();
	vp.add(text);
	getContentPane().add(scroller, BorderLayout.CENTER);


	JPanel p = new JPanel();
	JButton save = new JButton("Save");
	save.setSelected(true);
	p.add(save);

	save.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            try
	            {
	                text.write(new FileWriter(prFile));
			        JOptionPane.showMessageDialog(PropertiesEditor.this,
			                                  "The changes will take effect the next time you restart jScope.",
			                                  "Info", JOptionPane.WARNING_MESSAGE);
	            }
	            catch (IOException exc){exc.printStackTrace();};
	        }
	    });

	JButton close = new JButton("Close");
	close.setSelected(true);
	p.add(close);
	close.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
	        {
	            setVisible(false);
	            dispose();
	        }
	    });


	getContentPane().add(p, BorderLayout.SOUTH);
	pack();
	setSize(680,700);

    }
}