import java.awt.*;
import javax.swing.*;

/**
 * <p>
 * Title:
 * </p>
 * <p>
 * Description:
 * </p>
 * <p>
 * Copyright: Copyright (c) 2003
 * </p>
 * <p>
 * Company:
 * </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class J221RFXSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	JScrollPane jScrollPane1 = new JScrollPane();
	JPanel jPanel5 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	JPanel jPanel6 = new JPanel();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField9 = new DeviceField();
	JPanel jPanel8 = new JPanel();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	DeviceField deviceField10 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField11 = new DeviceField();
	JPanel jPanel9 = new JPanel();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	DeviceField deviceField12 = new DeviceField();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceField deviceField13 = new DeviceField();
	JPanel jPanel10 = new JPanel();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	DeviceField deviceField14 = new DeviceField();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	DeviceField deviceField15 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	DeviceField deviceField16 = new DeviceField();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	DeviceField deviceField17 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	DeviceField deviceField18 = new DeviceField();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	DeviceField deviceField19 = new DeviceField();
	JPanel jPanel13 = new JPanel();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	DeviceField deviceField110 = new DeviceField();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	DeviceField deviceField111 = new DeviceField();
	JPanel jPanel14 = new JPanel();
	DeviceChannel deviceChannel9 = new DeviceChannel();
	DeviceField deviceField112 = new DeviceField();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	DeviceField deviceField113 = new DeviceField();
	JPanel jPanel15 = new JPanel();
	DeviceChannel deviceChannel10 = new DeviceChannel();
	DeviceField deviceField114 = new DeviceField();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	DeviceField deviceField115 = new DeviceField();
	JPanel jPanel16 = new JPanel();
	DeviceChannel deviceChannel11 = new DeviceChannel();
	DeviceField deviceField116 = new DeviceField();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	DeviceField deviceField117 = new DeviceField();
	JPanel jPanel17 = new JPanel();
	DeviceChannel deviceChannel12 = new DeviceChannel();
	DeviceField deviceField118 = new DeviceField();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	DeviceField deviceField119 = new DeviceField();
	JPanel jPanel18 = new JPanel();
	DeviceChannel deviceChannel13 = new DeviceChannel();
	DeviceField deviceField1110 = new DeviceField();
	DeviceChoice deviceChoice13 = new DeviceChoice();

	public J221RFXSetup()
	{
		try
		{
			jbInit();
		}
		catch (final Exception e)
		{
			e.printStackTrace();
		}
	}

	private void jbInit() throws Exception
	{
		this.setWidth(617);
		this.setHeight(617);
		this.setDeviceType("J221RFX");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Jorway J221 Timing generator");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(3);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("CAMAC Name: ");
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Comment: ");
		deviceField2.setNumCols(30);
		deviceField2.setIdentifier("");
		deviceField4.setOffsetNid(3);
		deviceField4.setLabelString("Trig. Source: ");
		deviceField4.setNumCols(15);
		deviceField4.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Frequency: ");
		deviceField3.setIdentifier("");
		jPanel5.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(12);
		deviceChannel1.setLabelString("Ch 1");
		deviceChannel1.setOffsetNid(5);
		deviceChannel1.setBorderVisible(true);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(6);
		deviceChoice1.setLabelString("Mode: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField6.setOffsetNid(7);
		deviceField6.setLabelString("Repeat: ");
		deviceField6.setNumCols(8);
		deviceField6.setIdentifier("");
		deviceField5.setOffsetNid(8);
		deviceField5.setLabelString("Delta ");
		deviceField5.setNumCols(8);
		deviceField5.setIdentifier("");
		deviceField9.setIdentifier("");
		deviceField9.setNumCols(8);
		deviceField9.setLabelString("Delta ");
		deviceField9.setOffsetNid(63);
		deviceChannel3.setLabelString("Ch 12");
		deviceChannel3.setOffsetNid(60);
		deviceChannel3.setBorderVisible(true);
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setShowVal("");
		deviceField10.setOffsetNid(62);
		deviceField10.setLabelString("Repeat: ");
		deviceField10.setNumCols(8);
		deviceField10.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(61);
		deviceChoice3.setLabelString("Mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceField11.setIdentifier("");
		deviceField11.setNumCols(8);
		deviceField11.setLabelString("Delta ");
		deviceField11.setOffsetNid(58);
		deviceChannel4.setLabelString("Ch 11");
		deviceChannel4.setOffsetNid(55);
		deviceChannel4.setBorderVisible(true);
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setShowVal("");
		deviceField12.setOffsetNid(57);
		deviceField12.setLabelString("Repeat: ");
		deviceField12.setNumCols(8);
		deviceField12.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(56);
		deviceChoice4.setLabelString("Mode: ");
		deviceChoice4.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceField13.setIdentifier("");
		deviceField13.setNumCols(8);
		deviceField13.setLabelString("Delta ");
		deviceField13.setOffsetNid(53);
		deviceChannel5.setLabelString("Ch 10");
		deviceChannel5.setOffsetNid(50);
		deviceChannel5.setBorderVisible(true);
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setShowVal("");
		deviceField14.setOffsetNid(52);
		deviceField14.setLabelString("Repeat: ");
		deviceField14.setNumCols(8);
		deviceField14.setIdentifier("");
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setOffsetNid(51);
		deviceChoice5.setLabelString("Mode: ");
		deviceChoice5.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceField15.setIdentifier("");
		deviceField15.setNumCols(8);
		deviceField15.setLabelString("Delta ");
		deviceField15.setOffsetNid(48);
		deviceChannel6.setLabelString("Ch 9");
		deviceChannel6.setOffsetNid(45);
		deviceChannel6.setBorderVisible(true);
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setShowVal("");
		deviceField16.setOffsetNid(47);
		deviceField16.setLabelString("Repeat: ");
		deviceField16.setNumCols(8);
		deviceField16.setIdentifier("");
		deviceChoice6.setChoiceIntValues(null);
		deviceChoice6.setChoiceFloatValues(null);
		deviceChoice6.setOffsetNid(46);
		deviceChoice6.setLabelString("Mode: ");
		deviceChoice6.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceField17.setIdentifier("");
		deviceField17.setNumCols(8);
		deviceField17.setLabelString("Delta ");
		deviceField17.setOffsetNid(43);
		deviceChannel7.setLabelString("Ch 8");
		deviceChannel7.setOffsetNid(40);
		deviceChannel7.setBorderVisible(true);
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setShowVal("");
		deviceField18.setOffsetNid(42);
		deviceField18.setLabelString("Repeat: ");
		deviceField18.setNumCols(8);
		deviceField18.setIdentifier("");
		deviceChoice7.setChoiceIntValues(null);
		deviceChoice7.setChoiceFloatValues(null);
		deviceChoice7.setOffsetNid(41);
		deviceChoice7.setLabelString("Mode: ");
		deviceChoice7.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setIdentifier("");
		deviceField19.setIdentifier("");
		deviceField19.setNumCols(8);
		deviceField19.setLabelString("Delta ");
		deviceField19.setOffsetNid(38);
		deviceChannel8.setLabelString("Ch 7");
		deviceChannel8.setOffsetNid(35);
		deviceChannel8.setBorderVisible(true);
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setShowVal("");
		deviceField110.setOffsetNid(37);
		deviceField110.setLabelString("Repeat: ");
		deviceField110.setNumCols(8);
		deviceField110.setIdentifier("");
		deviceChoice8.setChoiceIntValues(null);
		deviceChoice8.setChoiceFloatValues(null);
		deviceChoice8.setOffsetNid(36);
		deviceChoice8.setLabelString("Mode: ");
		deviceChoice8.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setIdentifier("");
		deviceField111.setIdentifier("");
		deviceField111.setNumCols(8);
		deviceField111.setLabelString("Delta ");
		deviceField111.setOffsetNid(33);
		deviceChannel9.setLabelString("Ch 6");
		deviceChannel9.setOffsetNid(30);
		deviceChannel9.setBorderVisible(true);
		deviceChannel9.setInSameLine(true);
		deviceChannel9.setUpdateIdentifier("");
		deviceChannel9.setShowVal("");
		deviceField112.setOffsetNid(32);
		deviceField112.setLabelString("Repeat: ");
		deviceField112.setNumCols(8);
		deviceField112.setIdentifier("");
		deviceChoice9.setChoiceIntValues(null);
		deviceChoice9.setChoiceFloatValues(null);
		deviceChoice9.setOffsetNid(31);
		deviceChoice9.setLabelString("Mode: ");
		deviceChoice9.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setIdentifier("");
		deviceField113.setIdentifier("");
		deviceField113.setNumCols(8);
		deviceField113.setLabelString("Delta ");
		deviceField113.setOffsetNid(28);
		deviceChannel10.setLabelString("Ch 5");
		deviceChannel10.setOffsetNid(25);
		deviceChannel10.setBorderVisible(true);
		deviceChannel10.setInSameLine(true);
		deviceChannel10.setUpdateIdentifier("");
		deviceChannel10.setShowVal("");
		deviceField114.setOffsetNid(27);
		deviceField114.setLabelString("Repeat: ");
		deviceField114.setNumCols(8);
		deviceField114.setIdentifier("");
		deviceChoice10.setChoiceIntValues(null);
		deviceChoice10.setChoiceFloatValues(null);
		deviceChoice10.setOffsetNid(26);
		deviceChoice10.setLabelString("Mode: ");
		deviceChoice10.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setIdentifier("");
		deviceField115.setIdentifier("");
		deviceField115.setNumCols(8);
		deviceField115.setLabelString("Delta ");
		deviceField115.setOffsetNid(23);
		deviceChannel11.setLabelString("Ch 4");
		deviceChannel11.setOffsetNid(20);
		deviceChannel11.setBorderVisible(true);
		deviceChannel11.setInSameLine(true);
		deviceChannel11.setUpdateIdentifier("");
		deviceChannel11.setShowVal("");
		deviceField116.setOffsetNid(22);
		deviceField116.setLabelString("Repeat: ");
		deviceField116.setNumCols(8);
		deviceField116.setIdentifier("");
		deviceChoice11.setChoiceIntValues(null);
		deviceChoice11.setChoiceFloatValues(null);
		deviceChoice11.setOffsetNid(21);
		deviceChoice11.setLabelString("Mode: ");
		deviceChoice11.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setIdentifier("");
		deviceField117.setIdentifier("");
		deviceField117.setNumCols(8);
		deviceField117.setLabelString("Delta ");
		deviceField117.setOffsetNid(18);
		deviceChannel12.setLabelString("Ch 3");
		deviceChannel12.setOffsetNid(15);
		deviceChannel12.setBorderVisible(true);
		deviceChannel12.setInSameLine(true);
		deviceChannel12.setUpdateIdentifier("");
		deviceChannel12.setShowVal("");
		deviceField118.setOffsetNid(17);
		deviceField118.setLabelString("Repeat: ");
		deviceField118.setNumCols(8);
		deviceField118.setIdentifier("");
		deviceChoice12.setChoiceIntValues(null);
		deviceChoice12.setChoiceFloatValues(null);
		deviceChoice12.setOffsetNid(16);
		deviceChoice12.setLabelString("Mode: ");
		deviceChoice12.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		deviceField119.setIdentifier("");
		deviceField119.setNumCols(8);
		deviceField119.setLabelString("Delta ");
		deviceField119.setOffsetNid(13);
		deviceChannel13.setLabelString("Ch 2");
		deviceChannel13.setOffsetNid(10);
		deviceChannel13.setBorderVisible(true);
		deviceChannel13.setInSameLine(true);
		deviceChannel13.setUpdateIdentifier("");
		deviceChannel13.setShowVal("");
		deviceField1110.setOffsetNid(12);
		deviceField1110.setLabelString("Repeat: ");
		deviceField1110.setNumCols(8);
		deviceField1110.setIdentifier("");
		deviceChoice13.setChoiceIntValues(null);
		deviceChoice13.setChoiceFloatValues(null);
		deviceChoice13.setOffsetNid(11);
		deviceChoice13.setLabelString("Mode: ");
		deviceChoice13.setChoiceItems(new String[]
		{ "PULSE", "TOGGLE" });
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setIdentifier("");
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField2, null);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField4, null);
		jPanel3.add(deviceField3, null);
		this.getContentPane().add(jScrollPane1, BorderLayout.CENTER);
		jScrollPane1.getViewport().add(jPanel5, null);
		jPanel5.add(deviceChannel1, null);
		deviceChannel1.add(jPanel6, BorderLayout.CENTER);
		jPanel6.add(deviceChoice1, null);
		jPanel6.add(deviceField6, null);
		jPanel6.add(deviceField5, null);
		jPanel5.add(deviceChannel13, null);
		deviceChannel13.add(jPanel18, BorderLayout.CENTER);
		jPanel18.add(deviceChoice13, null);
		jPanel18.add(deviceField1110, null);
		jPanel18.add(deviceField119, null);
		jPanel5.add(deviceChannel12, null);
		deviceChannel12.add(jPanel17, BorderLayout.CENTER);
		jPanel17.add(deviceChoice12, null);
		jPanel17.add(deviceField118, null);
		jPanel17.add(deviceField117, null);
		jPanel5.add(deviceChannel11, null);
		deviceChannel11.add(jPanel16, BorderLayout.CENTER);
		jPanel16.add(deviceChoice11, null);
		jPanel16.add(deviceField116, null);
		jPanel16.add(deviceField115, null);
		jPanel5.add(deviceChannel10, null);
		deviceChannel10.add(jPanel15, BorderLayout.CENTER);
		jPanel15.add(deviceChoice10, null);
		jPanel15.add(deviceField114, null);
		jPanel15.add(deviceField113, null);
		jPanel5.add(deviceChannel9, null);
		deviceChannel9.add(jPanel14, BorderLayout.CENTER);
		jPanel14.add(deviceChoice9, null);
		jPanel14.add(deviceField112, null);
		jPanel14.add(deviceField111, null);
		jPanel5.add(deviceChannel8, null);
		deviceChannel8.add(jPanel13, BorderLayout.CENTER);
		jPanel13.add(deviceChoice8, null);
		jPanel13.add(deviceField110, null);
		jPanel13.add(deviceField19, null);
		jPanel5.add(deviceChannel7, null);
		deviceChannel7.add(jPanel12, BorderLayout.CENTER);
		jPanel12.add(deviceChoice7, null);
		jPanel12.add(deviceField18, null);
		jPanel12.add(deviceField17, null);
		jPanel5.add(deviceChannel6, null);
		deviceChannel6.add(jPanel11, BorderLayout.CENTER);
		jPanel11.add(deviceChoice6, null);
		jPanel11.add(deviceField16, null);
		jPanel11.add(deviceField15, null);
		jPanel5.add(deviceChannel5, null);
		deviceChannel5.add(jPanel10, BorderLayout.CENTER);
		jPanel10.add(deviceChoice5, null);
		jPanel10.add(deviceField14, null);
		jPanel10.add(deviceField13, null);
		jPanel5.add(deviceChannel4, null);
		deviceChannel4.add(jPanel9, BorderLayout.CENTER);
		jPanel9.add(deviceChoice4, null);
		jPanel9.add(deviceField12, null);
		jPanel9.add(deviceField11, null);
		jPanel5.add(deviceChannel3, null);
		deviceChannel3.add(jPanel8, BorderLayout.CENTER);
		jPanel8.add(deviceChoice3, null);
		jPanel8.add(deviceField10, null);
		jPanel8.add(deviceField9, null);
	}
}
