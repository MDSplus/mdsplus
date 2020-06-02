/*
		A basic implementation of the DeviceSetup class.
*/

import java.awt.*;
import javax.swing.*;

public class MPBDecoderSetup extends DeviceSetup
{
	private static final long serialVersionUID = 1L;
	public MPBDecoderSetup(JFrame parent)
	{
		super(parent);

		// This code is automatically generated by Visual Cafe when you add
		// components to the visual environment. It instantiates and initializes
		// the components. To modify the code, only use code syntax that matches
		// what Visual Cafe can generate, or Visual Cafe may be unable to back
		// parse your Java file into its visual environment.
		//{{INIT_CONTROLS
		setDeviceProvider("150.178.3.112");
		setDeviceTitle("MPB Decoder");
		setDeviceType("MPBDecoder");
		getContentPane().setLayout(null);
		setSize(639,376);
		deviceField1.setTextOnly(true);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("CAMAC Name:");
		getContentPane().add(deviceField1);
		deviceField1.setBounds(0,12,240,36);
		getContentPane().add(deviceDispatch1);
		deviceDispatch1.setBounds(468,12,131,40);
		deviceChoice1.setOffsetNid(4);
		{
			String[] tempString = new String[10];
			tempString[0] = "EVENT 1";
			tempString[1] = "EVENT 2";
			tempString[2] = "EVENT 3";
			tempString[3] = "EVENT 4";
			tempString[4] = "EVENT 5";
			tempString[5] = "1MHz";
			tempString[6] = "100KHz";
			tempString[7] = "10KHz";
			tempString[8] = "1KHz";
			tempString[9] = "100Hz";
			deviceChoice1.setChoiceItems(tempString);
		}
		deviceChoice1.setLabelString("Master Source:");
		getContentPane().add(deviceChoice1);
		deviceChoice1.setBounds(0,48,192,36);
		deviceField2.setNumCols(6);
		deviceField2.setOffsetNid(3);
		deviceField2.setLabelString("Master Division: ");
		getContentPane().add(deviceField2);
		deviceField2.setBounds(204,48,200,36);
		deviceField3.setNumCols(5);
		deviceField3.setOffsetNid(7);
		deviceField3.setLabelString("Start Event: ");
		getContentPane().add(deviceField3);
		deviceField3.setBounds(240,12,204,36);
		getContentPane().add(JTabbedPane1);
		JTabbedPane1.setBounds(12,84,624,252);
		deviceChannel1.setLines(4);
		deviceChannel1.setOffsetNid(9);
		deviceChannel1.setLabelString("On");
		deviceChannel1.setLayout(new BorderLayout(0,0));
		JTabbedPane1.add(deviceChannel1);
		deviceChannel1.setBounds(2,24,619,225);
		JPanel1.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel1.add(JPanel1);
		deviceField4.setNumCols(8);
		deviceField4.setOffsetNid(11);
		deviceField4.setLabelString("Load: ");
		JPanel1.add(deviceField4);
		deviceField5.setNumCols(8);
		deviceField5.setOffsetNid(12);
		deviceField5.setLabelString("Hold: ");
		JPanel1.add(deviceField5);
		deviceField6.setNumCols(8);
		deviceField6.setOffsetNid(10);
		deviceField6.setLabelString("Event: ");
		JPanel1.add(deviceField6);
		JPanel2.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel1.add(JPanel2);
		deviceChoice3.setOffsetNid(13);
		{
			String[] tempString = new String[7];
			tempString[0] = "NONE";
			tempString[1] = "TCN-1";
			tempString[2] = "GATE N+1";
			tempString[3] = "GATE N-1";
			tempString[4] = "GATE N";
			tempString[5] = "TRIGGER RISING";
			tempString[6] = "TRIGGER FALLING";
			deviceChoice3.setChoiceItems(tempString);
		}
		deviceChoice3.setLabelString("Gating: ");
		JPanel2.add(deviceChoice3);
		deviceChoice4.setOffsetNid(14);
		{
			String[] tempString = new String[11];
			tempString[0] = "TCN-1";
			tempString[1] = "1MHz";
			tempString[2] = "100KHz";
			tempString[3] = "10KHz";
			tempString[4] = "1KHz";
			tempString[5] = "100Hz";
			tempString[6] = "EVENT 1";
			tempString[7] = "EVENT 2";
			tempString[8] = "EVENT 3";
			tempString[9] = "EVENT 4";
			tempString[10] = "EVENT 5";
			deviceChoice4.setChoiceItems(tempString);
		}
		deviceChoice4.setLabelString("Clock Source: ");
		JPanel2.add(deviceChoice4);
		deviceChoice5.setOffsetNid(15);
		{
			String[] tempString = new String[2];
			tempString[0] = "RISING";
			tempString[1] = "FALLING";
			deviceChoice5.setChoiceItems(tempString);
		}
		deviceChoice5.setLabelString("Clock Edge: ");
		JPanel2.add(deviceChoice5);
		JPanel3.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel1.add(JPanel3);
		deviceChoice6.setOffsetNid(16);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice6.setChoiceItems(tempString);
		}
		deviceChoice6.setLabelString("Special Gate: ");
		JPanel3.add(deviceChoice6);
		deviceChoice7.setOffsetNid(17);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice7.setChoiceItems(tempString);
		}
		deviceChoice7.setLabelString("Double Load: ");
		JPanel3.add(deviceChoice7);
		deviceChoice8.setOffsetNid(18);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice8.setChoiceItems(tempString);
		}
		deviceChoice8.setLabelString("Repeat Count: ");
		JPanel3.add(deviceChoice8);
		JPanel4.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel1.add(BorderLayout.CENTER, JPanel4);
		deviceChoice9.setOffsetNid(19);
		{
			String[] tempString = new String[2];
			tempString[0] = "BINARY";
			tempString[1] = "BCD";
			deviceChoice9.setChoiceItems(tempString);
		}
		deviceChoice9.setLabelString("Count Mode: ");
		JPanel4.add(deviceChoice9);
		deviceChoice10.setOffsetNid(20);
		{
			String[] tempString = new String[2];
			tempString[0] = "ASCENDING";
			tempString[1] = "DESCENDING";
			deviceChoice10.setChoiceItems(tempString);
		}
		deviceChoice10.setLabelString("Count Dir.: ");
		JPanel4.add(deviceChoice10);
		deviceChoice11.setOffsetNid(21);
		{
			String[] tempString = new String[6];
			tempString[0] = "ALWAYS HIGH";
			tempString[1] = "LOW PULSES";
			tempString[2] = "TOGGLE: INITIAL HIGH";
			tempString[3] = "TOGGLE: INITIAL LOW";
			tempString[4] = "ALWAYS LOW";
			tempString[5] = "HIGH PULSES";
			deviceChoice11.setChoiceItems(tempString);
		}
		deviceChoice11.setLabelString("Output Mode: ");
		JPanel4.add(deviceChoice11);
		deviceChannel2.setLines(4);
		deviceChannel2.setOffsetNid(23);
		deviceChannel2.setLabelString("On");
		deviceChannel2.setLayout(new BorderLayout(0,0));
		JTabbedPane1.add(deviceChannel2);
		deviceChannel2.setBounds(2,24,619,225);
		JPanel5.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel2.add(JPanel5);
		deviceField7.setNumCols(8);
		deviceField7.setOffsetNid(25);
		deviceField7.setLabelString("Load: ");
		JPanel5.add(deviceField7);
		deviceField8.setNumCols(8);
		deviceField8.setOffsetNid(26);
		deviceField8.setLabelString("Hold: ");
		JPanel5.add(deviceField8);
		deviceField9.setNumCols(8);
		deviceField9.setOffsetNid(24);
		deviceField9.setLabelString("Event: ");
		JPanel5.add(deviceField9);
		JPanel6.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel2.add(JPanel6);
		deviceChoice12.setOffsetNid(27);
		{
			String[] tempString = new String[7];
			tempString[0] = "NONE";
			tempString[1] = "TCN-1";
			tempString[2] = "GATE N+1";
			tempString[3] = "GATE N-1";
			tempString[4] = "GATE N";
			tempString[5] = "TRIGGER RISING";
			tempString[6] = "TRIGGER FALLING";
			deviceChoice12.setChoiceItems(tempString);
		}
		deviceChoice12.setLabelString("Gating: ");
		JPanel6.add(deviceChoice12);
		deviceChoice13.setOffsetNid(28);
		{
			String[] tempString = new String[11];
			tempString[0] = "TCN-1";
			tempString[1] = "1MHz";
			tempString[2] = "100KHz";
			tempString[3] = "10KHz";
			tempString[4] = "1KHz";
			tempString[5] = "100Hz";
			tempString[6] = "EVENT 1";
			tempString[7] = "EVENT 2";
			tempString[8] = "EVENT 3";
			tempString[9] = "EVENT 4";
			tempString[10] = "EVENT 5";
			deviceChoice13.setChoiceItems(tempString);
		}
		deviceChoice13.setLabelString("Clock Source: ");
		JPanel6.add(deviceChoice13);
		deviceChoice14.setOffsetNid(29);
		{
			String[] tempString = new String[2];
			tempString[0] = "RISING";
			tempString[1] = "FALLING";
			deviceChoice14.setChoiceItems(tempString);
		}
		deviceChoice14.setLabelString("Clock Edge: ");
		JPanel6.add(deviceChoice14);
		JPanel7.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel2.add(JPanel7);
		deviceChoice15.setOffsetNid(30);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice15.setChoiceItems(tempString);
		}
		deviceChoice15.setLabelString("Special Gate: ");
		JPanel7.add(deviceChoice15);
		deviceChoice16.setOffsetNid(31);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice16.setChoiceItems(tempString);
		}
		deviceChoice16.setLabelString("Double Load: ");
		JPanel7.add(deviceChoice16);
		deviceChoice17.setOffsetNid(32);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice17.setChoiceItems(tempString);
		}
		deviceChoice17.setLabelString("Repeat Count: ");
		JPanel7.add(deviceChoice17);
		JPanel8.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel2.add(BorderLayout.CENTER, JPanel8);
		deviceChoice18.setOffsetNid(33);
		{
			String[] tempString = new String[2];
			tempString[0] = "BINARY";
			tempString[1] = "BCD";
			deviceChoice18.setChoiceItems(tempString);
		}
		deviceChoice18.setLabelString("Count Mode: ");
		JPanel8.add(deviceChoice18);
		deviceChoice19.setOffsetNid(34);
		{
			String[] tempString = new String[2];
			tempString[0] = "ASCENDING";
			tempString[1] = "DESCENDING";
			deviceChoice19.setChoiceItems(tempString);
		}
		deviceChoice19.setLabelString("Count Dir.: ");
		JPanel8.add(deviceChoice19);
		deviceChoice20.setOffsetNid(35);
		{
			String[] tempString = new String[6];
			tempString[0] = "ALWAYS HIGH";
			tempString[1] = "LOW PULSES";
			tempString[2] = "TOGGLE: INITIAL HIGH";
			tempString[3] = "TOGGLE: INITIAL LOW";
			tempString[4] = "ALWAYS LOW";
			tempString[5] = "HIGH PULSES";
			deviceChoice20.setChoiceItems(tempString);
		}
		deviceChoice20.setLabelString("Output Mode: ");
		JPanel8.add(deviceChoice20);
		deviceChannel3.setLines(4);
		deviceChannel3.setOffsetNid(37);
		deviceChannel3.setLabelString("On");
		deviceChannel3.setLayout(new BorderLayout(0,0));
		JTabbedPane1.add(deviceChannel3);
		deviceChannel3.setBounds(2,24,619,225);
		JPanel9.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel3.add(JPanel9);
		deviceField10.setNumCols(8);
		deviceField10.setOffsetNid(39);
		deviceField10.setLabelString("Load: ");
		JPanel9.add(deviceField10);
		deviceField11.setNumCols(8);
		deviceField11.setOffsetNid(40);
		deviceField11.setLabelString("Hold: ");
		JPanel9.add(deviceField11);
		deviceField12.setNumCols(8);
		deviceField12.setOffsetNid(38);
		deviceField12.setLabelString("Event: ");
		JPanel9.add(deviceField12);
		JPanel10.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel3.add(JPanel10);
		deviceChoice21.setOffsetNid(41);
		{
			String[] tempString = new String[7];
			tempString[0] = "NONE";
			tempString[1] = "TCN-1";
			tempString[2] = "GATE N+1";
			tempString[3] = "GATE N-1";
			tempString[4] = "GATE N";
			tempString[5] = "TRIGGER RISING";
			tempString[6] = "TRIGGER FALLING";
			deviceChoice21.setChoiceItems(tempString);
		}
		deviceChoice21.setLabelString("Gating: ");
		JPanel10.add(deviceChoice21);
		deviceChoice22.setOffsetNid(42);
		{
			String[] tempString = new String[11];
			tempString[0] = "TCN-1";
			tempString[1] = "1MHz";
			tempString[2] = "100KHz";
			tempString[3] = "10KHz";
			tempString[4] = "1KHz";
			tempString[5] = "100Hz";
			tempString[6] = "EVENT 1";
			tempString[7] = "EVENT 2";
			tempString[8] = "EVENT 3";
			tempString[9] = "EVENT 4";
			tempString[10] = "EVENT 5";
			deviceChoice22.setChoiceItems(tempString);
		}
		deviceChoice22.setLabelString("Clock Source: ");
		JPanel10.add(deviceChoice22);
		deviceChoice23.setOffsetNid(43);
		{
			String[] tempString = new String[2];
			tempString[0] = "RISING";
			tempString[1] = "FALLING";
			deviceChoice23.setChoiceItems(tempString);
		}
		deviceChoice23.setLabelString("Clock Edge: ");
		JPanel10.add(deviceChoice23);
		JPanel11.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel3.add(JPanel11);
		deviceChoice24.setOffsetNid(44);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice24.setChoiceItems(tempString);
		}
		deviceChoice24.setLabelString("Special Gate: ");
		JPanel11.add(deviceChoice24);
		deviceChoice25.setOffsetNid(45);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice25.setChoiceItems(tempString);
		}
		deviceChoice25.setLabelString("Double Load: ");
		JPanel11.add(deviceChoice25);
		deviceChoice26.setOffsetNid(46);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice26.setChoiceItems(tempString);
		}
		deviceChoice26.setLabelString("Repeat Count: ");
		JPanel11.add(deviceChoice26);
		JPanel12.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel3.add(BorderLayout.CENTER, JPanel12);
		deviceChoice27.setOffsetNid(47);
		{
			String[] tempString = new String[2];
			tempString[0] = "BINARY";
			tempString[1] = "BCD";
			deviceChoice27.setChoiceItems(tempString);
		}
		deviceChoice27.setLabelString("Count Mode: ");
		JPanel12.add(deviceChoice27);
		deviceChoice28.setOffsetNid(48);
		{
			String[] tempString = new String[2];
			tempString[0] = "ASCENDING";
			tempString[1] = "DESCENDING";
			deviceChoice28.setChoiceItems(tempString);
		}
		deviceChoice28.setLabelString("Count Dir.: ");
		JPanel12.add(deviceChoice28);
		deviceChoice29.setOffsetNid(49);
		{
			String[] tempString = new String[6];
			tempString[0] = "ALWAYS HIGH";
			tempString[1] = "LOW PULSES";
			tempString[2] = "TOGGLE: INITIAL HIGH";
			tempString[3] = "TOGGLE: INITIAL LOW";
			tempString[4] = "ALWAYS LOW";
			tempString[5] = "HIGH PULSES";
			deviceChoice29.setChoiceItems(tempString);
		}
		deviceChoice29.setLabelString("Output Mode: ");
		JPanel12.add(deviceChoice29);
		deviceChannel4.setLines(4);
		deviceChannel4.setOffsetNid(51);
		deviceChannel4.setLabelString("On");
		deviceChannel4.setLayout(new BorderLayout(0,0));
		JTabbedPane1.add(deviceChannel4);
		deviceChannel4.setBounds(2,24,619,225);
		JPanel13.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel4.add(JPanel13);
		deviceField13.setNumCols(8);
		deviceField13.setOffsetNid(53);
		deviceField13.setLabelString("Load: ");
		JPanel13.add(deviceField13);
		deviceField14.setNumCols(8);
		deviceField14.setOffsetNid(54);
		deviceField14.setLabelString("Hold: ");
		JPanel13.add(deviceField14);
		deviceField15.setNumCols(8);
		deviceField15.setOffsetNid(52);
		deviceField15.setLabelString("Event: ");
		JPanel13.add(deviceField15);
		JPanel14.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel4.add(JPanel14);
		deviceChoice30.setOffsetNid(55);
		{
			String[] tempString = new String[7];
			tempString[0] = "NONE";
			tempString[1] = "TCN-1";
			tempString[2] = "GATE N+1";
			tempString[3] = "GATE N-1";
			tempString[4] = "GATE N";
			tempString[5] = "TRIGGER RISING";
			tempString[6] = "TRIGGER FALLING";
			deviceChoice30.setChoiceItems(tempString);
		}
		deviceChoice30.setLabelString("Gating: ");
		JPanel14.add(deviceChoice30);
		deviceChoice31.setOffsetNid(56);
		{
			String[] tempString = new String[11];
			tempString[0] = "TCN-1";
			tempString[1] = "1MHz";
			tempString[2] = "100KHz";
			tempString[3] = "10KHz";
			tempString[4] = "1KHz";
			tempString[5] = "100Hz";
			tempString[6] = "EVENT 1";
			tempString[7] = "EVENT 2";
			tempString[8] = "EVENT 3";
			tempString[9] = "EVENT 4";
			tempString[10] = "EVENT 5";
			deviceChoice31.setChoiceItems(tempString);
		}
		deviceChoice31.setLabelString("Clock Source: ");
		JPanel14.add(deviceChoice31);
		deviceChoice32.setOffsetNid(57);
		{
			String[] tempString = new String[2];
			tempString[0] = "RISING";
			tempString[1] = "FALLING";
			deviceChoice32.setChoiceItems(tempString);
		}
		deviceChoice32.setLabelString("Clock Edge: ");
		JPanel14.add(deviceChoice32);
		JPanel15.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel4.add(JPanel15);
		deviceChoice33.setOffsetNid(58);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice33.setChoiceItems(tempString);
		}
		deviceChoice33.setLabelString("Special Gate: ");
		JPanel15.add(deviceChoice33);
		deviceChoice34.setOffsetNid(59);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice34.setChoiceItems(tempString);
		}
		deviceChoice34.setLabelString("Double Load: ");
		JPanel15.add(deviceChoice34);
		deviceChoice35.setOffsetNid(60);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice35.setChoiceItems(tempString);
		}
		deviceChoice35.setLabelString("Repeat Count: ");
		JPanel15.add(deviceChoice35);
		JPanel16.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel4.add(BorderLayout.CENTER, JPanel16);
		deviceChoice36.setOffsetNid(61);
		{
			String[] tempString = new String[2];
			tempString[0] = "BINARY";
			tempString[1] = "BCD";
			deviceChoice36.setChoiceItems(tempString);
		}
		deviceChoice36.setLabelString("Count Mode: ");
		JPanel16.add(deviceChoice36);
		deviceChoice37.setOffsetNid(62);
		{
			String[] tempString = new String[2];
			tempString[0] = "ASCENDING";
			tempString[1] = "DESCENDING";
			deviceChoice37.setChoiceItems(tempString);
		}
		deviceChoice37.setLabelString("Count Dir.: ");
		JPanel16.add(deviceChoice37);
		deviceChoice38.setOffsetNid(63);
		{
			String[] tempString = new String[6];
			tempString[0] = "ALWAYS HIGH";
			tempString[1] = "LOW PULSES";
			tempString[2] = "TOGGLE: INITIAL HIGH";
			tempString[3] = "TOGGLE: INITIAL LOW";
			tempString[4] = "ALWAYS LOW";
			tempString[5] = "HIGH PULSES";
			deviceChoice38.setChoiceItems(tempString);
		}
		deviceChoice38.setLabelString("Output Mode: ");
		JPanel16.add(deviceChoice38);
		deviceChannel5.setLines(4);
		deviceChannel5.setOffsetNid(65);
		deviceChannel5.setLabelString("On");
		deviceChannel5.setLayout(new BorderLayout(0,0));
		JTabbedPane1.add(deviceChannel5);
		deviceChannel5.setBounds(2,24,619,225);
		JPanel17.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel5.add(JPanel17);
		deviceField16.setNumCols(8);
		deviceField16.setOffsetNid(67);
		deviceField16.setLabelString("Load: ");
		JPanel17.add(deviceField16);
		deviceField17.setNumCols(8);
		deviceField17.setOffsetNid(68);
		deviceField17.setLabelString("Hold: ");
		JPanel17.add(deviceField17);
		deviceField18.setNumCols(8);
		deviceField18.setOffsetNid(66);
		deviceField18.setLabelString("Event: ");
		JPanel17.add(deviceField18);
		JPanel18.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel5.add(JPanel18);
		deviceChoice39.setOffsetNid(69);
		{
			String[] tempString = new String[7];
			tempString[0] = "NONE";
			tempString[1] = "TCN-1";
			tempString[2] = "GATE N+1";
			tempString[3] = "GATE N-1";
			tempString[4] = "GATE N";
			tempString[5] = "TRIGGER RISING";
			tempString[6] = "TRIGGER FALLING";
			deviceChoice39.setChoiceItems(tempString);
		}
		deviceChoice39.setLabelString("Gating: ");
		JPanel18.add(deviceChoice39);
		deviceChoice40.setOffsetNid(70);
		{
			String[] tempString = new String[11];
			tempString[0] = "TCN-1";
			tempString[1] = "1MHz";
			tempString[2] = "100KHz";
			tempString[3] = "10KHz";
			tempString[4] = "1KHz";
			tempString[5] = "100Hz";
			tempString[6] = "EVENT 1";
			tempString[7] = "EVENT 2";
			tempString[8] = "EVENT 3";
			tempString[9] = "EVENT 4";
			tempString[10] = "EVENT 5";
			deviceChoice40.setChoiceItems(tempString);
		}
		deviceChoice40.setLabelString("Clock Source: ");
		JPanel18.add(deviceChoice40);
		deviceChoice41.setOffsetNid(71);
		{
			String[] tempString = new String[2];
			tempString[0] = "RISING";
			tempString[1] = "FALLING";
			deviceChoice41.setChoiceItems(tempString);
		}
		deviceChoice41.setLabelString("Clock Edge: ");
		JPanel18.add(deviceChoice41);
		JPanel19.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel5.add(JPanel19);
		deviceChoice42.setOffsetNid(72);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice42.setChoiceItems(tempString);
		}
		deviceChoice42.setLabelString("Special Gate: ");
		JPanel19.add(deviceChoice42);
		deviceChoice43.setOffsetNid(73);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice43.setChoiceItems(tempString);
		}
		deviceChoice43.setLabelString("Double Load: ");
		JPanel19.add(deviceChoice43);
		deviceChoice44.setOffsetNid(74);
		{
			String[] tempString = new String[2];
			tempString[0] = "DISABLED";
			tempString[1] = "ENABLED";
			deviceChoice44.setChoiceItems(tempString);
		}
		deviceChoice44.setLabelString("Repeat Count: ");
		JPanel19.add(deviceChoice44);
		JPanel20.setLayout(new FlowLayout(FlowLayout.CENTER,5,5));
		deviceChannel5.add(BorderLayout.CENTER, JPanel20);
		deviceChoice45.setOffsetNid(75);
		{
			String[] tempString = new String[2];
			tempString[0] = "BINARY";
			tempString[1] = "BCD";
			deviceChoice45.setChoiceItems(tempString);
		}
		deviceChoice45.setLabelString("Count Mode: ");
		JPanel20.add(deviceChoice45);
		deviceChoice46.setOffsetNid(76);
		{
			String[] tempString = new String[2];
			tempString[0] = "ASCENDING";
			tempString[1] = "DESCENDING";
			deviceChoice46.setChoiceItems(tempString);
		}
		deviceChoice46.setLabelString("Count Dir.: ");
		JPanel20.add(deviceChoice46);
		deviceChoice47.setOffsetNid(77);
		{
			String[] tempString = new String[6];
			tempString[0] = "ALWAYS HIGH";
			tempString[1] = "LOW PULSES";
			tempString[2] = "TOGGLE: INITIAL HIGH";
			tempString[3] = "TOGGLE: INITIAL LOW";
			tempString[4] = "ALWAYS LOW";
			tempString[5] = "HIGH PULSES";
			deviceChoice47.setChoiceItems(tempString);
		}
		deviceChoice47.setLabelString("Output Mode: ");
		JPanel20.add(deviceChoice47);
		JTabbedPane1.setSelectedIndex(0);
		JTabbedPane1.setSelectedComponent(deviceChannel1);
		JTabbedPane1.setTitleAt(0,"Channel 1");
		JTabbedPane1.setTitleAt(1,"Channel 2");
		JTabbedPane1.setTitleAt(2,"Channel 3");
		JTabbedPane1.setTitleAt(3,"Channel 4");
		JTabbedPane1.setTitleAt(4,"Channel 5");
		{
			String[] tempString = new String[1];
			tempString[0] = "init";
			deviceButtons1.setMethods(tempString);
		}
		getContentPane().add(deviceButtons1);
		deviceButtons1.setBounds(168,336,324,40);
		deviceChoice2.setOffsetNid(6);
		{
			String[] tempString = new String[2];
			tempString[0] = "ENABLED";
			tempString[1] = "DISABLED";
			deviceChoice2.setChoiceItems(tempString);
		}
		deviceChoice2.setLabelString("Synch. Scalers: ");
		getContentPane().add(deviceChoice2);
		deviceChoice2.setBounds(408,48,200,34);
		//}}
	}

	public MPBDecoderSetup()
	{
		this((JFrame)null);
	}

	public MPBDecoderSetup(String sTitle)
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
		(new MPBDecoderSetup()).setVisible(true);
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
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	javax.swing.JTabbedPane JTabbedPane1 = new javax.swing.JTabbedPane();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	javax.swing.JPanel JPanel1 = new javax.swing.JPanel();
	DeviceField deviceField4 = new DeviceField();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	javax.swing.JPanel JPanel2 = new javax.swing.JPanel();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	javax.swing.JPanel JPanel3 = new javax.swing.JPanel();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	javax.swing.JPanel JPanel4 = new javax.swing.JPanel();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	javax.swing.JPanel JPanel5 = new javax.swing.JPanel();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceField deviceField9 = new DeviceField();
	javax.swing.JPanel JPanel6 = new javax.swing.JPanel();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	javax.swing.JPanel JPanel7 = new javax.swing.JPanel();
	DeviceChoice deviceChoice15 = new DeviceChoice();
	DeviceChoice deviceChoice16 = new DeviceChoice();
	DeviceChoice deviceChoice17 = new DeviceChoice();
	javax.swing.JPanel JPanel8 = new javax.swing.JPanel();
	DeviceChoice deviceChoice18 = new DeviceChoice();
	DeviceChoice deviceChoice19 = new DeviceChoice();
	DeviceChoice deviceChoice20 = new DeviceChoice();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	javax.swing.JPanel JPanel9 = new javax.swing.JPanel();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	javax.swing.JPanel JPanel10 = new javax.swing.JPanel();
	DeviceChoice deviceChoice21 = new DeviceChoice();
	DeviceChoice deviceChoice22 = new DeviceChoice();
	DeviceChoice deviceChoice23 = new DeviceChoice();
	javax.swing.JPanel JPanel11 = new javax.swing.JPanel();
	DeviceChoice deviceChoice24 = new DeviceChoice();
	DeviceChoice deviceChoice25 = new DeviceChoice();
	DeviceChoice deviceChoice26 = new DeviceChoice();
	javax.swing.JPanel JPanel12 = new javax.swing.JPanel();
	DeviceChoice deviceChoice27 = new DeviceChoice();
	DeviceChoice deviceChoice28 = new DeviceChoice();
	DeviceChoice deviceChoice29 = new DeviceChoice();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	javax.swing.JPanel JPanel13 = new javax.swing.JPanel();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	javax.swing.JPanel JPanel14 = new javax.swing.JPanel();
	DeviceChoice deviceChoice30 = new DeviceChoice();
	DeviceChoice deviceChoice31 = new DeviceChoice();
	DeviceChoice deviceChoice32 = new DeviceChoice();
	javax.swing.JPanel JPanel15 = new javax.swing.JPanel();
	DeviceChoice deviceChoice33 = new DeviceChoice();
	DeviceChoice deviceChoice34 = new DeviceChoice();
	DeviceChoice deviceChoice35 = new DeviceChoice();
	javax.swing.JPanel JPanel16 = new javax.swing.JPanel();
	DeviceChoice deviceChoice36 = new DeviceChoice();
	DeviceChoice deviceChoice37 = new DeviceChoice();
	DeviceChoice deviceChoice38 = new DeviceChoice();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	javax.swing.JPanel JPanel17 = new javax.swing.JPanel();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	javax.swing.JPanel JPanel18 = new javax.swing.JPanel();
	DeviceChoice deviceChoice39 = new DeviceChoice();
	DeviceChoice deviceChoice40 = new DeviceChoice();
	DeviceChoice deviceChoice41 = new DeviceChoice();
	javax.swing.JPanel JPanel19 = new javax.swing.JPanel();
	DeviceChoice deviceChoice42 = new DeviceChoice();
	DeviceChoice deviceChoice43 = new DeviceChoice();
	DeviceChoice deviceChoice44 = new DeviceChoice();
	javax.swing.JPanel JPanel20 = new javax.swing.JPanel();
	DeviceChoice deviceChoice45 = new DeviceChoice();
	DeviceChoice deviceChoice46 = new DeviceChoice();
	DeviceChoice deviceChoice47 = new DeviceChoice();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	//}}

}
