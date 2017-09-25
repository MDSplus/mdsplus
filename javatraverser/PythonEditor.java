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
import java.util.*;


public class PythonEditor extends JPanel implements Editor{
    String retVar;
    String program;
    int rows = 7, columns = 20;
    JTextArea text_area;
    JTextField text_field;
    boolean default_scroll;
    boolean editable = true;

    static final int OPC_FUN = 162;
    
    public PythonEditor(Data []dataArgs)
    {
  	JScrollPane scroll_pane;
 	if(rows > 1)
	    default_scroll = true;
	if(dataArgs != null)
        {
	    getProgram(dataArgs);
        }
        else
        {
            program = "";
            retVar = "";
        }
            
	text_area = new JTextArea(rows,columns);
	text_area.setText(program);
        text_field = new JTextField(10);
        text_field.setText(retVar);
        Dimension d = text_area.getPreferredSize();
	d.height += 20;
	d.width += 20;
        JPanel jp = new JPanel();
        jp.setLayout(new BorderLayout());
        JPanel jp1 = new JPanel();
        jp1.setLayout(new BorderLayout());
        jp1.setBorder(BorderFactory.createTitledBorder("Return Variable"));
        jp1.add(text_field);
        jp.add(jp1, "North");
        JPanel jp2 = new JPanel();
        jp2.setLayout(new BorderLayout());
        jp2.setBorder(BorderFactory.createTitledBorder("Program"));
        scroll_pane = new JScrollPane(text_area); 
	scroll_pane.setPreferredSize(d);
        jp2.add(scroll_pane);
        jp.add(jp2, "Center");
	setLayout(new BorderLayout());
	add(jp, "Center");
    }
    public void reset()
    {
	text_area.setText(program);
        text_field = new JTextField(retVar);
    }
    void getProgram(Data []dataArgs)
    {
        if(dataArgs.length <= 3)
            retVar = "";
        else
        {
            try {
                retVar = dataArgs[3].getString();
            }catch(Exception exc){retVar = "";}
        }
        String [] lines;
        try {
            if(dataArgs[2] instanceof StringArray)
                lines = dataArgs[2].getStringArray();
            else
            {
                lines = new String[1];
                lines[0] = dataArgs[1].getString();
            }
            program = "";
            for(int i = 0; i < lines.length; i++)
            {
                program += lines[i] + "\n";
            }
        } catch(Exception exc){program = "";}
    }
    public Data getData()
    {
        String programTxt = text_area.getText();
        if(programTxt == null || programTxt.equals("")) 
            return null;
        StringTokenizer st = new StringTokenizer(programTxt, "\n");
        String [] lines = new String[st.countTokens()];
        int idx = 0;
        int maxLen = 0;
        while(st.hasMoreTokens())
        {
            lines[idx] = st.nextToken();
            if(maxLen < lines[idx].length())
                maxLen = lines[idx].length(); 
            idx++;
        }
        for(int i = 0; i < lines.length; i++)
        {
            int len = lines[i].length();
            for(int j = 0; j < maxLen - len; j++)
                lines[i] += " ";
        }
        StringArray stArr = new StringArray(lines);
        String retVarTxt = text_field.getText();
        FunctionData retData;
        Data retArgs[];
        if(retVarTxt == null || retVarTxt.equals(""))
        {
            retArgs = new Data[3];
            retArgs[0] = null;
            retArgs[1] = new StringData("Py");
            retArgs[2] = stArr;
        }
        else
        {
            retArgs = new Data[4];
            retArgs[0] = null;
            retArgs[1] = new StringData("Py");
            retArgs[2] = stArr;
            retArgs[3] = new StringData(retVarTxt);
        }
        return new FunctionData(OPC_FUN, retArgs);
    }
    
    public void setEditable(boolean editable)
    {
	this.editable = editable;
	if(text_area != null) text_area.setEditable(editable);
	if(text_field != null) text_field.setEditable(editable);
    }	
    

}



