/*
		A basic implementation of the DeviceSetup class.
	        Korea Basic Science Institute(KBSI)
	        Version 3.0
	        Channel Label Tree Node Add & Modify
*/

import java.awt.*;
import java.lang.String;
import javax.swing.*;
import javax.swing.border.*;

public class NI5122Setup extends DeviceSetup
{
	static final int NID_ADDRESS = 1;
	static final int NID_CHANNELS = 3;
	static final int NID_SAMPLE_RATE = 5;
	static final int NID_SAMPLE_NUM = 6;
	static final int NID_TRIG_DELAY = 8;
	static final int NID_TRIG_SOURCE = 9;
	static final int NID_TRIG_LEVEL = 10;
	static final int NID_TRIG_SLOPE = 11;
	static final int NID_TRIG_COUPLING = 12;
	static final int NID_INPUT_IMPEDANCE = 16;
	static final int NID_INPUT_RANGE = 17;
	static final int NID_INPUT_RANGE_OFFSET = 18;
	static final int NID_INPUT_COUPLING = 19;
	static final int NID_INPUT_PROBE_ATTENUATION = 20;
	static final int NID_INPUT_LABEL = 22;



	public NI5122Setup(Frame parent)
	{
	  super(parent);
	  int i;
	  JPanel jPanel;
	        setTitle("Nationan Instruments NI PXI-5122 14-Bit 100MS/s Digitizer");
	        setSize(600,600);
	        Container contentPane = getContentPane();
		contentPane.setLayout(new GridBagLayout());
	        GridBagConstraints c = new GridBagConstraints();
//Address
	        jPanel = new JPanel();
	        address.setNumCols(15);
	        address.setTextOnly(true);
	        address.setLabelString("Address:");
	        address.setOffsetNid(NID_ADDRESS);
	        jPanel.add(address);
	        c.gridy = 0;
	        contentPane.add(jPanel, c);
//Channel Select
	        jPanel = new JPanel();
	        String[] channelString = new String[3];
	        channelString[0] ="0 Channel";
	        channelString[1] ="1 Channel";
	        channelString[2] ="0,1 Channel";
	        int chstring[] = {0, 1, 2};
	        channels.setChoiceItems(channelString);
	        channels.setChoiceIntValues(chstring);
	        channels.setLabelString("Channel:");
	        channels.setOffsetNid(NID_CHANNELS);
	        jPanel.add(channels);

	        c.gridy = 1;
	        contentPane.add(jPanel, c);
//Channel Label
	       jPanel = new JPanel();
	       label1.setNumCols(45);
	       label1.setTextOnly(true);
	       label1.setLabelString("Channel Label "+0);
	       label1.setOffsetNid(NID_INPUT_LABEL );
	       jPanel.add(label1);
	       c.gridy = 2;
	       contentPane.add(jPanel, c);

	       jPanel = new JPanel();
	       label2.setNumCols(45);
	       label2.setTextOnly(true);
	       label2.setLabelString("Channel Label "+1);
	       label2.setOffsetNid(NID_INPUT_LABEL + 9);
	       jPanel.add(label2);
	       c.gridy = 3;
	       contentPane.add(jPanel, c);

//Channel Sample Rate and Sample count
	       jPanel = new JPanel();
	       jPanel.setBorder(new TitledBorder("Input Channels 0&1 #Sam_Rate(1.526kS/s ~100MS/s) #Sam_Num(Max:32MB(131072)"));
	       {
	        sampleRate.setNumCols(10);
	        sampleRate.setOffsetNid(NID_SAMPLE_RATE);
	        sampleRate.setLabelString("Sampling Rate(d0):");
	        jPanel.add(sampleRate);
	       }
	       {
	        sampleNum.setNumCols(10);
	        sampleNum.setOffsetNid(NID_SAMPLE_NUM);
	        sampleNum.setLabelString("# Points to Read:");
	        jPanel.add(sampleNum);
	       }
	       c.gridy = 4;
	       contentPane.add(jPanel, c);


//Channel Number 0 setting
	        jPanel = new JPanel();
	        jPanel.setLayout(new GridLayout(3,4));
	        jPanel.setBorder(new TitledBorder("Input Channels 0 "));

	        {
	                String[] rangeString = new String[7];
	                rangeString[0] = ".2 V";
	                rangeString[1] = ".4 V";
	                rangeString[2] = "1.0 V";
	                rangeString[3] = "2.0 V";
	                rangeString[4] = "4.0 V";
	                rangeString[5] = "10.0 V";
	                rangeString[6] = "20.0 V(10MOhm)";
	                double range[] = {.2, .3, 1.0, 2.0, 4.0, 10.0, 20.0};
	                ranges = new DeviceChoice();
	                ranges.setChoiceItems(rangeString);
	                ranges.setChoiceDoubleValues(range);
	                ranges.setLabelString("Vertical Range :");
	                ranges.setOffsetNid(NID_INPUT_RANGE);
	                jPanel.add(ranges);
	         }
	         {
	                 String[] impedanceString = new String[2];
	                 impedanceString[0] = "1_MEG_OHM";
	                 impedanceString[1] = "50_OHMS";
	                 double range[] = {1000000.0, 50.0};
	                 imranges = new DeviceChoice();
	                 imranges.setChoiceItems(impedanceString);
	                 imranges.setChoiceDoubleValues(range);
	                 imranges.setLabelString("Input Impedance :");
	                 imranges.setOffsetNid(NID_INPUT_IMPEDANCE);
	                 jPanel.add(imranges);
	          }

	         {
	          rangeOffset.setNumCols(8);
	          rangeOffset.setOffsetNid(NID_INPUT_RANGE_OFFSET);
	          rangeOffset.setLabelString("Verti_Range_Offset(d0):");
	          jPanel.add(rangeOffset);
	         }
	         {
	           String[] tempString = new String[3];
	           tempString[0] = "INPUT_SIGNAL_AC";
	           tempString[1] = "INPUT_SIGNAL_DC";
	           tempString[2] = "INPUT_SIGNAL_GND";
	           int acdc[] = {0, 1, 2};
	           coupling.setChoiceItems(tempString);
	           coupling.setChoiceIntValues(acdc);
	           coupling.setLabelString("Channel Coupling:");
	           coupling.setOffsetNid(NID_INPUT_COUPLING);
	           jPanel.add(coupling);
	         }
	         {
	          probeAtten.setNumCols(5);
	          probeAtten.setOffsetNid(NID_INPUT_PROBE_ATTENUATION);
	          probeAtten.setLabelString("Channel Probe Attenuation(d0):");
	          jPanel.add(probeAtten);
	         }

	        c.gridy = 6;
//                c.gridheight = 2;
	        contentPane.add(jPanel, c);

//Channel Number 1 setting
	        jPanel = new JPanel();
	        jPanel.setLayout(new GridLayout(3,4));
	        jPanel.setBorder(new TitledBorder("Input Channels 1 "));
	        {
	                String[] rangeString = new String[7];
	                rangeString[0] = ".2 V";
	                rangeString[1] = ".4 V";
	                rangeString[2] = "1.0 V";
	                rangeString[3] = "2.0 V";
	                rangeString[4] = "4.0 V";
	                rangeString[5] = "10.0 V";
	                rangeString[6] = "20.0 V(10MOhm)";
	                double range[] = {.2, .3, 1.0, 2.0, 4.0, 10.0, 20.0};
	                ranges1 = new DeviceChoice();
	                ranges1.setChoiceItems(rangeString);
	                ranges1.setChoiceDoubleValues(range);
	                ranges1.setLabelString("Vertical Range :");
	                ranges1.setOffsetNid(NID_INPUT_RANGE +9);
	                jPanel.add(ranges1);
	         }
	         {
	                 String[] impedanceString = new String[2];
	                 impedanceString[0] = "1_MEG_OHM";
	                 impedanceString[1] = "50_OHMS";
	                 double range[] = {1000000.0, 50.0};
	                 imranges1 = new DeviceChoice();
	                 imranges1.setChoiceItems(impedanceString);
	                 imranges1.setChoiceDoubleValues(range);
	                 imranges1.setLabelString("Input Impedance :");
	                 imranges1.setOffsetNid(NID_INPUT_IMPEDANCE +9);
	                 jPanel.add(imranges1);
	          }

	         {
	          rangeOffset1.setNumCols(8);
	          rangeOffset1.setOffsetNid(NID_INPUT_RANGE_OFFSET +9);
	          rangeOffset1.setLabelString("Verti_Range_Offset(d0):");
	          jPanel.add(rangeOffset1);
	         }
	         {
	           String[] tempString = new String[3];
	           tempString[0] = "INPUT_SIGNAL_AC";
	           tempString[1] = "INPUT_SIGNAL_DC";
	           tempString[2] = "INPUT_SIGNAL_GND";
	           int acdc[] = {0, 1, 2};
	           coupling1.setChoiceItems(tempString);
	           coupling1.setChoiceIntValues(acdc);
	           coupling1.setLabelString("Channel Coupling:");
	           coupling1.setOffsetNid(NID_INPUT_COUPLING +9);
	           jPanel.add(coupling1);
	         }
	         {
	          probeAtten1.setNumCols(5);
	          probeAtten1.setOffsetNid(NID_INPUT_PROBE_ATTENUATION +9);
	          probeAtten1.setLabelString("Channel Probe Attenuation(d0):");
	          jPanel.add(probeAtten1);
	         }

	        c.gridy = 7;
	        contentPane.add(jPanel, c);



// Trigger setting
	jPanel = new JPanel();
	jPanel.setLayout(new GridLayout(3,2));
	jPanel.setBorder(new TitledBorder("Triggering"));
		{
			String[] tempString = new String[4];
			tempString[0] = "Immediate";
			tempString[1] = "Ch 0";
			tempString[2] = "Ch 1";
	                tempString[3] = "External";
			trigSource.setChoiceItems(tempString);
	                trigSource.setConvert(true);
	                trigSource.setLabelString("Trigger Source   :  ");
	                trigSource.setOffsetNid(NID_TRIG_SOURCE);
		}
		jPanel.add(trigSource);
	        /*- Defined values for IVISCOPE_ATTR_TRIGGER_SLOPE -*/
//            #define IVISCOPE_VAL_POSITIVE                                   (1L)
//            #define IVISCOPE_VAL_NEGATIVE                                   (0L)
	        {
	                String[] tempString = new String[2];
	                tempString[0] = " Negative  (Falling) ";
	                tempString[1] = " Positive  (Rising) ";
	                trigSlope.setChoiceItems(tempString);
	                trigSlope.setConvert(true);
	                trigSlope.setLabelString("Trigger Slope  :  ");
	                trigSlope.setOffsetNid(NID_TRIG_SLOPE);
	        }
	        jPanel.add(trigSlope);
	        {
	        trigLevel.setNumCols(8);
	        trigLevel.setLabelString("Trigger Level(0.d0):");
	        trigLevel.setOffsetNid(NID_TRIG_LEVEL);
	        jPanel.add(trigLevel);
	        }
	        /*- Defined values for IVISCOPE_ATTR_VERTICAL_COUPLING -*/
//            #define IVISCOPE_VAL_AC                                         (0L)
//            #define IVISCOPE_VAL_DC                                         (1L)
//            #define IVISCOPE_VAL_GND                                        (2L)
	        {
	          String[] tempString = new String[3];
	          tempString[0] = "NISCOPE_VAL_AC";
	          tempString[1] = "NISCOPE_VAL_DC";
	          tempString[2] = "NISCOPE_VAL_GND";
	          trigCoupling.setChoiceItems(tempString);
	          trigCoupling.setConvert(true);
	          trigCoupling.setLabelString("Trigger Coupling:");
	          trigCoupling.setOffsetNid(NID_TRIG_COUPLING);
	          jPanel.add(trigCoupling);
	        }
	        {
	        trigDelay.setNumCols(8);
	        trigDelay.setLabelString("Trigger Delay(0.d0):");
	        trigDelay.setOffsetNid(NID_TRIG_DELAY);
	        jPanel.add(trigDelay);
	        }
	        c.gridy = 8;
	        contentPane.add(jPanel, c);

	        c.gridy= 9;
	        contentPane.add(dispatch, c);
	        {
	                String[] tempString = new String[3];
	                tempString[0] = "INIT";
	                tempString[1] = "ARM";
	                tempString[2] = "STORE";
	                deviceButtons.setMethods(tempString);
	        }
	        c.gridy = 10;
	        contentPane.add(deviceButtons, c);
	        pack();
		//}}
	}

