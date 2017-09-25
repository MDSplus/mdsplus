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

/* $Id$ */
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import javax.swing.*;
import javax.swing.table.*;


public class SignalsBoxDialog extends JDialog
{  
    jScopeFacade scope;
    JTable table;
    TableModel dataModel;
            
    SignalsBoxDialog(JFrame f, String title, boolean modal)
    {
        super(f, title, false);
        scope = (jScopeFacade)f;
        dataModel = new AbstractTableModel() 
        {
            public int getColumnCount() { return 2; }
            public int getRowCount() { return WaveInterface.sig_box.signals_name.size();}
            public Object getValueAt(int row, int col) 
            { 
                switch(col)
                {
                    case 0 : return WaveInterface.sig_box.getYexpr(row);
                    case 1 : return WaveInterface.sig_box.getXexpr(row);
                }
                return null;
            }
            public String getColumnName(int col) 
            { 
                switch(col)
                {
                    case 0 : return "Y expression";
                    case 1 : return "X expression";
                }
                return null;
            }
        };

        table = new JTable(dataModel);
        JScrollPane scrollpane = new JScrollPane(table);
        getContentPane().add("Center", scrollpane);
                
        
        JPanel p = new JPanel();
        p.setLayout(new FlowLayout(FlowLayout.CENTER));
    			
	    JButton add = new JButton("Add");
	    add.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
    	        int idx[] = table.getSelectedRows();
    	        String x_expr[] = new String[idx.length];
    	        String y_expr[] = new String[idx.length];
    	        
    	        for(int i = 0; i < idx.length; i++)
    	        {
    	            y_expr[i] = (String)table.getValueAt(idx[i], 0);
    	            x_expr[i] = (String)table.getValueAt(idx[i], 1);
    	        }
    	        scope.wave_panel.AddSignals(null, null, x_expr, y_expr, true, false);
            }
        });
        p.add(add);
/*  
  	    JButton remove = new JButton("Remove");
	    remove.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
    	        int idx[] = table.getSelectedRows();
                table.clearSelection();
    	        
    	        for(int i = 0; i < idx.length; i++)
    	        {
                    WaveInterface.sig_box.removeExpr(idx[i]-i);
    	        }
    	        table.updateUI();
            }
        });
        p.add(remove);
*/
        
        
        JButton cancel = new JButton("Cancel");
	    cancel.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
    	        dispose();	
            }
        });

        p.add(cancel);

        getContentPane().add("South",p);
              
	    pack();	 
	    setLocationRelativeTo(f);
    }
    
}
