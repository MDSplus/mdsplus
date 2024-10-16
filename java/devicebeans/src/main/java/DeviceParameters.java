import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class DeviceParameters extends DeviceComponent
{
	
	private JScrollPane scrollP; 
	private int parameterOffset;
	private int numParameters;
	private JTextField valuesTF[];
	private JLabel labels[];
	public DeviceParameters()
	{
 	}
 	@Override
	protected void initializeData(String data, boolean is_on)
        {
            mode = STRUCTURE;
            valuesTF = new JTextField[numParameters];
            labels = new JLabel[numParameters];
            JPanel jp = new JPanel();
            jp.setLayout(new GridLayout(numParameters, 1));
            int currNid = baseNid + offsetNid + 2 + 3 * parameterOffset;
            int i = 0;
            while(i < numParameters)
            {
                int numChildren = 0;
                try {
                    numChildren = subtree.getInt("GETNCI("+subtree.getFullPath(currNid+1)+",\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc){System.out.println("Internal error in DeviceParameters.initialize)");}
                if(numChildren > 0)
                {
                    currNid += 3;
                    continue;
                }
                JPanel jp1 = new JPanel();
                String parName = "";
                try {
                    parName = subtree.getString(subtree.getDataExpr(currNid));
                }catch(Exception exc){System.out.println("Cannot read parameter name");}
 
                jp1.setBorder(new TitledBorder(parName));
                jp1.setLayout(new BorderLayout());
                jp1.add(valuesTF[i] = new JTextField(), "Center");
                jp.add(jp1);
                currNid += 3;
                i++;
            }
            scrollP = new JScrollPane(jp);
            setLayout(new BorderLayout());
            add(scrollP, "Center");
            displayData(data, is_on);
 
        }
	protected void displayData(String data, boolean is_on)
	{
            int currNid = baseNid + offsetNid + 3 + 3 * parameterOffset;
            int parIdx = 0;
            while(parIdx < numParameters)
            {
                int numChildren = 0;
                try {
                    numChildren = subtree.getInt("GETNCI("+subtree.getFullPath(currNid)+",\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc){System.out.println("Internal error in DeviceParameters.initialize)");}
                if(numChildren > 0)
                {
                    currNid += 3;
                    continue;
                }
                try {
                    valuesTF[parIdx].setText(subtree.getDataExpr(currNid));
                }catch(Exception exc){System.out.println("Cannot read parameter value");}
                currNid += 3;
                parIdx++;
            }
	}

	protected String getData() {return null;}
 	protected  boolean getState(){return true;}
      
        public void apply() throws Exception
        {
            int currNid = baseNid + offsetNid + 3 + 3 * parameterOffset;
            int parIdx = 0;
            while(parIdx < numParameters)
            {
                int numChildren = 0;
                try {
                    numChildren = subtree.getInt("GETNCI("+subtree.getFullPath(currNid)+",\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc){System.out.println("Internal error in DeviceParameters.initialize)");}
                if(numChildren > 0)
                {
                    currNid += 3;
                    continue;
                }
                try {
                    subtree.putDataExpr(currNid, valuesTF[parIdx].getText());
                }catch(Exception exc){System.out.println("Cannot write parameter "+labels[parIdx]+": "+exc);}
                currNid += 3;
                parIdx++;
            }
       }
       public int getNumParameters() {return numParameters;}
       public void setNumParameters(int numParameters) {this.numParameters = numParameters;}
       public int getParameterOffset() {return parameterOffset;}
       public void setParameterOffset(int parameterOffset) {this.parameterOffset = parameterOffset;}
}
