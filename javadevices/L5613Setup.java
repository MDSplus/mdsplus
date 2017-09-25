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
/*
		A basic implementation of the DeviceSetup class.
*/

import java.awt.*;
import javax.swing.*;

public class L5613Setup extends DeviceSetup
{
	public L5613Setup(JFrame parent)
	{
		super(parent);

		// This code is automatically generated by Visual Cafe when you add
		// components to the visual environment. It instantiates and initializes
		// the components. To modify the code, only use code syntax that matches
		// what Visual Cafe can generate, or Visual Cafe may be unable to back
		// parse your Java file into its visual environment.
		//{{INIT_CONTROLS
		setDeviceProvider("150.178.3.101");
		setDeviceTitle("LeCroy - L5613");
		setDeviceType("L5613");
		getContentPane().setLayout(null);
		setSize(457,260);
		deviceField1.setNumCols(15);
		deviceField1.setTextOnly(true);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Camac Name: ");
		getContentPane().add(deviceField1);
		deviceField1.setBounds(12,12,300,40);
		getContentPane().add(deviceDispatch1);
		deviceDispatch1.setBounds(324,12,131,40);
		deviceField2.setNumCols(30);
		deviceField2.setTextOnly(true);
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Comment: ");
		getContentPane().add(deviceField2);
		deviceField2.setBounds(12,60,456,40);
		deviceChoice1.setOffsetNid(3);
		{
			String[] tempString = new String[4];
			tempString[0] = "STD";
			tempString[1] = "100";
			tempString[2] = "200";
			tempString[3] = "300";
			deviceChoice1.setChoiceItems(tempString);
		}
		deviceChoice1.setLabelString("Model: ");
		getContentPane().add(deviceChoice1);
		deviceChoice1.setBounds(0,96,132,40);
		deviceChoice2.setChoiceFloatValues(new float[] {(float)100.0,(float)50.0,(float)20.0,(float)10.0,(float)5.0,(float)2.0,(float)1.0,(float)0.5,(float)0.2});
		deviceChoice2.setOffsetNid(7);
		{
			String[] tempString = new String[9];
			tempString[0] = "100";
			tempString[1] = "50";
			tempString[2] = "20";
			tempString[3] = "10";
			tempString[4] = "5";
			tempString[5] = "2";
			tempString[6] = "1";
			tempString[7] = "0.5";
			tempString[8] = "0.2";
			deviceChoice2.setChoiceItems(tempString);
		}
		deviceChoice2.setLabelString("Chan A range: ");
		getContentPane().add(deviceChoice2);
		deviceChoice2.setBounds(132,96,168,40);
		deviceChoice3.setChoiceFloatValues(new float[] {(float)100.0,(float)50.0,(float)20.0,(float)10.0,(float)5.0,(float)2.0,(float)1.0,(float)0.5,(float)0.2});
		deviceChoice3.setOffsetNid(11);
		{
			String[] tempString = new String[9];
			tempString[0] = "100";
			tempString[1] = "50";
			tempString[2] = "20";
			tempString[3] = "10";
			tempString[4] = "5";
			tempString[5] = "2";
			tempString[6] = "1";
			tempString[7] = "0.5";
			tempString[8] = "0.2";
			deviceChoice3.setChoiceItems(tempString);
		}
		deviceChoice3.setLabelString("Chan B range");
		getContentPane().add(deviceChoice3);
		deviceChoice3.setBounds(300,96,158,40);
		deviceField3.setNumCols(30);
		deviceField3.setOffsetNid(5);
		deviceField3.setLabelString("Chan A Output:  ");
		getContentPane().add(deviceField3);
		deviceField3.setBounds(0,144,456,40);
		deviceField4.setNumCols(30);
		deviceField4.setOffsetNid(9);
		deviceField4.setLabelString("Chan B Output: ");
		getContentPane().add(deviceField4);
		deviceField4.setBounds(0,180,456,40);
		getContentPane().add(deviceButtons1);
		deviceButtons1.setBounds(108,216,300,40);
		//}}
	}

	public L5613Setup()
	{
		this((JFrame)null);
	}

	public L5613Setup(String sTitle)
	{
		this();
		setTitle(sTitle);
	}

	static public void main(String args[])
	{
		(new L5613Setup()).setVisible(true);
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
	DeviceField deviceField2 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	//}}

}
