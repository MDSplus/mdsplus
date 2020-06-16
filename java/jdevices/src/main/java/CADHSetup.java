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
public class CADHSetup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	BorderLayout borderLayout2 = new BorderLayout();
	JPanel jPanel2 = new JPanel();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	GridLayout gridLayout1 = new GridLayout();
	FlowLayout flowLayout1 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField1 = new DeviceField();
	DeviceField deviceField2 = new DeviceField();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceField deviceField3 = new DeviceField();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	FlowLayout flowLayout4 = new FlowLayout();
	JPanel jPanel6 = new JPanel();
	DeviceField deviceField4 = new DeviceField();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceField deviceField5 = new DeviceField();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel7 = new JPanel();
	DeviceField deviceField6 = new DeviceField();
	FlowLayout flowLayout7 = new FlowLayout();
	DeviceField deviceField7 = new DeviceField();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	FlowLayout flowLayout8 = new FlowLayout();
	JPanel jPanel8 = new JPanel();
	DeviceField deviceField8 = new DeviceField();
	GridLayout gridLayout2 = new GridLayout();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	JPanel jPanel9 = new JPanel();
	FlowLayout flowLayout9 = new FlowLayout();
	DeviceField deviceField9 = new DeviceField();
	DeviceField deviceField10 = new DeviceField();
	DeviceField deviceField11 = new DeviceField();
	DeviceField deviceField12 = new DeviceField();
	JPanel jPanel10 = new JPanel();
	FlowLayout flowLayout10 = new FlowLayout();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	DeviceField deviceField13 = new DeviceField();
	DeviceField deviceField14 = new DeviceField();
	JPanel jPanel11 = new JPanel();
	FlowLayout flowLayout11 = new FlowLayout();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	DeviceField deviceField15 = new DeviceField();
	DeviceField deviceField16 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	FlowLayout flowLayout12 = new FlowLayout();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel13 = new JPanel();
	JPanel jPanel14 = new JPanel();
	JPanel jPanel15 = new JPanel();
	JPanel jPanel16 = new JPanel();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	FlowLayout flowLayout13 = new FlowLayout();
	FlowLayout flowLayout14 = new FlowLayout();
	FlowLayout flowLayout15 = new FlowLayout();
	FlowLayout flowLayout16 = new FlowLayout();

	public CADHSetup()
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
		this.setWidth(580);
		this.setHeight(450);
		this.setDeviceType("CADH");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("INCAA CADH 4 Channels Transient Recorder");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(borderLayout2);
		deviceChannel1.setLabelString("Ch1:  ");
		deviceChannel1.setOffsetNid(10);
		deviceChannel1.setBorderVisible(true);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceChannel1.setLayout(flowLayout1);
		jPanel3.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(4);
		jPanel5.setLayout(flowLayout2);
		deviceField2.setOffsetNid(11);
		deviceField2.setLabelString("Start: ");
		deviceField2.setIdentifier("");
		deviceField1.setOffsetNid(12);
		deviceField1.setLabelString("End: ");
		deviceField1.setIdentifier("");
		deviceField3.setIdentifier("");
		deviceField3.setLabelString("End: ");
		deviceField3.setOffsetNid(30);
		deviceChannel2.setLayout(flowLayout3);
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setBorderVisible(true);
		deviceChannel2.setOffsetNid(28);
		deviceChannel2.setLabelString("Ch4:  ");
		jPanel6.setLayout(flowLayout4);
		deviceField4.setOffsetNid(29);
		deviceField4.setLabelString("Start: ");
		deviceField4.setIdentifier("");
		deviceField5.setIdentifier("");
		deviceField5.setLabelString("End: ");
		deviceField5.setOffsetNid(24);
		deviceChannel3.setLayout(flowLayout5);
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setBorderVisible(true);
		deviceChannel3.setOffsetNid(22);
		deviceChannel3.setLabelString("Ch3:  ");
		jPanel7.setLayout(flowLayout6);
		deviceField6.setOffsetNid(23);
		deviceField6.setLabelString("Start: ");
		deviceField6.setIdentifier("");
		deviceField7.setIdentifier("");
		deviceField7.setLabelString("End: ");
		deviceField7.setOffsetNid(18);
		deviceChannel4.setLayout(flowLayout7);
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setBorderVisible(true);
		deviceChannel4.setOffsetNid(16);
		deviceChannel4.setLabelString("Ch2:  ");
		jPanel8.setLayout(flowLayout8);
		deviceField8.setOffsetNid(17);
		deviceField8.setLabelString("Start: ");
		deviceField8.setIdentifier("");
		jPanel4.setLayout(gridLayout2);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(4);
		deviceChannel5.setLabelString("Ch1: ");
		deviceChannel5.setOffsetNid(10);
		deviceChannel5.setBorderVisible(true);
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setShowVal("");
		deviceChannel5.setLayout(flowLayout14);
		jPanel9.setLayout(flowLayout9);
		deviceField10.setOffsetNid(13);
		deviceField10.setLabelString("Start Idx: ");
		deviceField10.setIdentifier("");
		deviceField9.setOffsetNid(14);
		deviceField9.setLabelString("End Idx: ");
		deviceField9.setIdentifier("");
		deviceField11.setIdentifier("");
		deviceField11.setLabelString("Start Idx: ");
		deviceField11.setOffsetNid(31);
		deviceField12.setIdentifier("");
		deviceField12.setLabelString("End Idx: ");
		deviceField12.setOffsetNid(32);
		jPanel10.setLayout(flowLayout10);
		deviceChannel6.setLabelString("Ch4: ");
		deviceChannel6.setOffsetNid(28);
		deviceChannel6.setBorderVisible(true);
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setShowVal("");
		deviceChannel6.setLayout(flowLayout16);
		deviceField13.setIdentifier("");
		deviceField13.setLabelString("Start Idx: ");
		deviceField13.setOffsetNid(25);
		deviceField14.setIdentifier("");
		deviceField14.setLabelString("End Idx: ");
		deviceField14.setOffsetNid(26);
		jPanel11.setLayout(flowLayout11);
		deviceChannel7.setLabelString("Ch3: ");
		deviceChannel7.setOffsetNid(22);
		deviceChannel7.setBorderVisible(true);
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setShowVal("");
		deviceChannel7.setLayout(flowLayout15);
		deviceField15.setIdentifier("");
		deviceField15.setLabelString("Start Idx: ");
		deviceField15.setOffsetNid(19);
		deviceField16.setIdentifier("");
		deviceField16.setLabelString("End Idx: ");
		deviceField16.setOffsetNid(20);
		jPanel12.setLayout(flowLayout12);
		deviceChannel8.setLabelString("Ch2: ");
		deviceChannel8.setOffsetNid(16);
		deviceChannel8.setBorderVisible(true);
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setShowVal("");
		deviceChannel8.setLayout(flowLayout13);
		jPanel2.setLayout(gridLayout3);
		gridLayout3.setColumns(1);
		gridLayout3.setRows(4);
		deviceField17.setOffsetNid(1);
		deviceField17.setTextOnly(true);
		deviceField17.setLabelString("CAMAC Name: ");
		deviceField17.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(8);
		deviceChoice1.setLabelString("Use Time: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "TRUE", "FALSE" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField18.setOffsetNid(2);
		deviceField18.setTextOnly(true);
		deviceField18.setLabelString("Comment: ");
		deviceField18.setNumCols(35);
		deviceField18.setIdentifier("");
		deviceField20.setOffsetNid(6);
		deviceField20.setLabelString("Clock Source: ");
		deviceField20.setNumCols(15);
		deviceField20.setIdentifier("");
		deviceField19.setOffsetNid(5);
		deviceField19.setLabelString("Trig. Source: ");
		deviceField19.setNumCols(15);
		deviceField19.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(4);
		deviceChoice4.setLabelString("Clock Mode: ");
		deviceChoice4.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(new float[]
		{ (float) 500000.0, (float) 250000.0, (float) 125000.0, (float) 50000.0, (float) 10000.0, (float) 5000.0,
				(float) 1000.0, (float) 500.0 });
		deviceChoice3.setOffsetNid(7);
		deviceChoice3.setLabelString("Frequency: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "500E3", "250E3", "125E3", "50E3", "10E3", "5E3", "1E3", "500" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice2.setChoiceIntValues(new int[]
		{ 1, 2, 4 });
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(3);
		deviceChoice2.setLabelString("Num. Channels: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "1", "2", "4" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "TRIGGER", "STORE" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel2, BorderLayout.CENTER);
		jPanel2.add(jPanel13, null);
		jPanel13.add(deviceField17, null);
		jPanel13.add(deviceDispatch1, null);
		jPanel13.add(deviceChoice1, null);
		jPanel2.add(jPanel16, null);
		jPanel16.add(deviceField18, null);
		jPanel2.add(jPanel15, null);
		jPanel15.add(deviceField20, null);
		jPanel15.add(deviceField19, null);
		jPanel2.add(jPanel14, null);
		jPanel14.add(deviceChoice4, null);
		jPanel14.add(deviceChoice3, null);
		jPanel14.add(deviceChoice2, null);
		jPanel1.add(jTabbedPane1, BorderLayout.SOUTH);
		jTabbedPane1.add(jPanel3, "Channels (Time)");
		jPanel3.add(deviceChannel1, null);
		deviceChannel1.add(jPanel5, null);
		jPanel5.add(deviceField2, null);
		jPanel5.add(deviceField1, null);
		jPanel3.add(deviceChannel4, null);
		deviceChannel4.add(jPanel8, null);
		jPanel8.add(deviceField8, null);
		jPanel8.add(deviceField7, null);
		jPanel3.add(deviceChannel3, null);
		deviceChannel3.add(jPanel7, null);
		jPanel7.add(deviceField6, null);
		jPanel7.add(deviceField5, null);
		jPanel3.add(deviceChannel2, null);
		deviceChannel2.add(jPanel6, null);
		jPanel6.add(deviceField4, null);
		jPanel6.add(deviceField3, null);
		jTabbedPane1.add(jPanel4, "Channels (Samples)");
		jPanel4.add(deviceChannel5, null);
		deviceChannel5.add(jPanel9, null);
		jPanel9.add(deviceField10, null);
		jPanel9.add(deviceField9, null);
		jPanel4.add(deviceChannel8, null);
		deviceChannel8.add(jPanel12, null);
		jPanel12.add(deviceField15, null);
		jPanel12.add(deviceField16, null);
		jPanel4.add(deviceChannel7, null);
		deviceChannel7.add(jPanel11, null);
		jPanel11.add(deviceField13, null);
		jPanel11.add(deviceField14, null);
		jPanel4.add(deviceChannel6, null);
		deviceChannel6.add(jPanel10, null);
		jPanel10.add(deviceField11, null);
		jPanel10.add(deviceField12, null);
	}
}
