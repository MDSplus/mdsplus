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
public class TR32Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	DeviceField deviceField1 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField5 = new DeviceField();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel1 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	JPanel jPanel3 = new JPanel();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceChoice deviceChoice6 = new DeviceChoice();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	FlowLayout flowLayout1 = new FlowLayout();
	FlowLayout flowLayout2 = new FlowLayout();
	DeviceField deviceField8 = new DeviceField();
	DeviceChoice deviceChoice7 = new DeviceChoice();
	FlowLayout flowLayout3 = new FlowLayout();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	DeviceField deviceField9 = new DeviceField();
	FlowLayout flowLayout4 = new FlowLayout();
	JPanel jPanel4 = new JPanel();
	DeviceField deviceField10 = new DeviceField();
	DeviceChoice deviceChoice8 = new DeviceChoice();
	FlowLayout flowLayout5 = new FlowLayout();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	DeviceField deviceField11 = new DeviceField();
	FlowLayout flowLayout6 = new FlowLayout();
	JPanel jPanel5 = new JPanel();
	DeviceField deviceField12 = new DeviceField();
	DeviceChoice deviceChoice9 = new DeviceChoice();
	FlowLayout flowLayout7 = new FlowLayout();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	DeviceField deviceField13 = new DeviceField();
	FlowLayout flowLayout8 = new FlowLayout();
	JPanel jPanel6 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	JPanel jPanel2 = new JPanel();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	FlowLayout flowLayout9 = new FlowLayout();
	JPanel jPanel7 = new JPanel();
	DeviceField deviceField14 = new DeviceField();
	DeviceField deviceField15 = new DeviceField();
	DeviceChoice deviceChoice10 = new DeviceChoice();
	FlowLayout flowLayout10 = new FlowLayout();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	FlowLayout flowLayout11 = new FlowLayout();
	JPanel jPanel8 = new JPanel();
	FlowLayout flowLayout12 = new FlowLayout();
	DeviceField deviceField16 = new DeviceField();
	DeviceField deviceField17 = new DeviceField();
	DeviceChoice deviceChoice11 = new DeviceChoice();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	JPanel jPanel9 = new JPanel();
	FlowLayout flowLayout13 = new FlowLayout();
	FlowLayout flowLayout14 = new FlowLayout();
	DeviceField deviceField18 = new DeviceField();
	DeviceField deviceField19 = new DeviceField();
	DeviceChoice deviceChoice12 = new DeviceChoice();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	FlowLayout flowLayout15 = new FlowLayout();
	JPanel jPanel10 = new JPanel();
	FlowLayout flowLayout16 = new FlowLayout();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceChoice deviceChoice13 = new DeviceChoice();
	DeviceChoice deviceChoice14 = new DeviceChoice();
	DeviceField deviceField22 = new DeviceField();
	DeviceChoice deviceChoice15 = new DeviceChoice();

	public TR32Setup()
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
		this.setWidth(796);
		this.setHeight(550);
		this.setDeviceType("TR32");
		this.setDeviceProvider("150.178.3.101");
		this.setDeviceTitle("INCAA TR32 CPCI Transient Recorder");
		this.getContentPane().setLayout(null);
		deviceField1.setOffsetNid(1);
		deviceField1.setLabelString("Board Id: ");
		deviceField1.setNumCols(4);
		deviceField1.setIdentifier("");
		deviceField1.setBounds(new Rectangle(12, 13, 168, 31));
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(2);
		deviceChoice1.setLabelString("Software Mode: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "LOCAL", "REMOTE" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice1.setBounds(new Rectangle(167, 11, 231, 35));
		deviceField2.setOffsetNid(3);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Crate IP Addr: ");
		deviceField2.setNumCols(15);
		deviceField2.setIdentifier("");
		deviceField2.setBounds(new Rectangle(409, 13, 287, 30));
		deviceField3.setOffsetNid(4);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Comment: ");
		deviceField3.setNumCols(35);
		deviceField3.setIdentifier("");
		deviceField3.setBounds(new Rectangle(7, 44, 518, 30));
		deviceField4.setOffsetNid(8);
		deviceField4.setLabelString("Clock Source: ");
		deviceField4.setNumCols(30);
		deviceField4.setIdentifier("");
		deviceField4.setBounds(new Rectangle(4, 73, 445, 30));
		deviceDispatch1.setBounds(new Rectangle(545, 45, 130, 31));
		deviceField5.setOffsetNid(7);
		deviceField5.setLabelString("Trig. Source: ");
		deviceField5.setNumCols(30);
		deviceField5.setIdentifier("");
		deviceField5.setBounds(new Rectangle(-2, 104, 460, 30));
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(14);
		deviceChoice2.setLabelString("Trig. Edge: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "RISING", "FALLING" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice2.setBounds(new Rectangle(196, 136, 184, 35));
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(5);
		deviceChoice3.setLabelString("Clock Mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice3.setBounds(new Rectangle(444, 73, 221, 35));
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(6);
		deviceChoice4.setLabelString("Trig. Mode: ");
		deviceChoice4.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceChoice4.setBounds(new Rectangle(8, 136, 190, 35));
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice5.setChoiceFloatValues(new float[]
		{ (float) 3000000.0, (float) 1500000.0, (float) 1000000.0, (float) 500000.0, (float) 200000.0, (float) 100000.0,
				(float) 50000.0, (float) 20000.0, (float) 10000.0, (float) 5000.0, (float) 2000.0, (float) 1000.0,
				(float) 500.0, (float) 200.0, (float) 100.0 });
		deviceChoice5.setOffsetNid(9);
		deviceChoice5.setLabelString("Frequency (Hz): ");
		deviceChoice5.setChoiceItems(new String[]
		{ "3E6", "1.5E6", "1E6", "500E3", "200E3", "100E3", "50E3", "20E3", "10E3", "5E3", "2E3", "1E3", "500", "200",
				"100" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceChoice5.setBounds(new Rectangle(476, 105, 208, 35));
		jTabbedPane1.setBounds(new Rectangle(9, 209, 779, 238));
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(4);
		deviceChannel1.setLabelString("Ch 1: ");
		deviceChannel1.setOffsetNid(15);
		deviceChannel1.setBorderVisible(true);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceChannel1.setLayout(flowLayout1);
		deviceField7.setOffsetNid(17);
		deviceField7.setLabelString("Start: ");
		deviceField7.setNumCols(15);
		deviceField7.setIdentifier("");
		deviceField6.setOffsetNid(18);
		deviceField6.setLabelString("End: ");
		deviceField6.setNumCols(15);
		deviceField6.setIdentifier("");
		deviceChoice6.setChoiceIntValues(null);
		deviceChoice6.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice6.setOffsetNid(16);
		deviceChoice6.setLabelString("Range (+/-V): ");
		deviceChoice6.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice6.setUpdateIdentifier("");
		deviceChoice6.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "TRIGGER", "STORE" });
		deviceButtons1.setBounds(new Rectangle(213, 454, 301, 36));
		jPanel3.setLayout(flowLayout2);
		flowLayout2.setHgap(5);
		flowLayout2.setVgap(10);
		deviceField8.setIdentifier("");
		deviceField8.setNumCols(15);
		deviceField8.setLabelString("Start: ");
		deviceField8.setOffsetNid(38);
		deviceChoice7.setIdentifier("");
		deviceChoice7.setUpdateIdentifier("");
		deviceChoice7.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice7.setLabelString("Range (+/-V): ");
		deviceChoice7.setOffsetNid(37);
		deviceChoice7.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice7.setChoiceIntValues(null);
		deviceChannel4.setLayout(flowLayout3);
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setOffsetNid(36);
		deviceChannel4.setBorderVisible(true);
		deviceChannel4.setLabelString("Ch 4: ");
		deviceField9.setOffsetNid(39);
		deviceField9.setLabelString("End: ");
		deviceField9.setNumCols(15);
		deviceField9.setIdentifier("");
		flowLayout4.setHgap(5);
		flowLayout4.setVgap(10);
		jPanel4.setLayout(flowLayout4);
		deviceField10.setIdentifier("");
		deviceField10.setNumCols(15);
		deviceField10.setLabelString("Start: ");
		deviceField10.setOffsetNid(31);
		deviceChoice8.setIdentifier("");
		deviceChoice8.setUpdateIdentifier("");
		deviceChoice8.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice8.setLabelString("Range (+/-V): ");
		deviceChoice8.setOffsetNid(30);
		deviceChoice8.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice8.setChoiceIntValues(null);
		deviceChannel3.setLayout(flowLayout5);
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setOffsetNid(29);
		deviceChannel3.setBorderVisible(true);
		deviceChannel3.setLabelString("Ch 3: ");
		deviceField11.setOffsetNid(32);
		deviceField11.setLabelString("End: ");
		deviceField11.setNumCols(15);
		deviceField11.setIdentifier("");
		flowLayout6.setHgap(5);
		flowLayout6.setVgap(10);
		jPanel5.setLayout(flowLayout6);
		deviceField12.setIdentifier("");
		deviceField12.setNumCols(15);
		deviceField12.setLabelString("Start: ");
		deviceField12.setOffsetNid(24);
		deviceChoice9.setIdentifier("");
		deviceChoice9.setUpdateIdentifier("");
		deviceChoice9.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice9.setLabelString("Range (+/-V): ");
		deviceChoice9.setOffsetNid(23);
		deviceChoice9.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice9.setChoiceIntValues(null);
		deviceChannel2.setLayout(flowLayout7);
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setOffsetNid(22);
		deviceChannel2.setBorderVisible(true);
		deviceChannel2.setLabelString("Ch 2: ");
		deviceField13.setOffsetNid(25);
		deviceField13.setLabelString("End: ");
		deviceField13.setNumCols(15);
		deviceField13.setIdentifier("");
		flowLayout8.setHgap(5);
		flowLayout8.setVgap(10);
		jPanel6.setLayout(flowLayout8);
		gridLayout2.setColumns(1);
		gridLayout2.setRows(4);
		jPanel2.setLayout(gridLayout2);
		jPanel2.setDebugGraphicsOptions(0);
		deviceChannel5.setLayout(flowLayout9);
		deviceChannel5.setLabelString("Ch 1:");
		deviceChannel5.setOffsetNid(15);
		deviceChannel5.setBorderVisible(true);
		deviceChannel5.setLines(1);
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setShowVal("");
		deviceField14.setOffsetNid(19);
		deviceField14.setLabelString("Start Idx :");
		deviceField14.setNumCols(15);
		deviceField14.setIdentifier("");
		deviceField15.setOffsetNid(20);
		deviceField15.setLabelString("End Idx :");
		deviceField15.setNumCols(15);
		deviceField15.setIdentifier("");
		deviceChoice10.setChoiceIntValues(null);
		deviceChoice10.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice10.setOffsetNid(16);
		deviceChoice10.setLabelString("Range (+/- V):");
		deviceChoice10.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice10.setUpdateIdentifier("");
		deviceChoice10.setIdentifier("");
		jPanel7.setLayout(flowLayout10);
		flowLayout10.setVgap(10);
		deviceChannel6.setLabelString("Ch 2:");
		deviceChannel6.setOffsetNid(22);
		deviceChannel6.setBorderVisible(true);
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setShowVal("");
		deviceChannel6.setLayout(flowLayout11);
		jPanel8.setLayout(flowLayout12);
		deviceField16.setOffsetNid(26);
		deviceField16.setLabelString("Start Idx :");
		deviceField16.setNumCols(15);
		deviceField16.setIdentifier("");
		flowLayout12.setVgap(10);
		deviceField17.setOffsetNid(27);
		deviceField17.setLabelString("End Idx :");
		deviceField17.setNumCols(15);
		deviceField17.setIdentifier("");
		deviceChoice11.setChoiceIntValues(null);
		deviceChoice11.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice11.setOffsetNid(23);
		deviceChoice11.setLabelString("Range (+/- V):");
		deviceChoice11.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice11.setUpdateIdentifier("");
		deviceChoice11.setIdentifier("");
		deviceChannel7.setLabelString("Ch 3 :");
		deviceChannel7.setOffsetNid(29);
		deviceChannel7.setBorderVisible(true);
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setShowState(true);
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setShowVal("");
		deviceChannel7.setLayout(flowLayout14);
		jPanel9.setDebugGraphicsOptions(0);
		jPanel9.setLayout(flowLayout13);
		deviceField18.setOffsetNid(33);
		deviceField18.setLabelString("Start Idx :");
		deviceField18.setNumCols(15);
		deviceField18.setIdentifier("");
		deviceField19.setOffsetNid(34);
		deviceField19.setLabelString("End Idx :");
		deviceField19.setNumCols(15);
		deviceField19.setIdentifier("");
		deviceChoice12.setChoiceIntValues(null);
		deviceChoice12.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice12.setOffsetNid(30);
		deviceChoice12.setLabelString("Range (+/- V):");
		deviceChoice12.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice12.setUpdateIdentifier("");
		deviceChoice12.setIdentifier("");
		flowLayout13.setVgap(10);
		flowLayout14.setVgap(5);
		deviceChannel8.setLayout(flowLayout15);
		deviceChannel8.setLabelString("Ch 4 :");
		deviceChannel8.setOffsetNid(36);
		deviceChannel8.setBorderVisible(true);
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setShowVal("");
		jPanel10.setLayout(flowLayout16);
		flowLayout16.setVgap(10);
		deviceField20.setOffsetNid(40);
		deviceField20.setLabelString("Start Idx :");
		deviceField20.setNumCols(15);
		deviceField20.setIdentifier("");
		deviceField21.setOffsetNid(41);
		deviceField21.setLabelString("End Idx :");
		deviceField21.setNumCols(15);
		deviceField21.setIdentifier("");
		deviceChoice13.setChoiceIntValues(null);
		deviceChoice13.setChoiceFloatValues(new float[]
		{ (float) 0.125, (float) 0.15625, (float) 0.25, (float) 0.3125, (float) 0.5, (float) 0.625, (float) 1.0,
				(float) 1.25, (float) 2.0, (float) 2.5, (float) 4.0, (float) 5.0, (float) 8.0, (float) 10.0 });
		deviceChoice13.setOffsetNid(37);
		deviceChoice13.setLabelString("Range (+/- V):");
		deviceChoice13.setChoiceItems(new String[]
		{ "0.125", "0.15625", "0.25", "0.3125", "0.5", "0.625", "1", "1.25", "2", "2.5", "4", "5", "8", "10" });
		deviceChoice13.setUpdateIdentifier("");
		deviceChoice13.setIdentifier("");
		deviceChoice14.setChoiceIntValues(null);
		deviceChoice14.setChoiceFloatValues(null);
		deviceChoice14.setOffsetNid(12);
		deviceChoice14.setLabelString("Use Time:");
		deviceChoice14.setChoiceItems(new String[]
		{ "TRUE", "FALSE" });
		deviceChoice14.setUpdateIdentifier("");
		deviceChoice14.setIdentifier("");
		deviceChoice14.setBounds(new Rectangle(11, 172, 172, 35));
		deviceField22.setOffsetNid(10);
		deviceField22.setLabelString("Clock Division:");
		deviceField22.setNumCols(6);
		deviceField22.setIdentifier("");
		deviceField22.setBounds(new Rectangle(457, 141, 207, 30));
		deviceChoice15.setChoiceIntValues(null);
		deviceChoice15.setChoiceFloatValues(null);
		deviceChoice15.setOffsetNid(11);
		deviceChoice15.setLabelString("Clock Termination:");
		deviceChoice15.setChoiceItems(new String[]
		{ "ENABLED", "DISABLED" });
		deviceChoice15.setUpdateIdentifier("");
		deviceChoice15.setIdentifier("");
		deviceChoice15.setBounds(new Rectangle(421, 171, 238, 35));
		this.getContentPane().add(deviceField3, null);
		this.getContentPane().add(deviceChoice3, null);
		jPanel3.add(deviceField7, null);
		jPanel3.add(deviceField6, null);
		jPanel3.add(deviceChoice6, null);
		jPanel1.add(deviceChannel1, null);
		deviceChannel1.add(jPanel3, null);
		jPanel1.add(deviceChannel2, null);
		deviceChannel2.add(jPanel6, null);
		jPanel6.add(deviceField12, null);
		jPanel6.add(deviceField13, null);
		jPanel6.add(deviceChoice9, null);
		jPanel1.add(deviceChannel3, null);
		deviceChannel3.add(jPanel5, null);
		jPanel5.add(deviceField10, null);
		jPanel5.add(deviceField11, null);
		jPanel5.add(deviceChoice8, null);
		jPanel1.add(deviceChannel4, null);
		deviceChannel4.add(jPanel4, null);
		jPanel4.add(deviceField8, null);
		jPanel4.add(deviceField9, null);
		jPanel4.add(deviceChoice7, null);
		jTabbedPane1.add(jPanel2, "Channels (Samples)");
		jTabbedPane1.add(jPanel1, "Channels (Time)");
		jPanel7.add(deviceField14, null);
		jPanel7.add(deviceField15, null);
		jPanel7.add(deviceChoice10, null);
		jPanel2.add(deviceChannel5, null);
		deviceChannel5.add(jPanel7, null);
		jPanel2.add(deviceChannel6, null);
		deviceChannel6.add(jPanel8, null);
		jPanel8.add(deviceField16, null);
		jPanel8.add(deviceField17, null);
		jPanel8.add(deviceChoice11, null);
		jPanel2.add(deviceChannel7, null);
		deviceChannel7.add(jPanel9, null);
		this.getContentPane().add(deviceDispatch1, null);
		this.getContentPane().add(deviceField4, null);
		this.getContentPane().add(deviceField5, null);
		this.getContentPane().add(deviceField1, null);
		this.getContentPane().add(deviceChoice1, null);
		this.getContentPane().add(deviceField2, null);
		jPanel9.add(deviceField18, null);
		jPanel9.add(deviceField19, null);
		jPanel9.add(deviceChoice12, null);
		jPanel2.add(deviceChannel8, null);
		deviceChannel8.add(jPanel10, null);
		jPanel10.add(deviceField20, null);
		jPanel10.add(deviceField21, null);
		jPanel10.add(deviceChoice13, null);
		this.getContentPane().add(deviceField22, null);
		this.getContentPane().add(deviceChoice15, null);
		this.getContentPane().add(deviceButtons1, null);
		this.getContentPane().add(deviceChoice5, null);
		this.getContentPane().add(deviceChoice4, null);
		this.getContentPane().add(deviceChoice2, null);
		this.getContentPane().add(deviceChoice14, null);
		this.getContentPane().add(jTabbedPane1, null);
		jTabbedPane1.setSelectedComponent(jPanel1);
	}
}