	public NI5122Setup()
	{
		this((Frame)null);
	}

	public NI5122Setup(String sTitle)
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
		(new NI5122Setup()).setVisible(true);
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
	DeviceField address = new DeviceField();
	DeviceChoice channels = new DeviceChoice();
	DeviceField sampleRate = new DeviceField();
	DeviceField sampleNum = new DeviceField();
	DeviceChoice ranges = new DeviceChoice();
	DeviceChoice ranges1 = new DeviceChoice();
	DeviceChoice imranges = new DeviceChoice();
	DeviceChoice imranges1 = new DeviceChoice();
	DeviceField rangeOffset = new DeviceField();
	DeviceField rangeOffset1 = new DeviceField();
	DeviceChoice coupling = new DeviceChoice();
	DeviceChoice coupling1 = new DeviceChoice();
	DeviceField probeAtten = new DeviceField();
	DeviceField probeAtten1 = new DeviceField();
	DeviceChoice trigSource = new DeviceChoice();
	DeviceField trigLevel = new DeviceField();
	DeviceChoice trigSlope = new DeviceChoice();
	DeviceChoice trigCoupling = new DeviceChoice();
	DeviceField trigDelay = new DeviceField();
	DeviceField label1 = new DeviceField();
	DeviceField label2 = new DeviceField();
	DeviceButtons deviceButtons = new DeviceButtons();
	DeviceDispatch dispatch = new DeviceDispatch();
	//}}

}