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