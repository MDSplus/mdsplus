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
public class TR6_3Setup extends DeviceSetup
{
	/**
	 *
	 */
	private static final long serialVersionUID = 1L;
	BorderLayout borderLayout1 = new BorderLayout();
	DeviceButtons deviceButtons1 = new DeviceButtons();
	JPanel jPanel1 = new JPanel();
	JPanel jPanel2 = new JPanel();
	GridLayout gridLayout1 = new GridLayout();
	JPanel jPanel3 = new JPanel();
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	DeviceField deviceField1 = new DeviceField();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField2 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField3 = new DeviceField();
	DeviceField deviceField4 = new DeviceField();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceChoice deviceChoice5 = new DeviceChoice();
	JTabbedPane jTabbedPane1 = new JTabbedPane();
	JPanel jPanel9 = new JPanel();
	GridLayout gridLayout2 = new GridLayout();
	BorderLayout borderLayout2 = new BorderLayout();
	DeviceChannel deviceChannel1 = new DeviceChannel();
	JPanel jPanel11 = new JPanel();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceChannel deviceChannel2 = new DeviceChannel();
	DeviceField deviceField8 = new DeviceField();
	JPanel jPanel12 = new JPanel();
	DeviceField deviceField9 = new DeviceField();
	DeviceChannel deviceChannel3 = new DeviceChannel();
	DeviceField deviceField10 = new DeviceField();
	JPanel jPanel13 = new JPanel();
	DeviceField deviceField11 = new DeviceField();
	DeviceChannel deviceChannel4 = new DeviceChannel();
	DeviceField deviceField12 = new DeviceField();
	JPanel jPanel14 = new JPanel();
	DeviceField deviceField13 = new DeviceField();
	DeviceChannel deviceChannel5 = new DeviceChannel();
	DeviceField deviceField14 = new DeviceField();
	JPanel jPanel15 = new JPanel();
	DeviceField deviceField15 = new DeviceField();
	DeviceChannel deviceChannel6 = new DeviceChannel();
	DeviceField deviceField16 = new DeviceField();
	JPanel jPanel16 = new JPanel();
	DeviceField deviceField17 = new DeviceField();
	DeviceField deviceField18 = new DeviceField();
	JPanel jPanel17 = new JPanel();
	DeviceChannel deviceChannel7 = new DeviceChannel();
	JPanel jPanel18 = new JPanel();
	DeviceField deviceField19 = new DeviceField();
	DeviceChannel deviceChannel8 = new DeviceChannel();
	DeviceField deviceField110 = new DeviceField();
	DeviceField deviceField20 = new DeviceField();
	DeviceField deviceField21 = new DeviceField();
	DeviceField deviceField22 = new DeviceField();
	GridLayout gridLayout3 = new GridLayout();
	JPanel jPanel19 = new JPanel();
	DeviceField deviceField111 = new DeviceField();
	DeviceChannel deviceChannel9 = new DeviceChannel();
	DeviceChannel deviceChannel10 = new DeviceChannel();
	JPanel jPanel110 = new JPanel();
	DeviceField deviceField112 = new DeviceField();
	DeviceField deviceField113 = new DeviceField();
	DeviceChannel deviceChannel11 = new DeviceChannel();
	DeviceField deviceField114 = new DeviceField();
	DeviceField deviceField23 = new DeviceField();
	DeviceChannel deviceChannel12 = new DeviceChannel();
	JPanel jPanel111 = new JPanel();
	JPanel jPanel112 = new JPanel();
	JPanel jPanel10 = new JPanel();

