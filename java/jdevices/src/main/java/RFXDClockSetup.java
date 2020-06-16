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
public class RFXDClockSetup extends DeviceSetup
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
	JPanel jPanel4 = new JPanel();
	JPanel jPanel5 = new JPanel();
	JPanel jPanel6 = new JPanel();
	JPanel jPanel7 = new JPanel();
	JPanel jPanel8 = new JPanel();
	DeviceDispatch deviceDispatch1 = new DeviceDispatch();
	DeviceField deviceField1 = new DeviceField();
	DeviceChoice deviceChoice1 = new DeviceChoice();
	DeviceChoice deviceChoice2 = new DeviceChoice();
	DeviceField deviceField2 = new DeviceField();
	DeviceField deviceField3 = new DeviceField();
	DeviceChoice deviceChoice3 = new DeviceChoice();
	DeviceField deviceField4 = new DeviceField();
	DeviceChoice deviceChoice4 = new DeviceChoice();
	DeviceField deviceField5 = new DeviceField();
	DeviceField deviceField6 = new DeviceField();
	DeviceField deviceField7 = new DeviceField();
	DeviceField deviceField8 = new DeviceField();

	public RFXDClockSetup()
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
		this.setWidth(550);
		this.setHeight(400);
		this.setDeviceType("RFXDClock");
		this.setDeviceProvider("localhost");
		this.setDeviceTitle("Dual Speed Clock for MPB Timing");
		this.getContentPane().setLayout(borderLayout1);
		jPanel1.setLayout(gridLayout1);
		gridLayout1.setColumns(1);
		gridLayout1.setRows(6);
		deviceField1.setOffsetNid(1);
		deviceField1.setTextOnly(true);
		deviceField1.setLabelString("Comment: ");
		deviceField1.setNumCols(20);
		deviceField1.setIdentifier("");
		deviceField2.setOffsetNid(2);
		deviceField2.setLabelString("Decoder: ");
		deviceField2.setIdentifier("");
		deviceChoice2.setChoiceIntValues(new int[]
		{ 1, 2, 3, 4, 5 });
		deviceChoice2.setChoiceFloatValues(null);
		deviceChoice2.setOffsetNid(3);
		deviceChoice2.setLabelString("Gate chan. :");
		deviceChoice2.setChoiceItems(new String[]
		{ "1", "2", "3", "4", "5" });
		deviceChoice2.setUpdateIdentifier("");
		deviceChoice2.setIdentifier("");
		deviceChoice1.setChoiceIntValues(new int[]
		{ 1, 2, 3, 4, 5 });
		deviceChoice1.setChoiceFloatValues(null);
		deviceChoice1.setOffsetNid(4);
		deviceChoice1.setLabelString("Clock chan.: ");
		deviceChoice1.setChoiceItems(new String[]
		{ "1", "2", "3", "4", "5" });
		deviceChoice1.setUpdateIdentifier("");
		deviceChoice1.setIdentifier("");
		deviceChoice3.setChoiceIntValues(null);
		deviceChoice3.setChoiceFloatValues(null);
		deviceChoice3.setOffsetNid(5);
		deviceChoice3.setLabelString("Trigger mode: ");
		deviceChoice3.setChoiceItems(new String[]
		{ "EVENT", "TRIGGER RISING", "TRIGGER FALLING", "SOFTWARE" });
		deviceChoice3.setUpdateIdentifier("");
		deviceChoice3.setIdentifier("");
		deviceField3.setOffsetNid(6);
		deviceField3.setTextOnly(true);
		deviceField3.setLabelString("Event: ");
		deviceField3.setIdentifier("");
		deviceChoice4.setChoiceIntValues(null);
		deviceChoice4.setChoiceFloatValues(null);
		deviceChoice4.setOffsetNid(12);
		deviceChoice4.setLabelString("Output mode: ");
		deviceChoice4.setChoiceItems(new String[]
		{ "SINGLE SWITCH: TOGGLE", "SINGLE SWITCH: HIGH PULSES", "SINGLE SWITCH: LOW PULSES", "DOUBLE SWITCH: TOGGLE",
				"DOUBLE SWITCH: HIGH PULSES", "DOUBLE SWITCH: LOW PULSES" });
		deviceChoice4.setUpdateIdentifier("");
		deviceChoice4.setIdentifier("");
		deviceField4.setOffsetNid(7);
		deviceField4.setLabelString("Ext. trigger: ");
		deviceField4.setIdentifier("");
		deviceField6.setOffsetNid(8);
		deviceField6.setLabelString("Delay(s): ");
		deviceField6.setIdentifier("");
		deviceField5.setOffsetNid(11);
		deviceField5.setLabelString("Duration(s): ");
		deviceField5.setIdentifier("");
		deviceField8.setOffsetNid(9);
		deviceField8.setLabelString("Frequency1(Hz): ");
		deviceField8.setIdentifier("");
		deviceField7.setOffsetNid(10);
		deviceField7.setLabelString("Frequency2(Hz): ");
		deviceField7.setIdentifier("");
		deviceButtons1.setCheckExpressions(null);
		deviceButtons1.setCheckMessages(null);
		deviceButtons1.setMethods(new String[]
		{ "INIT" });
		this.getContentPane().add(deviceButtons1, BorderLayout.SOUTH);
		this.getContentPane().add(jPanel1, BorderLayout.CENTER);
		jPanel1.add(jPanel2, null);
		jPanel2.add(deviceField1, null);
		jPanel2.add(deviceDispatch1, null);
		jPanel1.add(jPanel8, null);
		jPanel8.add(deviceField2, null);
		jPanel8.add(deviceChoice2, null);
		jPanel8.add(deviceChoice1, null);
		jPanel1.add(jPanel7, null);
		jPanel7.add(deviceChoice3, null);
		jPanel7.add(deviceField3, null);
		jPanel1.add(jPanel6, null);
		jPanel6.add(deviceChoice4, null);
		jPanel6.add(deviceField4, null);
		jPanel1.add(jPanel5, null);
		jPanel5.add(deviceField6, null);
		jPanel5.add(deviceField5, null);
		jPanel1.add(jPanel4, null);
		jPanel4.add(deviceField8, null);
		jPanel4.add(deviceField7, null);
	}
}
