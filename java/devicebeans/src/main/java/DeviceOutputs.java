import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class DeviceOutputs extends DeviceComponent
{
	
	private JScrollPane scrollP; 
	private int numOutputs;
	private JTextField segLensTF[], streamNamesTF[];
        private int segLenNids[], streamNameNids[];
        private int numOutputChildren = 0;
        private int numItems;
	public DeviceOutputs()
	{
 	}
 	@Override
	protected void initializeData(String data, boolean is_on)
        {
            mode = STRUCTURE;
            int currNid = baseNid;
            try {
              currNid = subtree.getInt("GETNCI("+subtree.getFullPath(baseNid)+".OUTPUTS, \'NID_NUMBER\')");
            }catch(Exception exc){numOutputs = 0;}
            //int currNid = baseNid + offsetNid;
            try {
                numOutputs = subtree.getInt("GETNCI("+subtree.getFullPath(currNid)+",\'NUMBER_OF_CHILDREN\')");
            }catch(Exception exc){numOutputs = 0;}
            int currOutNid = currNid + 7;
            //Count number of actual outputs (i.e. for which nSamomes and streamName make sense
            numItems = 0;
            for(int outIdx = 0; outIdx < numOutputs; outIdx++)
            {
                int numChildren, numMembers, numPars, numFields;
                try {
                    numChildren = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+",\'NUMBER_OF_CHILDREN\')");
                    numMembers = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+",\'NUMBER_OF_MEMBERS\')");
                    numPars = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+":PARAMETERS,\'NUMBER_OF_CHILDREN\')"); 
                }catch(Exception exc)
                {
                    System.out.println("Error getting Output fields");
                    numOutputs = 0;
                    break;
                }
                try{
                     numFields = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+":FIELDS,\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc)
                {
                    numFields = 0; //node FIELDS missing
                }
                if(numFields == 0)
                    numItems += 1; //no structure
                else
                    numItems += numFields;
                currOutNid += 1 + numChildren + numMembers + 2 * numPars + 8 * numFields;
            }
            segLensTF = new JTextField[numItems];
            streamNamesTF = new JTextField[numItems];
            segLenNids = new int[numItems];
            streamNameNids = new int[numItems];
            JPanel jp = new JPanel();
            jp.setLayout(new GridLayout(numItems, 1));
            currOutNid = currNid + 7;
            int currItem = 0;
            for(int outIdx = 0; outIdx < numOutputs; outIdx++)
            {
                int numChildren, numMembers, numPars, numFields;
                try {
                    numChildren = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+",\'NUMBER_OF_CHILDREN\')");
                    numMembers = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+",\'NUMBER_OF_MEMBERS\')");
                    numPars = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+":PARAMETERS,\'NUMBER_OF_CHILDREN\')"); 
                }catch(Exception exc)
                {
                    System.out.println("Error getting Output fields");
                    numOutputs = 0;
                    break;
                }
                try{
                     numFields = subtree.getInt("GETNCI("+subtree.getFullPath(currOutNid)+":FIELDS,\'NUMBER_OF_CHILDREN\')");
                }catch(Exception exc)
                {
                    numFields = 0; //node FIELDS missing
                }
                String outName = "";
                try {
                   outName = subtree.getString(subtree.getDataExpr(currOutNid + 1));
                }catch(Exception exc){}
                if(numFields == 0)
                {
                    JPanel jp1 = new JPanel();
                    jp1.setBorder(new TitledBorder(outName));
                    jp1.setLayout(new GridLayout(1,2));
                    jp1.add(new JLabel("Segment len.: "));
                    jp1.add(segLensTF[currItem] = new JTextField(10));
                    jp1.add(new JLabel("Stream name: "));
                    jp1.add(streamNamesTF[currItem] = new JTextField(10));
                    jp.add(jp1);
                    segLenNids[currItem] = currOutNid + 5;
                    streamNameNids[currItem] = currOutNid + 6;
                    currItem++;
                }
                else
                {
                    for(int fieldIdx = 0; fieldIdx < numFields; fieldIdx++)
                    {
                        int fieldNid = currOutNid + numChildren +numMembers +1 + 2 * numPars + 8 * fieldIdx;
                        segLenNids[currItem] = fieldNid + 5;
                        streamNameNids[currItem] = fieldNid + 6;
                        String fieldName = "";
                        try {
                           fieldName = subtree.getString(subtree.getDataExpr(fieldNid + 1));
                        }catch(Exception exc){}
                        JPanel jp1 = new JPanel();
                        jp1.setBorder(new TitledBorder(outName+'.'+fieldName));
                        jp1.setLayout(new GridLayout(1,2));
                        jp1.add(new JLabel("Segment len.: "));
                        jp1.add(segLensTF[currItem] = new JTextField(10));
                        jp1.add(new JLabel("Stream name: "));
                        jp1.add(streamNamesTF[currItem] = new JTextField(10));
                        jp.add(jp1);
                        currItem++;
                    }
                }
                currOutNid += 1 + numChildren + numMembers + 2 * numPars + 8 * numFields;
 
            }
            scrollP = new JScrollPane(jp);
            setLayout(new BorderLayout());
            add(scrollP, "Center");
            displayData(data, is_on);
        }
	protected void displayData(String data, boolean is_on)
	{
            for(int idx = 0; idx < numItems; idx++)
            {
                try {
                     segLensTF[idx].setText(subtree.getDataExpr(segLenNids[idx]));
                }catch(Exception exc)
                {
                    segLensTF[idx].setText("");
                }
                try {
                     streamNamesTF[idx].setText(subtree.getString(subtree.getDataExpr(streamNameNids[idx])));
                }catch(Exception exc)
                {
                    streamNamesTF[idx].setText("");
                }
            }
	}

	protected String getData() {return null;}
 	protected  boolean getState(){return true;}
      
        public void apply() throws Exception
        {
            for(int idx = 0; idx < numItems; idx++)
            {
                try {
                    subtree.putDataExpr(segLenNids[idx], segLensTF[idx].getText());
                }catch(Exception exc)
                {}
                try {
                    String name = streamNamesTF[idx].getText();
                    subtree.putDataExpr(streamNameNids[idx], "\""+streamNamesTF[idx].getText()+"\"");
                }catch(Exception exc)
                {}
           }
        }
 
 }  