	public TR6_3Setup()
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
		this.setWidth(591);
		this.setHeight(591);
		this.setDeviceType("TR6_3");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("JOERGER TR612?3 6 Channels Transient Recorder");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setRows(6);
		gridLayout1.setVgap(1);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("CAMAC Name: ");
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setTextOnly(true);
		deviceField2.setLabelString("Comment: ");
		deviceField2.setNumCols(30);
		deviceField2.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(3);
		deviceChoice3.setLabelString("Trigger Mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceChoice2.setChoiceIntValues(null);
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(5);
		deviceChoice2.setLabelString("Clock Mode: ");
		deviceChoice2.setChoiceItems(new String[]
		{ "INTERNAL", "EXTERNAL" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(null);
		deviceChoice1.setChoiceFloatValues(new float[]
		{ (float) 3000000.0, (float) 2000000.0, (float) 1000000.0, (float) 100000.0 });
		deviceChoice1.setOffsetNid(7);
		deviceChoice1.setLabelString("Clock Frequency: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "3E6", "2E6", "1E6", "1E5" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceField3.setOffsetNid(4);
		deviceField3.setLabelString("Trigger Source: ");
		deviceField3.setNumCols(25);
		deviceField3.setIdentifier("");
		deviceField4.setOffsetNid(6);
		deviceField4.setLabelString("Clock Source ");
		deviceField4.setNumCols(25);
		deviceField4.setIdentifier("");
		deviceChoice5.setChoiceIntValues(null);
		deviceChoice5.setChoiceFloatValues(null);
		deviceChoice5.setOffsetNid(8);
		deviceChoice5.setLabelString("Op. Mode: ");
		deviceChoice5.setChoiceItems(new String[]
		{ "NORMAL", "BURST" });
		deviceChoice5.setUpdateIdentifier("");
		deviceChoice5.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(9);
		deviceChoice4.setLabelString("Use Time: ");
		deviceChoice4.setChoiceItems(new String[]
		{ "TRUE", "FALSE" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		jPanel9.setLayout(gridLayout2);
		gridLayout2.setRows(6);
		gridLayout2.setVgap(1);
		jPanel2.setLayout(borderLayout2);
		deviceChannel1.setLabelString("CH 1");
		deviceChannel1.setOffsetNid(12);
		deviceChannel1.setInSameLine(true);
		deviceChannel1.setUpdateIdentifier("");
		deviceChannel1.setShowVal("");
		deviceField5.setOffsetNid(15);
		deviceField5.setLabelString("Start Time: ");
		deviceField5.setIdentifier("");
		deviceField6.setOffsetNid(16);
		deviceField6.setLabelString("End Time:");
		deviceField6.setIdentifier("");
		deviceField7.setIdentifier("");
		deviceField7.setLabelString("Start Time: ");
		deviceField7.setOffsetNid(55);
		deviceChannel2.setShowVal("");
		deviceChannel2.setUpdateIdentifier("");
		deviceChannel2.setInSameLine(true);
		deviceChannel2.setOffsetNid(52);
		deviceChannel2.setLabelString("CH 6");
		deviceField8.setOffsetNid(56);
		deviceField8.setLabelString("End Time:");
		deviceField8.setIdentifier("");
		deviceField9.setIdentifier("");
		deviceField9.setLabelString("Start Time: ");
		deviceField9.setOffsetNid(47);
		deviceChannel3.setShowVal("");
		deviceChannel3.setUpdateIdentifier("");
		deviceChannel3.setInSameLine(true);
		deviceChannel3.setOffsetNid(44);
		deviceChannel3.setLabelString("CH 5");
		deviceField10.setOffsetNid(48);
		deviceField10.setLabelString("End Time:");
		deviceField10.setIdentifier("");
		deviceField11.setIdentifier("");
		deviceField11.setLabelString("Start Time: ");
		deviceField11.setOffsetNid(39);
		deviceChannel4.setShowVal("");
		deviceChannel4.setUpdateIdentifier("");
		deviceChannel4.setInSameLine(true);
		deviceChannel4.setOffsetNid(36);
		deviceChannel4.setLabelString("CH 4");
		deviceField12.setOffsetNid(40);
		deviceField12.setLabelString("End Time");
		deviceField12.setIdentifier("");
		deviceField13.setIdentifier("");
		deviceField13.setLabelString("Start Time: ");
		deviceField13.setOffsetNid(31);
		deviceChannel5.setShowVal("");
		deviceChannel5.setUpdateIdentifier("");
		deviceChannel5.setInSameLine(true);
		deviceChannel5.setOffsetNid(28);
		deviceChannel5.setLabelString("CH 3");
		deviceField14.setOffsetNid(32);
		deviceField14.setLabelString("End Time:");
		deviceField14.setIdentifier("");
		deviceField15.setIdentifier("");
		deviceField15.setLabelString("Start Time: ");
		deviceField15.setOffsetNid(23);
		deviceChannel6.setShowVal("");
		deviceChannel6.setUpdateIdentifier("");
		deviceChannel6.setInSameLine(true);
		deviceChannel6.setOffsetNid(20);
		deviceChannel6.setLabelString("CH 2");
		deviceField16.setOffsetNid(24);
		deviceField16.setLabelString("End Time:");
		deviceField16.setIdentifier("");
		deviceField17.setIdentifier("");
		deviceField17.setLabelString("End Idx: ");
		deviceField17.setOffsetNid(34);
		deviceField18.setIdentifier("");
		deviceField18.setLabelString("Start Idx: ");
		deviceField18.setOffsetNid(17);
		deviceChannel7.setShowVal("");
		deviceChannel7.setUpdateIdentifier("");
		deviceChannel7.setInSameLine(true);
		deviceChannel7.setOffsetNid(36);
		deviceChannel7.setLabelString("CH 4");
		deviceField19.setIdentifier("");
		deviceField19.setLabelString("Start Idx: ");
		deviceField19.setOffsetNid(25);
		deviceChannel8.setShowVal("");
		deviceChannel8.setUpdateIdentifier("");
		deviceChannel8.setInSameLine(true);
		deviceChannel8.setOffsetNid(28);
		deviceChannel8.setLabelString("CH 3");
		deviceField110.setIdentifier("");
		deviceField110.setLabelString("Start Idx: ");
		deviceField110.setOffsetNid(33);
		deviceField20.setIdentifier("");
		deviceField20.setLabelString("Start Idx: ");
		deviceField20.setOffsetNid(57);
		deviceField21.setOffsetNid(58);
		deviceField21.setLabelString("End Idx: ");
		deviceField21.setIdentifier("");
		deviceField22.setOffsetNid(18);
		deviceField22.setLabelString("End Idx:");
		deviceField22.setIdentifier("");
		gridLayout3.setRows(6);
		gridLayout3.setVgap(1);
		deviceField111.setOffsetNid(26);
		deviceField111.setLabelString("End Idx: ");
		deviceField111.setIdentifier("");
		deviceChannel9.setShowVal("");
		deviceChannel9.setUpdateIdentifier("");
		deviceChannel9.setInSameLine(true);
		deviceChannel9.setOffsetNid(20);
		deviceChannel9.setLabelString("CH 2");
		deviceChannel10.setShowVal("");
		deviceChannel10.setUpdateIdentifier("");
		deviceChannel10.setInSameLine(true);
		deviceChannel10.setOffsetNid(44);
		deviceChannel10.setLabelString("CH 5");
		deviceField112.setOffsetNid(50);
		deviceField112.setLabelString("End Idx: ");
		deviceField112.setIdentifier("");
		deviceField113.setIdentifier("");
		deviceField113.setLabelString("Start Idx: ");
		deviceField113.setOffsetNid(41);
		deviceChannel11.setLabelString("CH 1");
		deviceChannel11.setOffsetNid(12);
		deviceChannel11.setInSameLine(true);
		deviceChannel11.setUpdateIdentifier("");
		deviceChannel11.setShowVal("");
		deviceField114.setOffsetNid(42);
		deviceField114.setLabelString("End Idx:");
		deviceField114.setIdentifier("");
		deviceField23.setIdentifier("");
		deviceField23.setLabelString("Start Idx: ");
		deviceField23.setOffsetNid(49);
		deviceChannel12.setShowVal("");
		deviceChannel12.setUpdateIdentifier("");
		deviceChannel12.setInSameLine(true);
		deviceChannel12.setOffsetNid(52);
		deviceChannel12.setLabelString("CH 6");
		jPanel10.setLayout(gridLayout3);
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT", "TRIGGER", "STORE" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.NORTH);
		jPanel1.add(jPanel3, null);
		jPanel3.add(deviceField1, null);
		jPanel3.add(deviceDispatch1, null);
		jPanel1.add(jPanel8, null);
		jPanel8.add(deviceField2, null);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceChoice3, null);
		jPanel7.add(deviceChoice2, null);
		jPanel7.add(deviceChoice1, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceField3, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField4, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceChoice5, null);
		jPanel4.add(deviceChoice4, null);
		this.getContentPane().add(jPanel2, BorderLayout.CENTER);
		jPanel2.add(jTabbedPane1, BorderLayout.CENTER);
		jTabbedPane1.add(jPanel9, "Channels (Times)");
		jPanel9.add(deviceChannel1, null);
		deviceChannel1.add(jPanel11, BorderLayout.CENTER);
		jPanel11.add(deviceField5, null);
		jPanel11.add(deviceField6, null);
		jPanel9.add(deviceChannel6, null);
		deviceChannel6.add(jPanel16, BorderLayout.CENTER);
		jPanel16.add(deviceField15, null);
		jPanel16.add(deviceField16, null);
		jPanel9.add(deviceChannel5, null);
		deviceChannel5.add(jPanel15, BorderLayout.CENTER);
		jPanel15.add(deviceField13, null);
		jPanel15.add(deviceField14, null);
		jPanel9.add(deviceChannel4, null);
		deviceChannel4.add(jPanel14, BorderLayout.CENTER);
		jPanel14.add(deviceField11, null);
		jPanel14.add(deviceField12, null);
		jPanel9.add(deviceChannel3, null);
		deviceChannel3.add(jPanel13, BorderLayout.CENTER);
		jPanel13.add(deviceField9, null);
		jPanel13.add(deviceField10, null);
		jPanel9.add(deviceChannel2, null);
		deviceChannel2.add(jPanel12, BorderLayout.CENTER);
		jPanel12.add(deviceField7, null);
		jPanel12.add(deviceField8, null);
		jTabbedPane1.add(jPanel10, "Channels (Samples)");
		jPanel19.add(deviceField18, null);
		jPanel19.add(deviceField22, null);
		jPanel10.add(deviceChannel11, null);
		deviceChannel11.add(jPanel19, BorderLayout.CENTER);
		jPanel10.add(deviceChannel9, null);
		deviceChannel9.add(jPanel17, BorderLayout.CENTER);
		jPanel17.add(deviceField19, null);
		jPanel17.add(deviceField111, null);
		jPanel10.add(deviceChannel8, null);
		deviceChannel8.add(jPanel112, BorderLayout.CENTER);
		jPanel112.add(deviceField110, null);
		jPanel112.add(deviceField17, null);
		jPanel10.add(deviceChannel7, null);
		deviceChannel7.add(jPanel111, BorderLayout.CENTER);
		jPanel111.add(deviceField113, null);
		jPanel111.add(deviceField114, null);
		jPanel10.add(deviceChannel10, null);
		deviceChannel10.add(jPanel18, BorderLayout.CENTER);
		jPanel18.add(deviceField23, null);
		jPanel18.add(deviceField112, null);
		jPanel10.add(deviceChannel12, null);
		deviceChannel12.add(jPanel110, BorderLayout.CENTER);
		jPanel110.add(deviceField20, null);
		jPanel110.add(deviceField21, null);
	}
}
