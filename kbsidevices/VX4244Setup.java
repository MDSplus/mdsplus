/*
		A basic implementation of the DeviceSetup class.
	        Korea Basic Science Institute(KBSI)
	        Version 1.0
*/
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.JCheckBox;
import java.awt.Frame.*;
import javax.swing.border.*;
import javax.swing.event.*;



public class VX4244Setup extends DeviceSetup
{
	static final int NID_ADDRESS = 1;
	static final int NID_SAMPLE_PERIOD = 5;
	static final int NID_SAMPLE_DELAY = 6;
	static final int NID_SAMPLE_NUM = 8;
	static final int NID_TRIG_SOURCE = 11;
	static final int NID_INPUT_RANGE = 19;



	public VX4244Setup(Frame parent)
	{
	    super(parent);
	    int i;
	    int j;
	    JPanel jPanel;
	    JPanel jTrigPanel;
	    JPanel AddressPanel;
	    JLabel rangetext;

	    Container contentPane;
	    JTabbedPane jTabbedPane;  //JTabbedPaneDemo Start

	    setTitle("Tektronix,AI 16-CH, 200 kS/s Simultaneous Digitizer VX4244");
	    contentPane = getContentPane();
	    contentPane.setLayout(new GridBagLayout());
	    GridBagConstraints c = new GridBagConstraints();

	    AddressPanel = new JPanel();
	    address.setNumCols(15);
	    address.setTextOnly(true);
	    address.setLabelString("Address:");
	    address.setOffsetNid(NID_ADDRESS);
	    AddressPanel.add(address);
	    c.gridy =0;
	    contentPane.add(AddressPanel,c);
	    jTabbedPane = new JTabbedPane();

	    //Trigger Source Setting Part
	    for (j=0; j<4; j++)
	    {
	      trigbits = new DeviceBits(13);
	      jTrigPanel = new JPanel();
	      trigbits.setLayout(new GridLayout(10,2));
	      jTrigPanel.setBorder(new TitledBorder("Trig Group"+(j+1)));

	      String[] trigString = new String[13];
	     trigString[0]= "  External";
	      trigString[1]= "Immediate";
	      trigString[2]= "Threshold";
	      trigString[3]= "    VXIcmd";
	      trigString[4]= "    TTL 0    ";
	      trigString[5]= "    TTL 1    ";
	      trigString[6]= "    TTL 2    ";
	      trigString[7]= "    TTL 3    ";
	      trigString[8]= "    TTL 4    ";
	      trigString[9]= "    TTL 5    ";
	      trigString[10]= "    TTL 6    ";
	      trigString[11]= "    TTL 7    ";
	      trigString[12]= "Logic AND-0 OR-1";
	       trigbits.setLabelString("Trig Source");
	      trigbits.setCheckString(trigString);
	      trigbits.setOffsetNid(NID_TRIG_SOURCE+(39*j));
	      jTrigPanel.add(trigbits);
	      jTabbedPane.addTab( "Trig Group #"+(j+1), jTrigPanel);
	    }

	       for (j=0; j<4; j++)
	       {   // Group 1 = 1-4 channel setting
	           jPanel = new JPanel();
	           jPanel.setLayout(new GridLayout(10,1));
	           jPanel.setBorder(new TitledBorder("Group"+(j+1)+", #Sample Max : 262,144/Group" ));
	           rangetext = new JLabel();
	           rangetext.setText("                                     Range");
	           jPanel.add(rangetext);

	           String[] tempString = new String[8];
	           tempString[0] = "OFF";
	           tempString[1] = ".20 V";
	           tempString[2] = ".50 V";
	           tempString[3] = "1.00 V";
	           tempString[4] = "2.00 V";
	           tempString[5] = "5.00 V";
	           tempString[6] = "10.00 V";
	           tempString[7] = "20.00 V";
	           int range[] = {7, 0, 1, 2, 3, 4, 5, 6};

	           for (i=0; i<4; i++)
	           {
	              ch[i] = new DeviceChoice();
	              ch[i].setChoiceItems(tempString);
	              ch[i].setChoiceIntValues(range);
	              ch[i].setLabelString("AI " + ((i+1)+(4*j)));
	              ch[i].setOffsetNid(NID_INPUT_RANGE + (6*i+39*j));
	              jPanel.add(ch[i]);
	           }
	           //sample length setting part
	           sampleNum = new DeviceField();
	           sampleNum.setLabelString("Samples length:");
	           sampleNum.setNumCols(4);
	           sampleNum.setOffsetNid(NID_SAMPLE_NUM +(39*j));
	           jPanel.add(sampleNum);
	           // Sample Rate Setting Part
	           String[] samString = new String[10];
	           samString[0] = "200000";
	           samString[1] = "125000";
	           samString[2] = "100000";
	           samString[3] = "50000";
	           samString[4] = "40000";
	           samString[5] = "25000";
	           samString[6] = "20000";
	           samString[7] = "10000";
	           samString[8] = "5000";
	           samString[9] = "1000";
	           double period[] = {200000., 125000., 100000., 50000., 40000., 25000., 20000., 10000., 5000., 1000.};
	           samplePeriod = new DeviceChoice();
	           samplePeriod.setChoiceItems(samString);
	           samplePeriod.setChoiceDoubleValues(period);
	           samplePeriod.setLabelString("Period(S.R):");
	           samplePeriod.setOffsetNid(NID_SAMPLE_PERIOD+(39*j));
	           jPanel.add(samplePeriod);

	           //Delay Time(m/s) Setting Part
	           sampleDelay = new DeviceField();
	           sampleDelay.setNumCols(4);
	           sampleDelay.setOffsetNid(NID_SAMPLE_DELAY+(39*j));
	           sampleDelay.setLabelString("Delay m/s");
	           jPanel.add(sampleDelay);
	           jTabbedPane.addTab( "Input Group #"+(j+1), jPanel);
	        }
	        c.gridy=1;
	        contentPane.add(jTabbedPane, c);

	        c.gridy=2;
	        contentPane.add(dispatch, c);
	       {
	              String[] tempString = new String[3];
	              tempString[0] = "INIT";
	              tempString[1] = "ARM";
	              tempString[2] = "STORE";
	              deviceButtons.setMethods(tempString);
	              c.gridy=3;
	              contentPane.add(deviceButtons,c);
	        }
	      this.setSize(new Dimension(800, 800));
	     }
	     public VX4244Setup()
	     {
	             this((Frame)null);
	     }
	     public VX4244Setup(String sTitle)
	     {
	             this();
	             setTitle(sTitle);
	     }
	     public void setVisible(boolean b)
	     {
	             if (b)
	             setLocation(50, 50);
	             super.setVisible(b);
	     }
	     static public void main(String args[])
	     {
	             (new VX4244Setup()).setVisible(true);
	     }
	     public void addNotify()
	     {
	             // Record the size of the window prior to calling parents addNotify.
	             Dimension size = getSize();
	             super.addNotify();
	             if (frameSizeAdjusted)
	                     return;
	             frameSizeAdjusted = true;
	             // Adjust size of frame according to the insets
	             Insets insets = getInsets();
	             setSize(insets.left + insets.right + size.width, insets.top + insets.bottom + size.height);
	     }
	     // Used by addNotify
	     boolean frameSizeAdjusted = false;
	     //{{DECLARE_CONTROLS
	     DeviceButtons deviceButtons = new DeviceButtons();
	     DeviceDispatch dispatch = new DeviceDispatch();
	     DeviceChoice samplePeriod;
	     DeviceField sampleNum;
	     DeviceField sampleNumPre = new DeviceField();
	     DeviceField sampleDelay;
	     DeviceChoice sampleSource = new DeviceChoice();
	     DeviceChoice trigSlope = new DeviceChoice();
	     DeviceField trigLevel = new DeviceField();
	     DeviceChoice trigSource = new DeviceChoice();
	     DeviceField ttlOut = new DeviceField();
	     DeviceChoice ch[] = new DeviceChoice[5];
	     DeviceChoice port[] = new DeviceChoice[2];
	     DeviceField address = new DeviceField();
	     DeviceBits trigbits;

	     //}}
     }
