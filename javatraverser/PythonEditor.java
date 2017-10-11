//package jTraverser;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.util.StringTokenizer;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

public class PythonEditor extends JPanel implements Editor {
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



