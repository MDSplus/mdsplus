/*
		A basic implementation of the DeviceSetup class.
*/

import java.awt.*;
import javax.swing.*;

public class MPBEncoderSetup extends DeviceSetup
{
	private static final long serialVersionUID = 1L;
	public MPBEncoderSetup(JFrame parent)
	{
		super(parent);

		// This code is automatically generated by Visual Cafe when you add
		// components to the visual environment. It instantiates and initializes
		// the components. To modify the code, only use code syntax that matches
		// what Visual Cafe can generate, or Visual Cafe may be unable to back
		// parse your Java file into its visual environment.
		//{{INIT_CONTROLS
		setDeviceTitle("MPB Event generator");
		setDeviceType("MPBEncoderSetup");
		getContentPane().setLayout(null);
		setSize(492,299);
		deviceField1.setNumCols(15);
		deviceField1.setTextOnly(true);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("CAMAC Name: ");
		getContentPane().add(deviceField1);
		deviceField1.setBounds(12,12,324,40);
		deviceField2.setNumCols(30);
		deviceField2.setTextOnly(true);
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Comment: ");
		getContentPane().add(deviceField2);
		deviceField2.setBounds(24,48,468,40);
		getContentPane().add(deviceDispatch1);
		deviceDispatch1.setBounds(360,12,131,40);
		getContentPane().add(channels);
		channels.setBounds(12,96,468,144);
		deviceChannel1.setOffsetNid(3);
		deviceChannel1.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel1);
		deviceChannel1.setBounds(2,24,463,117);
		deviceField3.setNumCols(30);
		deviceField3.setTextOnly(true);
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Event Name: ");
		deviceChannel1.add(deviceField3);
		deviceField4.setNumCols(30);
		deviceField4.setOffsetNid(5);
		deviceField4.setLabelString("Trigger time: ");
		deviceChannel1.add(deviceField4);
		deviceChannel2.setOffsetNid(6);
		deviceChannel2.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel2);
		deviceChannel2.setBounds(2,24,463,117);
		deviceField5.setNumCols(30);
		deviceField5.setTextOnly(true);
		deviceField5.setOffsetNid(7);
		deviceField5.setLabelString("Event Name: ");
		deviceChannel2.add(deviceField5);
		deviceField6.setNumCols(30);
		deviceField6.setOffsetNid(8);
		deviceField6.setLabelString("Trigger time: ");
		deviceChannel2.add(deviceField6);
		deviceChannel3.setOffsetNid(9);
		deviceChannel3.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel3);
		deviceChannel3.setBounds(2,24,463,117);
		deviceField7.setNumCols(30);
		deviceField7.setTextOnly(true);
		deviceField7.setOffsetNid(10);
		deviceField7.setLabelString("Event Name: ");
		deviceChannel3.add(deviceField7);
		deviceField8.setNumCols(30);
		deviceField8.setOffsetNid(11);
		deviceField8.setLabelString("Trigger time: ");
		deviceChannel3.add(deviceField8);
		deviceChannel4.setOffsetNid(12);
		deviceChannel4.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel4);
		deviceChannel4.setBounds(2,24,463,117);
		deviceField9.setNumCols(30);
		deviceField9.setTextOnly(true);
		deviceField9.setOffsetNid(13);
		deviceField9.setLabelString("Event Name: ");
		deviceChannel4.add(deviceField9);
		deviceField10.setNumCols(30);
		deviceField10.setOffsetNid(14);
		deviceField10.setLabelString("Trigger time: ");
		deviceChannel4.add(deviceField10);
		deviceChannel5.setOffsetNid(15);
		deviceChannel5.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel5);
		deviceChannel5.setBounds(2,24,463,117);
		deviceField11.setNumCols(30);
		deviceField11.setTextOnly(true);
		deviceField11.setOffsetNid(16);
		deviceField11.setLabelString("Event Name: ");
		deviceChannel5.add(deviceField11);
		deviceField12.setNumCols(30);
		deviceField12.setOffsetNid(17);
		deviceField12.setLabelString("Trigger time: ");
		deviceChannel5.add(deviceField12);
		deviceChannel6.setOffsetNid(18);
		deviceChannel6.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel6);
		deviceChannel6.setBounds(2,24,463,117);
		deviceField13.setNumCols(30);
		deviceField13.setTextOnly(true);
		deviceField13.setOffsetNid(19);
		deviceField13.setLabelString("Event Name: ");
		deviceChannel6.add(deviceField13);
		deviceField14.setNumCols(30);
		deviceField14.setOffsetNid(20);
		deviceField14.setLabelString("Trigger time: ");
		deviceChannel6.add(deviceField14);
		deviceChannel8.setOffsetNid(21);
		deviceChannel8.setLayout(new BorderLayout(0,0));
		channels.add(deviceChannel8);
		deviceChannel8.setBounds(2,24,463,117);
		deviceField17.setNumCols(30);
		deviceField17.setTextOnly(true);
		deviceField17.setOffsetNid(22);
		deviceField17.setLabelString("Event Name: ");
		deviceChannel8.add(deviceField17);
		deviceField18.setNumCols(30);
		deviceField18.setOffsetNid(23);
		deviceField18.setLabelString("Trigger time: ");
		deviceChannel8.add(deviceField18);
		channels.setSelectedIndex(0);
		channels.setSelectedComponent(deviceChannel1);
		channels.setTitleAt(0,"Chan 1");
		channels.setTitleAt(1,"Chan 2");
		channels.setTitleAt(2,"Chan 3");
		channels.setTitleAt(3,"Chan 4");
		channels.setTitleAt(4,"Chan 5");
		channels.setTitleAt(5,"Chan 6");
		channels.setTitleAt(6,"Chan SW");
		getContentPane().add(deviceButtons1);
		deviceButtons1.setBounds(96,252,336,40);
		//}}
	}

	public MPBEncoderSetup()
	{
		this((JFrame)null);
	}

	public MPBEncoderSetup(String sTitle)
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
		(new MPBEncoderSetup()).setVisible(true);
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
	DeviceField deviceField2 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	javax.swing.JTabbedPane channels = new javax.swing.JTabbedPane();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	//}}

}
