import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class DeviceInputs extends DeviceComponent
{
	
	private JScrollPane scrollP; 
	private int numInputs;
	private JTextField valuesTF[], fieldsTF[][], parametersTF[][];
	private JLabel labels[];
        int numParameters[], numFields[];
	public DeviceInputs()
	{
 	}
 	@Override
	protected void initializeData(String data, boolean is_on)
        {
            mode = STRUCTURE;
            int currNid = baseNid;
            try {
              currNid = subtree.getInt("GETNCI("+subtree.getFullPath(baseNid)+".INPUTS, \'NID_NUMBER\')");
            }catch(Exception exc){numInputs = 0;}
            offsetNid = currNid - baseNid;
            //int currNid = baseNid + offsetNid;
            try {
                numInputs = subtree.getInt("GETNCI("+subtree.getFullPath(currNid)+",\'NUMBER_OF_CHILDREN\')");
            }catch(Exception exc){numInputs = 0;}
            valuesTF = new JTextField[numInputs];
            labels = new JLabel[numInputs];
            fieldsTF = new JTextField[numInputs][];
            numParameters = new int[numInputs];
            numFields = new int[numInputs];
            parametersTF = new JTextField[numInputs][];
            JPanel jp = new JPanel();
            jp.setLayout(new GridLayout(numInputs, 1));
            int currInputNid = currNid + 1;
            for(int i = 0; i < numInputs; i++)
            {   
                int numInputChildren = 0;
                try {
                    int children  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_CHILDREN\')");
                    int members  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_MEMBERS\')");
                    numInputChildren = children + members;
                }catch(Exception exc)
                {
                    System.out.println("Error getting number of input children");
                }
                try {
                    numParameters[i] = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid+6)+",\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc)
                {
                    numParameters[i] = 0;
                }
                if(numInputChildren == 6) //Old version without fields
                    numFields[i] = 0;
                else
                {
                    try {
                        numFields[i] = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid+7+numParameters[i])+",\'NUMBER_OF_CHILDREN\')");
                    }catch(Exception exc)
                    {
                        numFields[i] = 0;
                    }
                }
                fieldsTF[i] = new JTextField[numFields[i]];
                parametersTF[i] = new JTextField[numParameters[i]];
                String inputName;
                try {
                    inputName = subtree.getString(subtree.getDataExpr(currInputNid + 5));
                }catch(Exception exc)
                {
                    inputName = "";
                }
                JPanel jp1 = new JPanel();
                TitledBorder titledBorder = new TitledBorder(inputName);
                //titledBorder.setTitleColor(Color.red);
                jp1.setBorder(titledBorder);
                
                jp1.setLayout(new GridLayout(1 + numFields[i],1));
                JPanel jp2 = new JPanel();
               // jp2.setLayout(new BorderLayout());
                //jp2.add(valuesTF[i] = new JTextField(), "Center");
                jp2.add(valuesTF[i] = new JTextField(30));
                for(int parIdx = 0; parIdx < numParameters[i]; parIdx++)
                {
                    java.lang.String parName = "";
                    try {
                        parName = subtree.getString(subtree.getDataExpr(currInputNid + 8 + 3 * parIdx));
                    }catch(Exception exc)
                    {
                        System.out.println("Cannot read parameter name");
                    }
                    jp2.add(new JLabel(parName+":"));
                    jp2.add(parametersTF[i][parIdx] = new JTextField(10));
                }
                jp1.add(jp2);
                for (int fieldIdx = 0; fieldIdx < numFields[i]; fieldIdx++)
                {
                    String fieldName;
                    try {
                        fieldName = subtree.getString(subtree.getDataExpr(currInputNid + 8 + 3 * numParameters[i] + 6 * fieldIdx +  4));
                    }catch(Exception exc)
                    {
                        fieldName = "";
                    }
                    jp2 = new JPanel();
                    jp2.setLayout(new BorderLayout());
                    jp2.setBorder(new TitledBorder(inputName+"."+fieldName));
                    jp2.add(fieldsTF[i][fieldIdx] = new JTextField(), "Center");
                    jp1.add(jp2);
                }
                currInputNid += numInputChildren + 1 + 3 * numParameters[i] + 6 * numFields[i];
               jp.add(jp1);
           }
            scrollP = new JScrollPane(jp);
            setLayout(new BorderLayout());
            add(scrollP, "Center");
            displayData(data, is_on);
        }
	protected void displayData(String data, boolean is_on)
	{
            int currInputNid = baseNid + offsetNid + 1;
            for(int inputIdx = 0; inputIdx < numInputs; inputIdx++)
            {
                int numInputChildren = 0;
                try {
                    int children  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_CHILDREN\')");
                    int members  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_MEMBERS\')");
                    numInputChildren = children + members;
                }catch(Exception exc)
                {
                    System.out.println("Error getting number of input children");
                }
                try {
                     valuesTF[inputIdx].setText(subtree.getDataExpr(currInputNid + 4));
                }catch(Exception exc)
                {
                    valuesTF[inputIdx].setText("");
                }
                for(int parIdx = 0; parIdx < numParameters[inputIdx]; parIdx++)
                {
                    try {
                        parametersTF[inputIdx][parIdx].setText(subtree.getDataExpr(currInputNid + 9 + 3 * parIdx));
                    }catch(Exception exc)
                    {
                        parametersTF[inputIdx][parIdx].setText("");
                    }
                }
                for(int fieldIdx = 0; fieldIdx < numFields[inputIdx]; fieldIdx++)
                {
                    try {
                         fieldsTF[inputIdx][fieldIdx].setText(subtree.getDataExpr(currInputNid + 8 + 3 * numParameters[inputIdx] + 6 * fieldIdx +  5));
                    }catch(Exception exc){fieldsTF[inputIdx][fieldIdx].setText("");}
               }
               currInputNid += numInputChildren + 1 + 3 * numParameters[inputIdx] + 6 * numFields[inputIdx]; 
            }
	}

	protected String getData() {return null;}
 	protected  boolean getState(){return true;}
      
        public void apply() throws Exception
        {
            int currInputNid = baseNid + offsetNid + 1;
            for(int inputIdx = 0; inputIdx < numInputs; inputIdx++)
            {
                int numInputChildren = 0;
                try {
                    int children  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_CHILDREN\')");
                    int members  = subtree.getInt("GETNCI("+subtree.getFullPath(currInputNid)+",\'NUMBER_OF_MEMBERS\')");
                    numInputChildren = children + members;
                }catch(Exception exc)
                {
                    System.out.println("Error getting number of input children");
                }
                try {
                     subtree.putDataExpr(currInputNid + 4, valuesTF[inputIdx].getText());
                }catch(Exception exc)
                {
                    JOptionPane.showMessageDialog(null, ""+exc, "Error in input field "+inputIdx,  JOptionPane.WARNING_MESSAGE);
                }
                for(int parIdx = 0; parIdx < numParameters[inputIdx]; parIdx++)
                {
                    try {
                        subtree.putDataExpr(currInputNid + 9 + 3 * parIdx, parametersTF[inputIdx][parIdx].getText());
                     }catch(Exception exc)
                    {
                        JOptionPane.showMessageDialog(null, ""+exc, "Error in paremeter field "+inputIdx,  JOptionPane.WARNING_MESSAGE);
                    }
                }
                for(int fieldIdx = 0; fieldIdx < numFields[inputIdx]; fieldIdx++)
                {
                    try {
                       subtree.putDataExpr(currInputNid + 8 + 3 * numParameters[inputIdx] + 6 * fieldIdx +  5, fieldsTF[inputIdx][fieldIdx].getText());
                    }catch(Exception exc)
                    {
                        JOptionPane.showMessageDialog(null, ""+exc, "Error in subfield of input field "+inputIdx,  JOptionPane.WARNING_MESSAGE);
                    }
             
                }
                currInputNid += numInputChildren + 1 + 3 * numParameters[inputIdx] + 6 * numFields[inputIdx]; 
            }
       }
}  
